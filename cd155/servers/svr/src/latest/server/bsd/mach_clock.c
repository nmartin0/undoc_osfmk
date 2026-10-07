/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: mach_clock.c,v $
 * Revision 1.11  1995/02/24  22:28:00  stans
 *  lint picking
 *
 *  Reviewer:self
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW07 sats
 *
 * Revision 1.10  1994/11/30  22:56:18  jlitvin
 * Don't use a local #define of DEBUG for this file.
 *
 *  Reviewer: suri
 *  Risk: low
 *  Benefit or PTS #: 11745
 *  Testing: developer
 *  Module(s): server/bsd/mach_clock.c
 *
 * Revision 1.9  1994/11/18  20:27:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/06/17  23:59:44  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.7  1994/01/13  17:52:53  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.6  1993/07/14  17:48:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:48:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:04:16  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:24:41  cfj
 * Initial 1.0.3 code drop
 * Revision 1.1.2.1.2.1  1992/12/16  05:58:46  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:54:40  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:16:20  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:06:36  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.8  92/11/11  17:26:21  condict
 * 	Change explicit calls to cthread_fork into ux_thread_bootstrap,
 * 	for better layering of modules.
 * 	[92/11/11            condict]
 * 
 * Revision 2.10  1993/04/08  11:20:00  loverso
 * 	Revision 3.10  92/04/22  16:48:14  barbou
 * 	Fix for bug #136: prevent null or negative timeouts when converting from
 * 	timeval to hz.
 *
 * 	ux server threads are wired by default. (loverso)
 *
 * Revision 2.9  93/01/07  11:16:56  condict
 * 	New profiling design means we don't need to check for a change in
 * 	profiling status here or have a special version of cthread_fork.
 * 	[1992/06/10  17:29:04  emcmanus]
 * 
 * Revision 2.8  92/11/11  17:26:21  condict
 * 	Change explicit calls to cthread_fork into ux_thread_bootstrap,
 * 	for better layering of modules.
 * 	[92/11/11            condict]
 * 
 * Revision 2.7  92/05/31  18:58:18  loverso
 * 	afterwards. Needs to include user.h to get unix_master (pjg).
 * 
 * Revision 2.6  92/05/24  14:16:52  pjg
 * 	92/03/13  15:18:32  condict
 * 	Change name of callout_init to softclock_init, as in integrated kernel.
 * 	[92/05/18            srl]
 * 
 * Revision 2.5  92/03/09  14:24:30  durriya
 * 	[Revision 3.8  92/02/25  17:48:04  condict]
 * 	Change all calls to cthread_wire to ux_thread_wire, so ux_server_loop
 * 	can correctly compute required number of Mach kernel threads.
 * 
 * 	[Revision 3.7  92/02/12  10:58:17  bernadat]
 * 	Fixed mach_msg timeout argument in softclock thread
 * 
 * Revision 2.4  91/12/17  13:38:35  roy
 * 	91/12/09  18:49:22  emcmanus
 * 	Divert cthread_fork to gprof_cthread_fork if profiling the server.
 * 	This is normally done by user.h, but we don't include that here.
 * 	Check for change in profiling status every time we wake up.
 * 
 * 	91/10/23  16:37:57  condict
 * 	Replace get_time calls with use of the global time var and TIME LOCKS.
 * 
 * 	91/10/17  15:44:40  condict
 * 	Get def of struct callout from callout.h, now, like integrated OSF/1.
 * 	Also, moved definition of hz and tick to conf/param.c for same reason.
 * 
 * Revision 2.3  91/10/04  14:48:14  chrisp
 * Get rid of reference to msgh_kind field (it is now a sequence number).
 * 
 * Revision 2.2  91/08/31  13:22:33  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/06/28  14:13:46  sp
 * Make callout_zone pageable
 * 
 * Revision 3.2  91/06/27  15:48:06  sp
 * use OSF/1 zinit/zchange interfaces
 * 
 * Revision 3.1  91/06/25  16:58:44  condict
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.0  91/01/17  12:11:32  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.5  90/08/06  15:31:25  rwd
 * 	Add Statistics code for the callout queue.
 * 	[90/06/08            rwd]
 * 
 * Revision 2.4  90/06/02  15:21:30  rpd
 * 	Updated set_thread_priority call for the new priority range.
 * 	[90/04/23            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  19:35:09  rpd]
 * 
 * Revision 2.3  89/12/08  20:14:22  rwd
 * 	Fic deallocation in untimeout_special
 * 	[89/11/29            rwd]
 * 	Change callout structures to a zone.  Add special
 * 	timeout/untimeout
 * 	[89/11/01            rwd]
 * 
 * 	Call ux_thread_wire before setting thread priority.
 * 	[89/11/01            rwd]
 * 
 * Revision 2.2  89/10/17  11:25:04  rwd
 * 	Make hzto return 0 if time has already passed.
 * 	[89/09/22            dbg]
 * 
 */
