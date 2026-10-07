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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/allocator.c,v 1.11 1994/12/19 19:45:25 sdh Exp $
 *
 * HISTORY
 * $Log: allocator.c,v $
 * Revision 1.11  1994/12/19  19:45:25  sdh
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
 * Revision 1.10  1994/11/19  03:16:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.8.2.3  1994/04/27  18:01:26  jkearns
 * Add a new debug switch, ALARMS, to ensure that certain debug statements
 * in allocator.c are only printed when truly useful.
 *
 *  Reviewer: Scott Hahn
 *  Risk: Low
 *  Benefit or PTS #: 9142
 *  Module(s): allocator.c defs.h
 *
 * Revision 1.9  1994/04/20  23:50:42  sdh
 *  Merge of PTS #6711 from R1.2
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.8.2.2  1994/04/20  18:40:13  sdh
 *  Added a check of the partition id in the event_of_interest() routine
 *  just in case two different services hashed to the same hash bucket.
 *
 *  Reviewer: jkearns
 *  Risk: low
 *  Benefit or PTS #: 6711
 *  Testing: EATS: sched, controlc, rmcall
 * 	manual testing
 *  Module(s): allocator.c
 *
 * Revision 1.8.2.1  1994/04/06  19:51:17  jkearns
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
 * Revision 1.8  1993/11/18  20:27:22  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headrs out of export tree
 *  Testing: built on Suns and 486
 *  Module(s): Makefile  allocator.c defs.h
 *
 * Revision 1.7  1993/08/28  01:19:21  carbajal
 * Got rid of looking up the allocator port, just call
 * nx_register_daemon(). PTS #6141
 *
 * Revision 1.6  1993/01/19  20:12:36  rkl
 * Added timeout to select() so alarms will be updated when the allocator
 * optimizes events to SMD.
 *
 * Revision 1.5  1992/12/31  01:37:23  rkl
 * Fixed timing problem where SMD would try to talk to the allocator before
 * it had registered it's port.
 *
 * Revision 1.4  1992/12/17  20:14:22  shala
 * Fixed path to mach_error.h and mach_init.h.
 *
 * Revision 1.3  1992/11/17  19:36:05  rkl
 * Added nx_allocator_node() call to norma_get_nameserver_port() call
 * instead of hard coded zero.
 *
 * Revision 1.2  1992/11/13  01:06:06  shala
 * Fixed path to find mach include files.
 *
 * Revision 1.1  1992/10/05  23:30:24  rkl
 * Initial revision
 *
 *
 */
#include <stdio.h>
#include <nx/defines.h>
#include "defs.h"

/*
 *  notify_allocator:
 *
 *	Contact the allocator and indicate we which to receive
 *	scheduler events.
 */
notify_allocator()
{
	u_char		*addr;
	int		addr_len;

	TRACE(ENTRY, ("notify_allocator()\n"));

	/*
	 *  Format the socket address.
	 */
	format_sock_addr(&addr, &addr_len);

	if (nx_register_daemon(SMD_DAEMON_ID, addr, addr_len) != 0){
		perror("SMD allocator registration");
		exit(1);
	}
}

/*
 *  app_event:
 *
 *	Service scheduler events.
 */
app_event(ind, event_mask, resp)
	allocator_ind_t	*ind;
	int		event_mask;
	int		resp;
{
	TRACE(ENTRY, ("app_event(ind=%x, event_mask=%d, resp=%d)\n",
							ind, event_mask, resp));
	/*
	 *  Update application database and run through service list.
	 */
	update_app(ind);
	do_service_list(ind, event_mask, resp);
}

/*
 *  do_service_list:
 *
 *	Run through all service request list and process the event.  Internal
 *	timer events will enter here instead of app_event() to get alarm
 *	responses returned to the user.
 */
do_service_list(ind, event_mask, resp)
	allocator_ind_t	*ind;
	int		event_mask;
	int		resp;
{
	smd_service_t	*serv;

	TRACE(ENTRY, ("do_service(ind=%x, event_mask=%d, resp=%d)\n",
							ind, event_mask, resp));
	/*
	 *  Find all service requests that are interested in this event.
	 */

	/* 
	* p_table[0] is the hash line for serv's that have a part_id of
	* QUALIFY_ANY.
	*/
	for (serv = p_table[0]; serv; serv = serv->next)
		do_event(ind, serv, event_mask, resp);

	for (serv = p_table[p_hash(ind->data.part_id)]; serv; serv = serv->next)
		do_event(ind, serv, event_mask, resp);
}

