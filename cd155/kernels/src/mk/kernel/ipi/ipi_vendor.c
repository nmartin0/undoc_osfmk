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
 * $Log: ipi_vendor.c,v $
 * Revision 1.2  1994/11/18  20:51:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/08  16:54:09  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_vendor.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	IPI-3 driver vendor specific command interface.
 */

#include <ipi.h>
#if	NIPI > 0

#include <ipi/ipi_compat.h>
#include <ipi/ipi_endian.h>
#include <ipi/ipi_defs.h>
#include <ipi/ipi_map.h>
#include <ipi/ipi-3.h>

/*
 * Defines used for slave device vendor identification
 */
#define	VENDOR_UNKNOWN		0x00	/* Unknown vendor		      */
#define	VENDOR_GEN_4		0x01	/* Maximum Strategy Inc. GEN-4 System */


/*
 * Attempt to identify this vendor's device
 * Called during auto-configuration by the slave routine
 */
boolean_t
ipi_vendor(tgt, class)
	target_info_t	*tgt;
	int		*class;
{
	boolean_t	known_vendor = FALSE;
	cmd_queue_t	*cmd_q;
	ipi_vendor_t	*ven;
	int		ret, retry;

	/*
	 * Go ahead and initialize the vendor ID as unknown
	 */
	tgt->vendor = VENDOR_UNKNOWN;

	/* Get a command buffer */
	if ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL) {
		panic("ipi_vendor");
		return known_vendor;
	}
	cmd_q->ior = NULL;

	/*
	 * Get vendor ID information
	 */
	cmd_q->flags |= CMD_OPTIONAL;
	retry = 0;
	do {
		ret = ipi3_get_attribute(tgt, cmd_q, IPI_PARAM_VENDOR_ID);
	} while ((ret == IPI_RET_RETRY) && (retry++ <= ipi_retries));
	cmd_q->flags &= ~CMD_OPTIONAL;

	IPI_REL_CMD(cmd_q);	/* Free the command buffer */

	/* check for errors */
	if (ret != IPI_RET_SUCCESS) {
		printf(
"slave %d: Error determining vendor ID, using defaults\n",
			tgt->target_id);
		return known_vendor;
	}

	ven = (ipi_vendor_t *)(((ipi_response_t *)cmd_q->cmd_ptr) + 1);

	/*
	 * Parse through the vendor strings looking for a match
	 * If we find a matching vendor then we check the model
	 */

	/*
	 * Maximum Strategy, Inc.
	 */
	if (strncmp(ven->identification, "MAXIMUM STRATEGY", 16) == 0) {

		/*
		 * GEN-4 System
		 */
		if (strncmp(ven->model, "S-HIPPI ", 8) == 0) {
			/* We know this is a disk array */
			*class	     = IPI_DISK;
			tgt->vendor  = VENDOR_GEN_4;
			known_vendor = TRUE;
		}
	}

	return known_vendor;
}


/*
 * Disk device optimization
 */
ipi_vendor_disk_optimize(tgt)
	target_info_t	*tgt;
{
	cmd_queue_t		*cmd_q;
	int			ret;

	/* Get a command buffer */
	if ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL) {
		panic("ipi_vendor_disk_optimize");
		return IPI_RET_ERROR;
	}
	cmd_q->ior = NULL;

	switch (tgt->vendor) {

		case VENDOR_GEN_4:
		{
			ret = gen4_disk_optimize(tgt, cmd_q);
			break;
		}

		case VENDOR_UNKNOWN:
		default:
		{
			ret = IPI_RET_SUCCESS;
			break;
		}
	}

	IPI_REL_CMD(cmd_q);	/* Free the command buffer */
	return ret;
}


/*
 * Unit control (start/stop, lock/unlock, etc.)
 */
