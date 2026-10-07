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
 * $Log: ipi_master.c,v $
 * Revision 1.6  1995/02/02  19:12:33  jerrie
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
 * Revision 1.5  1994/12/16  21:35:48  jerrie
 * Corrected problems with ioctl commands.  The expected size of the ioctl
 * argument parameters was not calculated correctly causing size checks in
 * the driver to fail.  In addition, if the IPI device had not previously
 * been probed, an uninitialized pointer was being used when changing the
 * facility and command reference numbers via ioctl calls.
 *
 *  Reviewer:  Arlin Davis
 *  Risk:	    Low.  Only affects ioctl's to IPI devices.
 *  Benefit or PTS #:  11392
 *  Testing:   Used the test case in the bug, and an Evaluation test to
 * 	    verify all IPI ioctl commands.  Tested before and after
 * 	    probing the device where applicable.
 *  Module(s): The following kernel files are being modified:
 * 		kernel/ipi/ipi_disk.c
 * 		kernel/ipi/ipi_master.c
 * 		kernel/ipi/ipi_misc.c
 * 		kernel/ipi/ipi_endian.h
 *
 * Revision 1.4  1994/11/18  20:51:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/10/04  16:21:37  jerrie
 * Added the DEV_GET_SIZE ioctl flavor to return the device partition size
 * and block size.  The partition size returned is zero since we know very
 * little about the device in this module.  This ioctl is required by the
 * server.
 *
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: PTS 10662 "panic: cipi_open.device_get_status failure
 * 	0x9c9" opening ipi-3 master devices
 *  Testing: Used the test case in the bug that detected the problem.
 * 	Ran Evaluation's HiPPI and IPI-3 EATs on a kernel with the
 * 	fixes in place.
 *  Module(s): ipi_master.c
 *
 * Revision 1.2  1994/06/22  18:07:35  arlin
 * ifield problems, must be in big indian format.
 *
 * Revision 1.1  1994/06/08  16:53:55  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_master.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	IPI-3 driver master interface.
 */

#include <ipi.h>
#if	NIPI > 0

#include <ipi/ipi_compat.h>
#include <ipi/ipi_endian.h>
#include <ipi/ipi_defs.h>
#include <ipi/ipi_map.h>


ipm_open(dev, mode, ior)
	int		dev;
	dev_mode_t	mode;
	io_req_t	ior;
{
	ipi_softc_t	*sc;
	target_info_t	*tgt;
	int		target_id;

	if (IPI_CONTROLLER(dev) >= NIPI ||
	    (sc = ipi_softc[IPI_CONTROLLER(dev)]) == 0)
		return D_NO_SUCH_DEVICE;

	target_id = IPI_SLAVE(dev);

	/*
	 * Make sure we have a target descriptor
	 */
	if (sc->target[target_id] == 0) {
		/* Allocate a target descriptor */
		tgt = ipi_slave_alloc(sc->masterno, target_id, sc->hw_state);

		/*
		 * Read bootmagic variables
		 */
		ipi_get_bootmagic(tgt);

		/*
		 * Don't bother to check for configuration errors
		 * Any errors can be corrected via the ioctls below
		 */
	} else {
		tgt = sc->target[target_id];

		/*
		 * Don't bother to check for configuration errors
		 * Any errors can be corrected via the ioctls below
		 */
	}

	tgt->open_count++;

	return D_SUCCESS;
}


ipm_close(dev)
	int		dev;
{
	ipi_softc_t	*sc;
	target_info_t	*tgt;

	if (IPI_CONTROLLER(dev) >= NIPI ||
	    (sc = ipi_softc[IPI_CONTROLLER(dev)]) == 0)
		return D_NO_SUCH_DEVICE;

	tgt = sc->target[IPI_SLAVE(dev)];

	tgt->open_count--;

	return D_SUCCESS;
}


#include <sys/ioctl.h>
#include <ipi/ipi_status.h>

