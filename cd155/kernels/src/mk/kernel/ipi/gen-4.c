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
 * $Log: gen-4.c,v $
 * Revision 1.2  1994/11/18  20:50:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/08  16:53:19  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	gen-4.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Maximum Strategy GEN-4 system command specific interface.
 */

#include <ipi.h>
#if	NIPI > 0

#include <ipi/ipi_compat.h>
#include <ipi/ipi_endian.h>
#include <ipi/ipi_defs.h>
#include <ipi/ipi_map.h>
#include <ipi/ipi-3.h>
#include <ipi/gen-4.h>


/*
 * GEN-4 Device Optimization
 */
gen4_disk_optimize(tgt, cmd_q)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
{
	gen_4_port_data_t	*port;
	int			ret, retry;

	/*
	 * Send an ATTRIBUTES command with parameter id 0xe2.
	 */
	retry = 0;
	do {
		ret = ipi3_get_attribute(tgt, cmd_q, GEN_4_PORT_DATA_PARAM);
	} while ((ret == IPI_RET_RETRY) && (retry++ <= ipi_retries));

	/* check for errors */
	if (ret != IPI_RET_SUCCESS) {
		printf("\nslave %d: Error determining port configuration\n",
			tgt->target_id);
		tgt->flags = 0;
		return ret;
	}

	port = (gen_4_port_data_t *)(((ipi_response_t *)cmd_q->cmd_ptr) + 1);

	/*
	 * Configure the GEN-4 to our liking
	 */

	/*
	 * We will leave the default facility and default port numbers alone.
	 * The system administrator should have set these up correctly.
	 * Besides, the user can always specify the facility using ioctl's.
	 */

	/*
	 * We need the mode of this port to be command/data
	 */
	port->mode = GEN_4_PORT_COMMAND_DATA;

	/*
	 * For the IFP control flags, we want full first bursts for the best
	 * read performance (because our H/W doesn't support short burst
	 * tagging) and stay connected to be off.  We will disable the
	 * special 4k header and the no header bursts modes.  All other
	 * bits remain unchanged.
	 */
	port->full_first_burst = TRUE;		/* pad out short bursts	   */
	port->stay_connected   = FALSE;		/* drop the REQUEST signal */
	port->four_k_header    = FALSE;		/* disable 4k headder mode */
	port->no_header	       = FALSE;		/* send headers too	   */

	port->length = GEN_4_PORT_DATA_PARAM_LEN;

	retry = 0;
	do {
		ret = ipi3_set_attribute(tgt, cmd_q,
			port, sizeof(gen_4_port_data_t));
	} while ((ret == IPI_RET_RETRY) && (retry++ <= ipi_retries));

	/* check for errors */
	if (ret != IPI_RET_SUCCESS) {
		printf("\nslave %d: Error setting port configuration\n",
			tgt->target_id);
		tgt->flags = 0;
		return ret;
	}

	return IPI_RET_SUCCESS;
}


/*
 * GEN-4 Unit control (start/stop)
 */
