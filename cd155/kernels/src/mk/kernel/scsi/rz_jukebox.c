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
 * $Log: rz_jukebox.c,v $
 * Revision 1.3  1995/03/14  23:48:27  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.2  1994/11/18  20:59:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/09/24  22:52:00  jerrie
 * Initial check-in.
 *
 */
/*
 *	File:	rz_jukebox.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	08/93
 *
 *	Top layer of the SCSI driver: interface with the MI.
 *	This file contains operations specific to media changer devices.
 */

#include <mach/std_types.h>
#include <machine/machparam.h>		/* spl definitions */
#include <vm/vm_kern.h>
#include <scsi/compat_30.h>

#include <scsi/scsi.h>
#include <scsi/scsi2.h>
#include <scsi/scsi_defs.h>
#include <scsi/rz.h>

int scsi_jukebox_timeout = 5*60;	/* secs, slow when positioning */

/*
 * Specialized side of the open routine for jukeboxes
 */
scjb_open(tgt, req)
	target_info_t	*tgt;
	io_req_t	req;
{
	extern int	rz_open_timeout;
	scsi_ret_t	ret;
	io_req_t	ior;
	int		i;

#ifdef	MACH_KERNEL
	req->io_device->flag |= D_EXCL_OPEN;
#endif	/*MACH_KERNEL*/

	/* Preferably allow jukeboxes to disconnect */
	if (BGET(scsi_might_disconnect,tgt->masterno,tgt->target_id))
		BSET(scsi_should_disconnect,tgt->masterno,tgt->target_id);

	if (scsi_watchdog_period < scsi_jukebox_timeout)
		scsi_watchdog_period += scsi_jukebox_timeout;

	/*
	 * Dummy ior for proper sync purposes
	 */
	io_req_alloc(ior, 0);
        bzero(ior, sizeof(struct io_req));
	ior->io_unit = req->io_unit;
	ior->io_next = 0;
	ior->io_count = 0;
	ior->io_residual = 0;

	/*
	 * Bring the unit online, retrying if necessary.
	 * If the target is busy we wait for it.
	 */
	if (!(tgt->flags & TGT_ONLINE)) {

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

			if (i == 5) printf("%s%d: %s\n", 
					   (*tgt->dev_ops->driver_name)(TRUE),
					   tgt->unit_no,
					   "Waiting to come online..");
			timeout(wakeup, tgt, hz);
			await(tgt);
		}

		if (i == rz_open_timeout) {
			io_req_free(ior);
			return D_DEVICE_DOWN;
		}
	}

	return ret;
}

/*
 * Specialized side of the close routine for jukeboxes
 */
scjb_close(dev, tgt)
	int		dev;
	target_info_t	*tgt;
{
	io_return_t	ret = SCSI_RET_SUCCESS;

	if (tgt->ior) printf("JUKEBOX: Close with pending requests ?? \n");

	return ret;
}

/*
 * Jukebox strategy routine
 */
scjb_strategy(ior)
	register io_req_t	ior;
{
	register int	i = ior->io_unit;

	/*
	 * Don't do anything for a pass-through device, just acknowledge
	 * the request.  This gives us a null SCSI device.
	 */
	if (rzpassthru(i)) {
		ior->io_residual = 0;
		iodone(ior);
		return D_SUCCESS;
	}

	/*
	 * Cannot read/write a media changer device
	 */
	ior->io_error = D_INVALID_OPERATION;
	ior->io_op |= IO_ERROR;
	ior->io_residual = ior->io_count;
	iodone(ior);

	return;
}

/*
 * Start/completion routine for jukeboxes
 */
