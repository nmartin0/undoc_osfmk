/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: rz_cpu.c,v $
 * Revision 1.10  1995/03/14  23:48:20  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.9  1994/11/18  20:59:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/08/31  21:25:50  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.6.20.1  1994/08/04  16:14:27  richardg
 *  Reviewer: Jerry Coffman
 *  Risk: Medium
 *  Benefit or PTS #: 10171
 *  Testing: ran Tensor's tapewrt program simultaneously against both LUNs.
 *  Module(s): sccpu_strategy
 *
 * Revision 1.6  1993/06/30  22:53:30  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.5  1993/05/12  18:26:13  jerrie
 * Fix for bug 5099.
 * Initialize the tgt->lun field before starting a SCSI command.
 *
 * Revision 1.4  1993/04/27  20:47:39  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:53:06  dleslie
 * First R1_0 release
 *
 * Revision 2.2.2.2  92/04/30  12:00:21  bernadat
 * 	Changes from TRUNK:
 * 		On bad errors, be loquacious and decode all sense data.
 * 		Do not retry IO_INTERNAL operations, cuz we donno what it is.
 * 		[92/04/06            af]
 * 
 * Revision 2.2.2.1  92/03/28  10:15:22  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:31:50  jeffreyh]
 * 
 * Revision 2.3  92/02/23  22:44:15  elf
 * 	Changed the interface of a number of functions not to
 * 	require the scsi_softc pointer any longer.  It was
 * 	mostly unused, now it can be found via tgt->masterno.
 * 	[92/02/22  19:30:48  af]
 * 
 * Revision 2.2  91/08/24  12:27:47  af
 * 	Created.
 * 	[91/08/02  03:52:11  af]
 * 
 */
/*
 *	File: rz_cpu.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	7/91
 *
 *	Top layer of the SCSI driver: interface with the MI.
 *	This file contains operations specific to CPU-like devices.
 *
 * We handle here the case of simple devices which do not use any
 * sophisticated host-to-host communication protocol, they look
 * very much like degenerative cases of TAPE devices.
 *
 * For documentation and debugging, we also provide code to act like one.
 */

#include <mach/std_types.h>
#include <machine/machparam.h>		/* spl definitions */
#include <scsi/compat_30.h>

#include <scsi/scsi.h>
#include <scsi/scsi_defs.h>
#include <scsi/rz.h>

/*
 * This function decides which 'protocol' we well speak
 * to a cpu target. For now the decision is left to a
 * global var. XXXXXXX
 */
extern scsi_devsw_t scsi_host;
scsi_devsw_t	*scsi_cpu_protocol = /* later &scsi_host*/
			&scsi_devsw[SCSI_CPU];

sccpu_new_initiator(self, initiator)
	target_info_t	*self, *initiator;
{
	initiator->dev_ops = scsi_cpu_protocol;
	if (initiator == self) {
		self->flags = TGT_DID_SYNCH|TGT_DID_WIDE|
			      TGT_FULLY_PROBED|TGT_ONLINE|
			      TGT_ALIVE|TGT_US;
		self->dev_info.cpu.req_pending = FALSE;
	} else {
		initiator->flags = TGT_DID_SYNCH|TGT_DID_WIDE|
				   TGT_FULLY_PROBED|TGT_ONLINE|
				   TGT_ALIVE;
		initiator->dev_info.cpu.req_pending = TRUE;
	}
}

sccpu_strategy(ior)
	register io_req_t	ior;
{
	target_info_t  *tgt;
	register scsi_softc_t	*sc;
	scsi_ret_t      ret;
	register int    i = ior->io_unit;
	io_req_t	head, tail;

	/*
	 * Don't do anything for a pass-through device, just acknowledge
	 * the request.  This gives us a null SCSI device.
	 */
	if (rzpassthru(i)) {
		ior->io_residual = 0;
		iodone(ior);
		return D_SUCCESS;
	}

	sc = scsi_softc[rzcontroller(i)];
	tgt = sc->target[rzslave(i)][rzlun(i)];

	ior->io_next = 0;
	ior->io_prev = 0;

	i = splbio();
	simple_lock(&tgt->target_lock);
	if (head = tgt->ior) {
		/* Queue it up at the end of the list */
		if (tail = head->io_prev)
			tail->io_next = ior;
		else
			head->io_next = ior;
		head->io_prev = ior;	/* tail pointer */

#ifdef	SCSI_STATISTICS
		if (++tgt->statistics.chain_depth > tgt->statistics.chain_max)
			tgt->statistics.chain_max = tgt->statistics.chain_depth;
#endif	SCSI_STATISTICS

		simple_unlock(&tgt->target_lock);
	} else {
		/* Was empty, start operation */
		tgt->ior = ior;
		simple_unlock(&tgt->target_lock);
		sccpu_start( tgt, FALSE);
	}
	splx(i);

	return D_SUCCESS;
}


