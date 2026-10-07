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
 * $Id: interrupt.c,v 0.57 1995/04/07 17:20:28 lenb Exp $
 */
#include <cpus.h>
#include <mach_kdb.h>
#include <mach_assert.h>

#include <kern/assert.h>
#include <kern/ast.h>
#include <kern/cpu_number.h>
#include <kern/lock.h>
#include <kern/processor.h>
#include <mach/machine.h>
#include <i860/thread.h>
#include <i860paragon/dp.h>
#include <i860paragon/ecc.h>
#include <i860paragon/lbus.h>
#include <i860paragon/led.h>
#include <i860paragon/nic.h>
#include <i860paragon/spl.h>
#include <i860/psl.h>

#include <i860paragon/expansion.h>
#include <i860paragon/sdb/sdb.h>

#include <i860paragon/baton.h>
#include <i860paragon/mp.h>
#if     NCPUS > 1
#define	MP_DEBUG MACH_ASSERT
#endif

extern unsigned long	inl();
extern unsigned long	dpset[NDPS];
extern unsigned long	dpclr[NDPS];
extern unsigned long	dpstatus[NDPS];

int	la_trigger_spurious;	/* XXX */
int	spurious_interrupts[NCPUS];
int	show_spurious_interrupts = 0;
int	interrupt_nesting_level[NCPUS];

unsigned int parity_count_nic[NCPUS];
unsigned int parity_count_trap[NCPUS];
unsigned int parity_count_intr[NCPUS];

char *mdc_str="Memory Daughtercard";
char *base_str="Baseboard Memory";

int	mcmsg_msgp_bear;	/* for postmortem debug */

/*
 * We declare a 64-bit integer timestamp as a union because
 * the only 64-bit loads available are for 64-bit floating doubles.
 */
typedef union {
	double	d;
	unsigned int	i[2];
} timestamp;

#define	EPOCH_DADDR	((volatile double *)(0xffff4000+0x630))
#define snapshot(t) (t).d = *EPOCH_DADDR
#define timestamp_to_int_cycles_high(ts)	(unsigned int)((ts).i[1])
#define timestamp_to_int_cycles_low(ts)		(unsigned int)((ts).i[0])
#define timestamp_ll_to_double(ts) \
	((double)((ts).i[1]) * (4.294967296e+9) + (double)((ts).i[0]))

timestamp nic_parity_intr_ts;
timestamp nic_parity_trap_ts;

#if	NCPUS > 1
#if	MP_DEBUG
unsigned	clocks[NCPUS];
int		slave_ints;
int		slave_noise=0;
int             cross_rcv[NCPUS]; /* MPDEBUG */
#endif

#if NCPUS > 1 && MP_DEBUG
#if NCPUS <= 3
static int	max_nesting[3]={6,6,6};
#else
/* #error	ERROR: this file assumes NCPUS <= 3 */
extern	FORCE_COMPILE_TIME_ERROR();
#endif	NCPUS <= 3
#endif	NCPUS > 1 && MP_DEBUG

extern int      spl_current_ipl[NCPUS];
extern int      cross_xmit[];
#endif	/* NCPUS > 1 */

int		(*expansion_interrupt)();
static int	dp_hardware_error();
static int	dp_timer_interrupt();

#if	MCMSG
extern int	mcmsg_reentry;
#endif	MCMSG

/*
 *	central interrupt dispatch.
 *
 *	we arrive and leave this place at sploff and without the baton.
 *
 *	the individual interrupt service routines are responsible
 *	for acquiring the baton after they enable interrupts, and
 *	releasing the baton bafore they return.
 *
 *	walk the status bits in the DP ASIC and dispatch
 *	accordingly.  "hang_time" can be used to spin
 *	in this routine looking for an interrupt that
 *	is expected to arrive sometime soon.
 */
