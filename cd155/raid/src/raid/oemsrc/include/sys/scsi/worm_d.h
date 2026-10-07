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
#ident	"@(#)./sccs/NSTSRC/relintel/COMMON/usr/src/arch/mca/uts/i386/sys/scsi/worm_d.h:worm_d.h	1.1"

#ifndef SYS_WORM_D_H
#define SYS_WORM_D_H

struct WORM_Cmd_Page {
	struct All_Types_Cmd_Page all;
	struct SCSI_Op	*(*vdev_read)();
	struct SCSI_Op	*(*vdev_synchronize)();
	struct SCSI_Op	*(*vdev_write)();
	struct SCSI_Op	*(*vdev_user0)();
	struct SCSI_Op	*(*vdev_user1)();
	struct SCSI_Op	*(*vdev_user2)();
	struct SCSI_Op	*(*vdev_user3)();
	struct SCSI_Op	*(*vdev_user4)();
	struct SCSI_Op	*(*vdev_user5)();
	};

#endif
