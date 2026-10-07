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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/response.c,v 1.5 1995/02/10 23:44:56 carolr Exp $
 *
 * HISTORY
 * $Log: response.c,v $
 * Revision 1.5  1995/02/10  23:44:56  carolr
 * Removed an extra format character (%s) from a DEBUG printf string.
 *
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: 12318
 *  Testing: controlc, rmcall, rmcmd, sched, manual tests
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/smd/response.c
 *
 * Revision 1.4  1994/12/19  19:45:59  sdh
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
 * Revision 1.3  1994/11/19  03:16:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.2.4.1  1994/04/06  19:51:30  jkearns
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
 * Revision 1.2  1993/07/18  19:56:32  carbajal
 * Changed global time
 *
 * Revision 1.1  1992/10/05  23:30:26  rkl
 * Initial revision
 *
 *
 */

#include "defs.h"

/*
 *  send_resp:
 *
 *	Send a response/indication to the user.
 */
send_resp(user, resp, type)
	smd_user_t	*user;
	smd_resp_t	*resp;
	int		type;
{
	smd_hdr_t	hdr;

	TRACE(ENTRY, ("send_resp(user=%x, resp=%x)\n", user, resp));

	hdr.type = type;
	hdr.len  = sizeof(smd_resp_t);
	send_msg(user, (char*) &hdr, sizeof(hdr));
	send_msg(user, (char*) resp, hdr.len);

	TRACE(DUMP_USER_RESPONSE, ("\
RESP_DUMP pgid = %d part_id = %d acct_id = %d uid = %d req_id = %s\n\
          status = %d global_time = %f event_time = %d\n\
	  cpu_time = %d app_size = %d app_priority = %d part_size = %d\n",
		resp->q_id.pgid,          resp->q_id.part_id,
		resp->q_id.acct_id,       resp->q_id.uid,
		resp->q_id.req_id,        resp->value.status,
		resp->value.global_time,  resp->value.event_time,
		resp->value.cpu_time,     resp->value.app_size,
		resp->value.app_priority, resp->value.part_size));
	TRACE(TIMINGS, ("%d RESPONS1 %d %d %d %d %s\n", time(0), 
		resp->q_id.pgid,          resp->q_id.part_id,
                resp->q_id.acct_id,       resp->q_id.uid,
		resp->q_id.req_id));
	TRACE(TIMINGS, ("%d RESPONS2 %d %d %d %d %d %d \n", time(0),
		resp->value.status,	  resp->value.event_time,
		resp->value.cpu_time,     resp->value.app_size,
                resp->value.app_priority, resp->value.part_size));

}

/*
 *  send_error_resp:
 */
send_error_resp(user, req, type, error)
	smd_user_t	*user;
	smd_req_t	*req;
	int		type;
	int		error;
{
	smd_resp_t	resp;

	TRACE(ENTRY, ("send_resp(user=%x, req=%x, error=%d)\n",user,req,error));

	resp.q_id                = req->q_id;		/* structure copy */
	resp.value.status        = error;
	resp.value.global_time   = get_global_time();
	resp.value.event_time    = (time_t) time((time_t*)0);
	resp.value.cpu_time      = -1;
#ifdef not_now
	resp.value.part_avail    = -1;
	resp.value.part_utilized = -1;
	resp.value.part_active   = -1;
	resp.value.part_idle     = -1;
	resp.value.node_type     = -1;
#endif
	resp.value.app_size      = -1;
	resp.value.app_priority  = -1;
	resp.value.part_size     = -1;

	TRACE(TIMINGS, ("%d SEND_ERROR_RESPONSE\n", time(0)));
	send_resp(user, &resp, type);
}

/*
 *  send_event_resp;
 *
 *	Send a application event response to the user.
 */
send_event_resp(serv, ind, type, status)
	smd_service_t	*serv;
	allocator_ind_t	*ind;
	int		type;
	int		status;
{
	smd_resp_t	resp;

	TRACE(ENTRY,
	       ("send_event_resp(serv=%x, ind=%d, type=%d, status=%d)\n",
						serv, ind, type, status));

	/*
	 *  Make sure the connection is still up.
	 */
	if (serv->user->fd == -1) {
		TRACE(TIMINGS, ("%d EVNT_RESPNS_ERR %d %x %d %d %d\n", 
			time(0), serv->user->fd, serv, ind, type, status));
		TRACE(VERBOSE,
		      ("send_event_resp: FD closed for user %x\n", serv->user));
		return;
	}

	/*
	 *  Fill in the qualifier.
	 */
	resp.q_id.pgid    = ind->data.pgid;
	resp.q_id.part_id = ind->data.part_id;
	resp.q_id.acct_id = ind->data.acct_id;
	resp.q_id.uid     = ind->data.uid;
	bcopy(serv->q_id.req_id, resp.q_id.req_id, sizeof(resp.q_id.req_id));

	/*
	 *  Fill in the response parameters.
	 */
	resp.value.status        = status;
	resp.value.global_time   = ind->data.global_time;
	resp.value.event_time    = ind->data.event_time;
	resp.value.cpu_time      = ind->data.app_rollin ? NODE_SECS(ind) : 0;
#ifdef not_now
	resp.value.part_avail    = -1;
	resp.value.part_utilized = -1;
	resp.value.part_active   = -1;
	resp.value.part_idle     = -1;
	resp.value.node_type     = ind->data.node_type;
#endif
	resp.value.app_size      = ind->data.app_size;
	resp.value.app_priority  = ind->data.app_priority;
	resp.value.part_size     = ind->data.part_size;

	send_resp(serv->user, &resp, type);
}

/*
 *  send_status_resp;
 *
 *	Send an application status response to the user.
 */
send_status_resp(user, resp, app, req)
	smd_user_t	*user;
	smd_app_t	*app;
	smd_resp_t	*resp;
	smd_req_t	*req;
{
	resp->q_id.pgid    = app->data.pgid;
	resp->q_id.part_id = app->data.part_id;
	resp->q_id.acct_id = app->data.acct_id;
	resp->q_id.uid     = app->data.uid;
	bcopy(req->q_id.req_id, resp->q_id.req_id, SMD_REQ_ID_LEN);

	resp->value.status       = SMD_OK;
	resp->value.global_time  = app->data.global_time;
	resp->value.event_time   = app->data.event_time;
	resp->value.cpu_time     = NODE_SECS(app);
	resp->value.app_size     = app->data.app_size;
	resp->value.app_priority = app->data.app_priority;
	resp->value.part_size    = app->data.part_size;

	TRACE(TIMINGS, ("%d SNDMSG1 %d %d %d %d %s\n", time(0), 
		resp->q_id.pgid,          resp->q_id.part_id,
                resp->q_id.acct_id,       resp->q_id.uid,
		resp->q_id.req_id));
	TRACE(TIMINGS, ("%d SNDMSG2 %d %d %d %d %d %d\n", time(0),
		resp->value.status,	  resp->value.event_time,
		resp->value.cpu_time,     resp->value.app_size,
                resp->value.app_priority, resp->value.part_size));
	send_msg(user, (char*) resp, sizeof(smd_resp_t));
}
