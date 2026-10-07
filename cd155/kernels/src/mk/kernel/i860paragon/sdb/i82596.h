/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: i82596.h,v $
 * Revision 1.2.8.1  1995/06/11  18:32:36  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1995/03/14  23:43:28  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	i82596.h
 *	Author:	Jim Chorn
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	11/94
 *
 *	Defines for the Intel 82596-CA Ethernet Controller
 */


#ifndef	_I82596_H_
#define	_I82596_H_

/*
 *	82596-CA Data Structures taken from the i82596 Data Sheet
 *
 *	NOTES:	These data structures are for i82596 linear mode operation
 *		and are *NOT* compatible with 82586 or 82596 segmented modes.
 *		Segmented mode could be used but 82586 compat mode 
 *		drives top address lines to zero and may not be used.
 *		
 *		This code was orginally written with no compatibility
 *		or correspondence to the i82586.h header file. Apon
 *		noticing my prototypical error, I have renamed structure
 *		names to be similar.
 *
 *		This code was not written to be portable to 64 bit platforms
 *		which don't have 32 bit pointers.
 */

/*
 * Port Function selection (from table). These commands are applied to
 * the PORT address of the 82596 LAN controller.
 *
 * Note: See Book: 32-Bit Local Area Network (LAN) Component User's Manual 1992
 *	 for a detailed description of monitor modes (Intel Lit# 296853-001).
 *	 *** highly recommended ***
 */
#define LAN_RESET       0               /* D31-D4=X, D3-D0=0=reset */
#define LAN_SELFTEST(x) ((long)(x) | 1) /* D31-D4=&results, D3-D0=1=selftest */
#define LAN_SCP(x)      ((long)(x) | 2) /* D31-D4=&ascp, D3-D0=2=altscp */
#define LAN_DUMP(x)     ((long)(x) | 3) /* D31-D4=&dump, D3-D0=3=dump */

/*
 * LAN_SELFTEST using the PORT command access definitions.
 */
#define rom_signiture(x)	(*(u_int *)(x))	
#define test_results(x)		((u_int)((x)->ac_next))
#define TR_SELFTEST		0x00001000	/* Self-test Failed */
#define TR_DIAGNOSE		0x00000020	/* Diagnose Failed */
#define TR_BUS_TIMER		0x00000010	/* Bus timer Failed */
#define TR_REGISTER		0x00000008	/* Register Failed */
#define TR_ROM			0x00000004	/* Rom content Failed */

/*
 * System Configuration Pointer (SCP) SYSBUS definitions.
 */
#define SCP_BE		0x80	/* Big Endian data structures */
#define SCP_ALWAYS_ONE	0x40	/* Always One */
#define SCP_LOW_INT	0x20	/* Active Low Interrupt */
#define SCP_DIS_LOCK	0x10	/* Lock disable */
#define SCP_TRG		0x08	/* Throttle timers triggering */
#define SCP_RES_MODE	0x06	/* Sysbus Reserved combination */
#define SCP_LINEAR_MODE	0x04	/* Sysbus 32 bit linear mode */
#define SCP_32SEG_MODE	0x02	/* Sysbus 32 bit segmented mode */
#define SCP_82586_MODE	0x00	/* Sysbus Compatibility mode */

/*
 * System Block Control (SCB) linear mode.
 */

#define SCB_SW_INT	0xF000	/* SCB interrupt cause field */
#define SCB_SW_CX   	0x8000	/* CU Finished a command */
#define SCB_SW_FR   	0x4000	/* RU Finished recv'ing a frame */
#define SCB_SW_CNA  	0x2000	/* CU Left active state */
#define SCB_SW_RNR  	0x1000	/* RU Left ready state */

#define SCB_CUS		0x0700	/* Command Unit Status field */
#define SCB_CUS_IDLE	0x0000	/* CU Is idle */
#define SCB_CUS_SUSP	0x0100	/* CU Is suspended */
#define SCB_CUS_ACTV	0x0200	/* CU Is active */
#define CUS_IS_IDLE(x)	(((x) & SCB_CUS) == SCB_CUS_IDLE)

