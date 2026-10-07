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
 * $Log: ipi_disk.c,v $
 * Revision 1.7  1995/03/24  22:04:34  jerrie
 * A parameter in a printf statement in ipi_disk_error() was out of order.
 * The printf was attempting to use the I/O transfer count as a string
 * pointer which resulted in a kernel page fault panic.
 *
 *  Reviewer:	   Arlin Davis
 *  Risk:		   Low.  Only affects the IPI disk device driver.
 *  Benefit or PTS #: 12779
 *  Testing:	   Attempted to access an invalid facility to trigger
 * 		   the printf.
 *  Module(s):	   kernel/ipi/ipi_disk.c
 *
 * Revision 1.6  1994/12/16  21:27:05  jerrie
 * Corrected problems with ioctl commands.  The expected size of the ioctl
 * argument parameters was not calculated correctly causing size checks in
 * the driver to fail.  Added more printf's to the disk error handling
 * routine for better tracing.
 *
 *  Reviewer:  Arlin Davis
 *  Risk:	    Low.  Only affects ioctl's to IPI devices.
 *  Benefit or PTS #:  11392
 *  Testing:   Used the test case in the bug, and an Evaluation test to
 * 	    verify all IPI ioctl commands.  Also created a test to
 * 	    verify disklabel setting and writing via the ioctl interface.
 * 	    Tested before and after probing the device where applicable.
 *  Module(s): The following kernel files are being modified:
 * 		kernel/ipi/ipi_disk.c
 * 		kernel/ipi/ipi_master.c
 * 		kernel/ipi/ipi_misc.c
 * 		kernel/ipi/ipi_endian.h
 *
 * Revision 1.5  1994/11/18  20:50:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/10/04  16:05:01  jerrie
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
 * Revision 1.3  1994/07/19  20:59:33  dbm
 * Removed incorrect check from DIOCGDINFO.  This check was causing
 * the ioctl to fail all of the time.  This ioctl function now looks
 * the same as the scsi driver.
 *
 * Revision 1.2  1994/06/28  23:49:27  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.1  1994/06/08  16:53:43  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_disk.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Middle layer of the IPI-3 driver
 *	Driver operations specific to IPI magnetic disk devices
 */

#include <ipi.h>
#if	NIPI > 0

#include <ipi/ipi_compat.h>
#include <ipi/ipi_endian.h>
#include <ipi/ipi_defs.h>
#include <ipi/ipi_map.h>
#include <ipi/ipi-3.h>


/*
 * Label used if the disk does not have one
 */
struct disklabel ipi_default_label =
{
	DISKMAGIC, DTYPE_IPI, 0,
	"IPI", "",
	0, 1, 1, 1, 1, 1, 0, 0, 0,
	5400, 1, 1, 1, 0, 0, 0,
	{0,}, {0,},
	DISKMAGIC, 0,
	8, 65536, 65536,
	{{ -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 },
	 { -1, 0, 1024, FS_BSDFFS, 8, 3 }}
};


char *
ipi_disk_name(internal)
	boolean_t	internal;
{
	return internal ? "imd" : "disk";
}


/*
 * Specialized side of the open routine for magnetic disks
 */