void
interrupt(regs)
	register struct i860_saved_state *regs;
{
	unsigned long	stat0, stat1, mask0, mask1, bits0, bits1, bits2=0;
	int		s, hwerr;
	int		my_cpu = cpu_number();
	int		hang_time = 0;
	int		seen_some = 0;

	interrupt_nesting_level[my_cpu]++;

	if (my_cpu == master_cpu)
	{
		RED_ON(RED_INT);
	}

hang_in_there:

	/*
	 * For the other 2 processors, execute the same code as was
	 * used on the GP node (2 processors)
	 */

	/*
	 * read the DP ASIC status and control registers.
	 */
	if (my_cpu == 2) {
		bits2 = (inl(VDP_STATUS) & inl(DP_CONTROL2_READ)) & 0x30;
		stat0=stat1=mask0=mask1=0;
	}
	else {
		stat0 = inl(DP_STATUS_LO);
		stat1 = inl(DP_STATUS_HI);
		mask0 = inl(DP_CONTROL0_READ);
		mask1 = inl(DP_CONTROL1_READ);
	}
		

	/*
	 * CPU1's (user processor) mask bits are 8..15 where CPU0's bits
	 * are 0..7. Move CPU1's bits into CPU0's position so the following
	 * code will act correctly.
	 */
	if ( my_cpu == 1) {
		/*
		 * DP low
		 * isolate USR(cpu1) mask bits and shift to where cpu0 bits
		 * are normally found. Preserve bit #17 (int on error).
		 */
		mask0 &= ~0x100ff;	/* clear cpu 0 mask bits */
		mask0 |= ((mask0 & 0xff00) >> 8); /* cpu1 -> cpu0 bits */
		mask0 &= 0x200ff;	/* clear unused bits */

		/*
		 * DP high
		 * isolate USR(cpu1) mask bits and shift to where cpu0 bits are
		 * normally found.
		 */
		mask1 &= ~0x100ff;	/* clear cpu 0 mask bits */
		mask1 |= ((mask1 & 0xff00) >> 8);
		mask1 &= 0x200ff;	/* clear unused bits */
	}

	/*
	 * For the Master Processor, mask out the interrupts intended for
	 * USER0
	 */
	if ( my_cpu == 0) {
		stat1 &= ~0xFF;
	}


	/*
	 * Determine which interrupts reasons correspond to the current
	 * spl level.
	 */
	bits0 = stat0 & mask0;
	bits1 = stat1 & mask1;

	assert( interrupt_nesting_level[my_cpu] > 0 );
	assert( interrupt_nesting_level[my_cpu] < 10 );

#if	NCPUS > 1 && MP_DEBUG 
        if ( interrupt_nesting_level[my_cpu] > max_nesting[my_cpu] ) {
                max_nesting[my_cpu] = interrupt_nesting_level[my_cpu];
                printf("nest %d b(%x,%x) m(%x,%x) spl(%d,%d) dbell:r(%d,%d) x(%d,%d)\n",
                        max_nesting[my_cpu],bits0,bits1,mask0,mask1,
                        spl_current_ipl[0],spl_current_ipl[1],
                        cross_rcv[0], cross_rcv[1], cross_xmit[0], cross_xmit[1]
                );

        }
#endif	/* NCPUS > 1 && MP_DEBUG */

        /*
         * Check for hardware errors: single-bit corrected ECC, parity
         * bit cpu0(bit 16) or cpu1(bit 17) enable interrupts for bits (8+9+10)
         */
	s = -1;		/* indicate we have not handled this interrupt */

	/* 
	 * any hardware errors? Some are correctable.
	 */
	hwerr = (1 << DP_IMSK_HWERR_CPU0) | (1 << DP_IMSK_HWERR_CPU1);

	if ( (mask0 & hwerr) && (stat0 & DP_ISTAT_HW_ERROR0) ||
	     (mask1 & hwerr) && (stat1 & DP_ISTAT_HW_ERROR0) )
	{
		assert(my_cpu == master_cpu);

		if(dp_hardware_error(regs, DP0, stat0, stat1))
		{
			panic("Memory error");
		}
		goto intr_exit;
	}
	/*
	 * Cross processor interrupt; did the door bell ring?
	 * DP_INTR_CPU0 works for both processor as cpu1's interrupt status
	 * bits are in the same cpu0's by virture of preceeding code.
	 * CPU0 see's it doorbell interrupt on DPlow asic while CPU1 uses
	 * DPhigh.
	 */
	if ( (bits0 | bits1 | bits2) & (1<<DP_INTR_CPU0) )
	{
		s = dp_cpu_interrupt(regs, my_cpu);
	}

        /*
         * clock tick?
         *      cpu 0 uses/(sees only) DPlow asic timer interrupts.
         *      cpu 1 uses/(sees only) DPlhigh asic timer interrupts.
         */
        else if ( (bits0 | bits1 | bits2) & DP_ISTAT_TIMER ) {
                s = dp_timer_interrupt(regs, my_cpu);
	}
	else if (bits0 & DP_ISTAT_NIC) {
		s = nic_interrupt(regs);
#define	HANG_TIME_FLAG NSPL		/* move me to a .h file */
		if (s >= HANG_TIME_FLAG)
		{
			s -= HANG_TIME_FLAG;
			hang_time = 1;
		}
	}


	/*
	 * Expansion card interrupt?
	 */
	else if (bits0 & DP_ISTAT_EXP) {
		assert(expansion_interrupt != (int (*)()) 0);
		/*
		 * miointr() grabs baton,
		 * are there other expansion interrupt routines?
		 */
		s = (*expansion_interrupt)(regs);
	}

	/*
	 * LTU interrupt?
	 */
	else if (bits1 & (DP_ISTAT_LTU0_CNT | DP_ISTAT_LTU1_CNT)) {
		/* We expect LTU operations to be polled */
		panic("Unexpected LTU %s interrupt!",
		      (bits1 & DP_ISTAT_LTU0_CNT) ? "receive" : "transmit");
	}

	/*
	 * Did the 'INT_TO_NODE' line go low?
	 */
	else if (bits0 & DP_ISTAT_NET) {
		s = scan_interrupt(regs);
	}

	/*
	 * Swing around again?
	 */
	if (hang_time > 0) {
		if (s != -1) {
			splx_noi(s);
			seen_some++;
		}
		s = -1;
		hang_time--;
		goto hang_in_there;
	}

	/*
	 * Nobody owns this interrupt?
	 */
	if ((s == -1) && (seen_some == 0))
	{

		if (la_trigger_spurious)	/* XXX */
			inl(0x60300000);	/* XXX */

		spurious_interrupts[my_cpu]++;

		if (show_spurious_interrupts) {
			printf("\nspurious interrupt! psr=0x%x,spl=%d,stat=0x%08x:%08x,mask=0x%08x:%08x\n",
				get_psr(), get_spl(), stat1, stat0, mask1, mask0);
		}
	}
	if (s != -1) {
		extern int	spl_soft_clock_needed[];

#if	MACH_ASSERT
		/*
		 * we should never get here with interrupts off
		 */
		if (!i860_interrupts_enabled())
		{
			printf("ERROR: interrupt: psr 0x%x\n", get_psr());
			printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
				stat0, stat1, mask0, mask1,
				bits0, bits1, bits2);
			printf("0x%x 0x%x\n",
				spl_soft_clock_needed[my_cpu], s);
			gimmeabreak();
		}
#endif	/* MACH_ASSERT */

		/*
		 * Do we need a softclock interrupt?
		 * Must have been at spl0.
		 */
		if ( spl_soft_clock_needed[my_cpu] && (s == SPL0) ) {
			spl_soft_clock_needed[my_cpu] = 0;

			(void) splsoftclock();
			baton_enter();
			softclock();
			baton_exit();
		}
		splx_noi(s);
	}

