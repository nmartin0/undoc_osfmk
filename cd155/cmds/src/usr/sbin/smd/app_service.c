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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/app_service.c,v 1.4 1994/12/19 19:45:41 sdh Exp $
 *
 * HISTORY
 * $Log: app_service.c,v $
 * Revision 1.4  1994/12/19  19:45:41  sdh
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
 * Revision 1.2.14.2  1994/04/06  19:55:26  jkearns
 * SMD wasn't verifying that partition IDs matched when a cancel request
 * was received.
 *
 *  Reviewer: Scott Hahn
 *  Risk: Medium
 *  Benefit or PTS #: 8865
 *  Testing: SDSC ran EATs against it.
 *  Module(s): app_service.c
 *
 * Revision 1.2.14.1  1994/04/06  19:51:22  jkearns
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
 * Revision 1.3  1994/11/19  03:16:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/29  17:21:12  rkl
 * Fixed hang bug.  Changed remove() to remove_user().
 *
 * Revision 1.1  1992/10/05  23:30:25  rkl
 * Initial revision
 *
 *
 */

#include "defs.h"

/*
 *  register_event:
 *
 *	Register SMD_EVENT_REQ request.
 */
register_event(user, req)
	smd_user_t	*user;
	smd_req_t	*req;
{
	extern	smd_service_t*	alloc_service();

	smd_service_t	*serv;

	TRACE(ENTRY, ("event(user=%x, req=%x)\n", user, req));

	/*
	 *  Allocate the service structure and add it to the service
	 *  lists.  If there is no memory, send and error response.
	 */
	if ((serv = alloc_service(req)) == 0) {
		int	type = SMD_APP_END_IND;		/* set default */

		/*
		 *  Determine the response type.
		 */
		if (req->param.events & APP_START)
			type = SMD_APP_START_IND;
		else if (req->param.events & APP_ROLLIN)
			type = SMD_APP_ROLLIN_IND;
		else if (req->param.events & APP_ROLLOUT)
			type = SMD_APP_ROLLOUT_IND;
		send_error_resp(user, req, type, NO_RESOURCES);

	} else {
		add_service(user, serv);
	}
}

/*
 *  register_alarm:
 *
 *	Register SMD_APP_ALARM_REQ request.
 */
register_alarm(user, req)
	smd_user_t	*user;
	smd_req_t	*req;
{
	smd_service_t	*serv;

	TRACE(ENTRY, ("app_alarm(user=%x, req=%x)\n", user, req));

	/*
	 *  Allocate the service structure and add it to the service
	 *  lists.  If there is no memory, send and error response.
	 */
	if ((serv = alloc_service(req)) == 0)
		send_error_resp(user, req, SMD_APP_ALARM_IND, NO_RESOURCES);
	else
		add_service(user, serv);
}

/*
 *  get_status:
 *
 *	Service SMD_GET_STATUS_REQ request.
 */
get_status(user, req)
	smd_user_t	*user;
	smd_req_t	*req;
{
	smd_app_t	*app;
	smd_hdr_t	 hdr;
	smd_resp_t	 resp;
	int		 i;
	int		 found = total_apps;

	TRACE(ENTRY, ("get_status(user=%x, req=%x)\n", user, req));

	/*
	 *  If there are no applications outstanding, send the error
	 *  response.
	 */
	if (!found)
		goto no_apps;
	/*
	 *  Do the common case (resource deamon) first.
	 */
	if ((req->q_id.pgid    == QUALIFY_ANY) &&
	    (req->q_id.part_id == QUALIFY_ANY) &&
	    (req->q_id.acct_id == QUALIFY_ANY) &&
	    (req->q_id.uid     == QUALIFY_ANY)) {

		hdr.type = SMD_STATUS_RESP;
		hdr.len  = total_apps * sizeof(smd_resp_t);
		send_msg(user, (char*) &hdr, sizeof(hdr));

		for (i = 0; i < A_HASH_SIZE; i++)
			for (app = a_table[ i ]; app; app = app->next)
				send_status_resp(user, &resp, app, req);
	} else {
		/*
		 *  Get a count of the number of apps that qualify.
		 */
		found = 0;
		for (i = 0; i < A_HASH_SIZE; i++) {
			for (app = a_table[ i ]; app; app = app->next)
				if (app_of_interest(req, app))
					found++;
		}

		/*
		 *  Bail if nothing is of interest.
		 */
		if (!found)
			goto no_apps;

		/*
		 *  Send the header now that we now how many responses
		 *  there will be.
		 */
		hdr.type = SMD_STATUS_RESP;
		hdr.len  = found * sizeof(smd_resp_t);
		send_msg(user, (char*) &hdr, sizeof(hdr));

		/*
		 *  Got through the list (again) to send the data.
		 */
		for (i = 0; i < A_HASH_SIZE; i++)
			for (app = a_table[ i ]; app; app = app->next)
				if (app_of_interest(req, app))
					send_status_resp(user, &resp, app, req);
	}

	/*
	 *  If there were no qualifing applications, if the error response.
	 */
no_apps:
	if (!found) {
		hdr.type = SMD_STATUS_RESP;
		hdr.len  = sizeof(smd_resp_t);
		send_msg(user, (char*) &hdr, sizeof(hdr));

		resp.q_id         = req->q_id;		/* structure copy */
		resp.value.status = APP_NOT_FOUND;
		send_msg(user, (char*) &resp, sizeof(smd_resp_t));
	}
}