sccpu_start( tgt, done)
	target_info_t	*tgt;
	boolean_t	done;
{
	io_req_t		head, ior = tgt->ior;
	scsi_ret_t		ret;

	/* this is to the doc & debug code mentioned in the beginning */
	if (!done && tgt->dev_info.cpu.req_pending)
		return sccpu_act_as_target( tgt);

	if (ior == 0)
		return;

	if (done) {

		/* see if we must retry */
		if ((tgt->done == SCSI_RET_RETRY) &&
		    ((ior->io_op & IO_INTERNAL) == 0)) {
#ifdef	SCSI_STATISTICS
			tgt->statistics.retries++;
#endif	SCSI_STATISTICS
			delay(1000000);/*XXX*/
			goto start;
		} else
		/* got a bus reset ? shouldn't matter */
		if ((tgt->done == (SCSI_RET_ABORTED|SCSI_RET_RETRY)) &&
		    ((ior->io_op & IO_INTERNAL) == 0)) {
			goto start;
		} else

		/* check completion status */

		if ((tgt->cur_cmd == SCSI_CMD_REQUEST_SENSE) &&
		    (!rzpassthru(ior->io_unit))) {
			scsi_sense_data_t *sns;

			ior->io_op = ior->io_temporary;
			ior->io_error = D_IO_ERROR;
			ior->io_op |= IO_ERROR;

			sns = (scsi_sense_data_t *)tgt->cmd_ptr;
			if (scsi_debug)
				scsi_print_sense_data(sns);

			if (scsi_check_sense_data(tgt, sns)) {
			    if (sns->u.xtended.ili) {
				if (ior->io_op & IO_READ) {
				    int residue;

				    residue =	sns->u.xtended.info0 << 24 |
						sns->u.xtended.info1 << 16 |
						sns->u.xtended.info2 <<  8 |
						sns->u.xtended.info3;
				    if (scsi_debug)
					printf("Cpu Short Read (%d)\n", residue);
				    /*
				     * NOTE: residue == requested - actual
				     * We only care if > 0
				     */
				    if (residue < 0) residue = 0;/* sanity */
				    ior->io_residual += residue;
				    ior->io_error = 0;
				    ior->io_op &= ~IO_ERROR;
				    /* goto ok */
				}
			    }
			}
		}

		else if ((tgt->done != SCSI_RET_SUCCESS) &&
			 (!rzpassthru(ior->io_unit))) {

		    if (tgt->done == SCSI_RET_NEED_SENSE) {

			ior->io_temporary = ior->io_op;
			ior->io_op = IO_INTERNAL;
			if (scsi_debug)
				printf("[NeedSns x%x x%x]", ior->io_residual, ior->io_count);
			scsi_request_sense(tgt, ior, 0);
			return;

		    } else if (tgt->done == SCSI_RET_RETRY) {
			/* only retry here READs and WRITEs */
			if ((ior->io_op & IO_INTERNAL) == 0) {
				ior->io_residual = 0;
				goto start;
			} else{
				ior->io_error = D_WOULD_BLOCK;
				ior->io_op |= IO_ERROR;
			}
		    } else {
			ior->io_error = D_IO_ERROR;
			ior->io_op |= IO_ERROR;
		    }
		}

		if (scsi_debug)
			printf("[Resid x%x]", ior->io_residual);

		/* If this is a pass-through device, save the target result */
		if (rzpassthru(ior->io_unit)) ior->io_error = tgt->done;

		/* dequeue next one */
		head = ior;

		simple_lock(&tgt->target_lock);
		ior = head->io_next;
		tgt->ior = ior;
		if (ior)
			ior->io_prev = head->io_prev;
		simple_unlock(&tgt->target_lock);

#ifdef	SCSI_STATISTICS
		if (!(head->io_op & IO_INTERNAL)) {
			SCSI_DRIVER_CHECKOUT(tgt, ior);
		}
#endif	SCSI_STATISTICS

		iodone(head);

		if (ior == 0)
			return;

#ifdef	SCSI_STATISTICS
		/*
		 * Pulled an ior off the chain
		 */
		tgt->statistics.chain_depth--;
#endif	SCSI_STATISTICS
	}
	ior->io_residual = 0;
start:
	if (ior->io_op & IO_READ) {
		tgt->flags &= ~TGT_WRITTEN_TO;
		ret = scsi_receive( tgt, ior );
	} else if ((ior->io_op & IO_INTERNAL) == 0) {
		tgt->flags |= TGT_WRITTEN_TO;
		ret = scsi_send( tgt, ior );
	}
}

/*
 * This is a simple code to make us act as a dumb
 * processor type.  Use for debugging only.
 */
static struct io_req	sccpu_ior;

sccpu_act_as_target(self)
	target_info_t	*self;
{
	target_info_t	*tgt;
	static char	buffer[512] = "For debugging only";

	self->dev_info.cpu.req_pending = FALSE;
/*	tgt = sc->target[self->dev_info.cpu.req_id];*/
	sccpu_ior.io_next = 0;
	sccpu_ior.io_count = (512 < self->dev_info.cpu.req_len) ?
		512 : self->dev_info.cpu.req_len;
	sccpu_ior.io_data = buffer;
	if ((self->cur_cmd = self->dev_info.cpu.req_cmd) == SCSI_CMD_RECEIVE) {
		sccpu_ior.io_op = IO_READ;
	} else {
		sccpu_ior.io_op = IO_WRITE;
	}
	self->ior = &sccpu_ior;
}
