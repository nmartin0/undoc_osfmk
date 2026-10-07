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
 Name:		$RCSfile: ccdrv.h,v $
 Version:	$Id: ccdrv.h,v 1.2.4.1 1995/06/11 23:24:48 kat Exp $
  Title:	Common controller drive definitions
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 23:24:48 $
		(last change by: $Author: kat $)
  Programmer:	rmj
  Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

  COPYRIGHT 1992, NCR Corporation

  Description:	This module contains the common controller drive definitions.
*/

/******************************************************************************
 ***				VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
#define H_ID "@(#)$RCSfile	$Revision: 1.2.4.1 $  Include file delta date:  $Date: 1995/06/11 23:24:48 $"
#undef H_ID


/* ======================== TYPE DEFINITIONS ========================== */

typedef unsigned char			byte;		/* 8 bits */
typedef unsigned short int		word;		/* 16 bits */
typedef unsigned long int		lword;		/* 32 bits */
typedef unsigned long int		dword;		/* 32 bits */
typedef short int				sint;		/* 16 bits */
typedef long int				Lint;		/* 32 bits */
typedef byte					*pointer;

typedef Lint  (*FUNCint)();
typedef Lint *(*FUNCintptr)();

typedef word  (*FUNCword)();
typedef word *(*FUNCwordptr)();

typedef void  (*FUNCvoid)();
typedef void *(*FUNCvoidptr)();

#define	FALSE 	0
#define TRUE  	1

#define	OK		0
#define	ERROR	1

#undef	NULL
#define	NULL (pointer) 0


/*============================================================================
			Hardware Controls
============================================================================*/

/* 
    The following definitions identify the offsets from the Multibus I/O
    Base for the board controls.  
    
    **Important**  For byte-accessed control ports, the Tower address 
    must be exclusive-ored with 1 of the specified port address.
    For example, the board reset port is offset 2, as given in the board's
    hardware specification; in the Tower, a write must be applied to 
    offset 3.  For word accessed ports, Tower uses the specified offset
    without change.
*/
/* Output ports */
#define	IO_ATTENTION		0x0000
#define	IO_ACKNOWLEDGE		0x0001
#define	IO_BOARD_RESET		0x0002
#define	IO_IO_RESET			0x0003
#define	IO_DPRAM_BASE		0x0004
#define	IO_DMA_WINDOW		0x0006
#define	IO_FIFO				0x000A
/* Input ports */
#define	IO_STATUS			0x0000
#define	IO_DIAG_DATA		0x0001

/* Control port status (CC_IO_STATUS) bits: */
#define INTREQ			0x80		/* set if interrupt is pending */

#define	MBMEM_SIZE		2048		/* 2048 bytes used in DP RAM area */
#define MBIO_SIZE		256			/* 256 bytes used by board in I/O space */


/*============================================================================
			Shared Memory Format
============================================================================*/
struct IPCMEM 				
{
	word		online_indicator;
	word		cpf_status;
	word		appl_status;
	word		reset_count;
	word		configuration;

	word		channel_status;
	word		channel_command;
	word		channel_interrupt_info;

	word		reg_status;
	word		reg_command;
	word		reg_activity;
	word		reg_data[9];
	word		reg_list[4];

	word		outgoing_list[4];
	word		incoming_list[4];
	word		interchange_buffer[150];
	word		rb_base;
};

/* IPC configuration word bit masks */
#define	NBR_OF_HOSTS	0x0f00		/* maximum number of hosts */
#define FIFO_AVL		0x0080		/* set if FIFO option available */
#define IPC_VERSION		0x003f		/* IPC version number */

/* Board online indicator bit pattern: */
#define BOARD_ONLINE	0x4e1f		/* constant pattern in online reg */

/* CPF status word bits */
#define CPF_BUSY		0x0080		/* set if CPF is busy */
#define CPF_RESET		0x0040		/* set if reset is in progress */
#define CPF_DIAG		0x0020		/* set if level 0's in progress */
#define CPF_CDIAG		0x0010		/* set if cont. diag in progress */
#define CPF_XDIAG		0x0008		/* set if extended diag in progress */
#define CPF_LDREQ		0x0004		/* set if appl load requested */
#define CPF_SUSPEND		0x0002		/* set if boot processing suspended */
#define CPF_FAILED		0x0001		/* set if board failed in startup */


/*============================================================================
			Host Registration Structures and Definitions
============================================================================*/

struct IBCB 					/* Input Board Configuration Block */
{
	word		block_count;
	word		block_size;
	word		my_tag;
	word		default_srb_count;
	word		filler[4];
};

struct OBCB 					/* Output Board Configuration Block */
{
	word		block_count;
	word		first_block_number;
	word		first_block_offset;
	word		filler[5];
};

struct IHCB 					/* Input Host Configuration Block */
{
	word		block_count;
	word		mb_int_number;
	word		mb_int_vector;
	word		filler[5];
};

struct OHCB 					/* Output Host Configuration Block */
{
	word		id_code;
	word		outgoing_vector;	/* from LPF */
	word		incoming_vector;	/* to LPF */
	word		first_block_number;
	word		first_block_offset;
	word		remaining_count;
	word		filler[2];
};

struct BCB 						/* Board Configuration Block (total) */
{
	struct	IBCB	ibcb;
	struct	OBCB	obcb;
};

struct HCB 						/* Host Configuration Block (total) */
{	struct	IHCB	ihcb;
	struct	OHCB	ohcb;
};

/* Registration activity word bits */
#define REG_BUSY		0x0080		/* set if registration in progress */

/* Registration commands */
#define INIT_REG		0x0001		/* initiate registration */
#define TERM_REG		0x0002		/* terminate registration */
#define SET_BOARD_CFG	0x0003		/* set board IPC config */
#define GET_BOARD_CFG	0x0004		/* get board IPC config */
#define SET_HOST_CFG	0x0005		/* set host IPC config */
#define GET_HOST_CFG	0x0006		/* get host IPC config */

/* Registration status codes */
#define RG_SUCCESS		0x0020		/* successful operation */
#define RG_INV_INPT		0x0021		/* invalid input parameter */
#define RG_INSUF_SP		0x0022		/* not enough space for req blks */
#define RG_ALRDY_CFG	0x0023		/* IPC already configured */
#define RG_CFG_REQ		0x0024		/* board configuration required */
#define RG_REG_LIMIT	0x0040		/* no more hosts may register */
#define RG_HCFG_ERR		0x0041		/* host configuration error */
#define RG_BCFG_ERR		0x0042		/* board configuration error */

#define MAX_REG_ATTEMPTS	3		/* maximum attempts to register */

/*============================================================================
			Request Block Format and Definitions
============================================================================*/

struct SRB 						/* 	Shared Memory Request Block */
{
	word control;
	word link;
};

/* Request Block Control word flags */
#define RB_IN_USE 	0x0080
#define RB_ACK 		0x0040
#define RB_MINE 	0x0020
#define RB_ERROR 	0x0010
#define RB_KEEP 	0x0008
#define RB_FREE		0x0004


/*============================================================================
			Miscellaneous Driver Definitions
============================================================================*/

/* Configuration parameters: */
#define MAXREG_ATTEMPTS		5		/* maximum attempted registrations */

/* Event check statuses: */
#define EVENT_OCCURRED 	OK
#define	NO_EVENT		1
#define TIMEOUT			-1		/* timeout before condition seen */

