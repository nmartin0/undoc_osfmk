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
 * @OSF_COPYRIGHT@
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: netisr.c,v $
 * Revision 1.8  1995/02/01  21:33:46  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.7  1994/11/18  20:33:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  18:08:14  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/09  15:04:29  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.1.1.3  1993/07/01  19:28:28  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  18:59:51  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/03  03:06:26  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:00:22  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:57:29  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:26:54  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:31  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:48  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.12  93/07/07  12:50:49  mjl
 * Fix typo.
 * 
 * Revision 2.11  93/07/07  10:32:41  mjl
 * [LCC #0314] Add code to support deferred start up of network threads.
 * 
 * Revision 2.10  1993/04/08  11:30:30  loverso
 * 	ux server threads are wired by default. (loverso)
 *
 * Revision 2.9  93/04/05  18:05:38  durriya
 * 	start netisr on all nodes since all fileserver nodes can run the 
 * 	     filesystem side of the NFS server code. 
 * 	change name of netisr_init() to netinput_init(). Initialise 
 * 	     netisr_is_init() in netinit()
 * 	[93/04/05            durriya]
 * 
 * Revision 2.8  93/03/25  14:42:20  durriya
 * 	pass fullnode to domaininit (durriya)
 * 
 * Revision 2.7  93/03/24  15:38:09  loverso
 * 	netinit() now takes an argument identifying if this is a full network
 * 	node or not.  This is to fix an uninitialized lock, as the old code
 * 	was not running domain_init() on AD non-TNC fileservers.  (loverso)
 * 
 * Revision 2.6  92/11/11  17:27:21  condict
 * 	Change cthread_wire to ux_thread_wire, as required by
 * 	ux_server_loop.c for proper server thread management.
 * 	[92/11/11            condict]
 * 
 * Revision 2.5  92/05/24  14:34:25  pjg
 * 	92/03/31  15:40:40  emcmanus
 * 	Added name parameter to net_threadstart to set the cthread's name,
 * 	if compiling with NETISR_THREAD.
 * 	[92/05/18            srl]
 * 
 * Revision 2.4  92/05/01  10:30:24  rabii
 * 	Added mi driver initialization.(bhk)
 * 
 * Revision 2.3  92/03/09  14:41:33  durriya
 * 	92/01/07  23:34:28  condict
 * 	Set netisr flag to 0 when Netintr processes a scheduled softnet interrupt.
 * 
 * 	91/12/18  17:16:34  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.2  91/08/31  13:40:23  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:33:35  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.16.6.2  91/03/15  17:47:57  tmt
 * 	Change type of netisr_thread to int.
 * 	[91/03/13  19:05:59  tmt]
 * 
 * Revision 1.16  90/10/07  14:32:22  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:11:00  gm]
 * 
 * 	Look at DYNAMIC token before configuring subsystem.
 * 	[90/09/29  17:14:36  tmt]
 * 
 * 	Added lock debugging code.
 * 	[90/09/28  12:38:33  nags]
 * 
 * Revision 1.15  90/09/23  15:55:30  devrcs
 * 	Add inet, ns configuration parameters.
 * 	[90/09/15  15:34:29  tmt]
 * 
 * 	Use netthreads.h to decide number of isr threads.
 * 	[90/09/11  11:40:28  tmt]
 * 
 * Revision 1.14  90/09/13  11:48:01  devrcs
 * 	Fix configuration tests.
 * 	[90/08/28  11:21:43  tmt]
 * 
 * Revision 1.13  90/08/24  12:13:48  devrcs
 * 	Test UIPC/INET/NS/NSL options before configuring.
 * 	[90/08/19  14:16:41  tmt]
 * 
 * Revision 1.12  90/07/27  08:59:20  devrcs
 * 	Checkpoint at Reno merge.
 * 	Add netisr_af() for loopback.
 * 	[90/07/19  22:05:28  tmt]
 * 
 * Revision 1.11  90/07/05  23:12:36  devrcs
 * 	Uniprocessor compatibility using DOMAIN_FUNNEL().
 * 	Add softnet->active to protect domain.
 * 	[90/07/03  18:46:47  tmt]
 * 
 * Revision 1.10  90/06/22  20:38:38  devrcs
 * 	Do pullups on wildcard isr's to protect headers.
 * 	Add silly code to allow MACH_LDEBUG to not panic.
 * 	Only start 4 isr threads - NCPUS is too huge.
 * 	[90/06/06  15:07:41  tmt]
 * 
 * Revision 1.9  90/05/13  18:44:47  devrcs
 * 	Use UIPC/INET config options. No argument to DOMAIN_LOCK macros.
 * 	[90/04/30  10:29:28  tmt]
 * 
 * Revision 1.8  90/04/27  19:13:04  devrcs
 * 	Fix taking assert_wait before calling isr. Do slattach().
 * 	[90/04/20  12:29:39  tmt]
 * 
 * Revision 1.7  90/04/14  00:32:23  devrcs
 * 	Off by one on schednetisr. Also, start 4 threads by default.
 * 	Fix possible leak in netisr_input() and typo.
 * 	[90/04/10  15:34:25  tmt]
 * 
 * 	Add dynamic isr add, delete, packet input. Call config entries
 * 	for Route/Raw, Unix, and Internet domains. Create softnet_intr
 * 	table for holding isr stuff. Move mbuf isr to uipc_mbuf.c.
 * 	[90/04/09  16:10:21  tmt]
 * 
 * Revision 1.6  90/02/05  15:49:59  robert
 * 	Use ffs for netisr bit tests. Do NETISR_MB isr instead of getmclusters()
 * 	[90/01/19  14:36:58  tmt]
 * 
 * Revision 1.5  90/01/18  08:43:53  gm
 * 	Fix splclock() to splsoftclock().
 * 	[90/01/11  13:31:03  tmt]
 * 
 * 	Reset netisrthreads to actual count after all started.
 * 	Use log not printf for error.
 * 	Enhance netisr loop exit (when netisr == 0 or all bits checked).
 * 	spl0() is void on (some?) Unixes.
 * 	[90/01/08  15:53:14  tmt]
 * 
 * 	OSF/1 "one" snapshot revision.
 * 	[90/01/02  12:00:00  tmt]
 * 
 * 	- Base is BSD 4.4 (Alpha) networking.
 * 	- Encore multiprocessing merged in with some structural
 * 	  modifications to support flexible configuration.
 * 	- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 	  lock testing under Unix, thread or software interrupt netisr's,
 * 	  locking and/or spl synchronization, single or multiple CPUs.
 * 	[89/12/20  12:00:00  tmt]
 * 
 * Revision 1.4  90/01/03  12:40:48  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:37:58  gm]
 * 
 * Revision 1.3  89/12/26  09:45:43  gm
 * 	Created.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */

