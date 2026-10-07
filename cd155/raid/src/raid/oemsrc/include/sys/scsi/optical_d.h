/*
 *
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */ 

/* Copyright 1990 NCR Corporation - Dayton, Ohio, USA */
#ident	"@(#)./sccs/NSTSRC/relintel/COMMON/usr/src/arch/mca/uts/i386/sys/scsi/optical_d.h:optical_d.h	1.1"

#ifndef SYS_OPTICAL_D_H
#define SYS_OPTICAL_D_H

struct OM {
	Disk_Control_Block_t	*Disk_Control_Block_Ptr;
	DiskGeometry_t		Geometry;
	int			current_mode;
	int			MaximumBlockNumber;
} OM;
	
struct OM *omptr;

struct Optical_Mem_Cmd_Page {
	struct All_Types_Cmd_Page all;
	struct SCSI_Op	*(*vdev_format)();
	struct SCSI_Op	*(*vdev_read)();
	struct SCSI_Op	*(*vdev_read_defect_data)();
	struct SCSI_Op	*(*vdev_start_stop)();
	struct SCSI_Op	*(*vdev_synchronize_cache)();
	struct SCSI_Op	*(*vdev_verify)();
	struct SCSI_Op	*(*vdev_write)();
	struct SCSI_Op	*(*vdev_user0)();
	struct SCSI_Op	*(*vdev_user1)();
	struct SCSI_Op	*(*vdev_user2)();
	struct SCSI_Op	*(*vdev_user3)();
	struct SCSI_Op	*(*vdev_user4)();
	struct SCSI_Op	*(*vdev_user5)();
	};

/*
 * Optical specific Virtual Device (vdev) Access Macros.
 */

#define VDEV_om_Format(dp, Format_Data, Complete_List, Defect_List_Format, Interleave, Parameter_List, Parameter_List_Length) \
		(*((struct Optical_Mem_Cmd_Page *)((dp)->Cmd_Page))->vdev_format)(dp, Format_Data, \
			Complete_List, Defect_List_Format, Interleave, Parameter_List, \
			Parameter_List_Length)

#define VDEV_om_Write(dp, blk, address, byte_cnt) \
		(*((struct Optical_Mem_Cmd_Page *)((dp)->Cmd_Page))->vdev_write)(dp, blk, address, byte_cnt)

#define VDEV_om_Read(dp, blk, address, byte_cnt) \
		(*((struct Optical_Mem_Cmd_Page *)((dp)->Cmd_Page))->vdev_read)(dp, blk, address, byte_cnt)

#endif