scjb_start(tgt, done)
	target_info_t	*tgt;
	boolean_t	done;
{
	register io_req_t	ior = tgt->ior;

	if (ior == 0)
		return;

	/*
	 * Cannot be called without done flag set
	 */
	assert(done);

	/* see if we must retry */
	if (tgt->done == SCSI_RET_RETRY) {
	} else

	/* got a bus reset ? ouch, that hurts */
	if (tgt->done == (SCSI_RET_ABORTED|SCSI_RET_RETRY)) {
		/* just mark it as a retry */
		tgt->done = SCSI_RET_RETRY;
	} else

	/* check completion status */

	if ((tgt->cur_cmd == SCSI_CMD_REQUEST_SENSE) &&
	    (!rzpassthru(ior->io_unit))) {
		scsi_sense_data_t *sns;

		ior->io_op    = ior->io_temporary;
		ior->io_error = D_IO_ERROR;
		ior->io_op   |= IO_ERROR;

		sns = (scsi_sense_data_t *)tgt->cmd_ptr;

		if (scsi_debug)
			scsi_print_sense_data(sns);

		if (scsi_check_sense_data(tgt, sns)) {
			if (sns->u.xtended.sense_key == SCSI_SNS_ILLEGAL_REQ &&
			    sns->u.xtended.add_bytes[4] == SCSI_ERR_NO_LUN) {
				printf("Logical unit not supported!\n");
				ior->io_error = D_NO_SUCH_DEVICE; 
				ior->io_op   &= ~IO_ERROR;
			}
		}
	} else

	if ((tgt->done != SCSI_RET_SUCCESS) &&
	    (!rzpassthru(ior->io_unit))) {

		if (tgt->done == SCSI_RET_NEED_SENSE) {
			ior->io_temporary = ior->io_op;
			ior->io_op	  = IO_INTERNAL;
			scsi_request_sense(tgt, ior, 0);
			return;
		}
	}

	/* dequeue next one */
	{
		io_req_t	next;

		simple_lock(&tgt->target_lock);
		next = ior->io_next;
		tgt->ior = next;
		simple_unlock(&tgt->target_lock);

		iodone(ior);
	}
}

#include <sys/ioctl.h>
#include <scsi/scsi_status.h>

struct mcread	scjb_mcread;

