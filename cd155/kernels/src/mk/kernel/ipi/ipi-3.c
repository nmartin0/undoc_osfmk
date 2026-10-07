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
 * $Log: ipi-3.c,v $
 * Revision 1.4  1994/11/18  20:50:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/10/04  16:12:16  jerrie
 * Round up the requested read or write byte count to a device block boundary.
 * The device layer guarantees that the scatter/gather list is large enough to
 * satisfy the request.
 *
 *  Reviewer: Arlin Davis
 *  Risk: Low. Only affects scatter/gather to block devices.
 *  Benefit or PTS #: PTS 10477 IPI-3 Raw I/O hangs or returns I/O error
 * 	if data length is not a multiple of the device block size.
 *  Testing: Used two of Evaluation's tests and created one of my own to
 * 	walk through various combinations of block sizes and offsets.
 * 	Ran Evaluation's HiPPI and IPI-3 EATs on a kernel with the
 * 	fixes in place.
 *  Module(s): vm/vm_kern.c, device/ds_routines.c, ipi/ipi_disk.c,
 * 	ipi/ipi_labels.c, ipi/ipi-3.c
 *
 * Revision 1.2  1994/06/28  23:54:00  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.1  1994/06/08  16:53:34  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi-3.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Driver command interface for the Intelligent Peripheral Interface
 *	Part 3: Device generic command set for magnetic and optical disk drives
 */

#include <ipi.h>
#if	NIPI > 0

#include <ipi/ipi_compat.h>
#include <ipi/ipi_endian.h>
#include <ipi/ipi_defs.h>
#include <ipi/ipi_map.h>
#include <ipi/ipi-3.h>



ipi3_nop(tgt, cmd_q)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
{
	io_req_t	ior = cmd_q->ior;
	hippi_fp_t	*hdr = cmd_q->hdr_ptr;
	ipi_cmd_nop_t	*cmd = (ipi_cmd_nop_t *)cmd_q->cmd_ptr;

	/* build the HiPPI-FP header */
	hdr->ulp_id	 = ULP_ID_IPI_3_SLAVE;
	hdr->p		 = TRUE;
	hdr->b		 = FALSE;
	hdr->d1_size_msb = (sizeof(ipi_cmd_nop_t) / 8) >> D1_SIZE_MSB_SHIFT;
	hdr->d1_size_lsb = (sizeof(ipi_cmd_nop_t) / 8) >> 0;
	hdr->d2_offset	 = 0;
	hdr->d2_size_msb = 0;	/* no data */
	hdr->d2_size_b2  = 0;
	hdr->d2_size_b1  = 0;
	hdr->d2_size_lsb = 0;

	/* build the IPI-3 NOP command */
	cmd->packet_length_msb = (sizeof(ipi_cmd_nop_t) - 2) >> 8;
	cmd->packet_length_lsb = (sizeof(ipi_cmd_nop_t) - 2) >> 0;
	cmd->reference_msb     = cmd_q->cmd_ref >> 8;
	cmd->reference_lsb     = cmd_q->cmd_ref >> 0;
	cmd->opcode	       = IPI_CMD_NOP;
	cmd->modifier	       = IPI_INDIVIDIAL_CMD;
	cmd->slave	       = tgt->target_id;
	cmd->facility	       = (ior) ? IPI_FACILITY(ior->io_unit) :
					 IPI_SLAVE_ONLY;

	/* set data direction */
	cmd_q->dir = NO_DATA;

	/* set length of first burst */
	cmd_q->sreq.fb_len = sizeof(hippi_fp_t) + sizeof(ipi_cmd_nop_t);
	assert((cmd_q->sreq.fb_len % 8) == 0);	/* per HiPPI-FP 7.2.2 */

	/* initialize the ior and scatter/gather pointers */
	cmd_q->sreq.ior    = ior;
	cmd_q->sreq.sg_ptr = NULL;

	IPI_GO(tgt, cmd_q);
}


