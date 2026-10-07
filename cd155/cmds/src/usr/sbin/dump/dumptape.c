/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: dumptape.c,v $
 * Revision 1.4  1994/11/19  03:05:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/03/24  17:55:46  robboy
 * Bug fix: don't tape offline before closing device
 *
 * Revision 1.2  1992/10/12  21:46:16  shala
 * New version to understand maj, min and node numbers.
 *
 * Revision 2.10.5.2  91/08/19  07:54:59  dwm
 * 	Avoid killing of pid -1; bug #3047.
 * 	[91/08/17  12:12:55  dwm]
 * 
 * Revision 2.10  90/10/07  22:08:23  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  16:50:35  gm]
 * 
 * Revision 2.9  90/08/25  12:28:59  devrcs
 * 	insert rcsid string;
 * 	delete old sccs id string;
 * 	[90/08/09  03:58:21  nm]
 * 
 * Revision 2.8  90/08/09  14:46:10  devrcs
 * 	rmtopen has lost iys incompatible mode argument;
 * 	[90/08/02  09:05:40  nm]
 * 
 * Revision 2.7  90/07/27  11:19:29  devrcs
 * 	insert missing breaks in switch in unmount_tape().
 * 	[90/06/22  06:40:53  nm]
 * 
 * 	integration to osc.12
 * 	[90/05/31  04:38:41  nm]
 * 
 * Revision 2.6  90/06/22  22:12:02  devrcs
 * 	integration to osc.12
 * 	[90/05/31  04:38:41  nm]
 * 
 * Revision 2.5  90/04/14  00:12:59  devrcs
 * 	integration of Jim's code
 * 	[90/04/10  08:12:24  nm]
 * 
 * 	Merged AIX3.1 and BSD4.3 to one source.
 * 	Made major changes fixing that code up. (Inserted comments too!)
 * 	Merged (BSD4.3 --> CMU) changes with what resulted.
 * 	Disabled edump (cannot quite figure out what it wants to do,
 * 		also not supported by rmt).
 * 	[90/01/11            walkerj]
 * 
 * Revision 2.4  90/03/13  20:48:03  mbrown
 * 	to prevent redeclaration conflicts  rewind() --- > rewind_it()
 * 	[90/03/02  09:27:36  mbrown]
 * 
 * Revision 2.3  90/01/02  18:41:36  gm
 * 	Fixes for first snapshot.
 * 	[90/01/02            gm]
 * 
 * Revision 2.2  89/12/26  08:57:45  gm
 * 	Current version from CMU.
 * 	[89/12/21            gm]
 * 
 * 	Add check to see if all non-acked writes succeeded. This must be
 * 	done after the flushing of we could clobber one of the slave
 * 	processes output or input. Also clear errno before doing a write
 * 	to reduce confusion.
 * 	[89/08/16  13:29:57  ern]
 * 
 * 	Add support for block mode (no record gap).
 * 	[89/07/31  00:28:12  ern]
 * 
 * 	Made minor changes to handling of full tape close for edump.
 * 	[89/06/20  23:09:25  ern]
 * 
 * 	Handle rewinding of tapes better.
 * 	Add edump support.
 * 	[89/06/12  22:33:36  ern]
 * 
 * $EndLog$
 */
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: dumptape.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 03:05:38 $";
#endif

/*
 * This module contains IBM CONFIDENTIAL code. -- (IBM Confidential Restricted
 * when combined with the aggregated modules for this product) OBJECT CODE ONLY
 * SOURCE MATERIALS (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * Copyright (c) 1980 Regents of the University of California. All
 * rights reserved.  The Berkeley software License Agreement specifies the
 * terms and conditions for redistribution.
 */

#include	"dump.h"

#include	<sys/ipc.h>
#include	<sys/sem.h>
#include	<errno.h>
#include	<sys/ioctl.h>
#include	<sys/mtio.h>

static int	atomic();
static void	alloc_tape();
static void	change_tape();
static void	check_point();
static void	end_slaves();
static void	flusht();
static void	get_semaphore();
static void	initialize_slaves();
static void	kill_slaves();
static void	mount_tape();
static void	open_tape();
static void	remove_semaphore();
static void	rewind_tape();
static void	sigpipe();
static void	slave_block();
static void	slave_unblock();
static void	slave_wait();
static void	slave_work();
static void	start_slaves();
static void	tape_error();
static void	timeest();
static void	unmount_tape();
static void	write_to_slave();

/*
 * Concurrent dump mods (Caltech) - disk block reading and tape writing are
 * exported to several slave processes.  While one slave writes the tape, the
 * others read disk blocks; they pass control of the tape in a ring via
 * semop(). The parent process traverses the filesystem and sends
 * spclrec()'s and lists of daddr's to the slaves via pipes.
 */

/*
 * The master process collect requests until it has blocks_per_write tape
 * blocks' worth.  Then they are written down the pipe to the next slave.
 *
 * Each request can be either 1) write a single tape block whose contents
 * are provided (using taprec()), or 2) write multiple tape blocks whose
 * file system data block address is provided (using dmpblk()).
 *
 * In 1), the data is passed from master to slave in the pipe-block array,
 * which immediately follows the request array.
 * In 2), only the request is passed to the slave, who reads the data from
 * the disk itself.
 *
 * Each request is stored in the next member of the request array.
 * The data for each type 1) request is stored in the next member of the
 * pipe-block array.
 *
 * In the worst case, each request is of type 1) for a single tape block,
 * so both the request array and the pipe-block array have a total of
 * blocks_per_write members.
 */

/* request packet sent from master to slave */

struct rq_block
{
	daddr_t		device_block_num;
	int		num_blocks;
};

/* tape block structure, used in both tape-buffer and pipe-block array */

struct tp_block
{
	char		dummy[TP_BSIZE];
};

/* these variables used to collect requests which are sent down the pipe */
/* from master to slave */

static struct rq_block *request_array;
static int		request_array_size;
static int		request_num = 0;
static int		count_tape_blocks = 0;

/* these variables are used to hold the data for those requests which have */
/* it, and are sent down the pipe with the requests */

static struct tp_block *pipe_buffer;
static int		pipe_array_size;
static int		pipe_block_num = 0;

/* these variables used by the slave to assemble the data, from both the */
/* pipe and the disk, which are to be written to tape */

