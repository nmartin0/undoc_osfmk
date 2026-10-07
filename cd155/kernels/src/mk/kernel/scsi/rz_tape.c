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
 * $Log: rz_tape.c,v $
 * Revision 1.29  1995/03/14  23:48:37  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.28  1995/02/23  17:50:19  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: 12499
 *  Testing: read and set density on an 8500, then read the tape on an 8200
 *  Module(s): sctape_get_status(), sctape_set_status()
 *
 * Revision 1.27  1994/11/18  20:59:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.26  1994/10/28  17:50:42  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: 10964
 *  Testing: reset a 3480 during an MTOFFL ioctl
 *  Module(s): sctape_set_status()
 *
 * Revision 1.25  1994/10/20  22:34:53  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: Low
 *  Benefit or PTS #: 10964
 *  Testing: wrote a program that wrote to tape, then issues an MTOFFL.  during
 * 	  the rewind of the MTOFFL I manually reset the 3480.
 *  Module(s): sctape_set_status()
 *
 * Revision 1.24  1994/08/31  21:25:54  mtm
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
 * Revision 1.21.2.2  1994/08/26  17:15:05  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: 10680
 *  Testing: Tape EATs, Tensors tapewrt/taperead, tar
 *  Module(s): sctape_open
 *
 * Revision 1.21.2.1  1994/08/04  16:13:53  richardg
 *  Reviewer: Jerry Coffman
 *  Risk: Medium
 *  Benefit or PTS #: 10171
 *  Testing: ran Tensor's tapewrt program simultaneously against both LUNs.
 *  Module(s): sctape_strategy, sctape_start
 *
 * Revision 1.21  1994/05/25  23:18:09  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: return D_NO_SPACE at physical end of media on tape
 *  Testing: write past end of tape
 *  Module(s): sctape_start
 *
 * Revision 1.20  1994/05/24  16:00:03  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: merged mtio.h and tape_status.h
 *  Testing:
 *  Module(s): sctape_get_status/set_status.
 *
 * Revision 1.19  1994/04/20  22:19:30  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: added for 3480 Tape Library support - LOAD DISPLAY.
 *  Testing: tape EAT's and costom code to exercise the ioctl
 *  Module(s): sctape_set_status()
 *
 * Revision 1.18  1994/03/08  17:35:38  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: adding 3480 tape library support
 *  Testing: tape EAT's, a short program that uses the MTLOCATE ioctl
 *  Module(s): sctape_set_status. added the case for MTLOCATE.
 *
 * Revision 1.17  1994/03/03  23:13:08  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: adding support for 3480 tape library. this is the ioctl
 * 			for READ POSITION.
 *  Testing: tape EAT's, test program that uses the ioctl.
 *  Module(s): sctape_get_status().
 *
 * Revision 1.16  1994/02/09  21:52:17  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s): This adds the ioctl to enable/disable onboard buffering of
 * the tape device.  It also merges in the the bug fix for #7986 that was
 * mandatory in R1.2
 *
 * Revision 1.15  1993/12/21  19:12:05  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: Low
 *  Benefit or PTS #: 7253
 *  Testing: tested with both models of DAT tape on a remote i/o node with and
 * 	  without a disk attached to the i/o node.
 *  Module(s): scsi/rz_tape.c
 *
 * Revision 1.14  1993/10/13  17:31:05  richardg
 * PTS bug# 6428.  In the open routine, exit after printing an error message once
 * if the condition is MEDIA NOT PRESENT.
 *
 * Revision 1.13  1993/09/24  17:23:48  jerrie
 * Moved scsi_medium_removal to the end of the sctape_close routine.
 * Added tape data compression support.
 *
 * Revision 1.12  1993/09/16  18:12:00  richardg
 * modified the memory allocation to evenly divide available MIO memory between existing scsi devices
 *
 * Revision 1.11  1993/08/04  17:44:36  richardg
 * PTS #5746.  tape accesses in a multinode service partition were hanging because
 * io_count was non-zero during non r/w operations in sctape_open.
 * I now explicitly initialize io_count and io_residual to zero for all non
 * r/w ops.
 *
 * Revision 1.10  1993/06/30  22:53:46  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.9  1993/06/28  18:53:43  jerrie
 * Clear ior->io_count before doing a mode sense, otherwise sctape_start
 * may find a io_count non-zero and start looping for a large read/write.
 *
 * Revision 1.8  1993/06/22  22:17:55  richardg
 * the io_count could be erroneously adjusted when NOT doing a read or write.
 * This would cause a tape drive to hang.
 *
 * Revision 1.7  1993/06/08  22:26:20  jerrie
 * MTIOCGET flavor now returns MT_ISSCSI instead of 7.  Added scsi commands
 * moved from rz_open to sctape_open.  Added scsi commands moved from rz_close
 * to sctape_close.  Initialize tgt->lun in open, close, get/set status routines
 * (tgt->lun was already set for read/write).
 *
 * Revision 1.6  1993/05/12  18:51:54  jerrie
 * Fix for bug 5099.
 * Initialize the tgt->lun field before starting a SCSI command.  This also
 * adds multiple LUN support for tape devices by using the tzlun() macro.
 *
 * Revision 1.5  1993/05/12  18:19:46  richardg
 * PTS #3135.  this code allows the driver to handle r/w requests that are greater than the max dma of the NCR SCSI chip (64k).  The driver loops internally to satisfy the request when >max_dma. The code is restricted to sctape_start().
 *
 * Revision 1.1  1993/02/22  19:48:14  richardg
 * Initial revision
 *
 * Revision 2.10.2.2  92/04/30  12:00:46  bernadat
 * 	Changes from TRUNK:
 * 		Use scsi_print_sense_data where applicable.
 * 		[92/04/06            af]
 * 
 * Revision 2.10.2.1  92/03/28  10:15:44  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:32:28  jeffreyh]
 * 
 * Revision 2.11  92/02/23  22:44:37  elf
 * 	Changed the interface of a number of functions not to
 * 	require the scsi_softc pointer any longer.  It was
 * 	mostly unused, now it can be found via tgt->masterno.
 * 	[92/02/22  19:30:10  af]
 * 
 * Revision 2.10  91/10/09  16:17:19  af
 * 	 Revision 2.9.1.2  91/09/01  17:16:50  af
 * 	 	Zero io_error before using the ior.
 * 	 Revision 2.9.1.1  91/08/29  18:08:50  af
 * 	 	Fixed to compile again under 2.5.
 * 
 * Revision 2.9.1.2  91/09/01  17:16:50  af
 * 	Zero io_error before using the ior.
 * 
 * Revision 2.9.1.1  91/08/29  18:08:50  af
 * 	Fixed to compile again under 2.5.
 * 
 * Revision 2.9  91/08/24  12:28:15  af
 * 	Flag tapes as exclusive open, Spl defs, now we
 * 	understand fixed-size tapes (QIC-11), multiP locks.
 * 	[91/08/02  03:56:41  af]
 * 
 *	Cast args for bcopy.
 * 
 * Revision 2.8  91/06/25  20:56:33  rpd
 * 	Tweaks to make gcc happy.
 * 
 * Revision 2.7  91/06/19  11:57:12  rvb
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.6  91/05/14  17:27:00  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/05/13  06:34:29  af
 * 	Do not say we failed to bspfile when the target really was
 * 	only busy.
 * 	Retrieve from mode_sense speed, density and writeprotect info.
 * 	Deal with targets that are busy, note when we get confused due
 * 	to a scsi bus reset.  Set speed and density if user asks to.
 * 	Generally, made it work properly [it passes Rudy's tests].
 * 	Tapes really work now.
 * 
 * Revision 2.4.2.1  91/05/12  16:05:10  af
 * 	Do not say we failed to bspfile when the target really was
 * 	only busy.
 * 
 * 	Retrieve from mode_sense speed, density and writeprotect info.
 * 	Deal with targets that are busy, note when we get confused due
 * 	to a scsi bus reset.  Set speed and density if user asks to.
 * 	Generally, made it work properly [it passes Rudy's tests].
 * 
 * 	Tapes really work now.
 * 
 * Revision 2.4.1.3  91/04/05  13:10:07  af
 * 	Do not say we failed to bspfile when the target really was
 * 	only busy.
 * 
 * Revision 2.4.1.2  91/03/29  17:10:38  af
 * 	Retrieve from mode_sense speed, density and writeprotect info.
 * 	Deal with targets that are busy, note when we get confused due
 * 	to a scsi bus reset.  Set speed and density if user asks to.
 * 	Generally, made it work properly [it passes Rudy's tests].
 * 
 * Revision 2.4.1.1  91/02/21  19:00:22  af
 * 	Tapes really work now.
 * 
 * 
 * Revision 2.4  91/02/05  17:44:01  mrt
 * 	Added author notices
 * 	[91/02/04  11:17:11  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:15:49  mrt]
 * 
 * Revision 2.3  90/12/05  23:34:04  af
 * 	Mild attempt to get it working.  Needs lots of work still.
 * 	[90/12/03  23:34:27  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:44:11  af
 * 	Created.
 * 	[90/10/21            af]
 */
/*
 *	File: rz_tape.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Top layer of the SCSI driver: interface with the MI.
 *	This file contains operations specific to TAPE-like devices.
 */

#include <mach/std_types.h>
#include <machine/machparam.h>		/* spl definitions */
#include <scsi/compat_30.h>
#include <machine/machparam.h>

#include <sys/ioctl.h>
#ifdef	MACH_KERNEL
#include <device/tape_status.h>
#else	/*MACH_KERNEL*/
#include <mips/PMAX/tape_status.h>
#endif	/*MACH_KERNEL*/

#include <scsi/scsi.h>
#include <scsi/scsi2.h>
#include <scsi/scsi_defs.h>
#include <scsi/rz.h>

int scsi_tape_timeout = 5*60;	/* secs, tk50 is slow when positioning far apart */

sctape_open(tgt, req)
	target_info_t	*tgt;
	io_req_t	req;
{
	extern int	rz_open_timeout;
	scsi_ret_t	ret;
	io_req_t	ior;
	int		i;
	scsi_mode_sense_data_t *mod;
	boolean_t		buffer_mode = FALSE;

#ifdef	MACH_KERNEL
	req->io_device->flag |= D_EXCL_OPEN;
#endif	/*MACH_KERNEL*/

	/* Preferably allow tapes to disconnect */
	if (BGET(scsi_might_disconnect,tgt->masterno,tgt->target_id))
		BSET(scsi_should_disconnect,tgt->masterno,tgt->target_id);

	/*
	 * Dummy ior for proper sync purposes
	 */
	io_req_alloc(ior,0);
        bzero(ior, sizeof(struct io_req));
	ior->io_unit = req->io_unit;
	ior->io_next = 0;
	ior->io_count = 0;
	ior->io_residual = 0;

	/*
	 * Bring the unit online, retrying if necessary.
	 * If the target is spinning up we wait for it.
	 */
	if ( ! (tgt->flags & TGT_ONLINE)) {

		for (i = 0; i < rz_open_timeout; i++) {

			ior->io_op = IO_INTERNAL;
			ior->io_error = 0;
			ret = scsi_test_unit_ready(tgt, ior);

			if (ret == SCSI_RET_SUCCESS)
				break;

			if (ret == SCSI_RET_DEVICE_DOWN) {
				i = rz_open_timeout;
				break;
			}

			if (ret == SCSI_RET_NEED_SENSE) {

				ior->io_op = IO_INTERNAL;
				ior->io_count = 0;
				ior->io_residual = 0;
				tgt->ior = ior;
				scsi_request_sense(tgt, ior, 0);
				iowait(ior);

			}
			if (ret == SCSI_RET_RETRY) {
		{
		/* check for presence of media */
		scsi_sense_data_t *sns = (scsi_sense_data_t *)tgt->cmd_ptr;
		  if ((sns->error_class == SCSI_SNS_XTENDED_SENSE_DATA) &&
		    (sns->u.xtended.sense_key == SCSI_SNS_NOTREADY)   &&
		    (sns->u.xtended.add_len >= 6)		      &&
		    (sns->u.xtended.add_bytes[4] == 0x3a)  /* ASC  */ &&
		    (sns->u.xtended.add_bytes[5] == 0x00)) /* ASCQ */ {
		    if (scsi_debug) {
			printf("%s%d: %s\n",
				(*tgt->dev_ops->driver_name)(TRUE),
				tgt->unit_no, "Media not present");
		    }
			i = rz_open_timeout;
			break;
		  }
		}
			}

			if (i == 5) printf("%s%d: %s\n", 
					   (*tgt->dev_ops->driver_name)(TRUE),
					   tgt->unit_no,
					   "Waiting to come online..");
			timeout(wakeup, tgt, hz);
			await(tgt);
		}

		/* lock on removable media */
		if ((i != rz_open_timeout) && (tgt->flags & TGT_REMOVABLE_MEDIA)) {
			ior->io_op = IO_INTERNAL;
			ior->io_count = 0;
			ior->io_residual = 0;
			/* too many dont support it. Sigh */
			tgt->flags |= TGT_OPTIONAL_CMD;
			(void) scsi_medium_removal( tgt, FALSE, ior);
			tgt->flags &= ~TGT_OPTIONAL_CMD;
		}

		if (i == rz_open_timeout) {
			io_req_free(ior);
			return D_DEVICE_DOWN;
		}
	}

	ior->io_count = 0;

	/* Exabyte wants a mode sense first */
	do {
		ior->io_op = IO_INTERNAL;
		ior->io_next = 0;
		ior->io_error = 0;
		ior->io_count = 0;
		ior->io_residual = 0;
		ret = scsi_mode_sense(tgt, 0, 32, ior);
	} while (ret == SCSI_RET_RETRY);

	mod = (scsi_mode_sense_data_t *)tgt->cmd_ptr;
	if (scsi_debug) {
		int	p[5];
		bcopy((char*)mod, (char*)p, sizeof(p));
		printf("[modsns(%x): x%x x%x x%x x%x x%x]", ret,
			p[0], p[1], p[2], p[3], p[4]);
	}
	if (ret == SCSI_RET_DEVICE_DOWN)
		goto out;
	if (ret == SCSI_RET_SUCCESS) {
		tape_spec_t *tape_spec = (tape_spec_t *)&mod->device_spec;
		tgt->dev_info.tape.read_only = tape_spec->wp;
		if(!tape_spec->buffer_mode) {
			tgt->dev_info.tape.buffer_mode = 1;
			buffer_mode = TRUE;
		}
		else
		tgt->dev_info.tape.buffer_mode = tape_spec->buffer_mode;

		tgt->dev_info.tape.speed = tape_spec->speed;
		tgt->dev_info.tape.density = mod->bdesc[0].density_code;
	}

	/* Some tapes have limits on record-length */
again:
	ior->io_op = IO_INTERNAL;
	ior->io_next = 0;
	ior->io_error = 0;
	ior->io_count = 0;
	ior->io_residual = 0;
	ret = scsi_read_block_limits( tgt, ior);
	if (ret == SCSI_RET_RETRY) goto again;
	if (!ior->io_error && (ret == SCSI_RET_SUCCESS)) {
		scsi_blimits_data_t	*lim;
		int			maxl;

		lim = (scsi_blimits_data_t *) tgt->cmd_ptr;

		tgt->block_size = (lim->minlen_msb << 8) |
				   lim->minlen_lsb;

		maxl =	(lim->maxlen_msb << 16) |
			(lim->maxlen_sb  <<  8) |
			 lim->maxlen_lsb;
		if (maxl == 0)
			maxl = (unsigned)-1;
		tgt->dev_info.tape.maxreclen = maxl;
		tgt->dev_info.tape.fixed_size = (maxl == tgt->block_size);
	} else {
		/* let the user worry about it */
		tgt->dev_info.tape.maxreclen = (unsigned)-1;
		tgt->dev_info.tape.fixed_size = FALSE;
	}

	/* Try hard to do a mode select */
	for (i = 0; i < 5; i++) {
		ior->io_op = IO_INTERNAL;
		ior->io_error = 0;
		ior->io_count = 0;
		ior->io_residual = 0;
#ifdef	PARAGON860	/* Tape Compression Support */
		ret = sctape_mode_select(tgt, 0, 0, TRUE, buffer_mode, ior);
#else	PARAGON860	/* Tape Compression Support */
		ret = sctape_mode_select(tgt, 0, 0, buffer_mode, ior);
#endif	PARAGON860	/* Tape Compression Support */
		if (ret == SCSI_RET_SUCCESS)
			break;
	}
	if (scsi_watchdog_period < scsi_tape_timeout)
		scsi_watchdog_period += scsi_tape_timeout;

#if 0	/* this might imply rewind, which we do not want, although yes, .. */
	/* we want the tape loaded */
	ior->io_op = IO_INTERNAL;
	ior->io_next = 0;
	ior->io_error = 0;
	ret = scsi_start_unit(tgt, SCSI_CMD_SS_START, ior);
#endif

out:
	io_req_free(ior);
	return ret;
}

sctape_close(dev, tgt)
	int		dev;
	target_info_t	*tgt;
{
	io_return_t	ret = SCSI_RET_SUCCESS;
	io_req_t	ior;

	/*
	 * Dummy ior for proper sync purposes
	 */
	io_req_alloc(ior,0);
        bzero(ior, sizeof(struct io_req));
	ior->io_op = IO_INTERNAL;
	ior->io_unit = dev;
	ior->io_error = 0;
	ior->io_next = 0;
	ior->io_count = 0;
	ior->io_residual = 0;

	if (tgt->ior) printf("TAPE: Close with pending requests ?? \n");

	if ( ! (tgt->flags & TGT_ONLINE)) { /* the tape is already offline */
		io_req_free(ior);
		tgt->flags &= ~(TGT_WRITTEN_TO|TGT_REWIND_ON_CLOSE);
        	return ret;
	}
	/* write a filemark if we xtnded/truncated the tape */
/*	if (tgt->flags & TGT_WRITTEN_TO) {
		tgt->ior = ior;
		ior->io_error = 0;
		ret = scsi_write_filemarks(tgt, 2, ior);
		if (ret != SCSI_RET_SUCCESS)
			 printf("%s%d: wfmark failed x%x\n",
			 (*tgt->dev_ops->driver_name)(TRUE), tgt->unit_no, ret);
*/
		/*
		 * Don't bother repositioning if we'll rewind it
		 */
/*
		if (tgt->flags & TGT_REWIND_ON_CLOSE)
			goto rew;
retry:
		tgt->ior = ior;
		ior->io_op = IO_INTERNAL;
		ior->io_error = 0;
		ior->io_next = 0;
		ret = scsi_space(tgt, SCSI_CMD_SP_FIL, -1, ior);
		if (ret != SCSI_RET_SUCCESS) {
			if (ret == SCSI_RET_RETRY) {
				timeout(wakeup, tgt, hz);
				await(tgt);
				goto retry;
			}
			printf("%s%d: bspfile failed x%x\n",
			 (*tgt->dev_ops->driver_name)(TRUE), tgt->unit_no, ret);
		}
	}
*/
rew:
	if (tgt->flags & TGT_REWIND_ON_CLOSE) {
		/* Rewind tape */
		ior->io_op = IO_INTERNAL;
		ior->io_error = 0;
		ior->io_count = 0;
		tgt->ior = ior;
		(void) scsi_rewind(tgt, ior, TRUE);
		iowait(ior);
		if (tgt->done == SCSI_RET_RETRY) {
			timeout(wakeup, tgt, 5*hz);
			await(tgt);
			goto rew;
		}
	}

	if (tgt->flags & TGT_REMOVABLE_MEDIA) {
		ior->io_op = IO_INTERNAL;
		ior->io_error = 0;
		ior->io_count = 0;
		/* too many dont support it. Sigh */
		tgt->flags |= TGT_OPTIONAL_CMD;
		(void) scsi_medium_removal( tgt, TRUE, ior);
		tgt->flags &= ~TGT_OPTIONAL_CMD;
	}

	io_req_free(ior);

	tgt->flags &= ~(TGT_ONLINE|TGT_WRITTEN_TO|TGT_REWIND_ON_CLOSE);
	return ret;
}

sctape_strategy(ior)
	register io_req_t	ior;
{
	target_info_t  *tgt;
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

	tgt = scsi_softc[rzcontroller(i)]->target[rzslave(i)][rzlun(i)];

	if (((ior->io_op & IO_READ) == 0) &&
	    tgt->dev_info.tape.read_only) {
		ior->io_error = D_IO_ERROR; /* rag changed from D_INVALID_OP*/
		ior->io_op |= IO_ERROR;
		ior->io_residual = ior->io_count;
		iodone(ior);
		return ior->io_error;
	}

	ior->io_next = 0;
	ior->io_prev = 0;

	if(scsi_debug)
		printf("sctape_strategy: ior->io_count = %d\n", ior->io_count);
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
		sctape_start( tgt, FALSE);
	}
	splx(i);

	return D_SUCCESS;
}

sctape_start( tgt, done)
	target_info_t	*tgt;
	boolean_t	done;
{
	io_req_t		head, ior = tgt->ior;
	scsi_ret_t		ret;
	int			peot = 0;

	if (ior == 0)
		return;

	if (done) {
#ifdef PARAGON860 /* large read/write fix */
                register unsigned int   xferred;
                unsigned int            max_dma_data;

	/* performance mod change */
                 max_dma_data = tgt->max_dma_data; 
                /* max_dma_data = scsi_softc[tgt->masterno]->max_dma_data; */
#endif

		/* see if we must retry */
		if ((tgt->done == SCSI_RET_RETRY) &&
		    ((ior->io_op & IO_INTERNAL) == 0)) {
#ifdef	SCSI_STATISTICS
			tgt->statistics.retries++;
#endif	SCSI_STATISTICS
			delay(1000000);/*XXX*/
			goto start;
		} else
		/* got a bus reset ? ouch, that hurts */
		if (tgt->done == (SCSI_RET_ABORTED|SCSI_RET_RETRY)) {
			/*
			 * we really cannot retry because the tape position
			 * is lost.
			 */
			printf("Lost tape position\n");
			ior->io_error = D_IO_ERROR;
			ior->io_op |= IO_ERROR;
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
			if (sns->u.xtended.sense_key == SCSI_SNS_ILLEGAL_REQ &&
			    sns->u.xtended.add_bytes[4] == SCSI_ERR_NO_LUN) {
				printf("Logical unit not supported!\n");
				ior->io_error = D_NO_SUCH_DEVICE; 
				ior->io_op &= ~IO_ERROR;
			}
			    else if (sns->u.xtended.eom) {
				int residue;
				ior->io_error = D_NO_SPACE; 
				ior->io_op &= ~IO_ERROR;
				residue = sns->u.xtended.info0 << 24 |
					  sns->u.xtended.info1 << 16 |
					  sns->u.xtended.info2 <<  8 |
					  sns->u.xtended.info3;
				residue *= tgt->block_size;
#ifdef PARAGON860 /* large read/write fix */
				    if(ior->io_count > max_dma_data)
				      residue += ior->io_count - max_dma_data;
#endif
			/* only adjust if we're doing a read or write */
				if(ior->io_count) { 
					ior->io_count += ior->io_residual;
					ior->io_residual = residue;
				}
				if (scsi_debug) {
					printf("End of Physical Tape!\n");
					printf("io_error = %d\n",ior->io_error);
				}
			    } else if (sns->u.xtended.fm) {
				int residue;
				ior->io_error = 0;
				ior->io_op &= ~IO_ERROR;
				residue = sns->u.xtended.info0 << 24 |
					  sns->u.xtended.info1 << 16 |
					  sns->u.xtended.info2 <<  8 |
					  sns->u.xtended.info3;
				residue *= tgt->block_size;
#ifdef PARAGON860 /* large read/write fix */
				    if(ior->io_count > max_dma_data ) 
				      residue += ior->io_count - max_dma_data;
#endif
				ior->io_count += ior->io_residual;
				ior->io_residual = residue;
				if (scsi_debug)
					printf("File Mark\n");
			    } else if (sns->u.xtended.ili) {
				if (ior->io_op & IO_READ) {
				    int residue;
				    ior->io_error = 0;
				    ior->io_op &= ~IO_ERROR;
				    residue =	sns->u.xtended.info0 << 24 |
						sns->u.xtended.info1 << 16 |
						sns->u.xtended.info2 <<  8 |
						sns->u.xtended.info3;
				    residue *= tgt->block_size; 
#ifdef PARAGON860 /* large read/write fix */
				    if (ior->io_count > max_dma_data)
					residue += ior->io_count - max_dma_data;
#endif
				     	/* NOTE: residue == requested - actual
				     	 * We only care if > 0
				     	 */
				    	if (residue < 0) residue = 0; /*sanity*/
					ior->io_count  += ior->io_residual;
					ior->io_data   -= ior->io_residual;
					ior->io_recnum -=
					    ior->io_residual/tgt->block_size; 
				    	ior->io_residual = residue;
				    	if (scsi_debug) {
						printf("Tape Short Read (%d)\t", 							residue);
						printf(
						    "count=%d, residual=%d\n",
							ior->io_count,
							ior->io_residual);
					}
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
			if (scsi_debug) {
				printf("[NeedSns x%x x%x]",
					ior->io_residual, ior->io_count);
			}
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
#ifdef PARAGON860 /* large read/write fix */
                /*
                 * No errors.
                 * See if we requested more than the max
                 * (We use io_residual in a flip-side way here)
                 */
                else if (ior->io_count > (xferred = max_dma_data)) {
			if (ior->io_op & IO_SGLIST) {
				ior->io_residual  = ior->io_count - xferred;
			} else {
				ior->io_residual += xferred;
				ior->io_count	 -= xferred;
				ior->io_data	 += xferred;
				ior->io_recnum	 += xferred / tgt->block_size;
				if (scsi_debug) {
				    printf("[sctape_start:resid %d]",
					ior->io_residual);
				    printf(" [io_count %d]\n", ior->io_count);
				}
				goto start;
			}
                }
                else if (xferred = ior->io_residual) {
                        ior->io_data	-= xferred;
                        ior->io_count	+= xferred;
                        ior->io_recnum	-= xferred / tgt->block_size;
                        ior->io_residual = 0;
                } /* that's it */

#endif

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
			SCSI_DRIVER_CHECKOUT(tgt, head);
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
		ret = sctape_read( tgt, ior );
	} else if ((ior->io_op & IO_INTERNAL) == 0) {
		tgt->flags |= TGT_WRITTEN_TO;
		ret = sctape_write( tgt, ior );
	}
}

io_return_t
sctape_get_status( dev, tgt, flavor, status, status_count)
	int		dev;
	target_info_t	*tgt;
	int		flavor;
	dev_status_t	status;
	unsigned int	*status_count;
{
	switch (flavor) {

#ifdef	MACH_KERNEL
	case DEV_GET_SIZE:
		status[DEV_GET_SIZE_DEVICE_SIZE] = 0;
		status[DEV_GET_SIZE_RECORD_SIZE] = tgt->block_size;
		*status_count = DEV_GET_SIZE_COUNT;
		break;
#endif	MACH_KERNEL

	case TAPE_STATUS: {
		struct tape_status *ts = (struct tape_status *) status;

		ts->mt_type		= MT_ISSCSI;
		ts->speed		= tgt->dev_info.tape.speed;
		ts->density		= tgt->dev_info.tape.density;
		ts->flags		= (tgt->flags & TGT_REWIND_ON_CLOSE) ?
						TAPE_FLG_REWIND : 0;

#ifdef	PARAGON860	/* Tape Compression Support */
		ts->flags		|= (tgt->flags & TGT_COMPRESSION) ?
						TAPE_FLG_WRITE_COMPRESSION : 0;
#endif	PARAGON860	/* Tape Compression Support */

		if (tgt->dev_info.tape.read_only)
			ts->flags |= TAPE_FLG_WP;
#ifdef	MACH_KERNEL
		*status_count = TAPE_STATUS_COUNT;
#endif	MACH_KERNEL

		break;
	    }
	/* U*x compat */
	case MTIOCGET: {
		struct mtget *g = (struct mtget *) status;
	        scsi_ret_t              ret;

		bzero(g, sizeof(struct mtget));
#ifdef	PARAGON860
		g->mt_type = MT_ISSCSI;

#else	PARAGON860
		g->mt_type = 0x7;	/* Ultrix compat */
#endif	PARAGON860
#ifdef	MACH_KERNEL
		*status_count = sizeof(struct mtget)/sizeof(int);
#endif	MACH_KERNEL
		break;
	    }
	case MTIODGET: {
		struct mtget *g = (struct mtget *) status;
	        scsi_ret_t              ret;

		bzero(g, sizeof(struct mtget));
#ifdef	PARAGON860
		g->mt_type = MT_ISSCSI;
		g->mt_dsreg = tgt->dev_info.tape.density;

#else	PARAGON860
		g->mt_type = 0x7;	/* Ultrix compat */
#endif	PARAGON860
#ifdef	MACH_KERNEL
		*status_count = sizeof(struct mtget)/sizeof(int);
#endif	MACH_KERNEL
		break;
	    }
	case MTIOPOS: { /* SCSI read position */
		struct position *g = (struct position *) status;
		io_req_t	ior;
	        scsi_ret_t              ret;

		bzero(g, sizeof(struct position));

		io_req_alloc(ior,0);
		bzero(ior, sizeof(struct io_req));
try_again:
		ior->io_count = 0;
		ior->io_residual = 0;
		ior->io_op = IO_INTERNAL;
		ior->io_unit = dev;
		ior->io_error = 0;
		ior->io_next = 0;

		ret = scsi_read_position(tgt,ior);
		if (ret == SCSI_RET_RETRY) goto try_again;
       		if (!ior->io_error && (ret == SCSI_RET_SUCCESS)) {
       	        	scsi_position_data_t    *position;

       	         	position = (scsi_position_data_t *) tgt->cmd_ptr;
                	g->first_block = position->firstblk_msb << 24 |
                                         position->firstblk_sb1 << 16 |
                                         position->firstblk_sb2 << 8  |
                                          position->firstblk_lsb;

			if(scsi_debug)
				printf("read position = %0X\n",g->first_block);
		}
		else {
			io_req_free(ior);
			if (scsi_debug) {
				printf("[sctape_gstatus: %s]\n",
				  "scsi_read_position failed");
			}
			return D_IO_ERROR;
		}
		io_req_free(ior);
		break;
	    }
	default:
		return D_INVALID_OPERATION;
	}
	return D_SUCCESS;
}

io_return_t
sctape_set_status( dev, tgt, flavor, status, status_count)
	int		dev;
	target_info_t	*tgt;
	int		flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
	int			i;
	scsi_ret_t		ret;
#ifdef	PARAGON860	/* Tape Compression Support */
	io_return_t		io_ret;
#endif	PARAGON860	/* Tape Compression Support */

	switch (flavor) {
	case TAPE_STATUS: {
		struct tape_status *ts = (struct tape_status *) status;
		if (ts->flags & TAPE_FLG_REWIND)
			tgt->flags |= TGT_REWIND_ON_CLOSE;
		else
			tgt->flags &= ~TGT_REWIND_ON_CLOSE;

#ifdef	PARAGON860	/* Tape Compression Support */
		io_ret = sctape_compression(tgt,dev,
		    (ts->flags & TAPE_FLG_WRITE_COMPRESSION) ? TRUE : FALSE);

		if (io_ret != D_SUCCESS) return io_ret;
#endif	PARAGON860	/* Tape Compression Support */

		if (ts->speed || ts->density) {
			unsigned int ospeed, odensity;
			io_req_t	ior;

			io_req_alloc(ior,0);
			bzero(ior, sizeof(struct io_req));
			ior->io_op = IO_INTERNAL;
			ior->io_unit = dev;
			ior->io_error = 0;
			ior->io_next = 0;
			ior->io_count = 0;
			ior->io_residual = 0;

			ospeed = tgt->dev_info.tape.speed;
			odensity = tgt->dev_info.tape.density;
			tgt->dev_info.tape.speed = ts->speed;
			tgt->dev_info.tape.density = ts->density;

#ifdef	PARAGON860	/* Tape Compression Support */
			ret = sctape_mode_select(tgt, 0, 0, TRUE,
				(ospeed == ts->speed), ior);
#else	PARAGON860	/* Tape Compression Support */
			ret = sctape_mode_select(tgt, 0, 0, (ospeed == ts->speed), ior);
#endif	PARAGON860	/* Tape Compression Support */
			if (ret != SCSI_RET_SUCCESS) {
				tgt->dev_info.tape.speed = ospeed;
				tgt->dev_info.tape.density = odensity;
			}

			io_req_free(ior);
		}

		break;
	    }
	/* U*x compat */
	case MTIOCTOP: {
		struct mtop *mt = (struct mtop *) status;
		io_req_t	ior;

		if (scsi_debug)
			printf("[sctape_sstatus: %x %x %x]\n",
				flavor, mt->mt_op, mt->mt_count);

		io_req_alloc(ior,0);
		bzero(ior, sizeof(struct io_req));
retry:
		ior->io_count = 0;
		ior->io_residual = 0;
		ior->io_op = IO_INTERNAL;
		ior->io_unit = dev;
		ior->io_error = 0;
		ior->io_next = 0;
		tgt->ior = ior;

		/* compat: in U*x it is a short */
		switch ((short)(mt->mt_op)) {
		case MTWEOF:	/* write an end-of-file record */
			ret = scsi_write_filemarks(tgt, mt->mt_count, ior);
			break;
		case MTFSF:	/* forward space file */
			ret = scsi_space(tgt, SCSI_CMD_SP_FIL, mt->mt_count, ior);
			break;
		case MTBSF:	/* backward space file */
			ret = scsi_space(tgt, SCSI_CMD_SP_FIL, -mt->mt_count,ior);
			break;
		case MTFSR:	/* forward space record */
			ret = scsi_space(tgt, SCSI_CMD_SP_BLOCKS, mt->mt_count, ior);
			break;
		case MTBSR:	/* backward space record */
			ret = scsi_space(tgt, SCSI_CMD_SP_BLOCKS, -mt->mt_count, ior);
			break;
		case MTLOCATE:	/* position tape to  mt->mt_repeate_count */
			ret = scsi_locate(tgt, mt->mt_count, ior);
			break;
		case MTREW:	/* rewind */
		case MTOFFL:	/* rewind and put the drive offline */
			ior->io_error = 0;
			ret = scsi_rewind(tgt, ior, TRUE);
			iowait(ior);
			if (ior->io_error)
				return D_IO_ERROR;
			if ((short)(mt->mt_op) == MTREW) break;

			/* unlock removable media - tape should be removable, but check anyway */
			if (tgt->flags & TGT_REMOVABLE_MEDIA) {
				ior->io_op = IO_INTERNAL;
				ior->io_next = 0;
				ior->io_error = 0;
				/* too many dont support it. Sigh */
				tgt->flags |= TGT_OPTIONAL_CMD;
				(void) scsi_medium_removal( tgt, TRUE, ior);
				tgt->flags &= ~TGT_OPTIONAL_CMD;
			}
                        if (ior->io_error)
                                return D_IO_ERROR;

			ior->io_op = IO_INTERNAL;
			ior->io_next = 0;
			ior->io_error = 0;
			ret = scsi_start_unit(tgt, 0, ior);
			if (ret != SCSI_RET_SUCCESS) {
				for (i = 0; i < 5; i++) {
					delay(15000);
					ior->io_op = IO_INTERNAL;
       					ior->io_next = 0;
       					ior->io_error = 0;
       					ret = scsi_start_unit(tgt, 0, ior);
					if (ret == SCSI_RET_SUCCESS)
						break;
       				}
			}
                        if (ret != SCSI_RET_SUCCESS) {
                                io_req_free(ior);
                                if (scsi_debug) {
                                        printf("[sctape_set_status: %s]\n",
                                          "mode select error - MTOFFL ioctl");
                                }
                                return D_IO_ERROR;
			}

			/* mark drive as offline */
			tgt->flags &= ~(TGT_ONLINE|TGT_REWIND_ON_CLOSE);
			break;
		case MTNOP:	/* no operation, sets status only */
			ret = SCSI_RET_SUCCESS;
			break;
		case MTCACHE:	/* enable controller cache */
			if(tgt->dev_info.tape.buffer_mode == 1){
				tgt->ior = 0;
				ret = SCSI_RET_SUCCESS;
			}
			else {
				tgt->dev_info.tape.buffer_mode = 1;
				ior->io_op = IO_INTERNAL;
				ior->io_next = 0;
				ior->io_error = 0;
				ret = sctape_mode_select(tgt,0,0,0,TRUE, ior);
			}
			if (ret != SCSI_RET_SUCCESS) {
				io_req_free(ior);
				if (scsi_debug) {
					printf("[sctape_sstatus: %s]\n",
					  "mode select error - buffer mode");
				}
				return D_IO_ERROR;
			}
			break;
		case MTNOCACHE:	/* disable controller cache */
			if(tgt->dev_info.tape.buffer_mode == 0){
				tgt->ior = 0;
				ret = SCSI_RET_SUCCESS;
			}
			else {
				tgt->dev_info.tape.buffer_mode = 0;
				ior->io_op = IO_INTERNAL;
				ior->io_next = 0;
				ior->io_error = 0;
				ret = sctape_mode_select(tgt,0,0,0,TRUE, ior);
			}
			if (ret != SCSI_RET_SUCCESS) {
				io_req_free(ior);
				if (scsi_debug) {
					printf("[sctape_sstatus: %s]\n",
					  "mode select error - buffer mode");
				}
			return D_IO_ERROR;
			}
			break;
		case MTDENSITY:	/* set tape density */
			if(tgt->dev_info.tape.density == mt->mt_count){
				tgt->ior = 0;
				ret = SCSI_RET_SUCCESS;
			}
			else {
				tgt->dev_info.tape.density = mt->mt_count;
				ior->io_op = IO_INTERNAL;
				ior->io_next = 0;
				ior->io_error = 0;
				ret = sctape_mode_select(tgt,0,0,0,TRUE, ior);
			}
			if (ret != SCSI_RET_SUCCESS) {
				io_req_free(ior);
				if (scsi_debug) {
					printf("[sctape_sstatus: %s]\n",
					  "mode select error - density select");
				}
			return D_IO_ERROR;
			}
			break;
		default:
			tgt->ior = 0;
			io_req_free(ior);
			return D_INVALID_OPERATION;
		}

		if (ret == SCSI_RET_RETRY) {
			timeout(wakeup, ior, 5*hz);
			await(ior);
			goto retry;
		}
		if (ret != SCSI_RET_SUCCESS || /* rag */
		  (ior->io_error && (short)(mt->mt_op) != MTREW)) { 
			if (scsi_debug) {
			  printf("sctape_set_status returning D_IO_ERROR\n");
			  printf(
			    "ret=%d, SCSI_RET_SUCCESS=%d, ior->io_error=%d\n",
				ret, SCSI_RET_SUCCESS, ior->io_error);
			}
			io_req_free(ior);
			return D_IO_ERROR;
		}
		io_req_free(ior); 
		break;
	}
	case MTIODISPLY: { /* SCSI load display (3480 ONLY) */
		scsi_ld_data_t *data = (scsi_ld_data_t *) status;
		io_req_t	ior;
	        scsi_ret_t      ret;


		io_req_alloc(ior,0);
		bzero(ior, sizeof(struct io_req));
doit_again:
		ior->io_count = 0;
		ior->io_residual = 0;
		ior->io_op = IO_INTERNAL;
		ior->io_unit = dev;
		ior->io_error = 0;
		ior->io_next = 0;

		ret = scsi_load_display(tgt, data, sizeof(*data), ior);
		if (ret == SCSI_RET_RETRY) goto doit_again;
       		if (ior->io_error || (ret != SCSI_RET_SUCCESS)) {
			io_req_free(ior);
			if (scsi_debug) {
				printf("[sctape_sstatus: %s]\n",
				  "scsi_load_display failed");
			}
			return D_IO_ERROR;
		}
		io_req_free(ior);
		break;
	    }
	case MTIOCIEOT:
	case MTIOCEEOT:
	default:
		return D_INVALID_OPERATION;
	}
	return D_SUCCESS;
}

#ifdef	PARAGON860	/* Tape Compression Support */
io_return_t
sctape_compression(tgt, dev, want_compression)
	target_info_t	*tgt;
	int	dev;
	boolean_t	want_compression;
{
	io_req_t		ior;
	scsi_ret_t		ret;

	/*
	 * Some drives support a data compression characteristics
	 * mode page 15 (0x0f) to control compression while others
	 * use the device configuration mode page 16 (0x10).  We try
	 * the data compression characteristics mode page first.
	 * If that fails, we try the device configuration mode page.
	 */

	/*
	 * Try the data compression characteristics mode page 15 (0x0f)
	 */

	io_req_alloc(ior,0);
        bzero(ior, sizeof(struct io_req));
	ior->io_op = IO_INTERNAL;
	ior->io_unit = dev;
	ior->io_error = 0;
	ior->io_next = 0;
	ior->io_count = 0;
	ior->io_residual = 0;

	/*
	 * read the current values, too many don't support it. Sigh
	 */
	tgt->flags |= TGT_OPTIONAL_CMD;
	ret = scsi_mode_sense(tgt, 15,
		sizeof(scsi_mode_sense_data_t) +
		sizeof(scsi_mode_sense_page15_t), ior);
	tgt->flags &= ~TGT_OPTIONAL_CMD;

	if ((ret == SCSI_RET_SUCCESS) && (ior->io_error == 0)) {

		scsi_mode_sense_page15_t	*page15;
		unsigned char			page15_buf
				[sizeof(scsi_mode_sense_page15_t)];

		page15 = (scsi_mode_sense_page15_t *)
			(((scsi_mode_sense_data_t *)tgt->cmd_ptr) + 1);

		if (want_compression) {
			if (page15->dcc == 0) {
				/* Drive does not support compression! */
				io_req_free(ior);
				return D_NO_SUCH_DEVICE;
			}

			/* Enable data compression */
			tgt->flags |= TGT_COMPRESSION;
			if (page15->dce == 1) {
				/* Data compression is already enabled */
				io_req_free(ior);
				return D_SUCCESS;
			}
			page15->dce = 1;
		} else {
			/* Disable data compression */
			tgt->flags &= ~TGT_COMPRESSION;
			if ((page15->dcc == 0) || (page15->dce == 0)) {
				/* Data compression is already disabled */
				io_req_free(ior);
				return D_SUCCESS;
			}
			page15->dce = 0;
		}

		/* copy sense data to temporary buffer */
		bcopy((char *)page15, page15_buf, sizeof(page15_buf));

		ior->io_op = IO_INTERNAL;
		ior->io_next = 0;
		ior->io_error = 0;
	        ior->io_count = 0;

		/* select new values */
		ret = sctape_mode_select(tgt, page15_buf, sizeof(page15_buf),
			FALSE, FALSE, ior);

		if (ret != SCSI_RET_SUCCESS) {
			io_req_free(ior);
			if (scsi_debug) {
				printf("[sctape_sstatus: %s]\n",
					"mode select error - page 15");
			}
			return D_IO_ERROR;
		}
	} else {
		/*
		 * Try the device configuration mode page 16 (0x10)
		 */

		boolean_t			dcc;
		scsi_mode_sense_page16_t	*page16;
		unsigned char			page16_buf
				[sizeof(scsi_mode_sense_page16_t)];

			if (scsi_debug) {
				printf("[sctape_sstatus: %s]\n",
				    "mode sense error - page 15 ");
				printf("[sctape_sstatus: %s %d]\n",
				    "ior->io_error = ",ior->io_error);
			}
		ior->io_op = IO_INTERNAL;
		ior->io_next = 0;
		ior->io_error = 0;
	        ior->io_count = 0;

		/*
		 * read changeable values, too many don't support it. Sigh
		 */
		tgt->flags |= TGT_OPTIONAL_CMD;
		ret = scsi_mode_sense(tgt, 0x40 | 16, /* changeable */
		    sizeof(scsi_mode_sense_data_t) + sizeof(page16_buf), ior);
		tgt->flags &= ~TGT_OPTIONAL_CMD;

		if ((ret != SCSI_RET_SUCCESS) || (ior->io_error != 0)) {
			io_req_free(ior);

			/*
			 * If we got here, the drive does not support
			 * mode page 15 or 16.  If we are trying to
			 * disable data compression, we will assume
			 * this condition is alright because we could
			 * not have enabled data compression, and drive
			 * should not support data compression if it
			 * does not support either of these mode pages.
			 * Otherwise this is an error.
			 */
			if (!want_compression)
				return D_SUCCESS;	/* well, alright */

			if (scsi_debug) {
				printf("[sctape_sstatus: %s]\n",
				    "mode sense error - page 16 (changeable)");
			}
			return D_IO_ERROR;
		}

		page16 = (scsi_mode_sense_page16_t *)
			(((scsi_mode_sense_data_t *)tgt->cmd_ptr) + 1);

		dcc = (page16->select_data_compression_algorithm) ?
				TRUE : FALSE;

		if (dcc == FALSE) {
			io_req_free(ior);

			if (want_compression) {
				/* Drive does not support compression! */
				return D_NO_SUCH_DEVICE;
			}

			tgt->flags &= ~TGT_COMPRESSION;
			return D_SUCCESS;
		}

		ior->io_op = IO_INTERNAL;
		ior->io_next = 0;
		ior->io_error = 0;
		ior->io_count = 0;

		/* read current values */
		ret = scsi_mode_sense(tgt, 16,
		    sizeof(scsi_mode_sense_data_t) + sizeof(page16_buf), ior);

		if (ret != SCSI_RET_SUCCESS) {
			io_req_free(ior);
			if (scsi_debug) {
				printf("[sctape_sstatus: %s]\n",
					"mode sense error - page 16");
			}
			return D_IO_ERROR;
		}

		if (want_compression) {
			/* Enable data compression */
			tgt->flags |= TGT_COMPRESSION;
			if (page16->select_data_compression_algorithm == 1) {
				/* Data compression is already enabled */
				io_req_free(ior);
				return D_SUCCESS;
			}
			page16->select_data_compression_algorithm = 1;
		} else {
			/* Disable data compression */
			tgt->flags &= ~TGT_COMPRESSION;
			if (page16->select_data_compression_algorithm == 0) {
				/* Data compression is already disabled */
				io_req_free(ior);
				return D_SUCCESS;
			}
			page16->select_data_compression_algorithm = 0;
		}

		/* copy sense data to temporary buffer */
		bcopy((char *)page16, page16_buf, sizeof(page16_buf));

		ior->io_op = IO_INTERNAL;
		ior->io_next = 0;
		ior->io_error = 0;
		ior->io_count = 0;

		/* select new values */
		ret = sctape_mode_select(tgt, page16_buf, sizeof(page16_buf),
			FALSE, FALSE, ior);

		if (ret != SCSI_RET_SUCCESS) {
			io_req_free(ior);
			if (scsi_debug) {
			printf("[sctape_sstatus: %s]\n",
				"mode select error - page 16");
			}
			return D_IO_ERROR;
		}
	}

	io_req_free(ior);
	return D_SUCCESS;
}

#endif	PARAGON860	/* Tape Compression Support */
