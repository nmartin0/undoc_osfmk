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
 * $Log: net_globals.h,v $
 * Revision 1.7  1995/02/10  23:38:27  stans
 *  If 'CONST' is defined, then undef it.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing:WW50 sats
 *
 * Revision 1.6  1995/02/01  21:33:32  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:33:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:08:02  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/09  15:04:20  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.1.1.3  1993/07/01  19:28:14  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  18:59:46  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:26:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:24  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:19:07  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:41  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.5  93/07/07  10:31:55  mjl
 * [LCC #0314] Define NET_THREADSTART() macro for delayed start up of network
 * threads.
 * 
 * Revision 2.4  1993/04/08  11:30:21  loverso
 * 	Include synch.h to get the null spl macro defs and avoid func calls.
 * 	[1992/06/04  14:14:14  condict]
 *
 * Revision 2.3  92/05/24  14:34:14  pjg
 * 	92/03/23  18:03:35  condict
 * 	Allow NCPUS == 1 in the server, to compile optimally for a
 *	uni-processor.
 * 	[92/05/18            srl]
 * 
 * Revision 2.2  91/08/31  13:40:05  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/07/31  15:33:27  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.13.4.2  91/02/19  15:24:16  tmt
 * 	Fix comment string within comment.
 * 	[91/02/16  15:49:40  tmt]
 * 
 * Revision 1.13  90/10/31  14:01:45  devrcs
 * 	Enable locking when UNIX_LOCKS option set. Add comments on Unix build.
 * 	[90/10/06  16:35:50  tmt]
 * 
 * Revision 1.12  90/10/07  14:32:08  devrcs
 * 	Cleanup RESERVED and ASSERT and fix Unix build.
 * 	[90/10/01  17:02:55  tmt]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:10:39  gm]
 * 
 * 	Add Unix glue for OSF parallelization in filesystem.
 * 	[90/09/29  19:45:20  tmt]
 * 
 * Revision 1.11  90/09/23  15:55:27  devrcs
 * 	In the interest of speed for now, don't do locking on uni's.
 * 	[90/09/16  10:54:31  tmt]
 * 
 * 	Add netthreads.h include. Use UNIX_LOCKS option to
 * 	decide locking. Rearrange settings and add comments.
 * 	[90/09/11  11:40:04  tmt]
 * 
 * Revision 1.10  90/07/27  08:59:09  devrcs
 * 	Checkpoint at Reno update.
 * 	[90/07/19  16:36:35  tmt]
 * 
 * Revision 1.9  90/07/05  23:12:33  devrcs
 * 	Change name of SPL macros. Add comment.
 * 	[90/07/03  18:45:40  tmt]
 * 
 * Revision 1.8  90/06/22  20:38:32  devrcs
 * 	nags merge
 * 	[90/06/12  21:35:16  gmf]
 * 
 * 	MMAX_MP is finally gone, set glue directly.
 * 	[90/06/11  16:39:24  tmt]
 * 
 * 	Enable parallelization!
 * 	Clean up #defines and tokens. Add PARALLEL_SELECT.
 * 	Add lock_t typedefs for Unix. get sys/types.h for prototypes.
 * 	Use ASSERT macros under Mach. Ignore MMAX_MP (almost).
 * 	[90/06/06  10:05:33  tmt]
 * 
 * Revision 1.7  90/04/27  19:12:55  devrcs
 * 	Add assert_wait, clear_wait null macros for Unix.
 * 	[90/04/20  12:27:43  tmt]
 * 
 * Revision 1.6  90/03/27  13:21:01  gm
 * 	Change args to lock_init2() [gmf]
 * 
 * Revision 1.5  90/02/05  15:49:54  robert
 * 	Refinements for changes in other header files for user compile compat.
 * 	[90/01/19  14:31:08  tmt]
 * 
 * Revision 1.4  90/01/18  08:43:42  gm
 * 	Get machine/cpu.h for NCPUS. Currently disable parallelization by
 * 	forcing NETNCPUS (# cpus networking may execute on) to 1.
 * 	[90/01/08  15:52:15  tmt]
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
 * Revision 1.3  90/01/03  12:40:45  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:37:55  gm]
 * 
 * Revision 1.2  89/12/26  09:45:09  gm
 * 	Created.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
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
 * Global #defines for OSF/1 networking.
 *
 * Ugly as this file is, it makes the code a lot cleaner!
 *
 * The following major #defines are supported:
 *	MACH		multi or uniprocessor mach
 *	NCPUS		number of processors
 *	NNETTHREADS	pseudo-device specifying threads
 *	UNIX_LOCKS	configure with locking
 *	VAGUE_STATS	no locking around stats
 *
 * These defines yield the following internal defines:
 *	NETNCPUS	number of processors doing network
 *	NETSYNC_SPL	do process synch with spl's (may co-exist w/locks)
 *	NETSYNC_LOCK	do process sync with locks (may co-exist w/spl)
 *	NETISR_THREAD	do isr's via thread (else via software interrupt)
 *	NETSYNC_LOCKTEST	turn on extra lock debugging (UNIX, too)
 *
 * Current prerequisites (not enforced!):
 *	One or both of NETSYNC_SPL && NETSYNC_LOCK
 *	If !MACH, then (implied) UNIX
 *	NETISR_THREAD requires MACH
 *	NETSYNC_LOCK requires NETISR_THREAD
 *	UNIX requires NETSYNC_SPL
 */

