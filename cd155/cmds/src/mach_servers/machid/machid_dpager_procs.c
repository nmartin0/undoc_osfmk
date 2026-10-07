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
 * $Log: machid_dpager_procs.c,v $
 * Revision 1.2  1994/11/21  16:39:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/05/04  01:39:16  stans
 * Initial revision
 *
 * Revision 1.2  1992/05/12  14:33:06  devrcs
 * 	Created for OSF/1 MK
 * 	[1992/05/04  05:38:21  condict]
 *
 * Revision 2.3  92/01/22  22:51:48  rpd
 * 	Added <servers/machid_lib.h>.
 * 	[92/01/22            rpd]
 * 
 * Revision 2.2  92/01/17  15:05:58  rpd
 * 	Added page_size to default_pager_info.
 * 	Added default_pager_object_pages.
 * 	[92/01/03            rpd]
 * 
 * 	Added default_pager_objects.
 * 	Moved default_pager_info here from machid_procs.c.
 * 	Created.
 * 	[91/12/16            rpd]
 * 
 * $EndLog$
 */

#include <stdio.h>
#include <mach.h>
#include <mach/default_pager.h>
#include <servers/machid_types.h>
#include <servers/machid_lib.h>
#include "machid_internal.h"

kern_return_t
do_default_pager_info(server, auth, default_pager, infop)
    mach_port_t server;
    mach_port_t auth;
    mdefault_pager_t default_pager;
    default_pager_info_t *infop;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(default_pager, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_DEFAULT_PAGER;
	return kr;
    }

    kr = default_pager_info(port, infop);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_default_pager_objects(server, auth, default_pager,
			 objectsp, numobjectsp)
    mach_port_t server;
    mach_port_t auth;
    mdefault_pager_t default_pager;
    default_pager_object_t **objectsp;
    unsigned int *numobjectsp;
{
    default_pager_object_t *objects = *objectsp;
    unsigned int numobjects = *numobjectsp;
    mach_port_t ports_buf[1024];
    mach_port_t *ports = ports_buf;
    unsigned int numports = sizeof ports_buf/sizeof ports_buf[0];
    unsigned int i;
    mhost_priv_t host;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(default_pager, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_DEFAULT_PAGER;
	return kr;
    }

    kr = default_pager_objects(port, &objects, &numobjects, &ports, &numports);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    if (numobjects != numports) {
	for (i = 0; i < numports; i++) {
	    if (MACH_PORT_VALID(ports[i])) {
		kr = mach_port_deallocate(mach_task_self(), ports[i]);
		if (kr != KERN_SUCCESS)
		    quit(1, "machid: mach_port_deallocate: %s\n",
			 mach_error_string(kr));
	    }
	}

	if ((ports != ports_buf) && (numports != 0)) {
	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) ports,
			       (vm_size_t) (numports * sizeof *ports));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n",
		     mach_error_string(kr));
	}

	if ((objects != *objectsp) && (numobjects != 0)) {
	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) objects,
			       (vm_size_t) (numobjects * sizeof *objects));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n",
		     mach_error_string(kr));
	}

	return KERN_FAILURE;
    }

    /* propogate host from default pager to memory object */
    host = assoc_lookup(default_pager, MACH_TYPE_HOST_PRIV);

    for (i = 0; i < numobjects; i++) {
	mobject_name_t object;

	object = name_lookup(ports[i], MACH_TYPE_OBJECT_NAME);
	assoc_create(object, MACH_TYPE_HOST_PRIV, host);

	objects[i].dpo_object = object;
    }

    if ((ports != ports_buf) && (numports != 0)) {
	kr = vm_deallocate(mach_task_self(), (vm_offset_t) ports,
			   (vm_size_t) (numports * sizeof *ports));
	if (kr != KERN_SUCCESS)
	    quit(1, "machid: vm_deallocate: %s\n",
		 mach_error_string(kr));
    }

    *objectsp = objects;
    *numobjectsp = numobjects;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_default_pager_object_pages(server, auth, default_pager, object,
			      pagesp, countp)
    mach_port_t server;
    mach_port_t auth;
    mdefault_pager_t default_pager;
    mobject_name_t object;
    default_pager_page_array_t *pagesp;
    unsigned int *countp;
{
    mach_port_t dport, oport;
    kern_return_t kr;

    kr = port_lookup(default_pager, auth, mo_Info, &dport);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_DEFAULT_PAGER;
	return kr;
    }

    kr = port_lookup(object, auth, mo_Info, &oport);
    if (kr != KERN_SUCCESS) {
	port_consume(dport);
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_MEMORY_OBJECT;
	return kr;
    }

    kr = default_pager_object_pages(dport, oport, pagesp, countp);
    port_consume(dport);
    port_consume(oport);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}