intr_exit:
	if (--interrupt_nesting_level[my_cpu] == 0)
	{
		if (my_cpu == master_cpu)
		{
			RED_OFF(RED_INT);
		}
	}
	return;
}

/*
 *      Memory expansion card hardware error interrupts.
 */
void
memexp_hw_err_interrupt(struct i860_saved_state *regs)
{
	if(dp_hardware_error(regs, MXDP0, inl(dpstatus[MXDP0]),
				inl(dpstatus[MXDP1])))
	{
		/*
		 * could be either ECC UE or Parity
		 */
		panic("Memory Error detected by Expansion Card.");
	}
}

/*
 * Correctable ECC and Parity handler control, see i860paragon/ecc.h
 */

unsigned int parity_control_nic =
		0;

unsigned int parity_control_trap =
		ECC_CONTROL_STOP |
		ECC_CONTROL_PRINT_ALL |
		ECC_CONTROL_RED_LED |
		0;

unsigned int parity_control_intr =
		ECC_CONTROL_STOP |
		ECC_CONTROL_PRINT_ALL |
		ECC_CONTROL_RED_LED |
		0;

unsigned int ecc_control_ce =
		ECC_CONTROL_PRINT_2 |
		0;

#define ECC_CE_PANIC_AT (1 << 20)	/* all this many ce intrs < panic */
#define PARITY_LIMIT_NIC (1 << 20)	/* red light to indicate thrashing */

int nic_intr_delay_usec = 40;
int nic_parity_credits[NDPS];
/*
 * handle_nic_parity_intr()
 *
 * if NIC parity trap happened within the last sec,
 * assume this DP trap is the same error.
 *
 * return 1 to stop the show
 * return 0 to continue
 */

