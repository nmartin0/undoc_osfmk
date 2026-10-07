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
 * $Log: restart_call.c,v $
 * Revision 1.2  1994/11/18  21:07:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:56:54  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:18:32  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:33:36  hao
 * 	Initial check-in.
 * 
 */
#include <errno.h>
#include <stdio.h>
#include "../common/vstnc.h"
#include <tnc/chkpnt.h>
#include <sys/mode.h>
#include <sys/fcntl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>

int seghandler();
/*
 * Number of tests.
 */
int ntests = 9;

#define	CHKPNT_EINVAL -1

#define DEBUG 0
#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

char    *CONFIG_BADPATH = "/chkpnt/proc.bad";
char    *CONFIG_GOODFILE = "/chkpnt/goodfile";

int do_test_end();

int int_err = FALSE;

char casedescript[MAXBUF]; /* desctiption of this testcase  */

char *myname;
main(argc, argv, envp)
int argc;
char *argv[], *envp[];
{
	int 	testcase;		/* The test case number from argv[1] */
	pid_t	test_pid;

	myname = argv[0];

	/*
	 * First, find out what test the shell asked us to run,
	 * checking the validity of the request as well.
	 */
	if( argc != 3 || (testcase = conv_arg(argv[1])) == 0 ) {
		fprintf(stderr, "usage: %s [ 1 - %d ]\n", myname, ntests);
		exit(1);
	}

	test_pid = atoi(argv[2]);

	init_config_globals();

	/*
	 * Execute the test specified, and log its results.
	 * Only ONE test case per run.
	 */
	(void)do_test(testcase, test_pid);

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
/* Returns:	None						*/
/*								*/
/* Parameters:	testcase, the test case number, valid between	*/
/*		1 and ntests.					*/
/****************************************************************/

int
do_test(int testcase, int test_pid)
{
	int 		nproc, waitstat;
	pid_t		pid, return_id, expected_return_pid;
	int		expected_return, expected_errno;
	int		actual_return;
	int		option, count;
	char		restart_path_name[100];
	char		bogus_file_name[100];
	int		pipe_val[2];
	char		*path_to_use;
	int		status;
	struct 		rlimit rl;

	errno = 0;

	pid = fork();
	sprintf(restart_path_name, "/chkpnt/pgrp.%d", test_pid);

	if (pid < 0) {
		int_err++;
		fprintf(stderr, "fork failed for test case.\n", testcase);
		return(-1);
	}

	/* 
	 * This is really where the test is.
	 */
	if (pid == 0) {

		path_to_use = restart_path_name;
		expected_return_pid = test_pid;

		switch(testcase) {
			case 1:
			{
				expected_return = 2;
				expected_errno = 0;
				option = RESTART_SIGALL;
				break;
			}

			case 2:
			{
				expected_return = 2;
				expected_errno = 0;
				option = RESTART_EXECROOT | RESTART_SIGALL;
				break;
			}

			case 3:
			{
				expected_return = 2;
				expected_errno = 0;
				expected_return_pid = getpgrp();
				option = RESTART_NOSETPGID | RESTART_SIGALL;
				break;
			}
	
			case 4:
			{
				expected_return = -1;
				expected_errno = ENOENT;
				path_to_use = CONFIG_BADPATH;
				break;
			}
	
			case 5:
			{
				expected_return = -1;
				expected_errno = ENOTDIR;
				path_to_use = CONFIG_GOODFILE;
				break;
			}
	
			case 6:
			{
				expected_return = -1;
				expected_errno = EINVAL;
				sprintf(bogus_file_name, "%s/0.%d.%d.0.core",
					restart_path_name, test_pid, test_pid);
				creat(bogus_file_name, O_RDWR);
				break;
			}

			case 7:
			{
				expected_return = -1;
				expected_errno = ECHILD;
				status = setuid(config_userid1);
				option = RESTART_SIGALL;
				if (status == -1) {
					int_err++;
					fprintf(stderr, "Can't setuid for test case %d.\n", testcase);
					return(-1);
				}
				break;
			}

			case 8:
			{
				expected_return = -1;
				expected_errno = EACCES;
				chmod(restart_path_name, S_IRWXU | S_IXGRP | S_IXOTH);
				status = setuid(config_userid1);
				option = RESTART_SIGALL;
				if (status == -1) {
					int_err++;
					fprintf(stderr, "Can't setuid for test case %d.\n", testcase);
					return(-1);
				}
				sleep(2);
				/*
				printf("doing testcase 8\n");
				*/
				break;
			}

			/*
			case 9:
			{
				int	*temp;
				status = getrlimit(RLIMIT_DATA, &rl);
				rl.rlim_cur = 10000;
				rl.rlim_max = 10000;
				expected_return = -1;
				expected_errno = ENOMEM;
				option = RESTART_SIGALL;
				status = setrlimit(RLIMIT_DATA, &rl);
				if (status == -1) {
					int_err++;
					fprintf(stderr, "Can't setrlimit for case %d.\n", testcase);
					return(-1);
				}
				printf("after setrlimi\n");
				getchar();
				signal(SIGSEGV, seghandler);
				temp = (int *) malloc(20000);
				if (temp == NULL) 
					printf("malloc failed \n");
				else
					printf("malloc succeded\n");
				break;
			}
			*/

			default:
			{
				strcat(casedescript, 
					"default case reached, internal error.\n");
				++int_err;
				fprintf(stderr, "Invalid test case %d must be 1 - %d\n",
		 		testcase, ntests);
				return( -1 );
			}
		}

		actual_return = restart(path_to_use, &return_id, option);
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

		if (expected_return == -1)
			return(1);

		if (return_id == -expected_return_pid) {
			printf("PASSED %s return id TEST %2d\n", myname, testcase);
		} else {
			fprintf(stderr, "FAILED %s return id TEST %2d: expected %d got %d\n",
		  	         myname, testcase, expected_errno, errno);
			fflush(stderr);
		}

		if (testcase == 1 || testcase == 3) {
			count = 0;
			for (;;) {
				sleep(1);
				waitpid(test_pid, &waitstat, WNOHANG);
				if (WIFEXITED(waitstat));
					break;
				if (count++ == 20) {
					fprintf(stderr, "FAILED %s resume TEST %2d\n", myname, testcase);
					exit(-1);
				}	
			}
		}

		if (testcase == 2) {
			fprintf(stderr, "FAILED %s EXECROOT TEST %2d\n", myname, testcase);
			exit(-1);
		}
		exit(0);
	}

	/*
	 * This is the parent.  Wait for child and make sure 
	 * all things were OK.  For case 1 and 2, make sure
	 * the correct chkpnt images are left behind.
	 */

	wait(&waitstat);
	if (waitstat != 0)
		return(-1);

	if (testcase == 1) {
		sleep(10);
		status = check_data_file(test_pid);
	}
	else 
		if (testcase ==2 || testcase == 3) {
			sleep(10);
			status = check_data_file(getpgrp());
		}
		else 
			return(-1);
	if (status != 1) {
		fprintf(stderr, "FAILED %s chkpnt image TEST %2d\n", myname, testcase);
			fflush(stderr);
			return(-1);
	}	
	else 
		printf("PASSED %s chkpnt image TEST %2d\n", myname, testcase);
	return(-1);
}

int
check_data_file(pid_t target_pgrp)
{
	FILE		*fd1, *fd2;
	pid_t		child_pgrp, parent_pgrp;

	fd1 = fopen("parent.data", "r");
	fd2 = fopen("child.data", "r");

	if (fd1 == NULL || fd2 == NULL) {
		fprintf(stderr, "Can't re-open data files.\n");
		return(0);
	}
	
	fscanf(fd1, "%d", &parent_pgrp);
	fscanf(fd2, "%d", &child_pgrp);


	if (parent_pgrp != target_pgrp || child_pgrp != target_pgrp) {
		fprintf(stderr, "Bad data in chkpnt data files.\n");
		return(0);
	}
	return(1);
}

seghandler()
{
	return(0);
}
