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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_intr.c,v 1.29 1995/03/06 23:56:53 terry Exp $
 */

/*
 * msgp_intr.c
 *
 * Interrupt handler for multicomputer message passing
 */

#include <kern/assert.h>
#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/msgp/msgp_hw.h>
#include <i860paragon/spl.h>
#include <i860paragon/baton.h>

/*
 * Send store 
 * 
 * Ring buffer to store packet send calls while the outgoing FIFO is busy. 
 * The indices are unsigned characters so that they wrap automatically, 
 * so MAXSEND is required to be exactly 256 or code must change. 
 */ 

#define MAXSEND	256
typedef
struct mcmsg_send_store {
	int		(*routine)();
	mcmsg_task_t	*mcmsg_task;
	unsigned long	a[10];
	unsigned long	fill[4];
} mcmsg_send_store_t;

static mcmsg_send_store_t mcmsg_send_store[MAXSEND];
unsigned char mcmsg_send_store_in;	/* global for dispatch.s */
unsigned char mcmsg_send_store_out;	/* global for dispatch.s */

extern mcmsg_console_need_int;

extern char    *control_name[];		/* mcmsg_config.c */

/*
 * Debugging declarations
 *
 * mcmsg_reentry is used to detect a second entry into the message passing
 * code while some thread is already inside. It is only used in the single
 * processor case.
 */

int mcmsg_unexpected_send_intr;

#if	!MACH_ASSERT
int	mcmsg_reentry;
#endif	MACH_ASSERT

#define DEBUG_SEND_STORE 0

#if    DEBUG_SEND_STORE
#define MCMSG_TRACE_STORE(s,n,a,b,c,d) mcmsg_trace_debug(s,n,a,b,c,d)
#else  DEBUG_SEND_STORE
#define MCMSG_TRACE_STORE(s,n,a,b,c,d)
#endif DEBUG_SEND_STORE

/*
 * The mcmsg_send() technique depends on the compiler properly detecting
 * tail recursion, which it doesn't. The bad result is stack overflow.
 * mcmsg_send() is coded in asm with proper
 * tail recursion code, and there is a special entry mcmsg_send_tail()
 * which gets the caller to unwind its stack and then go to mcmsg_send().
 */

/*
 * Initialize send store indices
 */

mcmsg_init_send_store()
{

	mcmsg_send_store_in = 0;
	mcmsg_send_store_out = 0;
	mcmsg_hw.send_int_enable = 0;
}

/*
 * Stubs for configuration tables.
 *
 * mcmsg_send_sw_null is a nop used when no packet is to be sent after all.
 * mcmsg_send_sw_err  is used to fill in unused send slots.
 * mcmsg_recv_sw_err  is used to fill in unused receive slots.
 */

mcmsg_send_sw_null(mt)
	mcmsg_task_t	*mt;
{

	return 0;
}

mcmsg_send_sw_err(mt)
	mcmsg_task_t	*mt;
{

	mcmsg_trace_drop("invalid send packet", mt);
	assert(0);
	return 0;
}

mcmsg_recv_sw_err(hdr1, hdr2)
	unsigned long hdr1, hdr2;
{

	mcmsg_trace_drop("invalid receive packet",
			 hdr1 & 0xFFFF);
	mcmsg_trace_debug("invalid msg data", 2,
			hdr1, hdr2, 0, 0);
	mcmsg_flush_invalid();
}

/*
 *  Disable the transmit interrupts.
 */

void
mcmsg_disable_tx_interrupts()
{

	/*
	 * Clear precalculated send enable bit in NIC control register
	 */

	NIC.clear.full = mcmsg_hw.send_enable.full;
	mcmsg_hw.send_int_enable = 0;
}


/*
 * Enable the transmit interrupts.
 * (In uniprocessor mode will actually be enabled when SPL drops)
 */

void
mcmsg_enable_tx_interrupts()
{

	/*
	 * Set precalculated send enable bit in NIC control register
	 */

	NIC.set.full = mcmsg_hw.send_enable.full;
	mcmsg_hw.send_int_enable = 1;
}

/*
 * NIC interrupt routine
 *
 * Routine:
 *	nic_interrupt(regs)
 *
 * Arguments:
 *	regs:	Points to saved registers (not used)
 *
 * Purpose:
 *	Called from interrupt.c when there is an interrupt from the NIC.
 *	Used in uniprocessor or symmetric mode, not with the message processor.
 */

