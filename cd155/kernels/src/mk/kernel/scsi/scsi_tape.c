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
 * $Log: scsi_tape.c,v $
 * Revision 1.19  1995/03/28  17:33:51  jerrie
 * Added SCSI_CMD_QUEUE_DEPTH bootmagic to control tagged queue depth.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low
 *  Benefit or PTS #: Provides a way to change the queue depth.
 *  Testing:	   Developer
 *  Module(s):	   scsi_disk.c, scsi_tape.c
 *
 * Revision 1.18  1995/03/14  23:49:24  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.17  1995/02/23  17:52:00  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #:12499
 *  Testing:read and set density on 8500 then read the tape on an 8200
 *  Module(s): sctape_mode_select()
 *
 * Revision 1.16  1994/11/18  21:00:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.15  1994/08/31  21:25:59  mtm
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
 * Revision 1.13.8.1  1994/08/04  20:38:19  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: Low
 *  Benefit or PTS #: 10329
 *  Testing: tested against eval's cache testing programs
 *  Module(s): sctape_mode_select
 *
 * Revision 1.13  1994/04/20  22:20:48  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: added for 3480 Tape Library support - LOAD DISPLAY.
 *  Testing: tape EAT's and costom code to exercise the ioctl
 *  Module(s): added scsi_cmd_load_display().
 *
 * Revision 1.12  1994/03/08  17:34:27  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: adding 3480 tape library support
 *  Testing: tape EAT's, a short program that uses the MTLOCATE ioctl
 *  Module(s): added the scsi_locate function.
 *
 * Revision 1.11  1994/03/03  23:15:16  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: adding support for 3480 tape library - READ POSITION
 * 		SCSI command.
 *  Testing: tape EAT's, test program that uses the ioctl.
 *  Module(s): added scsi_read_position().
 *
 * Revision 1.10  1994/02/09  21:54:55  richardg
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 * This merges in the fix for PTS #7986.
 *
 * Revision 1.9  1993/09/24  17:38:53  jerrie
 * Modified the sctape_mode_select function to support tape data compression.
 * Corrected bug when building the SCSI command in sctape_read.
 *
 * Revision 1.8  1993/09/16  18:12:29  richardg
 * modified the memory allocation to evenly divide available MIO memory between existing scsi devices
 *
 * Revision 1.7  1993/06/30  22:54:23  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.6  1993/06/01  21:44:57  richardg
 * a bug fix back in R1.0 that writes EOF to tapes that have the TGT_WRITTEN_TO
 * bit set before they are rewound, was somehow lost.  This puts it back in the
 * main trunk.
 *
 * Revision 1.5  1993/05/12  18:23:58  richardg
 * PTS #3135.  Commented out the use of ior->io_residual in the read and write
 * routines. residual is being manipulated in sctape_start to handle > max_dma r/w
 * requests.
 *
 * Revision 2.11.2.1  92/03/28  10:16:14  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:33:18  jeffreyh]
 * 
 * Revision 2.12  92/02/23  22:45:07  elf
 * 	Changed the interface of a number of functions not to
 * 	require the scsi_softc pointer any longer.  It was
 * 	mostly unused, now it can be found via tgt->masterno.
 * 	[92/02/22  19:29:10  af]
 * 
 * Revision 2.11  91/11/12  11:17:27  rvb
 * 	Deal with fixed-size tapes more gracefully: fixed a bug in
 * 	read (incorrectly called the go routine with the number of
 * 	blocks rather than the number of bytes), in mode select
 * 	set the block size too.
 * 	We seem to be happy with DEC TZK10s now.
 * 	[91/10/30  13:39:34  af]
 * 
 * Revision 2.10  91/08/24  12:29:16  af
 * 	Support for fixed-size-tapes: careful about speed selections,
 * 	round sizes properly, use fixed bit.
 * 	[91/08/02  03:58:18  af]
 * 
 * Revision 2.9  91/06/19  11:58:13  rvb
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.8  91/05/14  17:31:26  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/05/13  06:35:55  af
 * 	Cleaned up mode_select, vendor unique data can be passed
 * 	in if necessary.  Density and speed are taken from target 
 * 	structure.  Added rewind command and all th eothers that
 * 	were missing (but what we do not use is disabled).
 * 
 * Revision 2.6.2.1  91/05/12  16:25:01  af
 * 	Cleaned up mode_select, vendor unique data can be passed
 * 	in if necessary.  Density and speed are taken from target 
 * 	structure.  Added rewind command and all th eothers that
 * 	were missing (but what we do not use is disabled).
 * 
 * Revision 2.6.1.1  91/03/29  17:26:59  af
 * 	Cleaned up mode_select, vendor unique data can be passed
 * 	in if necessary.  Density and speed are taken from target 
 * 	structure.  Added rewind command and all th eothers that
 * 	were missing (but what we do not use is disabled).
 * 
 * 
 * Revision 2.6  91/02/05  17:46:24  mrt
 * 	Added author notices
 * 	[91/02/04  11:20:18  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:19:07  mrt]
 * 
 * Revision 2.5  90/12/05  23:35:49  af
 * 	Mild attempt to get it working.  Actually wrote and read
 * 	back a tape.
 * 	[90/12/03  23:49:49  af]
 * 
 * Revision 2.3.1.1  90/11/01  03:40:30  af
 * 	Created, from the SCSI specs:
 * 	"Small Computer Systems Interface (SCSI)", ANSI Draft
 * 	X3T9.2/82-2 - Rev 17B December 1985
 * 	"Small Computer System Interface - 2 (SCSI-II)", ANSI Draft
 * 	X3T9.2/86-109 -  Rev 10C March 1990
 * 	[90/10/11            af]
 */
