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
#ident	"@(#)./usr/src/arch/mca/uts/i386/sys/scsi:dcb.h	1.3"

#ifndef SYS_DCB_H
#define SYS_DCB_H
#include "sys/types.h"

#define Partitions_Per_Disk		16
#define Extensions_Per_Disk		16
#define Stripe_Descriptors_Per_DPCB	32
#define Mirror_Descriptors_Per_DPCB	16

/*
 * Partition Identifier defines the physical location of a disk partition.
 */

typedef struct partID {
	unsigned int	OS_Major	:8,
		IO_Bus		:3,
		SCSI_Controller	:4,
		SCSI_Bus	:3,
		PUN		:4,
		LUN		:3,
		reserved	:3,
		Partition	:4;
	} Partition_Identifier_t;
/*
 * There is one Disk Partition Control Block for each disk partition. PartitionStart
 * and PartitionSize are 512 byte block numbers relative to block 0 of the disk.
 * NumberOfStripes is 1 for the non-distributed partition else is the number of participants
 * in the distribution. StripeExtentDPCB is the index of the extension dpcb within the array
 * "ExtensionControlBlocks" in the DiskControlBlock_t. Only the array "Stripe"
 * and the StripeExtentDPCB are used in the StripeExtentDPCB. NumberOfMirrors and 
 * MirrorExtentDPCB are synonymous with the similar "Stripe" variables.
 * StripeSize is the number of 512 byte blocks that constitute an instance of a stripe.
 */

typedef struct dpcb {
		daddr_t		Partition_Start;
		int		Partition_Size;
		Partition_Identifier_t	Who_Am_I;
		int		Number_Of_Stripes;
		int		Stripe_Extent_DPCB;
		int		Number_Of_Mirrors;
		int		Mirror_Extent_DPCB;
		int		Stripe_Size;
		Partition_Identifier_t	Stripe[ Stripe_Descriptors_Per_DPCB ];
		Partition_Identifier_t	Mirror[ Mirror_Descriptors_Per_DPCB ];
		int			filler[ 8 ];
	} Disk_Partition_Control_Block_t;

typedef struct dcb {
		Disk_Partition_Control_Block_t	Partition_Control_Blocks[ Partitions_Per_Disk ];
		Disk_Partition_Control_Block_t	Extension_Control_Blocks[ Extensions_Per_Disk ];
	} Disk_Control_Block_t;

/*
 * Coercion macros for transition between dev_t and PartitionIdentifier_t types.
 */

/*
 * Convert an l_dev_t to a PartitionIndentifier_t ( dev can not be type "register" )
 */
#ifdef SVR40
#define Mk_Part_Id( Part_Id, l_dev_t) { \
	Part_Id.OS_Major = l_dev_t->OS_Major; \
	Part_Id.IO_Bus = l_dev_t->IO_Bus; \
	Part_Id.SCSI_Controller = l_dev_t->SCSI_Controller; \
	Part_Id.SCSI_Bus = l_dev_t->SCSI_Bus; \
	Part_Id.LUN = l_dev_t->LUN; \
	Part_Id.PUN = l_dev_t->PUN; \
	Part_Id.Partition = l_dev_t->Partition; \
}
#else
#define Mk_Part_Id(dev) (*(Partition_Identifier_t *)&dev)
#endif

/*
 * Convert a Partition_Identifier_t to an l_dev_t
 */

#ifdef SVR40
#define Mk_Dev_t( l_dev_t, Part_Id ) { \
	l_dev_t->OS_Major = Part_Id.OS_Major; \
	l_dev_t->IO_Bus = Part_Id.IO_Bus; \
	l_dev_t->SCSI_Controller = Part_Id.SCSI_Controller; \
	l_dev_t->SCSI_Bus = Part_Id.SCSI_Bus; \
	l_dev_t->LUN = Part_Id.LUN; \
	l_dev_t->PUN = Part_Id.PUN; \
	l_dev_t->Partition = Part_Id.Partition; \
}
#else
#define Mk_Dev_t(Part_Id) (*(l_dev_t *)&Part_Id)
#endif

typedef struct DiskGeometry {
	int Capacity;
	int NumberOfCylinders;
	int NumberOfHeads;
	int SectorsPerTrack;
	int BytesPerSector;
	int SectorsPerCylinder;
} DiskGeometry_t;

#endif