static struct tp_block *tape_buffer;
static int		tape_write_size;

/* these variables are used for keeping track of how much of the tape */
/* and how many blocks have been used so far */

static time_t	time_start_write;	/* when started writing the first
					 * tape block on first tape */
static int	tot_blocks_written = 0; /* total number of blocks written
					 * so far */

long	lastspclrec = -1;	/* tape block number of last written header */

/* alloc_tape() is the initialization routine for the output processes. */
/* Its primary duty is to allocate buffer space for each the request array, */
/* the pipe-block array, and the tape-buffer. */

static void
alloc_tape()
{
	unsigned long	max_offset;
	char	       *buffer;

	/* figure out the byte lengths of the request array, the pipe */
	/* buffer array, and the tape buffer array */

	request_array_size = blocks_per_write * sizeof(struct rq_block);
	pipe_array_size = blocks_per_write * sizeof(struct tp_block);
	tape_write_size = blocks_per_write * sizeof(struct tp_block);

	/*
	 * Allocate pipe buffer contiguous with the array of request
	 * blocks, so flusht() can write them together with one write().
	 */

	buffer = (char *) malloc((unsigned) request_array_size + pipe_array_size);

	request_array = (struct rq_block *) buffer;
	pipe_buffer = (struct tp_block *) ((unsigned long) buffer + request_array_size);

	/* allocate a space enough larger than the required tape buffer */
	/* size so the tape buffer can be moved forward to put its */
	/* starting address on a page boundary, to make writing to the */
	/* tape faster */

	max_offset = getpagesize() - 1;

	buffer = (char *) malloc((unsigned) tape_write_size + max_offset);

	tape_buffer = (struct tp_block *) (((unsigned long) buffer + max_offset) & ~max_offset);
}

/* Taprec() writes to the slave pipe (puts in the next request & pipe */
/* buffers) a single tape block pointed to by the argument.  It is called */
/* by spclrec() to output single spcl records, and iteratively by bitmap() */
/* to output bit maps. */

void
taprec(block_to_write)
	char	       *block_to_write;
{
	/* fill the next request buffer array member with dummy block number */
	/* (zero) and number of blocks 1 */

	request_array[request_num].device_block_num = (daddr_t) 0;
	request_array[request_num].num_blocks = 1;
	++request_num;

	/* fill the next pipe buffer block with the data */

	pipe_buffer[pipe_block_num] = *((struct tp_block *) block_to_write);
	++pipe_block_num;

	/* increment the record number in the spcl record */

	lastspclrec = spcl.c_tapea;
	++spcl.c_tapea;

	/* increment the count of tape blocks and call flusht() if enough */
	/* tape blocks' worth have been accumulated for a full tape write */

	++count_tape_blocks;
	if (count_tape_blocks == blocks_per_write)
	{
		flusht();
	}
}

/* Dmpblk() arranges for the writing of the file system data block whose */
/* block number is disk_block_num and has length byte_length.  It does not */
/* actually access the data, but only puts the request in the next slot */
/* in the request array. */

void
dmpblk(disk_block_num, byte_length)
	daddr_t		disk_block_num;
	int		byte_length;
{
	int		device_block_num;
	int		remaining_tape_blocks;
	int		blocks_to_write;

	device_block_num = fsbtodb(super_block, disk_block_num);

	remaining_tape_blocks = byte_length / TP_BSIZE;

	/* Figure out how many tape blocks to write this iteration. */
	/* This may involve splitting the request over multiple */
	/* tape/pipe-writes. */

	while ((blocks_to_write = min(remaining_tape_blocks, blocks_per_write - count_tape_blocks)) > 0)
	{
		/* put the starting device block number and the number of */
		/* tape blocks to write in the next request array member */

		request_array[request_num].device_block_num = device_block_num;
		request_array[request_num].num_blocks = blocks_to_write;
		++request_num;

		spcl.c_tapea += blocks_to_write;

		/* increment the count of tape blocks and call flusht() */
		/* if enough tape blocks have been accumulated for a */
		/* full tape write */

		count_tape_blocks += blocks_to_write;
		if (count_tape_blocks == blocks_per_write)
		{
			flusht();
		}

		device_block_num += blocks_to_write * (TP_BSIZE / dev_bsize);
		remaining_tape_blocks -= blocks_to_write;
	}
}

/* Flusht() is called when the request array is full of enough requests */
/* to do a whole tape write.  The work is dispatched to the next slave */
/* in the ring. */

static void
flusht()
{
	int		byte_length;

	/* the pipe-write length is always the full request array plus */
	/* any pipe-array blocks which may have been filled */

	byte_length = request_array_size + pipe_block_num * sizeof(struct tp_block);

	/* send the request block to a slave */

	/* request array and pipe-block array are contiguous so they */
	/* can both be written to the pipe in one write */

	write_to_slave((char *) request_array, byte_length);

	/* reset the request, tape-block, and pipe-block counters */

	request_num = 0;
	count_tape_blocks = 0;
	pipe_block_num = 0;

	/* update the tape block or tape length counters and */
	/* if the block or tape limit has been reached, */
	/* close up this tape and get a new one */

	if (pipe_out_flag == FALSE)
	{
		if (by_blocks_flag == TRUE)
		{
			blocks_on_curr_tape += blocks_per_write;
			if (blocks_on_curr_tape >= curr_tape_blocks)
			{
				change_tape();
				blocks_on_curr_tape = 0;
			}
		}
		else
		{
			inches_on_curr_tape += inches_per_write;
			if (inches_on_curr_tape >= curr_tape_inches)
			{
				change_tape();
				inches_on_curr_tape = 0.0;
			}
		}
	}

	tot_blocks_written += blocks_per_write;

	/* print an estimate of the remaining time for the user */

	timeest();
}

/*
 * print out an estimate of the amount of time left to do the dump
 */

