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
 * $Id: rz.c,v 1.20 1995/03/24 22:31:58 jerrie Exp $
 */

/*
 *
 * HISTORY
 * $Log: rz.c,v $
 * Revision 1.20  1995/03/24  22:31:58  jerrie
 * Locking code was added to force open and closes to be sequential.
 * Added baton hand off code to the passthru device which waits for
 * the active command queue to drain.
 *
 *  Reviewers:	   Arlin Davis, Richard Griffiths
 *  Risk:		   Medium.  Changes the open and close routines in the
 * 		   SCSI device driver.
 *  Benefit or PTS #: 12663
 *  Testing:	   Configured the SCSI device as a PFS file system and
 * 		   booted many times to force a parallel fsck.  Used
 * 		   debugging code to prove that opens and closes are now
 * 		   sequential operations.
 *  Module(s):	   kernel/scsi/rz.c
 *
 * Revision 1.19  1995/03/14  23:48:13  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.17  1995/01/17  20:28:12  lenb
 *  Reviewer: jerrie
 *  Risk: low
 *  Benefit or PTS #: 9625
 *    Prevent dead-lock in rz_set_status() by releasing the baton so
 *    that interrupts can drain the target queue. (9625)
 *  Module(s): kernel/scsi/rz.c
 *
 * Revision 1.16.2.1  1995/01/09  19:22:32  jerrie
 * Initial checkin for SCSI-16.
 *
 * Revision 1.16  1994/11/30  18:38:29  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: 11637
 *  Testing: formatted RAIDs installed with 4GB Seagates.
 *  Module(s): rz_set_status()
 *
 * Revision 1.15  1994/11/18  20:59:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.14  1994/08/31  21:25:47  mtm
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
 * Revision 1.11.2.2  1994/08/16  23:38:22  richardg
 *   Reviewer: Jerrie Coffman
 *   Risk: Low
 *   Benefit or PTS #: 10634
 *   Testing: the can utility, then piping the output of can to ace.
 *   Module(s): rz_get_status.
 *
 * Revision 1.11.2.1  1994/08/04  16:10:23  richardg
 *  Reviewer: Jerry Coffman
 *  Risk: Medium
 *  Benefit or PTS #: 10171
 *  Testing: ran Tensor's tapewrt program simultaneously against both LUNs.
 *  Module(s): rz_check, rz_open, rz_close, rz_get_status, rz_set_status
 *
 * Revision 1.11  1994/06/28  23:44:47  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.10  1994/05/05  21:09:48  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: return D_READ_ONLY instead of D_INVALID_OPERATION on an
 * 		 rz_write when the device is flagged READ_ONLY.
 *  Testing: tape EATS.
 *  Module(s): rz_write.
 *
 * Revision 1.9  1993/09/16  18:11:46  richardg
 * modified the memory allocation to evenly divide available MIO memory between existing scsi devices
 *
 * Revision 1.8  1993/09/01  20:58:07  jerrie
 * Fix for PTS bug number 6420.
 * rz_devinfo now returns DEV_BSIZE instead of tgt->block_size.
 * Writes larger than VM_MAP_COPY_PAGE_LIST_MAX_SIZE re-cycle
 * the ior in the device layer and now adjust the io_recnum
 * correctly for each continuation.
 *
 * Revision 1.7  1993/06/30  22:53:24  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.6  1993/06/08  22:24:00  jerrie
 * Moved scsi commands from open and close to the device type specific layer
 * to allow the tgt->lun field to be set to the on a per device basis.  Added
 * a lun parameter to the scsi_probe routine call.  Added the dev parameter to
 * the dev_ops->close function.
 *
 * Revision 1.5  1993/05/12  18:32:05  jerrie
 * Fix bug 5099.  Initialize the tgt->lun field using the LUN passed
 * down in the SCSI command descriptor block.  Clear the LUN field in
 * the SCSI command descriptor block per the SCSI-2 specification.
 *
 * Revision 1.4  1993/04/27  20:47:35  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:53:01  dleslie
 * First R1_0 release
 *
 * Revision 2.11.3.2  92/03/28  10:52:28  jeffreyh
 * 	Changes from TRUNK
 * 	[92/03/10  13:21:56  jeffreyh]
 * 
 * Revision 2.13  92/02/23  22:44:12  elf
 * 	Changed the interface of a number of functions not to
 * 	require the scsi_softc pointer any longer.  It was
 * 	mostly unused, now it can be found via tgt->masterno.
 * 	[92/02/22  19:31:18  af]
 * 
 * Revision 2.12  92/01/03  20:44:06  dbg
 * 	Added rz_devinfo().
 * 	[91/12/26  11:10:59  af]
 * 
 * Revision 2.11  91/10/09  16:16:57  af
 * 	Zero io_error before using the ior.
 * 
 * Revision 2.10  91/08/24  12:27:43  af
 * 	Pass along the ior in open calls.
 * 	Removed luna88k specialization, as the device headers have been
 * 	 rationalized. 
 * 	[91/08/02  03:51:40  af]
 * 
 * Revision 2.9  91/07/09  23:22:23  danner
 * 	Added gross ifdef luna88k to use <sd.h> instead of <scsi.h>. Will
 * 	 be fixed as soon as I figure out the configuration tools.
 * 	[91/07/09  11:05:28  danner]
 * 
 * Revision 2.8  91/06/19  11:56:54  rvb
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.7  91/05/14  17:26:11  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/05/13  06:04:14  af
 * 	Use a longer timeout on opens (exabytes are so slooooow to
 * 	get ready).  If we are still rewinding the tape make open
 * 	fail.  Start watchdog on first open, for those adapters
 * 	that need it.  Ask for sense data when unit does not
 * 	come online quickly enough. On close, invoke anything
 * 	target-specific that needs to.
 * 	[91/05/12  16:04:29  af]
 * 
 * Revision 2.5.1.1  91/03/29  17:21:00  af
 * 	Use a longer timeout on opens (exabytes are so slooooow to
 * 	get ready).  If we are still rewinding the tape make open
 * 	fail.  Start watchdog on first open, for those adapters
 * 	that need it.  Ask for sense data when unit does not
 * 	come online quickly enough. On close, invoke anything
 * 	target-specific that needs to.
 * 
 * Revision 2.5  91/02/05  17:43:37  mrt
 * 	Added author notices
 * 	[91/02/04  11:16:27  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:15:22  mrt]
 * 
 * Revision 2.4  90/12/20  16:59:59  jeffreyh
 * 	Do not use minphys(), we do not need to trim requests.
 * 	[90/12/11            af]
 * 
 * Revision 2.3  90/12/05  23:33:46  af
 * 	Cut&retry for requests that exceed the HBA's dma capacity.
 * 	[90/12/03  23:32:38  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:43:31  af
 * 	Created.
 * 	[90/10/21            af]
 */