ipi_ret_t
ipi_disk_open(tgt, req)
	target_info_t	*tgt;
	io_req_t	req;
{
	register int 		i;
	extern int		ipi_open_timeout;
	register ipi_ret_t	ret = IPI_RET_SUCCESS;
	io_req_t		ior;
	cmd_queue_t		*cmd_q;
	ipi_response_t		*rsp;
	int			access;
	unsigned int		capacity, blocks_per_cylinder, block_per_track;
	unsigned int		heads, cylinders, rpm, seek_time, switch_time;
	char			*data;
	struct disklabel	*label;

	if (tgt->flags & TGT_ONLINE)
		return IPI_RET_SUCCESS;

	/*
	 * Dummy ior for proper sync purposes
	 */
	io_req_alloc(ior, 0);
	ior->io_unit   = req->io_unit;
	ior->io_device = 0;
	ior->io_next   = 0;
	ior->io_prev   = 0;
	ior->io_count  = 0;

	/*
	 * Bring the unit online, retrying if necessary.
	 * If the target is spinning up we wait for it.
	 */

	if (ipi_debug)
		printf("\n");

	/*
	 * Start the disk and unlock the heads
	 */
	ior->io_error = 0;
	ior->io_op    = IO_INTERNAL;
	ret = ipi_unit_control(tgt, ior, LOAD_HEADS | SPIN_UP);

	if (ret != IPI_RET_SUCCESS) {
		io_req_free(ior);
		return D_DEVICE_DOWN;
	}

	for (i = 0; i < ipi_open_timeout; i++) {

		ior->io_error = 0;
		ior->io_op    = IO_INTERNAL;
		ret = ipi_test_unit_ready(tgt, ior);

		if (ret == IPI_RET_SUCCESS)
			break;

		if (ret == IPI_RET_ERROR ||
		    ret == IPI_RET_DEVICE_DOWN) {
			i = ipi_open_timeout;
			break;
		}

		if (ret == IPI_RET_RETRY) {
			if (i == 5)
				printf("%s%d: %s\n", 
				    (*tgt->dev_ops->driver_name)(TRUE),
				    tgt->target_id,
				    "Waiting to come online...");

			timeout(wakeup, tgt, hz);
			await(tgt);
			continue;
		}
	}

	if (i == ipi_open_timeout) {
		io_req_free(ior);
		return D_DEVICE_DOWN;
	}

	if (i >= 5)
		printf("ready.");

	/* Get a command buffer */
	while ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL);
	cmd_q->ior = ior;

	/*
	 * Perform get attributes with param 0x5b for the
	 * the given device facility to get the type.
	 */
	ior->io_error = 0;
	ior->io_op    = IO_INTERNAL;
	ret = ipi3_get_attribute(tgt, cmd_q,
		IPI_PARAM_DISK_PARTITION_DEFINITION);

	if (ret != IPI_RET_SUCCESS) {
		io_req_free(ior);
		return D_DEVICE_DOWN;
	}

	{
		ipi_disk_part_t	*disk_part;

		rsp = (ipi_response_t *)cmd_q->cmd_ptr;
		disk_part = (ipi_disk_part_t *)(rsp + 1);

		/*
		 * Check for removable media
		 */
		if (disk_part->removable)
			tgt->flags |= TGT_REMOVABLE_MEDIA;
	}

	/*
	 * Get the device status to determine if there
	 * is media present and the media access rights
	 */
	ior->io_error = 0;
	ior->io_op    = IO_INTERNAL;
	ret = ipi_media_access(tgt, ior, &access);

	if (ret != IPI_RET_SUCCESS) {
		io_req_free(ior);
		return D_DEVICE_DOWN;
	}

	/*
	 * Check media access
	 */
	if (access == IPI_NO_ACCESS) {
		printf("%s%d: %s\n",
			(*tgt->dev_ops->driver_name)(TRUE),
			tgt->target_id, "Cannot access media");
		io_req_free(ior);
		return D_DEVICE_DOWN;
	}

	if (access == IPI_WRITE_ONLY) {
		printf("%s%d: %s\n",
			(*tgt->dev_ops->driver_name)(TRUE),
			tgt->target_id, "Cannot read media");
		io_req_free(ior);
		return D_DEVICE_DOWN;
	}

	/*
	 * Check for write protection
	 */
	if (access == IPI_READ_ONLY)
		tgt->flags |= TGT_READONLY;

	/* Lock removable media */
	if (tgt->flags & TGT_REMOVABLE_MEDIA) {
		ior->io_error = 0;
		ior->io_op    = IO_INTERNAL;
		(void)ipi_unit_control(tgt, ior,
			LOAD_HEADS | SPIN_UP | LOCK_CARTRIDGE);
	}

	/*
	 * Find out about the physical disk geometry
	 */

	/*
	 * Do an attributes command for parameter id
	 * 0x51 to determine the disk block size
	 */
	ior->io_error = 0;
	ior->io_op    = IO_INTERNAL;
	ret = ipi3_get_attribute(tgt, cmd_q,
		IPI_PARAM_SIZE_OF_DISK_DATA_BLOCKS);

	if (ret != IPI_RET_SUCCESS) {
		io_req_free(ior);
		return D_DEVICE_DOWN;
	}

	{
		ipi_data_block_t	*data_block;

		rsp = (ipi_response_t *)cmd_q->cmd_ptr;
		data_block = (ipi_data_block_t *)(rsp + 1);

		tgt->block_size = (data_block->size_msb << 24) |
				  (data_block->size_b2  << 16) |
				  (data_block->size_b1  <<  8) |
				  (data_block->size_lsb <<  0);
	}

	if (ipi_debug)
		printf("%s%d: Block size is %d bytes/block\n", 
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id,
			tgt->block_size);

	if (tgt->block_size % DISK_GRANULE) {
		printf(
		"%s%d: Block size %d is not a multiple of DISK_GRANULE (%d)\n", 
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id,
			tgt->block_size, DISK_GRANULE);
		io_req_free(ior);
		return D_DEVICE_DOWN;
	}

	/*
	 * Adjust the maximum I/O count to a multiple of the block size
	 */
	if (tgt->max_io_count != IPI_UNLIMITED_IO_COUNT) {
		assert(tgt->block_size <= tgt->max_io_count);
		tgt->max_io_count -= tgt->max_io_count % tgt->block_size;
	}

	/*
	 * Determine the disk capacity, blocks/cylinder, and blocks/track
	 */
	{
		ipi_num_block_t	*number_of;

		ior->io_error = 0;
		ior->io_op    = IO_INTERNAL;
		ret = ipi_read_capacity(tgt, ior, &number_of);

		if (ret != IPI_RET_SUCCESS) {
			io_req_free(ior);
			return D_DEVICE_DOWN;
		}

		capacity =
			(number_of->blocks_per_partition_msb << 24) |
			(number_of->blocks_per_partition_b2  << 16) |
			(number_of->blocks_per_partition_b1  <<  8) |
			(number_of->blocks_per_partition_lsb <<  0);

		blocks_per_cylinder =
			(number_of->blocks_per_cylinder_msb << 24) |
			(number_of->blocks_per_cylinder_b2  << 16) |
			(number_of->blocks_per_cylinder_b1  <<  8) |
			(number_of->blocks_per_cylinder_lsb <<  0);

		block_per_track =
			(number_of->blocks_per_track_msb << 24) |
			(number_of->blocks_per_track_b2  << 16) |
			(number_of->blocks_per_track_b1  <<  8) |
			(number_of->blocks_per_track_lsb <<  0);
	}

	if (ipi_debug)
		printf("%s%d: Holds %d blocks\n", 
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id,
			capacity);

	/*
	 * Do an attributes command for parameter id
	 * 0x5f to determine the number of cylinders,
	 * number of heads, seek time, rpm, and head
	 * switch time
	 */
	ior->io_error = 0;
	ior->io_op    = IO_INTERNAL;
	ret = ipi3_get_attribute(tgt, cmd_q,
		IPI_PARAM_PHYSICAL_DISK_CONFIGURATION);

	if (ret != IPI_RET_SUCCESS) {
		io_req_free(ior);
		return D_DEVICE_DOWN;
	}

	{
		ipi_disk_config_t	*disk_config;

		rsp = (ipi_response_t *)cmd_q->cmd_ptr;
		disk_config = (ipi_disk_config_t *)(rsp + 1);

		heads =
			(disk_config->heads_per_cylinder_msb << 8) |
			(disk_config->heads_per_cylinder_lsb << 0);

		cylinders =	/* last cylinder including spares */
			(disk_config->last_data_cylinder_msb << 24) |
			(disk_config->last_data_cylinder_b2  << 16) |
			(disk_config->last_data_cylinder_b1  <<  8) |
			(disk_config->last_data_cylinder_lsb <<  0);
		cylinders++;	/* convert to number of cylinders */

		rpm =	/* rotational period in microseconds */
			(disk_config->rotation_time_msb << 24) |
			(disk_config->rotation_time_b2  << 16) |
			(disk_config->rotation_time_b1  <<  8) |
			(disk_config->rotation_time_lsb <<  0);
		rpm *= 60;	/* convert to rpm */

		seek_time =
			(disk_config->average_cyl_seek_time_msb << 24) |
			(disk_config->average_cyl_seek_time_b2  << 16) |
			(disk_config->average_cyl_seek_time_b1  <<  8) |
			(disk_config->average_cyl_seek_time_lsb <<  0);

		switch_time =
			(disk_config->head_switch_time_msb << 24) |
			(disk_config->head_switch_time_b2  << 16) |
			(disk_config->head_switch_time_b1  <<  8) |
			(disk_config->head_switch_time_lsb <<  0);
	}

	if (ipi_debug) {
		printf("%s%d: %d heads, %d cylinders\n",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id,
			heads, cylinders);
		printf("%s%d: %d rpm, %d usec seek, %d usec head switch\n", 
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id,
			rpm, seek_time, switch_time);
	}

	/*
	 * Get partition table off pack
	 */
	ior->io_error = 0;
	ior->io_op    = IO_READ | IO_INTERNAL;

	/* Allocate wired-down memory for read with scatter/gather list */
	if (device_read_alloc_sg(ior, tgt->block_size) != KERN_SUCCESS) {
		io_req_free(ior);
		return D_NO_MEMORY;
	}

	data = (char *)ior->io_data;

	/* First, look for a BSD label */	
	ior->io_count = tgt->block_size;
	ipi3_read(tgt, cmd_q, LABELOFFSET / tgt->block_size);
	iowait(ior);

	if (ior->io_op & IO_ERROR) {
		/* Free the allocated read buffer */
		kmem_free(kernel_map, data, ior->io_alloc_size);
		io_sglist_free(ior->io_sgp);

		io_req_free(ior);
		return D_DEVICE_DOWN;
	}

	{
		/* Search for BSD label, might be a bit further along */
		register int	i;
		boolean_t	found;

		for (i = LABELOFFSET % tgt->block_size, found = FALSE;
		     i < (tgt->block_size - sizeof(struct disklabel));
		     i += sizeof(int)) {
			label = (struct disklabel *)&data[i];
			if (label->d_magic  == DISKMAGIC &&
			    label->d_magic2 == DISKMAGIC) {
				found = TRUE;
				break;
			}
		}
		if (!found) label = 0;
	}

	if (label) {
		if (ipi_debug)
			printf("%s%d: Using BSD label\n", 
				(*tgt->dev_ops->driver_name)(TRUE),
				tgt->target_id);
		tgt->dev_info.disk.l = *label;
	} else {
		/*
		 * Look for a vendor's label, but first
		 * fill in defaults and what we found
		 */

		label = &tgt->dev_info.disk.l;
		*label = ipi_default_label;
		label->d_secsize    = tgt->block_size;
		label->d_nsectors   = block_per_track;
		label->d_ntracks    = heads;
		label->d_ncylinders = cylinders;
		label->d_secpercyl  = blocks_per_cylinder;
		label->d_secperunit = capacity;
		label->d_trkseek    = seek_time;
		label->d_headswitch = switch_time;
		label->d_rpm	    = rpm;

		ior->io_data = data;
		if (!ipi_vendor_label(tgt, cmd_q, label)) {

			if (ior->io_op & IO_ERROR) {
				/* Free the allocated read buffer */
				kmem_free(kernel_map, data, ior->io_alloc_size);
				io_sglist_free(ior->io_sgp);

				io_req_free(ior);
				return D_DEVICE_DOWN;
			}

			printf("\n%s %s%d, %s\n",
				"WARNING: No valid disk label on",
				(*tgt->dev_ops->driver_name)(TRUE),
				tgt->target_id, "using defaults");

			/* Validate partitions a and c for initialization */
			tgt->dev_info.disk.l.d_partitions[0].p_offset = 0;
			tgt->dev_info.disk.l.d_partitions[0].p_size = capacity;
			tgt->dev_info.disk.l.d_partitions[2].p_offset = 0;
			tgt->dev_info.disk.l.d_partitions[2].p_size = capacity;
		}
		label->d_checksum = 0;
		label->d_checksum = ipi_checksum_label(label);
	}

	/* Free the allocated read buffer */
	kmem_free(kernel_map, data, ior->io_alloc_size);
	io_sglist_free(ior->io_sgp);

	io_req_free(ior);

	return IPI_RET_SUCCESS;
}