static void
timeest()
{
	static time_t	time_next_message = (time_t) 0;
	time_t		time_now;
	double		portion_finished;
	time_t		time_writing;
	time_t		total_time_needed;
	time_t		time_remaining;

	/* get the current time */

	(void) time(&time_now);

	/* if this is the first time in this routine, initialize the */
	/* next-message time to now */

	if (time_next_message == (time_t) 0)
	{
		time_next_message = time_now;
	}

	/* if fewer than 500 blocks have been written, then making an */
	/* estimate now would not be a good idea, because the potential */
	/* error would be too significant */

	if (tot_blocks_written < 500)
	{
		return;
	}

	/* if it's time to issue the next message (at least five minutes */
	/* after the last one), then do it */

	if (time_now >= time_next_message)
	{
		/* figure out what portion of the work is finished */

		portion_finished = (double) tot_blocks_written / est_tot_blocks;

		/* figure out how much more time is needed */

		time_writing = time_now - time_start_write;
		total_time_needed = (time_t) (time_writing / portion_finished);
		time_remaining = total_time_needed - time_writing;

		/* tell the operator */

		msg(MSGSTR(FINISH, "%3.2f%% done -- finished in %02d:%02d\n"),
		    portion_finished * 100.0,
		    time_remaining / HOUR,
		    (time_remaining % HOUR) / MINUTE);

		/* set the time of the next message to at least five */
		/* minutes from now */

		time_next_message = time_now + 300;
	}
}

void
open_at_start()
{
	/* alloc the request, pipe, and tape buffers */

	alloc_tape();

	/* establish check point to return to if error */

	check_point();

	/* open tape file */

	open_tape();

	spcl.c_tapea = 0;

	/* Each tape's child process is a new master and gets */
	/* a new set of slaves. */

	initialize_slaves();

	/* get the current time so we know when we started writing the */
	/* first tape */

	(void) time(&time_start_write);

	/* put volume label on tape first thing */

	volume_label();
}

static void
change_tape()
{
	int		blocks_in_prev_c_addr;
	int		i;

	/* close up slaves and wait for them to finish */

	end_slaves();

	/* remove semaphore structures */

	remove_semaphore();

	/* force tape to rewind to load point */

	rewind_tape(TRUE);

	/* ask to mount new tape, and mount it */

	mount_tape(TRUE);

	/* reinitialize the tape size from full tape size */

	if (by_blocks_flag == TRUE)
	{
		curr_tape_blocks = full_tape_blocks;
	}
	else
	{
		curr_tape_inches = full_tape_inches;
	}

	/* establish check point to return to if error */

	check_point();

	/* open tape file */

	open_tape();

	++curr_tape_num;
	++curr_volume_num;
	new_tape_flag = TRUE;

	/* Each tape's child process is a new master and gets */
	/* a new set of slaves. */

	initialize_slaves();

	/* calculate the number of data blocks remaining in the current */
	/* section of the file which was begun on the previous tape; */
	/* these data blocks will immediately follow the tape label, and */
	/* will be followed by the next spcl record of this tape */

	blocks_in_prev_c_addr = 0;
	for (i = 0; i < spcl.c_count; ++i)
	{
		if (spcl.c_addr[i] != 0)
		{
			++blocks_in_prev_c_addr;
		}
	}
	spcl.c_count = blocks_in_prev_c_addr - (spcl.c_tapea - (lastspclrec + 1));

	/* put volume label on tape first thing */

	volume_label();
}

void
close_at_end()
{
	/* close up slaves and wait for them to finish */
	end_slaves();

	/* remove semaphore structures */
	remove_semaphore();

	/* rewind the last tape, but do not force if no rewind specified */
	rewind_tape(FALSE);

	/* prompt to unmount it */
	unmount_tape();
}

void
rewrite_tape()
{
	/* kill slaves and wait for them to quit */

	kill_slaves();

	/* remove semaphore structures */

	remove_semaphore();

	/* force tape to rewind to load point */

	rewind_tape(TRUE);

	/* mount tape */

	mount_tape(FALSE);

	/* When this process exits with status X_REWRITE, its parent, */
	/* who is waiting, starts over again from the last checkpoint */
	/* on the tape this process was trying to write. */

	Exit(X_REWRITE);

	/* NOTREACHED */
}

/* Rewind_tape rewinds the tape and waits until the tape device */
/* file can be reopened, which means it is at load point. */

static void
rewind_tape(force_rewind_flag)
	int		force_rewind_flag;
{
	int		wait_for_rewind_fd;
	struct mtop	mt;

	if (pipe_out_flag == TRUE)
	{
		return;
	}

#if	EDUMP

	/* check if everything was written on to tape okay */

	if (rmtAflush() != 0)
	{
		msg(MSGSTR(TPFF, "Tape flush failed\n"));
		abort_dump();
	}

	rmtclose();

	/* open in edump will block until tape is ready */

#else	! EDUMP

	if (no_rewind_flag == FALSE || force_rewind_flag == TRUE)
	{
		switch (medium_flag) {
		case REGULAR_FILE:
			break;
		case TAPE:
			msg(MSGSTR(TREWIND, "Tape rewinding and unloading\n"));
			break;
		case CARTRIDGE:
			msg(MSGSTR(CREWIND, "Cartridge rewinding\n"));
			break;
		case DISKETTE:
			break;
		default:
			msg(MSGSTR(ILLMEDIUM, "%s(%d):Illegal flag %d.\n"),
					__FILE__, __LINE__, medium_flag);
			break;
		}

	}

#if	REMOTE

	/* close the remote device file */

	rmtclose();

	/* loop until the remote tape device file can be reopened; this will */
	/* happen when the rewound tape gets to the load point */

	while (rmtopen(tape_file_name, O_RDONLY) < 0)
	{
		(void) sleep(5);
	}
	rmtclose();

#else	! REMOTE

	/* call ioctl to rewind tape */

	/* Robboy, 3/19/93: Changed the operand mt.mt_op from  MTOFFL to
	 * MTREW, that is, from "offline" to "rewind."
	 * Taking a DAT offline ejects the  cartridge, then it
	 * can't be closed.  The code caused cascading errors on the
	 * console when it tried and retried to close the tape which had
	 * been ejected.  Now it doesn't eject the tape automatically.
	 * For cartridge tapes and SCSI tape devices, it should be sufficient
	 * to just close the device, all this rewind/wait/close business
	 * is not necessary.  In the interest of not creating bugs for
	 * some kind of old-fashioned device, I made the most conservative
	 * possible change here.
	 */
	if ((medium_flag == TAPE || medium_flag == CARTRIDGE) &&
	    (no_rewind_flag == FALSE || force_rewind_flag == TRUE) )
	{
		mt.mt_op = MTREW;
		mt.mt_count = 1;
		if (ioctl(out_tape_fd, MTIOCTOP, &mt) < 0)
		{
			msg(MSGSTR(TREWF, "Tape rewind failed\n"));
			dump_perror("rewind_tape(): ioctl()");
		}
	}

	/* close the tape device file */

	(void) close(out_tape_fd);

	/* loop until the tape device file can be reopened; this will */
	/* happen when the rewound tape gets to the load point */

	while ((wait_for_rewind_fd = open(tape_file_name, O_RDONLY)) < 0)
	{
		(void) sleep(5);
	}
	(void) close(wait_for_rewind_fd);

#endif	! REMOTE

#endif	! EDUMP

}