/*
 *	File: rz.c
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	10/90
 *
 *	Top layer of the SCSI driver: interface with the MI side.
 */

/*
 * This file contains the code that is common to all scsi devices,
 * operations and/or behaviours specific to certain devices live
 * in the corresponding rz_mumble files.
 */

#include <scsi.h>

#if	(NSCSI>0)

#include <mach/std_types.h>
#include <scsi/compat_30.h>

#ifdef	MACH_KERNEL
#include <kern/time_out.h>
#include <vm/vm_kern.h>
#else	/*MACH_KERNEL*/
#include <sys/kernel.h>		/* for hz */

static io_req_t	getbp();
#endif	/*MACH_KERNEL*/

#include <scsi/scsi.h>
#include <scsi/scsi_defs.h>
#include <scsi/rz.h>

#if	PARAGON860
#include <i860paragon/baton.h>
#include <i860paragon/spl.h>
#include <kern/cpu_number.h>
#endif	PARAGON860

/*static*/ boolean_t
rz_check(dev, p_sc, p_tgt)
	scsi_softc_t	**p_sc;
	target_info_t	**p_tgt;
{
	if (rzcontroller(dev) >= NSCSI ||
	    (*p_sc = scsi_softc[rzcontroller(dev)]) == 0)
		return FALSE;

	if (rzpassthru(dev)) /* use LUN 0 for passthru devices */
		*p_tgt = (*p_sc)->target[rzslave(dev)][0];
	else
		*p_tgt = (*p_sc)->target[rzslave(dev)][rzlun(dev)];

	if (!*p_tgt ||
	    !((*p_tgt)->flags&TGT_ALIVE))
		return FALSE;
	return TRUE;
}

/*
 * Open routine
 *
 * On tapes and other devices might have to wait a bit for
 * the unit to come alive. The following patchable variable
 * takes this into account
 */