ipi3_read(tgt, cmd_q, secno)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	unsigned int	secno;
{
	io_req_t		ior = cmd_q->ior;
	register hippi_fp_t	*hdr = cmd_q->hdr_ptr;
	register ipi_cmd_read_t	*cmd = (ipi_cmd_read_t *)cmd_q->cmd_ptr;
	unsigned int		nbytes;
	unsigned int		nblks;

	nbytes = ior->io_count;

	/*
	 * Round up transfer length to a multiple of the device block size.
	 * The device layer guarantees that the read buffer is large enough
	 * to satisfy the request.
	 */
	if (nbytes % tgt->block_size) {
		nbytes += tgt->block_size - nbytes % tgt->block_size;
	}

	/* build the HiPPI-FP header */
	hdr->ulp_id	 = ULP_ID_IPI_3_SLAVE;
	hdr->p		 = TRUE;
	hdr->b		 = FALSE;
	hdr->d1_size_msb = (sizeof(ipi_cmd_read_t) / 8) >> D1_SIZE_MSB_SHIFT;
	hdr->d1_size_lsb = (sizeof(ipi_cmd_read_t) / 8) >> 0;
	hdr->d2_offset	 = 0;
	hdr->d2_size_msb = 0;	/* no data */
	hdr->d2_size_b2  = 0;
	hdr->d2_size_b1  = 0;
	hdr->d2_size_lsb = 0;

	/* build the IPI-3 READ command */
	cmd->packet_length_msb	  = (sizeof(ipi_cmd_read_t) - 2) >> 8;
	cmd->packet_length_lsb	  = (sizeof(ipi_cmd_read_t) - 2) >> 0;
	cmd->reference_msb	  = cmd_q->cmd_ref >> 8;
	cmd->reference_lsb	  = cmd_q->cmd_ref >> 0;
	cmd->opcode		  = IPI_CMD_READ;
	cmd->modifier		  = IPI_QUEUED_CMD	     |
				    IPI_CMD_READ_BLOCK_COUNT |
				    IPI_CMD_READ_DATA_BLOCK;
	cmd->slave		  = tgt->target_id;
	cmd->facility		  = IPI_FACILITY(ior->io_unit);

	cmd->partition_len	  = IPI_PARAM_PARTITION_LEN;
	cmd->partition_id	  = IPI_PARAM_PARTITION;
	cmd->partition_reserved   = 0;
	cmd->partition_partition  = tgt->partition;

	nblks = btodb(nbytes);

	cmd->cmd_extent_len	  = IPI_PARAM_COMMAND_EXTENT_LEN;
	cmd->cmd_extent_id	  = IPI_PARAM_COMMAND_EXTENT;
	cmd->cmd_extent_count_msb = (nblks >> 24) & 0xff;
	cmd->cmd_extent_count_b2  = (nblks >> 16) & 0xff;
	cmd->cmd_extent_count_b1  = (nblks >>  8) & 0xff;
	cmd->cmd_extent_count_lsb = (nblks >>  0) & 0xff;
	cmd->cmd_extent_addr_msb  = (secno >> 24) & 0xff;
	cmd->cmd_extent_addr_b2	  = (secno >> 16) & 0xff;
	cmd->cmd_extent_addr_b1	  = (secno >>  8) & 0xff;
	cmd->cmd_extent_addr_lsb  = (secno >>  0) & 0xff;

	cmd->pad0 = 0;
	cmd->pad1 = 0;

	if (ipi_debug) {
		printf("ipi3_read: first blk %d, last blk %d, count %d\n",
			secno, secno + nblks - 1, nblks);
	}

	/* set data direction */
	cmd_q->dir = DATA_IN;

	/* set length of first burst */
	cmd_q->sreq.fb_len = sizeof(hippi_fp_t) + sizeof(ipi_cmd_read_t);
	assert((cmd_q->sreq.fb_len % 8) == 0);	/* per HiPPI-FP 7.2.2 */

	/* initialize the ior and scatter/gather pointers */
	cmd_q->sreq.ior    = ior;
	cmd_q->sreq.sg_ptr = NULL;

	IPI_GO(tgt, cmd_q);

#ifdef	IPI_STATISTICS
	if (!(ior->io_op & IO_INTERNAL)) {
		tgt->statistics.read.requests++;
		tgt->statistics.read.xfer_count += nblks;
	}
#endif
}


ipi3_write(tgt, cmd_q, secno)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	unsigned int	secno;
{
	io_req_t		 ior = cmd_q->ior;
	register hippi_fp_t	 *hdr = cmd_q->hdr_ptr;
	register ipi_cmd_write_t *cmd = (ipi_cmd_write_t *)cmd_q->cmd_ptr;
	unsigned int		 nbytes;
	unsigned int		 nblks;

	nbytes = ior->io_count;

	/*
	 * Round up transfer length to a multiple of the device block size.
	 * The device layer guarantees that the scatter/gather list points
	 * to enough data to satisfy the request.
	 */
	if (nbytes % tgt->block_size) {
		nbytes += tgt->block_size - nbytes % tgt->block_size;
	}

	/* build the HiPPI-FP header */
	hdr->ulp_id	 = ULP_ID_IPI_3_SLAVE;
	hdr->p		 = TRUE;
	hdr->b		 = TRUE;
	hdr->d1_size_msb = (sizeof(ipi_cmd_write_t) / 8) >> D1_SIZE_MSB_SHIFT;
	hdr->d1_size_lsb = (sizeof(ipi_cmd_write_t) / 8) >> 0;
	hdr->d2_offset	 = 0;
	hdr->d2_size_msb = (nbytes >> 24);
	hdr->d2_size_b2  = (nbytes >> 16);
	hdr->d2_size_b1  = (nbytes >>  8);
	hdr->d2_size_lsb = (nbytes >>  0);

	/* build the IPI-3 WRITE command */
	cmd->packet_length_msb	  = (sizeof(ipi_cmd_write_t) - 2) >> 8;
	cmd->packet_length_lsb	  = (sizeof(ipi_cmd_write_t) - 2) >> 0;
	cmd->reference_msb	  = cmd_q->cmd_ref >> 8;
	cmd->reference_lsb	  = cmd_q->cmd_ref >> 0;
	cmd->opcode		  = IPI_CMD_WRITE;
	cmd->modifier		  = IPI_QUEUED_CMD	      |
				    IPI_CMD_WRITE_BLOCK_COUNT |
				    IPI_CMD_WRITE_DATA_BLOCK;
	cmd->slave		  = tgt->target_id;
	cmd->facility		  = IPI_FACILITY(ior->io_unit);

	cmd->partition_len	  = IPI_PARAM_PARTITION_LEN;
	cmd->partition_id	  = IPI_PARAM_PARTITION;
	cmd->partition_reserved   = 0;
	cmd->partition_partition  = tgt->partition;

	nblks = btodb(nbytes);

	cmd->cmd_extent_len	  = IPI_PARAM_COMMAND_EXTENT_LEN;
	cmd->cmd_extent_id	  = IPI_PARAM_COMMAND_EXTENT;
	cmd->cmd_extent_count_msb = (nblks >> 24) & 0xff;
	cmd->cmd_extent_count_b2  = (nblks >> 16) & 0xff;
	cmd->cmd_extent_count_b1  = (nblks >>  8) & 0xff;
	cmd->cmd_extent_count_lsb = (nblks >>  0) & 0xff;
	cmd->cmd_extent_addr_msb  = (secno >> 24) & 0xff;
	cmd->cmd_extent_addr_b2	  = (secno >> 16) & 0xff;
	cmd->cmd_extent_addr_b1	  = (secno >>  8) & 0xff;
	cmd->cmd_extent_addr_lsb  = (secno >>  0) & 0xff;

	cmd->pad0 = 0;
	cmd->pad1 = 0;

	if (ipi_debug) {
		printf("ipi3_write: first blk %d, last blk %d, count %d\n",
			secno, secno + nblks - 1, nblks);
	}

	/* set data direction */
	cmd_q->dir = DATA_OUT;

	/* set length of first burst */
	cmd_q->sreq.fb_len = sizeof(hippi_fp_t) + sizeof(ipi_cmd_write_t);
	assert((cmd_q->sreq.fb_len % 8) == 0);	/* per HiPPI-FP 7.2.2 */

	/* initialize the ior and scatter/gather pointers */
	cmd_q->sreq.ior    = ior;
	cmd_q->sreq.sg_ptr = ior->io_sgp;

	IPI_GO(tgt, cmd_q);

#ifdef	IPI_STATISTICS
	if (!(ior->io_op & IO_INTERNAL)) {
		tgt->statistics.write.requests++;
		tgt->statistics.write.xfer_count += nblks;
	}
#endif
}