static int
handle_nic_parity_intr(char *ramstr, int which, unsigned long status0,
	unsigned long status1)
{
	int mycpu = cpu_number();
	int stop = parity_control_nic & ECC_CONTROL_STOP;
	double now_d, then_d;
	char *dp_str;
	int dp0 = 0;
	int dp1 = 0;

	/*
	 * If the trap was on this cpu, it already ran.
	 * If it is on another cpu, we may have to delay a few cycles
	 * to be sure we lose the race.
	 */

	snapshot(nic_parity_intr_ts);

	now_d = timestamp_ll_to_double(nic_parity_intr_ts);
	then_d = timestamp_ll_to_double(nic_parity_trap_ts);

	if (now_d > (then_d + 2000))	/* look back 40 usec */
	{
		delay(nic_intr_delay_usec);	/* wait another 40 usec */

		/* re-read trap timestamp */
		then_d = timestamp_ll_to_double(nic_parity_trap_ts);
		if (now_d > (then_d + 4000))	/* look back 80 usec */
		{
			return (1);
		}
	}

	/*
	 * Assure this mechanism does not ignore more than 1 parity
	 * error interrupt per dp.
	 */

	if (status0 & DP_ISTAT_PARITY0)
	{
		dp0 = 1;
		dp_str = "DP0";
		if (nic_parity_credits[which] == 0)
		{
			stop = 1;
		}
		nic_parity_credits[which] = 0;
	}
	if (status1 & DP_ISTAT_PARITY0)
	{
		dp1 = 1;
		dp_str = "DP1";
		if (nic_parity_credits[which + 1] == 0)
		{
			stop = 1;
		}
		nic_parity_credits[which + 1] = 0;
	}

	if (dp0 && dp1)
	{
		dp_str = "DP0 and DP1";
	}

	/*
	 * else we've had a nic status parity trap sufficiently recently.
	 * print something if we're also printing nic parity traps.
	 */

	if ((parity_control_nic & ECC_CONTROL_PRINT_ALL) ||
		((parity_control_nic & ECC_CONTROL_PRINT_2) &&
			(popcnt(parity_count_nic[mycpu]) == 1)))
	{
		printf("NOTICE:%s NIC status parity intr,"
			" detected by %s %s.\n",
			stop ? "" : " Ignored", ramstr, dp_str);

		/*
		 * base LED decision on union of parity_control_intr
		 * and parity_control_nic.
		 */
		if ((parity_control_intr | parity_control_nic)
			& ECC_CONTROL_RED_LED)
		{
			RED_ON(RED_ECC);
		}
	}

	return (stop);
}

/*
 * We're here because a DP detected a bus parity error.
 * If handle_parity_trap() has recently ignored a parity trap on
 * NIC status register read, we ignore this interrupt.
 */
/*ARGSUSED*/
static int
dp_parity_error(regs, which, status0, status1, ramstr)
	register struct i860_saved_state *regs;
	register int which;
	register unsigned long status0;
	register unsigned long status1;
	char *ramstr;
{
	int mycpu = cpu_number();
	int which_dp;
	char *dp_str;
	int dp0 = 0;
	int dp1 = 0;

	parity_count_intr[mycpu]++;

	if (!handle_nic_parity_intr(ramstr, which, status0, status1))
		return (0);

	if ((expansion_id() == EXP_ID_SCSI) &&
	    (sdb_configuration & SDB_CONFIGURATION_IGNORE_PARITY)) {
		return (0);
	}

	if (status0 & DP_ISTAT_PARITY0)
	{
		dp_str = "DP0";
		dp0 = 1;
	}
	if (status1 & DP_ISTAT_PARITY0)
	{
		dp_str = "DP1";
		dp1 = 1;
	}
	if (dp0 && dp1)
	{
		dp_str = "DP0 and DP1";
	}

	if ((parity_control_intr & ECC_CONTROL_PRINT_ALL) ||
		((parity_control_intr & ECC_CONTROL_PRINT_2) &&
			(popcnt(parity_count_intr[mycpu]) == 1)))
	{
		printf("WARNING: %d Bus Parity Errors detected by %s %s.\n",
			parity_count_intr[mycpu],
			ramstr, dp_str);

		if (parity_control_intr & ECC_CONTROL_RED_LED)
		{
			RED_ON(RED_ECC);
		}
	}

	if (parity_control_intr & ECC_CONTROL_STOP)
	{
		return (1);
	}

	return (0);
}

/*
 * This system can't recover from Uncorrectable ECC errors
 * because the hardware does not latch the address of the error,
 * and doesn't synchronously tell us about the error,
 * so we can't reliably know which CPU took the error
 * or if it was in user or kernel mode.
 */
/*ARGSUSED*/
static int
dp_ecc_log_ue(regs, which, status0, status1, ramstr)
	register struct i860_saved_state *regs;
	register int which;
	register unsigned long status0;
	register unsigned long status1;
	char *ramstr;
{
	int dp0 = 0;
	int dp1 = 0;

	if (status0 &  DP_ISTAT_ECC0)
		dp0 = 1;

	if (status1 &  DP_ISTAT_ECC0)
		dp1 = 1;

	printf("WARNING: Uncorrectable ECC Error detected on %s%s%s.\n",
		ramstr, dp0 ? " dp0" : "", dp1 ? " dp1" : "");

	/*
	 * The ECC address registers are interesting only
	 * upon uncorrectable errors latched by both DPs
	 */
	if (dp0 && dp1)
	{
		unsigned long ecc0, ecc1;
		if (which >= MXDP0) {
			ecc0 = inl(MXDP_DRAM_ECCERR0);
			ecc1 = inl(MXDP_DRAM_ECCERR1);
		} else {
			ecc0 = inl(DP_DRAM_ECCERR0);
			ecc1 = inl(DP_DRAM_ECCERR1);
		}
		printf("\tecc1.ecc0: 0x%08x.%08x\n", ecc1, ecc0);
	}

	return (1);
}

struct {
	unsigned int last_syndrome;
	unsigned int last_syndrome_count;
} ecc_log[NDPS];