int rz_open_timeout = 60;/* seconds */

rz_open(dev, mode, ior)
	int		dev;
	dev_mode_t	mode;
	io_req_t	ior;
{
	scsi_softc_t	*sc = 0;
	target_info_t	*tgt;
	scsi_ret_t	ret;

	if (!rz_check(dev, &sc, &tgt)) {
		/*
		 * Probe it again: might have installed a new device
		 */
		if (!sc || !scsi_probe(sc, &tgt, rzslave(dev), rzlun(dev), ior))
			return D_NO_SUCH_DEVICE;
	}

	/*
	 * Don't do anything if a pass-through device is opened.
	 * The controlling program had better know what it's doing!
	 */
	if (rzpassthru(dev)) return 0;	/* XXX */

	/* tapes do not wait for rewind to complete on close... but we do
	if (tgt->ior && !(tgt->flags & TGT_ONLINE))
		return D_WOULD_BLOCK;
	*/

	/*
	 * Grab the target lock.
	 * We can only process one open at a time.
	 */
	while (!SIMPLE_LOCK_TRY(&tgt->target_lock)) {
		timeout(wakeup, ior, hz);
		await(ior);			/* sweet dreams... */
	}

	if (scsi_debug) {
		printf("opening %s%d..",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->unit_no);
	}

	if (sc->watchdog) {
		(*sc->watchdog)(tgt->hw_state);
		sc->watchdog = 0;
	}

	/*
	 * Perform anything open-time special on the device
	 */
	if (tgt->dev_ops->open) {
		ret = (*tgt->dev_ops->open)(tgt, ior);
		if (ret != SCSI_RET_SUCCESS) {
			 printf("%s%d: open failed x%x\n",
			 	(*tgt->dev_ops->driver_name)(TRUE),
				tgt->unit_no, ret);
			SIMPLE_UNLOCK(&tgt->target_lock);
			return ret;
		}
	}

	tgt->flags |= TGT_ONLINE;

	SIMPLE_UNLOCK(&tgt->target_lock);

	return 0;	/* XXX */
}

rz_close(dev)
	int		dev;
{
	scsi_softc_t	*sc;
	target_info_t	*tgt;
	io_req_t	ior;
	scsi_ret_t	ret;

	if (!rz_check(dev, &sc, &tgt))
		return D_NO_SUCH_DEVICE;

	/*
	 * Don't do anything if a pass-through device is closed.
	 * The controlling program had better know what it's doing!
	 */
	if (rzpassthru(dev)) return 0;	/* XXX */

	/*
	 * Dummy ior for proper sync purposes
	 */
	io_req_alloc(ior, 0);
	ior->io_unit   = dev;
	ior->io_device = 0;
	ior->io_next   = 0;
	ior->io_prev   = 0;
	ior->io_count  = 0;
	ior->io_error  = 0;
	ior->io_op     = IO_INTERNAL;

	/*
	 * Grab the target lock.
	 * We can only process one close at a time.
	 */
	while (!SIMPLE_LOCK_TRY(&tgt->target_lock)) {
		timeout(wakeup, ior, hz);
		await(ior);			/* sweet dreams... */
	}

	if (scsi_debug) {
		printf("closing %s%d..",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->unit_no);
	}

	/*
	 * Perform anything close-time special on the device
	 */
	if (tgt->dev_ops->close) {
		ret = (*tgt->dev_ops->close)(dev, tgt);
		if (ret != SCSI_RET_SUCCESS) {
			 printf("%s%d: close failed x%x\n",
			 	(*tgt->dev_ops->driver_name)(TRUE),
				tgt->unit_no, ret);
		}
	}
	if (tgt->flags & TGT_REMOVABLE_MEDIA)
		tgt->flags &= ~TGT_ONLINE;

	SIMPLE_UNLOCK(&tgt->target_lock);

	io_req_free(ior);

	return 0;	/* XXX */
}

/* our own minphys */
rz_minphys(ior)
	io_req_t	ior;
{
#ifdef	MACH_KERNEL
#else	/*MACH_KERNEL*/
	if (ior->io_count > scsi_per_target_virtual)
		ior->io_count = scsi_per_target_virtual;
#endif	/*MACH_KERNEL*/
}

