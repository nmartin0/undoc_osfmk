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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/app_db.c,v 1.6 1994/12/19 19:45:35 sdh Exp $
 *
 * HISTORY
 * $Log: app_db.c,v $
 * Revision 1.6  1994/12/19  19:45:35  sdh
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
 * Revision 1.5  1994/11/19  03:16:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.4.4.1  1994/04/06  19:51:19  jkearns
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
 * Revision 1.4  1993/07/18  19:55:17  carbajal
 * Changed printing of global time from %d to %f
 *
 * Revision 1.3  1993/04/16  21:27:33  carbajal
 * Corrected link list maintainence in remove_app.
 *
 * Revision 1.2  1993/01/19  20:12:40  rkl
 * Added timeout to select() so alarms will be updated when the allocator
 * optimizes events to SMD.
 *
 * Revision 1.1  1992/10/05  23:30:24  rkl
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <time.h>
#include "defs.h"

/*
 *  alloc_app:
 *
 *	Allocate an smd_app_t structure.
 */
smd_app_t*
alloc_app()
{
	smd_app_t	*app;

	TRACE(ENTRY, ("alloc_app()\n"));

	app = (smd_app_t*) malloc(sizeof(smd_app_t));
	if (app == 0)
		fprintf(stderr, "SMD: No memory for application structure\n");

	/*
	 *  Put things in a know state.
	 */
	app->state           = APP_STATE_UKNOWN;
	app->data.event_time = time(0);
	return (app);
}

/*
 *  free_app:
 *
 *	Free an smd_app structure.
 */
free_app(app)
	smd_app_t	*app;
{
	TRACE(ENTRY, ("free_app(app=%x)\n", app));
	TRACE(VERBOSE, ("freeing application structure %x\n", app));

	free(app);
}

/*
 *  find_app:
 *
 *	Search the application database for this applications entry.
 */
smd_app_t*
find_app(pgid)
	pid_t	pgid;
{
	smd_app_t  *app;

	TRACE(ENTRY, ("find_app(pgid=%x)\n", pgid));

	for (app = a_table[ A_HASH(pgid) ]; app; app = app->next)
		if (app->data.pgid == pgid)
			break;
	return (app);
}

/*
 *  app_entries:
 *
 *	Count the number of application database entries.
 */
app_entries()
{
	smd_app_t	*app;
	int		i;

	TRACE(ENTRY, ("app_entries()\n"));

	i = 0;
	for (app = a_table[ 0 ]; app; app = app->next)
		i++;

	TRACE(VERBOSE, ("app_entries = %d\n", i));
	return (i);
}

/*
 *  add_app:
 *
 *	Add an smd_app_t structure to the database.
 */
add_app(app)
	smd_app_t  *app;
{
	smd_app_t  **a;

	TRACE(ENTRY, ("add_app(app=%x)\n", app));

	a = a_table + A_HASH(app->data.pgid);
	app->next = *a;
	app->prev = 0;
	if (*a)
		(*a)->prev = app;
	*a = app;

	total_apps++;
TRACE(TIMINGS, ("%d ADDAPP %x\n", time(0), app));
trace_apps();

}

/*
 *  remove_app:
 *
 *	Remove an smd_app_t structure from the database.
 */
remove_app(app)
	smd_app_t  *app;
{
	TRACE(ENTRY, ("remove_app(app=%x)\n", app));
	TRACE(VERBOSE, ("removing applicaition %x\n", app));

	/*
	 *  Unlink the application table.
	 */
	if (app->prev)
		app->prev->next = app->next;
	else
		a_table[ A_HASH(app->data.pgid) ] = app->next;
	
	if (app->next)
		app->next->prev = app->prev;

	total_apps--;
TRACE(TIMINGS, ("%d DELAPP %x\n", time(0), app));
trace_apps();
}

/*
 *  update_app:
 *
 *	In order to satisfy SMD_GET_STATUS_REQ services, we need to
 *	keep information on all applications.  This routine updates
 *	the application database.  If this application hasn't been
 *	seen before and it's not an ALLOCATOR_END_IND message (the
 *	scheduler can send several of these when an application
 *	dies), it is added to the database.
 */
update_app(ind)
	allocator_ind_t	*ind;
{
	smd_app_t	*app;

	TRACE(ENTRY, ("update_app(ind=%x)\n", ind));

	/*
	 *  Find or allocate the application structure and
	 *  copy the incoming data.
	 */
	if (app = find_app(ind->data.pgid)) {
		app->data = ind->data;		/* structure copy */
	} else if ((ind->hdr.type != ALLOCATOR_END_IND) && (app = alloc_app())){
		app->data = ind->data;		/* structure copy */
		add_app(app);
	}
	app->state = ind->hdr.type;
	
	TRACE(DUMP_SCHED_EVENT, ("\
SCHED_EVENT pgid = %d acct_id = %d uid = %d app_size = %d\n\
	    app_rollin = %d app_priority = %d part_id = %d\n\
	    part_size = %d rollin_quantum = %d part_rollin = %d\n\
	    global_time = %f event_time = %d\n",
	ind->data.pgid,        ind->data.acct_id,     ind->data.uid,
	ind->data.app_size,    ind->data.app_rollin,  ind->data.app_priority,
	ind->data.part_id,     ind->data.part_size,   ind->data.rollin_quantum,
	ind->data.part_rollin, ind->data.global_time, ind->data.event_time));
}

/*
 *  timer_update:
 *
 *	Update rollin time of all applications that have been started
 *	and/or rolled in.  After updating the rollin time, we construct
 *	a fake Allocator indication to see if there are alarm service
 *	responses that need to be sent to the user.
 */
static	allocator_ind_t	ind;

#define	UPDATE_TIME(a,t) \
    a->data.app_rollin += (double)((double)(t - a->data.event_time) * 1000.0)

timer_update()
{
	smd_app_t	*app;
	int		i;
	time_t		cur_time = time(0);

	TRACE(ENTRY, ("timer_update()\n"));

	for (i = 0; i < A_HASH_SIZE; i++) {
		for (app = a_table[ i ]; app; app = app->next) {
			/*
			 *  If this is an active application, construct
			 *  a fake allocator indication and send it through
			 *  the do_service_list() function to look for
			 *  expired timer.
			 */
			if ((app->state == ALLOCATOR_START_IND) ||
			    (app->state == ALLOCATOR_ROLLIN_IND)) {
				UPDATE_TIME(app, cur_time);
				app->data.event_time = cur_time;
				ind.hdr.type = app->state;
				ind.data     = app->data;  /* structure copy */
				do_service_list(&ind, 0, -1);
			}
		}
	}

}
