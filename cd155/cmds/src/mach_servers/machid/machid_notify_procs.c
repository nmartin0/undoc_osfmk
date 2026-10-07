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
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * $Log: machid_notify_procs.c,v $
 * Revision 1.2  1994/11/21  16:39:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/05/04  01:39:27  stans
 * Initial revision
 *
 * Revision 1.2  1992/05/12  14:33:18  devrcs
 * 	Created for OSF/1 MK
 * 	[1992/05/04  05:40:04  condict]
 *
 * Revision 2.4  91/03/27  17:26:29  mrt
 * 	Changed mach.h include
 * 
 * Revision 2.3  91/03/19  12:30:44  mrt
 * 	Changed to new copyright
 * 
 * Revision 2.2  90/09/12  16:31:50  rpd
 * 	Created.
 * 	[90/06/18            rpd]
 * 
 * $EndLog$
 */

#include <stdio.h>
#include <mach.h>
#include "machid_internal.h"

kern_return_t
do_mach_notify_port_deleted(notify, name)
    mach_port_t notify;
    mach_port_t name;
{
    quit(1, "machid: do_mach_notify_port_deleted\n");
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_msg_accepted(notify, name)
    mach_port_t notify;
    mach_port_t name;
{
    quit(1, "machid: do_mach_notify_msg_accepted\n");
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_port_destroyed(notify, port)
    mach_port_t notify;
    mach_port_t port;
{
    quit(1, "machid: do_mach_notify_port_destroyed\n");
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_no_senders(notify, mscount)
    mach_port_t notify;
    mach_port_mscount_t mscount;
{
    quit(1, "machid: do_mach_notify_no_senders\n");
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_send_once(port)
    mach_port_t port;
{
    quit(1, "machid: do_mach_notify_send_once\n");
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_dead_name(notify, name)
    mach_port_t notify;
    mach_port_t name;
{
	printf("machid: deadname for %d\n", name);
    port_destroy(name);
    return KERN_SUCCESS;
}