ipi3_get_attribute(tgt, cmd_q, id)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	unsigned char	id;
{
	io_req_t		ior = cmd_q->ior;
	hippi_fp_t		*hdr = cmd_q->hdr_ptr;
	ipi_cmd_attributes_t	*cmd = (ipi_cmd_attributes_t *)cmd_q->cmd_ptr;
	ipi_request_t		*req = (ipi_request_t *)(cmd + 1);

	/* build the HiPPI-FP header */
	hdr->ulp_id	 = ULP_ID_IPI_3_SLAVE;
	hdr->p		 = TRUE;
	hdr->b		 = FALSE;
	hdr->d1_size_msb =
		((sizeof(ipi_cmd_attributes_t) + sizeof(ipi_request_t)) / 8) >>
			D1_SIZE_MSB_SHIFT;
	hdr->d1_size_lsb =
		((sizeof(ipi_cmd_attributes_t) + sizeof(ipi_request_t)) / 8) >>
			0;
	hdr->d2_offset	 = 0;
	hdr->d2_size_msb = 0;	/* no data */
	hdr->d2_size_b2  = 0;
	hdr->d2_size_b1  = 0;
	hdr->d2_size_lsb = 0;

	/* build the IPI-3 Attributes command */
	cmd->packet_length_msb =
		((sizeof(ipi_cmd_attributes_t) + sizeof(ipi_request_t)) - 2) >>
			8;
	cmd->packet_length_lsb =
		((sizeof(ipi_cmd_attributes_t) + sizeof(ipi_request_t)) - 2) >>
			0;
	cmd->reference_msb     = cmd_q->cmd_ref >> 8;
	cmd->reference_lsb     = cmd_q->cmd_ref >> 0;
	cmd->opcode	       = IPI_CMD_ATTRIBUTES;
	cmd->modifier	       = IPI_CMD_ATTRIBUTES_REPORT;	/* get them */
	cmd->slave	       = tgt->target_id;
	cmd->facility	       = (ior) ? IPI_FACILITY(ior->io_unit) :
					 IPI_SLAVE_ONLY;

	/* select the desired parameter */
	req->length	       = 3;
	req->id		       = IPI_PARAM_REQUEST_PARAM;
	req->control	       = PARAMS_IN_RESPONSE;
	req->param_id	       = id;
	req->pad0	       = 0;
	req->pad1	       = 0;
	req->pad2	       = 0;
	req->pad3	       = 0;

	/* set data direction */
	cmd_q->dir = NO_DATA;

	/* set length of first burst */
	cmd_q->sreq.fb_len = sizeof(hippi_fp_t)		  +
			     sizeof(ipi_cmd_attributes_t) +
			     sizeof(ipi_request_t);
	assert((cmd_q->sreq.fb_len % 8) == 0);	/* per HiPPI-FP 7.2.2 */

	/* initialize the ior and scatter/gather pointers */
	cmd_q->sreq.ior    = ior;
	cmd_q->sreq.sg_ptr = NULL;

	IPI_GO_AND_WAIT(tgt, cmd_q);

	return cmd_q->result;
}


ipi3_set_attribute(tgt, cmd_q, data, len)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	unsigned char	*data;
	int		len;
{
	io_req_t		ior = cmd_q->ior;
	hippi_fp_t		*hdr = cmd_q->hdr_ptr;
	ipi_cmd_attributes_t	*cmd = (ipi_cmd_attributes_t *)cmd_q->cmd_ptr;

	/* build the HiPPI-FP header */
	hdr->ulp_id	 = ULP_ID_IPI_3_SLAVE;
	hdr->p		 = TRUE;
	hdr->b		 = FALSE;
	hdr->d1_size_msb =
		((sizeof(ipi_cmd_attributes_t) + len) / 8) >>
			D1_SIZE_MSB_SHIFT;
	hdr->d1_size_lsb =
		((sizeof(ipi_cmd_attributes_t) + len) / 8) >>
			0;
	hdr->d2_offset	 = 0;
	hdr->d2_size_msb = 0;	/* no data */
	hdr->d2_size_b2  = 0;
	hdr->d2_size_b1  = 0;
	hdr->d2_size_lsb = 0;

	/* build the IPI-3 Attributes command */
	cmd->packet_length_msb =
		((sizeof(ipi_cmd_attributes_t) + len) - 2) >> 8;
	cmd->packet_length_lsb =
		((sizeof(ipi_cmd_attributes_t) + len) - 2) >> 0;
	cmd->reference_msb     = cmd_q->cmd_ref >> 8;
	cmd->reference_lsb     = cmd_q->cmd_ref >> 0;
	cmd->opcode	       = IPI_CMD_ATTRIBUTES;
	cmd->modifier	       = IPI_CMD_ATTRIBUTES_LOAD;	/* set them */
	cmd->slave	       = tgt->target_id;
	cmd->facility	       = (ior) ? IPI_FACILITY(ior->io_unit) :
					 IPI_SLAVE_ONLY;

	/* append the attribute data */
	bcopy(data, (unsigned char *)(cmd + 1), len);

	/* set data direction */
	cmd_q->dir = NO_DATA;

	/* set length of first burst */
	cmd_q->sreq.fb_len = sizeof(hippi_fp_t)		  +
			     sizeof(ipi_cmd_attributes_t) +
			     len;
	assert((cmd_q->sreq.fb_len % 8) == 0);	/* per HiPPI-FP 7.2.2 */

	/* initialize the ior and scatter/gather pointers */
	cmd_q->sreq.ior    = ior;
	cmd_q->sreq.sg_ptr = NULL;

	IPI_GO_AND_WAIT(tgt, cmd_q);

	return cmd_q->result;
}