ipi_unit_control(tgt, ior, mode)
	target_info_t	*tgt;
	io_req_t	ior;
	unsigned char	mode;
{
	cmd_queue_t		*cmd_q;
	int			ret;

	/* Get a command buffer */
	while ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL);
	cmd_q->ior = ior;

	switch (tgt->vendor) {

		case VENDOR_GEN_4:
		{
			ret = gen4_unit_control(tgt, cmd_q, mode);
			break;
		}

		case VENDOR_UNKNOWN:
		default:
		{
			ipi_disk_modes_t	dmode;

			dmode.length   = IPI_PARAM_DISK_MODES_LEN;
			dmode.id       = IPI_PARAM_DISK_MODES;
			dmode.mode     = mode;
			dmode.reserved = 0;
			dmode.pad0     = 0;
			dmode.pad1     = 0;
			dmode.pad2     = 0;
			dmode.pad3     = 0;

			/* Too many don't support it.  Sigh. */
			cmd_q->flags |= CMD_OPTIONAL;
			ret = ipi3_operating_mode(tgt, cmd_q,
				&dmode, sizeof(ipi_disk_modes_t));
			cmd_q->flags &= ~CMD_OPTIONAL;

			break;
		}
	}

	IPI_REL_CMD(cmd_q);	/* Free the command buffer */
	return ret;
}


/*
 * Determine if the target is ready
 */
ipi_test_unit_ready(tgt, ior)
	target_info_t	*tgt;
	io_req_t	ior;
{
	cmd_queue_t	*cmd_q;
	int		ret;

	/* Get a command buffer */
	while ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL);
	cmd_q->ior = ior;

	switch (tgt->vendor) {

		case VENDOR_GEN_4:
		{
			ret = gen4_test_unit_ready(tgt, cmd_q);
			break;
		}

		case VENDOR_UNKNOWN:
		default:
		{
			ipi_condition_t	*condition;

			/*
			 * Get the condition parameter
			 * Too many don't support it.  Sigh.
			 */
			cmd_q->flags |= CMD_OPTIONAL;
			ret = ipi3_report_addressee_status(tgt, cmd_q,
				IPI_CMD_REPORT_ADDRESSEE_STATUS_CONDITION);
			cmd_q->flags &= ~CMD_OPTIONAL;

			if (ior && !(ior->io_op & IO_ERROR)) {
				condition = (ipi_condition_t *)
				    (((ipi_response_t *)cmd_q->cmd_ptr) + 1);

				/*
				 * We must be operational and ready
				 * See ISO IPI-3 spec section 4.7
				 */
				if (!condition->l_available)
					ret = IPI_RET_RETRY;
			}

			break;
		}
	}

	IPI_REL_CMD(cmd_q);	/* Free the command buffer */
	return ret;
}


/*
 * Report media access rights
 */
ipi_media_access(tgt, ior, access)
	target_info_t	*tgt;
	io_req_t	ior;
	int		*access;
{
	cmd_queue_t	*cmd_q;
	int		ret;

	/* Get a command buffer */
	while ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL);
	cmd_q->ior = ior;

	switch (tgt->vendor) {

		case VENDOR_GEN_4:
		{
			ret = gen4_media_access(tgt, cmd_q, access);
			break;
		}

		case VENDOR_UNKNOWN:
		default:
		{
			ipi_response_t		*rsp;
			ipi_media_status_t	*media_status;

			ret = ipi3_report_addressee_status(tgt, cmd_q,
				IPI_CMD_REPORT_ADDRESSEE_STATUS_STATUS);

			if (ior && !(ior->io_op & IO_ERROR)) {
				ret = IPI_RET_ERROR;
				break;
			}

			/*
			 * The ISO IPI-3 spec section 6.4.3 says that the
			 * response shall be a media status parameter and
			 * a vendor unique status parameter.
			 */
			rsp = (ipi_response_t *)cmd_q->cmd_ptr;

			media_status = (ipi_media_status_t *)
				ipi_param(rsp, NULL, IPI_PARAM_MEDIA_STATUS);

			if (media_status == NULL) {
				printf(
			    "Cannot locate parameter ID 0x%02x in response\n",
					IPI_PARAM_MEDIA_STATUS);
				return IPI_RET_ERROR;
			}

			/*
			 * Check for media present
			 */
			if (!media_status->media_present)
				*access = IPI_NO_ACCESS;

			/*
			 * Check for write protection
			 */
			else if (media_status->write_protect)
				*access = IPI_READ_ONLY;

			/*
			 * Full access
			 */
			else
				*access = IPI_READ_WRITE;

			break;
		}
	}

	IPI_REL_CMD(cmd_q);	/* Free the command buffer */
	return ret;
}