rz_read(dev, ior)
	int		dev;
	io_req_t	ior;
{
	target_info_t	*tgt;

	SCSI_DRIVER_CHECKIN(ior);

	tgt = scsi_softc[rzcontroller(dev)]->target[rzslave(dev)][rzlun(dev)];

#ifdef	MACH_KERNEL
	return block_io(tgt->dev_ops->strategy, rz_minphys, ior);
#else	/*MACH_KERNEL*/
	return physio(tgt->dev_ops->strategy, getbp(dev), dev, IO_READ, rz_minphys, ior);
#endif	/*MACH_KERNEL*/
}

rz_write(dev, ior)
	int		dev;
	io_req_t	ior;
{
	target_info_t	*tgt;

	SCSI_DRIVER_CHECKIN(ior);

	tgt = scsi_softc[rzcontroller(dev)]->target[rzslave(dev)][rzlun(dev)];

	if (tgt->flags & TGT_READONLY)
		return D_READ_ONLY;

#ifdef	MACH_KERNEL
	return block_io(tgt->dev_ops->strategy, rz_minphys, ior);
#else	/*MACH_KERNEL*/
	return physio(tgt->dev_ops->strategy, getbp(dev), dev, IO_WRITE, rz_minphys, ior);
#endif	/*MACH_KERNEL*/
}

#include <sys/ioctl.h>
#include <scsi/scsi_status.h>

struct scsi_io scsi_passthru;

rz_get_status(dev, flavor, status, status_count)
	int		dev;
	int		flavor;
	dev_status_t	status;
	unsigned int	*status_count;
{
	target_info_t	*tgt;

	if (!rzpassthru(dev))
	  tgt = scsi_softc[rzcontroller(dev)]->target[rzslave(dev)][rzlun(dev)];
	else
	  tgt = scsi_softc[rzcontroller(dev)]->target[rzslave(dev)][0];

	if (scsi_debug)
		printf("rz_get_status: x%x x%x x%x x%x\n",
			dev, flavor, status, *status_count);

	switch (flavor) {

		case DIOCGPHYS:
		{
			struct scsi_phys *phys = (struct scsi_phys *)status;

			/*
			 * Return physical information
			 * for a pass-through device
			 */

			if (!rzpassthru(dev))
				return D_INVALID_OPERATION;

#if	iPSC386 || PARAGON860
			phys->slot       = node_self();
#else	iPSC386 || PARAGON860
			phys->slot       = 0;
#endif	iPSC386 || PARAGON860
			phys->controller = rzcontroller(dev);
			phys->target_id  = rzslave(dev);
			phys->lun        = tgt->true_lun;

			*status_count = sizeof(struct scsi_phys)/sizeof(int);

			break;
		}

		case DIOCSCSI:
		{
			/*
			 * Not much to do here, everything was handled in
			 * rz_set_status.  Just copy out the scsi_passthru
			 * structure.
			 */
			*(struct scsi_io *)status = *(&scsi_passthru);
			*status_count = sizeof(struct scsi_io)/sizeof(int);

			break;
		}

		case SCSIGSTATS:
		{
			struct scsi_stats *stats = (struct scsi_stats *)status;

			if (*status_count != sizeof(struct scsi_stats))
				return D_INVALID_SIZE;

			/*
			 * We copy the statistics over one at time in case
			 * there is a difference in the structure definitions
			 */

			stats->read.requests	=
				tgt->statistics.read.requests;
			stats->read.xfer_count  =
				tgt->statistics.read.xfer_count;
			stats->read.driver_time =
				tgt->statistics.read.driver_time;
			stats->read.device_time =
				tgt->statistics.read.device_time;

			stats->write.requests	 =
				tgt->statistics.write.requests;
			stats->write.xfer_count  =
				tgt->statistics.write.xfer_count;
			stats->write.driver_time =
				tgt->statistics.write.driver_time;
			stats->write.device_time =
				tgt->statistics.write.device_time;

			stats->queue_max   = tgt->statistics.queue_max;
			stats->chain_max   = tgt->statistics.chain_max;
			stats->queue_depth = tgt->statistics.queue_depth;
			stats->chain_depth = tgt->statistics.chain_depth;
			stats->driver_full = tgt->statistics.driver_full;
			stats->device_full = tgt->statistics.device_full;
			stats->retries	   = tgt->statistics.retries;

			*status_count = sizeof(struct scsi_stats)/sizeof(int);

			break;
		}

		default:
			return (*tgt->dev_ops->get_status)
				(dev, tgt, flavor, status, status_count);
	}

	return D_SUCCESS;
}