/*
 *	netisr.c - Kernel thread(s) for network code.
 *	Also does network initialization.
 */

#include "net/net_globals.h"

#include "sys/param.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/domain.h"
#include "sys/errno.h"
#include "sys/syslog.h"

#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif

#include "net/if.h"
#include "net/netisr.h"

#if	MACH
#include "sys/sysconfig.h"
#endif

LOCK_ASSERTL_DECL

struct softnet_intr softnet_intr[NETISR_MAX+1];	/* softnet structs */

#if	NETISR_THREAD
#include "sys/user.h"
/* Configurable number of netisr threads (always at least 1). */
int netisrthreads = NNETTHREADS;	/* configurable as "pseudo-device" */

/* Can't pass parameters to threads, so put them here with protection. */
static int (*net_threadfunc)();
static lock_data_t net_threadlock, net_threadwait;
static char *net_threadname;
#ifdef	TNC
static lock_data_t		deferred_threadstart_lock;
#endif
#else	NETISR_THREAD
int	netisr = 0;	/* Indicates a scheduled soft network interrupt */
#endif

#if	NETSYNC_LOCK
simple_lock_data_t	netisr_slock;
#endif

boolean_t netisr_is_init = FALSE;

/*
 * If fullnode==TRUE, we run all network code.
 * If fullnode==FALSE, we are an AD (non-TNC) fileserver, only running
 *		mbinit, uipc_config, and domain_init.
 */