ipi3_operating_mode(tgt, cmd_q, data, len)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	unsigned char	*data;
	int		len;
{
	io_req_t		ior = cmd_q->ior;
	hippi_fp_t		*hdr = cmd_q->hdr_ptr;
	ipi_cmd_op_mode_t	*cmd = (ipi_cmd_op_mode_t *)cmd_q->cmd_ptr;

	/* build the HiPPI-FP header */
	hdr->ulp_id	 = ULP_ID_IPI_3_SLAVE;
	hdr->p		 = TRUE;
	hdr->b		 = FALSE;
	hdr->d1_size_msb =
		((sizeof(ipi_cmd_op_mode_t) + len) / 8) >>
			D1_SIZE_MSB_SHIFT;
	hdr->d1_size_lsb =
		((sizeof(ipi_cmd_op_mode_t) + len) / 8) >>
			0;
	hdr->d2_offset	 = 0;
	hdr->d2_size_msb = 0;	/* no data */
	hdr->d2_size_b2  = 0;
	hdr->d2_size_b1  = 0;
	hdr->d2_size_lsb = 0;

	/* build the IPI-3 Attributes command */
	cmd->packet_length_msb = (sizeof(ipi_cmd_op_mode_t) + len - 2) >> 8;
	cmd->packet_length_lsb = (sizeof(ipi_cmd_op_mode_t) + len - 2) >> 0;
	cmd->reference_msb     = cmd_q->cmd_ref >> 8;
	cmd->reference_lsb     = cmd_q->cmd_ref >> 0;
	cmd->opcode	       = IPI_CMD_OPERATING_MODE;
	cmd->modifier	       = IPI_CMD_OPERATING_MODE_SET;
	cmd->slave	       = tgt->target_id;
	cmd->facility	       = (ior) ? IPI_FACILITY(ior->io_unit) :
					 IPI_SLAVE_ONLY;

	/* append the mode data */
	bcopy(data, (unsigned char *)(cmd + 1), len);

	/* set data direction */
	cmd_q->dir = NO_DATA;

	/* set length of first burst */
	cmd_q->sreq.fb_len = sizeof(hippi_fp_t)	       +
			     sizeof(ipi_cmd_op_mode_t) +
			     len;
	assert((cmd_q->sreq.fb_len % 8) == 0);	/* per HiPPI-FP 7.2.2 */

	/* initialize the ior and scatter/gather pointers */
	cmd_q->sreq.ior    = ior;
	cmd_q->sreq.sg_ptr = NULL;

	IPI_GO_AND_WAIT(tgt, cmd_q);

	return cmd_q->result;
}


ipi3_report_addressee_status(tgt, cmd_q, id)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	unsigned char	id;
{
	io_req_t		ior = cmd_q->ior;
	hippi_fp_t		*hdr = cmd_q->hdr_ptr;
	ipi_report_status_t	*cmd = (ipi_report_status_t *)cmd_q->cmd_ptr;

	/* build the HiPPI-FP header */
	hdr->ulp_id	 = ULP_ID_IPI_3_SLAVE;
	hdr->p		 = TRUE;
	hdr->b		 = FALSE;
	hdr->d1_size_msb = (sizeof(ipi_report_status_t) / 8) >>
							D1_SIZE_MSB_SHIFT;
	hdr->d1_size_lsb = (sizeof(ipi_report_status_t) / 8) >> 0;
	hdr->d2_offset	 = 0;
	hdr->d2_size_msb = 0;	/* no data */
	hdr->d2_size_b2  = 0;
	hdr->d2_size_b1  = 0;
	hdr->d2_size_lsb = 0;

	/* build the IPI-3 Attributes command */
	cmd->packet_length_msb = (sizeof(ipi_report_status_t) - 2) >> 8;
	cmd->packet_length_lsb = (sizeof(ipi_report_status_t) - 2) >> 0;
	cmd->reference_msb     = cmd_q->cmd_ref >> 8;
	cmd->reference_lsb     = cmd_q->cmd_ref >> 0;
	cmd->opcode	       = IPI_CMD_REPORT_ADDRESSEE_STATUS;
	cmd->modifier	       = id;
	cmd->slave	       = tgt->target_id;
	cmd->facility	       = (ior) ? IPI_FACILITY(ior->io_unit) :
					 IPI_SLAVE_ONLY;

	/* set data direction */
	cmd_q->dir = NO_DATA;

	/* set length of first burst */
	cmd_q->sreq.fb_len = sizeof(hippi_fp_t)	       +
			     sizeof(ipi_report_status_t);
	assert((cmd_q->sreq.fb_len % 8) == 0);	/* per HiPPI-FP 7.2.2 */

	/* initialize the ior and scatter/gather pointers */
	cmd_q->sreq.ior    = ior;
	cmd_q->sreq.sg_ptr = NULL;

	IPI_GO_AND_WAIT(tgt, cmd_q);

	return cmd_q->result;
}


