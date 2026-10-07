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
 Name:		$RCSfile: ccwrk.h,v $
 Version:	$Id: ccwrk.h,v 1.2.4.1 1995/06/11 23:25:00 kat Exp $
  Title:	
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 23:25:00 $
		(last change by: $Author: kat $)
  Programmer:	xxx
  Documents:	


  COPYRIGHT 1992, NCR Corporation

  Description:
*/
/*****************************************************************************
                   COMMON CONTROLLER WORK STRUCTURE

                        Include file:  $RCSfile: ccwrk.h,v $
                        Version:       $Revision: 1.2.4.1 $
                        Change date:   $Date: 1995/06/11 23:25:00 $  

                  Copyright (C) 1987, NCR Corporation
                 Engineering & Manufacturing - Wichita
                      Network Products Development
                            Wichita, Kansas

*****************************************************************************/

#define H_ID "@(#)$RCSfile	$Revision: 1.2.4.1 $  Include file delta date:  $Date: 1995/06/11 23:25:00 $"
#undef H_ID


struct ipcwrk 
{
    unsigned char    ipc_version;   /* version number of IPC used by board */
    unsigned short    srb_count;    /* the number of request blocks in shared 
                                       memory as allocated by the board */
    unsigned short    srb_length;   /* the length of the shared memory 
                                       request block */
    unsigned short    id_code;      /* index associated with the PMC used for 
                                       reference to fields in the shared 
                                       memory on the board */
    unsigned short    *srb_send_list;/* address in shared memory to the list 
                                       head for request blocks sent to the 
                                       board */
    unsigned short    *srb_recv_list;/* address in shared memory to the list 
                                       head for request blocks received from 
                                       the board */
    unsigned short    first_block_number;/* number of first request block assigned 
                                       to the PMC by the board */
    struct  SRB *first_block;       /* offset of first request block assigned 
                                       to the PMC by the board */
    int     (*router_recv)();       /* address of function called when the 
                                       ISR receives a request block */
    void    (*check_release)();     /* address of function called when a 
                                       shared memory request block is 
                                       released */
    unsigned short    board_status; /* the status unsigned short from the board's 
                                       shared memory if the board failed its 
                                       level 0 diagnostics */

	short	opening_user_id;		/* take from u.u_uid at open time */
	unsigned char   board_present;	
    unsigned char   reset_in_progress;/* indicator of whether board is being
                                       reset under control of the driver */
	unsigned char	driver_open_flag;/* indicator of whether "device" is open
									   by a user for ioctl access */
	unsigned char   ioctl_busy;		/* ioctl function call is in progress */
	unsigned char	config_defined_flag;	
	unsigned char	registered_flag;
	unsigned char 	host_tag;
	unsigned short	new_block_code;
	unsigned short	free_block_code;
	unsigned short	reg_cmd;
	unsigned short	reg_stat;
	unsigned short	reg_code;
	unsigned short	*to_board_list_entry;
	unsigned short	*to_host_list_entry;
	unsigned short	*reg_list_entry;
#ifdef CCDRIVER
	struct	IPCMEM 	*ipcmem;
	struct	SRB		*base_block;
	struct	SRB		*next_free_block;
#else
	unsigned char 	*ipcmem;
	unsigned char 	*base_block;
	unsigned char 	*next_free_block;
#endif
	struct	CCSTATS	ipcstats;			/* IPC statistics record */
	unsigned char	*lpfmr_stat_record;	/* Msg Router statistics record adr */
	unsigned short	lpfmr_stat_length;	/* byte length of the record */
};
