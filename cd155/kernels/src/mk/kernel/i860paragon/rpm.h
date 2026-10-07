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
 * $Id: rpm.h,v 0.10 1994/11/18 20:43:13 mtm Exp $
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
 *	RPM resource utilization counters.
 *
 *	The RPM contains 9 counters.  Eight are used for monitoring
 *	bus ownership and MRC directional information.  The other
 *	counter is driven by the global 10MHz backplane clock.
 *	They are resetable, and can be disabled.
 *
 *	The MP3 board contains 3 additional counters. These counters
 *	can be read on a GP node without fear of hanging and the returned
 *	values will probably be 1's due to Xilix internal pullups.
 *
 */

/*
 *	RPM counter registers
 */

#ifndef __RPM_H__
#define __RPM_H__

#define	RPM_BASE_PADDR		(0x60100000)	/* base physical address */
#define	RPM_BASE_VADDR		(0xffff2000)	/* base virtual address */

/* read offsets */
#define	RPM_GLOBAL_TIME		(0x0040)	/* 56-bit global clock */
#define	RPM_BUSUTIL_CPU0	(0x0080)	/* cpu0 bus ownership */
#define	RPM_BUSUTIL_CPU1	(0x0100)	/* cpu1 bus ownership */
#define	RPM_BUSUTIL_LTU		(0x0200)	/* ltu bus ownership */
#define	RPM_BUSUTIL_EXP		(0x0400)	/* expansion bus ownership */
#define	RPM_BUSUTIL_CPU2	(0x0440)	/* cpu2 bus ownership (MP3)*/
#define	RPM_DATABUS_BUSY	(0x0480)	/* data bus busy counter (MP3)*/
#define	RPM_DRAMCNTRL_BUSY	(0x0500)	/* dram controller busy (MP3) */
#define RPM_MRC_NORTH		(0x0840)	/* MRC north counter */
#define RPM_MRC_SOUTH		(0x0880)	/* MRC south counter */
#define RPM_MRC_EAST		(0x0900)	/* MRC east counter */
#define RPM_MRC_WEST		(0x0a00)	/* MRC west counter */
#define	RPM_PROGRAM_STATUS	(0x1000)	/* programming complete */

/* write offsets */
#define	RPM_CONTROL_REG		(0x0000)	/* control register */
#define RPM_PROGRAM_START	(0x1000)	/* begin programming */
#define RPM_PROGRAM_DATA	(0x0000)	/* when programming... */


/*
 *	Control register bits
 */
/* counter enables */
#define _RPM_ENAB(n,o)	(((1) << (n)) << (o))
#define RPM_MRC_NORTH_ENABLE	_RPM_ENAB(0,16)	/* enable MRC north counter */
#define RPM_MRC_SOUTH_ENABLE	_RPM_ENAB(1,16)	/* enable MRC south counter */
#define RPM_MRC_EAST_ENABLE	_RPM_ENAB(2,16)	/* enable MRC east counter */
#define RPM_MRC_WEST_ENABLE	_RPM_ENAB(3,16)	/* enable MRC west counter */
#define RPM_CPU0_ENABLE		_RPM_ENAB(4,16)	/* enable cpu0 counter */
#define RPM_CPU1_ENABLE		_RPM_ENAB(5,16)	/* enable cpu1 counter */
#define RPM_LTU_ENABLE		_RPM_ENAB(6,16)	/* enable ltu counter */
#define RPM_EXP_ENABLE		_RPM_ENAB(7,16)	/* enable expansion counter */

/*
 * These are new counters for the MP3 board
 */
#define RPM_CPU2_ENABLE		_RPM_ENAB(0,0)	/* enable cpu2 counter */
#define RPM_BUSBUSY_ENABLE	_RPM_ENAB(1,0)	/* enable bus busy counter */
#define RPM_DRAMBUSY_ENABLE	_RPM_ENAB(2,0)	/* enable dram cntrl counter */

/* counter resets */
#define _RPM_RESET(n,o)	_RPM_ENAB(n,o)
#define RPM_MRC_NORTH_RESET	_RPM_RESET(0,24)/* reset MRC north counter */
#define RPM_MRC_SOUTH_RESET	_RPM_RESET(1,24)/* reset MRC south counter */
#define RPM_MRC_EAST_RESET	_RPM_RESET(2,24)/* reset MRC east counter */
#define RPM_MRC_WEST_RESET	_RPM_RESET(3,24)/* reset MRC west counter */
#define RPM_CPU0_RESET		_RPM_RESET(4,24)/* reset cpu0 counter */
#define RPM_CPU1_RESET		_RPM_RESET(5,24)/* reset cpu1 counter */
#define RPM_LTU_RESET		_RPM_RESET(6,24)/* reset ltu counter */
#define RPM_EXP_RESET		_RPM_RESET(7,24)/* reset expansion counter */

/*
 * These are new for the MP3 board
 */
#define RPM_CPU2_RESET		_RPM_ENAB(0,4)	/* reset cpu2 counter */
#define RPM_BUSBUSY_RESET	_RPM_ENAB(1,4)	/* reset bus busy counter */
#define RPM_DRAMBUSY_RESET	_RPM_ENAB(2,4)	/* reset dram cntrl counter */