ipi3_abort(tgt, cmd_q, facility, cmd_ref)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	unsigned char	facility;
	unsigned short	cmd_ref;
{
	io_req_t		ior = cmd_q->ior;
	hippi_fp_t		*hdr = cmd_q->hdr_ptr;
	ipi_abort_t		*cmd = (ipi_abort_t *)cmd_q->cmd_ptr;

	/* build the HiPPI-FP header */
	hdr->ulp_id	 = ULP_ID_IPI_3_SLAVE;
	hdr->p		 = TRUE;
	hdr->b		 = FALSE;
	hdr->d1_size_msb = (sizeof(ipi_abort_t) / 8) >>
							D1_SIZE_MSB_SHIFT;
	hdr->d1_size_lsb = (sizeof(ipi_abort_t) / 8) >> 0;
	hdr->d2_offset	 = 0;
	hdr->d2_size_msb = 0;	/* no data */
	hdr->d2_size_b2  = 0;
	hdr->d2_size_b1  = 0;
	hdr->d2_size_lsb = 0;

	/* build the IPI-3 Abort command */
	cmd->packet_length_msb = (sizeof(ipi_abort_t) - 2) >> 8;
	cmd->packet_length_lsb = (sizeof(ipi_abort_t) - 2) >> 0;
	cmd->reference_msb     = cmd_q->cmd_ref >> 8;
	cmd->reference_lsb     = cmd_q->cmd_ref >> 0;
	cmd->opcode	       = IPI_CMD_ABORT;
	cmd->modifier	       = IPI_PRIORITY_CMD |
				 IPI_CMD_ABORT_TERMINATE_COMMAND;
	cmd->slave	       = tgt->target_id;
	cmd->facility	       = facility;

	cmd->cmd_ref_len       = IPI_PARAM_COMMAND_REFERENCE_NUMBER_LEN;
	cmd->cmd_ref_id        = IPI_PARAM_COMMAND_REFERENCE_NUMBER;
	cmd->cmd_ref_msb       = (cmd_ref >> 8) & 0xff;
	cmd->cmd_ref_lsb       = (cmd_ref >> 0) & 0xff;

	cmd->pad0 = 0;
	cmd->pad1 = 0;
	cmd->pad2 = 0;
	cmd->pad3 = 0;

	/* set data direction */
	cmd_q->dir = NO_DATA;

	/* set length of first burst */
	cmd_q->sreq.fb_len = sizeof(hippi_fp_t)	       +
			     sizeof(ipi_abort_t);
	assert((cmd_q->sreq.fb_len % 8) == 0);	/* per HiPPI-FP 7.2.2 */

	/* initialize the ior and scatter/gather pointers */
	cmd_q->sreq.ior    = ior;
	cmd_q->sreq.sg_ptr = NULL;

	IPI_GO(tgt, cmd_q);
}


ipi3_reallocate(tgt, cmd_q, secno)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	unsigned int	secno;
{
	io_req_t		ior = cmd_q->ior;
	hippi_fp_t		*hdr = cmd_q->hdr_ptr;
	ipi_cmd_reallocate_t	*cmd = (ipi_cmd_reallocate_t *)cmd_q->cmd_ptr;

	/* build the HiPPI-FP header */
	hdr->ulp_id	 = ULP_ID_IPI_3_SLAVE;
	hdr->p		 = TRUE;
	hdr->b		 = FALSE;
	hdr->d1_size_msb =
		(sizeof(ipi_cmd_reallocate_t) / 8) >> D1_SIZE_MSB_SHIFT;
	hdr->d1_size_lsb = (sizeof(ipi_cmd_reallocate_t) / 8) >> 0;
	hdr->d2_offset	 = 0;
	hdr->d2_size_msb = 0;	/* no data */
	hdr->d2_size_b2  = 0;
	hdr->d2_size_b1  = 0;
	hdr->d2_size_lsb = 0;

	/* build the IPI-3 REALLOCATE command */
	cmd->packet_length_msb	  = (sizeof(ipi_cmd_reallocate_t) - 2) >> 8;
	cmd->packet_length_lsb	  = (sizeof(ipi_cmd_reallocate_t) - 2) >> 0;
	cmd->reference_msb	  = cmd_q->cmd_ref >> 8;
	cmd->reference_lsb	  = cmd_q->cmd_ref >> 0;
	cmd->opcode		  = IPI_CMD_REALLOCATE;
	cmd->modifier		  = IPI_CMD_REALLOCATE_RELOCATE_DATA;
	cmd->slave		  = tgt->target_id;
	cmd->facility		  = IPI_FACILITY(ior->io_unit);

	cmd->partition_len	  = IPI_PARAM_PARTITION_LEN;
	cmd->partition_id	  = IPI_PARAM_PARTITION;
	cmd->partition_reserved   = 0;
	cmd->partition_partition  = tgt->partition;

	cmd->cmd_extent_len	  = IPI_PARAM_COMMAND_EXTENT_LEN;
	cmd->cmd_extent_id	  = IPI_PARAM_COMMAND_EXTENT;
	cmd->cmd_extent_count_msb = 0;
	cmd->cmd_extent_count_b2  = 0;
	cmd->cmd_extent_count_b1  = 0;
	cmd->cmd_extent_count_lsb = 1;
	cmd->cmd_extent_addr_msb  = (secno >> 24) & 0xff;
	cmd->cmd_extent_addr_b2	  = (secno >> 16) & 0xff;
	cmd->cmd_extent_addr_b1	  = (secno >>  8) & 0xff;
	cmd->cmd_extent_addr_lsb  = (secno >>  0) & 0xff;

	cmd->pad0 = 0;
	cmd->pad1 = 0;

	/* set data direction */
	cmd_q->dir = NO_DATA;

	/* set length of first burst */
	cmd_q->sreq.fb_len = sizeof(hippi_fp_t) + sizeof(ipi_cmd_reallocate_t);
	assert((cmd_q->sreq.fb_len % 8) == 0);	/* per HiPPI-FP 7.2.2 */

	/* initialize the ior and scatter/gather pointers */
	cmd_q->sreq.ior    = ior;
	cmd_q->sreq.sg_ptr = NULL;

	IPI_GO(tgt, cmd_q);
}