/*
 * Specialized side of the close routine for magnetic disks
 */
ipi_ret_t
ipi_disk_close(dev, tgt)
	int		dev;
	target_info_t	*tgt;
{
	if (tgt->flags & TGT_REMOVABLE_MEDIA) {
		io_req_t	ior;

		io_req_alloc(ior, 0);		
		ior->io_unit  = dev;
		ior->io_next  = 0;
		ior->io_prev  = 0;
		ior->io_count = 0;
		ior->io_error = 0;
		ior->io_op    = IO_INTERNAL;
		(void)ipi_unit_control(tgt, ior,
			UNLOAD_HEADS | SPIN_DOWN | UNLOCK_CARTRIDGE);
		io_req_free(ior);
	}

	return IPI_RET_SUCCESS;
}


/*
 * Magnetic disk device optimization routine
 */
ipi_ret_t
ipi_disk_optimize(tgt)
	target_info_t	*tgt;
{
	cmd_queue_t		*cmd_q;
	ipi_slave_config_t	*cfg;
	int			ret, retry;

	/* Get a command buffer */
	if ((cmd_q = IPI_GET_CMD(tgt, TRUE)) == NULL) {
		panic("ipi_disk_optimize");
		return IPI_RET_ERROR;
	}
	cmd_q->ior = NULL;

	/*
	 * Send an ATTRIBUTES command with parameter id 0x66.
	 */
	retry = 0;
	do {
		ret = ipi3_get_attribute(tgt, cmd_q,
			IPI_PARAM_SLAVE_CONFIGURATION);
	} while ((ret == IPI_RET_RETRY) && (retry++ <= ipi_retries));

	/* check for errors */
	if (ret != IPI_RET_SUCCESS) {
		printf("slave %d: Error determining slave configuration\n",
			tgt->target_id);
		tgt->flags = 0;
		return ret;
	}

	cfg = (ipi_slave_config_t *)(((ipi_response_t *)cmd_q->cmd_ptr) + 1);

	/*
	 * Are multiplexed data transfers supported?
	 */
	if (cfg->mux_data_transfers) {
		ipi_slave_reconfig_t	rcfg;
		unsigned char		*p;
		int			i;

		/*
		 * The driver/controller doesn't support
		 * multiplexed data transfers. We must
		 * re-configure the slave to disable them.
		 */

		/* Set the fields we don't want changed to 0xff */
		p = (unsigned char *)&rcfg;
		for (i = 0; i < sizeof(ipi_slave_reconfig_t); i++)
			*p++ = 0xff;

		rcfg.length = IPI_PARAM_SLAVE_RECONFIGURATION_LEN;
		rcfg.id	    = IPI_PARAM_SLAVE_RECONFIGURATION;

		/* Set maximum number of multiplexed data transfers to zero */
		rcfg.max_mux_xfer_msb = 0;
		rcfg.max_mux_xfer_lsb = 0;

		retry = 0;
		do {
			ret = ipi3_set_attribute(tgt, cmd_q,
				&rcfg, sizeof(ipi_slave_reconfig_t));
		} while ((ret == IPI_RET_RETRY) && (retry++ <= ipi_retries));

		/* check for errors */
		if (ret != IPI_RET_SUCCESS) {
			/*
			 * Well, you can't blame us for not trying
			 * We can't support this device
			 */
			printf("slave %d: Error setting slave configuration\n",
				tgt->target_id);
			tgt->flags = 0;
			return ret;
		}
	}

	/*
	 * Perform any vendor specific initialization
	 */
	if ((ret = ipi_vendor_disk_optimize(tgt)) != IPI_RET_SUCCESS)
		return ret;

	/*
	 * Set the target flag and clean up
	 */
	tgt->flags |= TGT_FULLY_PROBED;

	return IPI_RET_SUCCESS;
}