#define SCB_RUS		0x00F0	/* Recieve Unit Status field */
#define SCB_RUS_IDLE	0x0000	/* RU Is idle */
#define SCB_RUS_SUSP	0x0010	/* RU Is suspended */
#define SCB_RUS_NORESRC	0x0020	/* RU lacks RFD and RBD resources */
#define SCB_RUS_READY	0x0040	/* RU Is ready */
#define SCB_RUS_NORBDS	0x00A0	/* RU no RBD resources */
#define SCB_RUS_NOPRBDS	0x00C0	/* RU no more RBD resources (prefetch)*/
#define RUS_IS_READY(x)	(((x) & SCB_RUS) == SCB_RUS_READY)

#define SCB_SW_T	0x0008	/* bus throttle Timers loaded */

/*
 * SCB Command Word Definitions.
 */
#define SCB_ACK_CX	0x8000  /* Acknowledge Command Unit action command */
#define SCB_ACK_FR	0x4000  /* Acknowledge RU received frame */
#define SCB_ACK_CNA	0x2000  /* Acknowledge CU became not active */
#define SCB_ACK_RNR	0x1000  /* Acknowledge RU became not ready */

#define SCB_CU		0x0700	/* Reserved and doubles as mask */
#define SCB_CU_NOP	0x0000	/* NOP, CU not affected */
#define SCB_CU_START	0x0100	/* Start first command on command block list */
#define SCB_CU_RESUME	0x0200	/* Resume next command on command block list */
#define SCB_CU_SUSPND	0x0300	/* Suspend after current command completes */
#define SCB_CU_ABORT	0x0400	/* Abort current command immediately */
#define SCB_CU_TNEXT	0x0500	/* Load next throttle timer count */
#define SCB_CU_TLOAD	0x0600	/* Load and start bus throttle timers */

#define SCB_RESET	0x0080	/* Software Reset chip */

#define SCB_RU		0x0070	/* (5-7) Reserved and doubles as mask */
#define SCB_RU_NOP	0x0000	/* NOP, RU not affected */
#define SCB_RU_START	0x0010	/* Update RFA pointer, reception enabled */
#define SCB_RU_RESUME	0x0020	/* Resume previously suspended frame */
#define SCB_RU_SUSPEND	0x0030	/* Suspend reception after current frame */
#define SCB_RU_ABORT	0x0040	/* Abort reception immediately */


/*
 * Action Command status word definitions.
 */
#define AC_SW_C		0x8000	/* Command Completed */
#define AC_SW_B		0x4000	/* Block is Busy */
#define AC_SW_OK	0x2000	/* Command Completed OK */
#define AC_SW_A		0x1000	/* Abnormal termination */
#define AC_SW_F		0x0800	/* Failed status bit, diagnose command */
#define AC_GOOD		0xA000	/* C + OK, AC_SW_C + AC_SW_OK */

#define TC_LATECOLL	0x0800	/* Transmit cmd, Late Collision */
#define TC_CARRIER	0x0400	/* Transmit cmd, No Carrier sense during xmit */
#define TC_CTS		0x0200	/* Transmit cmd, No Clear To Send */
#define TC_DMA		0x0100	/* Transmit cmd, DMA underrun */
#define TC_DEFER	0x0080	/* Transmit cmd, due to previous activity */
#define TC_SQE		0x0040	/* Transmit cmd, Signal Heart Beat detected */
#define TC_COLLISION	0x0020	/* Transmit cmd, Max Collisions Reached */
#define TC_COLMASK	0x000F	/* Transmit cmd, Number of Collisions mask */

/*
 * Action Command command word definitions.
 */
#define AC_CW_EL	0x8000	/* marks the End of List */
#define AC_CW_S		0x4000	/* S bit, suspend after command */
#define AC_CW_I		0x2000	/* I bit, interrupt after command */
#define AC_CW_NC	0x0010	/* NC bit, No Crc Insertion Enable */
#define AC_CW_SF	0x0008	/* SF bit, Flexible Memory Data structures */

