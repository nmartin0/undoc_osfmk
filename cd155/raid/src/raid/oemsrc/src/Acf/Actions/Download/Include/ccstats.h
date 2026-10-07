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
 Name:		$RCSfile: ccstats.h,v $
 Version:	$Id: ccstats.h,v 1.2.4.1 1995/06/11 23:24:58 kat Exp $
  Title:	
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 23:24:58 $
		(last change by: $Author: kat $)
  Programmer:	xxx
  Documents:	

  COPYRIGHT 1992, NCR Corporation

  Description:
*/
/*****************************************************************************

               COMMON CONTROLLER STATISTICS RECORD FORMAT

                        Include file:  $RCSfile: ccstats.h,v $
                        Version:       $Revision: 1.2.4.1 $
                        Change date:   $Date: 1995/06/11 23:24:58 $  

                  Copyright (C) 1987, NCR Corporation
                 Engineering & Manufacturing - Wichita
                      Network Products Development
                            Wichita, Kansas

*****************************************************************************/

#define H_ID "@(#)$RCSfile	$Revision: 1.2.4.1 $  Include file delta date:  $Date: 1995/06/11 23:24:58 $"
#undef H_ID

struct CCSTATS
{

	unsigned long	board_resets;
	unsigned long	interrupts;
	unsigned long	board_attentions;
	unsigned long	ioctl_commands;
	unsigned long	ioctl_errors;
	unsigned long	channel_commands;
	unsigned long	channel_errors;
	unsigned long	registration_commands;
	unsigned long	registration_errors;
	unsigned long	srbs_allocated;
	unsigned long	srbs_released;
	unsigned long	srb_allocation_failures;
	unsigned long	srbs_sent;
	unsigned long	srbs_received;
	unsigned long	srb_protocol_errors;
	unsigned long	timeouts;
	unsigned long	interrupt_overruns;
};