ipi3_error(tgt, cmd_q)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
{
	ipi_response_t 	*rsp;
	char		*msg;

	/*
	 * If this was an optional command we don't want to display an error.
	 * But flag the requested operation as having an error.
	 */
	if (cmd_q->flags & CMD_OPTIONAL) {
		cmd_q->result = IPI_RET_SUCCESS;
		if (cmd_q->ior)
			cmd_q->ior->io_op |= IO_ERROR;
		return ACTION_SUCCESS;
	}

	rsp = (ipi_response_t *)cmd_q->cmd_ptr;

	switch (rsp->type) {
		case IPI_STANDARD_RESPONSE:
			msg = "Standard Command Completion Response";
			break;

		case IPI_EXTENDED_RESPONSE:
			msg = "Extended Command Completion Response";
			break;

		case IPI_ASYNCHRONOUS_RESPONSE:
			msg = "Asynchronous Response";
			break;

		case IPI_TRANSFER_NOTIFICATION:
			msg = "Transfer Notification Response";
			break;

		case IPI_EMBEDDED_DATA_RESPONSE:
			msg = "Imbedded Data Response";
			break;

		default:
			msg = "Unknown Response";
			break;
	}

	printf("\nSlave %c: %s Error\n",
		(tgt) ? tgt->unit_no + '0' : '?', msg);

	return ipi3_error_code(tgt, rsp);
}


static char *code_msg[] = {
	"Reserved 0",
	"Reserved 1",
	"Vendor Unique",
	"Message/Microcode Exception",
	"Intervention Required",
	"Alternate Port Exception",
	"Machine Exception",
	"Command Exception",
	"Command Aborted",
	"Conditional Success",
	"Incomplete",
	"Successful"
};

ipi3_error_code(tgt, rsp)
	target_info_t	*tgt;
	ipi_response_t 	*rsp;
{
	register int	i, code;
	unsigned int	action = 0;

	code = (rsp->status_msb << 8 | rsp->status_lsb) & 0xfff;

	for (i = 0; i < 12; i++) {
		if (code & (1 << i)) {
			printf("%s\n", code_msg[i]);

			/*
			 * The attached parameter may be for the slave or
			 * the facility.  Display detailed status for the
			 * slave first, then for the facility.  Refer to the
			 * ISO IPI-3 specification sections 5.4.1 and 5.4.2
			 */
			action |= ipi3_error_substatus(tgt, rsp,
				(0x01 << 4) | i);
			action |= ipi3_error_substatus(tgt, rsp,
				(0x02 << 4) | i);
		}
	}

	/*
	 * Some vendor's have unique error parameters that are not
	 * picked up by ipi3_display_error with the vendor unique
	 * major status bit.  This is the last chance to display
	 * those error parameters.
	 */
	action |= ipi_vendor_error_code(tgt, rsp);

	return action;
}


ipi3_error_substatus(tgt, rsp, id)
	target_info_t	*tgt;
	ipi_response_t 	*rsp;
	int		id;
{
	ipi_param_t		*prm;
	int			len, byte, bit;
	unsigned char   	p;
	unsigned int		action = 0;

	prm = (ipi_param_t *)ipi_param(rsp, NULL, id);
	if (prm == NULL)
		return action;

	if ((id & 0x0f) == 0x02)
		return ipi_vendor_error_substatus(tgt, id, prm);

	printf("%s Status:\n",
		((id >> 4) == 0x01) ? "Slave" : "Facility");

	for (byte = 1; byte <= 4; byte++) {
		p = prm->data[byte - 1];

		for (bit = 0; bit <= 7; bit++) {
			if (p & (1 << bit))
			    action |= ipi3_decode_substatus(rsp, id, byte, bit);
		}
	}

	/* check for extended substatus */
	if ((len = prm->length) > 5)
		action |= ipi_vendor_extended_substatus(tgt, id, prm);

	return action;
}


/*
 * Table of the official IPI-3 status messages
 * Last update:
 *	ISO/IEC 9318-3 : 1990 (E)
 */