/*
 *  do_event
 *
 *	Search the list for service requests that match the incoming event.
 */
do_event(ind, serv, event_mask, resp)
	allocator_ind_t	*ind;
	smd_service_t	*serv;
	int		event_mask;
	int		resp;
{
	int	remove = 0;
	int	status = SMD_OK;	/* hope for the best */
	long	tmp_nodesecs;		/* debug */

	TRACE(ENTRY, ("do_event(ind=%x, serv=%x, event_mask=%d, resp=%d)\n",
						ind, serv, event_mask, resp));

	/*
	 *  This is not of interest, return.
	 */
	if (!event_of_interest(ind, serv))
		return;

	/*
	 *  Process for type of service requested.
	 */
	switch (serv->param.service) {
		case SMD_EVENT_REQ:
		tmp_nodesecs=NODE_SECS(ind);
		TRACE(ALARMS, ("**DEBUG** %x %d %d %d %d %d %d %d\n",
			serv, serv->param.service, serv->param.events, 
			event_mask, serv->param.flags, serv->param.alarm_val, 
			tmp_nodesecs, ind->hdr.type));
			if ((serv->param.events & event_mask) == 0)
				return;
			/*
			 *  If not persistent, clear event flag.
			 */
			if (!(serv->param.flags & PERSISTENT)) {
				/*
				 *  If all requested events are complete,
				 *  remove and free service request.
				 */
				serv->param.events &= ~event_mask;
				if (serv->param.events == 0) 
					remove = 1;
			}
			break;

		case SMD_SET_APP_ALARM_REQ:

		tmp_nodesecs=NODE_SECS(ind);
		TRACE(ALARMS, ("**DEBUG - 2 ** %x %d %d %d %d %d %d %d\n",
			serv, serv->param.service, serv->param.events, 
			event_mask, serv->param.flags, serv->param.alarm_val, 
			tmp_nodesecs, ind->hdr.type));
			/*
			 *  If this is the last we're going to hear from this
			 *  application, decrement it's rollin time from the
			 *  alarm_val so that if it is a persistant alarm,
			 *  a new applciation will start where the previous
			 *  one left off.
			 */
			if (serv->param.alarm_val > tmp_nodesecs) {
				if (ind->hdr.type == ALLOCATOR_END_IND) {
					serv->param.alarm_val -= tmp_nodesecs;
					if (serv->param.flags & PERSISTENT)
						return;
					else 
						status = APP_TERMINATED;
				} else
					return;

			}
			/*
			 *  Alarm has expired.  Setup to send a response.
			 */
			resp = SMD_APP_ALARM_IND;
			remove = 1;
			break;
	}

	/*
	 *  Send event indication.
	 */
	send_event_resp(serv, ind, resp, status);
	if (serv->param.flags & SEND_SIGNAL) {
		if (kill(serv->param.sig_pid, serv->param.sig_num) < 0) {
 			TRACE(TIMINGS, ("%d KILLFAIL\n", time(0)));
			perror("Kill Failed : ");

		} else 
 			TRACE(TIMINGS, ("%d SNDALARM %x %d %d\n", 
				time(0), serv, serv->param.sig_pid, 
				serv->param.sig_num));
	}

	
	/*
	 *  Remove the service is so indicated.
	 */
	if (remove)
		remove_service(serv);
}

/*
 *  event_of_interest:
 *
 *	See if the scheduler event applies to this service request.
 */
event_of_interest(ind, serv)
	allocator_ind_t	*ind;
	smd_service_t	*serv;
{
	TRACE(ENTRY, ("event_of_interest(ind=%x, serv=%x)\n", ind, serv));

	/* 
	* Even though we have hashed on the part_id, we need to check
	* it here just in case more than one part_id hashed to the same
	* bucket.
	*/
	if (((serv->q_id.pgid    == QUALIFY_ANY) ||
	     (serv->q_id.pgid    == ind->data.pgid))	&&
            ((serv->q_id.part_id == QUALIFY_ANY) ||
             (serv->q_id.part_id == ind->data.part_id))  &&
	    ((serv->q_id.acct_id == QUALIFY_ANY) ||
	     (serv->q_id.acct_id == ind->data.acct_id))	&&
	    ((serv->q_id.uid     == QUALIFY_ANY) ||
	     (serv->q_id.uid     == ind->data.uid))) {
		TRACE(VERBOSE, ("event IS of interest\n"));
		return (1);
	} else {
		TRACE(VERBOSE, ("event is NOT of interest\n"));
		return (0);
	}
}
