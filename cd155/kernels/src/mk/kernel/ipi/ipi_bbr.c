/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1993 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: ipi_bbr.c,v $
 * Revision 1.2  1994/11/18  20:50:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/08  16:53:41  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_bbr.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	IPI device bad-block management functions
 */

#include <ipi.h>
#if	NIPI > 0

#include <ipi/ipi_compat.h>
#include <ipi/ipi_endian.h>
#include <ipi/ipi_defs.h>
#include <ipi/ipi_map.h>
#include <ipi/ipi-3.h>

/*
 * Scratch temporary in io_req structure (for error handling)
 */
#define	io_action	io_error


#define	BBR_ACTION_COMPLETE	1
#define	BBR_ACTION_RETRY_READ	2
#define	BBR_ACTION_RETRY_WRITE	3
#define BBR_ACTION_REASSIGN	4
#define	BBR_ACTION_COPY		5
#define	BBR_ACTION_VERIFY	6

/*
 * The defines below provide capability for soft and hard errors.
 * Sometimes disks can get themselves into a state where a block is
 * reported as bad when it is actually alright (perhaps due to self-
 * resonant frequencies in the disk drive heads).  If the command can
 * complete sucessfully within SOFT_RETRIES times, the block is not
 * reassigned, otherwise it is a hard error which may be reassigned.
 */
#define	SOFT_RETRIES		4
#define HARD_RETRIES		6
#define	MAX_RETRIES		(SOFT_RETRIES + HARD_RETRIES)

int	ipi_bbr_retries = MAX_RETRIES;


/*
 * Bad block replacement routine, invoked on unrecovereable read/write errors
 */
boolean_t
ipi_bad_block_replace(tgt, cmd_q, blockno)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	unsigned int	blockno;
{
	register io_req_t	ior = cmd_q->ior;

	if (!ipi_automatic_bbr || (ior->io_op & IO_INTERNAL))
		return FALSE;

	/*
	 * Signal that we are trying to take over. This will suspend
	 * the execution of commands until the operation is complete.
	 */
	tgt->flags   |= TGT_BBR_ACTIVE;

	/* Save some information */
	cmd_q->flags		|= CMD_NEEDS_BBR;
	cmd_q->bbr.retry_count	 = 0;
	cmd_q->bbr.badblockno	 = blockno;
	cmd_q->bbr.save_recnum	 = ior->io_recnum;
	cmd_q->bbr.save_data	 = ior->io_data;
	cmd_q->bbr.save_sgp	 = ior->io_sgp;
	cmd_q->bbr.save_count	 = ior->io_count;
	cmd_q->bbr.save_residual = ior->io_residual;

	/*
	 * On a write, we retry the command.  If we don't succeed within
	 * the soft retry count, we need to reassign the offending block.
	 * Note that the response extent parameter identified precisely
	 * which sector is bad.  At the end we'll retry the entire write,
	 * so if there is more than one bad sector involved they will be
	 * handled one at a time.
	 */
	if (!(ior->io_op & IO_READ)) {
		ior->io_action = BBR_ACTION_RETRY_WRITE;
	} else
	/*
	 * This is more complicated.  We asked for N bytes, and somewhere
	 * in there there is a chunk of bad data.  First off, we should retry
	 * at least a couple of times to retrieve that data [yes the drive
	 * should have done its best already, so what].  If that fails we
	 * should recover as much good data as possible (before the bad one).
	 */
	{
		ior->io_action = BBR_ACTION_RETRY_READ;
	}
	ior->io_residual = 0;

	/*
	 * Possibly start the replacement operation
	 */
	ipi_bbr_start(tgt);

	return TRUE;
}


