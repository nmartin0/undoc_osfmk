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
 * $Id: spl.c,v 0.29 1995/03/14 23:42:58 jerrie Exp $
 */

#include <kern/assert.h>
#include <kern/cpu_number.h>
#include <kern/lock.h>
#include <i860/psl.h>
#include <i860paragon/dp.h>
#include <i860paragon/spl.h>

extern unsigned long dpset[];
extern unsigned long dpclr[];
extern int	paragon_mp3;

decl_simple_lock_data(,spl_lock)

#define	STATIC

static int	set_spl();
static int	set_ipl_noi();
static int	interrupts_on=0;

STATIC int	spl_current_ipl[NCPUS];
STATIC int	spl_soft_clock_needed[NCPUS];
STATIC unsigned long	spl_disable[NSPL] = {
	/* interrupts to be masked at ipl 0 */
	(
	 ( SPLMSG	<= 0? (1 << SPL_IMASK_NIC)	: 0) |
	 ( SPLBIO	<= 0? (1 << SPL_IMASK_EXP)	: 0) |
	 ( SPLSCAN	<= 0? (1 << SPL_IMASK_NET)	: 0) |
	 ( SPLCPU	<= 0? (1 << SPL_IMASK_CPU0)	: 0) |
	 ( SPLCLOCK	<= 0? (1 << SPL_IMASK_TIMER0)	: 0) |
	 ( SPLLTU	<= 0? (1 << SPL_IMASK_LTU0)	: 0) |
	 ( SPLHWERR	<= 0? (1 << SPL_IMASK_HWERR0)	: 0) |
	 ( SPLCPU	<= 0? (1 << SPL_IMASK_CPU1)	: 0) |
	 ( SPLCLOCK	<= 0? (1 << SPL_IMASK_TIMER1)	: 0) |
	 ( SPLLTU	<= 0? (1 << SPL_IMASK_LTU1)	: 0) |
	 ( SPLHWERR	<= 0? (1 << SPL_IMASK_HWERR1)	: 0) |
	 ( SPLCPU	<= 0? (1 << SPL_IMASK_CPU2)	: 0) |
	 ( SPLCLOCK	<= 0? (1 << SPL_IMASK_TIMER2)	: 0)
	),

	/* interrupts to be masked at ipl 1 */
	(
	 ( SPLMSG	<= 1? (1 << SPL_IMASK_NIC)	: 0) |
	 ( SPLBIO	<= 1? (1 << SPL_IMASK_EXP)	: 0) |
	 ( SPLSCAN	<= 1? (1 << SPL_IMASK_NET)	: 0) |
	 ( SPLCPU	<= 1? (1 << SPL_IMASK_CPU0)	: 0) |
	 ( SPLCLOCK	<= 1? (1 << SPL_IMASK_TIMER0)	: 0) |
	 ( SPLLTU	<= 1? (1 << SPL_IMASK_LTU0)	: 0) |
	 ( SPLHWERR	<= 1? (1 << SPL_IMASK_HWERR0)	: 0) |
	 ( SPLCPU	<= 1? (1 << SPL_IMASK_CPU1)	: 0) |
	 ( SPLCLOCK	<= 1? (1 << SPL_IMASK_TIMER1)	: 0) |
	 ( SPLLTU	<= 1? (1 << SPL_IMASK_LTU1)	: 0) |
	 ( SPLHWERR	<= 1? (1 << SPL_IMASK_HWERR1)	: 0) |
	 ( SPLCPU	<= 1? (1 << SPL_IMASK_CPU2)	: 0) |
	 ( SPLCLOCK	<= 1? (1 << SPL_IMASK_TIMER2)	: 0)
	),

	/* interrupts to be masked at ipl 2 */
	(
	 ( SPLMSG	<= 2? (1 << SPL_IMASK_NIC)	: 0) |
	 ( SPLBIO	<= 2? (1 << SPL_IMASK_EXP)	: 0) |
	 ( SPLSCAN	<= 2? (1 << SPL_IMASK_NET)	: 0) |
	 ( SPLCPU	<= 2? (1 << SPL_IMASK_CPU0)	: 0) |
	 ( SPLCLOCK	<= 2? (1 << SPL_IMASK_TIMER0)	: 0) |
	 ( SPLLTU	<= 2? (1 << SPL_IMASK_LTU0)	: 0) |
	 ( SPLHWERR	<= 2? (1 << SPL_IMASK_HWERR0)	: 0) |
	 ( SPLCPU	<= 2? (1 << SPL_IMASK_CPU1)	: 0) |
	 ( SPLCLOCK	<= 2? (1 << SPL_IMASK_TIMER1)	: 0) |
	 ( SPLLTU	<= 2? (1 << SPL_IMASK_LTU1)	: 0) |
	 ( SPLHWERR	<= 2? (1 << SPL_IMASK_HWERR1)	: 0) |
	 ( SPLCPU	<= 2? (1 << SPL_IMASK_CPU2)	: 0) |
	 ( SPLCLOCK	<= 2? (1 << SPL_IMASK_TIMER2)	: 0)
	),

	/* interrupts to be masked at ipl 3 */
	(
	 ( SPLMSG	<= 3? (1 << SPL_IMASK_NIC)	: 0) |
	 ( SPLBIO	<= 3? (1 << SPL_IMASK_EXP)	: 0) |
	 ( SPLSCAN	<= 3? (1 << SPL_IMASK_NET)	: 0) |
	 ( SPLCPU	<= 3? (1 << SPL_IMASK_CPU0)	: 0) |
	 ( SPLCLOCK	<= 3? (1 << SPL_IMASK_TIMER0)	: 0) |
	 ( SPLLTU	<= 3? (1 << SPL_IMASK_LTU0)	: 0) |
	 ( SPLHWERR	<= 3? (1 << SPL_IMASK_HWERR0)	: 0) |
	 ( SPLCPU	<= 3? (1 << SPL_IMASK_CPU1)	: 0) |
	 ( SPLCLOCK	<= 3? (1 << SPL_IMASK_TIMER1)	: 0) |
	 ( SPLLTU	<= 3? (1 << SPL_IMASK_LTU1)	: 0) |
	 ( SPLHWERR	<= 3? (1 << SPL_IMASK_HWERR1)	: 0) |
	 ( SPLCPU	<= 3? (1 << SPL_IMASK_CPU2)	: 0) |
	 ( SPLCLOCK	<= 3? (1 << SPL_IMASK_TIMER2)	: 0)
	),

	/* interrupts to be masked at ipl 4 */
	(
	 ( SPLMSG	<= 4? (1 << SPL_IMASK_NIC)	: 0) |
	 ( SPLBIO	<= 4? (1 << SPL_IMASK_EXP)	: 0) |
	 ( SPLSCAN	<= 4? (1 << SPL_IMASK_NET)	: 0) |
	 ( SPLCPU	<= 4? (1 << SPL_IMASK_CPU0)	: 0) |
	 ( SPLCLOCK	<= 4? (1 << SPL_IMASK_TIMER0)	: 0) |
	 ( SPLLTU	<= 4? (1 << SPL_IMASK_LTU0)	: 0) |
	 ( SPLHWERR	<= 4? (1 << SPL_IMASK_HWERR0)	: 0) |
	 ( SPLCPU	<= 4? (1 << SPL_IMASK_CPU1)	: 0) |
	 ( SPLCLOCK	<= 4? (1 << SPL_IMASK_TIMER1)	: 0) |
	 ( SPLLTU	<= 4? (1 << SPL_IMASK_LTU1)	: 0) |
	 ( SPLHWERR	<= 4? (1 << SPL_IMASK_HWERR1)	: 0) |
	 ( SPLCPU	<= 4? (1 << SPL_IMASK_CPU2)	: 0) |
	 ( SPLCLOCK	<= 4? (1 << SPL_IMASK_TIMER2)	: 0)
	),

	/* interrupts to be masked at ipl 5 */
	(
	 ( SPLMSG	<= 5? (1 << SPL_IMASK_NIC)	: 0) |
	 ( SPLBIO	<= 5? (1 << SPL_IMASK_EXP)	: 0) |
	 ( SPLSCAN	<= 5? (1 << SPL_IMASK_NET)	: 0) |
	 ( SPLCPU	<= 5? (1 << SPL_IMASK_CPU0)	: 0) |
	 ( SPLCLOCK	<= 5? (1 << SPL_IMASK_TIMER0)	: 0) |
	 ( SPLLTU	<= 5? (1 << SPL_IMASK_LTU0)	: 0) |
	 ( SPLHWERR	<= 5? (1 << SPL_IMASK_HWERR0)	: 0) |
	 ( SPLCPU	<= 5? (1 << SPL_IMASK_CPU1)	: 0) |
	 ( SPLCLOCK	<= 5? (1 << SPL_IMASK_TIMER1)	: 0) |
	 ( SPLLTU	<= 5? (1 << SPL_IMASK_LTU1)	: 0) |
	 ( SPLHWERR	<= 5? (1 << SPL_IMASK_HWERR1)	: 0) |
	 ( SPLCPU	<= 5? (1 << SPL_IMASK_CPU2)	: 0) |
	 ( SPLCLOCK	<= 5? (1 << SPL_IMASK_TIMER2)	: 0)
	),

	/* interrupts to be masked at ipl 6 */
	(
	 ( SPLMSG	<= 6? (1 << SPL_IMASK_NIC)	: 0) |
	 ( SPLBIO	<= 6? (1 << SPL_IMASK_EXP)	: 0) |
	 ( SPLSCAN	<= 6? (1 << SPL_IMASK_NET)	: 0) |
	 ( SPLCPU	<= 6? (1 << SPL_IMASK_CPU0)	: 0) |
	 ( SPLCLOCK	<= 6? (1 << SPL_IMASK_TIMER0)	: 0) |
	 ( SPLLTU	<= 6? (1 << SPL_IMASK_LTU0)	: 0) |
	 ( SPLHWERR	<= 6? (1 << SPL_IMASK_HWERR0)	: 0) |
	 ( SPLCPU	<= 6? (1 << SPL_IMASK_CPU1)	: 0) |
	 ( SPLCLOCK	<= 6? (1 << SPL_IMASK_TIMER1)	: 0) |
	 ( SPLLTU	<= 6? (1 << SPL_IMASK_LTU1)	: 0) |
	 ( SPLHWERR	<= 6? (1 << SPL_IMASK_HWERR1)	: 0) |
	 ( SPLCPU	<= 6? (1 << SPL_IMASK_CPU2)	: 0) |
	 ( SPLCLOCK	<= 6? (1 << SPL_IMASK_TIMER2)	: 0)
	),

	/* interrupts to be masked at ipl 7 */
	(
	 ( SPLMSG	<= 7? (1 << SPL_IMASK_NIC)	: 0) |
	 ( SPLBIO	<= 7? (1 << SPL_IMASK_EXP)	: 0) |
	 ( SPLSCAN	<= 7? (1 << SPL_IMASK_NET)	: 0) |
	 ( SPLCPU	<= 7? (1 << SPL_IMASK_CPU0)	: 0) |
	 ( SPLCLOCK	<= 7? (1 << SPL_IMASK_TIMER0)	: 0) |
	 ( SPLLTU	<= 7? (1 << SPL_IMASK_LTU0)	: 0) |
	 ( SPLHWERR	<= 7? (1 << SPL_IMASK_HWERR0)	: 0) |
	 ( SPLCPU	<= 7? (1 << SPL_IMASK_CPU1)	: 0) |
	 ( SPLCLOCK	<= 7? (1 << SPL_IMASK_TIMER1)	: 0) |
	 ( SPLLTU	<= 7? (1 << SPL_IMASK_LTU1)	: 0) |
	 ( SPLHWERR	<= 7? (1 << SPL_IMASK_HWERR1)	: 0) |
	 ( SPLCPU	<= 7? (1 << SPL_IMASK_CPU2)	: 0) |
	 ( SPLCLOCK	<= 7? (1 << SPL_IMASK_TIMER2)	: 0)
	),
};