gen4_unit_control(tgt, cmd_q, mode)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	unsigned char	mode;
{
	unsigned char		saved_facility;
	io_req_t		ior = cmd_q->ior;
	gen_4_vendor_id_t	*ven;
	unsigned short		module_presense;
	register int		i;

	/*
	 * Get vendor ID information and save the module presense fields.
	 * The GEN-4 vendor ID parameter can only be addressed to the slave.
	 * To work around this limitation we save the target facility number
	 * and create a value that will cause the command to be addressed to
	 * the slave only.  When the command is complete we restore the old
	 * value.
	 */
	saved_facility = tgt->facility;
	tgt->facility = 0;
	tgt->facility = IPI_SLAVE_ONLY - IPI_FACILITY(ior->io_unit); 

	ipi3_get_attribute(tgt, cmd_q, IPI_PARAM_VENDOR_ID);

	tgt->facility = saved_facility;

	/* check for errors */
	if (ior && (ior->io_op & IO_ERROR))
		return IPI_RET_ERROR;

	ven = (gen_4_vendor_id_t *)(((ipi_response_t *)cmd_q->cmd_ptr) + 1);

	module_presense = (ven->module_presense_msb << 8) |
			  (ven->module_presense_lsb << 0);

	/*
	 * Only spin up and down are supported by the
	 * GEN-4 and both require different parameters
	 */

	if (mode & SPIN_UP) {
		gen_4_spin_up_t		spin;

		spin.disk_modes_len = GEN_4_DISK_MODES_PARAM_LEN;
		spin.disk_modes_id  = GEN_4_DISK_MODES_PARAM;
		spin.mode	    = SPIN_UP;
		spin.reserved	    = 0;

		spin.module_sel_len = GEN_4_MODULE_SELECT_PARAM_LEN;
		spin.module_sel_id  = GEN_4_MODULE_SELECT_PARAM;

		/*
		 * Per the GEN-4 IPI-3 Manual, there must be one
		 * and only one bit set in module select parameter
		 */

		/* Do the lsb first */
		spin.module_sel_msb = 0;
		for (i = 0; i < 8; i++) {

			/* skip non-existant modules */
			if (!(module_presense & (1 << i)))
				continue;	

			spin.module_sel_lsb = 1 << i;

			ior->io_error = 0;
			ior->io_op    = IO_INTERNAL;
			ipi3_operating_mode(tgt, cmd_q,
				&spin, sizeof(gen_4_spin_up_t));

			/* check for errors */
			if (ior && (ior->io_op & IO_ERROR))
				return IPI_RET_ERROR;
		}

		/* Now do the msb */
		spin.module_sel_lsb = 0;
		for (i = 8; i < 12; i++) {

			/* skip non-existant modules */
			if (!(module_presense & (1 << i)))
				continue;	

			spin.module_sel_msb = 1 << (i - 8);

			ior->io_error = 0;
			ior->io_op    = IO_INTERNAL;
			ipi3_operating_mode(tgt, cmd_q,
				&spin, sizeof(gen_4_spin_up_t));

			/* check for errors */
			if (ior && (ior->io_op & IO_ERROR))
				return IPI_RET_ERROR;
		}
	}

	if (mode & SPIN_DOWN) {
		gen_4_spin_down_t	spin;

		spin.disk_modes_len = GEN_4_DISK_MODES_PARAM_LEN;
		spin.disk_modes_id  = GEN_4_DISK_MODES_PARAM;
		spin.mode	    = SPIN_DOWN;
		spin.reserved0	    = 0;

		spin.module_sel_len = GEN_4_MODULE_SELECT_PARAM_LEN;
		spin.module_sel_id  = GEN_4_MODULE_SELECT_PARAM;

		spin.device_sel_len = GEN_4_DEVICE_SELECT_PARAM_LEN;
		spin.device_sel_id  = GEN_4_DEVICE_SELECT_PARAM;
		spin.reserved1	    = 0;
		spin.device_select  = GEN_4_DEVICE(ior->io_unit);
		spin.pad0	    = 0;
		spin.pad1	    = 0;
		spin.pad2	    = 0;
		spin.pad3	    = 0;

		/*
		 * There must be one and only one
		 * bit set in module select parameter
		 */

		/* Do the lsb first */
		spin.module_sel_msb = 0;
		for (i = 0; i < 8; i++) {

			/* skip non-existant modules */
			if (!(module_presense & (1 << i)))
				continue;	

			spin.module_sel_lsb = 1 << i;

			ior->io_error = 0;
			ior->io_op    = IO_INTERNAL;
			ipi3_operating_mode(tgt, cmd_q,
				&spin, sizeof(gen_4_spin_up_t));

			/* check for errors */
			if (ior && (ior->io_op & IO_ERROR))
				return IPI_RET_ERROR;
		}

		/* Now do the msb */
		spin.module_sel_lsb = 0;
		for (i = 8; i < 12; i++) {

			/* skip non-existant modules */
			if (!(module_presense & (1 << i)))
				continue;	

			spin.module_sel_msb = 1 << i;

			ior->io_error = 0;
			ior->io_op    = IO_INTERNAL;
			ipi3_operating_mode(tgt, cmd_q,
				&spin, sizeof(gen_4_spin_up_t));

			/* check for errors */
			if (ior && (ior->io_op & IO_ERROR))
				return IPI_RET_ERROR;
		}
	}

	return IPI_RET_SUCCESS;
}


/*
 * Determine if the target is ready
 */
gen4_test_unit_ready(tgt, cmd_q)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
{
	io_req_t	ior = cmd_q->ior;
	ipi_response_t	*rsp = (ipi_response_t *)cmd_q->cmd_ptr;
	gen_4_status_t	*status;

	/*
	 * Get the status parameter
	 */
	ipi3_report_addressee_status(tgt, cmd_q,
		IPI_CMD_REPORT_ADDRESSEE_STATUS_STATUS);

	/* check for errors */
	if (ior && (ior->io_op & IO_ERROR))
		return IPI_RET_ERROR;

	status = (gen_4_status_t *)ipi_param(rsp, NULL, GEN_4_STATUS_PARAM);
	if (status == NULL) {
		printf("Cannot locate parameter ID 0x%02x in response\n",
			GEN_4_STATUS_PARAM);
		return IPI_RET_ERROR;
	}

	/*
	 * If the facility is attached, mounted, and
	 * we have some access then the unit is ready
	 */
	if (status->attached && status->mounted &&
	    (status->access != GEN_4_NO_ACCESS))
		return IPI_RET_SUCCESS;

	return IPI_RET_RETRY;
}


