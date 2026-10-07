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
 *      $Id: nx_setup.c,v 1.12 1995/03/14 23:35:57 sdh Exp $
 *
 * HISTORY
 * $Log: nx_setup.c,v $
 * Revision 1.12  1995/03/14  23:35:57  sdh
 * Added code to ignore SIGPIPE when doing the write to the
 * pipe. This allows the program to receive an EPIPE error
 * if the write fails and do a printf before exiting.
 *
 *
 *  Reviewer: hfcheng
 *  Risk: low
 *  Benefit or PTS #: see description
 *  Testing:
 * 	EATS: sched, rmcall, rmcmd, controlc
 * 	manual testing
 *  Module(s):
 * 	nx_setup.c
 *
 * Revision 1.11  1995/02/15  22:50:12  raysx
 *  By backing out the previous check in (11651) the problem with nfs is
 *  resolved.  There seems to be a memmory corruption problem with libnx
 *  and 11651 caused it to manifest itself in a different place.  11651
 *  is now re-opened and 12227 is used to put things back the way they were.
 *
 *  Reviewer:
 *  Risk: Low
 *  Benefit or PTS #: 12227
 *  Testing: Ran nfs daemons with multiple interfaces configured.
 *  Module(s): usr/ccs/lib/common_nx_c/nx_setup.c
 *
 * Revision 1.10  1995/01/17  17:16:03  terry
 * This removes some dead code that has remained around in execve().  This
 * code tries to let exec program inherit the PTYPE during exec's in the
 * compute partition.  This code also has a side effect of causing memory
 * corruptions in the child program.
 *
 *  Reviewer: joel and tnt
 *  Risk: low
 *  Benefit or PTS #: 11651
 *  Testing: ran test causes
 *  Module(s): ../usr/ccs/lib/common_nx_c/nx_setup.c
 * 		    ../usr/ccs/lib/common_nx_c/execve.c
 *
 * Revision 1.9  1994/12/16  23:45:39  sdh
 * Added code to pass back a pid to the parent task.
 *
 *  Reviewer: hobbes
 *  Risk: Low/medium
 *  Benefit or PTS #: 11164, 11955, 11663
 *  Testing:
 * 	EATS: controlc, rmcall, rmcmd, sched
 * 	manual tests
 *  Module(s):
 * 	cmds_libs/src/usr/ccs/lib/libnx/nx_loadve.c
 *         cmds_libs/src/usr/ccs/lib/libnx/autoinit.c
 *         cmds_libs/src/usr/ccs/lib/common_nx_c/nx_setup.c
 *         cmds_libs/src/usr/include/nx/h.h
 *
 * Revision 1.8  1994/11/19  01:56:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/10/05  14:59:14  sdh
 * Changed code to call _mynode instead of mynode. This allows
 * non-nx applications to link since _mynode is defined in both
 * libc and libnx.
 *
 *  Reviewer:
 *  Risk: low
 *  Benefit or PTS #: 11083 related
 *  Testing: manual
 *  Module(s):
 * 	nx_setup.c
 *
 * Revision 1.6  1994/09/28  22:21:33  sdh
 * Added code to pick up the pipe fd from the arg list and
 * use it send status back to the parent process doing the
 * nx_loadve.
 *
 *  Reviewer: joel, nandy
 *  Risk: medium
 *  Benefit or PTS #: 11083
 *  Testing:
 * 	EATS: controlc, rmcall, rmcmd
 * 	manual tests
 *  Module(s):
 * 	usr/ccs/lib/libnx/nx_loadve.c
 * 	usr/ccs/lib/common_nx_c/nx_setup.c
 * 	usr/include/nx/h.h
 *
 * Revision 1.5  1992/09/19  23:08:52  cameron
 * Fixed problem with NULL argv parameter which would cause odd behaviour
 * such as getting errors returned from mynode() in child process. Needed
 * to check for argc < 2 rather than < 3.
 *
 * Revision 1.4  92/09/18  19:17:20  joel
 * changes to fix PTS 3150.  Ptypes will be inherit across exec() calls via
 * the special2 flag.
 * 
 * Revision 1.3  92/06/25  12:03:38  nandy
 * vanilla unix pgms with argc < 3 will now not try the strcmp()
 * 
 * Revision 1.2  92/06/10  16:49:17  stans
 * define special for a clean compile
 * 
 * Revision 1.1  92/06/10  16:42:43  stans
 * Initial revision
 * 
 *
 */
#include <sys/types.h>
#include <nx/h.h>
#include <signal.h>

/*
 * Called from crt0.c to setup the world for parallel applications, it isolates
 * all of the NX wierdness in one function to keep crt0.c less complicated.
 */
int
nx_setup( argc, argv, envp )
int     *argc;
char    *argv[];
char    *envp[];
{
        static char special[] = { '\007', '\0' }; /* get ptype from nx_loadve*/
        static char special2[] = { '\006', '\0' };/* inherit ptype from exec()*/
	long sp2_ptype;			/* ptype to inherit if special2 set */
	NX_LOADVE_STAT_T stat; 
	int pfd;
	extern int errno;
	struct sigaction new_sigaction;
	struct sigaction old_sigaction;

	if( *argc < 2) { 
		/*
		 * arguments must not have the ptype and the special
		 * flag set. Vanilla unix with argc < 2
		 */
		autoinit(argc, argv, envp);

	} else if ( strcmp(argv[*argc - 2], special) != 0 ) {
		/*
                 * Ptype flag is not present, this is the first executable
                 * on command line or a vanilla Unix program with argc >= 
		 * 2.
                 */

		/* 
	 	 * Check for special2 flag, if present we need to inherit
		 * ptype from parent (process that called libnx/execve() 
		 * the routine that sets special2 flag).
		 */
		if ( strcmp(argv[*argc - 2], special2) == 0 ) {
			sp2_ptype = atoi(argv[*argc - 1]);
                	*argc -= 2;
                	argv[*argc] =  (char *) 0;
                	autoinit(argc, argv, envp);
			setptype(sp2_ptype);
		} else {
                	autoinit(argc, argv, envp);
		}

        } else {
	        /*
		 * Ptype flag is present, this is not the first executable
		 * on the command line or it was exec'd by nx_load().
		 */
	         if (_setptype( atoi( argv[*argc - 1]) ) < 0) {
		    stat.errno = errno;
		 } else {
		    stat.errno = 0;
		 }
		 
		 stat.node = _mynode();
		 stat.pid = getpid();
		 /* Pick up the pipe fd from the command line.  */
		 pfd = atoi( argv[*argc - 3] );
		 /* Send status back to the parent */

		/* Ignore SIGPIPE before the write, so a write failure doesn't
		 * cause the program to get a SIGPIPE. This allows us to do 
		 * printf before exiting */
		new_sigaction.sa_handler = SIG_IGN;
		if (sigaction(SIGPIPE, &new_sigaction, &old_sigaction) == -1){
			perror("nx_setup: sigaction");
        	}

		 if (write(pfd, &stat, sizeof(stat)) < 0) {
		     printf("nx_loadve: write to parent failed from node %d errno %d\n", stat.node, errno);
		     /* write failed, so the parent doesn't get a status, so exit */
		     exit(1);
		 }

		/* reset the old action */
		if (sigaction(SIGPIPE, &old_sigaction, &old_sigaction) == -1){
			perror("nx_setup: sigaction");
		}

		 /* if an error occured, exit the process */
		 if (stat.errno) 
		     exit(1);

		close(pfd);

                *argc -= 3;
                argv[*argc] =  (char *) 0;
        }
}