#define DP_NOP 22	/* bit 22 is a no-op */

/*
 *	The matrix of {register,bit} tuples, indexed by
 *	interrupt source and cpu number.
 */

/*
 * CPU1 and CPU2 only sees DPhigh timer and doorbell interrupts.
 * CPU1 does NOT see hardware errors in the DP asic, CPU0 only!
 * CPU2 can only see doorbell and timer interrutps.
 */

#if NCPUS == 1
#define	SPL_ENT(a, b, c, d, e, f) { { (a), (b) } },
#elif NCPUS == 2
#define	SPL_ENT(a, b, c, d, e, f) { { (a), (b) }, { (c), (d) } },
#elif NCPUS == 3
#define	SPL_ENT(a, b, c, d, e, f) { { (a), (b) }, { (c), (d) }, { (e), (f) } },
#else
/* #error	ERROR: this file assumes NCPUS <= 3 */
FORCE COMPILE TIME ERROR
#endif

struct dp_imask_tab dp_imask_tab_early[N_INTRS][NCPUS] = {
	SPL_ENT( DP0, DP_NOP, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_NIC */
	SPL_ENT( DP0,      1, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_EXP */
	SPL_ENT( DP0, DP_NOP, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_NET */
	SPL_ENT( DP0, DP_NOP, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_CPU0 */
	SPL_ENT( DP0, DP_NOP, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_TIMER0 */
	SPL_ENT( DP1, DP_NOP, DP1, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_LTU0 */
	SPL_ENT( DP0, DP_NOP, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_HWERR0 */
	SPL_ENT( DP1, DP_NOP, DP1, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_CPU1 */
	SPL_ENT( DP1, DP_NOP, DP1, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_TIMER1 */
	SPL_ENT( DP1, DP_NOP, DP1, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_LTU1 */
	SPL_ENT( DP1, DP_NOP, DP1, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_HWERR1 */
	SPL_ENT( DP1, DP_NOP, DP1, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_CPU2 */
	SPL_ENT( DP1, DP_NOP, DP1, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_TIMER2 */
};


struct dp_imask_tab dp_imask_tab[N_INTRS][NCPUS] = {
	SPL_ENT( DP0,      0, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_NIC */
	SPL_ENT( DP0,      1, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_EXP */
	SPL_ENT( DP0,      2, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_NET */
	SPL_ENT( DP0,      4, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_CPU0 */
	SPL_ENT( DP0,      5, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_TIMER0 */
	SPL_ENT( DP1,      2, DP1, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_LTU0 */
	SPL_ENT( DP0,     16, DP0, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_HWERR0 */
	SPL_ENT( DP1, DP_NOP, DP1,     12, VDP, DP_NOP)	/* SPL_IMASK_CPU1 */
	SPL_ENT( DP1, DP_NOP, DP1,     13, VDP, DP_NOP)	/* SPL_IMASK_TIMER1 */
	SPL_ENT( DP1,      3, DP1, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_LTU1 */
	SPL_ENT( DP1,     16, DP1, DP_NOP, VDP, DP_NOP)	/* SPL_IMASK_HWERR1 */
	SPL_ENT( DP1, DP_NOP, DP1, DP_NOP, VDP,      4)	/* SPL_IMASK_CPU2 */
	SPL_ENT( DP1, DP_NOP, DP1, DP_NOP, VDP,      5)	/* SPL_IMASK_TIMER2 */
};

#undef SPL_ENT

struct dp_ipl dp_ipl[NSPL][NCPUS], dp_master[NCPUS];

/*
 * Fill out dp_ipl array
 */

init_dp_ipl(early)
	int	early;
{
	int		i, ipl, index, cpu;
	unsigned long	permit, forbid;
	int		mask;
	struct dp_imask_tab	*dp;
	unsigned long		set[NDPS], clr[NDPS];
	void	splx();

	for (ipl = 0; ipl < NSPL; ipl++) {
		forbid = spl_disable[ipl];
		permit = ~forbid & ((1 << N_INTRS) - 1);
		for (cpu = 0; cpu < NCPUS; cpu++) {
			for (i=0; i<3; i++) {
				set[i] = clr[i] = 0;
			}
			mask = 1;
			for (index = 0; index < N_INTRS; index++) {
				dp = early
					? &dp_imask_tab_early[index][cpu]
					: &dp_imask_tab[index][cpu]
				;
				if (permit & mask) {
					set[dp->dp_mask_off] |=
						(1 << dp->dp_mask_bit);
				}
				if (forbid & mask) {
					clr[dp->dp_mask_off] |=
						(1 << dp->dp_mask_bit);
				}
				mask <<= 1;
			}
			dp_ipl[ipl][cpu].dp0_set = set[0];
			dp_ipl[ipl][cpu].dp0_clr = clr[0];
			dp_ipl[ipl][cpu].dp1_set = set[1];
			dp_ipl[ipl][cpu].dp1_clr = clr[1];
			dp_ipl[ipl][cpu].vdp_set = set[2];
			dp_ipl[ipl][cpu].vdp_clr = clr[2];
		}
	}
	for (cpu = 0; cpu < NCPUS; cpu++) {
		dp_master[cpu].dp0_set = ~0;
		dp_master[cpu].dp0_clr = 0;
		dp_master[cpu].dp1_set = ~0;
		dp_master[cpu].dp1_clr = 0;
		dp_master[cpu].vdp_set = ~0;
		dp_master[cpu].vdp_clr = 0;
	}
}


/*
 * enable/disable ECC interrupts
 *
 * inputs:
 *	enable	1 == enable, 0 == disable.
 *
 * outputs:
 *	none.
 */
ecc_intr(enable)
	int	enable;
{
	int i;

	if ( enable ) {
		/* only CPU0 sees hardware errors */
		dp_imask_tab[SPL_IMASK_HWERR0][0].dp_mask_bit = 16; /* enable */
		dp_imask_tab[SPL_IMASK_HWERR1][0].dp_mask_bit = 16; /* enable */
		for (i=1; i<NCPUS; i++) {
			dp_imask_tab[SPL_IMASK_HWERR0][i].dp_mask_bit = 22;
			dp_imask_tab[SPL_IMASK_HWERR1][i].dp_mask_bit = 22;
		}
	}
	else {
		for (i=0; i<NCPUS; i++) {
			dp_imask_tab[SPL_IMASK_HWERR0][i].dp_mask_bit = 22;
			dp_imask_tab[SPL_IMASK_HWERR1][i].dp_mask_bit = 22;
		}
	}
}

void spl_init()
{
	init_dp_ipl(0);

        simple_lock_init( &spl_lock );
}
spl_init_early()
{
	init_dp_ipl(1);
}
int get_spl()
{
	return spl_current_ipl[cpu_number()];
}

int i860_interrupts_enabled()
{
        return (get_psr() & PSR_IM);
}

int spl0()
{
	return set_spl(SPL0);
}


int splDoorBell_noi()
{
        register int s = set_ipl_noi(SPLCPU);
        splon(PSR_IM);
        return s;
}

int splbio()
{
	return set_spl(SPLBIO);
}

int splnet()
{
	return set_spl(SPLSCAN);
}

int splnet_noi()
{
	int s = set_ipl_noi(SPLSCAN);
	splon(PSR_IM);
	return s;
}


int splclock()
{
	return set_spl(SPLCLOCK);
}


int splvm()
{
	return set_spl(SPLVM);
}


int splclock_noi()
{
	int s = set_ipl_noi(SPLCLOCK);
	splon(PSR_IM);
	return s;
}


int splmsg()
{
	return set_spl(SPLMSG);
}

int splmsg_noi()
{
	int s = set_ipl_noi(SPLMSG);
	splon(PSR_IM);
	return s;
}


int splhigh()
{
	return set_spl(SPLHI);
}


int splhi()
{
	return set_spl(SPLHI);
}


int splimp()
{
	return set_spl(SPLIMP);
}


int splsched()
{
	return set_spl(SPLSCHED);
}


int splsoftclock()
{
	return set_spl(SPLSOFTCLOCK);
}


int spltty()
{
	return set_spl(SPLTTY);
}


/*
 *	Return to interrupt priority level "old"
 */
void splx(old)
	int	old;
{
	(void) set_spl(old);
}


/*
 *	Return to interrupt priority level "old", but
 *	leave interrupts disabled in the psr.
 */
void splx_noi(old)
int	old;
{
	(void) sploff();
	(void) set_ipl_noi(old);
}

/*
 *	Arrange for softclock() to be called the next time
 *	the interrupt priority level is set to 0.
 */
void setsoftclock(int cpu)
{
	spl_soft_clock_needed[cpu]++;
}


int soft_clock_no_int, soft_clock_in_int;

/*
 *	Set the interrupt priority level to "new", and return
 *	the old ipl.
 */
static int set_spl(new)
	int	new;
{
	int	old, s, cpu;

again:
	if ((new < 0) || (new >= NSPL)) {
		printf("set_spl(new=%d)???\n", new);
		s = sploff();
		(void) set_ipl_noi(SPLHI);
		gimmeabreak();
	}
	assert(new >= 0 && new < NSPL);
	s = sploff();
	if (new == SPL0)
		s |= PSR_IM;

	old = set_ipl_noi(new);
	splon(s);

	return old;
}

static int set_ipl_noi(new)
	int	new;
{
	int	old, cpu;
	struct dp_ipl	*dp;
	struct dp_ipl	*dpm;
	unsigned long	set0, set1, clr0, clr1;

	assert(new >= 0 && new < NSPL);
	if ((new < 0) || (new >= NSPL)) {
		printf("set_ipl_noi: new=%d,psr=%x,ipl=%d!!\n",
			new, get_psr(), spl_current_ipl[cpu_number()]);
	}
	cpu = cpu_number();

	old = spl_current_ipl[cpu];

	if (old == new)
		return old;

	spl_current_ipl[cpu] = new;
	dp = &dp_ipl[new][cpu];
	dpm = &dp_master[cpu];

	set0 = dp->dp0_set & dpm->dp0_set;
	clr0 = dp->dp0_clr | dpm->dp0_clr;
	set1 = dp->dp1_set & dpm->dp1_set;
	clr1 = dp->dp1_clr | dpm->dp1_clr;

	simple_lock( &spl_lock );

	outl(DP_CONTROL0_CLEAR, clr0);
	outl(DP_CONTROL0_SET,   set0);
	outl(DP_CONTROL1_CLEAR, clr1);
	outl(DP_CONTROL1_SET,   set1);

	if (paragon_mp3) {
		unsigned long	set2, clr2;

		set2 = dp->vdp_set & dpm->vdp_set;
		clr2 = dp->vdp_clr | dpm->vdp_clr;

		outl(dpclr[VDP], clr2);
		outl(dpset[VDP], set2);
	}

	simple_unlock( &spl_lock );

	return old;
}

#include <mio.h>
#if	NMIO > 0
int	splmio_noi()
{
	int s = set_ipl_noi(SPLTTY);
	splon(PSR_IM);
	return s;
}
#endif	NMIO > 0

#include <sdb.h>
#if	NSDB > 0
int	splsdb_noi()
{
	int s = set_ipl_noi(SPLTTY);
	splon(PSR_IM);
	return s;
}
#endif	NSDB > 0

/*
 * Disable NIC interrupt to cpu0, and make sure it doesn't come back.
 * called from mcmsg_mp_init() running on cpu0.
 */

void
remove_nic_interrupt()
{
	int	i;

	for (i = 0; i < NSPL; i++) {
		dp_ipl[i][0].dp0_set &= ~1;
	}
	outl(DP_CONTROL0_CLEAR, 0x001);
}

/*
 * Enable the NIC interrupt to the MCP (cpu1)
 * called from the MCP when it enters mcmsg_mp_dispatch().
 */
void
nic_interrupt_enable_mcp()
{
	outl(DP_CONTROL0_SET,   0x100);
}
