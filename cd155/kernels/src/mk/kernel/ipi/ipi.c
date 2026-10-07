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
 * $Log: ipi.c,v $
 * Revision 1.6  1995/03/24  22:11:11  jerrie
 * The locking code used force open and closes to be sequential was not
 * defined for ASMP kernels.  The lock was changed to use the correct
 * definition and a sleep was added to allow the thread which has acquired
 * the lock to run.
 *
 *  Reviewers:	   Arlin Davis, Richard Griffiths
 *  Risk:		   Medium.  Changes the open and close routines in the
 * 		   IPI device driver.
 *  Benefit or PTS #: 12663
 *  Testing:	   Configured the IPI-3 device as a PFS file system and
 * 		   booted many times to force a parallel fsck.  Used
 * 		   debugging code to prove that opens and closes are now
 * 		   sequential operations.
 *  Module(s):	   kernel/ipi/ipi.c
 *
 * Revision 1.5  1995/03/10  21:30:58  jerrie
 * Locking code was missing around a critical portion of the IPI device
 * open and close routines.  This window allowed multiple threads to
 * attempt setting a HiPPI destination filter for the IPI master, which
 * resulted in filter set errors.  Locks have been added around the
 * critical areas and the maintenance of the driver open count has been
 * rearranged to minimize any exposure.
 *
 *  Reviewer:	   Arlin Davis
 *  Risk:		   Low.  Only affects the IPI device driver.
 *  Benefit or PTS #: 12663
 *  Testing:	   Configured /etc/fstab to mount six IPI device file
 * 		   systems and booted Paragon system to automatically
 * 		   run parallel fsck.  Ran the Evaluation EAT test
 * 		   listed in the bug report.
 *  Module(s):	   kernel/ipi/ipi.c
 *
 * Revision 1.4  1995/03/02  23:35:56  arlin
 *   Added Multiple Packet per Connection support
 *   and CONTinuation support for multiple I/O
 *   per packet and connection. F/W is R1.4
 *
 *   Reviewer: Jerrie Coffman, Bernie Keany
 *   Risk: medium
 *   Benefit or PTS #: 12411
 *   Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *       Also developed special applications to test
 *       new MPC and CONT modes.
 *   Module(s):
 *     /i860paragon/hippi/
 *       hctlr.c, hctlr.h, rhippi.h, rhippi.c,
 *       hippi_status.h, hdc.c
 *     /ipi/ipi_misc.c ipi_defs.h, ipi.c,
 *     /device/ds_routines.c
 *
 * Revision 1.3  1995/02/02  19:10:46  jerrie
 * Configuration checks during the first open of an IPI device failed due
 * to an I-field conflict with another IPI device, but returned leaving the
 * driver initialized to bypass the configuration checks on subsequent opens.
 *
 * The driver was changed to correct this problem and also to change the
 * policy used to perform I-field checking.  Under the new policy, the driver
 * returns a device busy error if the conflicting slave is open, otherwise
 * it succeeds.  The master device open will always succeed, regardless of
 * any I-field conflicts, to allow the user can correct an I-field conflict.
 * An ioctl call to set the I-field to a new value will fail if the new
 * I-field conflicts with another open device, otherwise it will succeed
 * with a warning message displayed on the I/O node console.
 *
 *  Reviewer:         Arlin Davis
 *  Risk:             Low.  Only affects the IPI device driver.
 *  Benefit or PTS #: 11413
 *  Testing:          Performed the test sequence outlined in the PTS bug
 *                    report.  Ran several test cases opening and closing
 *                    IPI devices with conflicting I-fields, using the master
 *                    device, and using ioctls to set new I-field values.
 *
 *  Module(s):        The following kernel files are being modified:
 *
 *                    kernel/i860paragon/hippi:       hdc.c
 *                    kernel/ipi:                     ipi.c, ipi_defs.h,
 *                                                    ipi_master.c, ipi_misc.c
 *
 * Revision 1.2  1994/11/18  20:50:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/08  16:53:39  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Top layer of the IPI-3 driver: interface with the MI side.
 */

/*
 * This file contains the code that is common to all IPI-3 devices.
 * Operations and/or behaviours specific to devices types reside in
 * the corresponding ipi_mumble files.
 */

#include <ipi.h>
#if	NIPI > 0

#include <ipi/ipi_compat.h>
#include <ipi/ipi_endian.h>
#include <ipi/ipi_defs.h>
#include <ipi/ipi_map.h>