int nic_interrupt(regs)
{
	register int	s;
	extern int was_nic_interrupt;

	s = splmsg_noi();
	baton_enter();
	assert(mcmsg_reentry++ == 0);
	CHECK_REENTRY("entry interrupt");
	RED_ON(RED_MSG);
	was_nic_interrupt = 1;
	s += mcmsg_intr();
	CHECK_REENTRY("exit  interrupt");
	assert(--mcmsg_reentry == 0);
	RED_OFF(RED_MSG);
	baton_exit();
	return s;
}

/*
 * Routine:
 *	nic_interrupt_poll()
 *
 * Purpose:
 *	Called from console code to poll for NIC input while waiting to
 *	send or receive a character.
 *	Used in uniprocessor or symmetric mode, not with the message processor.
 */

nic_interrupt_poll()
{
	register int	s;

	s = splmsg();
	assert(mcmsg_reentry++ == 0);
	CHECK_REENTRY("entry poll");
	RED_ON(RED_MSG);
	mcmsg_intr();
	CHECK_REENTRY("exit  poll");
	assert(--mcmsg_reentry == 0);
	RED_OFF(RED_MSG);
	splx(s);
}

/*
 * Routine:
 *	mcmsg_intr()
 *
 * Purpose:
 *	Handle NIC interrupt.
 */

int
mcmsg_intr()
{
	int come_again = 0;

	/*
	 * Process receive interrupts.
	 */

	if (mcmsg_recv_ready()) {
		register unsigned long	hdr1;
		register unsigned long	hdr2;
		register unsigned long	method;

		/*
		 * Read the first two header words from the NIC
		 */
		recv2_errchk(hdr1, hdr2);

		/*
		 * Switch on the packet type and dispatch receive procedure
		 */

		method = hdr1 & 0xFFFF;
		if (method < MCTRL_END) {

#if	BUMPERS
			mcmsg_hw.recv_status = RECV_INTR_MODE_1;
#endif	BUMPERS

			/*
			 * Call the receive method.
			 */
			mcmsg_recv_switch[method](hdr1, hdr2);
#if	BUMPERS
			/*
			 * switch receive interrupt mode
			 */
			mcmsg_set_bumper();
#endif	BUMPERS

#if	MACH_ASSERT
			/*
			 * Check that the receive procedure took exactly
			 * the whole packet.
			 * In case the condition is not true, retry to give
			 * the end of the bumper time to arrive from Sunmos
			 * into the NIC.
			 */
			if(!(mcmsg_eod_last()))
				assert(mcmsg_eod_last());
#endif	MACH_ASSERT

		} else {
			mcmsg_recv_sw_err(hdr1, hdr2);
		}
#if	NIC_TRACE
		mcmsg_nic_status(6);
#endif	NIC_TRACE

		/*
		 * Cause interrupt.c to poll briefly for another interrupt.
		 * Uniprocessor only.
		 */

		come_again++;
	}

	/*
	 * Process send interrupts.
	 */

	if (mcmsg_hw.send_int_enable && mcmsg_send_ready()) {
		register unsigned char	new_out;
		register mcmsg_send_store_t *sp;

		/*
		 * Take sends from send store
		 */

		new_out = mcmsg_send_store_out;
		assert(new_out != mcmsg_send_store_in);

		sp = &mcmsg_send_store[new_out];

		MCMSG_TRACE_STORE("intr send", 1, new_out, 0, 0, 0);

		sp->routine(
					sp->mcmsg_task,
					sp->routine,
					sp->a[0],
					sp->a[1],
					sp->a[2],
					sp->a[3],
					sp->a[4],
					sp->a[5],
					sp->a[6],
					sp->a[7],
					sp->a[8],
					sp->a[9]);

		MCMSG_TRACE_STORE("intr deq send", 1, new_out, 0, 0, 0);

		new_out = new_out + 1;
		if (new_out == mcmsg_send_store_in) {

			/*
			 * Reset indices to 0 to avoid walking
			 * through all msg processor cache
			 */

			mcmsg_send_store_in = 0;
			mcmsg_send_store_out = 0;

			/*
			 * Nothing left to send,
			 * disable send interrupts
			 */

			mcmsg_disable_tx_interrupts();

		} else {

			/*
			 * Step to next send
			 */

			mcmsg_send_store_out = new_out;

			/*
			 * Cause interrupt.c to poll briefly
			 * for another interrupt.
			 * Uniprocessor only.
			 */

			come_again++;
		}
	}

	/*
	 * Simulate a console interrupt if needed
	 */

	if (mcmsg_console_need_int) {
		mcmsg_console_ints();
	}
	return (come_again ? NSPL : 0);
}


