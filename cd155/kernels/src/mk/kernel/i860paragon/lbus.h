/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1992 by Intel Corporation,
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
 * $Id: lbus.h,v 0.10 1995/04/04 20:54:28 lenb Exp $
 */

/*
 *	Local Bus Address Space
 *
 *	Local bus space accesses the node control and status registers,
 *	the NIST MultiKron chip, the diagnostic SCAN ports, and flash ROM.
 *
 *	Local bus space is accessed when the most significant four bits of
 *	the address is equal to 6.
 *
 *	Address bits <22:20> specify the resources within the local
 *	bus space that are accessed.
 *
 *	This space does not recognize burst transfers, therefore all accesses
 *	in local bus space must be non-cached or I/O.
 *
 */

#ifndef	_LBUS_H_
#define	_LBUS_H_

/*
 *	Read the node status register
 */
#define	node_status_register_read() \
		(unsigned long) inl((unsigned long *)LB_NODE_STATUS)

/*
 *	Node status register
 *
 *	(read-only; bits <7:0> and <31:24> are unused.)
 */

#define	LB_NODE_STATUS		(0x60000000)
#define LB_GP_NODE_STAT_MEMSIZE		(1 << 11)	/* mem size 0=16MB, 1=32MB */
#define LB_NODE_STAT_ISMPNODE		(1 << 16)	/* High for MP node */
#define LB_NODE_STAT_RDSCANRDY		(1 << 17)	/* read scan ready */
#define LB_NODE_STAT_EXPPRESENT		(1 << 18)	/* expansion present */
#define LB_MP_NODE_STAT_CPU2_OWNS	(1 << 19)	/* cpu2 owns the bus */
#define LB_NODE_STAT_CPU0_OWNS		(1 << 20)	/* cpu0 owns the bus */
#define LB_NODE_STAT_CPU1_OWNS		(1 << 21)	/* cpu1 owns the bus */
#define LB_NODE_STAT_EXP_OWNS		(1 << 22)	/* expansion owns (?) */
#define LB_MP_NODE_STAT_CPRSNT		(1 << 29)	/* Serial conn when low */

#define	NODE_STATUS_NODEREV(reg)	(((reg) >> 8) & 0x7)
#define	NODE_STATUS_SLOTID(reg)		(((reg) >> 12) & 0xF)
#define	MP_NODE_STATUS_FRUID(reg)	(((reg) >> 24) & 0x1F)

#define	GP_NODE_STATUS_BYTES(reg)	((16*1024*1024) << (((reg) >> 11) & 1))

#define	MP_FRUID_MB(id)			(16 << ((id) & 3))
#define	MP_FRUID_BYTES(id)		((16*1024*1024) << ((id) & 3))
#define	MP_FRUID_FAB(id)		((((id) >> 2) & 3) + 1)

/*
 *	Node control register
 *
 *	(write-only; cleared to all zeros on reset)
 *
 * MP3 Board:	There are 3 green led's and 1 red led's. They are 
 *		positioned on board like this:
 *
 *			G R          A B
 *			G G          C D
 *		
 */
#define LB_NODE_CONTROL		(0x60000004)

