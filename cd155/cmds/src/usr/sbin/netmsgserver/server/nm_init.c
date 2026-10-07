/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
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
 * OSF/1 Release 1.0
 */
/*
 * nm_init.c
 *
 *
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: nm_init.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:11:37 $";
#endif

/*
 * Initialisation for the network server.
 */


#define NM_INIT_DEBUG	0

#include <pthread.h>
#include <sys/errno.h>
#include <mach/boolean.h>

#include "datagram.h"
#include "deltat.h"
#include "dispatcher.h"
#include "ipc.h"
#include "keyman.h"
#include "mem.h"
#include "netmsg.h"
#include "network.h"
#include "nm_init.h"
#include "nn.h"
#include "portcheck.h"
#include "portops.h"
#include "portrec.h"
#include "portsearch.h"
#include "srr.h"
#include "timer.h"
#include "uid.h"

#if	LOCK_THREADS
struct pthread_mutex		*pthread_lock;
pthread_mutex_t	thread_lock_data;
#endif	LOCK_THREADS



/*
 * nm_init
 *	initialises all the modules of the network server.
 *
 * Returns:
 *	TRUE or FALSE depending on whether initialisation was successful or not.
 *
 */
EXPORT boolean_t nm_init()
{
    boolean_t	success = TRUE;

#if	LOCK_THREADS
    pthread_lock = &thread_lock_data;
    pthread_mutex_init(pthread_lock, pthread_mutexattr_default);
    pthread_mutex_setname_np(pthread_lock, "NM_thread_lock");
    pthread_mutex_lock(pthread_lock);
#endif	LOCK_THREADS
    /*
     * Initialise the utility modules.
     */
    if (!(mem_init())) {
	panic("mem_init failed.");
	success = FALSE;
    }
    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "mem_init completed");
    if (!(lock_queue_init())) {
	panic("lock_queue_init failed.");
	success = FALSE;
    }
    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "lock_queue_init completed");
    if (!(uid_init())) {
	panic("uid_init failed.");
	success = FALSE;
    }
    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "uid_init completed");
    if (!(timer_init())) {
	panic("timer_init failed.");
	success = FALSE;
    }
    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "timer_init completed");
    if (!(network_init())) {
	ERROR((msg,"Autoconf: network_init failed: no network"));
	param.conf_network = FALSE;
    }
    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "network_init completed");
    if (!(disp_init())) {
	panic("disp_init failed.");
	success = FALSE;
    }
    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "disp_init completed");
    if (!(transport_init())) {
	panic("transport_init failed.");
	success = FALSE;
    }
    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "transport_init completed");
    if (!(pr_init())) {
	panic("pr_init fails.");
	success = FALSE;
    }
    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "pr_init completed");

    if (param.conf_network) {
	    /*
	     * Initialise the transport protocols about which we know.
	     */
#if	USE_DATAGRAM
	    if (!(datagram_init())) {
		panic("datagram_init failed.");
		success = FALSE;
	    }
	    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "datagram_init completed");
#endif	USE_DATAGRAM
#if	USE_SRR
	    if (!(srr_init())) {
		panic("srr_init failed.");
		success = FALSE;
	    }
	    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "srr_init completed");
#endif	USE_SRR

	    /*
	     * Start the IPC protocol(s).
	     */
	    tr_default_entry = TR_NOOP_ENTRY;

#if	USE_DELTAT
	    if (!(deltat_init())) {
		ERROR((msg,"Autoconf: deltat_init failed."));
	    } else {
		tr_default_entry = TR_DELTAT_ENTRY;
	    }
	    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "deltat_init completed");
#endif	USE_DELTAT

#if	USE_VMTP
#if	RPCMOD
	    if (!(vmtp2_init())) {
#else	RPCMOD
	    if (!(vmtp1_init())) {
#endif	RPCMOD
		ERROR((msg,"Autoconf: vmtp?_init failed."));
	    }
	    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "vmtp?_init completed");
#endif	USE_VMTP

#if	USE_TCP
	    if (!(tcp_init())) {
		ERROR((msg,"Autoconf: tcp_init failed."));
	    } else {
		    tr_default_entry = TR_TCP_ENTRY;
	    }
	    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "tcp_init completed");
#endif	USE_TCP

	    /*
	     * Make sure there is at least one valid IPC protocol.
	     */
	    if (tr_default_entry == TR_NOOP_ENTRY) {
		    panic("Network enabled, but no valid IPC transport protocol.");
	    }

    } else {
	    tr_default_entry = TR_NOOP_ENTRY;
    }


    /*
     * Initialise the higher level modules.
     */
    if (!(netname_init())) {
	panic("netname_init fails.");
	success = FALSE;
    }
    DEBUG_STRING(NM_INIT_DEBUG, 5, 2, "netname_init completed");
    if (!(ipc_init())) {
	panic("ipc_init failed.");
	success = FALSE;
    }
#if	USE_KEYMAN
    if (!(km_init())) {
	panic("km_init failed.");
	success = FALSE;
    }
#endif	USE_KEYMAN
    if (!(pc_init())) {
	panic("pc_init fails.");
	success = FALSE;
    }
    if (!(po_init())) {
	panic("po_init fails.");
	success = FALSE;
    }
    if (!(ps_init())) {
	panic("po_init fails.");
	success = FALSE;
    }
    if (!(ls_init_2())) {
	panic("ls_init_2 failed.");
	success = FALSE;
    }

#if	NETPORT
    if (netport_init(task_self()) != KERN_SUCCESS) {
	ERROR((msg,"Autoconf: netport_init failed."));
	param.conf_netport = FALSE;
    }
#endif	NETPORT

#if	CAMELOT
    if (!cam_stuff_init()) {
	panic("cam_stuff_init failed.");
	success = FALSE;
    }
#endif	CAMELOT

#if	LOCK_THREADS
    pthread_mutex_unlock(pthread_lock);
#endif	LOCK_THREADS

    return success;

}
