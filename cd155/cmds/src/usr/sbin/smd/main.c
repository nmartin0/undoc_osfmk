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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/main.c,v 1.5 1994/12/19 19:45:54 sdh Exp $
 *
 * HISTORY
 * $Log: main.c,v $
 * Revision 1.5  1994/12/19  19:45:54  sdh
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
 * Revision 1.4  1994/11/19  03:16:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.3.12.2  1994/04/06  19:51:28  jkearns
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
 * Revision 1.3.12.1  1994/02/23  19:00:11  jkearns
 *  Reviewer: SDSC
 *  Risk: Medium
 *  Benefit or PTS #: 7955
 *  Testing: Running at SDSC, KFA
 *  Module(s): init.c, main.c, user_db.c
 *
 * Revision 1.3  1993/01/19  20:12:52  rkl
 * Added timeout to select() so alarms will be updated when the allocator
 * optimizes events to SMD.
 *
 * Revision 1.2  1992/10/29  17:22:40  rkl
 * Added time stamp to "select" trace message and added debug safety check code
 * on user database search loop.
 *
 * Revision 1.1  1992/10/05  23:30:26  rkl
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <signal.h>
#include <sys/errno.h>
#include <sys/time.h>
#include "defs.h"

/*
 *  getopt(3) externals
 */
extern	char	*optarg;
extern	char	optopt;

/*
 *  main:
 *
 */
main(argc, argv)
	int	argc;
	char	**argv;
{
	extern	void	cleanup();

	int	opt;
	int     pid_tmp, fd;

	/*
	 *  Process arguments.
	 */
	while ((opt = getopt(argc, argv, "d:t:")) != EOF) {
		switch (opt) {
			case 'd':
				debug_level = strtol(optarg, 0, 0);
				break;
			case 't':
				timer_interval = strtol(optarg, 0, 0);
				break;
			case '?':
			default:
				usage(*argv);
				exit (1);
				break;
		}
	}
	
	errno = 0;

	pid_tmp = fork();
	if (pid_tmp) {               /* kill the parent and let the child live */
	   if (errno) {
	      (void) perror("fork failed : ");
	   }
	   exit(0); 
	}
	/*
	 * close stdin, but not stdout and stderr because they become the
	 * logfile.
	 */

	(void) close(0);             /* close stdin */

	/* take up the fd 0 so that subsequent open will skip fd 0 */

	fd = open("/dev/null", 1);


	/*
	 *  Initialize ourself, contact the Allocator and let it know we
	 *  want scheduler events, and service requests.
	 */
	init_smd();
	notify_allocator();
	service_loop();

	/*
	 *  We get here only if something bad happened in service_loop().
	 */
	cleanup();
	return (1);	/* for lint */
}

/*
 *  service_loop:
 *
 *	This is the main service loop for SMD.  It will wait for a
 *	selected FD to become active then act on the service request.
 */
service_loop()
{
	fd_set		tmpmask;
	int		reqs;
	smd_user_t	*user;
	time_t		target;
	struct timeval	select_timeout;

	TRACE(ENTRY, ("service_loop()\n"));

	/*
	 *  Service requests forever.
	 */
	target = time(0) + timer_interval;

	while (1) {
		TRACE(VERBOSE, ("\n"));

		/*
		 *  Set a new select timer.
		 */
		select_timeout.tv_sec  = target - time(0);
		select_timeout.tv_usec = 0;
		if (select_timeout.tv_sec <= 0 ) {
			target = time(0) + timer_interval;
			select_timeout.tv_sec = timer_interval;
		}
		TRACE(VERBOSE, ("select timeout=%d\n", select_timeout.tv_sec));

		/*
		 *  Make a copy of the current mask then do the select.
		 */
		bcopy(&readmask, &tmpmask, sizeof(readmask));

		/*
		 *  Wait for something to happen.
		 */
		reqs = select(	max_fds, &tmpmask,
				(fd_set*)0,
				(fd_set*)0,
				&select_timeout);
		if (reqs < 0) {
			if (errno == EINTR) {
				TRACE(VERBOSE, ("select interrupted\n"));
				continue;
			} else {
				perror("smd_service - select()");
				return;
			}
		}
		TRACE(VERBOSE, ("select returned %d at %d\n", reqs, time(0)));

		/*
		 *  Check for select timeout.
		 */
		if (reqs == 0) {
			timer_update();
			target = time(0) + timer_interval;
			continue;
		}

		/*
		 *  See if this is a new user.
		 */
		if (FD_ISSET(req_fd, &tmpmask)) {
			reqs--;
			add_user();
		}

		/*
		 *  Service requests.
		 */
#ifdef SAFETY_CHECK
		{
		int safety;
		safety = 0;
#endif
	        for (user = user_db; user && reqs;) {
	        	struct user     *tmpnext;
			tmpnext = user->next;
			/* function service_req() can free pointer user! */

			if ((user->fd != -1) && FD_ISSET(user->fd, &tmpmask)) {
				reqs--;
				service_req(user);
			}
#ifdef SAFETY_CHECK
			if (safety++ > 100) {
				fprintf(stderr, "saftey loop exceeded!!\n");
				break;
			}
#endif
			user = tmpnext;
		}
#ifdef SAFETY_CHECK
		}
#endif

		/*
		 *  Sanity check.
		 */
		if (reqs)
			fprintf(stderr, "Couldn't find user for selected fd\n");
	}
}

/*
 *  service_req:
 *
 *	Read request and initiate service.
 */