/*
 * Read capacity
 */
ipi_read_capacity(tgt, ior, capacity)
	target_info_t	*tgt;
	io_req_t	ior;
	ipi_num_block_t	**capacity;
{
	cmd_queue_t	*cmd_q;
	int		ret;

	/* Get a command buffer */
	while ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL);
	cmd_q->ior = ior;

	switch (tgt->vendor) {

		case VENDOR_GEN_4:
		{
			ret = gen4_read_capacity(tgt, cmd_q, capacity);
			break;
		}

		case VENDOR_UNKNOWN:
		default:
		{
			ipi_response_t	*rsp;

			/*
			 * Do an attributes command for parameter id 0x53
			 */
			ret = ipi3_get_attribute(tgt, cmd_q,
				IPI_PARAM_NUMBER_DISK_DATA_BLOCKS);

			rsp = (ipi_response_t *)cmd_q->cmd_ptr;
			*capacity = (ipi_num_block_t *)(rsp + 1);

			break;
		}
	}

	IPI_REL_CMD(cmd_q);	/* Free the command buffer */
	return ret;
}


/*
 * Display vendor unique error parameters which are not
 * indicated by the vendor unique major status bit
 */
ipi_vendor_error_code(tgt, rsp)
	target_info_t	*tgt;
	ipi_response_t 	*rsp;
{
	unsigned int	action;

	switch (tgt->vendor) {

		case VENDOR_GEN_4:
		{
			action = gen4_error_code(rsp);
			break;
		}

		case VENDOR_UNKNOWN:
		default:
		{
			action = 0;
			break;
		}
	}

	return action;
}


/*
 * Display vendor unique error substatus
 */
ipi_vendor_error_substatus(tgt, id, prm)
	target_info_t	*tgt;
	int		id;
	ipi_param_t	*prm;
{
	unsigned int	action;

	switch (tgt->vendor) {

		case VENDOR_GEN_4:
		case VENDOR_UNKNOWN:
		default:
		{
			unsigned char	*p;
			int		len, byte;

			action = ACTION_RETRY;

			printf("Vendor Unique %s Status:\n\t",
				((id >> 4) == 0x01) ? "Slave" : "Facility");

			p = &prm->id;
			len = prm->length;

			for (byte = 0; byte < len; byte++) {
				printf("0x%02x ", *p++);
				if ((byte % 8) == 7)
					printf("\n\t");
			}
			if ((byte % 8) != 0)
				printf("\n");

			break;
		}
	}

	return action;
}


/*
 * Display extended error substatus
 */
ipi_vendor_extended_substatus(tgt, id, prm)
	target_info_t	*tgt;
	int		id;
	ipi_param_t	*prm;
{
	unsigned int	action;

	switch (tgt->vendor) {

		case VENDOR_GEN_4:
		{
			action = gen4_extended_substatus(id, prm);
			break;
		}

		case VENDOR_UNKNOWN:
		default:
		{
			int	len, byte;

			action = ACTION_RETRY;

			printf("\tExtended Substatus:\n\t\t");

			len = prm->length - 1;

			for (byte = 4; byte < len; byte++) {
				printf("0x%02x ", prm->data[byte]);
				if ((byte % 8) == 7)
					printf("\n\t\t");
			}
			if ((byte % 8) != 0)
				printf("\n");

			break;
		}
	}

	return action;
}

#endif	NIPI > 0