static struct major_status_msg {
	unsigned char	id;		/* low-order four bits	 */
	unsigned char	octet;		/* byte offset in param	 */
	unsigned char	bit;		/* bit offset in byte	 */
	unsigned int	action;		/* error recovery action */
	char		*means;		/* error message string	 */
} status_msg[] = {

    /*
     * ID 0x13 or 0x23: Message/Microcode Exception Substatus
     */
    { 0x03, 1, 0, ACTION_ERROR, "Port Response" },
    { 0x03, 1, 1, ACTION_ERROR, "Port Disable Pending" },
    { 0x03, 1, 2, ACTION_ERROR, "Failure Message" },
    { 0x03, 1, 3, ACTION_ERROR, "Microcode Execution Error" },
    { 0x03, 1, 4, ACTION_ERROR, "Message" },
    { 0x03, 1, 5, ACTION_ERROR, "Slave Unable to IML" },
    { 0x03, 1, 6, ACTION_ERROR, "Request Master to IML Slave" },
    { 0x03, 1, 7, ACTION_ERROR, "Microcode Data Not Accepted" },
    { 0x03, 2, 7, ACTION_ERROR, "Facility Status" },

    /*
     * ID 0x14 or 0x24: Intervention Required Substatus
     */
    { 0x04, 1, 1, ACTION_ERROR, "Addressee Busy" },
    { 0x04, 1, 2, ACTION_ERROR, "Attribute Table may be Corrupted" },
    { 0x04, 1, 3, ACTION_ERROR, "Physical Link Failure" },
    { 0x04, 1, 4, ACTION_ERROR, "Not Ready Transition" },
    { 0x04, 1, 5, ACTION_ERROR, "Not P-Availiable Transition" },
    { 0x04, 1, 6, ACTION_ERROR, "Not Ready" },
    { 0x04, 1, 7, ACTION_ERROR, "Not P-Available" },

    /*
     * ID 0x15 or 0x25: Alternate Port Exception Substatus
     */
    { 0x05, 1, 3, ACTION_ERROR, "Facility Switched to Another Port" },
    { 0x05, 1, 4, ACTION_ERROR, "Format Completed" },
    { 0x05, 1, 5, ACTION_ERROR, "Initialization Completed" },
    { 0x05, 1, 6, ACTION_ERROR, "Attributes Updated" },
    { 0x05, 1, 7, ACTION_ERROR, "Priority Reserve Issued" },
    { 0x05, 2, 6, ACTION_ERROR, "Slave Diagnostic Terminated" },
    { 0x05, 2, 7, ACTION_ERROR, "Slave Diagnostic in Process" },

    /*
     * ID 0x16 or 0x26: Machine Exception Substatus
     */
    { 0x06, 1, 0, ACTION_ERROR, "Power Fail Alert" },
    { 0x06, 1, 1, ACTION_ERROR, "Environmental Error" },
    { 0x06, 1, 2, ACTION_RESET, "Slave Initiated Reset" },
    { 0x06, 1, 3, ACTION_RETRY, "Physical Interface Check" },
    { 0x06, 1, 4, ACTION_ERROR, "Operation Timeout" },
    { 0x06, 1, 5, ACTION_ERROR, "Ready Transition" },
    { 0x06, 1, 6, ACTION_ERROR, "P-Available Transition" },
    { 0x06, 1, 7, ACTION_ERROR, "Addressee No Longer Busy" },
    { 0x06, 2, 2, ACTION_ERROR, "Command Failure" },
    { 0x06, 2, 3, ACTION_QUEUE, "Queue Full" },
    { 0x06, 2, 4, ACTION_ERROR, "Hardware Write Protected" },
    { 0x06, 2, 5, ACTION_RETRY, "Fatal Error" },
    { 0x06, 2, 6, ACTION_BBR,	"Uncorrectable Data Check (on Perfect Data)" },
    { 0x06, 2, 7, ACTION_ERROR, "Data Check (on Raw Data)" },
    { 0x06, 3, 0, ACTION_RETRY, "Unexpected Master Action" },
    { 0x06, 3, 1, ACTION_ERROR, "End of Extent Detected" },
    { 0x06, 3, 2, ACTION_ERROR, "End of Media Detected" },
    { 0x06, 3, 3, ACTION_ERROR, "Reallocation Space Exhausted" },
    { 0x06, 3, 4, ACTION_RETRY, "Data Overrun" },
    { 0x06, 3, 5, ACTION_ERROR, "Write Access Violation" },
    { 0x06, 3, 6, ACTION_ERROR, "Read Access Violation" },
    { 0x06, 4, 4, ACTION_ERROR, "Position Lost" },
    { 0x06, 4, 5, ACTION_RETRY, "Logical Link Failure" },
    { 0x06, 4, 6, ACTION_ERROR, "Defect Directory Full" },
    { 0x06, 4, 7, ACTION_ERROR, "Error Log Full" },

    /*
     * ID 0x17 or 0x27: Command Exception Substatus
     */
    { 0x07, 1, 0, ACTION_ERROR, "Invalid Modifier" },
    { 0x07, 1, 1, ACTION_ERROR, "Invalid Opcode" },
    { 0x07, 1, 3, ACTION_ERROR, "Invalid Selection Address" },
    { 0x07, 1, 4, ACTION_ERROR, "Invalid Facility Address" },
    { 0x07, 1, 5, ACTION_ERROR, "Invalid Slave Address" },
    { 0x07, 1, 6, ACTION_ERROR, "Invalid Command Reference Number" },
    { 0x07, 1, 7, ACTION_ERROR, "Invalid Packet Length" },
    { 0x07, 2, 0, ACTION_ERROR, "Invalid Combination" },
    { 0x07, 2, 1, ACTION_ERROR, "Reserved Value Not Equal to Zero" },
    { 0x07, 2, 2, ACTION_ERROR, "Missing Parameter(s)" },
    { 0x07, 2, 3, ACTION_ERROR, "Invalid Parameter(s)" },
    { 0x07, 2, 4, ACTION_ERROR, "Out of Context" },
    { 0x07, 2, 5, ACTION_ERROR, "Invalid Extent" },
    { 0x07, 3, 7, ACTION_ERROR, "Not at Initial Position" },

    /*
     * ID 0x18 or 0x28: Command Aborted Substatus
     */
    { 0x08, 1, 1, ACTION_ERROR, "Unexecuted Command from Terminated Order" },
    { 0x08, 1, 2, ACTION_ERROR, "Command Order Terminated" },
    { 0x08, 1, 3, ACTION_ERROR, "Unexecuted Command from Terminated Chain" },
    { 0x08, 1, 4, ACTION_ERROR, "Command Chain Terminated" },
    { 0x08, 1, 5, ACTION_ERROR, "Unexecuted Command from Terminated Sequence" },
    { 0x08, 1, 6, ACTION_ERROR, "Command Sequence Terminated" },
    { 0x08, 1, 7, ACTION_ERROR, "Command Aborted" },

    /*
     * ID 0x19 or 0x29: Conditional Success Substatus
     */
    { 0x09, 1, 0, ACTION_SUCCESS, "Re-allocation Discontinuity" },
    { 0x09, 1, 1, ACTION_SUCCESS, "Re-allocation Required" },
    { 0x09, 1, 2, ACTION_SUCCESS, "Anticipated Data Error" },
    { 0x09, 1, 3, ACTION_SUCCESS, "Anticipated Error" },
    { 0x09, 1, 4, ACTION_SUCCESS, "Abort Received: Not Operational" },
    { 0x09, 1, 5, ACTION_SUCCESS, "Abort Received: Status Pending" },
    { 0x09, 1, 6, ACTION_SUCCESS, "Abort Received: No Active Command" },
    { 0x09, 1, 7, ACTION_SUCCESS, "Logging Data Appended" },
    { 0x09, 2, 0, ACTION_SUCCESS, "Request Diagnostic Control Command" },
    { 0x09, 2, 1, ACTION_SUCCESS, "Release of Unreserved Addressee" },
    { 0x09, 2, 2, ACTION_SUCCESS, "Soft Error" },
    { 0x09, 2, 3, ACTION_SUCCESS, "Data Correction Performed" },
    { 0x09, 2, 4, ACTION_SUCCESS, "Motion Retry Performed" },
    { 0x09, 2, 5, ACTION_SUCCESS, "Data Retry Performed" },
    { 0x09, 2, 6, ACTION_SUCCESS, "Error Retry Performed" },
    { 0x09, 2, 7, ACTION_SUCCESS, "Defect Directory Threshold Exceeded" },
    { 0x09, 3, 0, ACTION_SUCCESS, "Master-Terminated Transfer" },
    { 0x09, 3, 1, ACTION_SUCCESS, "Asynchronous Event Occurrence" },
    { 0x09, 3, 2, ACTION_SUCCESS, "Parameter Update Requested" },
    { 0x09, 3, 3, ACTION_SUCCESS, "Statistics Update Requested" },
    { 0x09, 3, 4, ACTION_SUCCESS, "End of Media Warning" },
    { 0x09, 3, 5, ACTION_SUCCESS, "Retention Required" },
    { 0x09, 3, 6, ACTION_SUCCESS, "Non-Interchange Volume" },
    { 0x09, 3, 7, ACTION_SUCCESS, "Error Log Request" },

    /*
     * ID 0x1a or 0x2a: Incomplete Substatus
     */
    { 0x0a, 1, 3, ACTION_ERROR, "Select Subservient Slave" },
    { 0x0a, 1, 4, ACTION_ERROR, "Response Packet Truncated" },
    { 0x0a, 1, 5, ACTION_ERROR, "COPY Source Space Empty" },
    { 0x0a, 1, 7, ACTION_ABORT, "Command May Be Resumed" },
    { 0x0a, 2, 4, ACTION_ERROR, "Link Not Connected" },
    { 0x0a, 2, 5, ACTION_ERROR, "Connect Identifier Already Assigned" },
    { 0x0a, 2, 6, ACTION_ERROR, "Disconnect Unsuccessful" },
    { 0x0a, 2, 7, ACTION_ERROR, "Connect Unsuccessful" },
    { 0x0a, 3, 1, ACTION_ERROR, "Block Not Found" },
    { 0x0a, 3, 2, ACTION_ERROR, "Data Length Difference" },
    { 0x0a, 3, 3, ACTION_ERROR, "Unrecorded Media" },
    { 0x0a, 3, 4, ACTION_ERROR, "Block Length Difference" },
    { 0x0a, 3, 5, ACTION_ERROR, "End of Extent Detected" },
    { 0x0a, 3, 6, ACTION_ERROR, "End of Media Warning (EMW)" },
    { 0x0a, 3, 7, ACTION_ERROR, "Beginning of Media (BOM) Detected" },

    { 0,    0, 0, 0,		0 }
};