rz_set_status(dev, flavor, status, status_count)
	int		dev;
	int		flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
	target_info_t	*tgt;

	if (rzpassthru(dev))
	  tgt = scsi_softc[rzcontroller(dev)]->target[rzslave(dev)][0];
	else
	  tgt = scsi_softc[rzcontroller(dev)]->target[rzslave(dev)][rzlun(dev)];

	if (scsi_debug)
		printf("rz_set_status: x%x x%x x%x x%x\n",
			dev, flavor, status, status_count);

	switch (flavor) {

		case DIOCSCSI:
		{
			struct scsi_io	*p = &scsi_passthru;
			int		datain = FALSE;
			unsigned int	i, cmd;
			unsigned int	old_timeout;
			io_req_t	ior;
			int		ret;

			/*
			 * Handle generic SCSI device pass-through command
			 */

			*(&scsi_passthru) = *(struct scsi_io *)status;

			if (!rzpassthru(dev))
				return D_INVALID_OPERATION;

			if (status_count != sizeof(struct scsi_io)/sizeof(int))
				return D_INVALID_SIZE;

			if (p->cmd_count > 256)
				return D_INVALID_SIZE;
#if	PARAGON860	/* performance mod */
			if (p->buf_len > tgt->max_dma_data)
#else
			if (p->buf_len >
			    scsi_softc[tgt->masterno]->max_dma_data)
#endif
				return D_INVALID_SIZE;

			/* ++ HACK Alert */
			/* cmd = p->cdb[0]; */
			switch (p->direction) {

				case SCSI_NONE:		/* no data phase */
					cmd = SCSI_CMD_TEST_UNIT_READY;
					break;

				case SCSI_TO_MEM:	/* data in phase */
					cmd = SCSI_CMD_READ;
					datain = TRUE;
					break;

				case MEM_TO_SCSI:	/* data out phase */
					cmd = SCSI_CMD_WRITE;
					break;

				default:
					return D_INVALID_OPERATION;
			}
			/* -- HACK Alert */

			/* Build the I/O request */
			io_req_alloc(ior, 0);
			bzero(ior, sizeof(struct io_req));
			ior->io_unit	 = dev;
			ior->io_op	 = (datain) ? IO_READ : 0;
			ior->io_op	|= IO_INTERNAL;
			ior->io_error	 = 0;
			ior->io_next	 = 0;
			ior->io_prev	 = 0;
			ior->io_unit	 = dev;
			ior->io_data	 = (io_buf_ptr_t)p->buf;
			ior->io_count	 = p->buf_len;
			ior->io_residual = 0;

			/*
			 * If the controller supports scatter/gather, allocate
			 * wired-down memory for the scatter/gather list 
			 */
			if (scsi_softc[tgt->masterno]->supports_sgio) {
				if (device_read_alloc_sg(ior, ior->io_count) !=
				    KERN_SUCCESS) {
		                	io_req_free(ior);
			                return D_NO_MEMORY;
				}
				bcopy(p->buf, ior->io_data, ior->io_count);
			}

			/*
			 * Spin waiting for previous requests to go away,
			 * then grab the target.
			 * 
			 * Problems may arise when command queuing is
			 * happening and the cmd_ptr's are swapped.
			 * Wait for the active_tag_q to empty.
			 */
			while (!queue_empty(&tgt->active_tag_q)) { /* ahem */
#if	PARAGON860
#if	ASMP
				/*
				 * EXP interrupts all go to master_cpu.
				 * If we spin here with the baton, waiting
				 * for the results of an interrupt, then the
				 * interrupt will block on the baton forever.
				 * So baton_exit/enter here to give interrupt
				 * a window.
				 */
				if (cpu_number() != master_cpu) {
					baton_exit();
					ASSERT_BATON_NOT_OWNER();
					delay(10);
					baton_enter();
				}
#endif	/* ASMP */
#endif	PARAGON860
			}
			do {
				assert(get_spl() == SPL0);

				i = splbio();
				simple_lock(&tgt->target_lock);
				if (!tgt->ior)
					tgt->ior = ior;
				simple_unlock(&tgt->target_lock);
				splx(i);
#if	PARAGON860
#if	ASMP
				/*
				 * EXP interrupts all go to master_cpu.
				 * If we spin here with the baton, waiting
				 * for the results of an interrupt, then the
				 * interrupt will block on the baton forever.
				 * So baton_exit/enter here to give interrupt
				 * a window.
				 */
				if ((tgt->ior != ior) &&
				    cpu_number() != master_cpu) {
					baton_exit();
					ASSERT_BATON_NOT_OWNER();
					delay(10);
					baton_enter();
				}
#endif	/* ASMP */
#endif	PARAGON860
			} while (tgt->ior != ior);		  /* ahem */

			if (tgt->ior != ior) {
				if (scsi_softc[tgt->masterno]->supports_sgio) {
					/* Free the allocated buffers */
					kmem_free(kernel_map, ior->io_data,
						ior->io_alloc_size);
					io_sglist_free(ior->io_sgp);
				}
				io_req_free(ior);
				return D_ALREADY_OPEN;
			}

			/*
			 * Use the command descriptor LUN
			 * field to set the target LUN
			 */
			tgt->lun = (p->cdb[1] & SCSI_LUN_MASK) >> 5;

			/*
			 * Clear the SCSI command descriptor LUN
			 * field as recommended by the SCSI-2
			 * specification (see section 6.2.2)
			 */
			p->cdb[1] &= ~SCSI_LUN_MASK;

			/*
			 * Copy the SCSI command to the
			 * target command pointer
			 */
			tgt->cur_cmd = cmd;
			for (i = 0; i < p->cmd_count; i++) {
				tgt->cmd_ptr[i] = p->cdb[i];
			}

			if (p->cdb[0] == SCSI_CMD_FORMAT_UNIT) {
				/* save timeout */
				old_timeout = scsi_watchdog_period;

				/* 8 hours should be enough, I hope */
				scsi_watchdog_period =  8 * 60 * 60;
			}

			/* Execute the SCSI command */
			scsi_go_and_wait(tgt, i,
					 (datain) ? p->buf_len : 0, ior);

			if (p->cdb[0] == SCSI_CMD_FORMAT_UNIT) {
				/* restore timeout */
				scsi_watchdog_period = old_timeout;
			}

			/* Save the amount of data actually transferred */
			p->buf_len = ior->io_count;

			if (scsi_softc[tgt->masterno]->supports_sgio) {
				bcopy(ior->io_data, p->buf, ior->io_count);

				/* Free the allocated buffers */
				kmem_free(kernel_map, ior->io_data,
					ior->io_alloc_size);
				io_sglist_free(ior->io_sgp);
			}

			/* Check the command results */
			ret = D_SUCCESS;
			if (ior->io_error != SCSI_RET_SUCCESS) {
				if (ior->io_error == SCSI_RET_NEED_SENSE) {
					p->status = SCSI_CHECK_CONDITION;
				} else
				if (ior->io_error == SCSI_RET_RETRY) {
					p->status = SCSI_BUSY;
				} else
				if (ior->io_error == SCSI_RET_DEVICE_DOWN) {
					ret = D_DEVICE_DOWN;
				} else {
					ret = D_IO_ERROR;
				}
			} else p->status = SCSI_GOOD;

			io_req_free(ior);

			return ret;
		}

		case SCSISSTATS:
		{
			if (status_count != sizeof(struct scsi_stats))
				return D_INVALID_SIZE;

			/*
			 * Clear the statistics
			 */

			tgt->statistics.read.requests	  = 0;
			tgt->statistics.read.xfer_count	  = 0;
			tgt->statistics.read.driver_time  = 0;
			tgt->statistics.read.device_time  = 0;

			tgt->statistics.write.requests	  = 0;
			tgt->statistics.write.xfer_count  = 0;
			tgt->statistics.write.driver_time = 0;
			tgt->statistics.write.device_time = 0;

			tgt->statistics.queue_max	  = 0;
			tgt->statistics.chain_max	  = 0;
			/*
			 * Don't clear these
			 * Depths are updated on the fly
			 *
			tgt->statistics.queue_depth	  = 0;
			tgt->statistics.chain_depth	  = 0;
			 */
			tgt->statistics.driver_full	  = 0;
			tgt->statistics.device_full	  = 0;
			tgt->statistics.retries		  = 0;

			break;
		}

		default:
			return (*tgt->dev_ops->set_status)
				(dev, tgt, flavor, status, status_count);
	}

	return D_SUCCESS;
}