unsigned int ecc_total_ce_intr_base;
unsigned int ecc_total_ce_intr_mdc;

/*
 * log ECC Correctable Error.
 * return 0 to continue, return 1 to stop the show
 */
static int
dp_ecc_log_ce(which, status0, status1, ramstr)
	register int which;
	register unsigned long status0;
	register unsigned long status1;
	char *ramstr;
{
	int print_it = 0;
	int dp0, dp1;
	int retval = 0;

	if (which >= MXDP0)
	{
		dp0 = MXDP0;
		dp1 = MXDP1;
		ecc_total_ce_intr_mdc++;
	}
	else
	{
		dp0 = DP0;
		dp1 = DP1;
		ecc_total_ce_intr_base++;
	}

	if (ecc_control_ce & ECC_CONTROL_PRINT_ALL)
		print_it = 1;

	if (status0 & DP_ISTAT_SEC0)
	{
		unsigned int syndrome0 = (status0 >> 24);

		if (ecc_log[dp0].last_syndrome != syndrome0)
		{
			ecc_log[dp0].last_syndrome = syndrome0;
			ecc_log[dp0].last_syndrome_count = 0;
		}
		ecc_log[dp0].last_syndrome_count += 1;

		
		if (!print_it && ((ecc_control_ce & ECC_CONTROL_PRINT_2)
			&& (popcnt(ecc_log[dp0].last_syndrome_count) == 1)))
		{
			print_it = 1;
		}
	}
	if (status1 & DP_ISTAT_SEC0)
	{
		unsigned int syndrome1 = (status1 >> 24);

		if (ecc_log[dp1].last_syndrome != syndrome1)
		{
			ecc_log[dp1].last_syndrome = syndrome1;
			ecc_log[dp1].last_syndrome_count = 0;
		}
		ecc_log[dp1].last_syndrome_count += 1;

		
		if (!print_it && ((ecc_control_ce & ECC_CONTROL_PRINT_2)
			&& (popcnt(ecc_log[dp1].last_syndrome_count) == 1)))
		{
			print_it = 1;
		}
	}

	/*
	 * detect the case where we're thrashing by
	 * taking too many correctable errors.
	 */
	if ((ecc_total_ce_intr_base + ecc_total_ce_intr_mdc) ==
		ECC_CE_PANIC_AT)
	{
		print_it = 1;
		retval = 1;
	}

	if (print_it)
	{
		int dp0_count = ecc_log[dp0].last_syndrome_count;
		int dp1_count = ecc_log[dp1].last_syndrome_count;

		printf("NOTICE: %d Correctable ECC Errors"
			" detected on %s.\n\t",
			(which >= MXDP0) ? ecc_total_ce_intr_mdc :
			ecc_total_ce_intr_base, ramstr);

		if (dp0_count)
			printf("%d @ dp0 syn=0x%02x%s",
				dp0_count, ecc_log[dp0].last_syndrome,
				dp1_count ? "; " : "\n");

		if (dp1_count)
			printf("%d @ dp1 syn=0x%02x\n",
				dp1_count, ecc_log[dp1].last_syndrome);
		
		if (ecc_control_ce & ECC_CONTROL_RED_LED)
		{
			RED_ON(RED_ECC);
		}
	}

	if (ecc_control_ce & ECC_CONTROL_STOP)
	{
		printf("BOOT_ECC_CONTROL=s - stopping for CE.\n");
		gimmeabreak();
	}

	return (retval);
}

/*
 * dp_hardware_error()
 *
 *	Clear the DP interrupt source.
 *	Diagnose ECC and parity errors
 *
 *	Return non-zero for un-recoverable errors, else return 0.
 *
 *	This routine reads the DP registers in pairs.
 *	'which' identifies the pair as either on the
 *	baseboard (DP0 | DP1) or on the MDC (MXDP0 | MXDP1)
 */

static int dp_hardware_error(regs, which, status0, status1)
	register struct i860_saved_state *regs;
	register int which;
	register unsigned long status0;
	register unsigned long status1;
{
	int	cc = 0;
	unsigned long	status = status0 | status1;
	char *ramstr;

	/*
	 * Clear DP hardware error interrupt line.
	 */
	if (which < MXDP0)
	{
		outl(dpset[DP0], (1 << DP_CTRL_CLEAR_HWERR));
		outl(dpset[DP1], (1 << DP_CTRL_CLEAR_HWERR));
		ramstr = base_str;
	} else
	{
		outl(dpset[MXDP0], (1 << DP_CTRL_CLEAR_HWERR));
		outl(dpset[MXDP1], (1 << DP_CTRL_CLEAR_HWERR));
		ramstr = mdc_str;
	}

	if (status & DP_ISTAT_PARITY0)
	{
		cc |= dp_parity_error(regs, which, status0, status1, ramstr);
	}

	/*
	 * do CE before UE in case we get both the printout looks right.
	 */
	if (status & DP_ISTAT_SEC0)
	{
		cc |= dp_ecc_log_ce(which, status0, status1, ramstr);
	}

	if (status & DP_ISTAT_ECC0)
	{
		cc |= dp_ecc_log_ue(regs, which, status0, status1, ramstr);
	}

#if	MACH_ASSERT
	if (!(status & (DP_ISTAT_PARITY0 | DP_ISTAT_SEC0 | DP_ISTAT_ECC0)))
	{
		/* unknown error */
		printf("Unknown %s DP Status 0x%08x.%08x\n",
			ramstr, status1, status0);
		cc = 1;
	}
#endif	MACH_ASSERT

	return (cc);
}