static boolean_t
ipi_check(dev, p_sc, p_tgt)
	int		dev;
	ipi_softc_t	**p_sc;
	target_info_t	**p_tgt;
{
	if (IPI_CONTROLLER(dev) >= NIPI ||
	    (*p_sc = ipi_softc[IPI_CONTROLLER(dev)]) == 0)
		return FALSE;

	*p_tgt = (*p_sc)->target[IPI_SLAVE(dev)];

	if (!*p_tgt				  ||
	    !((*p_tgt)->flags & TGT_FULLY_PROBED) ||
	    !((*p_tgt)->flags & TGT_ALIVE))
		return FALSE;

	return TRUE;
}


/*
 * Open routine
 */
ipi_open(dev, mode, ior)
	int		dev;
	dev_mode_t	mode;
	io_req_t	ior;
{
	ipi_softc_t	*sc = 0;
	target_info_t	*tgt;
	ipi_ret_t	ret;

	if (!ipi_check(dev, &sc, &tgt)) {
		/*
		 * Check for no controller
		 */
		if (!sc) {
			return D_NO_SUCH_DEVICE;
		}

		/*
		 * Probe it again: might have installed a new device
		 */
		if ((ret = ipi_probe(sc, &tgt, dev, ior)) != D_SUCCESS) {
			return ret;
		}
	} else {
		/*
		 * Check for configuration errors
		 */
		if ((ret = ipi_config_error(tgt, FALSE)) != D_SUCCESS) {
			return ret;
		}
	}

	/*
	 * Grab the target lock.
	 * We can only process one open at a time.
	 */
	while (!SIMPLE_LOCK_TRY(&tgt->target_lock)) {
		timeout(wakeup, ior, hz);
		await(ior);			/* sweet dreams... */
	}

	if (ipi_debug)
		printf("opening %s%d...",
		       (*tgt->dev_ops->driver_name)(TRUE), dev & DEV_MASK);

	if (sc->watchdog) {
		(*sc->watchdog)(tgt->hw_state);
		sc->watchdog = 0;
	}

	/*
	 * If this is the first open, register the destination channel filter
	 */
	if ((tgt->open_count++ == 0) &&
	    (sc->filter(tgt->hw_state, TRUE) != D_SUCCESS)) {
		printf("%s%d: Error setting destination channel filter\n",
			(*tgt->dev_ops->driver_name)(TRUE), dev & DEV_MASK);
		tgt->open_count--;
		SIMPLE_UNLOCK(&tgt->target_lock);
		return D_DEVICE_DOWN;
	}

	/*
	 * Perform anything open-time special on the device
	 */
	if (tgt->dev_ops->open) {
		ret = (*tgt->dev_ops->open)(tgt, ior);
		if (ret != IPI_RET_SUCCESS) {
			printf("%s%d: open failed 0x%x\n",
			  (*tgt->dev_ops->driver_name)(TRUE), dev & DEV_MASK,
			  ret);
			/*
			 * Clear the destination channel filter
			 */
			if (sc->filter(tgt->hw_state, FALSE) != D_SUCCESS) {
				printf("%s%d: %s\n",
				  (*tgt->dev_ops->driver_name)(TRUE),
				  dev & DEV_MASK,
				  "Error clearing destination channel filter");
			}
			tgt->open_count--;
			SIMPLE_UNLOCK(&tgt->target_lock);
			return ret;
		}
	}

	tgt->flags |= TGT_ONLINE;

	if (ipi_debug)
		printf("%s%d opened\n",
		       (*tgt->dev_ops->driver_name)(TRUE), dev & DEV_MASK);

	SIMPLE_UNLOCK(&tgt->target_lock);

	return D_SUCCESS;
}


ipi_close(dev)
	int		dev;
{
	ipi_softc_t	*sc;
	target_info_t	*tgt;
	io_req_t	ior;
	ipi_ret_t	ret = D_SUCCESS;

	sc  = ipi_softc[IPI_CONTROLLER(dev)];
	tgt = sc->target[IPI_SLAVE(dev)];

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

	if (ipi_debug)
		printf("closing %s%d...",
		       (*tgt->dev_ops->driver_name)(TRUE), dev & DEV_MASK);

	/*
	 * Perform anything close-time special on the device
	 */
	if (tgt->dev_ops->close) {
		ret = (*tgt->dev_ops->close)(dev, tgt);
		if (ret != IPI_RET_SUCCESS) {
			 printf("%s%d: close failed 0x%x\n",
			   (*tgt->dev_ops->driver_name)(TRUE), dev & DEV_MASK,
			   ret);
		}
	}

	if (tgt->flags & TGT_REMOVABLE_MEDIA)
		tgt->flags &= ~TGT_ONLINE;

	/*
	 * If this is the last close, clear the destination channel filter
	 */
	if ((tgt->open_count-- == 1) &&
	    (sc->filter(tgt->hw_state, FALSE) != D_SUCCESS)) {
		printf("%s%d: Error clearing destination channel filter\n",
			(*tgt->dev_ops->driver_name)(TRUE), dev & DEV_MASK);
	}

	if (ipi_debug)
		printf("%s%d closed\n",
		       (*tgt->dev_ops->driver_name)(TRUE), dev & DEV_MASK);

	SIMPLE_UNLOCK(&tgt->target_lock);

	io_req_free(ior);

	return ret;
}