/*
 *	Routine to return information to kernel.
 */
int
rz_devinfo(dev, flavor, info)
	int	dev;
	int	flavor;
	char	*info;
{
	scsi_softc_t	*sc;
	target_info_t	*tgt;
	register int	result;

	if (rzcontroller(dev) >= NSCSI ||
	    (sc = scsi_softc[rzcontroller(dev)]) == 0)
		return D_NO_SUCH_DEVICE;
	tgt = sc->target[rzslave(dev)][rzlun(dev)];

	/*
	 * If D_NO_SUCH_DEVICE is returned the calling function may send
	 * a request with sg_list pointer uninitialized.  Therefore remove
	 * this check:
	 *
	 * if (!rz_check(dev, &sc, &tgt))
	 * 	return D_NO_SUCH_DEVICE;
	 */

	result = D_SUCCESS;

	switch (flavor) {

	case D_INFO_BLOCK_SIZE:
		if (rzpassthru(dev)) *((unsigned int *)info) = tgt->block_size;
		else *((unsigned int *)info) = RZ_DEFAULT_BSIZE;
		break;

	case D_INFO_SGLIST_IO:
		*((boolean_t *)info) = sc->supports_sgio;
		break;

	default:
		result = D_INVALID_OPERATION;
	}

	return(result);
}