/* mount_tape() asks the user to mount a tape. */

static void
mount_tape(change_prompt_flag)
	int		change_prompt_flag;
{
	if (pipe_out_flag == TRUE)
	{
		return;
	}

	/* if this must be a new media volume, prompt for the next one */

	if (change_prompt_flag == TRUE)
	{
		switch (medium_flag) {
		case REGULAR_FILE:
			break;
		case TAPE:
			msg(MSGSTR(CHANGT, "Change Tapes: Mount volume %d, tape # %04d\n"), curr_volume_num + 1, curr_tape_num + 1);
			if (notify_flag == TRUE)
			{
				broadcast(MSGSTR(CHANGT2, "CHANGE TAPES!\7\7\n"));
			}
			break;
		case CARTRIDGE:
			msg(MSGSTR(CHANGC, "Change Cartridges: Insert volume %d, tape # %04d\n"), curr_volume_num + 1, curr_tape_num + 1);
			if (notify_flag == TRUE)
			{
				broadcast(MSGSTR(CHANGC2, "CHANGE CARTRIDGES!\7\7\n"));
			}
			break;
		case DISKETTE:
			msg(MSGSTR(CHANGD, "Change Diskettes: Insert volume %d, diskette # %04d\n"), curr_volume_num + 1, curr_tape_num + 1);
			if (notify_flag == TRUE)
			{
				broadcast(MSGSTR(CHANGD2, "CHANGE DISKETTES!\7\7\n"));
			}
			break;
		default:
			msg(MSGSTR(ILLMEDIUM, "%s(%d):Illegal flag %d.\n"),
					__FILE__, __LINE__, medium_flag);
			break;
		}
	}

#if	! EDUMP

	/* make sure the volume is ready */

	switch (medium_flag) {
	case REGULAR_FILE:
		break;
	case TAPE:
		while (query(MSGSTR(NEWTAPE, "Is the new tape mounted and ready to go")) == NO)
		{
			if (query(MSGSTR(ABORT1, "Do you want to abort")) == YES)
			{
				abort_dump();

				/* NOTREACHED */
			}
		}
		break;
	case CARTRIDGE:
		while (query(MSGSTR(NEWCART, "Is the new cartridge inserted and ready to go")) == NO)
		{
			if (query(MSGSTR(ABORT1, "Do you want to abort")) == YES)
			{
				abort_dump();

				/* NOTREACHED */
			}
		}
		break;
	case DISKETTE:
		while (query(MSGSTR(NEWDISK, "Is the new diskette ready to go")) == NO)
		{
			if (query(MSGSTR(ABORT1, "Do you want to abort")) == YES)
			{
				abort_dump();

				/* NOTREACHED */
			}
		}
		break;
	default:
		msg(MSGSTR(ILLMEDIUM, "%s(%d):Illegal flag %d.\n"),
				__FILE__, __LINE__, medium_flag);
		break;
	}

#endif	! EDUMP

}

static void
unmount_tape()
{
	if (pipe_out_flag == TRUE || no_rewind_flag == TRUE)
	{
		return;
	}

	switch (medium_flag) {
	case REGULAR_FILE:
		break;
	case TAPE:
		msg(MSGSTR(UNMNTT1, "Unmount last tape\n"));
		if (notify_flag == TRUE)
		{
			broadcast(MSGSTR(UNMNTT2, "UNMOUNT LAST TAPE!\7\7\n"));
		}
		break;
	case CARTRIDGE:
		msg(MSGSTR(UNMNTC1, "Remove last cartridge\n"));
		if (notify_flag == TRUE)
		{
			broadcast(MSGSTR(UNMNTC2, "REMOVE LAST CARTRIDGE!\7\7\n"));
		}
		break;
	case DISKETTE:
		msg(MSGSTR(UNMNTD1, "Remove last diskette\n"));
		if (notify_flag == TRUE)
		{
			broadcast(MSGSTR(UNMNTD2, "REMOVE LAST DISKETTE!\7\7\n"));
		}
		break;
	default:
		msg(MSGSTR(ILLMEDIUM, "%s(%d):Illegal flag %d.\n"),
				__FILE__, __LINE__, medium_flag);
		break;
	}
}

/* tape_error() is called when a SIGUSR1 is received by the master from a */
/* slave who experienced a tape write error.  The user is given the chance */
/* to redo the volume, in which case the slaves are killed, the tape is */
/* rewound, and the parent is notified (via exit status) to start the */
/* volume over. */