/*
 * Report media access rights
 */
gen4_media_access(tgt, cmd_q, access)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	int		*access;
{
	io_req_t	ior = cmd_q->ior;
	ipi_response_t	*rsp = (ipi_response_t *)cmd_q->cmd_ptr;
	gen_4_status_t	*status;

	/*
	 * Get the status parameter
	 */
	ipi3_report_addressee_status(tgt, cmd_q,
		IPI_CMD_REPORT_ADDRESSEE_STATUS_STATUS);

	/* check for errors */
	if (ior && (ior->io_op & IO_ERROR))
		return IPI_RET_ERROR;

	status = (gen_4_status_t *)ipi_param(rsp, NULL, GEN_4_STATUS_PARAM);
	if (status == NULL) {
		printf("Cannot locate parameter ID 0x%02x in response\n",
			GEN_4_STATUS_PARAM);
		return IPI_RET_ERROR;
	}

	if (status->access == GEN_4_NO_ACCESS)
		*access = IPI_NO_ACCESS;
	else if (status->access == GEN_4_READ_ONLY)
		*access = IPI_READ_ONLY;
	else if (status->access == GEN_4_WRITE_ONLY)
		*access = IPI_WRITE_ONLY;
	else if (status->access == GEN_4_READ_WRITE)
		*access = IPI_READ_WRITE;

	return IPI_RET_SUCCESS;
}


/*
 * Read capacity
 */
gen4_read_capacity(tgt, cmd_q, capacity)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	ipi_num_block_t	**capacity;
{
	io_req_t		ior = cmd_q->ior;
	hippi_fp_t		*hdr = cmd_q->hdr_ptr;
	ipi_cmd_attributes_t	*cmd = (ipi_cmd_attributes_t *)cmd_q->cmd_ptr;
	ipi_partition_param_t	*prt = (ipi_partition_param_t *)(cmd + 1);
	gen_4_read_capacity_t	*cap = (gen_4_read_capacity_t *)(prt + 1);
	ipi_response_t		*rsp;

	/* build the HiPPI-FP header */
	hdr->ulp_id	 = ULP_ID_IPI_3_SLAVE;
	hdr->p		 = TRUE;
	hdr->b		 = FALSE;
	hdr->d1_size_msb = ((sizeof(ipi_cmd_attributes_t)  +
			     sizeof(ipi_partition_param_t) +
			     sizeof(gen_4_read_capacity_t)) / 8) >>
				D1_SIZE_MSB_SHIFT;
	hdr->d1_size_lsb = ((sizeof(ipi_cmd_attributes_t)  +
			     sizeof(ipi_partition_param_t) +
			     sizeof(gen_4_read_capacity_t)) / 8) >>
				0;
	hdr->d2_offset	 = 0;
	hdr->d2_size_msb = 0;	/* no data */
	hdr->d2_size_b2  = 0;
	hdr->d2_size_b1  = 0;
	hdr->d2_size_lsb = 0;

	/* build the IPI-3 Attributes command */
	cmd->packet_length_msb = ((sizeof(ipi_cmd_attributes_t)  +
				   sizeof(ipi_partition_param_t) +
				   sizeof(gen_4_read_capacity_t)) - 2) >> 8;
	cmd->packet_length_lsb = ((sizeof(ipi_cmd_attributes_t)  +
				   sizeof(ipi_partition_param_t) +
				   sizeof(gen_4_read_capacity_t)) - 2) >> 0;
	cmd->reference_msb     = cmd_q->cmd_ref >> 8;
	cmd->reference_lsb     = cmd_q->cmd_ref >> 0;
	cmd->opcode	       = IPI_CMD_ATTRIBUTES;
	cmd->modifier	       = IPI_CMD_ATTRIBUTES_REPORT;
	cmd->slave	       = tgt->target_id;
	cmd->facility	       = (ior) ? IPI_FACILITY(ior->io_unit) :
					 IPI_SLAVE_ONLY;

	/* select the disk partition */
	prt->length	       = IPI_PARAM_PARTITION_LEN;
	prt->id		       = IPI_PARAM_PARTITION;
	prt->reserved	       = 0;
	prt->partition	       = tgt->partition;

	/* select the number of disk data blocks parameter (0x53) */
	cap->length	       = 3;
	cap->id		       = IPI_PARAM_REQUEST_PARAM;
	cap->control	       = PARAMS_IN_RESPONSE;
	cap->param_id	       = IPI_PARAM_NUMBER_DISK_DATA_BLOCKS;

	/* set data direction */
	cmd_q->dir = NO_DATA;

	/* set length of first burst */
	cmd_q->sreq.fb_len = sizeof(hippi_fp_t)		   +
			     sizeof(ipi_cmd_attributes_t)  +
			     sizeof(ipi_partition_param_t) +
			     sizeof(gen_4_read_capacity_t);
	assert((cmd_q->sreq.fb_len % 8) == 0);	/* per HiPPI-FP 7.2.2 */

	/* initialize the ior and scatter/gather pointers */
	cmd_q->sreq.ior    = ior;
	cmd_q->sreq.sg_ptr = NULL;

	IPI_GO_AND_WAIT(tgt, cmd_q);

	rsp = (ipi_response_t *)cmd_q->cmd_ptr;
	prt = (ipi_partition_param_t *)(rsp + 1);
	*capacity = (ipi_num_block_t *)(prt + 1);

	return cmd_q->result;
}


