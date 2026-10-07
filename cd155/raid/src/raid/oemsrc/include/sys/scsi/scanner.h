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
#ident	"@(#)usr/src/arch/mca/uts/i386/sys/scsi:scanner.h	1.2"

#ifndef SYS_SCANNER_H
#define SYS_SCANNER_H
#include "sys/errno.h"
#include "sys/scsi/scanner_d.h"

/*	STUBS for the Scanner Device Driver	      */

cssc_close(){}
cssc_read(){}
cssc_write(){}
cssc_init(){}
cssc_ioctl(){}
cssc_size(){}
cssc_pfr(){}

errno_t
cssc_open()
{
	printf("Scanner Device Driver not installed.\n");
	return((errno_t)ENXIO);
}
SCSI_Op_t *
cssc_strategy()
{
	printf("Scanner Device Driver not installed.\n");
	return((SCSI_Op_t *)NULL);
}

SCSI_2_Scanner_Read()
{
	printf("SCSI_2_Scanner_Read()\n");
}


struct Scanner_Cmd_Page            SCSI_2_Scanner_Cmd_Page = {
		{
		SCSI_2_Test_Unit_Ready,
		SCSI_2_Request_Sense,
		SCSI_2_Inquiry, 
		SCSI_2_Null_Function,	/* Reserve		*/ 
		SCSI_2_Null_Function,	/* Release		*/
		SCSI_2_Mode_Sense,
		SCSI_2_Mode_Select,
		SCSI_2_Send_Diagnostic,
		SCSI_2_Receive_Diagnostic,
		SCSI_2_Null_Function,	/* Prevent/allow	*/
		SCSI_2_Read_Capacity,
		SCSI_2_Null_Function,	/* Reassign		*/
		SCSI_2_Rezero_Unit,
		},
	SCSI_2_Null_Function,	/* Read				*/
	SCSI_2_Null_Function,	/* Set Window			*/
	SCSI_2_User_0,
	SCSI_2_User_1,
	SCSI_2_User_2,
	SCSI_2_User_3,
	SCSI_2_User_4,
	SCSI_2_User_5
	};
#endif