/*
 * Routine:
 *	mcmsg_send(mt, method, args...)
 *
 * Arguments:
 *	mt:	Mcmsg_task pointer
 *	method:		Send method (packet type)
 *	args:		Up to 10 arguments to send procedure
 *
 * Purpose:
 *	Send a packet unless network is busy.
 *	If its busy save the send request.
 *	The network is busy if there are already saved requests
 *	or if there is no room in the transmit FIFO.
 *
 *	This is critical code for two reasons,
 *	 because it is in every packet send path, and
 *	 because it does tail recursion that could overflow the stack.
 *	Since the compiler has not shown an ability to detect tail
 *	recursion properly here it is important to use the assembly
 *	language version for production.
 */

/*
 * The assembly language version
 *
 * WARNING: This mcmsg_send_tail requires compilation with -Mframe
 */

#if 1
/* See mcmsg_dispatch.s */
#else
/* Obsolete, passes method to mcmsg_save_send XXX */
mcmsg_send_holder()
{
asm("NICstat	=	0xFFFF0018			// NIC status	");
asm("NICrt	=	0xFFFF0038			// NIC reset/test");
asm("TX_ALMOST	=	0x4				// Xmit FIFO empty");
asm("TX_EMPTY	=	0x8				// Xmit FIFO empty");
asm("TX_STOP	=	0x11				// Stop xmit FIFO");
asm("_mcmsg_send::					//		");
asm("	orh	ha%_mcmsg_send_store_out, r0, r31	// Load ...out	");
asm("	ld.b	l%_mcmsg_send_store_out(r31), r28	// into r28	");
asm("	orh	ha%_mcmsg_send_store_in, r0, r31	// Load ...in	");
asm("	ld.b	l%_mcmsg_send_store_in(r31), r30	// into r30	");
asm("	btne	r28, r30, _mcmsg_save_send		// Exit if ne	");
asm("	orh	ha%NICstat, r0, r31			// Get NIC stat	");
asm("	fld.d	l%NICstat(r31), f16			// thru f16	");
asm("	fxfr	f16, r30				// in r30	");
#if	BURST
asm("	and	TX_EMPTY, r30, r0			// Check TX_EMPTY");
asm("	bc	_mcmsg_save_send			// Exit if not	");
asm("	adds	TX_STOP, r0, r30			// Stop xmit	");
asm("	ixfr	r30, f18				//  FIFO	");
asm("	orh	ha%NICrt, r0, r31			//		");
asm("	fst.d	f18, l%NICrt(r31)			//		");
#else	BURST
asm("	and	TX_ALMOST, r30, r0			// Check TX_ALMOST");
asm("	bc	_mcmsg_save_send			// Exit if not	");
#endif	BURST
asm("	orh	h%_mcmsg_send_switch, r0, r31		// Jump thru	");
asm("	or	l%_mcmsg_send_switch, r31, r30		// send switch	");
asm("	shl	2, r17, r29				// using method	");
asm("	ld.l	r30(r29), r30				//		");
asm("	bri	r30					//		");
asm("	nop						//		");
asm("							//		");
asm("_mcmsg_send_tail::					//		");
asm("	ld.l	4(fp), r30				// Get old r1	");
asm("	orh	h%_mcmsg_send_continue, r0, r31		// Replace it	");
asm("	or	l%_mcmsg_send_continue, r31, r31	//  -> below	");
asm("	bri	r1					// Exec ret code");
asm("	 st.l	r31, 4(fp)				//		");
asm("_mcmsg_send_continue::				//		");
asm("	br	_mcmsg_send				// Go mcmsg_send");
asm("	 or	r30, r0, r1				// with old r1	");
}
#endif



/*
 * Routine:
 *	mcmsg_save_send(mt, routine, args...)
 *
 * Arguments:
 *	mt:	Mcmsg_task pointer
 *	routine:	Send procedure
 *	args:		Up to 10 arguments to send procedure
 *
 * Purpose:
 *	Save a packet send request in mcmsg_send_store
 */

unsigned long mcmsg_send_stored;
unsigned long mcmsg_even_count;