/*
 * Display GEN-4 specific error parameters
 * The Maximum Strategy GEN-4 does not set the vendor unique major status bit
 */
gen4_error_code(rsp)
	ipi_response_t 	*rsp;
{
	ipi_param_t	*prm;
	unsigned int	action = 0;

	/*
	 * Locate the Module Message Parameter (0xd8)
	 */
	prm = (ipi_param_t *)ipi_param(rsp, NULL, GEN_4_MODULE_MSG_PARAM);
	if (prm != NULL) {
		gen_4_module_msg_t	*mm;

		mm = (gen_4_module_msg_t *)prm;

		printf("Maximum Strategy GEN-4 Module Message:\n");

		if (mm->status == GEN_4_NO_CORRECTION)
			printf("\tNo correction attempted\n");
		else
		if ((mm->status & GEN_4_RETRY_MASK) == GEN_4_RETRY_CORRECTION)
			printf("\tRetry correction attempted\n");
		else
		if ((mm->status & GEN_4_ECC_MASK) == GEN_4_ECC_CORRECTION)
			printf("\tECC correction attempted\n");

		/* Check if this was a unsuccessful message */
		if (mm->length > GEN_4_SUCCESSFUL_LENGTH) {

			action |= ACTION_ERROR;

			printf("\tBank %d Device %d: ", mm->bank, mm->device);
			gen4_decode_module_error(mm->error_code);

			printf("\tBlock Count: %d\n",
				(mm->block_count_msb << 8) |
				(mm->block_count_lsb << 0));
		}
	}

	/*
	 * Locate the IFP Fatal Error Parameter (0xd9)
	 */
	prm = (ipi_param_t *)ipi_param(rsp, NULL, GEN_4_IFP_FATAL_ERROR_PARAM);
	if (prm != NULL) {
		gen_4_ifp_fatal_error_t	*ifp;
		unsigned short		module;
		unsigned short		error;

		ifp = (gen_4_ifp_fatal_error_t *)prm;

		action |= ACTION_ERROR;

		printf("Maximum Strategy GEN-4 IFP Fatal Error:\n");

		printf("\tIFP Port Number %d\n", ifp->port_number);

		module = (ifp->module_mask_msb << 8) |
			 (ifp->module_mask_lsb << 0);

		if (module) {
			int	i;
			int	first = TRUE;

			printf("\tError detected to/from module(s):");
			for (i = 0; i <= 0x0c; i++) {
				if (module & (1 << i)) {
					printf("%s %x", (first) ? "" : ",",
						i + 1);
					first = FALSE;
				}
			}
			printf("\n");
		}

		error = (ifp->in_error_msb << 8) | (ifp->in_error_lsb << 0);
		gen4_decode_in_chan_error(error);

		error = (ifp->out_error_msb << 8) | (ifp->out_error_lsb << 0);
		gen4_decode_out_chan_error(error);

		error = (ifp->ifp_error_msb << 8) | (ifp->ifp_error_lsb << 0);
		gen4_decode_ifp_error(error);
	}

	return action;
}


/*
 * Display GEN-4 extended error substatus
 */
gen4_extended_substatus(id, prm)
	int		id;
	ipi_param_t	*prm;
{
	int		len, byte, bit;
	unsigned char   p;
	unsigned int	action = 0;

	len = prm->length - 1;

	for (byte = 5; byte < len; byte++) {
		p = prm->data[byte - 1];

		for (bit = 0; bit <= 7; bit++) {
			if (p & (1 << bit))
			    action |=
				gen4_decode_extended_substatus(id, byte, bit);
		}
	}

	return action;
}


/*
 * Table of the Maximum Strategy GEN-4 module error codes
 *	Taken from the "Maximum Strategy, Inc.  IPI-3 Manual Revision 1.02"
 *	Number: 1017
 *	Dated:  9-20-93
 *	Appendix A: Module Error Codes
 */
