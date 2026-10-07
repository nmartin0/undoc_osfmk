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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/user_db.c,v 1.3 1994/12/19 19:46:16 sdh Exp $
 *
 * HISTORY
 * $Log: user_db.c,v $
 * Revision 1.3  1994/12/19  19:46:16  sdh
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
 * Revision 1.2  1994/11/19  03:16:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.1.14.2  1994/04/06  19:51:34  jkearns
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
 * Revision 1.1.14.1  1994/02/23  19:00:15  jkearns
 *  Reviewer: SDSC
 *  Risk: Medium
 *  Benefit or PTS #: 7955
 *  Testing: Running at SDSC, KFA
 *  Module(s): init.c, main.c, user_db.c
 *
 * Revision 1.1  1992/10/05  23:30:28  rkl
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include "defs.h"

/*
 *  add_user:
 *
 *	Add a new user connection to the database.
 */
add_user()
{
	smd_user_t	*user;
	int		fd;

	TRACE(ENTRY, ("add_user()\n"));

	/*
	 *  Connect up with the user.
	 */
	fd = connect_user();
	if (fd < 0)
		return;

	/*
	 *  Allocate a user database structure.
	 */
	user = (smd_user_t*) malloc(sizeof(smd_user_t));
	if (user == 0) {
		fprintf(stderr, "SMD: No memory for user structure\n");
		close(fd);
		return;
	}

	/*
	 *  Set the FD and add them to the list.
	 */
	user->fd = fd;
	insert_user(user);
	FD_SET(fd, &readmask);
	TRACE(USERS, ("New user %x on fd %d\n", user, fd));
}

/*
 *  free_user:
 *
 *	Free user structure.
 */
free_user(user)
	smd_user_t *user;
{
	TRACE(ENTRY, ("free_user(user=%x)\n", user));
	TRACE(VERBOSE, ("freeing user %x)\n", user));
	free(user);
}

/*
 *  close_user:
 *
 *	Free resources associated with a user that has closed the
 *	service connection.
 */
close_user(user)
	smd_user_t *user;
{
	TRACE(ENTRY, ("close_user(user=%x)\n", user));
	FD_CLR(user->fd, &readmask);
	close(user->fd);
	user->fd = -1;
	clean_user(user);
}

/*
 *  insert_user:
 *
 *	Insert user database structure into the list of active users.
 */
insert_user(user)
	smd_user_t *user;
{
	TRACE(ENTRY, ("insert_user(user=%x)\n", user));
	TRACE(TIMINGS, ("%d ADDUSER %x %x %x %x %d\n", time(0), 
	user, user->next, user->prev, user->service, user->fd)); 

	user->next = user_db;
	user->prev = 0;
	if (user_db)
		user_db->prev = user;
	user_db = user;
	trace_users();
}

/*
 *  clean_user:
 *
 *	Free all resource associated with a user when a connection is
 *	closed.  If the user has set the KEEP_AFTER_CLOSE bit and has
 *	specified a signal interface, the service resources will be kept.
 */
clean_user(user)
	smd_user_t	*user;
{
	smd_service_t	*serv;

	TRACE(ENTRY, ("clean_user(user=%x)\n", user));

	for (serv = user->service; serv; ) {
		struct service  *tmpnext;
		tmpnext=serv->next;
		if ( (serv->param.flags & KEEP_AFTER_CLOSE) &&
		    ((serv->param.flags & SEND_SIGNAL)
#ifdef not_now
		     || (serv->param.service == SMD_MON_PARTITION_USAGE_REQ)
#endif
		     							)) {
			serv=tmpnext;
			continue;
		} else {
			remove_service(serv);
			free_service(serv);
		}
		serv=tmpnext;
	}

	/*
	 *  If all services have been removed, remove the user too.
	 */
	if (user->service == 0) {
		remove_user(user);
		free_user(user);
	}
}

/*
 *  remove_user:
 *
 *	Remove user structure from user list.
 */
remove_user(user)
	smd_user_t	*user;
{
	TRACE(ENTRY, ("remove_user(user=%x)\n", user));
	TRACE(VERBOSE, ("removing user %x)\n", user));
	
	/*
	 *  Unlink user list.
	 */
	if (user->prev) 
		user->prev->next = user->next;
	else
		user_db = user->next;

	if (user->next)
		user->next->prev = user->prev;
	TRACE(TIMINGS, ("%d DELUSER %x\n", time(0), user));
	trace_users();
	dump_services();	
}