/*
 * handle_parity_trap()
 * decides what to do about a *synchronous* parity error and does it.
 * if it returns, then we survived.  Lives here simply because the async
 * parity handler lives here.
 */


void
handle_parity_trap(struct i860_saved_state *regs)
{

	int mycpu = cpu_number();	/* always 0 for NCPUS=1 */
	int i_am_mcp;
	int bear;
	extern int mcmsg_mp_enable;

	bear = get_bear();		/* unlatch bear */

	i_am_mcp = (cpu_number1() == 1 && mcmsg_mp_enable);	/* can be 1 */

	if (i_am_mcp)
		mcmsg_msgp_bear = bear;	/* for postmortem debug */

	/*
	 * if the parity error is on the NIC status register
	 * optionally print out something & set RED_LED, then continue.
	 */

	if (bear == (NIC_ADDR_PH + 0x18))
	{
		int i;
		int print_it = 0;
		int stop = (parity_control_nic & ECC_CONTROL_STOP);

		parity_count_nic[mycpu]++;

		snapshot(nic_parity_trap_ts);

		for (i = 0; i < NDPS; ++i)
			nic_parity_credits[i] = 1;

		if ((parity_control_nic & ECC_CONTROL_PRINT_ALL) ||
			((parity_control_nic & ECC_CONTROL_PRINT_2) &&
			(popcnt(parity_count_nic[mycpu]) == 1)))
		{
			print_it = 1;
		}
		if (parity_count_nic[cpu_number()] == PARITY_LIMIT_NIC)
		{
			printf("too many nic parity errors\n");
			parity_control_nic |= ECC_CONTROL_RED_LED;
			print_it = 1;
			/* stop = 1; */
		}
		if (print_it)
		{
			printf("cpu%d: %d NIC status register"
				" parity errors %s\n",
				cpu_number1(), parity_count_nic[mycpu],
				stop ? "" : "ignored");

			if (parity_control_nic & ECC_CONTROL_RED_LED)
			{
				RED_ON(RED_ECC);
			}
		}
		if (stop)
		{
#if	NCPUS == 1
			if (i_am_mcp)
			{
				printf("MCP detected NIC parity error"
					" pc 0x%x regs 0x%x\n",
					regs->pc, regs);
				mcmsg_msgp_assert("interrupt.c",__LINE__);
			}
			else
#endif	NCPUS == 1
				panic("Unrecoverable NIC status parity error");
		}
	} else
	{
		parity_count_trap[mycpu]++;

		if ((parity_control_trap & ECC_CONTROL_PRINT_ALL) ||
			((parity_control_trap & ECC_CONTROL_PRINT_2) &&
			(popcnt(parity_count_trap[mycpu]) == 1)))
		{
			printf("WARNING: cpu%d processor %s error at 0x%x!\n",
				cpu_number1(), (regs->epsr & EPSR_PEF) ?
				" parity": " bus", bear);
#if	MACH_KDB
			(void) show_860_regs(regs);
#endif	MACH_KDB
			if (parity_control_trap & ECC_CONTROL_RED_LED)
			{
				RED_ON(RED_ECC);
			}
		}
		if (parity_control_trap & ECC_CONTROL_STOP)
		{
#if	NCPUS == 1
			if (i_am_mcp)
			{
				printf("MCP detected parity error"
					" pc 0x%x regs 0x%x\n",
					regs->pc, regs);
				mcmsg_msgp_assert("interrupt.c",__LINE__);
			}
			else
#endif	NCPUS == 1
			panic("Unrecoverable data error");
		}
	}

	return;
}

int	sw_refresh_enable;

void
sw_refresh()
{
asm("		orh	0xf800, r0, r16		// memory address	");
asm("		adds	8, r16, r20		// other bank		");
asm("		adds	16384, r0, r17		// RAM page size	");
asm("		adds	1023, r0, r18		// count		");
asm("		adds	-1, r0, r19		// decrement		");
asm("		bla	r19, r18, sr1		// jumpstart loop	");
asm("		 nop				//			");
asm("sr1:	ldio.l	r16, r0			// reference bank 0	");
asm("		addu	r16, r17, r16		// increment address	");
asm("		ldio.l	r20, r0			// reference bank 1	");
asm("		bla	r19, r18, sr1		// loop back		");
asm("		 addu	r20, r17, r20		// increment other addr	");
	return;
}