static struct gen4_module_error_codes {
	unsigned char	code;
	char		*means;
} gen4_module_error[] = {

    { 0xff,	"Device interface controller time-out" },

    { 0xfb,	"invalid start block no." },
    { 0xfa,	"invalid block count" },
    { 0xf8,	"invalid command parameter" },
    { 0xf7,	"invalid count field (RdConfig, WrConfig)" },
    { 0xf6,	"invalid bank number" },
    { 0xf5,	"invalid device ID number" },
    { 0xf4,	"invalid disk number" },
    { 0xf3,	"invalid channel number" },
    { 0xf2,	"invalid buffer offset" },
    { 0xef,	"unknown transaction ID (not in use)" },
    { 0xee,	"unknown bank number or bank not mounted" },
    { 0xee,	"device not mounted" },

    { 0xcf,	"bank write-protect" },
    { 0xcd,	"bank not on-line" },
    { 0xcc,	"data recovery not possible" },
    { 0xcb,	"bank busy" },

    { 0xbf,	"CPU detected parity error" },
    { 0xbe,	"DMC data detected parity error" },
    { 0xbd,	"disk fault" },
    { 0xbc,	"device not ready" },
    { 0xbb,	"device repeated seek failure" },
    { 0xb9,	"invalid or missing raw flaw table(s)" },
    { 0xb8,	"format abort--verify failed" },
    { 0xb6,	"too many bad tracks" },
    { 0xb5,	"flaw table overflow" },
    { 0xb0,	"IML checksum error" },
    { 0xaf,	"interface reported error (equipment check error)" },
    { 0xae,	"ECC data correction failed" },
    { 0xad,	"missing address mark" },
    { 0xac,	"no data sync" },
    { 0xa9,	"header CRC error or sector not found" },
    { 0xa8,	"verify header mis-compare" },
    { 0xa1,	"terminated from Abort Command" },
    { 0xa0,	"unrecoverable h/w error" },

    { 0x8e,	"Data Channel time-out" },
    { 0x8d,	"seek time-out" },
    { 0x8c,	"device interface controller time-out" },

    { 0x70,	"missed sector" },
    { 0x71,	"header timeout" },
    { 0x72,	"header no \"successful information transfer\"" },
    { 0x73,	"invalid step head" },
    { 0x74,	"no start sector found" },

    { 0x44,	"unknown interrupt" },
    { 0x43,	"fifo full" },
    { 0x42,	"stack full" },
    { 0x41,	"address overflow" },
    { 0x40,	"fifo exception" },
    { 0x31,	"code exception" },
    { 0x30,	"breakpoint" },
    { 0x27,	"data fifo not ready" },
    { 0x26,	"header lost" },
    { 0x25,	"data timeout" },
    { 0x24,	"header timeout" },
    { 0x23,	"verify error" },
    { 0x22,	"uncertain track" },
    { 0x21,	"dead track" },
    { 0x20,	"ECC error" },
    { 0x19,	"control ram address parity error" },
    { 0x18,	"control ram data parity error" },
    { 0x17,	"device_write channel parity error" },
    { 0x16,	"device_read channel parity error" },
    { 0x15,	"buffer parity error" },
    { 0x14,	"interlock error" },
    { 0x13,	"no successful information transfer" },
    { 0x12,	"no slave" },
    { 0x11,	"invalid slave" },
    { 0x10,	"select timeout" },
    { 0x03,	"not implemented" },
    { 0x02,	"invalid opcode" },
    { 0x01,	"invalid checksum" },
    { 0x00,	"transfer aborted" },

    { 0,	0 }
};

gen4_decode_module_error(code)
	int	code;
{
	register struct gen4_module_error_codes	*err;

	for (err = gen4_module_error; err->means; err++) {
		if (err->code != code)
			continue;

		printf("%s\n", err->means);
		return;
	}

	printf("Error code 0x%02x\n", code);
}


/*
 * Table of the Maximum Strategy GEN-4 IN-Chan error codes
 *	Taken from the "Maximum Strategy, Inc.  IPI-3 Manual Revision 1.02"
 *	Number: 1017
 *	Dated:  9-20-93
 *	Section 2.3.2.6
 *
 * Some additional information courtesy of Don Masters at
 * Maximum Strategy Technical Support.  Thanks Don!
 */
static struct gen4_in_chan_error_codes {
	unsigned short	code;
	char		*means;
} gen4_in_chan_error[] = {

    { 0xf501,	"short data burst error" },			/* bit-mask */
    { 0xf502,	"parity error" },				/* bit-mask */
    { 0xf504,	"LLRC error" },					/* bit-mask */
    { 0xf508,	"sequence error" },				/* bit-mask */
    { 0xf510,	"no interconnect" },				/* bit-mask */
    { 0xf520,	"loss of synchronization" },			/* bit-mask */

    { 0xf600,	"unexpected next connection" },
    { 0xf601,	"IN-Channel aborted by CPU" },
    { 0xf602,	"Odd number of data bursts received" },
    { 0xf603,	"connection terminated on boundary" },

    { 0,	0 }
};

