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
#ident	"@(#)usr/src/arch/mca/uts/i386/sys/scsi:processor.h	1.3"

#ifndef SYS_PROCESSOR_H
#define SYS_PROCESSOR_H
#include "sys/errno.h"
#include "sys/scsi/processor_d.h"

/*	STUBS for the Processer Device Driver	      */

cspr_close(){}
cspr_read(){}
cspr_write(){}
cspr_init(){}
cspr_ioctl(){}
cspr_size(){}
cspr_pfr(){}

errno_t
cspr_open()
{
	return((errno_t)ENXIO);
}
SCSI_Op_t *
cspr_strategy()
{
	return((SCSI_Op_t *)NULL);
}

SCSI_2_Processor_Read()
{
	return(0);
}

SCSI_2_Processor_Write()
{
	return(0);
}


struct Processor_Cmd_Page            SCSI_2_Processor_Cmd_Page = {
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
	SCSI_2_Null_Function,	/* Receive			*/
	SCSI_2_Null_Function,	/* Send			*/
	SCSI_2_User_0,
	SCSI_2_User_1,
	SCSI_2_User_2,
	SCSI_2_User_3,
	SCSI_2_User_4,
	SCSI_2_User_5
	};
#endif
