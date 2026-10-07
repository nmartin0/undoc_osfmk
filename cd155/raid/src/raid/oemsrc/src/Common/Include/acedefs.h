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
/******************************************************************************
 ***				 IDENTIFICATION				    ***
 ******************************************************************************
  Name:		$RCSfile: acedefs.h,v $
  Version:	$Id: acedefs.h,v 1.2.4.1 1995/06/11 23:28:38 kat Exp $
  Title:	'ace' Definitions
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 23:28:38 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX RAID Manager Release 3.0 FS 348-0024272
		ADP-92xx SCSI DISK ARRAY SOSTWARE RELEASE 3.xx 
		FUNCTIONAL SPECIFICATION 348-0024137

  COPYRIGHT 1992, NCR Corporation

  Description:	Defines, structures, and constants for the 'ace' utility.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: acedefs.h,v $
 * Revision 1.2.4.1  1995/06/11  23:28:38  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1993/05/12  21:05:28  richardg
 * added back the support for multiple luns.
 *
 * Revision 1.1  1992/12/28  18:33:25  richardg
 * Initial revision
 *
 * Revision 1.5  1992/05/15  15:57:39  root
 * sdpr u-119 Added block_size to lun_entry_t - bem
 *
 * Revision 1.4  1992/04/23  16:28:10  root
 * sdpr u-106  Added sharp define REPLACED_RD0_FORMATING
 *
 * Revision 1.3  1992/04/17  18:12:24  root
 * sdpr u-092 Changed drive_list size definition from MAX_DRIVES_IN_GRP to
 * MAX_DRIVES in grp_entry_t definition.   bem
 *
 * Revision 1.2  1992/03/20  17:08:27  bmyers
 * Added define for locking error.
 *
 * Revision 1.1  1992/03/18  19:40:32  root
 * Initial revision
 *
 */

#ifndef	_ACEDEFS_H
#define	_ACEDEFS_H

#include "scsidefs.h"

#define	MAX_LUNS		8

#define MAX_CHANNEL		5
#define MAX_SCSI_ID		7
#define MAX_DRIVES_IN_GRP	10
#define	MAX_DRIVES		( ( MAX_SCSI_ID + 1 ) * MAX_CHANNEL )
#define	MAX_GROUPS		( ( MAX_LUNS * 2 ) + 1 )

#define DRIVE_VIEW		1
#define LUN_VIEW		2

#define FROM_CREATE		1
#define FROM_MAINMENU		2

#define LOCK_ERROR		95
#define CONTINUE		96
#define QUIT			97
#define ALL			98
#define	NONE			99
#define TAB			9
#define SPACE			32
#define NEWLINE			10

/* LUN Type */
#define REGULAR_LUN		0
#define SUB_LUN			1

/* Drive Status */
#define OPTIMAL_DRV		0x00
#define NON_EXIST		0x01
#define SPARE			0x02
#define FAILED			0x03
#define REPLACED		0x04
#define WARNING			0x05
#define MISMATCH		0x06
#define CONTROLLER		0x07
#define FORMATING		0x08
#define WRONGDRV		0x09

/* Drive Actions */
#define ADD_DRIVE		0x80
#define DELETE_DRIVE		0x81
#define FAIL_DRIVE		0x83
#define REPLACE_DRIVE		0x84
#define REPLACE_DRIVE_FORMAT	0x94

/* LUN Status */
#define OPTIMAL			0x00
#define	DEGRADED		0x01
#define REPLACED_DRV_FORMATING	0x41
#define REPLACED_RD0_FORMATING	0x40
#define	RECONSTLUN		0x02
#define	DEAD			0x04

/*===========================================================================*/
/*                      Data Control Structures                              */
/*===========================================================================*/

typedef struct drv_ref {
	int		bold;
	int		status;
	int		group;
	int		drive;			/* chan | id */
} drv_ref_t;


typedef struct lun_entry {
	int		type;			/* F = lun, T = sublun */
	int		real;			/* T = existing lun    */
	int		status;
	int		lun_nbr;		/* lun number in system */
	int		raid_lvl;
	u_int32		block_size;
	u_int32		capacity;

} lun_entry_t;


typedef struct grp_entry {
	int		nbr_luns_in_grp;
	int		nbr_real_luns_in_grp;
	int		nbr_drvs_in_grp;
	int		lun_list[MAX_LUNS];
	lun_entry_t	lun_table[MAX_LUNS];
	drv_ref_t	drive_list[MAX_DRIVES];
} grp_entry_t;

typedef struct grptbl {
	int		group_count;
	int		lun_count;
	int		real_lun_count;
	u_char		pg2a[240];
	grp_entry_t	group[MAX_GROUPS];
} grptbl_t;

#endif
