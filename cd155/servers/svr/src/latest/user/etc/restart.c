/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright (c) 1993-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: restart.c,v $
 * Revision 1.3  1995/02/01  23:56:47  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.2  1994/11/18  20:53:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:48:06  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:09:46  slk
 * *** empty log message ***
 * 
 * Revision 2.1.1.6  93/08/04  10:11:50  chrisp
 * 	Change process kill loop not to check errno when kill() returns ESUCCESS.
 * 
 * Revision 2.1.1.5  93/08/03  14:14:17  chrisp
 * 	Correct condition uder which the default checkpoint directory path is ".".
 * 
 * Revision 2.1.1.4  93/07/20  09:40:59  chrisp
 * 	Add -n option to requests exec_restart()s not to stop. This lessens
 * 	the chance for exit from restart to occur when restarter processes
 * 	are still stopped and hence the entire group to receive an unwanted
 * 	CONT/HUP signal combination.
 * 	Don't disallow -f with other options.
 * 	Direct all messages to standard error; modify some message texts.
 * 
 * Revision 2.1.1.3  93/07/08  08:07:34  chrisp
 * 	If no process has been checkpointed in the given directory, print a
 * 		suitable message and exit.
 * 
 * Revision 2.1.1.2  93/06/22  12:16:26  chrisp
 * 	Correct kill loop; check errno not return value from kill().
 * 
 * Revision 2.1.1.1  93/06/10  11:54:38  chrisp
 * 	Revision 3.6  93/05/19  10:55:09  chrisp
 * 	Change the default behavior of restart to exec_restart() the root process
 * 		in the checkpointed set. Add the -p option to override this and
 * 		to fork the root process with the checkpoint pid.
 * 
 * 	Revision 3.5  93/05/11  15:37:47  hao2
 * 	Added -g option to the restart command so that restart does not
 * 	automatically setpgrp() to the chkpnted pgid.
 * 
 * 	Revision 3.4  93/04/30  13:04:29  chrisp
 * 	Add the -e option to directly exec_restart() a single process checkpoint
 * 		image (and then stop if -s option is also given).
 * 
 * 	Revision 3.3  93/04/30  08:45:02  chrisp
 * 	Remove the -s (stop) option and add -w (wait) so that the restart command
 * 		may be reqeuuested to wait for restarted child processes.
 * 
 * 	Revision 3.2  93/04/29  08:28:54  chrisp
 * 	Add debug option.
 * 
 * 	Revision 3.1  93/04/22  08:57:11  chrisp
 * 	Changes for new interface to chkpnt_getprocinfo() returning malloc'ed
 * 		arrays.
 * 
 * 	Revision 3.0  93/04/17  13:05:39  chrisp
 * 	Start of restart.
 * 	/
 * 	$EndLog$
 * 
 */
#include <stdio.h>
#include <string.h>
#include <mach/boolean.h>
#include <tnc/chkpnt.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/access.h>
#include <sys/mode.h>
#include <uxkern/bsd_types.h>
#include <tnc/chkpnt.h>

/*
 * This implements the restart command:
 *	restart [options] dir
 * where options are:
 *	-d	debug:	print messages at various stages during restart;
 *	-e	exec:	if dir contains a root process, exec_restart directly;
 *	-f	force:	if required pids are allocated, kill first; 
 *	-g	group:	inherit the restarter's process group; 
 *	-n	nostop:	don't stop processes during exec_restart;
 *	-p	pids:	re-create original processes, don't exec_restart();
 *	-s	stop:	if used with -s, remain stopped after exec_restart();
 *	-w	wait;	if not used with -e, wait for all children to exit.
 */
