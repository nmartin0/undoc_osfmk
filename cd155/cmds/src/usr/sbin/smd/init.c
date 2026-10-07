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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/init.c,v 1.4 1994/12/19 19:45:47 sdh Exp $
 *
 * HISTORY
 * $Log: init.c,v $
 * Revision 1.4  1994/12/19  19:45:47  sdh
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
 * Revision 1.3  1994/11/19  03:16:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.2.12.2  1994/06/03  21:10:07  sdh
 * Added a call to ignore SIGPIPE. Now when we write to a socket that has been
 * closed on the other side, we will get and EPIPE instaed of a SIGPIPE signal.
 *
 *  Reviewer:
 *  Risk: low
 *  Benefit or PTS #: 9488
 *  Testing: manual tests, EATS: nqs, sched
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/smd/init.c
 *
 * Revision 1.2.12.1  1994/02/23  19:00:07  jkearns
 *  Reviewer: SDSC
 *  Risk: Medium
 *  Benefit or PTS #: 7955
 *  Testing: Running at SDSC, KFA
 *  Module(s): init.c, main.c, user_db.c
 *
 * Revision 1.2  1993/01/19  20:12:48  rkl
 * Added timeout to select() so alarms will be updated when the allocator
 * optimizes events to SMD.
 *
 * Revision 1.1  1992/10/05  23:30:25  rkl
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <signal.h>
#include "defs.h"

/*
 *  smd_init:
 *
 *	Initialize SMD interfaces and structures.
 */
init_smd()
{
	/*
	 *  Put all output to stderr.
	 */
	dup2(2, 1);

	TRACE(ENTRY, ("init_smd()\n"));

	/*
	 *  Initialize internal data structures.
	 */
	init_data();

	/*
	 *  Initialize signal handling
	 */
	init_signals();

	/*
	 *  Initialize service request interface.
	 */
	init_service_iface();
}

/*
 *  init_signals:
 *
 *	Catch signals in order to clean up.
 */
init_signals()
{
	extern	void	cleanup();
	extern  void    SignalHandler();
#ifdef DUMPS
	extern	void	dump_users();
	extern	void	dump_parts();
	extern	void	dump_apps();
#endif
	/* Ignore sigpipe */
	(void) signal(SIGPIPE, SIG_IGN);

	(void) signal(SIGTERM, SignalHandler);
	(void) signal(SIGHUP,  SignalHandler);
	(void) signal(SIGQUIT, SignalHandler);
	(void) signal(SIGINT,  SignalHandler);
	(void) signal(SIGBUS,  SignalHandler);
	(void) signal(SIGSEGV, SignalHandler);
	(void) signal(SIGSYS,  SignalHandler);
	(void) signal(SIGURG,  SignalHandler);
	(void) signal(SIGTSTP, SignalHandler);
	(void) signal(SIGXCPU, SignalHandler);
	(void) signal(SIGXFSZ, SignalHandler);
	(void) signal(SIGLOST, SignalHandler);
#ifdef DUMPS
	signal(SIGXCPU, dump_users);
	signal(SIGXFSZ, dump_parts);
	signal(SIGLOST, dump_apps);
#endif
}

/*
 *  init_data:
 *
 *	Initialize data structures.
 */
init_data()
{
	TRACE(ENTRY, ("init_data()\n"));

	iface_name = SMD_IFACE_NAME;	/* service request path name */
	max_fds = getdtablesize();	/* find out what are FD limit is */
	user_db = 0;
	total_apps = 0;
	if (timer_interval == 0)
		timer_interval = 10;

	TRACE(INITS, ("max FDs = %d, timer interval = %d\n",
						max_fds, timer_interval));
}

/*
 *  init_service_iface:
 *
 *	Initialize the service request interface.
 */
init_service_iface()
{
	TRACE(ENTRY, ("init_service_iface()\n"));

	/*
	 *  Initialize the socket interface.
	 */
	socket_init();
}