/*
 *  app_of_interest:
 *
 *	See if the application qualifies with the user request.
 */
app_of_interest(req, app)
	smd_req_t	*req;
	smd_app_t	*app;
{
	TRACE(ENTRY, ("app_of_interest(req=%x, app=%x)\n", req, app));

	if (((req->q_id.pgid    == QUALIFY_ANY) ||
	     (req->q_id.pgid    == app->data.pgid))	&&
	    ((req->q_id.part_id == QUALIFY_ANY) ||
	     (req->q_id.part_id == app->data.part_id))	&&
	    ((req->q_id.acct_id == QUALIFY_ANY) ||
	     (req->q_id.acct_id == app->data.acct_id))	&&
	    ((req->q_id.uid     == QUALIFY_ANY) ||
	     (req->q_id.uid     == app->data.uid)))
		return (1);
	else
		return (0);
}

/*
 *  cancel:
 *
 *	Service SMD_CANCEL_REQ request.
 */
cancel(user, req)
	smd_user_t	*user;
	smd_req_t	*req;
{
	smd_service_t	*s, *stmp;
	smd_user_t	*u, *utmp;

	TRACE(ENTRY, ("cancel(user=%x, req=%x)\n", user, req));
	TRACE(TIMINGS, ("%d CANCELU %x\n", time(0), user));

	/*
	 *  Loop through all user service requests.
	 */
	u = user_db;
	while (u) {
		s = u->service;
		while (s) {
			if ((bcmp(req->q_id.req_id, s->q_id.req_id,
				  		SMD_REQ_ID_LEN) == 0)	&&
			   ((req->q_id.part_id  == QUALIFY_ANY) ||
			    (req->q_id.part_id  == s->q_id.part_id))	&&
			   ((req->q_id.pgid     == QUALIFY_ANY) ||
			    (req->q_id.pgid     == s->q_id.pgid))	&&
			   ((req->q_id.acct_id  == QUALIFY_ANY) ||
			    (req->q_id.acct_id  == s->q_id.acct_id))	&&
			   ((req->q_id.uid      == QUALIFY_ANY) ||
			    (req->q_id.uid      == s->q_id.uid))	&&
			   ((req->param.service == QUALIFY_ANY) ||
			    (req->param.service == s->param.service))) {
				/*
				 *  Save a temparary pointer to the next
				 *  service structure before freeing.
				 */
				stmp = s->user_link_next;
				remove_service(s);
				free_service(s);
				s = stmp;
			} else {
				s = s->user_link_next;
			}
		}

		/*
		 *  If the user has closed the connection and there are
		 *  no service requests outstanding, free structure.
		 */
		if ((u->service == 0) && (u->fd == -1)) {
			/*
			 *  Save a temparary pointer to the next
			 *  user structure before freeing.
			 */
			utmp = u->next;
			remove_user(u);
			free_user(u);
			u = utmp;
		} else {
			u = u->next;
		}
	}
}
