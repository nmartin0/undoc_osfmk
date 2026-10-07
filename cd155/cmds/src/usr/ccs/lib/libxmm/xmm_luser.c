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
 * @OSF_FREE_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: xmm_luser.c,v $
 * Revision 1.2  1994/11/19  02:38:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/23  01:05:46  stans
 * Initial revision
 *
 * Revision 1.2  1992/05/12  14:50:43  devrcs
 * 	Created for OSF/1 MK
 * 	[1992/05/04  08:10:50  condict]
 *
 * Revision 2.4  92/01/22  22:54:26  rpd
 * 	Fixed includes to use "" when appropriate.
 * 	[92/01/18            rpd]
 * 
 * Revision 2.3  91/08/30  15:42:04  rpd
 * 	Removed dummy memory_object_data_supply stub.
 * 	[91/08/30            rpd]
 * 
 * Revision 2.2  91/07/06  15:07:22  jsb
 * 	First checkin.
 * 
 * $EndLog$
 */
/*
 *	File:	xmm_luser.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Definitions for non-kernel xmm use.
 */

#include <mach.h>
#include <cthreads.h>
#include <mach/message.h>
#include "xmm_obj.h"

struct mutex master_mutex;

master_lock()
{
	mutex_lock(&master_mutex);
}

master_unlock()
{
	mutex_unlock(&master_mutex);
}

/* XXX should move master_mutex stuff into mig_loop.c? */

xmm_user_init()
{
	extern int the_mig_loop();
	extern int verbose;

	mutex_init(&master_mutex);
	if (verbose) {
		printf("Initializing mig subsystem...\n");
	}
	xmm_mig_init();
	if (verbose) {
		printf("Initializing protocols...\n");
	}
	net_init();
	if (verbose) {
		printf("Initializing obj subsystem...\n");
	}
	obj_init();
	if (verbose) {
		printf("Initializing xmm subsystem...\n");
	}
	xmm_init();
#if 1
	/* XXX should at least make sure this is safe to do */
	cthread_detach(cthread_fork(the_mig_loop, 0));
	if (verbose) {
		printf("ready.\n");
	}
#else
	if (verbose) {
		printf("ready.\n");
	}
	the_mig_loop();
#endif
}

obj_server(request, reply)	/* XXX move this into xmm_server */
	char *request;
	char *reply;
{
	if (memory_object_server(request, reply)) {
		return TRUE;
	}
	if (proxy_server(request, reply)) {
		return TRUE;
	}
	if (xmm_net_server(request, reply)) {
		return TRUE;
	}
	return FALSE;
}

panic(fmt, a, b, c, d, e, f, g, h, i, j, k, l)
	char *fmt;
	int a, b, c, d, e, f, g, h, i, j, k, l;
{
	printf("panic: ");
	printf(fmt, a, b, c, d, e, f, g, h, i, j, k, l);
	printf("\n");
	exit(1);
}

#undef	atop
#undef	ptoa

atop(a)
{
	return a / vm_page_size;
}

ptoa(p)
{
	return p * vm_page_size;
}