/* our own minphys */
static
ipi_minphys(ior)
	io_req_t	ior;
{
	target_info_t	*tgt;
	int		dev = ior->io_unit;

	tgt = ipi_softc[IPI_CONTROLLER(dev)]->target[IPI_SLAVE(dev)];

        if (ior->io_count > tgt->max_io_count)
            ior->io_count = tgt->max_io_count;
}

ipi_read(dev, ior)
	int		dev;
	io_req_t	ior;
{
	target_info_t	*tgt;

	IPI_DRIVER_CHECKIN(ior);

	tgt = ipi_softc[IPI_CONTROLLER(dev)]->target[IPI_SLAVE(dev)];

	return block_io(tgt->dev_ops->strategy, ipi_minphys, ior);
}

ipi_write(dev, ior)
	int		dev;
	io_req_t	ior;
{
	target_info_t	*tgt;

	IPI_DRIVER_CHECKIN(ior);

	tgt = ipi_softc[IPI_CONTROLLER(dev)]->target[IPI_SLAVE(dev)];

	if (tgt->flags & TGT_READONLY)
		return D_READ_ONLY;

	return block_io(tgt->dev_ops->strategy, ipi_minphys, ior);
}


#include <sys/ioctl.h>
#include <ipi/ipi_status.h>

ipi_get_status(dev, flavor, status, status_count)
	int		dev;
	unsigned int	flavor;
	dev_status_t	status;
	unsigned int	*status_count;
{
	target_info_t	*tgt;

	tgt = ipi_softc[IPI_CONTROLLER(dev)]->target[IPI_SLAVE(dev)];

	if (ipi_debug)
		printf("ipi_get_status: 0x%x 0x%x 0x%x 0x%x\n",
			dev, flavor, status, *status_count);

	switch (flavor) {

		case IPIPHYS:
		case IPIGIFIELD:
		case IPIGFACILITY:
		case IPIGPART:
		case IPIGCMDREF:
		case IPIGSTATS:
			return ipm_get_status
				(dev, flavor, status, status_count);

		default:
			return (*tgt->dev_ops->get_status)
				(dev, tgt, flavor, status, status_count);
	}

	/*NOTREACHED*/
	return D_SUCCESS;
}


ipi_set_status(dev, flavor, status, status_count)
	int		dev;
	unsigned int	flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
	target_info_t	*tgt;

	tgt = ipi_softc[IPI_CONTROLLER(dev)]->target[IPI_SLAVE(dev)];

	if (ipi_debug)
		printf("ipi_set_status: 0x%x 0x%x 0x%x 0x%x\n",
		       dev, flavor, status, status_count);

	switch (flavor) {

		case IPISIFIELD:
		case IPISFACILITY:
		case IPISPART:
		case IPISCMDREF:
		case IPISSTATS:
			return ipm_set_status
				(dev, flavor, status, status_count);

		default:
			return (*tgt->dev_ops->set_status)
				(dev, tgt, flavor, status, status_count);
	}

	/*NOTREACHED*/
	return D_SUCCESS;
}


/*
 * Routine to return information to kernel
 */
ipi_devinfo(dev, flavor, info)
	int		dev;
	unsigned int	flavor;
	char		*info;
{
	ipi_softc_t	*sc;
	target_info_t	*tgt;

	if (!ipi_check(dev, &sc, &tgt))
		return D_NO_SUCH_DEVICE;

	switch (flavor) {

		case D_INFO_BLOCK_SIZE:
			*((unsigned int *)info) = tgt->block_size;
			break;

		case D_INFO_SGLIST_IO:
			*((boolean_t *)info) = sc->supports_sgio;
			break;
			
		default:
			return D_INVALID_OPERATION;

	}

	return D_SUCCESS;
}

#endif	NIPI > 0