/*
 * Action Commands
 */
#define AC_NOP		0	/* NOP Command */
#define AC_IA_SETUP	1	/* Individual Address configuration */
#define AC_CONFIGURE	2	/* Configure Unit, See Configure below */
#define AC_MC_SETUP	3	/* Multicast setup */
#define AC_TRANSMIT	4	/* Transmit command */
#define AC_TDR		5	/* Time Domain Reflectonomy command */
#define AC_DUMP		6	/* Dump internal registers command */
#define AC_DIAGNOSE	7	/* Software self test command */
#define ACT_MASK	0x0007	/* Mask for command word */

/*
 * Configure bit definitions.
 *
 */

#define NO_BITS_SET	0x0000	/* No configuration bits in this word set */

/*
 * cfg_word 0 of configuration parameters (bytes 1 and 0)
 *
 * Note:Default mode is Monitor Mode 1, additional definitions are provided
 *	for non-default modes.
 */
#define MONITOR		0xC000	/* 11xx xxxx xxxx xxxx */
#define MONITOR_OFF	0xC000	/* Mon disabled recv enabled, normal */
#define MONITOR1	0x8000	/* Mon 1st opt recv enabled+mon bad */
#define MONITOR2	0x4000	/* Mon 2nd opt recv disabled+mon bad */
#define MONITOR3	0x0000	/* Mon 3rd opt recv disabled+mon bad++*/
#define FIFO_LIM	0x0800	/* xxxx 1000 xxxx xxxx */
#define PREFETCHED	0x0080	/* xxxx xxxx 1xxx xxxx */
#define BYTE_CNT	0x000E	/* Linear mode byte count is 14 */

#ifndef MONITOR_MODE
#define MONITOR_MODE	MONITOR_OFF /* Also used in byte 11 */
#endif

#define CONF_0_WORD	(PREFETCHED + FIFO_LIM + BYTE_CNT + MONITOR_MODE)

/*
 * cfg_word 1 of configuration parameters (bytes 3 and 2)
 */
#define EXT_LPBCK	0x8000	/* 1xxx xxxx xxxx xxxx */
#define INT_LPBCK	0x4000	/* x1xx xxxx xxxx xxxx */
#define PREAM_LEN	0x2000	/* xx10 xxxx xxxx xxxx */
#define AT_LOC		0x0800	/* xxxx 0xxx xxxx xxxx No src address insert */
#define ADR_LEN		0x0600	/* xxxx x110 xxxx xxxx */
#define SAV_BF		0x0080	/* xxxx xxxx 0xxx xxxx */
#define SRDY		0x0040	/* xxxx xxxx x1xx xxxx */
#define SDBE_LOOPBACK	(EXT_LPBCK|INT_LPBCK)

#define CONF_1_WORD	(PREAM_LEN + ADR_LEN + SRDY)
#define DIAG_1_WORD	(PREAM_LEN + ADR_LEN + SRDY + SAV_BF)

/*
 * cfg_word 2 of configuration parameters (bytes 5 and 4)
 */
#define IF_SPACE	0x6000	/* 0110 0000 xxxx xxxx */ /* 96 interframe */
#define BOF_MET		0x0080	/* xxxx xxxx 1xxx xxxx */
#define ACR		0x0000	/* xxxx xxxx x000 xxxx */
#define ACR1		0x0010	/* xxxx xxxx x001 xxxx */
#define ACR2		0x0020	/* xxxx xxxx x010 xxxx */
#define ACR4		0x0040	/* xxxx xxxx x100 xxxx */
#define LIN_PRIO	0x0000	/* xxxx xxxx xxxx x000 */
#define LIN_PRIO1	0x0001	/* xxxx xxxx xxxx x001 */
#define LIN_PRIO2	0x0002	/* xxxx xxxx xxxx x010 */
#define LIN_PRIO4	0x0004	/* xxxx xxxx xxxx x100 */