ipi3_decode_substatus(rsp, param_id, octet, bit)
	ipi_response_t 	*rsp;
	int		param_id;
	int		octet;
	int		bit;
{
	int					id;
	register struct major_status_msg	*msg;

	id = param_id & 0x0f;

	for (msg = status_msg; msg->means; msg++) {
		if ((msg->id	!= id)	  ||
		    (msg->octet != octet) ||
		    (msg->bit	!= bit)) continue;

		printf("\t%s\n", msg->means);

		/* Check for Command Exception errors */
		if (id == 7 && octet == 2) {
			if (bit == 2)		/* Missing parameters */
				ipi3_param_error(rsp, IPI_PARAM_MISSING_PARAM);
			else if (bit == 3)	/* Invalid parameters */
				ipi3_param_error(rsp, IPI_PARAM_INVALID_PARAM);
		}

		return msg->action;
	}

	printf("\tReserved Bit Set: Substatus ID 0x%02x, Octet %d, Bit %d\n",
		param_id, octet, bit);

	return ACTION_RETRY;
}


ipi3_param_error(rsp, id)
	ipi_response_t 	*rsp;
	int		id;
{
	ipi_param_t	*param = NULL;
	int		len, i;

	/*
	 * Display parameter error details
	 */

	param = (ipi_param_t *)ipi_param(rsp, param, id);
	if (param == NULL)
		return;

	len = param->length;

	if (id == IPI_PARAM_MISSING_PARAM) {
		printf("\t\tMissing Parameter(s):\n\t\t\t");

		len--;
		for (i = 0; i < len; i++) {
			printf("0x%02x ", param->data[i]);
			if ((i % 8) == 7)
				printf("\n\t\t\t");
		}
	} else {
		ipi_invalid_param_t	*prm = (ipi_invalid_param_t *)param;

		printf("\t\tInvalid Parameter(s):\n\t\t\t");
		printf("\t\t\tParameter offset: %d\n",
			prm->param_offset_msb << 8 | prm->param_offset_lsb);
		printf("\t\t\tField offset:     %d\n",
			prm->field_offset_msb << 8 | prm->field_offset_lsb);
		printf("\t\t\tParameter Data:\n");

		len -= 5;
		for (i = 0; i < len; i++) {
			printf("0x%02x ", prm->param_data[i]);
			if ((i % 8) == 7)
				printf("\n\t\t\t");
		}
	}
}

#endif	NIPI > 0