/*
 *	File: scsi_tape.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Middle layer of the SCSI driver: SCSI protocol implementation
 *
 * This file contains code for SCSI commands for SEQUENTIAL ACCESS devices.
 */

#include <mach/std_types.h>
#include <scsi/compat_30.h>

#include <scsi/scsi.h>
#include <scsi/scsi2.h>
#include <scsi/scsi_defs.h>

char *sctape_name(internal)
	boolean_t	internal;
{
	return internal ? "tz" : "tape";
}

sctape_optimize(tgt)
	target_info_t		*tgt;
{
	register int 	i;
	char		result[6];

	/* Some (DEC) tapes want to send you the self-test results */
	for (i = 0; i < 10; i++) {
		if (scsi_receive_diag( tgt, result, sizeof(result), 0)
		    == SCSI_RET_SUCCESS)
			break;
	}
	if (scsi_debug)
		printf("[tape_rcvdiag: after %d, x%x x%x x%x x%x x%x x%x]\n", i+1,
		result[0], result[1], result[2], result[3], result[4], result[5]);

#ifdef	TZ_TAGGED_QUEUING
	/*
	 * If the device supports tagged command queuing, set up the
	 * control mode page for the expected error handling behavior.
	 */
	if (tgt->flags & TGT_TAGGED_QUEUING) {
		scsi_ret_t			ret;
		scsi_mode_sense_page10_t	*page10;
		int				depth;

		/*
		 * Read the current values
		 */
		ret = scsi_mode_sense(tgt, 10,
			sizeof(scsi_mode_sense_data_t) +
			sizeof(scsi_mode_sense_page10_t), 0);

		if (ret != SCSI_RET_SUCCESS) {
			if (scsi_debug) {
				printf("[sctape_optimize: %s]\n",
				    "mode sense error - page 10 (changeable)");
			}

			/*
			 * In this case we can't support tagged
			 * command queuing on the device.
			 */
			BCLR(scsi_should_queue, tgt->masterno, tgt->target_id);

			return;
		}

		page10 = (scsi_mode_sense_page10_t *)
			(((scsi_mode_sense_data_t *)tgt->cmd_ptr) + 1);

		/*
		 * Check the current values.  For tapes, we want dque
		 * off (queuing enabled), qerr off (continue executing
		 * queued commands), and q_algorithm set for restriced
		 * re-ordering.
		 */
		if ((page10->dque != 0) ||
		    (page10->qerr != 0) ||
		    (page10->q_algorithm != SCSI_RESTRICTED_REORDERING)) {
			unsigned char		page10_buf
					[sizeof(scsi_mode_sense_page10_t)];

			/* Change the settings to our liking */
			page10->dque = 0;
			page10->qerr = 0;
			page10->q_algorithm = SCSI_RESTRICTED_REORDERING;

			/* Copy sense data to a temporary buffer */
			bcopy((char *)page10, page10_buf, sizeof(page10_buf));

			/* Select new values */
			ret = sctape_mode_select(tgt, page10_buf,
				sizeof(page10_buf), FALSE, FALSE, 0);

			if (ret != SCSI_RET_SUCCESS) {
				if (scsi_debug) {
					printf("[sctape_optimize: %s]\n",
						"mode select error - page 10");
				}

				/*
				 * In this case we can't support tagged
				 * command queuing on the device.
				 */
				BCLR(scsi_should_queue, tgt->masterno,
					tgt->target_id);

				return;
			}
		}

		/*
		 * Initialize the controller interface
		 * layer to support tagged command queuing,
		 * default to 64 queued commands for now
		 */
		depth = getbootint("SCSI_CMD_QUEUE_DEPTH", 64);
		if (scsi_softc[tgt->masterno]->tag_q_init) {
			(scsi_softc[tgt->masterno]->tag_q_init)(tgt, depth);
		}
	}
#else	TZ_TAGGED_QUEUING
	/*
	 * Disable tagged command queuing for tape devices
	 * Turn off both flags to be safe
	 */
	tgt->flags &= ~TGT_TAGGED_QUEUING;
	BCLR(scsi_should_queue, tgt->masterno, tgt->target_id);
#endif	TZ_TAGGED_QUEUING
}