#define CONF_2_WORD	(IF_SPACE + ACR + LIN_PRIO)

/*
 * cfg_word 3 of configuration parameters (bytes 7 and 6)
 */
#define RETRY_NUM	0xF000 	/* 1111 xxxx xxxx xxxx Maximum retries */
#define SLOT_TIME_HI	0x0200	/* xxxx x010 xxxx xxxx */
#define SLOT_TIME_LO	0x0000	/* xxxx xxxx 0000 0000 */

#define CONF_3_WORD	(RETRY_NUM + SLOT_TIME_HI + SLOT_TIME_LO)

/*
 * cfg_word 4 of configuration parameters (bytes 9 and 8)
 */
#define CDT_SRC		0x8000	/* 1xxx xxxx xxxx xxxx */
#define CDTF		0x7000	/* x111 xxxx xxxx xxxx */
#define CRS_SRC		0x0800	/* xxxx 1xxx xxxx xxxx */
#define CRSF		0x0700	/* xxxx x111 xxxx xxxx */
#define PAD		0x0080	/* xxxx xxxx 1xxx xxxx */
#define BT_STF		0x0040	/* xxxx xxxx x1xx xxxx Bit stuffing */
#define CRC_16		0x0020	/* xxxx xxxx xx1x xxxx 16 bit CRC type */
#define NCRC_INS	0x0010	/* xxxx xxxx xxx1 xxxx No CRC insertion */
#define TONO_CRS	0x0008	/* xxxx xxxx xxxx 1xxx Xmit with no CRS */
#define MANCH_NRZ	0x0004	/* xxxx xxxx xxxx x1xx */
#define BC_DIS		0x0002	/* xxxx xxxx xxxx xx1x Braodcast Disable */
#define PRM		0x0001	/* xxxx xxxx xxxx xxx1 Promiscuous mode */

#define CONF_4_WORD	(NO_BITS_SET)

/*
 * cfg_word 5 of configuration parameters (bytes 11 and 10)
 *
 * Note that the monitor mode is duplicated here from byte 1...
 */
#define MC_ALL		0x2000	/* xx1x xxxx Recv all MC frms */
#define CDBSAC		0x1000	/* xxx1 xxxx Collision Detect */
#define AUTOTX		0x0800	/* xxxx 1xxx Auto Tx */
#define CRCINM		0x0400	/* xxxx x1xx Rx CRC */
#define PRECRS		0x0100	/* xxxx xxx1 Preamble */
#define LNGFLD		0x0200	/* xxxx xx1x Pad Length */
#define FRM_LEN_DIAG	0x002E	/* xxxx xxxx 0010 1110 equals 46 */
#define MIN_FRM_LEN	0x0040	/* xxxx xxxx 0100 0000 equals 64 */

#define CONF_5_WORD	(MONITOR_MODE + PRECRS + LNGFLD + CRCINM \
			+ AUTOTX + CDBSAC)

/*
 * cfg_word 6 of configuration parameters (bytes 13 and 12)
 */
#define FORCED_2_ONE	0x3F00	/* xx11 1111 xxxx xxxx Forced to 1's */
#define DIS_BOF		0x8000	/* 1xxx xxxx xxxx xxxx Disable boff */
#define MULT_IA		0x4000	/* x1xx xxxx xxxx xxxx Mult Ind Addr */
#define	FDX		0x0040	/* xxxx xxxx 0100 0000 Full Duplex op */

#define CONF_6_WORD	(FORCED_2_ONE)

/*
 * Transmit Buffer Descriptor 
 */
#define TBD_EOF		0x8000	/* EOF bit in tbd_t */
#define TBD_COUNT	0x3FFF	/* tcb count & tbd count masks */

/*
 * Received Buffer Descriptor Structure
 */