ipm_get_status(dev, flavor, status, status_count)
	int		dev;
	unsigned int	flavor;
	dev_status_t	status;
	unsigned int	*status_count;
{
	target_info_t	*tgt;

	tgt = ipi_softc[IPI_CONTROLLER(dev)]->target[IPI_SLAVE(dev)];

	if (ipi_debug)
		printf("ipm_get_status: 0x%x 0x%x 0x%x 0x%x\n",
			dev, flavor, status, *status_count);

	switch (flavor) {

		case DEV_GET_SIZE:
		{
			if (*status_count != DEV_GET_SIZE_COUNT)
				return D_INVALID_SIZE;

			status[DEV_GET_SIZE_DEVICE_SIZE] = 0;
			status[DEV_GET_SIZE_RECORD_SIZE] = tgt->block_size;
			break;
		}

		case IPIPHYS:
		{
			struct ipi_phys	*phys = (struct ipi_phys *)status;

			if (*status_count !=
			    sizeof(struct ipi_phys) / sizeof(int))
				return D_INVALID_SIZE;

			/*
			 * Return physical information for the device
			 */
			phys->node	 = node_self();
			phys->controller = IPI_CONTROLLER(dev);
			phys->slave	 = IPI_SLAVE(dev);
			phys->facility	 = IPI_FACILITY(dev);

			break;
		}

		case IPIGIFIELD:
		{
			if (*status_count != sizeof(int) / sizeof(int))
				return D_INVALID_SIZE;

			/*
			 * Return the connection control information (I-field)
			 *
			 * NOTE: tgt->i_field is in big endian format
			 *	 Return it in little endian format
			 */
			*(unsigned int *)status = htonl(tgt->i_field);

			break;
		}

		case IPIGFACILITY:
		{
			if (*status_count != sizeof(int) / sizeof(int))
				return D_INVALID_SIZE;

			/*
			 * Return the facility base
			 */
			*(unsigned int *)status = tgt->facility;

			break;
		}

		case IPIGPART:
		{
			if (*status_count != sizeof(int) / sizeof(int))
				return D_INVALID_SIZE;

			/*
			 * Return the facility partition
			 */
			*(unsigned int *)status = tgt->partition;

			break;
		}

		case IPIGCMDREF:
		{
			struct ipi_cmd_ref *ref = (struct ipi_cmd_ref *)status;

			if (*status_count !=
			    sizeof(struct ipi_cmd_ref) / sizeof(int))
				return D_INVALID_SIZE;

			/*
			 * Return the min and max command reference numbers
			 */
			ref->min = tgt->cmd_ref_min;
			ref->max = tgt->cmd_ref_min + tgt->cmd_ref_len - 1;

			break;
		}

		case IPIGSTATS:
		{
			struct ipi_stats *stats = (struct ipi_stats *)status;

			if (*status_count !=
			    sizeof(struct ipi_stats) / sizeof(int))
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

			break;
		}

		default:
 			return D_INVALID_OPERATION;
	}

	return D_SUCCESS;
}