gen4_decode_in_chan_error(code)
	int	code;
{
	register struct gen4_in_chan_error_codes	*err;

	if (code == 0)
		return;

	printf("\tIN-Chan Error: DST: ");

	/*
	 * 0xf500 is a bit-mask error code
	 */
	if ((code & 0xff00) == 0xf500) {
		boolean_t	first;
		int		bit, bit_code;

		first = TRUE;
		for (bit = 0; bit <= 7; bit++) {
			bit_code = code & (0xff00 | (1 << bit));

			for (err = gen4_in_chan_error; err->means; err++) {
				if (err->code != bit_code)
					continue;

				printf("%s%s", (first) ? "" : ", ", err->means);
				first = FALSE;
				break;
			}
		}

		if (first)
			printf("0x%04x", code);

		printf("\n");
		return;
	}

	for (err = gen4_in_chan_error; err->means; err++) {
		if (err->code != code)
			continue;

		printf("%s\n", err->means);
		return;
	}

	printf("0x%04x\n", code);
}


/*
 * Table of the Maximum Strategy GEN-4 OUT-Chan error codes
 *	Taken from the "Maximum Strategy, Inc.  IPI-3 Manual Revision 1.02"
 *	Number: 1017
 *	Dated:  9-20-93
 *	Section 2.3.2.6
 *
 * Some additional information courtesy of Don Masters at
 * Maximum Strategy Technical Support.  Thanks Don!
 */
static struct gen4_out_chan_error_codes {
	unsigned short	code;
	char		*means;
} gen4_out_chan_error[] = {

    { 0xf200,	"destination broke connection" },
    { 0xf201,	"OUT-Channel aborted by CPU" },
    { 0xf202,	"need start connection (stay-connected)" },
    { 0xf300,	"disconnect time-out" },
    { 0xf301,	"connect time-out" },
    { 0xf302,	"ready time-out waiting for header burst" },
    { 0xf303,	"ready time-out waiting for data burst" },
    { 0xf304,	"connect retry count expired" },

    { 0xf404,	"invalid connect or ready from Destination" },	/* bit-mask */
    { 0xf406,	"packet negate, no burst sent in packet" },	/* bit-mask */
    { 0xf408,	"no destination interconnect" },		/* bit-mask */
    { 0xf410,	"parity error" },				/* bit-mask */
    { 0xf401,	"readys received" },				/* bit-mask */

    { 0,	0 }
};

gen4_decode_out_chan_error(code)
	int	code;
{
	register struct gen4_out_chan_error_codes	*err;

	if (code == 0)
		return;

	printf("\tOUT-Chan Error: SRC: ");

	/*
	 * 0xf400 is a bit-mask error code
	 */
	if ((code & 0xff00) == 0xf400) {
		boolean_t	first;
		int		bit, bit_code;

		first = TRUE;
		for (bit = 0; bit <= 7; bit++) {
			bit_code = code & (0xff00 | (1 << bit));

			for (err = gen4_out_chan_error; err->means; err++) {
				if (err->code != bit_code)
					continue;

				printf("%s%s", (first) ? "" : ", ", err->means);
				first = FALSE;
				break;
			}
		}

		if (first)
			printf("0x%04x", code);

		printf("\n");
		return;
	}

	for (err = gen4_out_chan_error; err->means; err++) {
		if (err->code != code)
			continue;

		printf("%s\n", err->means);
		return;
	}

	printf("0x%04x\n", code);
}


/*
 * Table of the Maximum Strategy GEN-4 IFP error codes
 *	Taken from the "Maximum Strategy, Inc.  IPI-3 Manual Revision 1.02"
 *	Number: 1017
 *	Dated:  9-20-93
 *	Section 2.3.2.6
 *
 * Some additional information courtesy of Don Masters at
 * Maximum Strategy Technical Support.  Thanks Don!
 */
