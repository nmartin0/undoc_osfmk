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
#ident	"@(#)./sccs/NSTSRC/relintel/COMMON/usr/src/arch/mca/uts/i386/sys/scsi/scsi_stat.h:scsi_stat.h	1.4"

#ifndef _SYS_SCSI_STAT_H
#define _SYS_SCSI_STAT_H
#include <sys/scsi/cs.h>
#include <sys/elog.h>
#include <sys/types.h>

#define NumberOfTallyCounters	39
/*
 * Statistic structure inside SCSI Driver -- one per device
 */

#ifndef NULL
#define NULL	0
#endif

typedef struct SCSI_Tallies {
	dev_t			DeviceId;
	uint			TallyData[NumberOfTallyCounters];
	struct SCSI_Tallies	*Next_Tally;
} SCSI_Tallies_t;

typedef struct StatStructure {
	struct SCSI_Device	*ForwardPointer;
	struct SCSI_Device	*BackPointer;
	struct iotime		SCSI_Stats;
	SCSI_Tallies_t		SCSI_Tallies;
} StatStructure_t;



/*
 * Constant values defined as indices into "SCSI_Tallies" array.
 */

#define P_Reads					0
#define	P_Writes				1
#define P_ControllerOperations			2
#define P_MiscOperations			3
#define	P_IO_QueueCount				4
#define P_BlockResponse				5
#define	P_RecoveredReadErrors			6
#define P_RecoveredWriteErrors			7
#define	P_AbortMessageReceived			8
#define P_UnrecoveredReadErrors			9
#define P_UnrecoveredWriteErrors		10
#define P_ReadyToNotReadyTransitions		11
#define	P_OperationTimeout			12
#define	P_SelectTimeout				13
#define P_RetryCount				14
#define	P_AbortedCommand			15
#define	P_BlankCheck				16
#define	P_UnitAttention_Media			17
#define	P_UnitAttention_Reset			18
#define	P_UnitAttention_OtherOrUnknown		19
#define	P_MediumError				20
#define P_HardwareError				21
#define P_DataProtect				22
#define P_IllegalRequest			23
#define P_VendorUnique				24
#define P_CheckCondition			25
#define P_Busy					26
#define P_Terminated				27
#define P_Ioctl					28
#define P_UnexpectedDisconnect			29
#define P_UnknownSCSI_Status			30
#define P_UnknownEvent				31
#define P_ParityError				32
#define P_ECC_Correction			33
#define P_PowerFailRecovery			34
#define P_ControllerError			35
#define P_SpuriousInterrupt			36
#define P_BusReset				37
#define P_BusError				38

#endif