void
netinit(boolean_t fullnode)
{
	int s;
	static char initialized;

	if (initialized)
		return;
	initialized = 1;

	/* Initialize mbufs and also netisr's (mbufs have one
	 * and are initialized earlier on some systems. */
	mbinit();

	if (fullnode) {
		/* Start thread reading packets from Mach */
		netinput_init();
	}

#if	NETISR_THREAD
	{
		/* 
		 * on nodes that do not have networking start only 1 netisr 
		 * thread. Non-networking nodes can execute NFS server code 
		 * and hence need atleast one netisr thread to free up M_EXT
		 * mbufs. Networking nodes will have 'netisrthreads' # of 
		 * netisr threads
		 */
		int i = fullnode ? netisrthreads : 1;
		int p = (i <= 1 ? 0 : 2);	/* High if 1, lower if more */
		lock_init(&net_threadlock, 1);
		lock_init(&net_threadwait, 1);
#ifdef	TNC
		lock_init(&deferred_threadstart_lock, 1);
#endif
		netisrthreads = 0;
		netisr_is_init = TRUE;
		do {
			/* Never defer these, so no NET_THREADSTART macro */
			net_threadstart(netisr_thread, p, "netinit");
			++netisrthreads;
		} while (--i > 0);
	}
#endif
	if (fullnode) {
		
		/* Attach those devices which do not attach themselves */
		/* config() has previously attached hardware */
#include <sl.h>
#if	NSL > 0
		slattach();
#endif
#ifdef TNC
#include <mi.h>
#if	NMI > 0
		miattach();
#endif
#endif	/* TNC */
		loattach();
	}

	/* Initialize interface lists, global network data, domains */
	s = splimp();
	if (fullnode)
		ifinit();
	domaininit(fullnode);
	splx(s);

	/*
	 * Configure the domains. Test for dynamic options and defer
	 * to config manager if set, else look at actual options.
	 */

	/* Raw and routing sockets. Configure unconditionally. */
	if (fullnode)
		route_config();

	/* Unix-local sockets (includes pipes, fifos!) */
#if	defined(MACH) && !defined(UIPC)
#include <uipc.h>
#if	UIPC_DYNAMIC
#undef	UIPC
#define UIPC	0
#endif
#endif

#if	UIPC
	uipc_config();
#endif

	if (fullnode) {

	/* TCP/IP protocols */
#if	defined(MACH) && !defined(INET)
#include <inet.h>
#if	INET_DYNAMIC
#undef	INET
#define INET	0
#endif
#endif

#if	INET
#if	MACH
		inet_config(SYSCONFIG_CONFIGURE,
			    (void *)NULL, 0, (void *)NULL, 0);
#else
		inet_config();
#endif
#endif

	/* XNS protocols */
#if	defined(MACH) && !defined(NS)
#include <ns.h>
#if	NS_DYNAMIC
#undef	NS
#define NS	0
#endif
#endif

#if	NS
#if	MACH
		ns_config(SYSCONFIG_CONFIGURE,
			  (void *)NULL, 0, (void *)NULL, 0);
#else
		ns_config();
#endif
#endif
	}

}

/*
 * Add/delete isr's in input table. Isr's are specified by number,
 * interrupt routine, and optional input queue and domain.
 */
netisr_add(num, isr, ifq, dp)
	int num;
	void (*isr)();
	struct ifqueue *ifq;
	struct domain *dp;
{
	int s, err = 0;
	struct softnet_intr *softnet;

	if ((unsigned)++num > sizeof softnet_intr / sizeof softnet_intr[0] ||
	    isr == NULL)
		return EINVAL;
	softnet = &softnet_intr[num];
	if (dp)
		DOMAINRC_REF(dp);
	s = splimp();
	NETISR_LOCK();
	if (softnet->isr && softnet->isr != isr)
		err = EEXIST;
	else {
		softnet->active = 0;
		softnet->pending = 0;
		softnet->dom = dp;
		softnet->ifq = ifq;
		softnet->isr = isr;
	}
	NETISR_UNLOCK();
	splx(s);
	if (err && dp)
		DOMAINRC_UNREF(dp);
	return err;
}