#ifndef	_NET_GLOBALS_H_
#define _NET_GLOBALS_H_

#if	MACH

#include <cpus.h>
#include <unix_locks.h>
#include <vague_stats.h>
#include <netthreads.h>

/*
 * These are default settings. Either or both of locking and spl are valid
 * for 1 or more cpus. However, recommend locks for multis, non-locks for unis.
 * The thread decision is dependent on several things. The biggest problem
 * with using softnets is reliable memory allocation, see net/net_malloc.h.
 */

#define NETNCPUS	NCPUS

#define NETSYNC_LOCK	UNIX_LOCKS	/* maybe locks for synch */
#define NETSYNC_SPL	!NETSYNC_LOCK	/* else spl for synch */
#define NETISR_THREAD	(NETSYNC_LOCK || (NETNCPUS > 1) || (NNETTHREADS > 0))

#define PARALLEL_SELECT	1		/* new-style select */

#else	/* UNIX */

/*
 * Builds under 4.3 Reno (4.4 alpha) are possible. To do so:
 *	Move bsd/uipc* and bsd/sys_socket.c to kern/
 *	Move all of {net,netinet,netns}/ to same
 *	Move sys/{socket,socketvar,un,unpcb,domain,protosw,mbuf}.h to sys/
 *	Add -D_KERNEL to conf/Makefile.<machine> and -I.. to includes
 *	Delete radix.c, rtsock.c, and af.c from conf/files, and add netisr.c
 *	Delete slattach, loattach, ifinit, domaininit in kern/init_main.c,
 *		replace with netinit()
 *	Delete all dispatch (ipintr, etc) in locore.s and call Netintr
 *	Important: add options UIPC to machine config
 *	Options NS will work, but options ISO or IMP will not. Someday...
 *	Add options DEBUG to test locking
 * Applications will work with the following:
 *	Netstat will require recompile, route -C will work if route doesn't.
 *	Existing binaries will work unmodified, but if recompiling, force
 *		_SOCKADDR_LEN on in sys/socket.h and disable the redefine
 *		of the send/recv libc functions there.
 */
#define NETNCPUS	1
#define NETISR_THREAD	0	/* UNIX uses software interrupts */
#define NETSYNC_LOCK	DEBUG	/* and tests locks in debug */
#define NETSYNC_SPL	1	/* but spl for actual synch */

