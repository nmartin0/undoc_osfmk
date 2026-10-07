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
 * $Log: ipi_labels.c,v $
 * Revision 1.3  1994/11/18  20:50:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/10/04  16:07:49  jerrie
 * Initialize the internally allocated ior->io_device field to zero to
 * prevent device_read_alloc_sg() from calling ipi_devinfo() to determine
 * the device block size.  The device_read_alloc_sg() function is handed
 * the correct block size as a function argument.
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
 * Revision 1.1  1994/06/08  16:53:48  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_labels.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Routines for various vendor's disk label formats.
 */

#include <ipi.h>
#if	NIPI > 0

#include <ipi/ipi_compat.h>
#include <ipi/ipi_endian.h>
#include <ipi/ipi_labels.h>
#include <ipi/ipi_defs.h>


/*
 * Find and convert a DEC label to BSD
 */
static boolean_t
ipi_dec_label(tgt, cmd_q, label)
	target_info_t		*tgt;
	cmd_queue_t		*cmd_q;
	struct disklabel	*label;
{
	/* Look for a DEC label */
	io_req_t			ior = cmd_q->ior;	
	register ipi_dec_label_t	*part;
	char				*data;
	int				i;

	ior->io_count = tgt->block_size;
	ior->io_op    = IO_READ | IO_SGLIST | IO_INTERNAL;

	ipi3_read(tgt, cmd_q, DEC_LABEL_BYTE_OFFSET / tgt->block_size);
	iowait(ior);

	if (ior->io_op & IO_ERROR)
		return FALSE;

	data = (char *)ior->io_data;
	part =
	  (ipi_dec_label_t *)&data[DEC_LABEL_BYTE_OFFSET % tgt->block_size];

	if (part->magic == DEC_LABEL_MAGIC) {
		if (ipi_debug)
			printf("%s%d: Using DEC label\n", 
				(*tgt->dev_ops->driver_name)(TRUE),
				tgt->target_id);

		for (i = 0; i < 8; i++) {
			label->d_partitions[i].p_size =
				part->partitions[i].n_sectors;
			label->d_partitions[i].p_offset =
				part->partitions[i].offset;
		}

		return TRUE;
	}

	return FALSE;
}


/*
 * Find and convert a Omron label to BSD
 */
static boolean_t
ipi_omron_label(tgt, cmd_q, label)
	target_info_t		*tgt;
	cmd_queue_t		*cmd_q;
	struct disklabel	*label;
{
	/* Look for an Omron label */
	io_req_t			ior = cmd_q->ior;	
	register ipi_omron_label_t	*part;
	char				*data;
	int				i;

	ior->io_count = tgt->block_size;
	ior->io_op    = IO_READ | IO_SGLIST | IO_INTERNAL;

	ipi3_read(tgt, cmd_q, OMRON_LABEL_BYTE_OFFSET / tgt->block_size);
	iowait(ior);

	if (ior->io_op & IO_ERROR)
		return FALSE;

	data = (char *)ior->io_data;
	part =
	  (ipi_omron_label_t *)&data[OMRON_LABEL_BYTE_OFFSET % tgt->block_size];

	if (part->magic == OMRON_LABEL_MAGIC) {
		if (ipi_debug)
			printf("%s%d: Using OMRON label\n", 
				(*tgt->dev_ops->driver_name)(TRUE),
				tgt->target_id);

		for (i = 0; i < 8; i++) {
			label->d_partitions[i].p_size =
				part->partitions[i].n_sectors;
			label->d_partitions[i].p_offset =
				part->partitions[i].offset;
		}

		bcopy(part->packname, label->d_packname, 16);
		label->d_ncylinders = part->ncyl;
		label->d_acylinders = part->acyl;
		label->d_ntracks    = part->nhead;
		label->d_nsectors   = part->nsect;
		/* Many disks have this wrong, therefore... */
#if 0
		label->d_secperunit = part->maxblk;
#else
		label->d_secperunit = label->d_ncylinders *
				      label->d_ntracks	  *
				      label->d_nsectors;
#endif
		return TRUE;
	}

	return FALSE;
}


/*
 * Find and convert a Intel BIOS label to BSD
 */
static boolean_t
ipi_bios_label(tgt, cmd_q, label)
	target_info_t		*tgt;
	cmd_queue_t		*cmd_q;
	struct disklabel	*label;
{
	/* Look for a BIOS label */
	io_req_t			ior = cmd_q->ior;	
	register ipi_bios_label_t	*part;
	char				*data;
	int				i;

	ior->io_count = tgt->block_size;
	ior->io_op    = IO_READ | IO_SGLIST | IO_INTERNAL;

	ipi3_read(tgt, cmd_q, BIOS_LABEL_BYTE_OFFSET / tgt->block_size);
	iowait(ior);