netisr_del(num)
	int num;
{
	int s, err = 0;
	register struct softnet_intr *softnet;
	struct domain *dp = 0;

	if ((unsigned)++num > sizeof softnet_intr / sizeof softnet_intr[0])
		return EINVAL;
	softnet = &softnet_intr[num];
	s = splimp();
	NETISR_LOCK();
	if (softnet->isr == NULL)
		err = ENOENT;
	else if (softnet->active)
		err = EBUSY;
	else {
		if (softnet->ifq) {
			IFQ_LOCK(softnet->ifq);
			for (;;) {
				register struct mbuf *m;
				IF_DEQUEUE_NOLOCK(softnet->ifq, m);
				if (m == NULL) break;
				m_freem(m);
				IF_DROP(softnet->ifq);
			}
			IFQ_UNLOCK(softnet->ifq);
		}
		dp = softnet->dom;
		softnet->active = 0;
		softnet->pending = 0;
		softnet->dom = NULL;
		softnet->ifq = NULL;
		softnet->isr = NULL;
	}
	NETISR_UNLOCK();
	splx(s);
	if (dp)
		DOMAINRC_UNREF(dp);
	return err;
}

/*
 * Receive packet for given isr. Packet is always freed.
 */
/*
 * Isr == -1 means just look for wildcard receiver.
 * Tries to avoid copies in case not deliverable to intended.
 *
 * N.B. I am unconvinced the raw isr is a useful construct, but it
 * doesn't hurt to have it at the moment. What it might try to provide 
 * is a way for passing packets up with no domains attached, and a
 * NIT-style interface. Note many protocol stacks step on the packet
 * buffer during processing so we pullup the headers for insurance.
 * Also note the loopback interface, among others, leaves few clues
 * to the packet's identity.
 */
netisr_input(num, m, header, hdrlen)
	register int num;
	struct mbuf *m;
	caddr_t header;
	int hdrlen;
{
	register int s, wild, err = 0;
	register struct ifqueue *ifq;
	struct softnet_intr *softnet;

	if ((unsigned)++num > sizeof softnet_intr / sizeof softnet_intr[0]) {
		err = EINVAL;
		num = 0;
	}
	softnet = &softnet_intr[num];
	s = splimp();
	NETISR_LOCK();
	wild = (num > 0 && softnet_intr[0].isr != NULL);
	if (softnet->isr == NULL)
		err = ENOENT;
	else
		ifq = softnet->ifq;
	NETISR_UNLOCK();
	if (err == 0) {
		if (ifq) {
			IFQ_LOCK(ifq);
			if (IF_QFULL(ifq)) {
				IF_DROP(ifq);	/* bump stat, leave err == 0 */
			} else {
				struct mbuf *mcopy = 0;
				if (wild) {
					mcopy=m_copym(m,0,M_COPYALL,M_DONTWAIT);
					if (mcopy)
					  mcopy = m_pullup(mcopy, MHLEN-hdrlen);
				}
				IF_ENQUEUE_NOLOCK(ifq, m);
				m = mcopy;
			}
			IFQ_UNLOCK(ifq);
		}
		schednetisr(num-1);
	}
	splx(s);

	if (m) {
		if (wild) {
			if (header && hdrlen > 0) {
				M_PREPEND(m, hdrlen, M_DONTWAIT);
				if (m == NULL)
					return err;
				bcopy(header, mtod(m, caddr_t), hdrlen);
			}
			(void) netisr_input(-1, m, (caddr_t)0, 0);
		} else
			m_freem(m);
	}
	return err;
}

/*
 * Return ISR appropriate for address family (used by loopback).
 */
netisr_af(af)
{
	static int isrs[] =
		{ -1, -1, NETISR_IP, -1, -1, -1, NETISR_NS, NETISR_ISO };

	if ((unsigned)af > sizeof isrs / sizeof isrs[0])
		return -1;
	return isrs[af];
}

/*
 * Process network interrupts by type. May be called from a software
 * interrupt callout, or from thread context (see below).
 */
