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
 Name:		$RCSfile: dlerror.h,v $
 Version:	$Id: dlerror.h,v 1.2.4.1 1995/06/11 23:25:04 kat Exp $
  Title:	Download error definitions
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 23:25:04 $
		(last change by: $Author: kat $)
  Programmer:	rmj
  Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


  COPYRIGHT 1992, NCR Corporation

  Description:	This module contains the download error definitions.
*/

/******************************************************************************
 ***				VARIABLE DEFINITIONS			    ***
 *****************************************************************************/

#define H_ID "@(#)$RCSfile	$Revision: 1.2.4.1 $  Include file delta date:  $Date: 1995/06/11 23:25:04 $"
#undef H_ID

/* ERROR TYPES
	[0]		status
	[1]		expected
	[2]		range
	[3]		or
	[4]		found
	[5]		IPC status
	[6]		CPF status
	[7]		File name
	[8]		Character
	
Flags:
	0x001 = status		0x010 = found			0x100 = character
	0x002 = expected	0x020 = IPC status    	0x200 = undefined
	0x004 = range		0x040 = CPF status     	0x400 = undefined
	0x008 = or			0x080 = File name      	0x800 = undefined
*/
#define	DL_MAX_ERROR_TYPES	9

#define	dl_main_err		0x00	/* MAIN error base */
#define	error_main		0x00
#define	dl_file_err		0x20	/* FILE error base */
#define	error_file		0x02
#define	dl_xfer_err		0x40	/* XFER error base */
#define	error_xfer		0x04

/* extra error messages to be displayed */
#define dle_status		0x0010
#define dle_expecting	0x0020
#define dle_range		0x0040
#define	dle_or			0x0080
#define dle_found		0x0100
#define	dle_erf			dle_expecting+dle_range+dle_found
#define	dle_ef			dle_expecting+dle_found
#define	dle_eof			dle_expecting+dle_or+dle_found
#define	dle_ipc			0x0200
#define	dle_cpf			0x0400
#define	dle_file		0x0800
#define	dle_char		0x1000

/* offsets into errval[] */
#define dle_status_offset		0
#define dle_expecting_offset	1
#define dle_range_offset		2
#define	dle_or_offset			3
#define dle_found_offset		4
#define	dle_ipc_offset			5
#define	dle_cpf_offset			6
#define	dle_file_offset			7
#define	dle_char_offset			8