#if	NCPUS > 1

#define	SLAVE_CLOCK 0
#if	SLAVE_CLOCK
/*
 * an example of how the maste cpu might distribute clock interrupts out
 * to the slave processor via the doorbell interrupt.
 */

slave_clock_interrupt( mycpu, prev_spl, regs )
	int	mycpu;
	int	prev_spl;
	struct i860_saved_state *regs;
{
	int		s;

	s = splclock();

	clocks[mycpu]++;

	if ( slave_ints++ > 30*60) {
		if ( slave_noise ) {
			extern int do_trap1,ctrap_it[2];
			extern int stack_check_usage;

			printf("\n(0)xmt %d rcv %d (1)%d %d\n",
				cross_xmit[0],cross_rcv[0],
				cross_xmit[1],cross_rcv[1]);

			/* if ( do_trap1 ) */
				printf("it[1] %d clocks(0)%d (1)%d\n",
					ctrap_it[1],clocks[0],clocks[1]);

			if ( stack_check_usage ) {
				unsigned total,maxusage;

				stack_statistics(&total, &maxusage);
				printf("stack: max %d Inest (0)%d (1)%d\n",
						maxusage,
						max_nesting[0],max_nesting[1]);
			}
			else
				printf("nest(%d,%d)\n",
						max_nesting[0],max_nesting[1]);
		}
		slave_ints = 0;
	}
	if ( machine_slot[mycpu].running == TRUE)
		hardclock(0, prev_spl, regs);
	splx(s);
}
#endif	/* SLAVE_CLOCK */
#endif	/* NCPUS > 1 */

/*
 * Door bell interrupt from a DP ASIC. Collect the doorbell interrupt reasons
 * and process. Loop until no doorbell reasons are left.
 *
 * inputs:
 *	regs	exception frame pointer
 *	cpu	my_cpu santiy check
 *
 * outputs:
 *	interrupt level at time of interrupt.
 */

/*
 * may be entered with or without the baton depending on interrupt nesting.
 * doorbell interrupt handlers are not permitted to block on the baton,
 * as they could then block receipt of PMAP doorbells and hang the system.
 */
#if	NCPUS > 1
int	old_style_doorbell_service = 0;
#else	/* NCPUS > 1 */
int	old_style_doorbell_service = 1;
#endif	/* NCPUS > 1 */

dp_cpu_interrupt(regs, cpu)
	register struct i860_saved_state *regs;
	register int	cpu;
{
	int	s;
	register int		reasons;
	register int		*myword;

	assert(cpu == cpu_number());

#if	MACH_ASSERT
	if ( (s=get_spl()) >= SPLCPU ) {
		printf("dp_cpu_interrupt() bad spl %d, looping.\n",s);
		while( 1 ) ;
	}
#endif	MACH_ASSERT

	/* 
	 * clear interrupt reason and enable interrupts.
	 */

	outl(dpclr[cpu], (1 << DP_CTRL_ATTEN_OTHER));
	s = splDoorBell_noi();

	myword = &cpu_int_word[cpu];
	/*
	 * Valid reason for being here?
	 * atomic fetch and clear door bell interupt reasons.
	 */
	if ( (reasons = atomic_set(myword,0)) == 0 )
		return s;

	/*
	 * process all cross-processor interrupt reasons while we are here!
	 */
	do {
#if	MP_DEBUG
		cross_rcv[cpu]++;
#endif
		if ( reasons & (1 << MP_TLB_FLUSH) ) {
			pmap_update_interrupt();
		}

#if	SLAVE_CLOCK
		if ( reasons & (1 << MP_CLOCK) ) {
			slave_clock_interrupt(cpu,s,regs);
		}
#endif

#if	NCPUS > 1
		if ( reasons & (1 << MP_KDB) ) {
                        extern kdb_is_slave[];

                        kdb_is_slave[cpu]++;
			/* just like a keyboard interrupt */
			kdb_kintr( regs );
		}
#endif	/* NCPUS > 1 */

		if ( reasons & (1 << MP_AST) ) {
			ast_check();
		}

		if ( reasons & (1 << MP_HALT) ) {
			halt_cpu();
		}

		if ( reasons & (1 << MP_NETSEND) ) {
			ast_on(cpu_number(), AST_NETSEND);
		}

		if ( reasons & (1 << MP_NETRECV) ) {
			ast_on(cpu_number(), AST_NETRECV);
		}

#if	MCMSG_ENG
		if ( old_style_doorbell_service ) {

			if ( reasons & (1 << MP_RPCREQ) )
				rpc_engine_request_intr();

			if ( reasons & (1 << MP_RPCREPLY) )
				rpc_engine_reply_intr();

			if ( reasons & (1 << MP_RPCDEPART) )
				rpc_engine_depart_intr();

		} else {

			if ( reasons & (1 << MP_RPCREQ) )
				ast_on(cpu_number(), AST_RPCREQ);

			if ( reasons & (1 << MP_RPCREPLY) )
				ast_on(cpu_number(), AST_RPCREPLY);

			if ( reasons & (1 << MP_RPCDEPART) )
				ast_on(cpu_number(), AST_RPCDEPART);
		}
#endif	/* MCMSG_ENG */

#if	MCMSG_ENG
		if (old_style_doorbell_service) {

			if ( reasons & (1 << MP_RDMASEND) )
				rdma_engine_send_intr();

			if ( reasons & (1 << MP_RDMARECV) )
				rdma_engine_recv_intr();

			if ( reasons & (1 << MP_RDMATXF) )
				rdma_engine_send_fault_intr();

			if ( reasons & (1 << MP_RDMARXF) )
				rdma_engine_recv_fault_intr();

		} else {

			if ( reasons & (1 << MP_RDMASEND) )
				ast_on(cpu_number(), AST_RDMASEND);

			if ( reasons & (1 << MP_RDMARECV) )
				ast_on(cpu_number(), AST_RDMARECV);

			if ( reasons & (1 << MP_RDMATXF) )
				ast_on(cpu_number(), AST_RDMATXF);

			if ( reasons & (1 << MP_RDMARXF) )
				ast_on(cpu_number(), AST_RDMARXF);
		}
#endif	/* MCMSG_ENG */

#if	MCMSG
		if ( reasons & (1 << MP_AST_MCMSG) ) {
			ast_on(cpu_number(), AST_MCMSG);
		}
#endif	MCMSG
		if ( reasons & (1 << MP_SCAN_INPUT) ) {
			ast_on(cpu_number(), AST_SCAN_INPUT);
		}

		if ( reasons & (1 << MP_CONSLOG_AST) ) {
			ast_on(cpu_number(), AST_CONSLOG);
		}
#if	NOT_YET
		if ( reasons & (1 << MP_SCAN_OUTPUT) ) {
			ast_on(cpu_number(), AST_SCAN_OUTPUT);
		}
#endif	NOT_YET

	} while( (reasons = atomic_set(myword,0)) );
		
	return s;
}