main(int argc, char *argv[])
{
	int		error = 0;
	extern char	*optarg;
	extern int	optind;
	char		ch;
	char		*my_name;
	char		*last_slash;
	char		*chkpnt_dir = NULL;
	char		chkpnt_dirpath[256];
	boolean_t	nest = FALSE;
	boolean_t	nosetpgid = FALSE;
	boolean_t	exec = TRUE;
	boolean_t	noexecstop = FALSE;
	boolean_t	stop = FALSE;
	boolean_t	force = FALSE;
	boolean_t	debug = FALSE;
	struct stat	stat_buf;
	int		nproc;
	int		i;
	pid_t		id;
	pid_t		pgid;
	pid_t		*pid_list;
	pid_t		*ppid_list;
	node_t		*node_list;

	last_slash = strrchr(argv[0], '/');
	if (last_slash == NULL)
		my_name = argv[0];
	else
		my_name = last_slash + 1;

	while ((ch = getopt(argc, argv, "defgnpsw")) != EOF) {
		switch (ch) {
#ifdef	TNC_CHKPNT_DEBUG
		case 'd':
			debug++;
			break;
#endif
		case 'e':
			break;
		case 'f':
			force++;
			break;
		case 'g':
			nosetpgid++;
			break;
		case 'n':
			noexecstop++;
			break;
		case 'p':
			exec = FALSE;
			break;
		case 's':
			stop++;
			break;
		case 'w':
			nest++;
			break;
		case '?':
		default:
			error++;
		}
	}
	if (nest && exec || nest && stop || nosetpgid && exec)
		error++;
	if (stop && noexecstop)
		error++;
	if (error) {
		fprintf(stderr,
			"usage: %s [-e [-s] | -p [-n|-w]] [-g] [-f] [dir]\n",
			my_name);
		exit(1);
	}

	sprintf(chkpnt_dirpath, "%s", (optind < argc) ? argv[optind] : ".");
	if (*chkpnt_dirpath != '/') {
		/*
		 * Have a relative pathname which needs conversion
		 * into an absolute path - so prepend our current
		 * working directory.
		 */
		path_name_t	cwd;
		path_name_t	rel_dir;
		(void) getcwd(cwd, sizeof(cwd));
		strcpy(rel_dir, chkpnt_dirpath);
		sprintf(chkpnt_dirpath, "%s/%s", cwd, rel_dir);
	}

	error = stat(chkpnt_dirpath, &stat_buf);
	if (error != ESUCCESS || (stat_buf.st_mode & S_IFMT) != S_IFDIR) {
		fprintf(stderr, "cannot find checkpoint directory %s\n",
			chkpnt_dirpath);
		exit(1);
	}

	error = chkpnt_getprocinfo(chkpnt_dirpath,
				   &pgid,
				   &nproc,
				   &pid_list,
				   &ppid_list,
				   &node_list);
	if (error != ESUCCESS) {
		perror("restart failed");
		exit(1);
	}
	if (nproc == 0) {
		fprintf(stderr, "no process checkpointed in directory %s\n",
			chkpnt_dirpath);
		exit(1);
	}


	/*
	 * Check that the required pids are not already allocated.
	 * If required, attempt to kill existing processes.
	 */
	for (i = 0; i < nproc; i++) {
		error = kill(pid_list[i], 0);
		if (error == ESUCCESS) {
			if (force) {
				int timeout = 5;
				fprintf(stderr, "restart: killing process %d\n",
				        pid_list[i]);
				error = kill(pid_list[i], SIGKILL);
				if (error != ESUCCESS) {
					perror("kill failed");
					exit(1);
				}
				while (kill(pid_list[i], 0) == ESUCCESS) {
					if (timeout > 0) {
						timeout--;
						sleep(1);
						continue;
					}
					fprintf(stderr, "restart: "
							"process not killed\n");
					exit(1);
				}
			} else {
				fprintf(stderr,
					"restart: process %d already exists\n",
					pid_list[i]);
				exit(1);	
			}
		}
	}

	/*
	 * Attempt the restart.
	 * Note that unless overridden by the -p option, the default
	 * behavior is for restart() to exec_restart() the root process
	 * (if such exists) and hence not to return here.
	 */
	error = restart(chkpnt_dirpath,
			&id,
			(stop ? 0 : RESTART_SIGALL) |
				(exec ? RESTART_EXECROOT : 0) |
				(debug ? RESTART_DEBUG : 0) |
				(noexecstop ? RESTART_NOSTOP : 0) |
				(nosetpgid ? RESTART_NOSETPGID : 0));

	if (error > 0) {
		if (id > 0)
			fprintf(stderr, "restart: process id %d\n",id);
		else
			fprintf(stderr,
				"restart: process group id %d (%d processes)\n",
				-id, error);
	} else {
		perror("restart failed");
		exit(1);
	}

	if (nest) {
		fprintf(stderr, "restart: waiting for all children to exit\n");
		do
			(void) wait(NULL);
		while (errno != ECHILD);
		fprintf(stderr, "restart: all children exited\n");
	}

	exit(0);
}

