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
 * $Log: forkfamily.c,v $
 * Revision 1.2  1994/11/18  21:06:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:54:53  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:16:13  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:18:52  hao
 * 	Initial check-in.
 * 
 */

#include <errno.h>
#include <stdio.h>
#include "../common/vstnc.h"

/*
 * Number of tests.
 */
int ntests = 7;

#define	CHILDMAX 10

#define DEBUG 0
#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

int do_test_end();
int get_valid_pid(int);
void print_pid_array(int *);

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
	int		pid_array[CHILDMAX];
	int		child_sum, parent_sum;
	int		expected_return, expected_errno;
	int		actual_return;
	char		*p;
	int		*temp_p = &pid_array[0];


	for( parent_sum=0,p=(char *)do_test; p<(char *)do_test_end; ++p ) {
		parent_sum += *p;
	}

	switch(testcase) {

		case 1:
		{
			expected_return = 1;
			expected_errno = 0;
			nproc = 1;
			pid_array[0] = get_valid_pid(-1);
			if (pid_array[0] == -1) {
				int_err++;
				fprintf(stderr, "fork failed for test case.\n", testcase);
				exit(-1);
			}
			break;
		}

		case 2:
		{
			expected_return = 1;
			expected_errno = 0;
			nproc = 1;
			pid_array[0] = get_valid_pid(config_goodnode);
			if (pid_array[0] == -1) {
				int_err++;
				fprintf(stderr, "fork failed for test case.\n", testcase);
				exit(-1);
			}
			break;
		}

		case 3:
		{
			expected_return = -1;
			expected_errno = EINVAL;
			nproc = 1;
			pid_array[0] = -9999;
			break;
		}
	
		case 4:
		{
			expected_return = 1;
			expected_errno = 0;
			nproc = CHILDMAX;
			for (count = 0; count < CHILDMAX; count++) {
				switch (count % 2) {
					case 0: {
						pid_array[count] = get_valid_pid(config_goodnode);
						break;
					}
					case 1: {
						pid_array[count] = get_valid_pid(-1);
						break;
					} 
				}
				if (pid_array[count] == -1) {
					int_err++;
					fprintf(stderr, "fork failed for test case.\n", testcase);
					exit(-1);
				}
			}
			break;
		}
	
		case 5:
		{
			expected_return = -1;
			expected_errno = EINVAL;
			nproc = CHILDMAX;
			for (count = 0; count < CHILDMAX; count++) {
				switch (count % 2) {
					case 0: {
						pid_array[count] = get_valid_pid(config_goodnode);
						break;
					}
					case 1: {
						pid_array[count] = -count;
						break;
					} 
				}
				if (pid_array[0] == -1) { 
					int_err++;
					fprintf(stderr, "fork failed for test case.\n", testcase);
					exit(-1);
				}
			}
			break;
		}
	
		case 6:
		{
			expected_return = -1;
			expected_errno = EFAULT;
			nproc = 1;
			temp_p = (int *) -1;
			break;
		}

		case 7:
		{
			expected_return = -1;
			expected_errno = EEXIST;
			nproc = 1;
			pid_array[0] = getpid();
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

	actual_return = forkfamily(nproc, temp_p);
	if( actual_return == 0 ) {	/* we are a child */
		for( child_sum=0,p=(char *)do_test; p<(char *)do_test_end; 
		     ++p ) {
			child_sum += *p;
		}
		if( child_sum != parent_sum )
			exit(1);
		exit(0);		/* good sum means 0 exit */
	}

	/* we are the parent again */

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

	if (expected_return == -1) 
		return(0);

	for( count=0; count < nproc; count++) {
		wait(&waitstat);
		if( waitstat != 0 ) {
			fprintf(stderr, "FAILED %s checksum TEST %2d pid=%d\n",
			 myname, testcase, actual_return);
			fflush(stderr);
		}
	}

	return(0);		/* no internal error */
}

int
get_valid_pid(int node)
{
	int pid;

	if (node == -1)
		pid = fork();
	else
		pid = rfork(node);

	if (pid == 0)
		exit();
	if (pid < 0)
		return(-1);
	wait();

	return(pid);
}

void
print_pid_array(int pid_array[])
{
	int i;
	for (i=0; i<CHILDMAX; i++) 
		printf("pid_array[%d] = %d\n", i, pid_array[i]);
}

do_test_end()
{
	return(0);		/* dummy routine marks end of do_test() */
}