/*
 *	Timer interrupt from the a DP ASIC
 *      'which' identifies which CPU, and thus which DP ASIC.
 */

/*
 * may be entered without the baton
 */
static int dp_timer_interrupt(regs, my_cpu)
	register struct i860_saved_state *regs;
	register int	my_cpu;
{
	int	s;

        /*
         * clear timer interrupt status bit from DP
         */
	outl(dpset[my_cpu], (1 << DP_CTRL_CLEAR_TIMER));

	s = splclock_noi();

	baton_enter();

#if	NCPUS > 1
#if	MP_DEBUG
	clocks[my_cpu]++;
#endif	/* MP_DEBUG */
	if ( my_cpu == master_cpu )
#endif	/* NCPUS > 1 */
	{
		scan_intr_poll();
#if	MCMSG
		mcmsg_nic_check();
#endif	MCMSG
	}

	/*
	 * if we are really up and running then process the clock tick
	 */
	if (machine_slot[my_cpu].running == TRUE)
	{
		hardclock(0, s, regs);
	}

	/*
	 * Lights, action... roll em.
	 */
	if (cpu_to_processor(my_cpu)->state == PROCESSOR_IDLE)
	{
		GREEN_OFF(GREEN_ACTIVE);
	}
	else
	{
		GREEN_ON(GREEN_ACTIVE);
	}

	if (my_cpu == master_cpu) {
		pretty_lights((regs->psr & PSR_PU) != 0);
	}

	if (sw_refresh_enable && my_cpu == master_cpu) {
		sw_refresh();
	}
#if	MCMSG
	if(mcmsg_reentry < 1)
#endif	/* MCMSG */
#if	NCPUS > 1
		if ( my_cpu == master_cpu )
#endif	/* NCPUS > 1 */
			scan_intr_poll();

	baton_exit();
	return (s);
}


/*
 *	Install a handler to call for expansion interrupts
 */
int interrupt_expansion_install(new, old)
	int	(*new)(), (**old)();
{
	*old = expansion_interrupt;
	expansion_interrupt = new;

	return 0;
}


/*
 *	Display a history of interrupts
 *
 *	XXX not yet retrofitted from the hypercube
 */
void
db_show_intr_hist()
{
	return;
}

/*
 *	Count bits
 */

char popcnt_data[256] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

popcnt(a)
	unsigned long	a;
{

	return	popcnt_data[ a        & 0xFF] +
		popcnt_data[(a >>  8) & 0xFF] +
		popcnt_data[(a >> 16) & 0xFF] +
		popcnt_data[(a >> 24)       ];
}