/*
 * Magnetic disk strategy routine
 */
ipi_disk_strategy(ior)
	register io_req_t	ior;
{
	ipi_softc_t		*sc;
	register target_info_t	*tgt;
	register int		i = ior->io_unit;
	int			part;
	register unsigned int	rec, max;
	cmd_queue_t		*cmd_q;

	sc = ipi_softc[IPI_CONTROLLER(i)];
	tgt = sc->target[IPI_SLAVE(i)];
	part = IPI_PARTITION(i);

	/*
	 * Validate the request
	 * Must be a scatter/gather request
	 */
	if (!(ior->io_op & IO_SGLIST) && sc->supports_sgio) {
		ior->io_error = D_INVALID_OPERATION;
		ior->io_op   |= IO_ERROR;
		ior->io_residual = ior->io_count;
		iodone(ior);
		return;
	}

	rec = ior->io_recnum;
	max = tgt->dev_info.disk.l.d_partitions[part].p_size;
	if (max == -1)
		max = tgt->dev_info.disk.l.d_secperunit -
			tgt->dev_info.disk.l.d_partitions[part].p_offset;
	i = btodb(ior->io_count + tgt->block_size - 1);
	if (((rec + i) > max) || (ior->io_count < 0) ||
#if 0
	    ((rec <= LABELSECTOR)      &&    /* don't clobber the disklabel */
	     (!(ior->io_op & IO_READ)) &&
	     ((tgt->flags & TGT_WRITE_LABEL) == 0))
#else
	    FALSE
#endif
	    ) {
		ior->io_error = D_INVALID_SIZE;
		ior->io_op   |= IO_ERROR;
		ior->io_residual = ior->io_count;
		iodone(ior);
		return;
	}

	/*
	 * Enqueue operation 
	 */
	i = splipi();

	simple_lock(&tgt->target_lock);
	if ((cmd_q = IPI_GET_CMD(tgt, FALSE)) == NULL) {
		/* Command queue is full, chain requests */
		if (tgt->ior) {
			/*
			 * Find the disk cylinder for disksort 
			 */
			rec +=
			    tgt->dev_info.disk.l.d_partitions[part].p_offset;
			ior->io_residual =
				rec / tgt->dev_info.disk.l.d_secpercyl;

			/* Use disksort to append to the chain */
			disksort(tgt->ior, ior);
		} else {
			/* Chain up first request */
			ior->io_next = 0;
			ior->io_prev = 0;
			tgt->ior = ior;
		}

#ifdef	IPI_STATISTICS
		tgt->statistics.driver_full++;
		if (++tgt->statistics.chain_depth > tgt->statistics.chain_max)
			tgt->statistics.chain_max = tgt->statistics.chain_depth;
#endif

		simple_unlock(&tgt->target_lock);
	} else {
		/* Start the command */
		cmd_q->ior = ior;
		simple_unlock(&tgt->target_lock);

#ifdef	IPI_STATISTICS
		if (++tgt->statistics.queue_depth > tgt->statistics.queue_max)
			tgt->statistics.queue_max = tgt->statistics.queue_depth;
#endif

		ipi_disk_start(tgt, cmd_q, FALSE);
	}

	splx(i);
}


