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
 Name:		$RCSfile: ccerrors.h,v $
 Version:	$Id: ccerrors.h,v 1.2.4.1 1995/06/11 23:24:52 kat Exp $
  Title:	LPF IPC error codes (only used by DAC download for proper 
		compilation)
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 23:24:52 $
		(last change by: $Author: kat $)
  Programmer:	rmj
  Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

  COPYRIGHT 1992, NCR Corporation

  Description:	This module is used by the DAC download only to ensure proper
		compilation.
*/

/******************************************************************************
 ***			       VARIABLE DEFINITIONS 			   ****
 *****************************************************************************/
#define H_ID "@(#)$RCSfile	$Revision: 1.2.4.1 $ Include file delta date:  $Date: 1995/06/11 23:24:52 $"
#undef H_ID

/* "cc_" function return values */
#define CC_SUCCESS		0		/* successful initialization */
#define CC_BADCONFIG 	1		/* invalid configuration parameters */
#define CC_NOTREADY		2		/* board not ready */
#define	CC_NOBOARD		3		/* board does not respond on Multibus */
#define CC_NOTLOADED	4		/* invalid configuration */
#define CC_FAILURE		5		/* failure of bringup diagnostics */
#define CC_TIMEOUT		7		/* timeout */
#define	CC_CANT_REGISTER 8		/* unable to register Unix host */
#define CC_SUSPENDED	9		/* CPF is suspended */
#define	CC_BUSY			10		/* CPF busy with boot process */
#define CC_GEN_ERR		255		/* generic error */
