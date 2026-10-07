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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/service_db.c,v 1.3 1994/12/19 19:46:04 sdh Exp $
 *
 * HISTORY
 * $Log: service_db.c,v $
 * Revision 1.3  1994/12/19  19:46:04  sdh
 * Merged R1.2 fixes into R1.3.
 *
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 11511
 *  Testing:
 * 	EATS: controlc, rmcall, rmcmd, nqs
 * 	manual tests
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/smd/allocator.c
 *       	 cmds_libs/src/usr/sbin/smd/app_db.c
 *       	 cmds_libs/src/usr/sbin/smd/app_service.c
 *       	 cmds_libs/src/usr/sbin/smd/init.c
 *       	 cmds_libs/src/usr/sbin/smd/main.c
 *       	 cmds_libs/src/usr/sbin/smd/response.c
 *       	 cmds_libs/src/usr/sbin/smd/service_db.c
 *       	 cmds_libs/src/usr/sbin/smd/socket_iface.c
 *       	 cmds_libs/src/usr/sbin/smd/user_db.c
 * 	 cmds_libs/src/usr/sbin/smd/Makefile
 *
 * Revision 1.2  1994/11/19  03:16:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.1.14.1  1994/04/06  19:51:32  jkearns
 * Added debug information for SMD logs.  (Invoke by issuing "smd -d 0x100".)
 *
 *  Reviewer: Scott Hahn
 *  Risk: Medium
 *  Benefit or PTS #: 8894
 *  Testing: SDSC ran NQS EATs against code.
 *  Module(s):  Makefile allocator.c app_db.c app_service.c debug.c
 * globals.h init.c jkdebug.c main.c part_service.c response.c service_db.c
 * socket_iface.c test_driver.c user_db.c debug2.c defs.h
 *
 * Revision 1.1  1992/10/05  23:30:27  rkl
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include "defs.h"

/*
 *  alloc_service:
 *
 *	Allocate and initialize an smd_service structure.
 */
smd_service_t*
alloc_service(req)
	smd_req_t	*req;
{
	smd_service_t	*serv;

	TRACE(ENTRY, ("alloc_service(req=%x)\n", req));

	/*
	 *  Allocate a service database structure.
	 */
	serv = (smd_service_t*) malloc(sizeof(smd_service_t));
	if (serv == 0) {
		fprintf(stderr, "SMD: No memory for service structure\n");
		return (0);
	}
	TRACE(VERBOSE, ("service %x allocated for request %x\n", serv, req));

	/*
	 *  Initialize the structure.
	 */
	serv->q_id	    = req->q_id;	/* structure copy */
	serv->param	    = req->param;	/* structure copy */
	serv->param.service = req->hdr.type;	/* save for later reference */
	return (serv);
}

/*
 *  free_service:
 *
 *	Free an smd_service structure.
 */
free_service(serv)
	smd_service_t	*serv;
{
	TRACE(ENTRY, ("free_service(serv=%x)\n", serv));
	TRACE(VERBOSE, ("freeing service structure %x\n", serv));

	free(serv);
}

/*
 *  add_service:
 *
 *	Add the service structure into the service list.
 *	Slot 0 of the partition hash table holds all service
 *	entries that have "don't care" as the partition qualifier.
 */
add_service(user, serv)
	smd_user_t	*user;
	smd_service_t	*serv;
{
	smd_service_t	**p;

	TRACE(ENTRY, ("add_service(user=%x, serv=%x)\n", user, serv));
	TRACE(TIMINGS,("%d ADDSERV %x %x\n", time(0), user, serv));


	/*
	 *  Link to the user structure.
	 */
	serv->user           = user;
	serv->user_link_next = user->service;
	serv->user_link_prev = 0;
	if (user->service)
		user->service->user_link_prev = serv;
	user->service   = serv;

	/*
	 *  Link to the partition table.
	 */
	p = p_table + p_hash(serv->q_id.part_id);
	serv->next = *p;
	serv->prev = 0;
	if (*p)
		(*p)->prev = serv;
	*p = serv;

	TRACE(DUMP_USER_REQUEST, ("\
REQ_DUMP pgid = %d part_id = %d acct_id = %d uid = %d req_id = %s\n\
         flags = 0x%x sig_num = %d sig_pid = %d events = 0x%x\n\
         alarm_val = %d service = %d\n",
		serv->q_id.pgid,       serv->q_id.part_id,   serv->q_id.acct_id,
		serv->q_id.uid,        serv->q_id.req_id,    serv->param.flags,
		serv->param.sig_num,   serv->param.sig_pid,  serv->param.events,
		serv->param.alarm_val, serv->param.service));
	trace_users();
	dump_services();
}

/*
 *  remove_service:
 *
 *	Remove the service structure from a user list and the 
 *	partition table.
 */
remove_service(serv)
	smd_service_t	*serv;
{
	TRACE(ENTRY, ("remove_service(serv=%x)\n", serv));
	TRACE(TIMINGS, ("%d DELSERV %x\n", time(0), serv));


	/*
	 *  Unlink from the user structure.
	 */
	if (serv->user_link_prev)
		serv->user_link_prev->user_link_next = serv->user_link_next;
	else
		serv->user->service = serv->user_link_next;

	if (serv->user_link_next)
		serv->user_link_next->user_link_prev = serv->user_link_prev;

	/*
	 *  Unlink from the partition table.
	 */
	if (serv->prev) 
		serv->prev->next = serv->next;
	else
		p_table[ p_hash(serv->q_id.part_id) ] = serv->next;

	if (serv->next)
		serv->next->prev = serv->prev;
	trace_users();
	dump_services();
}

/*
 *  p_hash:
 *
 *	Return a hash index based on the partition ID where a
 *	QUALIFY_ANY value returns 0.
 */
p_hash(part_id)
	int	part_id;
{
	if (part_id == QUALIFY_ANY)
		return (0);
	else	
		return (P_HASH(part_id));
}