void
Netintr()
{
	register void (*isr)();
	register struct softnet_intr *softnet;
	register struct domain *dp;
	int s;

	s = splimp();
	NETISR_LOCK();
#if	!NETISR_THREAD
	netisr = 0;	/* Indicate that the soft interrupt is being handled */
#endif
	for (;;) {
		isr = 0; dp = 0;
		softnet = &softnet_intr[0];
		do {
			if (softnet->pending) {
				isr = softnet->isr;
				dp = softnet->dom;
				++softnet->active;
				softnet->pending = 0;
				break;
			}
		} while (++softnet < &softnet_intr[NETISR_MAX+1]);
		if (!isr)
			break;
		NETISR_UNLOCK();
		splx(s);
#if	NETISR_THREAD
		clear_wait(current_thread(), THREAD_RESTART, FALSE);
#endif
		if (dp) {
			DOMAIN_FUNNEL_DECL(f)
			DOMAIN_FUNNEL(dp, f);
			(*isr)();
			DOMAIN_UNFUNNEL(f);
		} else
			(*isr)();
#if	NETISR_THREAD
		assert_wait(netisr_thread, FALSE);
#endif
		s = splimp();
		NETISR_LOCK();
		--softnet->active;
	}
	NETISR_UNLOCK();
	splx(s);
}

#if	NETISR_THREAD

/*
 * Common code for service thread mainlines. 
 * Call with function pointer to cycle periodically, and set
 * function up to return ticks to next call. Priority is optional.
 */

#if	NETSYNC_SPL
#include "kern/parallel.h"	/* for unix_master() */
#endif

void net_threadstart(func, pri, name)
	int (*func)(), pri;
	char *name;
{
	uthread_t thread;
	static void net_threadmain();

	lock_write(&net_threadlock);
	lock_write(&net_threadwait);
	net_threadfunc = func;
	net_threadname = name;
	ux_create_thread(net_threadmain);
	if (1 /* How can I tell if the thread create failed? */) {
		/*
		 * Don't do anything with the priority yet
		 */
#if	MACH_LDEBUG
		/* The lock debugging would assert otherwise */
		simple_lock(&net_threadwait.interlock);
		net_threadwait.lthread = (char *) thread;
		dec_lock(&net_threadwait, current_thread());
		simple_unlock(&net_threadwait.interlock);
#endif
		lock_write(&net_threadwait);	/* wait... */
	} else
		log(LOG_ERR, "Can't start network thread 0x%x\n", func);
	lock_done(&net_threadwait);
	lock_done(&net_threadlock);
}

static void
net_threadmain()
{
	uthread_t thread = current_thread();
	int ticks, (*func)();

	func = net_threadfunc;
	cthread_set_name(cthread_self(), net_threadname);

#if	MACH_LDEBUG
	simple_lock(&net_threadwait.interlock);
	net_threadwait.lthread = (char *) thread;
	inc_lock(&net_threadwait, thread);
	simple_unlock(&net_threadwait.interlock);
#endif
	lock_done(&net_threadwait);
#if	NETSYNC_SPL
	/* Bind thread to master cpu and pretend we're softclock() */
	unix_master();
	(void) splsoftclock();
#else
	spl0();
#endif

	/* Run (*func)() forever, waking it up after returned
	 * ticks expire. Note netisr threads don't return... */
	for (;;) {
		ticks = (*func)();
			/* <- TIMING HOLE if thread gets async wakeups */
		assert_wait(func, FALSE);
		if (ticks > 0) thread_set_timeout(ticks);
		thread_block();
	}
	/* NOTREACHED */
}

int
netisr_thread()
{

	cthread_set_name(cthread_self(), "netisr_thread");

	/*
	 * Make this thread high priority if it is working alone:
	 */
	if (NNETTHREADS <= 1)
 		set_thread_priority(mach_thread_self(), 3);


#if	NETSYNC_SPL
	(void) splnet();	/* All XXintr()'s need splnet() if spl */
#endif

	for (;;) {
		assert_wait(netisr_thread, FALSE);
		Netintr();		/* Process packets */
		thread_block();
	}
	/* NOTREACHED */
}