/*
 * Magnetic disk start/completion routine
 */
ipi_disk_start(tgt, cmd_q, done)
	target_info_t		*tgt;
	register cmd_queue_t	*cmd_q;
	boolean_t		done;
{
	register io_req_t	ior = cmd_q->ior;

	if (ior == 0) {
		IPI_REL_CMD(cmd_q);
		return;
	}

	if (done) {

		if (cmd_q->flags & CMD_BBR_ACTIVE)
			return ipi_bbr(tgt, cmd_q, done);

		/* See if we must retry */
		if (cmd_q->result == IPI_RET_RETRY) {
			/*
			 * May want a delay() call here to
			 * give the device some recovery time
			 */
			goto start;
		} else
		/* Got a bus reset?  Pifff... */
		if (cmd_q->result == (IPI_RET_ABORTED | IPI_RET_RETRY)) {
			goto start;
		} else
		/* See if we had errors */
		if (cmd_q->result != IPI_RET_SUCCESS) {

			if (cmd_q->result == IPI_RET_DEVICE_DOWN) {
				ior->io_error = D_DEVICE_DOWN;
				ior->io_op   |= IO_ERROR;
				ior->io_residual = ior->io_count;
			}
			/*
			 * Handle the error.  A TRUE return value indicates
			 * that we should just return; FALSE indicates that
			 * we should fall through and end this request.
			 */
			else if (ipi_disk_error(tgt, cmd_q))
				return;
		}
		/*
		 * That's it!
		 */

		/* Dequeue next request */
		{
			io_req_t	next;

			simple_lock(&tgt->target_lock);
			if (next = tgt->ior)
				tgt->ior = next->io_next;
			simple_unlock(&tgt->target_lock);

#ifdef	IPI_STATISTICS
			if (!(ior->io_op & IO_INTERNAL)) {
				IPI_DRIVER_CHECKOUT(tgt, ior);
				tgt->statistics.queue_depth--;
			}
#endif

			iodone(ior);

			if (next == 0) {
				IPI_REL_CMD(cmd_q);
				if (tgt->flags & TGT_BBR_ACTIVE)
					ipi_bbr_start(tgt);
				return;
			}

			cmd_q->state = CMD_LOCKED;
			cmd_q->ior = next;
			ior = next;
		}

		/*
		 * See if this request was chained up waiting for an abort
		 */
		if (ior->io_error) {
			cmd_q->ior = 0;
			ipi3_abort(tgt, cmd_q, IPI_FACILITY(ior->io_unit),
				ior->io_error);
			ior->io_error = 0;
			return;
		}

#ifdef	IPI_STATISTICS
		/*
		 * Pulled an ior off the chain and queued it
		 */
		tgt->statistics.chain_depth--;
		tgt->statistics.queue_depth++;
#endif
	}

	ior->io_residual = 0;
start:
	return ipi_disk_start_rw(tgt, cmd_q);
}