/*
 * SCSI commands specific to sequential access devices
 */
#ifdef	PARAGON860	/* Tape Compression Support */
sctape_mode_select( tgt, data, data_len, vuque, newspeed, ior)
#else	PARAGON860	/* Tape Compression Support */
sctape_mode_select( tgt, vuque_data, vuque_data_len, newspeed, ior)
#endif	PARAGON860	/* Tape Compression Support */
	register target_info_t	*tgt;
#ifdef	PARAGON860	/* Tape Compression Support */
	unsigned char		*data;
	int			data_len;
	boolean_t		vuque;
#else	PARAGON860	/* Tape Compression Support */
	unsigned char		*vuque_data;
	int			vuque_data_len;
#endif	PARAGON860	/* Tape Compression Support */
	io_req_t		ior;
{
	scsi_cmd_mode_select_t	*cmd;
	scsi_mode_select_param_t	*parm;
	tape_spec_t			*tape_spec;
	register int			offs;

	bzero(tgt->cmd_ptr, sizeof(*cmd) + 2 * sizeof(*parm));
	cmd = (scsi_cmd_mode_select_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_MODE_SELECT;
#ifdef	PARAGON860	/* Tape Compression Support */
	cmd->scsi_cmd_lun_and_lba1 = (vuque) ? 0 : SCSI_CMD_MSL_PF;
	cmd->scsi_cmd_xfer_len = sizeof(scsi_mode_select_param_t) + data_len;
#else	PARAGON860	/* Tape Compression Support */
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_xfer_len = sizeof(scsi_mode_select_param_t) + vuque_data_len;
#endif	PARAGON860	/* Tape Compression Support */

	parm = (scsi_mode_select_param_t*) (cmd + 1);
	tape_spec = (tape_spec_t *)&parm->device_spec;
	if (newspeed) {
		tape_spec->speed = tgt->dev_info.tape.speed;
		tape_spec->buffer_mode = tgt->dev_info.tape.buffer_mode;
		parm->descs[0].density_code = tgt->dev_info.tape.density;
	} else {
		tape_spec->buffer_mode = tgt->dev_info.tape.buffer_mode;
		tape_spec->speed = 0;
		parm->descs[0].density_code = tgt->dev_info.tape.density;;
	}
	parm->desc_len = 8;
	if (tgt->dev_info.tape.fixed_size) {
		register int reclen = tgt->block_size;
		parm->descs[0].reclen1 = reclen >> 16;
		parm->descs[0].reclen2 = reclen >>  8;
		parm->descs[0].reclen3 = reclen;
	}

#ifdef	PARAGON860	/* Tape Compression Support */
	if (data_len)
		bcopy(data, (char*)(parm+1), data_len);
#else	PARAGON860	/* Tape Compression Support */
	if (vuque_data_len)
		bcopy(vuque_data, (char*)(parm+1), vuque_data_len);
#endif	PARAGON860	/* Tape Compression Support */

	tgt->cur_cmd = SCSI_CMD_MODE_SELECT;

#ifdef	PARAGON860	/* Tape Compression Support */
	scsi_go_and_wait(tgt, sizeof(*cmd) + sizeof(*parm) + data_len, 0, ior);
#else	PARAGON860	/* Tape Compression Support */
	scsi_go_and_wait(tgt, sizeof(*cmd) + sizeof(*parm) + vuque_data_len, 0, ior);
#endif	PARAGON860	/* Tape Compression Support */

	return tgt->done;
}

sctape_read( tgt, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_read_t		*cmd;
	register unsigned	len, max;
#	define			nbytes max
	boolean_t		fixed = FALSE;

#ifdef PARAGON860       /* performance mod */
        max = tgt->max_dma_data;
#else
	max = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	len = ior->io_count;
	if (tgt->dev_info.tape.fixed_size) {
		unsigned int bs = tgt->block_size;
		fixed = TRUE;
		nbytes = len;
/* residual is being used in sctape_start for > max_dma_data requests */
/*		ior->io_residual += len % bs; */
		len = len / bs;
	} else {
		if (max > tgt->dev_info.tape.maxreclen)
			max = tgt->dev_info.tape.maxreclen;
		if (len > max) {
/* residual is being used in sctape_start for > max_dma_data requests */
/*			ior->io_residual = len - max; */
			len = max;
		}
		if (len < tgt->block_size)
			len = tgt->block_size;
		nbytes = len;
	}

	cmd = (scsi_cmd_read_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_READ;
	cmd->scsi_cmd_lun_and_lba1 = fixed ? SCSI_CMD_TP_FIXED : 0;
	cmd->scsi_cmd_lba2 	   = len >> 16;
	cmd->scsi_cmd_lba3 	   = len >>  8;
	cmd->scsi_cmd_xfer_len     = len;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_READ;

	scsi_go(tgt, sizeof(*cmd), nbytes, FALSE);

#ifdef	SCSI_STATISTICS
	if (!(ior->io_op & IO_INTERNAL)) {
		tgt->statistics.read.requests++;
		tgt->statistics.read.xfer_count += len;
	}
#endif	SCSI_STATISTICS
#undef	nbytes
}

sctape_write( tgt, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_write_t	*cmd;
	register unsigned	len, max;
	boolean_t		fixed = FALSE;

	len = ior->io_count;
#ifdef PARAGON860       /* performance mod */
        max = tgt->max_dma_data;
#else
	max = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	if (tgt->dev_info.tape.fixed_size) {
		unsigned int bs = tgt->block_size;
		fixed = TRUE;
/* residual is being used in sctape_start for > max_dma_data requests */
		/*ior->io_residual += len % bs; */
		len = len / bs;
	} else {
		if (max > tgt->dev_info.tape.maxreclen)
			max = tgt->dev_info.tape.maxreclen;
		if (len > max) {
/* residual is being used in sctape_start for > max_dma_data requests */
			/*ior->io_residual = len - max; */
			len = max;
		}
		if (len < tgt->block_size)
			len = tgt->block_size;
	}

	cmd = (scsi_cmd_write_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_WRITE;
	cmd->scsi_cmd_lun_and_lba1 = fixed ? SCSI_CMD_TP_FIXED : 0;
	cmd->scsi_cmd_lba2 	   = len >> 16;
	cmd->scsi_cmd_lba3 	   = len >>  8;
	cmd->scsi_cmd_xfer_len     = len;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_WRITE;

	scsi_go(tgt, sizeof(*cmd), 0, FALSE);

#ifdef	SCSI_STATISTICS
	if (!(ior->io_op & IO_INTERNAL)) {
		tgt->statistics.write.requests++;
		tgt->statistics.write.xfer_count += len;
	}
#endif	SCSI_STATISTICS
}

scsi_rewind( tgt, ior, wait)
	register target_info_t	*tgt;
	io_req_t		ior;
	boolean_t		wait;
{
	scsi_cmd_rewind_t	*cmd;
	 io_return_t     ret = SCSI_RET_SUCCESS;

        /* write a filemark if we xtnded/truncated the tape */
/*        if (tgt->flags & TGT_WRITTEN_TO) {
                tgt->ior = ior;
                ior->io_error = 0;
                ret = scsi_write_filemarks(tgt, 2, ior);
                if (ret != SCSI_RET_SUCCESS)
                         printf("%s%d: wfmark failed x%x\n",
                    (*tgt->dev_ops->driver_name)(TRUE), tgt->unit_no, ret);
        	else                                              
			tgt->flags &= ~TGT_WRITTEN_TO;
		tgt->ior = ior;                                 
        	ior->io_op = IO_INTERNAL;                      
        	ior->io_error = 0;                            
        	ior->io_count = 0;                            
        	ior->io_residual = 0;                            
        	ior->io_next = 0;                            
	}
*/


	cmd = (scsi_cmd_rewind_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_REWIND;
	cmd->scsi_cmd_lun_and_lba1 = wait ? 0 : SCSI_CMD_REW_IMMED;
	cmd->scsi_cmd_lba2 	   = 0;
	cmd->scsi_cmd_lba3 	   = 0;
	cmd->scsi_cmd_xfer_len     = 0;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_REWIND;

	scsi_go( tgt, sizeof(*cmd), 0, FALSE);
	return SCSI_RET_SUCCESS;
}

scsi_write_filemarks( tgt, count, ior)
	register target_info_t	*tgt;
	register unsigned int	count;
	io_req_t		ior;
{
	scsi_cmd_write_fil_t	*cmd;

	cmd = (scsi_cmd_write_fil_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_WRITE_FILEMARKS;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = count >> 16;
	cmd->scsi_cmd_lba3 	   = count >>  8;
	cmd->scsi_cmd_xfer_len     = count;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_WRITE_FILEMARKS;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

	return tgt->done;
}

scsi_space( tgt, mode, count, ior)
	register target_info_t	*tgt;
	register int		count;
	io_req_t		ior;
{
	scsi_cmd_space_t	*cmd;

	cmd = (scsi_cmd_space_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_SPACE;
	cmd->scsi_cmd_lun_and_lba1 = mode & 0x3;
	cmd->scsi_cmd_lba2 	   = count >> 16;
	cmd->scsi_cmd_lba3 	   = count >>  8;
	cmd->scsi_cmd_xfer_len     = count;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_SPACE;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

	return tgt->done;
}


scsi_read_block_limits( tgt, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_block_limits_t	*cmd;

	cmd = (scsi_cmd_block_limits_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_READ_BLOCK_LIMITS;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = 0;
	cmd->scsi_cmd_lba3 	   = 0;
	cmd->scsi_cmd_xfer_len     = 0;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_READ_BLOCK_LIMITS;

	scsi_go_and_wait(tgt, sizeof(*cmd), sizeof(scsi_blimits_data_t), ior);
	return tgt->done;
}


scsi_read_position( tgt, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_read_position_t	*cmd;

	cmd = (scsi_cmd_read_position_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_READ_POSITION;
	cmd->scsi_cmd_lun_and_relbit = SCSI_CMD_RP_BT; /* BT = 1 */
	cmd->scsi_cmd_lba1 	   = 0;
	cmd->scsi_cmd_lba2 	   = 0;
	cmd->scsi_cmd_lba3 	   = 0;
	cmd->scsi_cmd_lba4 	   = 0;
        cmd->scsi_cmd_xxx	   = 0;           /* reserved, mbz */
	cmd->scsi_cmd_xfer_len_1   = 0;
	cmd->scsi_cmd_xfer_len_2   = 0;
	cmd->scsi_cmd_ctrl_byte    = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_READ_POSITION;
	scsi_go_and_wait(tgt, sizeof(*cmd), sizeof(scsi_position_data_t), ior);
	return tgt->done;
}

scsi_locate( tgt, block_no, ior)
	register target_info_t	*tgt;
	register unsigned int	block_no;
	io_req_t		ior;
{
	scsi_cmd_locate_t	*cmd;

	cmd = (scsi_cmd_locate_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_LOCATE;
	cmd->scsi_cmd_lun_and_relbit = SCSI_CMD_LOCATE_BT; /* BT = 1 */
	cmd->scsi_cmd_lba1 	   = 0;
	cmd->scsi_cmd_lba2 	   = block_no >> 24;
	cmd->scsi_cmd_lba3 	   = block_no >> 16;
	cmd->scsi_cmd_lba4 	   = block_no >> 8;
        cmd->scsi_cmd_xxx	   = block_no;
	cmd->scsi_cmd_xfer_len_1   = 0;
	cmd->scsi_cmd_xfer_len_2   = 0;
	cmd->scsi_cmd_ctrl_byte    = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_LOCATE;
	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);
	return tgt->done;
}

scsi_load_display( tgt, data, data_len, ior)
	register target_info_t	*tgt;
	scsi_ld_data_t		*data;
	int			data_len;
	io_req_t		ior;
{
	scsi_ld_data_t		*param;
	scsi_cmd_load_display_t	*cmd;

	cmd = (scsi_cmd_load_display_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_LOAD_DISPLAY;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = 0;
	cmd->scsi_cmd_lba3 	   = 0;
	cmd->scsi_cmd_xfer_len     = SCSI_CMD_LD_LEN;  /* must be 17 bytes */
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */

	tgt->cur_cmd = SCSI_CMD_LOAD_DISPLAY;

	param = (scsi_ld_data_t*) (cmd + 1);

	bzero(param->messages, sizeof(param->messages));  
	bcopy(data, (char*)(param), 
		(data_len > SCSI_CMD_LD_LEN) ?  SCSI_CMD_LD_LEN : data_len);
	if(scsi_debug)
		printf("%s %d bytes\n",param->messages+1,
			sizeof(param->messages));

	scsi_go_and_wait(tgt, sizeof(*cmd) + sizeof(*param), 0, ior);
	return tgt->done;
}
#if 0 /* unused */

scsi_track_select( tgt, trackno, ior)
	register target_info_t	*tgt;
	register unsigned char	trackno;
	io_req_t		ior;
{
	scsi_cmd_seek_t	*cmd;

	cmd = (scsi_cmd_seek_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_TRACK_SELECT;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = 0;
	cmd->scsi_cmd_lba3 	   = 0;
	cmd->scsi_cmd_tp_trackno   = trackno;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_TRACK_SELECT;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

}

scsi_read_reverse( tgt, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_rev_read_t	*cmd;
	register unsigned	len;
	unsigned int		max_dma_data;

#ifdef PARAGON860       /* performance mod */
        max_dma_data = tgt->max_dma_data;
#else
	max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	len = ior->io_count;
	if (len > max_dma_data)
		len = max_dma_data;

	cmd = (scsi_cmd_rev_read_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_READ_REVERSE;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = len >> 16;
	cmd->scsi_cmd_lba3 	   = len >>  8;
	cmd->scsi_cmd_xfer_len     = len;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_READ_REVERSE;

	scsi_go(tgt, sizeof(*cmd), len, FALSE);
}

sctape_verify( tgt, len, ior)
	register target_info_t	*tgt;
	register unsigned int	len;
	io_req_t		ior;
{
	scsi_cmd_verify_t	*cmd;

	cmd = (scsi_cmd_verify_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_VERIFY_0;
	cmd->scsi_cmd_lun_and_lba1 = 0;/* XXX */
	cmd->scsi_cmd_lba2 	   = len >> 16;
	cmd->scsi_cmd_lba3 	   = len >>  8;
	cmd->scsi_cmd_xfer_len     = len;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_VERIFY_0;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

}


scsi_recover_buffered_data( tgt, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_recover_buffer_t	*cmd;
	register unsigned		len;
	unsigned int		max_dma_data;

#ifdef PARAGON860       /* performance mod */
        max_dma_data = tgt->max_dma_data;
#else
	max_dma_data = scsi_softc[tgt->masterno]->max_dma_data;
#endif

	len = ior->io_count;
	if (len > max_dma_data)
		len = max_dma_data;

	cmd = (scsi_cmd_recover_buffer_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_RECOVER_BUFFERED_DATA;
	cmd->scsi_cmd_lun_and_lba1 = 0;
	cmd->scsi_cmd_lba2 	   = len >> 16;
	cmd->scsi_cmd_lba3 	   = len >>  8;
	cmd->scsi_cmd_xfer_len     = len;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */
	
	tgt->cur_cmd = SCSI_CMD_RECOVER_BUFFERED_DATA;

	scsi_go(tgt, sizeof(*cmd), len, FALSE);
}

scsi_erase( tgt, mode, ior)
	register target_info_t	*tgt;
	io_req_t		ior;
{
	scsi_cmd_erase_t	*cmd;

	cmd = (scsi_cmd_erase_t*) (tgt->cmd_ptr);
	cmd->scsi_cmd_code = SCSI_CMD_ERASE;
	cmd->scsi_cmd_lun_and_lba1 = mode & SCSI_CMD_ER_LONG;
	cmd->scsi_cmd_lba2 	   = 0;
	cmd->scsi_cmd_lba3 	   = 0;
	cmd->scsi_cmd_xfer_len     = 0;
	cmd->scsi_cmd_ctrl_byte = 0;	/* not linked */


	tgt->cur_cmd = SCSI_CMD_ERASE;

	scsi_go_and_wait(tgt, sizeof(*cmd), 0, ior);

}

#endif

#ifdef	SCSI2
scsi_locate
/* scsi_read_position */
#endif	SCSI2
