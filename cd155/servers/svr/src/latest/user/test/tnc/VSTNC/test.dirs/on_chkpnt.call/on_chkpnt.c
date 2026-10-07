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
 * $Log: on_chkpnt.c,v $
 * Revision 1.2  1994/11/18  21:07:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:55:32  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:17:13  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:24:56  hao
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
#include <sys/signal.h>
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

int do_test_end(int, pid_t);
int my_handle();

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
/*		proc_pid, the pid to restart.			*/
/*								*/
/****************************************************************/

int
do_test(int testcase, pid_t proc_pid)
{
	extern	int	check_data_file(int);
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

	switch(testcase) {
		case 1:
		{
			expected_return = proc_pid;
			break;
			}

		case 2:
		{
			expected_return = proc_pid;
			break;
		}

		case 3:
		{
			expected_return = -1;
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

	if (!check_data_file(expected_return)) {
		fprintf(stderr, "FAILED %s data TEST %2d\n", myname, testcase);
		exit(-1);
	}

	printf("PASSED %s data TEST %2d\n", myname, testcase);

	if (testcase == 2) {
		nproc = 0;
		sprintf(path_name, "/chkpnt/proc.%d", proc_pid);
		status = chkpnt_getprocinfo(path_name, &id, &nproc, &pid_list, 
					    &ppid_list, &node_list);
		if (status == 0 && nproc == 0)
			printf("PASSED %s chkpnt TEST %2d\n", myname, testcase);
		else
			fprintf(stderr, "FAILED %s chkpnt TEST %2d\n", myname, testcase);
	}
	return(0);
}

int
check_data_file(int expected_return)
{
	FILE		*fd1, *fd2;
	pid_t		child_pgrp, parent_pgrp;

	fd1 = fopen("parent.data", "r");

	if (fd1 == NULL ) {
		if (expected_return == -1)
			return(1);
		else {
			return(0);
		}
	}
	fscanf(fd1, "%d", &parent_pgrp);

	if (parent_pgrp != expected_return) {
		fprintf(stderr, "Bad data in chkpnt data files.\n");
		return(0);
	}
	return(1);
}