static struct gen4_ifp_error_codes {
	unsigned short	code;
	char		*means;
} gen4_ifp_error[] = {

    { 0xf000,	"PAC: address count overflow" },
    { 0xf001,	"PAC: stack full" },
    { 0xf002,	"PAC: FIFO overflow" },
    { 0xf003,	"PAC: FIFO input ready" },
    { 0xf004,	"PAC: unknown" },
    { 0xf005,	"PAC: breakpoint" },
    { 0xf006,	"XIL: unexpected done" },
    { 0xf007,	"XIL: not done" },
    { 0xf008,	"XIL: bad prefix" },
    { 0xf009,	"XIL: bad serial data" },
    { 0xf00a,	"XIL: bad starting address" },
    { 0xf00b,	"XIL: bad load checksum" },
    { 0xf00c,	"XIL: mover ACK failure" },
    { 0xf00d,	"DST/SRC: interlock lost" },
    { 0xf010,	"ACC: reset failed" },
    { 0xf011,	"ACC: bad function" },
    { 0xf012,	"ACC: zero check fail" },
    { 0xf020,	"DST: In-event list full" },
    { 0xf021,	"DST: short data burst in headerless data-only mode" },
    { 0xf022,	"DST: header less than 4K in 4K-header mode" },
    { 0xf030,	"DST/SRC: bus error (tri-port address)" },
    { 0xf031,	"DST/SRC: bus error (PAC data)" },
    { 0xf032,	"DST/SRC: bus error (DPM address)" },
    { 0xf033,	"DST/SRC: bus error (OUT control parity)" },
    { 0xf034,	"DST/SRC: DPM parity error after start command" },
    { 0xf035,	"DST/SRC: DPM parity error" },

    { 0xf012,	"zero check failed" },

    { 0xf100,	"invalid function" },
    { 0xf101,	"wait check fail" },
    { 0xf1f0,	"DST/SRC: TPM > DPM parity error (X = error bits)" },
    { 0xf701,	"XFER: DPM parity error: CMD side" },
    { 0xf702,	"XFER: DPM parity error: PAC side" },

    { 0x9000,	"BUF: module parity error" },
    { 0xa000,	"BUF: DMC error disconnect" },
    { 0xc005,	"BUF: invalid buffer state, buffer full" },
    { 0xc000,	"BUF: invalid buffer state, buffer empty" },

    { 0x8101,	"bus A timeout" },				/* bit-mask */
    { 0x8102,	"bus A transmit parity error (DMC > IFP)" },	/* bit-mask */
    { 0x8104,	"bus A receive parity error (IFP > DMC)" },	/* bit-mask */
    { 0x8108,	"bus B timeout" },				/* bit-mask */
    { 0x8110,	"bus B transmit parity error (DMC > IFP)" },	/* bit-mask */
    { 0x8120,	"bus B receive parity error (IFP > DMC)" },	/* bit-mask */

    { 0,	0 }
};

gen4_decode_ifp_error(code)
	int	code;
{
	register struct gen4_ifp_error_codes	*err;

	if (code == 0)
		return;

	printf("\tInternal IFP Error: ");

	if ((code & 0xfff0) == 0xf1f0) {
		printf("DST/SRC: TPM > DPM parity error (0x%x = error bits)\n",
			code & 0xf);
		return;
	}

	/*
	 * 0x8100 is a bit-mask error code
	 */
	if ((code & 0xff00) == 0x8100) {
		boolean_t	first;
		int		bit, bit_code;

		printf("transfer bus error: ");

		first = TRUE;
		for (bit = 0; bit <= 7; bit++) {
			bit_code = code & (0xff00 | (1 << bit));

			for (err = gen4_ifp_error; err->means; err++) {
				if (err->code != bit_code)
					continue;

				printf("%s%s", (first) ? "" : ", ", err->means);
				first = FALSE;
				break;
			}
		}

		if (first)
			printf("0x%04x", code);

		printf("\n");
		return;
	}

	for (err = gen4_ifp_error; err->means; err++) {
		if (err->code != code)
			continue;

		printf("%s\n", err->means);
		return;
	}

	printf("0x%04x\n", code);
}


/*
 * Table of the Maximum Strategy extended substatus error codes
 *	Taken from the "Maximum Strategy, Inc.  IPI-3 Manual Revision 1.02"
 *	Number: 1017
 *	Dated:  9-20-93
 *	Section 2.3.1
 */