#define LB_NODE_CTRL_UNDER_LED	(1 <<  0) /* Under LED (bar graph) */
#define LB_NODE_CTRL_MIDDLE_LED	(1 <<  1) /* Node-Green -Middle LED  */
#define LB_NODE_CTRL_OVER_LED	(1 <<  2) /* Over LED (bar graph) */
#define LB_NODE_CTRL_TOP_LED	(1 <<  3) /* Top LED (bar graph */
#define LB_NODE_CTRL_RED_LED	(1 <<  4) /* Node-Red */
#define LB_NODE_CTRL_CPU1_RESET	(1 <<  5) /* cpu1 reset! */
#define LB_NODE_CTRL_CPU0_IDLE	(1 <<  6) /* cpu0 idle! (panel bit 1) */
#define LB_NODE_CTRL_CPU1_IDLE	(1 <<  7) /* cpu1 idle! (panel bit 0) */
#define LB_NODE_CTRL_BOOT	(1 <<  8) /* boot bit */
#define LB_NODE_CTRL_DRAMENABLE	(1 <<  9) /* enable access to DRAM */
#define LB_NODE_CTRL_CPU2_RESET (1 << 10) /* cpu2 reset! (MP3)*/
#define LB_NODE_CTRL_REF_DEF	(1 << 11) /* memory refresh defeat */
#define LB_NODE_CTRL_KEN	(1 << 12) /* cache-enable! */
#define LB_NODE_CTRL_PARK	(1 << 13) /* Don't arbitrate the bus */
#define LB_NODE_CTRL_INTOUT	(1 << 14) /* interrupt from node */
#define LB_NODE_CTRL_ECCSEL	(1 << 15) /* ECC on memory access */
#define LB_NODE_CTRL_EXPSCANIN	(1 << 16) /* expansion SCAN input */
#define LB_NODE_CTRL_EXPSCANMD	(1 << 17) /* expansion SCAN mode */
#define LB_NODE_CTRL_EXPSCANCK	(1 << 18) /* expansion SCAN clock */
#define LB_NODE_CTRL_BOTTOM_LED	(1 << 19) /* Bottom LED (bar graph) */
#define LB_NODE_CTRL_NISTRESET	(1 << 20) /* controlled NIST reset */
#define LB_NODE_CTRL_CPUTURBO	(1 << 21) /* enable 0-cycle wr latency */
#define LB_NODE_CTRL_EXPTURBO	(1 << 22) /* enable 0-cycle wr latency */
#define LB_NODE_CTRL_DIFFOUT	(1 << 23) /* bit-serial output (rs432) */

/*
 *	NIST MultiKron
 * This was moved to rpm.h by andyp
 */

/*
 *	SCAN ports (byte wide, bits <31:24>)
 */
#define LB_DIAG_IN		(0x60300003)	/* diagnostic SCAN input */
#define LB_DIAG_OUT		(0x60300007)	/* diagnostic SCAN output */

/*
 *	Flash ROM (128K)
 */
#define LB_FLASH_ROM_BASE	(0x60700000)	/* on-board flash rom */

#ifndef	ASSEMBLER

#if	NOT_YET
extern unsigned long inl(unsigned long *address);
#endif	NOT_YET

#include <kern/lock.h>

/*
 * ncr_lock -- Node Control Register Lock
 *
 * acquired at SPLOFF, held at SPLOFF, released at SPLOFF.
 * (NCR_ENTER()/NCR_EXIT() enforce this.)
 *
 * Note that breakpoints are prohibited where ncr_lock is held --
 * as kdb will try to acquire the ncr_lock to write the LEDs.
 */

decl_simple_lock_data(extern, ncr_lock);

#define	NCR_ENTER(s) { \
		s = sploff(); \
		SIMPLE_LOCK(&ncr_lock); \
	}
	
#define	NCR_EXIT(s) { \
		SIMPLE_UNLOCK(&ncr_lock); \
		splon(s); \
	}

/*
 * SCAN/FSCAN macros used by SCAN/FSCAN drivers.
 */
#define INT_TO_NODE_ACTIVE()	\
	((inl((unsigned long *)DP_STATUS_LO) & DP_ISTAT_NET) == DP_ISTAT_NET)

/*
 * Fast Path -- Called within ncr_enter()/ncr_exit()
 */
#define IntFromNodeHigh_locked() \
	ncr_set_locked(LB_NODE_CTRL_INTOUT) /* Go high */
#define IntFromNodeLow_locked() \
	ncr_clear_locked(LB_NODE_CTRL_INTOUT) /* Go low */

extern void ncr_set_locked(unsigned long bits);
extern void ncr_clear_locked(unsigned long bits);

/*
 * "Normal Path" -- does the ncr_enter()/ncr_exit() for you.
 */
#define IntFromNodeHigh() \
	node_control_register_set(LB_NODE_CTRL_INTOUT) /* Go high */
#define IntFromNodeLow() \
	node_control_register_clear(LB_NODE_CTRL_INTOUT) /* Go low */

extern void node_control_register_set(unsigned long bits);
extern void node_control_register_clear(unsigned long bits);

#endif	/* !ASSEMBLER */

#endif	/* _LBUS_H_ */