ipm_set_status(dev, flavor, status, status_count)
	int		dev;
	unsigned int	flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
	target_info_t	*tgt;
	ipi_ret_t	ret;

	tgt = ipi_softc[IPI_CONTROLLER(dev)]->target[IPI_SLAVE(dev)];

	if (ipi_debug)
		printf("ipm_set_status: 0x%x 0x%x 0x%x 0x%x\n",
		       dev, flavor, status, status_count);

	switch (flavor) {

		case IPISIFIELD:
		{
			unsigned int	old_i_field;
			int		i;

			if (tgt->open_count != 1)
				return D_ALREADY_OPEN;

			if (status_count != sizeof(int) / sizeof(int))
				return D_INVALID_SIZE;

			/*
			 * Set the connection control information (I-field)
			 * Save the old value just in case
			 *
			 * NOTE: tgt->i_field must be in big endian format
			 */
			old_i_field = tgt->i_field;
			tgt->i_field = htonl(*(unsigned int *)status);

			/*
			 * Verify the new value
			 *
			 * Check for I-field conflicts with other open
			 * targets first, then display any warnings
			 */
			/* I-field Errors */
			if ((ret = ipi_config_check(tgt, flavor, FALSE)) !=
			    D_SUCCESS) {
				/* Restore the old value */
				tgt->i_field = old_i_field;
				return ret;
			}
			/* I-field Warnings */
			if ((ret = ipi_config_check(tgt, flavor, TRUE)) !=
			    D_SUCCESS) {
				/* Restore the old value */
				tgt->i_field = old_i_field;
				return ret;
			}

			/*
			 * Update the source channel request structures
			 */
			if (tgt->cmd_q) {
			    cmd_queue_t	*cmd_q = tgt->cmd_q;

			    for (i = 0; i < tgt->cmd_ref_len; i++, cmd_q++) {
				hctlr_src_t	*sreq;

				sreq = &cmd_q->sreq;
				sreq->i_field = tgt->i_field;
			    }
			}

			break;
		}

		case IPISFACILITY:
		{
			unsigned int	old_facility;

			if (tgt->open_count != 1)
				return D_ALREADY_OPEN;

			if (status_count != sizeof(int) / sizeof(int))
				return D_INVALID_SIZE;

			/*
			 * Set the facility base
			 * Save the old value just in case
			 */
			old_facility = tgt->facility;
			tgt->facility = *(unsigned char *)status;

			/*
			 * Verify the new value
			 */
			if ((ret = ipi_config_check(tgt, flavor, FALSE)) !=
			    D_SUCCESS) {
				/* Restore the old value */
				tgt->facility = old_facility;
				return ret;
			}

			break;
		}

		case IPISPART:
		{
			unsigned char	old_partition;

			if (tgt->open_count != 1)
				return D_ALREADY_OPEN;

			if (status_count != sizeof(int) / sizeof(int))
				return D_INVALID_SIZE;

			/*
			 * Set the facility partition
			 * Save the old value just in case
			 */
			old_partition = tgt->partition;
			tgt->partition = *(unsigned char *)status;

			/*
			 * Verify the new value
			 */
			if ((ret = ipi_config_check(tgt, flavor, FALSE)) !=
			    D_SUCCESS) {
				/* Restore the old value */
				tgt->partition = old_partition;
				return ret;
			}

			break;
		}

		case IPISCMDREF:
		{
			ipi_softc_t	   *sc = ipi_softc[IPI_CONTROLLER(dev)];
			struct ipi_cmd_ref *ref = (struct ipi_cmd_ref *)status;
			unsigned short	   old_min, old_len;

			if (tgt->open_count != 1)
				return D_ALREADY_OPEN;

			if (status_count !=
			    sizeof(struct ipi_cmd_ref) / sizeof(int))
				return D_INVALID_SIZE;

			/*
			 * Set the min command reference number and length
			 * Save the old values just in case
			 */
			old_min = tgt->cmd_ref_min;
			old_len = tgt->cmd_ref_len;
			tgt->cmd_ref_min = ref->min;
			tgt->cmd_ref_len = ref->max - ref->min + 1;

			/*
			 * Verify the new values
			 */
			if ((ret = ipi_config_check(tgt, flavor, FALSE)) !=
			    D_SUCCESS) {
				/* Restore the old values */
				tgt->cmd_ref_min = old_min;
				tgt->cmd_ref_len = old_len;
				return ret;
			}

			/*
			 * Deallocate/reallocate target command queues
			 */
			ipi_queue_free(tgt);
			if (ipi_queue_alloc(tgt) != D_SUCCESS) {
				tgt->flags &= ~TGT_ALIVE;
				return D_DEVICE_DOWN;
			}

			break;
		}

		case IPISSTATS:
		{
			if (status_count !=
			    sizeof(struct ipi_stats) / sizeof(int))
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

#if	0
		case NSLXFER:
		{
			struct nsl_xfer *xfer = (struct nsl_xfer *)status;
			kern_return_t	ret;
			boolean_t	split;
			io_req_t	ior;

			if (status_count !=
			    sizeof(struct nsl_xfer) / sizeof(int))
				return D_INVALID_SIZE;

			if ((xfer->dir != NSL_READ) &&
			    (xfer->dir != NSL_WRITE)
				return D_INVALID_OPERATION;

			/*
			 * Allocate an ior and package it for the transfer
			 */
			io_req_alloc(ior, 0);

			ior->io_unit	= dev;
			ior->io_op	= (xfer->dir == NSL_READ) ?
				(IO_READ  | IO_INTERNAL | IO_OVERWRITE) :
				(IO_WRITE | IO_INTERNAL);
			ior->io_count	= (long)xfer->len;
			ior->io_total	= (long)xfer->len;
			ior->io_residual= 0;
			ior->io_data	= 0;
			ior->io_uaddr	= (vm_address_t)buf; /* user address */
			ior->io_error	= 0;
			ior->io_done	= 0;
			ior->io_next	= 0;
			ior->io_prev	= 0;

			/*
			 * If reading, allocate memory
			 * If writing, wire down the incoming memory
			 */
			split = FALSE;
			if (ior->io_op & IO_READ)
				/* XXX will this work for overwrites? XXX */
				ret = device_read_alloc(ior,
					(vm_size_t)ior->io_count);
			else
				ret = device_write_get(ior, &split);

			if (ret != KERN_SUCCESS) {
				io_req_free(ior);
				return ret;
			}

			/*
			 * Write operations cannot be split
			 */
			if (split) {
				/* Reset len to the amount we can do */
				xfer->len = ior->io_count;

				io_sglist_free(ior->io_sgp);
				io_req_free(ior);
				return D_INVALID_SIZE;
			}

			break;
		}
#endif

		default:
 			return D_INVALID_OPERATION;
	}

	return D_SUCCESS;
}

#endif	NIPI > 0