static boolean_t
ipi_disk_error(tgt, cmd_q)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
{
	io_req_t	ior = cmd_q->ior;
	ipi_response_t	*rsp = (ipi_response_t *)cmd_q->cmd_ptr;
	unsigned int	action;

	/*
	 * Examine the response message and handle errors here.
	 *
	 * A TRUE return value indicates to the start routine that
	 * we should just return.  It does not necessarily mean that
	 * the ior has failed.  For example, it may mean that the
	 * device command queue was full and we have simply chained
	 * the request for later processing.
	 * 
	 * A FALSE return value indicates to the start routine that
	 * that we should fall through and end this request.  The
	 * request may have been marked as an error by this routine
	 * or it may be successful.  For example, a soft error
	 * reported by the device is not considered fatal.
	 *
	 * Basically, we want one of three things out of this routine:
	 *
	 *	Request was successful:	return FALSE
	 *	Request needs a retry:	restart/chain request and return TRUE
	 *	Request was in error:	set io_error and return FALSE
	 *
	 * The major status is handled as follows:
	 *
	 *	Successful		     ok (handled by controller layer)
	 *	Conditional Success	     ok
	 *	Machine Exception	     examine and decide what to do
	 *	Incomplete		     may issue abort, otherwise error
	 *	Command Aborted		     error
	 *	Command Exception	     error unless command was optional
	 *	Intervention Required	     error
	 *	Alternate Port Exception     error
	 *	Message/Microcode Exception  error
	 *
	 * In all cases (except Successful, or Command Exception of an optinal
	 * command) an error message is displayed to indicate the cause of the
	 * condition.  Extended substatus parameters are vendor unique and not
	 * displayed.
	 */

	if (!(ior->io_op & IO_INTERNAL)) {
		printf(
	    "%s%d: Error during %s of %d bytes %s partition '%c' block %d\n",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id,
			(cmd_q->ior->io_op & IO_READ) ? "read" : "write",
			ior->io_count,
			(cmd_q->ior->io_op & IO_READ) ? "from" : "to",
			'a' + IPI_PARTITION(ior->io_unit),
			ior->io_recnum);
	}

	action = ipi3_error(tgt, cmd_q);

	if (action & ACTION_RESET) {
		watchdog_t	*hw = (watchdog_t *)tgt->hw_state;
		wd_t		*wdog = &hw->dog[tgt->target_id];

		/*
		 * An internal condition caused the slave to initiate a reset.
		 * We must assume that all outstanding commands are lost.
		 * Disable the watchdog and call the reset routine to retry
		 * all outstanding requests.
		 */

		/* This command was in progress when the reset occurred */
		cmd_q->state = CMD_IN_PROGRESS;

		wdog->nactive = 0;
		wdog->state   = IPI_WD_INACTIVE;

		printf("%s%d: Restarting active commands...",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id);

		(*hw->reset)(hw, tgt->target_id);

		printf("done.\n");

		return TRUE;
	}

	if (action & ACTION_ABORT) {
		cmd_queue_t	*abort_q;

		/*
		 * This indicates that the command is paused on
		 * the slave's queue, and the command reference
		 * number remains valid until the command is
		 * resumed or aborted.  We will abort the command.
		 */
		cmd_q->state   = CMD_LOCKED;

		printf("%s%d: Aborting command\n",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id);

		/*
		 * The last command buffer is reserved for aborts
		 */
		abort_q = tgt->cmd_q + (tgt->cmd_ref_len - 1);
		if (abort_q->state != CMD_FREE) {
			/*
			 * The last command is busy, presumably with
			 * another abort command.  Chain up this ior
			 * with io_error set to the command reference
			 * number to abort.  The io_error is used as a
			 * flag in the start routine to indicate that
			 * the command reference number needs aborting.
			 */
			ior->io_error = (io_return_t)cmd_q->cmd_ref;

			/*
			 * Chain up the ior at the head of the queue.
			 * The enqueue_head() function assumes the head is
			 * active and actually inserts following the head.
			 * We do this manaully force it to be first.
			 */
			simple_lock(&tgt->target_lock);
			if (tgt->ior) {
				ior->io_next = tgt->ior;
				ior->io_prev = ior->io_next->io_prev;
				ior->io_next->io_prev = ior;
				ior->io_prev->io_next = ior;
			} else {
				ior->io_next = 0;
				ior->io_prev = 0;
				tgt->ior = ior;
			}
			simple_unlock(&tgt->target_lock);

			return TRUE;
		}

		abort_q->state = CMD_LOCKED;
		abort_q->ior   = 0;

		ipi3_abort(tgt, cmd_q, IPI_FACILITY(ior->io_unit),
			cmd_q->cmd_ref);

		return TRUE;
	}

	if (action & ACTION_BBR) {
		ipi_response_extent_t	*rsp_extent;
		unsigned int		blockno;

		rsp_extent = (ipi_response_extent_t *)
			ipi_param(rsp, NULL, IPI_PARAM_RESPONSE_EXTENT);
		if (rsp_extent == NULL) {
			printf(
			"Cannot locate parameter ID 0x%02x in response\n",
				IPI_PARAM_RESPONSE_EXTENT);
			ior->io_error = D_IO_ERROR;
			ior->io_op   |= IO_ERROR;
			ior->io_residual = ior->io_count;
			return FALSE;
		}

		blockno = rsp_extent->address_msb << 24 |
			  rsp_extent->address_b2  << 16 |
			  rsp_extent->address_b1  <<  8 |
			  rsp_extent->address_lsb <<  0;

		cmd_q->state = CMD_LOCKED;

		printf("%s%d: Initiating bad block recovery for block %d\n",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id,
			blockno);

		return ipi_bad_block_replace(tgt, cmd_q, blockno);
	}

	if (action & ACTION_QUEUE) {
		/*
		 * The command queue for the addressee is full.
		 * Mark the command ready and chain it up for later.
		 */
		cmd_q->state = CMD_READY;
		simple_lock(&tgt->target_lock);
		enqueue_tail(&tgt->waiting_cmds, (queue_entry_t)cmd_q);
		simple_unlock(&tgt->target_lock);

		/* Set a flag for future requests */
		tgt->flags |= TGT_QUEUE_FULL;

#ifdef	IPI_STATISTICS
		tgt->statistics.device_full++;
#endif

		printf("%s%d: Device command queue full, restarting\n",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id);

		return TRUE;
	}

	if ((action & ACTION_RETRY) && !(ior->io_op & IO_INTERNAL)) {
		/*
		 * Retry the operation
		 */
		cmd_q->state = CMD_READY;

		printf("%s%d: Retrying command\n",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id);

		ipi_disk_start_rw(tgt, cmd_q);

#ifdef	IPI_STATISTICS
		tgt->statistics.retries++;
#endif

		return TRUE;
	}

	if (action & ACTION_ERROR) {
		ior->io_error = D_IO_ERROR;
		ior->io_op   |= IO_ERROR;
		ior->io_residual = ior->io_count;

		printf("%s%d: Unrecoverable error\n",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id);

		return FALSE;
	}

	if (action & ACTION_SUCCESS) {
		/*
		 * If the Success bit is set we assume all is well.
		 */
		printf("%s%d: Recovered error\n",
			(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id);

		return FALSE;
	}

	/*
	 * All other bits, mark the request as an error and return.
	 */
	ior->io_error = D_IO_ERROR;
	ior->io_op   |= IO_ERROR;
	ior->io_residual = ior->io_count;

	printf("%s%d: Unknown error\n",
		(*tgt->dev_ops->driver_name)(TRUE), tgt->target_id);

	return FALSE;
}


ipi_disk_start_rw(tgt, cmd_q)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
{
	io_req_t	ior = cmd_q->ior;
	unsigned int	part, secno;
	ipi_ret_t	ret;

	part = IPI_PARTITION(ior->io_unit);
	secno = ior->io_recnum +
		tgt->dev_info.disk.l.d_partitions[part].p_offset;

	if (ior->io_op & IO_READ)
		ret = ipi3_read(tgt, cmd_q, secno);
	else if (!(ior->io_op & IO_INTERNAL))
		ret = ipi3_write(tgt, cmd_q, secno);
	else
		panic("Invalid operation");

	return ret;
}


#include <sys/ioctl.h>

io_return_t
ipi_disk_get_status(dev, tgt, flavor, status, status_count)
	int		dev;
	target_info_t	*tgt;
	unsigned int	flavor;
	dev_status_t	status;
	unsigned int	*status_count;
{
	struct disklabel *lp;

	lp = &tgt->dev_info.disk.l;

	switch (flavor) {

		case DEV_GET_SIZE:
			if (*status_count != DEV_GET_SIZE_COUNT)
				return D_INVALID_SIZE;
			status[DEV_GET_SIZE_DEVICE_SIZE] =
				lp->d_partitions[IPI_PARTITION(dev)].p_size *
				lp->d_secsize;
			status[DEV_GET_SIZE_RECORD_SIZE] = lp->d_secsize;
			break;

		case DIOCGDINFO:
			if (*status_count !=
			    sizeof(struct disklabel) / sizeof(int))
				return D_INVALID_SIZE;
			*(struct disklabel *)status = *lp;
			break;

		case DIOMRINFO:
			/* Get the mach record size for this device: */
			if (*status_count != sizeof(int) / sizeof(int))
				return D_INVALID_SIZE;
			*(int *)status = tgt->block_size;
			break;

		default:
 			return D_INVALID_OPERATION;
	}

	return D_SUCCESS;
}


io_return_t
ipi_disk_set_status(dev, tgt, flavor, status, status_count)
	int		dev;
	target_info_t	*tgt;
	unsigned int	flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
	io_return_t	error = D_SUCCESS;
	struct disklabel *lp;

	lp = &tgt->dev_info.disk.l;

	switch (flavor) {

		case DIOCSRETRIES:
			if (status_count != sizeof(int) / sizeof(int))
				return D_INVALID_SIZE;
			ipi_bbr_retries = *(int *)status;
			break;

		case DIOCWLABEL:
			if (status_count !=
			    sizeof(int) / sizeof(int))
				return D_INVALID_SIZE;
			if (*(int *)status)
				tgt->flags |= TGT_WRITE_LABEL;
			else
				tgt->flags &= ~TGT_WRITE_LABEL;
			break;

		case DIOCSDINFO:
		case DIOCWDINFO:
			if (status_count !=
			    sizeof(struct disklabel) / sizeof(int))
				return D_INVALID_SIZE;
			error = ipi_set_label(lp, (struct disklabel *)status);
			if (error || (flavor == DIOCSDINFO))
				return error;
			error = ipi_write_label(dev, tgt);
			break;

		default:
 			error = D_INVALID_OPERATION;
			break;
	}

	return error;
}

#endif	NIPI > 0