/* structure elements */
typedef unsigned long 		rpm_pad_t;	/* unused */
typedef volatile double		rpm_timer_t;	/* 56bit counter */
typedef volatile unsigned long	rpm_counter_t;	/* 32bit counter */
typedef volatile unsigned long	rpm_ctrl_t;	/* 32bit control register */

struct rpm {
        rpm_ctrl_t      rpm_control;    rpm_pad_t rpm_pad0[15];
        rpm_timer_t     rpm_time;       rpm_pad_t rpm_pad1[14];
        rpm_counter_t   rpm_cpu0;       rpm_pad_t rpm_pad2[31];
        rpm_counter_t   rpm_cpu1;       rpm_pad_t rpm_pad3[63];
        rpm_counter_t   rpm_ltu;        rpm_pad_t rpm_pad4[127];
        rpm_counter_t   rpm_exp;        rpm_pad_t rpm_pad5[15];
        rpm_counter_t   rpm_cpu2;       rpm_pad_t rpm_pad6[15];
        rpm_counter_t   rpm_dbus;       rpm_pad_t rpm_pad7[31];
        rpm_counter_t   rpm_dram;       rpm_pad_t rpm_pad8[207];
        rpm_counter_t   rpm_north;      rpm_pad_t rpm_pad9[15];
        rpm_counter_t   rpm_south;      rpm_pad_t rpm_pad10[31];
        rpm_counter_t   rpm_east;       rpm_pad_t rpm_pad11[63];
        rpm_counter_t   rpm_west;       rpm_pad_t rpm_pad12[127];
};

/*
 *	Software-simulated counters.  One per cpu.
 *
 *	rpms_alltraps and rpms_it will be under-counted because
 *	the fast-traps code bypasses the statistics.
 *
 */
#define	RPMSOFT_BASE_VADDR	(0xffff5000)	/* base virtual address */
#define	RPMSOFT_BASE_VADDR_KERN	(0xffff6000)	/* writable addr for kernel */
struct rpmsoft {
	/* hi-res cpu idle time */
	rpm_timer_t	rpms_idle;	/* acc. cpu idle time */

	/* large-grain trap handler statistics */
	rpm_counter_t	rpms_alltraps;	/* all traps */
	rpm_counter_t	rpms_it;	/* trap instructions */
	rpm_counter_t	rpms_in;	/* interrupts */
	rpm_counter_t	rpms_iat;	/* instruction access traps */
	rpm_counter_t	rpms_dat;	/* data access traps */
	rpm_counter_t	rpms_ft;	/* fp traps */

	/* data-access trap statistics */
	rpm_counter_t	rpms_datld;	/* dat on ld.x */
	rpm_counter_t	rpms_datst;	/* dat on st.x */
	rpm_counter_t	rpms_datfldfst;	/* dat on fld.x or fst.x */
	rpm_counter_t	rpms_datpst;	/* dat on pst */
	rpm_counter_t	rpms_datpfld;	/* dat on pfld.y */
	rpm_counter_t	rpms_datauto;	/* dat on fld.x++, fst.x++, pfld.x++ */

	/* page_faults -- data-access traps from mmu */
	rpm_counter_t	rpms_notdirty;	/* store to a clean page */
	rpm_counter_t	rpms_notref;	/* access to unref page while locked */
	rpm_counter_t	rpms_notwr;	/* store to a read-only page */
	rpm_counter_t	rpms_pdenotu;	/* pde priv violation */
	rpm_counter_t	rpms_ptenotu;	/* pte priv violation */
	rpm_counter_t	rpms_pdenotp;	/* pde not valid */
	rpm_counter_t	rpms_ptenotp;	/* pte not valid */

	/* locked sequence related traps */
	rpm_counter_t	rpms_lockseq;	/* traps while in locked sequence */
	rpm_counter_t	rpms_lockres;	/* restarted locked sequences */
	rpm_counter_t	rpms_lockexp;	/* expired locked sequences */

	/* floating-point exception counts */
	rpm_counter_t	rpms_fpe_si;	/* sticky inexact */
	rpm_counter_t	rpms_fpe_se;	/* source exception */
	rpm_counter_t	rpms_fpe_mu;	/* multiplier underflow */
	rpm_counter_t	rpms_fpe_mo;	/* multiplier overflow */
	rpm_counter_t	rpms_fpe_mi;	/* multiplier inexact */
	rpm_counter_t	rpms_fpe_ma;	/* multiplier add-one */
	rpm_counter_t	rpms_fpe_au;	/* adder underflow */
	rpm_counter_t	rpms_fpe_ao;	/* adder overflow */
	rpm_counter_t	rpms_fpe_ai;	/* adder inexact */
	rpm_counter_t	rpms_fpe_aa;	/* adder add-one */
};

#ifdef	KERNEL
#define	RPMSOFT 1

#if	RPMSOFT

extern void rpmsoft_idle_tick(int);

#define	RPMSOFT_STAT(cpu, counter) \
	((struct rpmsoft *)RPMSOFT_BASE_VADDR_KERN + (cpu))->counter

#define	RPMSOFT_STAT_INC(cpu, counter) \
	RPMSOFT_STAT(cpu, counter)++

#else	/* RPMSOFT */

#define rpmsoft_idle_tick(int)
#define	RPMSOFT_INC(cpu, counter)

#endif	/* RPMSOFT */

#endif	/* KERNEL */

#endif /* __RPM_H__ */