mcmsg_save_send(mt, routine, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)
	mcmsg_task_t	*mt;
	int		(*routine)();
	unsigned long	a0, a1, a2, a3, a4, a5, a6, a7, a8, a9;
{
	register unsigned char	in;
	register unsigned char	new_in;
	register long		t;
	register mcmsg_send_store_t *sp;

	in = mcmsg_send_store_in;
	new_in = in + 1;
	if (new_in == mcmsg_send_store_out) {

		/*
		 * Send store is full, force one out.
		 */

		sp = &mcmsg_send_store[new_in];
		MCMSG_TRACE_STORE("send from q", 1, new_in, 0, 0, 0);

		assert((t = 1000000) != 0);
		while (!mcmsg_send_ready()) {
			assert(t-- != 0);
		}
		sp->routine(
				sp->mcmsg_task,
				sp->routine,
				sp->a[0],
				sp->a[1],
				sp->a[2],
				sp->a[3],
				sp->a[4],
				sp->a[5],
				sp->a[6],
				sp->a[7],
				sp->a[8],
				sp->a[9]);

		MCMSG_TRACE_STORE("deq send", 1, new_in, 0, 0, 0);
		mcmsg_send_store_out++;
	}

	/*
	 * Store the send.
	 */

	sp = &mcmsg_send_store[in];
	sp->mcmsg_task = mt;
	sp->routine = routine;
	sp->a[0] = a0;
	sp->a[1] = a1;
	sp->a[2] = a2;
	sp->a[3] = a3;
	sp->a[4] = a4;
	sp->a[5] = a5;
	sp->a[6] = a6;
	sp->a[7] = a7;
	sp->a[8] = a8;
	sp->a[9] = a9;

	MCMSG_TRACE_STORE("queue send", 4, routine, a0, a1, in);

	mcmsg_send_store_in = new_in;
	mcmsg_enable_tx_interrupts();
	mcmsg_send_stored++;
	return;
}

#if NX

/*
 * Routine:
 *      mcmsg_clear_force_sends(pid_si, sequence)
 *
 * Argumens:
 *      pid_si:         Correspondent identification
 *      sequence:       Message identification
 *
 * Purpose:
 *      Search stored sends for the specified NX force-type message
 *      and cancel it by changing the send routine to mcmsg_send_sw_null().
 */

select_item_t  *
mcmsg_clear_force_sends(pid_si, sequence)
        register select_item_t  *pid_si;
        register unsigned long  sequence;
{
        register select_item_t  *st;
        register int           (*routine)();
        register unsigned long  t;
        register unsigned char  s_out;

        s_out = mcmsg_send_store_out;
        assert((t = MAXLOOP) != 0);
        while (s_out != mcmsg_send_store_in) {
                routine = mcmsg_send_store[s_out].routine;
                if (routine == mcmsg_send_switch[MCTRL_NXS] ||
                    routine == mcmsg_send_switch[MCTRL_NXM] ||
                    routine == mcmsg_send_switch[MCTRL_NX1] ||
                    routine == mcmsg_send_switch[MCTRL_NXN]) {
                        st = (select_item_t *)mcmsg_send_store[s_out].a[0];
                        assert(st != 0);
                        if (st->nxrq.pid_si == pid_si &&
                            st->nxrq.sequence == sequence) {
                                mcmsg_send_store[s_out].routine =
                                        mcmsg_send_sw_null;
                                return st;
                        }
                }
                s_out++;
                assert(t-- != 0);
        }
		return (select_item_t *)0;
}

/*
 * Routine:
 *	mcmsg_release_send_store(pid_si, sequence)
 *
 * Argumens:
 *	pid_si:		Correspondent identification
 *	sequence:	Message identification
 *
 * Purpose:
 *	Search stored sends for the specified NX message
 *	and release it by changing the stop point to count.
 */


