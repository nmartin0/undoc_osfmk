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
 Name:		$RCSfile: cccfg.h,v $
 Version:	$Id: cccfg.h,v 1.2.4.1 1995/06/11 23:24:45 kat Exp $
  Title:	
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 23:24:45 $
		(last change by: $Author: kat $)
  Programmer:	xxx
  Documents:	


  COPYRIGHT 1992, NCR Corporation

  Description:
*/
/*****************************************************************************

               COMMON CONTROLLER CONFIGURATION STRUCTURE

                        Include file:  $RCSfile: cccfg.h,v $
                        Version:       $Revision: 1.2.4.1 $
                        Change date:   $Date: 1995/06/11 23:24:45 $  

                  Copyright (C) 1987, NCR Corporation
                 Engineering & Manufacturing - Wichita
                      Network Products Development
                            Wichita, Kansas

*****************************************************************************/

#define H_ID "@(#)$RCSfile	$Revision: 1.2.4.1 $  Include file delta date:  $Date: 1995/06/11 23:24:45 $"
#undef H_ID


#define	MAX_BOARDS				4		/* maximum number of boards */

#define	number_SRBs				32

#define	DPRAM_SIZE				2048
#define MBIO_SIZE				256

#ifdef TOWER_XP								/* if Tower XP ... */
#define	SYS_MBIO_BASE			0x1F7000
#define	BRD_MBMEM_BASE			0x7800		/* in MB I/O space */
#define	SYS_MBMEM_BASE			0x1F0000 + BRD_MBMEM_BASE
#define DPRAM_CONTROL			(BRD_MBMEM_BASE >> 8) + 1
#define	TRACE_PAUSE_COUNT		10000
#endif
#ifdef TOWER_32								/* if Tower 32 ... */
#define SYS_MBIO_BASE			0xFF7000
#define	BRD_MBMEM_BASE			0x800000 	/* in MB mem space */
#define	SYS_MBMEM_BASE			0x06000000 + BRD_MBMEM_BASE
#define DPRAM_CONTROL			BRD_MBMEM_BASE >> 8
#define	TRACE_PAUSE_COUNT		30000
#endif

#define	MB_INT_NUMBER			7
#define	MB_INT_VECTOR			0xFFFF
#define	DEFAULT_SRB_COUNT		number_SRBs
#define	DEFAULT_SRB_LENGTH		22
#define	HOST_TAG				0x10
#define	BOARD_TAG				0xE0
#define	TR_BOARD_TYPE			0x0010
#define	REG_THRESHOLD			4		/* Registration busy time */
#define	CCH_THRESHOLD			2		/* CPF Channel busy time */
#define	BSY_THRESHOLD			60		/* Time board is busy with bringup */
#define	ONL_THRESHOLD			4		/* Time until online indicator set */
#define	RST_THRESHOLD			2		/* Time for H/W reset of board,
										   during which no board access is 
										   allowed */

struct ipccfg 
{
	unsigned short	brd_num;
	unsigned char 	*shared_memory_base;
	unsigned char 	*board_dpram_base;
	unsigned char 	*MB_control_base;
	unsigned short	dpram_control_value;
	unsigned short	MB_int_number;
	unsigned short	MB_int_vector_number;
	unsigned short	default_srb_count;
	unsigned short	default_srb_length;
	unsigned char	host_tag;
	unsigned char	board_tag;
	unsigned short	expected_board_type;
	struct						  
	{
		unsigned short  registration;
		unsigned short  CPF_channel;
		unsigned short  busy;
		unsigned short  on_line;
		unsigned short	reset;
	}	threshold;
};
