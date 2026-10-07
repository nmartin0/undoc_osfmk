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
 *		Copyright (c) Locus Computing, 1991-92
 * 		This is UNPUBLISHED source code that is
 * 		the property of Locus Computing, containing
 *		proprietary secrets of LCC.  Any disclosure
 *		is strictly prohibited.  Locus makes no warantee,
 *		explicit or implicit, on the functionality of this code.
 */
/*
 * HISTORY
 * $Log: exec_restart.c,v $
 * Revision 1.2  1994/11/18  21:06:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:53:32  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:15:37  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:12:51  hao
 * 	Initial check-in.
 * 
 */

#include <errno.h>
#include <stdio.h>
#include <tnc/chkpnt.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include "../common/vstnc.h"

/*
 * Number of tests.
 */
int ntests = 3;

#define	CHKPNT_EINVAL -1

#define DEBUG 0
#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

int do_test(int, pid_t);
int check_data_file(pid_t);

int int_err = FALSE;

char casedescript[MAXBUF]; /* desctiption of this testcase  */

char *myname;
main(argc, argv, envp)
int argc;
char *argv[], *envp[];
{
	int testcase;		/* The test case number from argv[1] */
	pid_t proc_pid;

	myname = argv[0];

	/*
	 * First, find out what test the shell asked us to run,
	 * checking the validity of the request as well.
	 */
	if( argc != 3 || (testcase = conv_arg(argv[1])) == 0 ) {
		fprintf(stderr, "usage: %s [ 1 - %d ]\n", myname, ntests);
		exit(1);
	}

	init_config_globals();
	proc_pid = atoi(argv[2]);

	/*
	 * Execute the test specified, and log its results.
	 * Only ONE test case per run.
	 */
	(void)do_test(testcase, proc_pid);

	/* Handle internal bugs if found. */
	if (int_err) {
		fprintf(stderr, "Internal error:  test case (%d).\n",testcase);
		exit(1);
	}

	exit(0);
}
/****************************************************************/
/*								*/
/* Function:	do_test()					*/
/*								*/
/* Parameters:	testcase, the test case number, valid between	*/
/*		1 and ntests.					*/
/* 		proc_pid, the pid to be restarted.		*/
/****************************************************************/

int
do_test(int testcase, pid_t proc_pid)
{
	int 		nproc, count, waitstat;
	pid_t		pid;
	int		expected_return, expected_errno;
	int		actual_return;
	int		option;
	int		pipe_val[2];
	char		path_name[100];
	char		prefix[100];
	pid_t		id, *pid_list, *ppid_list;
	int		*node_list;
	int		status;
	int		fd;

	sprintf(path_name, "/chkpnt/proc.%d", proc_pid);
	
	status = chkpnt_getprocinfo(path_name, &id, &nproc, &pid_list, 
				    &ppid_list, &node_list);
	if (status == -1) {
		fprintf(stderr, "Can not getprocinfo from %s\n", path_name);
		fprintf(stderr, "FAILED errno %d\n", errno);
		int_err++;
		return(0);
	}

	pid = fork();

	if (pid < 0) {
		int_err++;
		fprintf(stderr, "fork failed for test case.\n", testcase);
		return(-1);
	}

	/* 
	 * This is really where the test is.
	 */
	if (pid == 0) {
		
		sprintf(prefix, "%s/%d.%d.%d.%d",
			path_name, pid_list[0], ppid_list[0], proc_pid, node_list[0]);
		switch(testcase) {
			case 1:
				expected_return = 0;
				expected_errno = 0;
				option = 0;
				break;
			case 2:
				expected_return = 0;
				expected_errno = 0;
				option = RESTARTEXEC_STOP;
				break;
			case 3:
				expected_return = -1;
				expected_errno = ENOTTY;
				option = RESTARTEXEC_STOP;
printf("expected_errno ENOTTY %d\n", expected_errno);
				fd = open("/dev/tty", O_RDWR);
				if (fd >= 0) {
					ioctl(fd, TIOCNOTTY, 0);
					close(fd);
				}
				break;
			default:
				strcat(casedescript, 
					"default case reached, internal error.\n");
				++int_err;
				fprintf(stderr, "Invalid test case %d must be 1 - %d\n",
		 		testcase, ntests);
				return( -1 );
		}

		errno = 0;
printf("expected_errno ENOTTY %d\n", expected_errno);
		actual_return = exec_restart(prefix, option);
printf("expected_errno ENOTTY %d\n", expected_errno);

		if (testcase == 1 || testcase == 2) {
			fprintf(stderr, "FAILED %s exec TEST %2d\n",
			        myname, testcase);
			exit(-1);
		}
	
		if( actual_return == expected_return ) {
			printf("PASSED %s retcode TEST %2d\n", myname, testcase);
		} else {
			fprintf(stderr,
		 	"FAILED %s retcode TEST %2d: expected %d got %d\n\n",
		 	myname, testcase, expected_return, actual_return);
			fflush(stderr);
		}

		if(errno == expected_errno) {
			printf("PASSED %s global errno TEST %2d\n", myname, testcase);
		} else {
			fprintf(stderr,
			 "FAILED %s global errno TEST %2d: expected %d got %d\n",
			 myname, testcase, expected_errno, errno);
			fflush(stderr);
		}

		exit();
	}

	if (testcase == 3)
		return(1);

	/*
	 * If the children are suppose to stop, make they are stopped.
	 */
	if (testcase == 2) {
		count = 0;
		for (;;) {
			sleep(1);
			status = waitpid(-1, &waitstat, WUNTRACED);
			if (WIFSTOPPED(waitstat) > 0) {
				kill(pid, SIGCONT);
				break;
			}
			if (count++ == 20) {
				fprintf(stderr, "FAILED %s resume TEST %2d\n", myname, testcase);
				exit(-1);
			}
		}
	}

	sleep(30);

	if (!check_data_file(getpgrp())) {
		fprintf(stderr, "FAILED %s data TEST %2d\n", myname, testcase);
		exit(-1);
	}

	printf("PASSED %s data TEST %2d\n", myname, testcase);

	return(0);
}

int
check_data_file(pid_t target_pgrp)
{
	FILE		*fd1, *fd2;
	pid_t		child_pgrp, parent_pgrp;

	fd1 = fopen("parent.data", "r");

	if (fd1 == NULL) {
		fprintf(stderr, "Can't re-open data files.\n");
		return(0);
	}
	
	fscanf(fd1, "%d", &parent_pgrp);

	if (parent_pgrp != target_pgrp) {
		fprintf(stderr, "Bad data in chkpnt data files.\n");
		return(0);
	}
	return(1);
}