/*
 * Out-of-kernel replacements for timeout.
 */

#include <uxkern/import_mach.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/kernel.h>
#include <sys/user.h>
#include <kern/parallel.h>
#include <sys/time.h>
#include <sys/synch.h>
#include <kern/zalloc.h>
#include <sys/callout.h>
#include <sys/dk.h>

#define CALLOUT_SIZE sizeof(struct callout)

zone_t		callout_zone;		/* callout zone */
struct mutex	callout_lock = MUTEX_INITIALIZER;
struct callout	calltodo = {&calltodo, &calltodo, 0, 0, 0, 0};
struct callout	*callout;

mach_port_t	softclock_port;
cthread_t	softclock_thread;
extern cthread_t ux_create_thread();

mach_msg_header_t poke_softclock_msg;

void	softclock();	/* forward */

#ifdef DEBUG
boolean_t	softclock_debug = FALSE;
boolean_t	callout_debug = FALSE;
struct callout_info {
	struct mutex	ci_mutex;
	int		ci_size;
	int		ci_inserts;
	int		ci_deletes;
	int		ci_zeros;
	int		ci_depth[16];
	int		ci_max;
} callout_info;
#endif DEBUG

#if GPROF
extern int profiling, oldprofiling;
#endif

softclock_init()
{
	callout_zone = zinit(CALLOUT_SIZE,1000*CALLOUT_SIZE,10*CALLOUT_SIZE,
			     "Callout zone");

	(void) mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
				  &softclock_port);

	(void) mach_port_insert_right(mach_task_self(),
				      softclock_port, softclock_port,
				      MACH_MSG_TYPE_MAKE_SEND);

	softclock_thread = ux_create_thread((any_t)softclock);

	/*
	 * Set up message used to poke softclock thread - it
	 * never changes.
	 */

	poke_softclock_msg.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
	poke_softclock_msg.msgh_size = 0;
	poke_softclock_msg.msgh_remote_port = softclock_port;
	poke_softclock_msg.msgh_local_port = MACH_PORT_NULL;
	poke_softclock_msg.msgh_id = 0;
#ifdef	DEBUG
	if (callout_debug) {
	    bzero(&callout_info, sizeof(callout_info));
	    mutex_init(&callout_info.ci_mutex);
	}
#endif	DEBUG
}

timeout(func, arg, t)
	int	(*func)();
	caddr_t	arg;
	int	t;
{
	(void) timeout_special(func, arg, t, 0);
}

/*
 * Arrange that (*func)(arg) is called in t/hz seconds.
 */
int timeout_special(func, arg, t, s)
	int	(*func)();
	caddr_t	arg;
	int	t;
	int	s;
{
	register callout_t cp, c;
	boolean_t	reset;
	struct timeval	timeout_time, cur_time;
#ifdef	DEBUG
	int i = 0;
#endif	DEBUG

	if (t <= 0)
		t = 1;

	/*
	 * Convert timeout to absolute time.
	 */
	TIME_READ_LOCK();
	cur_time = time;
	TIME_READ_UNLOCK();
	timeout_time = cur_time;
	timeout_time.tv_sec += t / hz;
	timeout_time.tv_usec += (t % hz) * tick;
	timevalfix(&timeout_time);
#ifdef DEBUG
if (softclock_debug)
  printf("At (%d,%d), timeout %d (msec) becomes time (%d,%d)\n",
    cur_time.tv_sec, cur_time.tv_usec,
    (t*1000)/hz, timeout_time.tv_sec, timeout_time.tv_usec);
#endif DEBUG

	mutex_lock(&callout_lock);

	c = (callout_t) zalloc(callout_zone);

	c->c_arg = arg;
	c->c_func = func;
	c->c_time = timeout_time;
	c->c_special = s;

	/*
	 * Insert the element at the correct place in the queue.
	 */
	for (cp = calltodo.c_next; cp != &calltodo; cp = cp->c_next) {
#ifdef	DEBUG
	    i++;
#endif	DEBUG

	    if (timercmp(&cp->c_time, &timeout_time, >)) {
		break;
	    }
	}
#ifdef	DEBUG
	if (callout_debug) {
		register int j;
		mutex_lock(&callout_info.ci_mutex);
		callout_info.ci_inserts++;
		callout_info.ci_size++;
		if (callout_info.ci_size > callout_info.ci_max)
		    callout_info.ci_max = callout_info.ci_size;
		j = i/16;
		if (j > 15) j = 15;
		callout_info.ci_depth[j]++;
		mutex_unlock(&callout_info.ci_mutex);
	}
#endif	DEBUG

	/*
	 * Insert new callout item
	 */
	c->c_next = cp;
	c->c_prev = cp->c_prev;
	cp->c_prev = c;
	(c->c_prev)->c_next = c;

	/*
	 * If the new callout item is now the first in the chain,
	 * poke the softclock thread to reset its timeout.
	 */
	reset = (calltodo.c_next == c);
	mutex_unlock(&callout_lock);

	if (reset && cthread_self() != softclock_thread) {
#ifdef DEBUG
if (softclock_debug)
  printf("        Resetting softclock\n");
#endif DEBUG
            (void) mach_msg(&poke_softclock_msg,
			    MACH_SEND_MSG|MACH_SEND_TIMEOUT,
			    sizeof poke_softclock_msg, 0, MACH_PORT_NULL,
			    0, MACH_PORT_NULL);
	}
	return (int) c;
}