io_return_t
scjb_get_status(dev, tgt, flavor, status, status_count)
	int		dev;
	target_info_t	*tgt;
	int		flavor;
	dev_status_t	status;
	unsigned int	*status_count;
{
	scsi_ret_t	ret = SCSI_RET_SUCCESS;
	io_req_t	ior;

	io_req_alloc(ior, 0);
        bzero(ior, sizeof(struct io_req));
retry_get:
	ior->io_op	 = IO_INTERNAL;
	ior->io_unit	 = dev;
	ior->io_count	 = 0;
	ior->io_residual = 0;
	ior->io_error	 = 0;
	ior->io_next	 = 0;

	switch (flavor) {

#ifdef	MACH_KERNEL
		case DEV_GET_SIZE:
		{
			if (*status_count != DEV_GET_SIZE_COUNT) {
				io_req_free(ior);
				return D_INVALID_SIZE;
			}

			status[DEV_GET_SIZE_DEVICE_SIZE] = 0;
			status[DEV_GET_SIZE_RECORD_SIZE] = 0;
		}
		break;
#endif	MACH_KERNEL

		case MCCAP:
		{
			if (*status_count !=
			    sizeof(struct mccap)/sizeof(unsigned int)) {
				io_req_free(ior);
				return D_INVALID_SIZE;
			}

			ret = scsi_mode_sense(tgt, 31,
				sizeof(scsi_mode_sense_data_t) +
				sizeof(scsi_mode_sense_page31_t), ior);

			if (ret == SCSI_RET_SUCCESS) {
				scsi_mode_sense_data_t	*header;
				unsigned char		*ptr;
				int			len;

				header = (scsi_mode_sense_data_t *)tgt->cmd_ptr;

				/*
				 * Set a pointer to the start of the data
				 * Adjust it past the mode parameter header
				 * plus any block descriptor length
				 */
				ptr  = (unsigned char *)header + 4;
				ptr += header->bdesc_len;

				/*
				 * Determine the length of data to move
				 * Note that the data_len field does not
				 * include the size of the data_len field
				 */
				len  = header->data_len - 3;
				len -= header->bdesc_len;

				if (len >= *status_count * sizeof(unsigned int))
				    len = *status_count * sizeof(unsigned int);
				else
				    *status_count = len / sizeof(unsigned int);

				bcopy(ptr, (unsigned char *)status, len);
			}
		}
		break;

		case MCADDR:
		{
			if (*status_count !=
			    sizeof(struct mcaddr)/sizeof(unsigned int)) {
				io_req_free(ior);
				return D_INVALID_SIZE;
			}

			ret = scsi_mode_sense(tgt, 29,
				sizeof(scsi_mode_sense_data_t) +
				sizeof(scsi_mode_sense_page29_t), ior);

			if (ret == SCSI_RET_SUCCESS) {
				scsi_mode_sense_data_t	*header;
				unsigned char		*ptr;
				int			len;

				header = (scsi_mode_sense_data_t *)tgt->cmd_ptr;

				/*
				 * Set a pointer to the start of the data
				 * Adjust it past the mode parameter header
				 * plus any block descriptor length
				 */
				ptr  = (unsigned char *)header + 4;
				ptr += header->bdesc_len;

				/*
				 * Determine the length of data to move
				 * Note that the data_len field does not
				 * include the size of the data_len field
				 */
				len  = header->data_len - 3;
				len -= header->bdesc_len;

				if (len >= *status_count * sizeof(unsigned int))
				    len = *status_count * sizeof(unsigned int);
				else
				    *status_count = len / sizeof(unsigned int);

				bcopy(ptr, (unsigned char *)status, len);
			}
		}
		break;

		case MCGEO:
		{
			if (*status_count !=
			    sizeof(struct mcgeo)/sizeof(unsigned int)) {
				io_req_free(ior);
				return D_INVALID_SIZE;
			}

			ret = scsi_mode_sense(tgt, 30,
				sizeof(scsi_mode_sense_data_t) +
				sizeof(scsi_mode_sense_page30_t), ior);

			if (ret == SCSI_RET_SUCCESS) {
				scsi_mode_sense_data_t	*header;
				unsigned char		*ptr;
				int			len;

				header = (scsi_mode_sense_data_t *)tgt->cmd_ptr;

				/*
				 * Set a pointer to the start of the data
				 * Adjust it past the mode parameter header
				 * plus any block descriptor length
				 */
				ptr  = (unsigned char *)header + 4;
				ptr += header->bdesc_len;

				/*
				 * Determine the length of data to move
				 * Note that the data_len field does not
				 * include the size of the data_len field
				 */
				len  = header->data_len - 3;
				len -= header->bdesc_len;

				if (len >= *status_count * sizeof(unsigned int))
				    len = *status_count * sizeof(unsigned int);
				else
				    *status_count = len / sizeof(unsigned int);

				bcopy(ptr, (unsigned char *)status, len);
			}
		}
		break;

		case MCREAD:
		{
			if (*status_count !=
			    sizeof(struct mcread)/sizeof(unsigned int)) {
				io_req_free(ior);
				return D_INVALID_SIZE;
			}

			/*
			 * Not much to do here, everything was handled in
			 * scjb_set_status.  Just copy out the mcread structure.
			 */
			*(struct mcread *)status = *(&scjb_mcread);
		}
		break;

		default:
		{
			io_req_free(ior);
			return D_INVALID_OPERATION;
		}
	}

	if (ret == SCSI_RET_RETRY) {
		timeout(wakeup, ior, 5*hz);
		await(ior);
		goto retry_get;
	}

	if ((ret != SCSI_RET_SUCCESS) || ior->io_error) {
		io_req_free(ior); 
		return D_IO_ERROR;
	}

	io_req_free(ior);

	return D_SUCCESS;
}

