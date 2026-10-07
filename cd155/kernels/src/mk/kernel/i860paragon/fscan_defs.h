/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1995 by Intel Corporation,
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
 *
 * $Id: fscan_defs.h,v 1.3 1995/04/04 20:53:40 lenb Exp $
 *
 */

#ifndef __FSCAN_DEFS_H__
#define __FSCAN_DEFS_H__

/*
 * Fscan driver internal definitions and declarations
 * included only by fscan.h
 */
 
#include <sys/types.h>

/*
 *	Define the buffer with the high and low water marks. This buffer
 *	is used to hold the data to be transmitted to the console. The
 *	high water mark should be low enough to fit an entire tty buffer.
 */
#define FSCAN_BUFFER_SIZE	4096
#define FSCAN_HIGH_WATER	4090
#define FSCAN_MAX_WIDTH		200	/* Maximum pulse width */
#define FSCAN_MAX_SPIN		8000	/* Maximum time to spin */
#define FSCAN_MAX_BLOCK		127	/* Maximum size of block */

/* define some byte position constants */
#define CHECKSUM_BYTE		0	/* the first byte in the byte stream */
#define COM_BUF_LEN_BYTE	1	/* the second byte in the stream */
#define STATE1_BYTE		2	/* the third byte in the stream */
#define STATE2_BYTE		3	/* the fourth byte in the stream */
#define NUM_STATE_BYTES		4	/* number of bytes for state stream */



/*
 * Define the states which this node can be in. This list MUST match the
 * list in the 'fscan' programming running on the diagnostic station.
 */
#define FSCAN_STATE_UNKNOWN     0x00
#define FSCAN_STATE_RUNNING     0x01
#define FSCAN_STATE_DEBUGGER    0x02
#define FSCAN_STATE_SUSPICIOUS  0x03
#define FSCAN_STATE_COMMATOSE   0x04
#define FSCAN_STATE_DEAD        0x05
#define FSCAN_STATE_REBOOT	0x06
#define FSCAN_STATE_MULTIUSER_BIT	0x40

/*
 *	Declare constants used to control the speed, size and duration
 *	of block transfers.
 */
#define CHARBITS		    7
#define PULSE_WIDTH		   90
#define BLOCK_SIZE		   82
#define GOAHEAD_TIMEOUT		10000
#define RESPONSE_TIMEOUT	80000

/*
 * These are the commands sent by the host. Make sure we reduce the
 * number of these commands because they take away from actual characters
 * which can be sent to the node.  NOTE: the FSCAN_LOW_CMD cannot go beyond
 * 28 because 27 is the ESC key used in vi editing.
 */
#define FSCAN_HIGH_CMD		31
#define FSCAN_SEND_STATE        31
#define FSCAN_SEND_NEXT         30
#define FSCAN_COMMAND		29	/* General purpose command */
#define FSCAN_LOW_CMD           29	

/* 
 * These commands are sent following the FSCAN_COMMAND command. This is
 * done to provide up to 127 additional commands.
 */
#define FSCAN_SHOW_COUNTS       01
#define FSCAN_SEND_VERSION	02
#define FSCAN_PULSE		03
#define FSCAN_ADJUST		04

/*
 * This is the main status and control data structure.
 */
struct
fscan_softc {
	decl_simple_lock_data(,com_lock)
        boolean_t       com_probed;     /* has probed once		*/
        boolean_t       com_primary;	/* Is fscan the primary cons	*/
        boolean_t       com_active;	/* Is fscan actively used?	*/
        boolean_t       com_polling;    /* in console polling mode	*/
        boolean_t       com_softCAR;    /* software carrier		*/
	boolean_t	com_multiuser;	/* Are we in multiuser mode?	*/
	boolean_t	com_reboot;	/* Do we need to reboot?	*/
        u_long          com_cnt_rx;     /* # of characters received	*/
        u_long          com_cnt_tx;     /* # of characters transmitted	*/
        u_long          com_err_ic;     /* Invalid Command error count	*/
        u_long          com_err_noresp; /* No response to xmit		*/
        u_long          com_err_sbs;    /* Start bit never started	*/
        u_long          com_err_sbe;    /* Start bit never ended	*/
        u_long          com_err_hin;    /* Host didn;t go inactive.	*/
	u_short		com_buf_len;	/* # of bytes waiting in buffer */
	u_short		com_buf_head;	/* Head of buffer		*/
	u_short		com_buf_tail;	/* Tail of buffer		*/
	u_short		com_overhead;	/* Overhead to access regs.	*/
	u_char		com_state;	/* State of the node		*/
	u_char		com_last_cmd;	/* Last command received	*/
	u_char		com_skew;	/* Slew rate			*/
	u_char		com_cmd_in_prog;/* Command is in progress	*/
	int		com_pulse_width;/* Pulse frequency		*/

	/*
	 * These function pointers are used to chain the real console 
	 * into the fscan stuff.
	 */
	int		(*com_pollc)();	/* The real 'pollc' routine	*/
	u_char		com_buf[FSCAN_BUFFER_SIZE+1];/* Xmit Buffer	*/
};

typedef struct fscan_softc *fscan_softc_t;

#define	FSCAN_COM_ENTER(s) { \
		s = sploff(); \
		SIMPLE_LOCK(&fscan->com_lock); \
	}

#define	FSCAN_COM_EXIT(s) { \
		SIMPLE_UNLOCK(&fscan->com_lock); \
		splon(s); \
	}

#endif	/*  __FSCAN_DEFS_H__ */