static void
tape_error()
{
	/* there is no way to recover (such as using a replacement media */
	/* volume) in the case of piped output */

	if (pipe_out_flag == TRUE)
	{
		msg(MSGSTR(TAPEWRE, "Write error on %s -- cannot recover\n"), tape_file_name);
		abort_dump();

		/* NOTREACHED */
	}

	/* notify the user (and the other operators) of the error */

	switch (medium_flag) {
	case REGULAR_FILE:
		break;
	case TAPE:
	case CARTRIDGE:
		if (by_blocks_flag == TRUE)
		{
			msg(MSGSTR(TAPEWRE1, "Write error %d blocks into volume %d, tape # %04d\n"), blocks_on_curr_tape, curr_volume_num, curr_tape_num);
		}
		else
		{
			msg(MSGSTR(TAPEWRE2, "Tape write error %d feet into volume %d, tape # %04d\n"), (int) ((inches_on_curr_tape / 12) + 0.5), curr_volume_num, curr_tape_num);
		}

		if (notify_flag == TRUE)
		{
			broadcast(MSGSTR(TAPEEE, "TAPE ERROR!\7\7\n"));
		}
		break;
	case DISKETTE:
		msg(MSGSTR(DISKWRE, "Diskette write error %d blocks into volume %d, diskette # %04d\n"), blocks_on_curr_tape, curr_volume_num, curr_tape_num);

		if (notify_flag == TRUE)
		{
			broadcast(MSGSTR(DISKEE, "DISKETTE ERROR!\7\7\n"));
		}
		break;
	default:
		msg(MSGSTR(ILLMEDIUM, "%s(%d):Illegal flag %d.\n"),
				__FILE__, __LINE__, medium_flag);
		break;
	}

#if	EDUMP

	msg(MSGSTR(UNLDCR1, "Unloading current tape\n"));
	msg(MSGSTR(UNLDCR2, "Expecting new tape to be mounted\n"));

#else	! EDUMP

	/* give the user the chance to restart this tape/diskette */

	if (query(MSGSTR(RESTART, "Do you want to restart this volume")) == NO)
	{
		abort_dump();

		/* NOTREACHED */
	}

	/* prompt for a new media volume */

	switch (medium_flag) {
	case REGULAR_FILE:
		break;
	case TAPE:
		msg(MSGSTR(REWIND, "This tape will rewind\n"));
		msg(MSGSTR(REPLACE, "Then replace the faulty tape with a new one\n"));
		break;
	case CARTRIDGE:
		msg(MSGSTR(CWREWIND, "This cartridge will rewind\n"));
		msg(MSGSTR(CREPLACE, "Then replace the faulty cartridge with a new one\n"));
		break;
	case DISKETTE:
		msg(MSGSTR(REPLACED, "Replace the faulty diskette with a new one\n"));
		break;
	default:
		msg(MSGSTR(ILLMEDIUM, "%s(%d):Illegal flag %d.\n"),
				__FILE__, __LINE__, medium_flag);
		break;
	}

	msg(MSGSTR(REWRITE, "This dump volume will be rewritten\n"));
	msg(MSGSTR(IFMORE1, "If this volume contained more than one\n"));
	msg(MSGSTR(IFMORE2, "dump, only the last one will be rewritten\n"));
	msg(MSGSTR(IFMORE3, "on the new volume; previous dumps are still\n"));
	msg(MSGSTR(IFMORE4, "intact on the faulty volume\n"));

#endif	! EDUMP

	rewrite_tape();

	/* NOTREACHED */
}

/*
 * if a broken pipe is detected, program unceremoniously dies by calling
 * abort_dump()
 */

static void
sigpipe()
{
	msg(MSGSTR(PIPEB, "Broken pipe\n"));
	abort_dump();

	/* NOTREACHED */
}

/*
 * We implement taking and restoring checkpoints on the tape level.
 *
 * Before each tape is opened, check_point() is called to created a new
 * process by forking; this saves all of the necessary context in the parent.
 *
 * The child continues the dump; the parent waits around, saving the context.
 *
 * If the child returns X_REWRITE, then it had problems writing that tape;
 * this causes the parent to fork again, duplicating the context, and
 * everything continues as if nothing had happened.
 */

static void
check_point()
{
	int		parent_pid;
	int		child_pid;
	int		child_status;
	int		waitpid;
	void	      (*save_int)();

	parent_pid = getpid();

	/* save the interrupt handler so that it can be restored */
	/* by the parent each time just before it is going to fork a new */
	/* child */

	save_int = signal(SIGINT, SIG_IGN);

	while (TRUE)
	{
		/* loop while is parent and child exits with X_REWRITE */

		/* restore the interrupt handler for the child */

		(void) signal(SIGINT, save_int);

		child_pid = fork();

		if (child_pid < 0)
		{
			child_pid = 0; /* paranoia */
			msg(MSGSTR(FORKF, "Checkpoint fork fails in parent %d\n"), parent_pid);
			dump_perror("check_point(): fork()");
			abort_dump();

			/* NOTREACHED */
		}

		if (child_pid != 0)
		{
			/*
			 * PARENT: save the context by waiting until the child
			 * doing all of the work exits.
			 *
			 * Don't catch the interrupt, child will do it
			 */

			(void) signal(SIGINT, SIG_IGN);

#if	TDEBUG

			msg("volume: %d, tape: %d, parent process: %d, child process %d\n",
			    curr_volume_num + 1, curr_tape_num + 1, parent_pid, child_pid);

#endif	TDEBUG

			/* wait for the child to return */

			while ((waitpid = wait(&child_status)) != child_pid)
			{
				msg(MSGSTR(DUMPPWAIT, "Parent %d waiting for child %d has another child %d return\n"),
				    parent_pid, child_pid, waitpid);
			}

			if (child_status & 0xFF)
			{
				msg(MSGSTR(CHILDR, "Child %d returns low-byte status %o\n"), child_pid, child_status & 0xFF);
			}

			child_status = (child_status >> 8) & 0xFF;

			/* exit with child's return status unless it was */
			/* X_REWRITE, in which case loop and try to write */
			/* the tape again. */

			switch (child_status)
			{
			case X_FINOK:

#if	TDEBUG

				msg("Child %d exits X_FINOK\n", child_pid);

#endif	TDEBUG

				Exit(X_FINOK);

				/* NOTREACHED */

			case X_ABORT:

#if	TDEBUG

				msg("Child %d exits X_ABORT\n", child_pid);

#endif	TDEBUG

				abort_dump();

				/* NOTREACHED */

			case X_REWRITE:

#if	TDEBUG

				msg("Child %d exits X_REWRITE\n", child_pid);

#endif	TDEBUG

				/* reinitialize the tape size from full tape size */

				if (by_blocks_flag == TRUE)
				{
					curr_tape_blocks = full_tape_blocks;
				}
				else
				{
					curr_tape_inches = full_tape_inches;
				}
				continue;

			default:

#if	TDEBUG

				msg("Child %d exits unknown %d\n", child_pid, child_status);

#endif	TDEBUG

				msg(MSGSTR(BADCODE, "Bad exit status from child %d: %d\n"), child_pid, child_status);
				abort_dump();

				/* NOTREACHED */
			}

			/* NOTREACHED */
		}

		/* we are the child process */

		/* break out of loop and return to caller to do the */
		/* rest of the work */

#if	TDEBUG
		/* allow time for parent's message to get out */

		(void) sleep(4);

		msg("Volume: %d, tape: %d, parent process: %d, child process %d\n",
		    curr_volume_num + 1, curr_tape_num + 1, parent_pid, getpid());

#endif	TDEBUG
		break;

	}

	/* only the child process reaches here to return to caller */
}