io_return_t
scjb_set_status(dev, tgt, flavor, status, status_count)
	int		dev;
	target_info_t	*tgt;
	int		flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
	scsi_ret_t	ret;
	io_req_t	ior;

	io_req_alloc(ior, 0);
        bzero(ior, sizeof(struct io_req));
retry_set:
	ior->io_op	 = IO_INTERNAL;
	ior->io_unit	 = dev;
	ior->io_count	 = 0;
	ior->io_residual = 0;
	ior->io_error	 = 0;
	ior->io_next	 = 0;

	switch (flavor) {

		case MCINIT:
		{
			if (status_count !=
			    sizeof(unsigned int)/sizeof(unsigned int)) {
				io_req_free(ior);
				return D_INVALID_SIZE;
			}

			ret = scsi_init_element_status(tgt, ior);
		}
		break;

		case MCREAD:
		{
			struct mcread	*mcr = &scjb_mcread;

			if (status_count !=
			    sizeof(struct mcread)/sizeof(unsigned int)) {
				io_req_free(ior);
				return D_INVALID_SIZE;
			}

			*(&scjb_mcread) = *(struct mcread *)status;

			ior->io_data = (io_buf_ptr_t)mcr->data;
			ior->io_count = mcr->allocation_length;

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
			}

			ret = scsi_read_element_status(tgt,
				mcr->element_type,
				mcr->volume_tag,
				mcr->starting_element_address,
				mcr->number_of_elements,
				mcr->allocation_length,
				ior);

			/*
			 * Copy the data to the buffer
			 */
			if (ret == SCSI_RET_SUCCESS) {
				bcopy(tgt->cmd_ptr, mcr->data, ior->io_count);
			}

			if (scsi_softc[tgt->masterno]->supports_sgio) {
				/* Free the allocated buffers */
				kmem_free(kernel_map, ior->io_data,
					ior->io_alloc_size);
				io_sglist_free(ior->io_sgp);
			}
		}
		break;

		case MCPOSN:
		{
			struct mcposn	*mcp = (struct mcposn *)status;

			if (status_count !=
			    sizeof(struct mcposn)/sizeof(unsigned int)) {
				io_req_free(ior);
				return D_INVALID_SIZE;
			}

			ret = scsi_position_to_element(tgt,
				mcp->transport_address,
				mcp->destination_address,
				mcp->invert, ior);
		}
		break;

		case MCMOVE:
		{
			struct mcmove	*mcm = (struct mcmove *)status;

			if (status_count !=
			    sizeof(struct mcmove)/sizeof(unsigned int)) {
				io_req_free(ior);
				return D_INVALID_SIZE;
			}

			ret = scsi_move_medium(tgt,
				mcm->transport_address,
				mcm->source_address,
				mcm->destination_address,
				mcm->invert, ior);
		}
		break;

		case MCXCHG:
		{
			struct mcxchg	*mcx = (struct mcxchg *)status;

			if (status_count !=
			    sizeof(struct mcxchg)/sizeof(unsigned int)) {
				io_req_free(ior);
				return D_INVALID_SIZE;
			}

			ret = scsi_exchange_medium(tgt,
				mcx->transport_address,
				mcx->source_address,
				mcx->destination_address1,
				mcx->destination_address2,
				mcx->invert1, mcx->invert2, ior);
		}
		break;

		default:
		{
			io_req_free(ior);
			return D_INVALID_OPERATION;
		}
		break;
	}

	if (ret == SCSI_RET_RETRY) {
		timeout(wakeup, ior, 5*hz);
		await(ior);
		goto retry_set;
	}

	if ((ret != SCSI_RET_SUCCESS) || ior->io_error) {
		io_req_free(ior); 
		return D_IO_ERROR;
	}

	io_req_free(ior); 

	return D_SUCCESS;
}