mcmsg_release_send_store(pid_si, sequence)
	register select_item_t	*pid_si;
	register unsigned long	sequence;
{
	register select_item_t	*st;
	int			(*routine)();
	register unsigned long	t;
	register unsigned char	s_out;
	extern mcmsg_send_nxs();
	extern mcmsg_send_nxm();
	extern mcmsg_send_nx1();
	extern mcmsg_send_nxn();

	s_out = mcmsg_send_store_out;
	assert((t = MAXLOOP) != 0);
	while (s_out != mcmsg_send_store_in) {
		routine = mcmsg_send_store[s_out].routine;
		if (routine == mcmsg_send_nxs ||
		    routine == mcmsg_send_nxm ||
		    routine == mcmsg_send_nx1 ||
		    routine == mcmsg_send_nxn) {
			st = (select_item_t *)mcmsg_send_store[s_out].a[0];
			assert(st != 0);
			if (st->nxrq.pid_si == pid_si &&
			    st->nxrq.sequence == sequence) {
				st->nxrq.stop = st->nxrq.count;
				mcmsg_send_store[s_out].routine = mcmsg_send_nxn;
				mcmsg_send_store[s_out].a[1] =
					(unsigned long)pid_si;
				mcmsg_send_store[s_out].a[2] = 
					st->nxrq.buf + st->nxrq.offset,
				mcmsg_send_store[s_out].a[3] = 
					st->nxrq.count - st->nxrq.offset,
				mcmsg_send_store[s_out].a[4] = 
					st->mcmsg_task->applinfo.pkt_size,
				mcmsg_send_store[s_out].a[5] = 
					st->nxrq.offset,
				mcmsg_send_store[s_out].a[6] = 
					st->nxrq.stop;
				mcmsg_trace_debug("rkr send", 1, st, 0, 0, 0);
				return 1;
			}
		}
		s_out++;
		assert(t-- != 0);
	}
	return 0;
}

/*
 * Routine:
 *	mcmsg_remove_send_store_task(mcmsg_task)
 *
 * Argumens:
 *  *mcmsg_task      mcmsg_task_t pointer to mcmsg_task structure
 *
 * Purpose:
 *  remove all objects from send_store that pertain to this mcmsg_task
 *  structure.
 */


mcmsg_remove_send_store_task(mt)
mcmsg_task_t    *mt;
{
	register unsigned char	s_out;
	register unsigned char	s1_out;

	s_out = mcmsg_send_store_out;
	while (s_out != mcmsg_send_store_in) {
		if(mcmsg_send_store[s_out].mcmsg_task == mt) {
			s1_out = s_out;
			while (s1_out != mcmsg_send_store_in) {
				mcmsg_send_store[s1_out] = mcmsg_send_store[++s1_out];
			}
			mcmsg_send_store_in--;
			s_out--;  /* need to check current again; did we move one in */
		}
		if(s_out == mcmsg_send_store_in) {
			break;
		}
		s_out++;
	}
	if(mcmsg_send_store_out == mcmsg_send_store_in) { /* nothing left to
														 send to; clear */
		mcmsg_disable_tx_interrupts();				  /* transmit interrupts */
	}
	return 0;
}

#endif NX

#include <ddb/db_command.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>
#include <machine/db_machdep.h>

extern	void mcmsg_printsym(db_expr_t, db_strategy_t);

void
mcmsg_db_sendstore(addr, have_addr, count, modif)
	unsigned long	addr;
	int		have_addr;
	int		count;
	char		*modif;
{
	int msg_ss_in;
	int msg_ss_out;
	int i,j,k;

	msg_ss_in = (int)(mcmsg_send_store_in);
	msg_ss_out = (int)(mcmsg_send_store_out);

	db_printf("mcmsg_send_store %x in %x, out %x\n",
		mcmsg_send_store, msg_ss_in,msg_ss_out);
	if ((i = msg_ss_out - 5) < 0) i = 0;
	for(; (i < MAXSEND) & (i < msg_ss_in + 5); i++) {
	    if (i == msg_ss_in) {
	        db_printf(" in -> ");
	    } else {
	        db_printf("       ");
	    }
	    if (i == msg_ss_out) {
	        db_printf("out -> ");
	    } else {
	        db_printf("       ");
	    }
	    for(j = 0; j < METHOD_MAX; j++) {
	        if ((int *)mcmsg_send_store[i].routine == 
			      (int *)mcmsg_send_switch[j]) {
	    	    db_printf("%s ",control_name[j]);
	            mcmsg_printsym((db_expr_t)mcmsg_send_switch[j],
			DB_STGY_ANY);
	            db_printf("(");
	            db_printf("%x",mcmsg_send_store[i].a[0]);
	            for(k = 1; k < 4; k++) {
	                db_printf(", %x",mcmsg_send_store[i].a[k]);
	            }
	            db_printf(")\n");
	            break;

	        }
	    }
	}
}