#define RBD_EOF		0x8000	/* EOF bit in rbd/rfd actual cnt */
#define RBD_EL		0x8000	/* EL bit in rbd size field */
#define RBD_STAT_F	0x4000	/* F (Full) bit in rbd/rfd actual cnt field */
#define RBD_STAT_P	0x4000	/* P (Prefetch) bit in rbd/rfd size field */
#define RBD_COUNT	0x3FFF	/* rbd actual cnt & rbd count masks */

/*
 * Receive Frame Descriptor Structure
 */
#define RFD_DONE	0x8000	/* C - Frame Completion bit */
#define RFD_BUSY	0x4000	/* B - Buffer has been prefetched and is busy */
#define RFD_OK		0x2000	/* OK - Used ONLY with save bad frame conf */
#define RFD_LENERR	0x1000	/* S12 Length Error if config'd for len check */
#define RFD_CRC		0x0800	/* S11 Received frame with crc error */
#define RFD_ALN		0x0400	/* S10 Received misaligned frame w/crc error */
#define RFD_RSC		0x0200	/* S9 No Buffer space (no resources) */
#define RFD_DMA		0x0100	/* S8 DMA Overrun, failure to obtain bus */
#define RFD_SHORT	0x0080	/* S7 Short Frame */
#define RFD_EOP		0x0040	/* S6 No EOP flag (bit stuffing only) */
#define RFD_BROADCAST	0x0002	/* S1 IA did not Match configured address */
#define RFD_COLL	0x0001	/* S0 Collision detected during reception */

/*
 * Time Domain Reflectomet
 */
#define TDR_LNK_OK	0x8000	/* No Link Problem Identified */
#define TDR_XVR_PRB	0x4000	/* Transciever Problem */
#define TDR_ET_OPN	0x2000	/* Transmission line not terminated */
#define TDR_ET_SRT	0x1000	/* Short Detected on line */
#define TDR_TIME_MASK	0x07FF	/* Mask to obtain time value */

/* 
 * Misc definitions
 */
#define ADDR_LEN	6	/* Physical Layer address length (ethernet=6) */
#define NIL_OFFSET	0xFFFF		/* null offset link in scb */
#define NIL_ADDR	0xFFFFFFFF	/* null address value(when AC_CW_SF=0)*/
#define ATTENTION_FLAG	0		/* Used to write CA address */
#define ALIGN_2K(x)	(((long)(x) + 2047) & ~2047)

/*
 * Data Structure Definitions
 */
#define CFGWORDS		7	/* See 82596CA spec, linear mode  */
typedef struct configure_t {
	u_short cf_words[CFGWORDS];	/* See CONF_*_WORDS Above */
	u_short	cf_pad;			/* Pad to 32 bit boundary */
} configure_t;

typedef struct tdr_t {			/* LoopBack Time Domain Reflectonomy */
	u_short	tdr_time;		/* Time, ET SRT, ET OPN ... */
	u_short tdr_zero;		/* Must always be zero */
} tdr_t;

typedef struct tbd_t {
	u_short		tbd_size;	/* Actual Count */
	u_short		tbd_zero;	/* Must always be zero */
	struct tbd_t	*tbd_next;	/* Pointer to the next tbd_t */
	u_char		*tbd_buf;	/* Transmit Data Buffer address */
} tbd_t;

typedef struct transmit_t {		/* tc_tbda=-1 when AC_CW_SF=0 */
	tbd_t	*tc_tbda;		/* Transmit Buffer Description Addr */
	u_int	tc_count;		/* Transmit Count */
	u_char	tc_dest[ADDR_LEN];	/* Destination Address */
	u_short tc_length;		/* Transmit Data Length */
} transmit_t;

typedef struct rbd_t {
	u_short		rbd_act_count;	/* Buffer data count */
	u_short		rbd_zero;	/* Always zero (next rbd offset) */
	struct rbd_t	*rbd_next;	/* Next rbd_t in chain */
	u_char		*rbd_buf;	/* Receive Buffer Data address */
	u_short		rbd_size;	/* Receive Buffer size */
	u_short		rbd_pad;	/* Pad to 32 bit boundary */
} rbd_t;