untimeout_special(i)
	int i;
{
	register callout_t c = (callout_t)i;

	mutex_lock(&callout_lock);
	if (c->c_next != (callout_t)0) {
		if (calltodo.c_next == c) {
			c->c_func = (int (*)())0;
		} else {
			(c->c_prev)->c_next = c->c_next;
			(c->c_next)->c_prev = c->c_prev;
#ifdef	DEBUG
			if (callout_debug) {
				mutex_lock(&callout_info.ci_mutex);
				callout_info.ci_size--;
				mutex_unlock(&callout_info.ci_mutex);
			}
#endif	DEBUG
			zfree(callout_zone, c);
		}
	} else {
		zfree(callout_zone, c);
	}
	mutex_unlock(&callout_lock);
}

/*
 * untimeout is called to remove a function timeout call
 * from the callout structure.
 */
untimeout(func, arg)
	int	(*func)();
	caddr_t	arg;
{
	register callout_t	c;

	mutex_lock(&callout_lock);

	for (c = calltodo.c_next; c != &calltodo; c = c->c_next) {

	    if (c->c_func == func && c->c_arg == arg) {
		/*
		 * Found callout.
		 */
		if (calltodo.c_next == c) {
		    /*
		     * If we remove the first element on the chain,
		     * the softclock thread will give its timeout to
		     * the next element (firing it too early).
		     * Instead, clear the function pointer so that
		     * it will not be executed.
		     */
		    c->c_func = (int (*)())0;
		}
		else {
		    /*
		     * Not the first element - remove it from the list.
		     */
		    (c->c_prev)->c_next = c->c_next;
		    (c->c_next)->c_prev = c->c_prev;
#ifdef	DEBUG
		    if (callout_debug) {
			mutex_lock(&callout_info.ci_mutex);
			callout_info.ci_size--;
			callout_info.ci_deletes++;
			mutex_unlock(&callout_info.ci_mutex);
		    }
#endif	DEBUG
		    zfree(callout_zone, c);
		}
		break;
	    }
	}

	mutex_unlock(&callout_lock);
}

int
time_to_hz(tv)
	struct timeval *tv;
{
	register long	ticks, sec;

	/*
	 * If number of milliseconds will fit in 32 bit arithmetic,
	 * then compute number of milliseconds to time and scale to
	 * ticks.  Otherwise just compute number of hz in time, rounding
	 * times greater than representible to maximum value.
	 *
	 * Delta times less than 25 days can be computed ``exactly''.
	 * Maximum value for any timeout in 10ms ticks is 250 days.
	 */
	sec = tv->tv_sec;
	if (sec <= 0x7fffffff / 1000 - 1000) {
		ticks = (tv->tv_sec * 1000 +
			 tv->tv_usec / 1000) / (tick / 1000);
		if (ticks <= 0) ticks = 1;
	} else if (sec <= 0x7fffffff / hz)
		ticks = sec * hz;
	else
		ticks = 0x7fffffff;

	return (ticks);
}

time_to_ms(tv)
	struct timeval *tv;
{
	register long	ms, sec;

	/*
	 * If number of milliseconds will fit in 32 bit arithmetic,
	 * then compute number of milliseconds.
	 * Otherwise just compute number of ms in time, rounding
	 * times greater than representible to maximum value.
	 *
	 */
	sec = tv->tv_sec;
	if (sec <= 0x7fffffff / 1000 - 1000)
		ms = (tv->tv_sec * 1000 +
			 tv->tv_usec / 1000);
	else if (sec <= 0x7fffffff / 1000)
		ms = sec * 1000;
	else
		ms = 0x7fffffff;

	return (ms);
}