boolean_t
ipi_bbr_start(tgt)
	target_info_t	*tgt;
{
	register int	i;
	cmd_queue_t	*cmd_q;
	int		depth;

	/*
	 * Examine the target command queues to determine if there are any
	 * outstanding commands.  Command states may be CMD_FREE or CMD_READY.
	 */
	depth  = tgt->cmd_ref_len;
	for (i = 0, cmd_q = tgt->cmd_q; i < depth; i++, cmd_q++) {
		if (cmd_q->flags & CMD_NEEDS_BBR)
			continue;
		if (cmd_q->state != CMD_FREE || cmd_q->state != CMD_READY)
			return FALSE;
	}

	/*
	 * No commands are active.
	 * Find the first one with that needs BBR and start it.
	 */
	for (i = 0, cmd_q = tgt->cmd_q; i < depth; i++, cmd_q++) {
		if (cmd_q->flags & CMD_NEEDS_BBR)
			break;
	}
	assert(i < depth);

	cmd_q->flags &= ~CMD_NEEDS_BBR;
	cmd_q->flags |= CMD_BBR_ACTIVE;

	printf("%s%d: Attempting bad block recovery, retry %s...",
		(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id,
		(!(cmd_q->ior->io_op & IO_READ)) ? "write" : "read");

	/* Start the operation */
	(void)ipi_disk_start_rw(tgt, cmd_q);

	return TRUE;
}


/*
 * This effectively replaces the strategy routine during bad block replacement
 */
ipi_bbr(tgt, cmd_q, done)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	boolean_t	done;
{
	register io_req_t	ior = cmd_q->ior;
	boolean_t		bbr_failed;
	char			*msg;

	assert(done == TRUE);

	switch (ior->io_action) {

	case BBR_ACTION_COMPLETE:

		/* All done, either way */
fin:
		cmd_q->flags	      &= ~CMD_BBR_ACTIVE;
		cmd_q->bbr.retry_count = 0;

		ior->io_recnum	 = cmd_q->bbr.save_recnum;
		ior->io_data	 = cmd_q->bbr.save_data;
		ior->io_sgp	 = cmd_q->bbr.save_sgp;
		ior->io_count	 = cmd_q->bbr.save_count;
		ior->io_residual = cmd_q->bbr.save_residual;
		ior->io_action	 = 0;

		if (cmd_q->result == IPI_RET_SUCCESS) {
			/* Restart normal life from the beginning */
			msg = "restarting.";
			bbr_failed = FALSE;
		} else {
			/* We could not fix it.  Tell the user and give up. */
			msg = "but could not recover.";
			bbr_failed = TRUE;
		}

		/*
		 * If there are any more commands needing
		 * BBR leave the target flag alone.
		 */
		{
			register int	i;
			cmd_queue_t	*cmd;
			int		depth;

			depth  = tgt->cmd_ref_len;
			for (i = 0, cmd = tgt->cmd_q; i < depth; i++, cmd++) {
				if (cmd->flags & CMD_NEEDS_BBR)
					break;
			}
			if (i >= depth)
				tgt->flags &= ~TGT_BBR_ACTIVE;
		}

		/*
		 * Commands for incoming ior's have been constructed and
		 * chained up in tgt->waiting_cmds while the bbr was active.
		 * If we are done, the interrupt routine will fire off as
		 * many chained ior's as possible (at least until the
		 * target command queue is full).  If there is still more
		 * to do as indicated by the TGT_BBR_ACTIVE flag we will
		 * remain in the bbr code.
		 */

		printf("done, %s\n", msg);
		return ipi_disk_start(tgt, cmd_q, bbr_failed);

	case BBR_ACTION_RETRY_READ:

		/* See if retry worked, if not do it again */
		if (cmd_q->result == IPI_RET_SUCCESS) {

			/* Whew, retry worked. */

			/*
			 * If the number of retries was within the
			 * soft error limit, don't reassign.
			 */
			if (cmd_q->bbr.retry_count < SOFT_RETRIES) {
				printf("recovered...");

				goto fin;
			}

			/*
			 * The number of retries was above the soft error
			 * limit.  Now reassign the bad block and don't
			 * forget to copy the good data over.
			 */
			printf("ok, now reassign...");

			ior->io_action = BBR_ACTION_COPY;
			ipi3_reallocate(tgt, cmd_q, cmd_q->bbr.badblockno);
			return;
		}

		if (++cmd_q->bbr.retry_count < ipi_bbr_retries) {
			(void)ipi_disk_start_rw(tgt, cmd_q);
			return;
		}

		/*
		 * Retrying was hopeless.
		 * Leave the bad block there for maintainance because
		 * we do not know what the heck to write on it.
		 */
		printf("failed after %d retries...", ipi_bbr_retries);

		goto fin;

	case BBR_ACTION_RETRY_WRITE:

		/* See if retry worked, if not do it again */
		if (cmd_q->result == IPI_RET_SUCCESS) {

			/* Whew, retry worked. */

			/*
			 * AND the number of retries was
			 * within the soft error limit
			 */
			goto fin;
		}

		if (++cmd_q->bbr.retry_count < SOFT_RETRIES) {
			(void)ipi_disk_start_rw(tgt, cmd_q);
			return;
		}

		/*
		 * Retrying was hopeless.
		 * Just reassign the block and get on with it.
		 */
		{
			printf("reassign...");

			ior->io_action = BBR_ACTION_REASSIGN;
			ipi3_reallocate(tgt, cmd_q, cmd_q->bbr.badblockno);
		}
		break;

	case BBR_ACTION_COPY:

		/* Retrying succeded and we rewired the bad block. */
		if (cmd_q->result == IPI_RET_SUCCESS) {

			printf("ok, rewrite...");

			/* Writeback the data to update the new block */
			ior->io_op &= ~IO_READ;

			/* Retry writeback the full amount */
			cmd_q->bbr.retry_count = 0;

			ior->io_action = BBR_ACTION_VERIFY;
			(void)ipi_disk_start_rw(tgt, cmd_q);
		} else {
			/*
			 * Either the reallocation is an unsupported command,
			 * or the reallocation space is exhausted.
			 */
			printf("reassign failed (really needs reformatting), ");

			goto fin;
		}
		break;

	case BBR_ACTION_VERIFY:

		if (cmd_q->result == IPI_RET_SUCCESS) {
			ior->io_op |= IO_READ;
			goto fin;
		}

		if (++cmd_q->bbr.retry_count < ipi_bbr_retries) {
			/* Retry, we are *this* close to success... */
			(void)ipi_disk_start_rw(tgt, cmd_q);
			return;
		}

		printf("failed after %d retries...", ipi_bbr_retries);

		ior->io_op |= IO_READ;

		goto fin;

	case BBR_ACTION_REASSIGN:

		/* If we wanted to issue the reassign multiple times */
		/* Currently unimplemented */
		goto fin;

	default:	/* snafu */
		panic("ipi_bbr");
	}
}

#endif	NIPI > 0
