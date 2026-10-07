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
 * $Log: chkpnt_self.call.c,v $
 * Revision 1.2  1994/11/18  21:05:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:53:03  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:15:10  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:06:59  hao
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
#include "../common/vstnc.h"

/*
 * Number of tests.
 */
int ntests = 8;

#define	CHKPNT_EINVAL -1

#define DEBUG 0
#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

int do_test_end();

int int_err = FALSE;

char casedescript[MAXBUF]; /* desctiption of this testcase  */

char *myname;
main(argc, argv, envp)
int argc;
char *argv[], *envp[];
{
	int testcase;		/* The test case number from argv[1] */

	myname = argv[0];

	/*
	 * First, find out what test the shell asked us to run,
	 * checking the validity of the request as well.
	 */
	if( argc != 2 || (testcase = conv_arg(argv[1])) == 0 ) {
		fprintf(stderr, "usage: %s [ 1 - %d ]\n", myname, ntests);
		exit(1);
	}

	init_config_globals();

	/*
	 * Execute the test specified, and log its results.
	 * Only ONE test case per run.
	 */
	(void)do_test(testcase);

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
/****************************************************************/

int
do_test(testcase)
int testcase;
{
	int 		nproc, count, waitstat;
	pid_t		pid;
	int		expected_return, expected_errno;
	int		actual_return;
	int		option;
	char		path_name[100];
	int		pipe_val[2];
	int		status;


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

		sprintf(path_name, "/chkpnt/proc.%d", getpid());

		status = mkdir(path_name, S_IREAD|S_IWRITE); 
		if (status != 0) {
			int_err++;
			fprintf(stderr, "Can't mkdir %s for test case %d.\n", 
				path_name, testcase);
			return(-1);
		}

		switch(testcase) {
			case 1:
			{
				expected_return = 0;
				expected_errno = 0;
				option = CHKPNT_ASYNC;
				break;
			}

			case 2:
			{
				expected_return = 0;
				expected_errno = 0;
				option = CHKPNT_KILL;
				break;
			}

			case 3:
			{
				expected_return = -1;
				expected_errno = EINVAL;
				option = CHKPNT_EINVAL;
				break;
			}
	
			case 4:
			{
				expected_return = -1;
				expected_errno = ENOTDIR;
				option = CHKPNT_KILL;
				status = rmdir(path_name);
				if (status != 0) {
					int_err++;
					fprintf(stderr, "Can't mkdir %s for test case %d.\n", 
						path_name, testcase);
					return(-1);
				}
				status = creat(path_name, O_RDWR);
				break;
			}
	
			case 5:
			{
				expected_return = -1;
				expected_errno = EACCES;
				option = CHKPNT_KILL;
				status = setuid(config_userid1);
				if (status != 0) {
					int_err++;
					fprintf(stderr, "Can't setuid %d for test case %d.\n", 
						config_userid1, testcase);
					return(-1);
				}
				break;
			}
	
			case 6:
			{
				expected_return = -1;
				expected_errno = EBADF;
				option = CHKPNT_KILL;
				status = pipe(pipe_val);
				if (status != 0) {
					int_err++;
					fprintf(stderr, "Can't open pipe for test case %d.\n", 
						testcase);
					return(-1);
				}
				break;
			}

			case 7:
			{
				expected_return = -1;
				expected_errno = EBADF;
				option = CHKPNT_KILL;
				status = socket(AF_UNIX, SOCK_STREAM, 0);
				if (status == -1) {
					int_err++;
					fprintf(stderr, "Can't open pipe for test case %d.\n", 
						testcase);
					return(-1);
				}
				break;
			}

			case 8:
			{
				expected_return = -1;
				expected_errno = ENOSYS;
				option = CHKPNT_KILL;
				printf("before vm_allocate\n");
				status = vm_allocate(mach_task_self(), 0, 100, TRUE);
				printf("after vm_allocate\n");
				if (status == -1) {
					int_err++;
					fprintf(stderr, "Can't open vm_allocate for test case %d.\n", testcase);
					return(-1);
				}
				break;
			}

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

		errno = 0;
		actual_return = chkpnt_self(option);
		if( actual_return == expected_return ) {
			printf("PASSED %s retcode TEST %2d\n", myname, testcase);
		} else {
			fprintf(stderr,
		 	"FAILED %s retcode TEST %2d: expected %d got %d\n\n",
		 	myname, testcase, expected_return, actual_return);
			fflush(stderr);
		}

		if( errno == expected_errno ) {
			printf("PASSED %s global errno TEST %2d\n", myname, testcase);
		} else {
			fprintf(stderr,
			 "FAILED %s global errno TEST %2d: expected %d got %d\n",
			 myname, testcase, expected_errno, errno);
			fflush(stderr);
		}
	
		/* 
		 * The Child process should have been killed after
		 * chkpnt_async with the CHKPNT_KILL option.
		 */
		if(testcase == 2)
			exit(-1);

		exit(0);
	}

	/*
	 * This is the parent.  Wait for child and make sure 
	 * all things were OK.  For case 1 and 2, make sure
	 * the correct chkpnt images are left behind.
	 */
	
	wait(&waitstat);
	if (waitstat != 0 && testcase != 2) {
		fprintf(stderr,"FAILED %s TEST %2d\n", myname, testcase);
		return(-1);
	}

	if (testcase == 1 || testcase == 2) {
		if (!check_chkpnt_images(pid, getpid(), getpgrp())) {
			fprintf(stderr, "FAILED %s chkpnt image TEST %2d\n",
			        myname, testcase);
			fflush(stderr);
			return(-1);
		}
		else	
			/*
			 * Since in testcase two, the call is not 
			 * returning.  If the image checks out, we
			 * print the success message here.
			 */
			printf("PASSED %s chkpnt image TEST %2d\n",
			       myname, testcase);
	}

	return(1);
}

int
check_chkpnt_images(int pid, int ppid, int pgrp)
{
	char		path_name[100];
	pid_t		id, *pid_list, *ppid_list;
	int		*node_list;
	int		nproc, status;

	sprintf(path_name, "/chkpnt/proc.%d", pid);
	
	status = chkpnt_getprocinfo(path_name, &id, &nproc, &pid_list, 
				    &ppid_list, &node_list);
	if (status == -1) 
		return(0);
	if (nproc != 1)
		return(0);
	if (id != pgrp || pid_list[0] != pid || ppid_list[0] != ppid ||
	    node_list[0] != config_mynode)
		return(0);

	return(1);
}