static void
open_tape()
{

#if	EDUMP

	/* just open the tape up, do not query user */

	out_tape_fd = rmtopen(tape_file_name, O_WRONLY | O_CREAT | O_TRUNC);
	if (out_tape_fd < 0)
	{
		msg(MSGSTR(OPENRTF, "Cannot open remote device file %s\n"), tape_file_name);
		dump_perror("open_tape(): rmtopen()");
		abort_dump();
	}

#else	! EDUMP

#if	! REMOTE

	if (pipe_out_flag == FALSE)
	{
		while (check_tape() == FALSE)
		{
			if (query(MSGSTR(ITCTMR, "Is the correct tape mounted and ready to go")) == NO)
			{
				if (query(MSGSTR(DYWTABT, "Do you wish to abort the dump")) == YES)
				{
					abort_dump();
				}
			}
		}
	}

#endif	! REMOTE

	/* try to open the output tape device file or channel until */
	/* successful or the user decides to quit */

#if	REMOTE

	while ((out_tape_fd = rmtopen(tape_file_name, O_WRONLY | O_CREAT | O_TRUNC)) < 0)
	{
		msg(MSGSTR(OPENRTF, "Cannot open remote device file %s\n"), tape_file_name);
		dump_perror("open_tape(): rmtopen()");

#else	! REMOTE

	while ((out_tape_fd = (pipe_out_flag == TRUE)? 1: open(tape_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
	{
		msg(MSGSTR(OPENTF, "Cannot open device file %s\n"), tape_file_name);
		dump_perror("open_tape(): open()");

#endif	! REMOTE

		if (query(MSGSTR(RTRYOP, "Do you want to retry the open")) == NO)
		{
			abort_dump();

			/* NOTREACHED */
		}
	}

#endif	! EDUMP

}

#define	NUM_SLAVES	3	/* 1 slave writing, 1 reading, 1 for slack */

static int	master_pid = 0;	/* pid of master, for sending error signals */
static int	next_slave;	/* next slave to be instructed */

static int	slave_write_fd[NUM_SLAVES];	/* pipes from master to slaves */
static int	slave_pid[NUM_SLAVES] = {0};	/* slave pids used by kill_slaves() */
static int	slave_semid = 0;		/* handle for the semaphore array */

/* abort_dump() causes entire dump job to abort immediately */

void
abort_dump()
{
	if (master_pid == 0)
	{
		/* no master nor slaves yet */

		msg(MSGSTR(ENTIRE, "The ENTIRE dump is aborted\n"));
	}
	else if (master_pid == getpid())
	{
		/* is master */
		/* kill all slaves */

		kill_slaves();

		/* remove semaphore structures */

		remove_semaphore();

		msg(MSGSTR(ENTIRE, "The ENTIRE dump is aborted\n"));
	}
	else
	{
		/* is slave */
		/* Signal master to call abort_dump */

		(void) kill(master_pid, SIGTERM);
	}
	Exit(X_ABORT);

	/* NOTREACHED */
}

/* Write_to_slave() writes the buffer to the next slave in the loop */

static void
write_to_slave(buf_to_write, byte_length)
	char	       *buf_to_write;
	int		byte_length;
{
	if (atomic(write, slave_write_fd[next_slave], buf_to_write, byte_length) != byte_length)
	{
		msg(MSGSTR(CWTS, "Cannot write to slave pipe\n"));
		dump_perror("write_to_slave(): write()");
		abort_dump();

		/* NOTREACHED */
	}

	/* figure out which slave will get the next piped request block */

	next_slave = (next_slave + 1) % NUM_SLAVES;
}

/* initialize semaphores for slave control, and start up the slaves */

static void
initialize_slaves()
{
	int		slave_num;

	/* allocate the semaphores with which the slaves will coordinate */
	/* the tape writing */

	get_semaphore();

	/* block all slaves from writing the tape initially */

	for (slave_num = 0; slave_num < NUM_SLAVES; ++slave_num)
	{
		slave_block(slave_num);
	}

	/* start up the slaves which will do the disk reading and tape */
	/* writing; the master will just do the inode traversal */

	start_slaves();

	/* pick first (zeroth) slave as next one */

	next_slave = 0;

	/* unblock the first slave to start the slave coordination loop */

	slave_unblock(next_slave);
}

/* Close_slaves() closes the slaves pipe fd's so that they will get an EOF */
/* and finish up.  Then we wait for all of the slaves to exit themselves. */

static void
end_slaves()
{
	int		slave_num;

	/* close the write descriptors on the pipe, so child could get an eof */

	for (slave_num = 0; slave_num < NUM_SLAVES; ++slave_num)
	{
		(void) close(slave_write_fd[slave_num]);
		slave_pid[slave_num] = 0;
	}

	/* wait for slaves to finish and exit */

	while (wait(NULL) >= 0)
	{
		;
	}
}

/* Kill_slaves() sends a SIGKILL to each slave and waits for them to finish */

static void
kill_slaves()
{
	register int	slave_num;

	for (slave_num = 0; slave_num < NUM_SLAVES; ++slave_num)
	{
		if (slave_pid[slave_num] != 0)
		{
			(void) kill(slave_pid[slave_num], SIGKILL);
		}
		slave_pid[slave_num] = 0;
	}

	/* wait for slaves to be killed */

	while (wait(NULL) >= 0)
	{
		;
	}
}

/* Exit() simply passes arg through to exit after print message in debug mode */

void
Exit(status)
{

#if	TDEBUG

	msg("Pid %d exits with status %d\n", getpid(), status);

#endif	TDEBUG

	exit(status);

	/* NOTREACHED */
}

/*
 * Start the slave processes.  Each slave will enter the routine
 * slave_work() and never return.  Only the master returns to the
 * calling routine.
 */

static void
start_slaves()
{
	register int	slave_num;
	int		prev_slave_num;
	int		pipe_fd[2];

	master_pid = getpid();

	(void) signal(SIGTERM, abort_dump);	/* Slave sends SIGTERM on abort_dump() */
	(void) signal(SIGUSR1, tape_error);	/* Slave sends SIGUSR1 on tape errors */
	(void) signal(SIGPIPE, sigpipe);

	for (slave_num = 0; slave_num < NUM_SLAVES; ++slave_num)
	{
		slave_pid[slave_num] = 0;
	}

	for (slave_num = 0; slave_num < NUM_SLAVES; ++slave_num)
	{
		if (pipe(pipe_fd) < 0)
		{
			msg(MSGSTR(SLAVES1, "Cannot open slave pipe\n"));
			dump_perror("start_slaves(): pipe()");
			abort_dump();

			/* NOTREACHED */
		}

		if ((slave_pid[slave_num] = fork()) < 0)
		{
			/* Avoid killing pid of -1 in kill_slaves() */
			slave_pid[slave_num] = 0;
			msg(MSGSTR(SLAVES2, "Cannot fork slave %d\n"), slave_num);
			dump_perror("start_slaves(): fork()");
			abort_dump();

			/* NOTREACHED */
		}

#if	TDEBUG

		msg("slavepid[%d] %d\n", slave_num, slave_pid[slave_num]);

#endif	TDEBUG

		/* master keeps write end fd of pipe to slave */

		slave_write_fd[slave_num] = pipe_fd[1];

		if (slave_pid[slave_num] == 0)
		{	
			/* Slave starts up here */

			/* ignore interrupt; master will catch it */

			(void) signal(SIGINT, SIG_IGN);

			/* close master's write end fds of pipes to */
			/* previously forked slaves and self */

			for (prev_slave_num = 0; prev_slave_num <= slave_num; ++prev_slave_num)
			{
				(void) close(slave_write_fd[prev_slave_num]);
			}

			/* do slave work and do not return */

			slave_work(pipe_fd[0], slave_num);

			/* NOTREACHED */
		}

		/* master only */

		/* close read end fd of slave pipe */

		(void) close(pipe_fd[0]);
	}

	/* only master returns */

}

/*
 * Synchronization - each slave has an associated semaphore. 
 * When one slave completes its tape write, it increments the semaphore
 * value of the next slave in the loop, so the next slave can awaken.
 * The current slave then decrements its own semaphore value, to zero,
 * at which point the slave will block waiting for the previous slave in
 * the loop to increment it again.  This essentially a circular token
 * passing scheme of slaves.
 */

/* Doslave() is where the slave stays until the master writes it no */
/* more instructions over the pipe.  When the slave leaves this routine */
/* it will exit immediately in the calling routine. */

static void
slave_work(slave_read_fd, slave_num)
	int		slave_read_fd;
	int		slave_num;
{
	int		bytes_read;
	int		next_slave_in_loop;
	int		request_num;
	int		tape_block_num;
	int		bytes_written;

	next_slave_in_loop = (slave_num + 1) % NUM_SLAVES;

	/* get slave's own seek pointer into input disk file by closing */
	/* and reopening the file */

	(void) close(in_disk_fd);
	if ((in_disk_fd = open(disk_file_name, O_RDONLY)) < 0)
	{
		msg(MSGSTR(CRODF, "Cannot re-open file system file %s\n"), disk_file_name);
		dump_perror("slave_work(): open()");
		abort_dump();

		/* NOTREACHED */
	}

	/* loop as long as request arrays can be read from the pipe */

	while ((bytes_read = atomic(read, slave_read_fd, request_array, request_array_size)) == request_array_size)
	{
		/* loop through the request array until enough tape blocks */
		/* for a full tape write have been gathered */

		for (tape_block_num = 0, request_num = 0;
		     tape_block_num < blocks_per_write;
		     tape_block_num += request_array[request_num].num_blocks, ++request_num)
		{
			/* If the device_block_num of the request is valid */
			/* (non-zero), get the requested number of tape */
			/* blocks from the disk into the tape buffer. */

			/* Otherwise, read the next tape block directly from */
			/* the pipe from the master. */

			if (request_array[request_num].device_block_num != (daddr_t) 0)
			{
				bread(request_array[request_num].device_block_num,
				      (char *) &tape_buffer[tape_block_num],
				      (int) (request_array[request_num].num_blocks * sizeof(struct tp_block)));
			}
			else if (request_array[request_num].num_blocks != 1)
			{
				msg(MSGSTR(MASSLV1, "Master/slave protocol botched\n"));
				abort_dump();

				/* NOTREACHED */
			}
			else if (atomic(read,
					slave_read_fd,
					(char *) &tape_buffer[tape_block_num],
					sizeof(struct tp_block)) != sizeof(struct tp_block))
			{
				msg(MSGSTR(MASSLV2, "Cannot read from master pipe\n"));
				dump_perror("slave_work(): read()");
				abort_dump();

				/* NOTREACHED */
			}
		}

		/* a full tape-buffer has now been obtained, now write it */
		/* out to the tape */

		/* first wait until this slave's turn in the tape writing */
		/* loop */

		slave_wait(slave_num);

		/* now it is our turn to write */

		errno = 0;	/* clear error info before writing */

#if	EDUMP

		if ((bytes_written = rmtAwrite((char *) tape_buffer, tape_write_size)) != tape_write_size)

#else	! EDUMP

#if	REMOTE

		if ((bytes_written = rmtwrite((char *) tape_buffer, tape_write_size)) != tape_write_size)

#else	! REMOTE

		if ((bytes_written = write(out_tape_fd, (char *) tape_buffer, tape_write_size)) != tape_write_size)

#endif	! REMOTE

#endif	! EDUMP

		{
			/* write error, so signal tape error to master */

			msg(MSGSTR(TAPWERW, "Tape write error -- wanted to write: %d, only wrote: %d\n"),
			    tape_write_size, bytes_written);
			dump_perror("slave_work(): write()");

			(void) kill(master_pid, SIGUSR1);
			for (;;)
			{
				(void) sigpause(0);
			}
		}

		/* block ourself until so the previous slave can wake */
		/* us up next time around */

		slave_block(slave_num);

		/* unblock the next slave so he can write the next tape block */

		slave_unblock(next_slave_in_loop);
	}

	if (bytes_read != 0)
	{
		msg(MSGSTR(SCNTRD, "Slave cannot read from pipe\n"));
		dump_perror("slave_work(): read()");
		abort_dump();

		/* NOTREACHED */
	}

	/*
	 * empty read on pipe, slave is finished
	 *
	 * When EOF is received, all following slaves will also receive
	 * EOF, so they do not need to be unblocked anymore.
	 *
	 * The slave with the last write will try to unblock the first
	 * slave to get an EOF, who will probably have exited already.
	 * There is no problem here because slave_block() and
	 * slave_unblock() themselves will not block.
	 */

	Exit(X_FINOK);

	/* NOTREACHED */
}

/*
 * gets a semaphore id associated with an array of NUM_SLAVES semaphores
 * to be used to coordinate tape writes
 */

static void
get_semaphore()
{
	int			slave_num;
	/*key_t*/ long		key;

#if	FILE_TO_KEY

	/* make keys from ftok() */

	char			tmpname[256];
	int			fd;
	extern /*key_t*/ long	ftok();

	(void) strcpy(tmpname, "/tmp/dumplockXXXXXX");
	(void) mktemp(tmpname);

	if ((fd = open(tmpname, O_CREAT | O_EXCL, 0600)) < 0)
	{
		msg(MSGSTR(LOCKF, "Cannot create key-creation lock file %s\n", tmpname));
		dump_perror("get_semaphore(): open()");
		abort_dump();

		/* NOTREACHED */
	}

	if ((key = ftok(tmpname, 's')) < 0)
	{
		msg(MSGSTR(FTOKF, "Cannot obtain key\n"));
		dump_perror("get_semaphore(): ftok()");
		abort_dump();

		/* NOTREACHED */
	}

	(void) close(fd);
	(void) unlink(tmpname);

#elif	MAKE_KEY

	/* make key with top-bytes 'b' and 'r', and last-bytes getpid() */

	key = (/*key_t*/ long) ((((long) 'b') << 24) |
				(((long) 'r') << 16) |
				(((long) getpid()) & 0xFFFF));

#else	PRIVATE_KEY

	key = IPC_PRIVATE;

#endif	PRIVATE_KEY

	/* make an array of NUM_SLAVES semaphores */

	if ((slave_semid = semget(key, NUM_SLAVES, 0600 | IPC_CREAT)) < 0)
	{
		msg(MSGSTR(SEMGETE, "Cannot get semaphores\n"));
		dump_perror("get_semaphore(): semget()");
		abort_dump();

		/* NOTREACHED */
	}

	/* initialize each semaphore value to zero (unblocked) */

	for (slave_num = 0; slave_num < NUM_SLAVES; ++slave_num)
	{
		if (semctl(slave_semid, slave_num, SETVAL, 0) < 0)
		{
			msg(MSGSTR(SEMCTLE, "Cannot set semaphore values\n"));
			dump_perror("get_semaphore(): semctl()");
			abort_dump();

			/* NOTREACHED */
		}
	}
}

/* block the slave whose number is passed as the argument by adding 1 to */
/* the associated semaphore in the semaphore array bringing its value up to 1 */
/* so that when the blocked slave does slave_wait() it will block until */
/* its predecessor unblocks it by subtracting 1 from its semaphore bringing */
/* its value back to 0 */

static void
slave_block(slave_num)
	int		slave_num;
{
	struct sembuf	sembuf;

	sembuf.sem_num = slave_num;
	sembuf.sem_op = 1;
	sembuf.sem_flg = IPC_NOWAIT;

	if (semop(slave_semid, &sembuf, 1) < 0 && errno != EAGAIN)
	{
		msg(MSGSTR(CNBSP, "Cannot block slave process %d\n"), slave_num);
		dump_perror("slave_block(): semop()");
		abort_dump();

		/* NOTREACHED */
	}
}

/* unblock the slave whose number is passed as the argument by subtracting 1 */
/* from the associated semaphore in the semaphore array bringing its value */
/* back to 0 so that the blocked slave which is waiting in slave_wait() */
/* will unblock and be allowed to proceed */

static void
slave_unblock(slave_num)
	int		slave_num;
{
	struct sembuf	sembuf;

	sembuf.sem_num = slave_num;
	sembuf.sem_op = -1;
	sembuf.sem_flg = IPC_NOWAIT;

	if (semop(slave_semid, &sembuf, 1) < 0 && errno != EAGAIN)
	{
		msg(MSGSTR(CNUSP, "Cannot unblock slave process %d\n"), slave_num);
		dump_perror("slave_unblock(): semop()");
		abort_dump();

		/* NOTREACHED */
	}
}

/* suspend execution until the semaphore in the semaphore array indexed */
/* by slave_num is unblocked (reset to zero) by the previous slave */

static void
slave_wait(slave_num)
	int		slave_num;
{
	struct sembuf	sembuf;

	sembuf.sem_num = slave_num;
	sembuf.sem_op = 0;
	sembuf.sem_flg = 0;

	if (semop(slave_semid, &sembuf, 1) < 0)
	{
		msg(MSGSTR(CNWSP, "Slave process %d cannot wait successfully\n"), slave_num);
		dump_perror("slave_wait(): semop()");
		abort_dump();

		/* NOTREACHED */
	}
}

/*
 * The tape is completed, so we can now remove the semaphore id.
 */

static void
remove_semaphore()
{
	if (slave_semid != 0)
	{
		if (semctl(slave_semid, 0, IPC_RMID, 0) < 0)
		{
			msg(MSGSTR(SEMCTL2, "Cannot remove semaphores\n"));
			dump_perror("remove_semaphore(): semctl()");
			slave_semid = 0;
		}
	}
}

/*
 * Since a read from a pipe may not return all we asked for, or a write may not
 * write all we ask if we get a signal, loop until the count is satisfied (or
 * error).
 */

static int
atomic(func, fd, buf, count)
	int	      (*func)(), fd, count;
	char	       *buf;
{
	int		got, need = count;

	while ((got = (*func)(fd, buf, need)) > 0 && (need -= got) > 0)
	{
		buf += got;
	}
	return((got < 0)? got: count - need);
}