#define PARALLEL_SELECT	0	/* old-style select */
#define VAGUE_STATS	0
#define assert_wait(a,b)
#define clear_wait(a,b,c)
#define unix_master()
#define unix_release()

/* OSF/1 Parallelization compat for UNIX builds */
#define ASSERT(x)
#define BM(x)
#define FP_LOCK(x)
#define FP_UNLOCK(x)
#define FP_REF(x)
#define FP_UNREF(x)
#define U_FDTABLE_LOCK(x)
#define U_FDTABLE_UNLOCK(x)
#define VN_LOCK(x)
#define VN_UNLOCK(x)

#define fdealloc(fp)		{ crfree((fp)->f_cred); (fp)->f_count = 0; }
#define U_FD_SET(fd, val, x)	u.u_ofile[fd] = (val)

#endif

#if	NETSYNC_LOCK	/* Configure locking */

#if	MACH

#include "kern/lock.h"
#include "kern/assert.h"
#include "sys/lock_types.h"
#include <mach_ldebug.h>
#define LOCK_ASSERTL_DECL
#define LOCK_ASSERTL(string, cond)	ASSERT(cond)
#define LOCK_ASSERT(string, cond)	ASSERT(cond)

#define NETSYNC_LOCKTEST (DEBUG || MACH_LDEBUG)
#define LOCK_NETSTATS	 (VAGUE_STATS == 0)

#else	/* UNIX */

#include "net/net_unixlock.h"
#define LOCK_ASSERTL_DECL		static char _file_[] = __FILE__; \
					extern char _net_lock_format_[];
#define LOCK_ASSERTL(string, cond)	{ if(!(cond)) printf(_net_lock_format_,\
						string, _file_, __LINE__); }
#define LOCK_ASSERT(string, cond)	{ if(!(cond)) printf("\t%s\n",string); }
#define NETSYNC_LOCKTEST DEBUG
#define LOCK_NETSTATS	 1

#endif

#else	/* !NETSYNC_LOCK */
#define LOCK_ASSERTL_DECL
#define LOCK_ASSERTL(string, cond)
#define LOCK_ASSERT(string, cond)
#define NETSYNC_LOCKTEST	0
#define LOCK_NETSTATS		0
#if	!MACH
typedef int	lock_data_t, *lock_t;
#endif

#endif

#if	NETSYNC_SPL	/* Do spl() synch */
#include <sys/synch.h>		/* For the spl macros */
#define NETSPL_DECL(s)		int s;
#ifdef	__STDC__
#define NETSPL(s,level)		s = spl##level()
#else
#define NETSPL(s,level)		s = spl/**/level()
#endif
#define NETSPLX(s)			splx(s)
#else			/* Don't spl() synch */
#define NETSPL_DECL(s)
#define NETSPL(s,level)
#define NETSPLX(s)
#endif

#if	LOCK_NETSTATS
#define NETSTAT_LOCKINIT(lockp)	simple_lock_init(lockp)
#define NETSTAT_LOCK(lockp)	simple_lock(lockp)
#define NETSTAT_UNLOCK(lockp)	simple_unlock(lockp)
#else
#define NETSTAT_LOCKINIT(lockp)
#define NETSTAT_LOCK(lockp)
#define NETSTAT_UNLOCK(lockp)
#endif

/* ANSI-C compatibility */
#ifdef	__STDC__
#ifdef	CONST
#undef	CONST
#endif
#define CONST		const
#define VOLATILE	volatile
#else
#define CONST
#define VOLATILE
#endif

/* Global function prototypes */
#include "sys/types.h"
#include "net/proto_net.h"
#include "net/proto_uipc.h"

/* Deferred startup of networking threads */
#if	defined(TNC) && NETISR_THREAD
extern void	(*net_thread_start_routine)(int (*)(), int, char *);
#define NET_THREADSTART	(*net_thread_start_routine)
#else
#define NET_THREADSTART	net_threadstart
#endif

#endif	/* _NET_GLOBALS_H_ */