typedef struct rfd_t {			/* rfd_rbda=-1 when AC_CW_SF=0 */
	u_short	rfd_stat;		/* Status word field */
	u_short	rfd_cmd;		/* Command word field */
	struct rfd_t *rfd_next;		/* Pointer to next rfd */
	rbd_t	*rfd_rbda;		/* Receive Buffer Descriptor Address */
	u_short	rfd_actual_cnt;		/* Set to 0, Recv'd Data Actual Count */
	u_short	rfd_size;		/* Buffer Size in Bytes mod 2 */
	u_char	rfd_dest_addr[ADDR_LEN];
	u_char 	rfd_src_addr [ADDR_LEN];
	u_short rfd_length;		/* 802.3 Data Field Length */
} rfd_t;

typedef struct ac_t {
	u_short	ac_stat;		/* Status Word */
	u_short	ac_cmd;			/* Command Word */
	struct ac_t	*ac_next;	/* Link address */
	union {
		u_char		iasetup[ADDR_LEN];
		configure_t	cfg;
		transmit_t	xmit;
		tdr_t		loop;	/* Live wire loopback, beware! */
		u_char		*dump_buffer;
	} ac_func;
} ac_t;

typedef struct scb_t {
	u_short	scb_status;		/* Status Word field */
	u_short	scb_command;		/* Command Unit Control field */
	ac_t	*scb_cba;		/* Command Block Address */
	rfd_t	*scb_rfa;		/* Recieve Frame Area address */
	u_int	scb_crcerrs;		/* CRC errors */
	u_int	scb_alnerrs;		/* Alignment errors */
	u_int	scb_rscerrs;		/* Resource errors */
	u_int	scb_ovrnerrs;		/* Over run errors */
	u_int	scb_rcvcdt;		/* Reciever collision errors */
	u_int	scb_shrtfrm;		/* Short Frame errors */
	u_short scb_offtimer;		/* Min time off bus (1x system clocks)*/
	u_short scb_ontimer;		/* Max time on bus (1x system clocks) */
} scb_t;

/*
 * In monitor modes the counters are used differently.
 * for example in monitor modes 2 or 3 overrun errors is 
 * the total good counter (scb_23_good), good+bad total (scb_23_goodbad)...
 *
 * Mode	    0		    1		    2		    3
 * 	(normal recv)	(mon+recv)	(monitor only)	(monitor only)
 *	scb_crcerrs	scb_crcerrs	scb_crcerrs	scb_crcerrs
 *	scb_alnerrs	scb_alnerrs	scb_alnerrs	scb_alnerrs
 *	scb_rscerrs	scb_rscerrs	scb_23_goodbad	scb_23_goodbad
 *	scb_ovrnerrs	scb_ovrnerrs	scb_23_good	scb_23_good
 *	scb_rcvcdt	scb_1_good	scb_rcvcdt	scb_rcvcdt
 *	scb_shrtfrm	scb_shrtfrm	scb_shrtfrm	scb_shrtfrm
 */
#define scb_23_goodbad	scb_rscerrs
#define scb_23_good	scb_ovrnerrs
#define scb_1_good	scb_rcvcdt
#define bad_frames(scb) ((scb)->scb_shrtfrm \
			+ (scb)->scb_crcerrs \
			+ (scb)->scb_alnerrs)  /* Total bad frames monitored. */
			
typedef struct iscp_t {
	u_short	iscp_busy;		/* When set, indicates 82586 is busy */
	u_short	iscp_offset;		/* Not used in linear mode */
	scb_t	*iscp_scb;		/* Scb base address */
} iscp_t;

typedef struct scp_t {
	u_short	scp_zero;		/* Always zero */
	u_char	scp_sysbus;		/* See Data Sheet for modes (0x44) */
	u_char	scp_zero1;		/* Always zero */
	u_int	scp_zero2;		/* Always zero */
	iscp_t	*scp_iscp;		/* Intermediate System Config Pointer */
} scp_t;

#endif	_I82596_H_