	if (ior->io_op & IO_ERROR)
		return FALSE;

	data = (char *)ior->io_data;
	part =
	  (ipi_bios_label_t *)&data[BIOS_LABEL_BYTE_OFFSET % tgt->block_size];

	if (part->magic == BIOS_LABEL_MAGIC) {
		struct bios_partition_info *bpart;

		if (ipi_debug)
			printf("%s%d: Using BIOS label\n", 
				(*tgt->dev_ops->driver_name)(TRUE),
				tgt->target_id);

		bpart = (struct bios_partition_info *)part->partitions;
		for (i = 0; i < 4; i++) {
			label->d_partitions[i].p_size	= bpart[i].n_sectors;
			label->d_partitions[i].p_offset = bpart[i].offset;
		}

		label->d_npartitions = 4;
		label->d_bbsize	     = BIOS_BOOT0_SIZE;

		return TRUE;
	}

	return FALSE;
}


/*
 * Try all of the above
 */
boolean_t
ipi_vendor_label(tgt, cmd_q, label)
	target_info_t		*tgt;
	cmd_queue_t		*cmd_q;
	struct disklabel	*label;
{
	io_req_t		ior = cmd_q->ior;	

	/* If for any reason there might be problems someday... */
#ifdef	VENDOR_LABEL
	if (VENDOR_LABEL(tgt, cmd_q, label)) return TRUE;
	if (ior->io_op & IO_ERROR) return FALSE;
#endif

	if (ipi_dec_label(tgt, cmd_q, label)) return TRUE;
	if (ior->io_op & IO_ERROR) return FALSE;

	if (ipi_bios_label(tgt, cmd_q, label)) return TRUE;
	if (ior->io_op & IO_ERROR) return FALSE;

	if (ipi_omron_label(tgt, cmd_q, label)) return TRUE;
	return FALSE;
}


/*
 * Perform some checks and then copy a disk label
 */
ipi_set_label(old_lp, new_lp)
	struct disklabel *old_lp, *new_lp;
{
	if (new_lp->d_magic  != DISKMAGIC ||
	    new_lp->d_magic2 != DISKMAGIC ||
	    (ipi_checksum_label(new_lp) != 0))
		return D_INVALID_OPERATION;

	*old_lp = *new_lp;

	return D_SUCCESS;
}


/*
 * Checksum a disk label
 */
ipi_checksum_label(lp)
	struct disklabel *lp;
{
	register unsigned short *start, *end, sum = 0;

	start = (unsigned short *)lp;
	end = (unsigned short*)&lp->d_partitions[lp->d_npartitions];

	while (start < end)
		sum ^= *start++;

	return sum;
}


/*
 * Write a label to the disk
 */
ipi_write_label(dev, tgt)
	int			dev;
	target_info_t		*tgt;
{
	io_req_t		ior;
	cmd_queue_t		*cmd_q;
	char			*data;
	struct disklabel	*label;
	io_return_t		error;

	io_req_alloc(ior, 0);

	ior->io_unit   = dev;
	ior->io_device = 0;
	ior->io_next   = 0;
	ior->io_prev   = 0;
	ior->io_error  = 0;
	ior->io_count  = tgt->block_size;
	ior->io_op     = IO_READ | IO_INTERNAL;

	/* Allocate wired-down memory for read with scatter/gather list */
	if (device_read_alloc_sg(ior, tgt->block_size) != KERN_SUCCESS) {
		io_req_free(ior);
		return D_NO_MEMORY;
	}

	data = (char *)ior->io_data;

	/* Get a command buffer */
	while ((cmd_q = IPI_GET_CMD(tgt, FALSE)) == NULL) {
		if (ipi_debug)
			printf("%s%d: ipi_write_label %s\n",
				(*tgt->dev_ops->driver_name)(TRUE),
				tgt->target_id,
				"cannot locate a free command buffer");
	}
	cmd_q->ior = ior;

	ipi3_read(tgt, cmd_q, LABELSECTOR);
	iowait(ior);
	if (error = ior->io_error)
		goto ret;

	label = (struct disklabel *)&data[LABELOFFSET % tgt->block_size];
	*label = tgt->dev_info.disk.l;

	ior->io_error = 0;
	ior->io_next  = 0;
	ior->io_prev  = 0;
	ior->io_count = tgt->block_size;
	ior->io_op    = IO_WRITE | IO_SGLIST | IO_INTERNAL;

	ipi3_write(tgt, cmd_q, LABELSECTOR);
	iowait(ior);
	error = ior->io_error;

ret:
	/* Free allocated data buffer */
	kmem_free(kernel_map, data, ior->io_alloc_size);
	io_sglist_free(ior->io_sgp);
	io_req_free(ior);

	return error;
}

#endif	NIPI > 0