static struct gen4_extended_substatus_msg {
	unsigned char	id;		/* low-order four bits	 */
	unsigned char	octet;		/* byte offset in param	 */
	unsigned char	bit;		/* bit offset in byte	 */
	unsigned int	action;		/* error recovery action */
	char		*means;		/* error message string	 */
} gen4_extended_msg[] = {

    /*
     * ID 0x13 or 0x23: Message/Microcode Exception Extended Substatus
     */
    /*
     * Reserved on Maximum Strategy GEN-4
     */

    /*
     * ID 0x14 or 0x24: Intervention Required Extended Substatus
     */
    /*
     * Reserved on Maximum Strategy GEN-4
     */

    /*
     * ID 0x15 or 0x25: Alternate Port Exception Extended Substatus
     */
    /*
     * Reserved on Maximum Strategy GEN-4
     */

    /*
     * ID 0x16 or 0x26: Machine Exception Extended Substatus
     */
    { 0x06, 5, 0, ACTION_RETRY,   "Auto Reconstruct Attempted" },
    { 0x06, 5, 1, ACTION_RETRY,   "Auto Disable Attempted" },
    { 0x06, 5, 6, ACTION_ERROR,   "Module Fatal Error" },
    { 0x06, 5, 7, ACTION_RETRY,   "Module Command Channel Busy" },
    { 0x06, 6, 5, ACTION_RETRY,   "Connection Refused" },
    { 0x06, 6, 6, ACTION_RETRY,   "Connection Time-out" },
    { 0x06, 6, 7, ACTION_RETRY,   "Unknown Host" },
    { 0x06, 7, 0, ACTION_ERROR,   "IFP Unexpected D1 Data Set Is Present" },
    { 0x06, 7, 1, ACTION_ERROR,   "IFP Expected D1 Data Set Is Not Present" },
    { 0x06, 7, 2, ACTION_ERROR,   "IFP Data Not On Proper Burst Boundary" },
    { 0x06, 7, 4, ACTION_ERROR,   "IFP Header Format Error" },
    { 0x06, 7, 5, ACTION_RETRY,   "IFP Header LLRC Error" },
    { 0x06, 7, 6, ACTION_RETRY,   "IFP Header Parity Error" },
    { 0x06, 7, 7, ACTION_RETRY,   "IFP Fatal Error" },
    { 0x06, 8, 4, ACTION_ERROR,   "IFP Other System Failed" },
    { 0x06, 8, 5, ACTION_ERROR,   "IFP Prefix Error" },
    { 0x06, 8, 6, ACTION_ERROR,   "IFP Unexpected D2 Data Set Is Present" },
    { 0x06, 8, 7, ACTION_ERROR,   "IFP D2 Data Set Is Not Present" },

    /*
     * ID 0x17 or 0x27: Command Exception Extended Substatus
     */
    { 0x07, 5, 6, ACTION_ERROR, "Invalid Module Type" },
    { 0x07, 5, 7, ACTION_ERROR, "Module Not Attached" },
    { 0x07, 6, 0, ACTION_ERROR, "Module Substituted" },
    { 0x07, 6, 1, ACTION_ERROR, "Module Reconstruction In Progress" },
    { 0x07, 6, 2, ACTION_ERROR, "Facility Reconstruction In Progress" },
    { 0x07, 6, 3, ACTION_ERROR, "Module Already Disabled" },
    { 0x07, 6, 4, ACTION_ERROR, "Module Already Substituted" },
    { 0x07, 6, 5, ACTION_ERROR, "No Stand-by Available" },
    { 0x07, 6, 6, ACTION_ERROR, "No Module Substituted" },
    { 0x07, 6, 7, ACTION_ERROR, "No Module Disabled" },

    /*
     * ID 0x18 or 0x28: Command Aborted Extended Substatus
     */
    { 0x08, 5, 5, ACTION_ERROR, "Aborted via Disconnect" },
    { 0x08, 5, 6, ACTION_RETRY, "Aborted via Command Time-out" },

    /*
     * ID 0x19 or 0x29: Conditional Success Extended Substatus
     */
    { 0x09, 5, 6, ACTION_SUCCESS, "Module Data Parity Replacement Performed" },
    { 0x09, 5, 7, ACTION_SUCCESS, "Data Parity Replacement Performed" },
    { 0x09, 6, 7, ACTION_SUCCESS, "Stale Status Data" },
    { 0x09, 7, 0, ACTION_ERROR,   "Diagnostic Error" },
    { 0x09, 8, 0, ACTION_ERROR,   "Diagnostic Bit 0, Header Parity Error" },
    { 0x09, 8, 1, ACTION_ERROR,   "Diagnostic Bit 1, Transmit Status Error" },
    { 0x09, 8, 2, ACTION_ERROR,   "Diagnostic Bit 2, I-field Error" },
    { 0x09, 8, 3, ACTION_ERROR,   "Diagnostic Bit 3, LLRC Error" },
    { 0x09, 8, 4, ACTION_ERROR,   "Diagnostic Pattern Compare Error" },
    { 0x09, 8, 5, ACTION_ERROR,   "Diagnostic Improper Transfer Count" },
    { 0x09, 8, 6, ACTION_ERROR,   "Diagnostic Zero Facility Size" },

    /*
     * ID 0x1a or 0x2a: Incomplete Extended Substatus
     */
    /*
     * Reserved on Maximum Strategy GEN-4
     */

    { 0,    0, 0, 0,		0 }
};

gen4_decode_extended_substatus(param_id, octet, bit)
	int	param_id;
	int	octet;
	int	bit;
{
	int						id;
	register struct gen4_extended_substatus_msg	*msg;

	id = param_id & 0x0f;

	for (msg = gen4_extended_msg; msg->means; msg++) {
		if ((msg->id	!= id)	  ||
		    (msg->octet != octet) ||
		    (msg->bit	!= bit)) continue;

		printf("\t%s\n", msg->means);

		return msg->action;
	}

	printf("\tReserved Bit Set: Substatus ID 0x%02x, Octet %d, Bit %d\n",
		param_id, octet, bit);

	return ACTION_RETRY;
}

#endif	NIPI > 0