/*
 * Compute number of hz until specified time.
 * Used to compute third argument to timeout() from an
 * absolute time.
 */
hzto(tv)
	struct timeval *tv;
{
	struct timeval desired_time;

	desired_time = *tv;
	TIME_READ_LOCK();
	if (!timercmp(&time, &desired_time, <)) {
		TIME_READ_UNLOCK();
		return (0);
	}
	timevalsub(&desired_time, &time);
	TIME_READ_UNLOCK();
	return (time_to_hz(&desired_time));
}

/*
 * Softclock thread.
 */
/*ARGSUSED*/
void
softclock(th_arg)
	any_t	th_arg;
{
	caddr_t	arg;
	int	t;
	int	(*func)();
	register callout_t c;
	struct timeval timeout_time, cur_time;
	mach_msg_header_t m;

	cthread_set_name(cthread_self(), "softclock");

	/*
	 * Make this thread high priority.
	 */
	set_thread_priority(mach_thread_self(), 1);

	while (TRUE) {
	    /*
	     * Set the timeout from the first element in the
	     * callout list that has not been removed.
	     */
	    mutex_lock(&callout_lock);
	    while ((c = calltodo.c_next) == &calltodo || c->c_func == 0) {
		if (c == &calltodo) {
		    /*
		     * No timeout - wait to be poked by timeout().
		     */
		    mutex_unlock(&callout_lock);

		    (void) mach_msg(&m, MACH_RCV_MSG,
				    0, sizeof m, softclock_port,
				    MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

		    mutex_lock(&callout_lock);
		}
		else {
		    /*
		     * Cleared timeout element.
		     */
		    calltodo.c_next = c->c_next;
		    (c->c_next)->c_prev = &calltodo;
#ifdef	DEBUG
		    if (callout_debug) {
			mutex_lock(&callout_info.ci_mutex);
			callout_info.ci_zeros++;
			callout_info.ci_size--;
			mutex_unlock(&callout_info.ci_mutex);
		    }
#endif	DEBUG
		    zfree(callout_zone, c);
		}
	    }
	    timeout_time = c->c_time;
	    mutex_unlock(&callout_lock);

	    /*
	     * Compute interval to wait.
	     */
	    TIME_READ_LOCK();
	    cur_time = time;
	    TIME_READ_UNLOCK();
#ifdef DEBUG
if (softclock_debug)
  printf("    At (%d,%d), timeout time (%d,%d)",
    cur_time.tv_sec, cur_time.tv_usec,
    timeout_time.tv_sec, timeout_time.tv_usec);
#endif DEBUG
	    if (!timercmp(&cur_time, &timeout_time, <))
		t = 0;
	    else {
		timevalsub(&timeout_time, &cur_time);
		t = time_to_ms(&timeout_time);
#ifdef DEBUG
if (softclock_debug)
  printf(" becomes interval %d msec\n", t);
#endif DEBUG
	    }

	    if (t > 0) {
		/*
		 * Wait for timeout to go off.  If we receive
		 * a message instead, the timeout queue has
		 * been altered and we must reset the timeout.
		 */

		if (mach_msg(&m, MACH_RCV_MSG|MACH_RCV_TIMEOUT,
			     0, sizeof m, softclock_port,
			     t, MACH_PORT_NULL) != MACH_RCV_TIMED_OUT)
		    continue;
	    }

	    /*
	     * Remove the timeout from the queue and call the
	     * function (unless it has been reset).
	     */
#ifdef DEBUG
if (softclock_debug)
  printf("........Softclock timed out\n");
#endif DEBUG
	    mutex_lock(&callout_lock);

	    c = calltodo.c_next;

	    calltodo.c_next = c->c_next;
	    (c->c_next)->c_prev = &calltodo;

	    arg  = c->c_arg;
	    func = c->c_func;

#ifdef	DEBUG
	    if (callout_debug) {
		mutex_lock(&callout_info.ci_mutex);
		callout_info.ci_size--;
		mutex_unlock(&callout_info.ci_mutex);
	    }
#endif	DEBUG
	    if (c->c_special && c->c_func) {
		c->c_next = (callout_t)0;
	    } else {
		zfree(callout_zone, c);
	    }

	    mutex_unlock(&callout_lock);

	    /*
	     * Skip this item if it was cleared before we had
	     * a chance to notice.
	     */
	    if (func != (int (*)())0) {
		interrupt_enter(SPLSOFTCLOCK);
#ifdef	OSF1_SERVER
		unix_master();
#endif
		(*func)(arg);
#ifdef	OSF1_SERVER
		unix_release();
#endif
		interrupt_exit(SPLSOFTCLOCK);
	    }
	}
}