#if	0
rz_size(dev)
	int	dev;
{	
	return -1;
}

rz_reset(adaptor)
	int	adaptor;
{}
#endif

#ifdef	MACH_KERNEL
#else	/*MACH_KERNEL*/

rz_strategy(ior)
	io_req_t	ior;
{
	target_info_t	*tgt;
	register int	dev = ior->io_unit;

	tgt = scsi_softc[rzcontroller(dev)]->target[rzslave(dev)][rzlun(dev)];

	return (*tgt->dev_ops->strategy)(ior);
}

#include <sys/ioctl.h>
#define	IOCPARM_SIZE(c)		(((c)>>16)&IOCPARM_MASK)
#define	IOC_WDSIZE(s)		((IOCPARM_SIZE(s))>>2)

rz_ioctl(dev, cmd, data, flag)
{
	io_return_t	error;
	unsigned int	count;

	count = IOC_WDSIZE(cmd);
        if (cmd & (IOC_VOID|IOC_IN)) {
            error = rz_set_status(dev, cmd, (dev_status_t)data, count);
            if (error)
                return (error);
        }
        if (cmd & IOC_OUT) {
            error = rz_get_status(dev, cmd, (dev_status_t *)data, &count);
            if (error)
                return (error);
        }
        return (0);
}

/* This is a very simple-minded config,
 * assumes we have << MAX_SCSI_TARGETS disks per bus */
#define NBUF	(NSCSI*MAX_SCSI_TARGETS)
struct	io_req	rz_buffers[NBUF];

static io_req_t
getbp(dev)
{
	io_req_t	ior;
	int		hash = minor(dev) >> 3;

	ior = &rz_buffers[hash];
#if 1
	if (ior->io_op & IO_BUSY) {
		register io_req_t ior;
		for (ior = rz_buffers; ior < &rz_buffers[NBUF]; ior++)
			if ((ior->io_op & IO_BUSY) == 0)
				return ior;
		
	}
#endif
	return ior;
}

/*
 * This ugliness is only needed because of the
 * way the minor is encoded for tapes.
 */
tz_open(dev, mode, ior)
	int		dev;
	dev_mode_t	mode;
	io_req_t	ior;
{
	io_return_t	error;

	error = rz_open(TAPE_UNIT(dev), mode, ior);
	if(error)
		return error;
	if (TAPE_REWINDS(dev)) {
		scsi_softc_t	*sc;
		target_info_t	*tgt;

		rz_check(TAPE_UNIT(dev), &sc, &tgt);
		tgt->flags |= TGT_REWIND_ON_CLOSE;
	}
	return 0;
}

tz_close(dev) { return rz_close(TAPE_UNIT(dev));}
tz_read(dev, ior) { return rz_read(TAPE_UNIT(dev), ior);}
tz_write(dev, ior) { return rz_write(TAPE_UNIT(dev), ior);}
tz_ioctl(dev, cmd, data, flag) { return rz_ioctl(TAPE_UNIT(dev), cmd, data, flag);}

#endif	/*MACH_KERNEL*/

#endif	(NSCSI>0)