#ifdef	TNC
/*
 *  Code for deferred startup of network threads.  Periodic networking
 *  threads should not run on compute nodes, or they will interfere with
 *  tightly synchronized application programs.  We defer starting such
 *  threads until a network interface is configured (thereby assuring
 *  that this node is not a compute node).
 *
 *  The NET_THREADSTART() macro in <net/net_globals.h> replaces all
 *  calls (with one exception) to the net_threadstart() routine.
 *  Instead of calling net_threadstart() directly, the macro makes an
 *  indirect call through the net_thread_start_routine function
 *  pointer.  Initially this pointer is set to net_threaddefer(),
 *  which places its arguments in a fifo.  When a network interface is
 *  successfully configured on a node, the net_threadstart_deferred()
 *  routine is called.  It (a) assigns net_threadstart() to
 *  net_thread_start_routine so that any subsequent calls to
 *  NET_THREADSTART() will start the thread immediately, and (b)
 *  "replays" the arguments stored in the fifo, calling
 *  net_threadstart() for each set of arguments and thus starting the
 *  deferred threads.
 *
 *  The only network threads not started with NET_THREADSTART() are
 *  those at the beginning of netinit(), which are needed to do mbuf
 *  garbage collection on service partition nodes that have no network
 *  interfaces.  These threads may be safely started on compute nodes,
 *  because they will not awaken periodically.
 */

#if	MACH_ASSERT
int	print_thread_deferral = 0;
#define	PRINT(x)	do { if (print_thread_deferral) printf x ; } while (0)
#else
#define PRINT(x)
#endif

#define	Static	/* static */

typedef struct threadstart_args {
	int	(*ta_func)();
	int	ta_pri;
	char	*ta_name;
	struct threadstart_args *ta_next;
} threadstart_args_t;

Static threadstart_args_t	*deferred_call_head = NULL;
Static threadstart_args_t	*deferred_call_tail = NULL;

Static void net_threaddefer(int (*)(), int, char *);
void (*net_thread_start_routine)() = net_threaddefer;

/*
 *  Keep call arguments passed to us via the NET_THREADSTART() macro,
 *  but don't actually start any threads just yet.
 */
Static void
net_threaddefer(
	int	(*func)(),
	int	pri,
	char	*name)
{
	threadstart_args_t	*args;

	lock_write(&deferred_threadstart_lock);

	/* Lost race against net_threadstart_deferred() ? */
	if (*net_thread_start_routine != net_threaddefer) {
		lock_done(&deferred_threadstart_lock);
		(*net_thread_start_routine)(func, pri, name);
		return;
	}

	args = (threadstart_args_t *) malloc(sizeof(threadstart_args_t));
	if (args == NULL)
		panic("net_threaddefer: malloc failed\n");
	args->ta_func = func;
	args->ta_pri = pri;
	args->ta_name = name;
	args->ta_next = NULL;
	PRINT(("Deferring \"%s\" network thread.\n", name));

	if (deferred_call_head == NULL) {
		deferred_call_head = args;
		deferred_call_tail = args;
	} else {
		ASSERT(deferred_call_tail &&
		       deferred_call_tail->ta_next == NULL);
		deferred_call_tail->ta_next = args;
		deferred_call_tail = args;
	}

	lock_done(&deferred_threadstart_lock);
}


/*
 *  Start networking threads recorded in the deferred call fifo.
 */
void
net_threadstart_deferred()
{
	threadstart_args_t	*args, *a2;
	static int		initialized = 0;

	if (initialized)
		return;
	lock_write(&deferred_threadstart_lock);
	net_thread_start_routine = net_threadstart;
	for (args = deferred_call_head; args; ) {

		PRINT(("Starting deferred \"%s\" network thread.\n",
		       args->ta_name));
		net_threadstart(args->ta_func, args->ta_pri, args->ta_name);

		/* Next set of args... */
		a2 = args->ta_next;
		free(args);
		args = a2;
	}
	deferred_call_head = deferred_call_tail = NULL;
	initialized = 1;
	lock_done(&deferred_threadstart_lock);
}

#undef	Static
#undef	PRINT

#endif	/* TNC */
#endif	/* NETISR_THREAD */