service_req(user)
	smd_user_t	*user;
{
	TRACE(ENTRY, ("service_req(user=%x)\n", user));
	
	/*
	 *  Read the request and check for errors and closed connections.
	 */
	if (read_msg(user, (smd_hdr_t*)&msg, sizeof(msg)) < 0) {
		TRACE(USERS, ("connection closed for user %x\n", user));
		TRACE(TIMINGS, ("%d CLOSEUSR %x\n", time(0), user));
		close_user(user);
	} else {
		TRACE(REQUESTS, ("service req %d from user %x\n",
						msg.req.hdr.type, user));
		if (msg.req.hdr.type > 29) {
			TRACE(TIMINGS, ("%d ALOCATR1 %x %d %d\n", time(0), 
				&msg, msg.req.hdr.type, msg.req.hdr.len));
			TRACE(TIMINGS, ("%d ALOCATR2 %d %d %d %d\n", time(0), 
				msg.ind.data.pgid, msg.ind.data.part_id, 
				msg.ind.data.acct_id, msg.ind.data.uid));
			TRACE(TIMINGS, ("%d ALOCATR3 %d %d %d %d %d %d %f\n", 
				time(0), msg.ind.data.app_rollin, 
				msg.ind.data.rollin_quantum, 
				msg.ind.data.part_rollin, msg.ind.data.app_size,
			 	msg.ind.data.app_priority, 
				msg.ind.data.part_size,
			 	msg.ind.data.global_time));
			TRACE(TIMINGS, ("%d ALOCATR4 %d\n", time(0), 
				msg.ind.data.event_time));
		}
		else {
			TRACE(TIMINGS, ("%d REQUEST1 %x %d %d\n", time(0), 
				&msg, msg.req.hdr.type, msg.req.hdr.len));
			TRACE(TIMINGS, ("%d REQUEST2 %d %d %d %d %s\n", time(0), 
				msg.req.q_id.pgid, msg.req.q_id.part_id, 
				msg.req.q_id.acct_id, msg.req.q_id.uid, 
				msg.req.q_id.req_id));
			TRACE(TIMINGS, ("%d REQUEST3 %x %d %d %x %d %x\n", 
				time(0), msg.req.param.flags, 
				msg.req.param.sig_num, 
				msg.req.param.sig_pid, msg.req.param.events, 
				msg.req.param.alarm_val, msg.req.param.service));
		}
		switch (msg.req.hdr.type) {
			case SMD_EVENT_REQ:
				register_event(user, (smd_req_t*) &msg);
				break;

			case SMD_SET_APP_ALARM_REQ:
				register_alarm(user, (smd_req_t*) &msg);
				break;

#ifdef not_now
			case SMD_SET_PART_AVAIL_ALARM_REQ:
			case SMD_SET_PART_ACTIVE_ALARM_REQ:
			case SMD_SET_PART_IDLE_ALARM_REQ:
				part_alarm(user, (smd_req_t*) &msg);
				break;

			case SMD_MON_PARTITION_USAGE_REQ:
				mon_part_usage(user, (smd_req_t*) &msg);
				break;

			case SMD_GET_PARTITION_USAGE_REQ:
				get_part_usage(user, (smd_req_t*) &msg);
				break;
#endif

			case SMD_GET_STATUS_REQ:
				get_status(user, (smd_req_t*) &msg);
				break;

			case SMD_CANCEL_REQ:
				cancel(user, (smd_req_t*) &msg);
				break;

			case ALLOCATOR_START_IND:
				app_event((allocator_ind_t*)&msg,
						APP_START, SMD_APP_START_IND);
				break;

			case ALLOCATOR_ROLLIN_IND:
				app_event((allocator_ind_t*)&msg,
						APP_ROLLIN, SMD_APP_ROLLIN_IND);
				break;

			case ALLOCATOR_ROLLOUT_IND:
				app_event((allocator_ind_t*)&msg,
					      APP_ROLLOUT, SMD_APP_ROLLOUT_IND);
				break;

			case ALLOCATOR_END_IND: {
				extern	smd_app_t* find_app();
					smd_app_t  *app;

				app_event((allocator_ind_t*)&msg,
						APP_END, SMD_APP_END_IND);
				/*
				 *  Find and remove the application.  If not
				 *  found it was probably a redundant
				 *  indication from the scheduler.
				 */
				if (app = find_app(msg.ind.data.pgid)) {
					remove_app(app);
					free_app(app);
				}
				break;
			}

			default:
				fprintf(stderr, "Unknow SMD service type %d\n",
							msg.req.hdr.type);
		}
	}
}

/*
 *  cleanup:
 *
 *	This routine will cleanup before exiting.
 */
void
cleanup()
{
	/*
	 *  Remove interface
	 */
	close(req_fd);
	unlink(iface_name);
	exit(1);
}

/*
 *  SignalHandler
 *
 */
void
SignalHandler(recv_sig)
   int             recv_sig;
{
   (void) signal(recv_sig, SignalHandler);
   switch (recv_sig) {
	case SIGHUP  :
	case SIGQUIT :
	case SIGINT  :
	case SIGXCPU :
	case SIGXFSZ :
	case SIGLOST : 
		TRACE(VERBOSE, ("%d   smd received signal %d\n", time(0), recv_sig));
		break;
	case SIGBUS  : 
	case SIGTERM : 
	case SIGSEGV :
	case SIGSYS  :
	case SIGURG  :
	case SIGTSTP :
   	default      : 
		TRACE(VERBOSE, ("EXITING because of signal %d\n", recv_sig));
		cleanup();
   }
}

/*
 *  usage:
 *
 *		Print usage and exit.
 */
usage(me)
	char	*me;
{
	printf("Usage: %s [-d level] [-t interval]\n", me);
	exit (1);
}
