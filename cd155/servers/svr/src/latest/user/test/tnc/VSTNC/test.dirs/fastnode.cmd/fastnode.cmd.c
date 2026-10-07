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
 * $Log: fastnode.cmd.c,v $
 * Revision 1.5  1994/11/18  21:06:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/02/04  01:42:52  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.3  93/06/09  17:18:07  yazz
 * Regularize VSTNC output.
 * 
 * Revision 3.2  93/03/17  11:54:12  jpaul
 * Fix for Bug #140, 200, 203. 
 * Use shared memory more logically and move all creation/deletion
 * code to common.c.  Don't try to detach from not attached memory.
 * Various fixes for failing testcases.
 * 
 * Revision 3.1  92/12/02  16:36:52  jpaul
 * Correct expected value.
 * 
 * Revision 3.0  92/07/22  16:50:15  jpaul
 * Initial Checkin
 * 
 */
/*******************************************************************************

Module:		fastnode.cmd.c

Purpose:	This module tests the TNC command fastnode.  Sample input,
		both correct and erroneous, is given to the command.  Then
		the command's standard output and standard error are
		compared against expected values.  The fastnode command
		uses shared memory data to supply sample input for this file 
		in standard input, and then it is written into this file by 
		a system("") call to the cat command as part of the test 
		execution.


*******************************************************************************/


#include "../common/vstnc.h"
#include <sys/table.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>

int ntests = 14;		/* number of test cases */

int int_err = 0;
char *myname;

#define DEBUG 1
/*
*/

#define NODECOUNT 3
#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

struct load_vec_info
{
	int node; /* node number */
	double lm; /* load measure */
};

struct  load_vec_info node_n_load[3];	

char casedescript[MAXBUF]; /* desctiption of this testcase  */

int do_test(int);
extern int setup_shared();
int want_detach = 0;

main(
	int argc,
	char *argv[])
{

	int testcase;		/* The test case number */
	int ret_value;		/* do_test() returns 0 for success else fail */
	int shret;

	myname = argv[0];

	/*
	 * First, find out what test the shell asked us to run, checking
	 * the validity of the request as well.
	 */
	if ((argc < 2) || ((testcase = conv_arg(argv[1])) == 0)) {
		fprintf(stderr, "usage: fastnode [-K keyfile] [1 - %d]\n", ntests);
		fflush(stderr);
		exit(1);
	}

	init_config_globals();  /* read config file. */

	/*
	 * This routine executes the test and logs the results.
	 */
	ret_value = do_test(testcase);

	/*
	 * Handle internal errors if any.
	 */
	if (int_err) {
		fprintf(stderr,"Internal error: test case (%d)\n\n",testcase);
		fflush(stderr);
		exit(1);
	}

	/* destroy shared mem segment  */
	if (want_detach != 0) {
		shret = shmemdestroy();  /* destroy and detach, in common.c */
		if (shret != 0) {
			printf("Shared memory destroy operation failed.\n");
			exit(-1);
		}
	}

	exit(0);
}

/*******************************************************************************

Function:	do_test

Returns:	0 for success, 1 for failure, -1 for internal error.

Parameters:	testcase, the test case number, valid between 1 and ntests.

		ptr_positive, a pointer to a flag the routine clears if
		the test cases is a negative one.

Purpose:	This function executes just one test.  All test cases
		correspond to the FV plan.


*******************************************************************************/


int
do_test(int testcase)
{
	FILE	*stdinfp, *stdoutfp, *stderrfp;	/* not ours, the test's */
	char	stdin_name[17], stdout_name[17], stderr_name[17];
#	define	SIZE 4096

	char	cmd_buf[SIZE];		/* 4 bufs for setup of test cases */
	char	stdin_buf[SIZE];
	char	stdout_buf[SIZE];
	char	stderr_buf[SIZE];

	char	master_cmd_buf[SIZE];	/* 4 more for performing the tests */
	char	master_stdin_buf[SIZE];
	char	master_stdout_buf[SIZE];
	char	master_stderr_buf[SIZE];

	int	len;
	int	st;
	char	*p;
	int	i;
	int	anyfail = 0;
	int	shret;			/* shared mem function return val */
	int	shmid = 0;

	/*
	 * Generate stdin, stdout and stderr filenames and
	 * create all three files for writing.
	 */
	sprintf(stdin_name, "test.%02d.stdin", testcase);
	stdinfp = fopen(stdin_name, "w+");
	if( stdinfp == NULL ) {
		++int_err;
		fprintf(stderr, "Can't create test file %s\n", stdin_name);
test_aborted:
#ifdef DEBUG
		printf("Aborting test; fastnode.cmd: key=%d\n", shmid);
#endif
		fprintf(stderr, "\nTEST ABORTED\n\n");
		fflush(stderr);
		exit(1);
	}

	sprintf(stdout_name, "test.%02d.stdout", testcase);
	stdoutfp = fopen(stdout_name, "w+");
	if( stdoutfp == NULL ) {
		++int_err;
		fprintf(stderr, "Can't create test file %s\n", stdout_name);
		goto test_aborted;
	}

	sprintf(stderr_name, "test.%02d.stderr", testcase);
	stderrfp = fopen(stderr_name, "w+");
	if( stderrfp == NULL ) {
		++int_err;
		fprintf(stderr, "Can't create test file %s\n", stderr_name);
		goto test_aborted;
	}

	/*
	 * Zero out the test case buffers: command string,  standard input,
	 * expected standard output and expected standard error.  The
	 * command string has no newlines in it whereas stdin, stdout and
	 * stderr are multi-line strings that have a newline ('\n') at
	 * the end of every line.
	 *
	 * All test cases require proper PATH and TZ environment variables
	 * from configuration.  The command string has no newlines in it.
	 * By contrast, standard in has one at the end of every line.
	 */
	bzero(cmd_buf, SIZE);
	bzero(stdin_buf, SIZE);
	bzero(stdout_buf, SIZE);
	bzero(stderr_buf, SIZE);


	switch( testcase ) {
	case 1:  	/* should choose node 0. */
		strcat(casedescript, "Node 0 should be the fastest; cmd: fastnode -K /tmp\n");

		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 1.12345;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.12346;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 1.12347;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}

		want_detach++;	/* want to try the shmem detach */
		
		strcpy(stdout_buf, "0\n");

		strcpy(stderr_buf, "");
		break;

	case 2:		/* should use node 0.  */

		strcat(casedescript, "Node 0 should be the fastest; cmd: fastnode -K /tmp\n");

		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 1.12345;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.12347;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 1.12346;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "0\n");

		strcpy(stderr_buf, "");
		break;

	case 3:		/* should choose node 1. */

		strcat(casedescript, "Node 1 should be the fastest; cmd: fastnode -K /tmp\n");

		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 1.12346;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.12345;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 1.12347;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "1\n");

		strcpy(stderr_buf, "");
		break;

	case 4:  /* should use node 1 */
		strcat(casedescript, "Node 1 should be the fastest; cmd: fastnode -K /tmp\n");

		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 1.12346;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.12345;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 1.12347;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "1\n");

		strcpy(stderr_buf, "");
		break;

	case 5:  /* should choose node 0 */

		strcat(casedescript, "Node 0 should be the fastest; cmd: fastnode -K /tmp\n");

		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 1.12347;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.12345;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 1.12346;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "1\n");

		strcpy(stderr_buf, "");
		break;

	case 6:		/* should choose node 2, first in list */
		strcat(casedescript, "Node 2 should be the fastest; cmd: fastnode -K /tmp\n");
		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 1.12347;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.12346;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 1.12346;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "2\n");

		strcpy(stderr_buf, "");
		break;

	case 7:		/* one expected */

		strcat(casedescript, "Node 1 should be the fastest; cmd: fastnode -K /tmp\n");
		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 3.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 2.0;
		node_n_load[2].node = 0;
		node_n_load[2].lm = 1.0;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "0\n");

		strcpy(stderr_buf, "");
		break;

	case 8:		/* should return 0.  */
		strcat(casedescript, "Node 0 should be the fastest, some negative loads; cmd: fastnode -K /tmp\n");
		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 1.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = -1.0;
		node_n_load[2].node = 2;
		node_n_load[2].lm = -1.0;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "0\n");

		strcpy(stderr_buf, "");
		break;

	case 9:  /* expecting 2..  */
		strcat(casedescript, "Node 2 should be the fastest; cmd: fastnode -K /tmp\n");
		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 0;
		node_n_load[0].lm = -1.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = -1.0;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 3.0;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "2\n");

		strcpy(stderr_buf, "");
		break;

	case 10:		/* expecting 2. */
		strcat(casedescript, "Node 2 should be the fastest; cmd: fastnode -K /tmp\n");
		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 4.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = -1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 3.0;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "2\n");

		strcpy(stderr_buf, "");
		break;

	case 11:		/*  expecting 2. */
		strcat(casedescript, "Node 2 should be the fastest; cmd: fastnode -K /tmp\n");
		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 2;
		node_n_load[0].lm = 3.0;
		node_n_load[1].node = 2;
		node_n_load[1].lm = 1.0;
		node_n_load[2].node = 1;
		node_n_load[2].lm = 2.0;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "2\n");

		strcpy(stderr_buf, "");
		break;

	case 12:		/* should pick node 2.  */
		strcat(casedescript, "Node 2 should be the fastest; cmd: fastnode -K /tmp\n");
		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 2;
		node_n_load[0].lm = 3.0;
		node_n_load[1].node = 2;
		node_n_load[1].lm = 2.0;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 1.0;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "2\n");

		strcpy(stderr_buf, "");
		break;

	case 13:		/* should get 0 out of this */
		strcat(casedescript, "Node 0 should be the fastest; cmd: fastnode -K /tmp\n");
		strcat(cmd_buf, "fastnode -K /tmp");

		node_n_load[0].node = 0;
		node_n_load[0].lm = -1.0;
		node_n_load[1].node = 0;
		node_n_load[1].lm = 1.0;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.0;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}

		want_detach++;	/* want to try the shmem detach */

		strcpy(stdout_buf, "0\n");

		strcpy(stderr_buf, "");
		break;

	case 14:	/* should get 0 here, and warning msg. */
		strcat(casedescript, "Error condition: shared mem not setup or unaccessible; cmd: fastnode -K /tmp\n");
		strcat(cmd_buf, "fastnode -K /bin");

		/* purposely don't setup shared mem 
			node_n_load[0].node = 0;
			node_n_load[0].lm = -1.0;
			node_n_load[1].node = 1;
			node_n_load[1].lm = -1.0;
			node_n_load[2].node = 2;
			node_n_load[2].lm = -1.0;
		*/

		want_detach = 0;

		sprintf(stdout_buf, "%d\n", (node_self()));

		strcpy(stderr_buf, "Load information not available; using local node.\n");
		break;

	default:
	{
		strcat(casedescript, "default: testcase not known.\n");
		fprintf(stderr, "Unknown test case #%d\n\n", testcase);
		goto test_aborted;
	} 
	}  /* end switch */
	
	/* tell user what the testcase is testing.  */
	printf("%s", casedescript);
	fflush(stdout);

	/*
	 * Write our standard input string to the standard input file.
	 * Multiple lines of input can be placed in a single string.
	 */
	strcpy(master_stdin_buf, stdin_buf);
	len = strlen(master_stdin_buf);
	st = fwrite(master_stdin_buf, sizeof(char), len, stdinfp);
	if( st != len ) {
		fprintf(stderr, "Can't write into test file %s\n", stdin_name);
		goto test_aborted;
	}

	/*
	 * Now close all three files.  Standard output and error should
	 * be zero length.
	 */
	if( fclose(stdinfp) == EOF ) {
		fprintf(stderr, "Can't close test file %s errno=%d\n",
				stdin_name, errno);
		goto test_aborted;
	}
	if( fclose(stdoutfp) == EOF ) {
		fprintf(stderr, "Can't close test file %s errno=%d\n",
				stdout_name, errno);
		goto test_aborted;
	}
	if( fclose(stderrfp) == EOF ) {
		fprintf(stderr, "Can't close test file %s errno=%d\n",
				stderr_name, errno);
		goto test_aborted;
	}

	/*
	 * Construct the complete command string with environment values
	 * for TZ and PATH ("TZ=xxx; PATH=xxx; export TZ PATH; ")
	 * plus the command string for this particular testcase, plus
	 * redirection of standard in, standard out and standard error
	 * (" < testXXstdin > testXXstdout 2> testXXstderr").
	 */
	strcpy(master_cmd_buf, "TZ=");
	strcat(master_cmd_buf, config_TZ);
	strcat(master_cmd_buf, "; PATH=");
	strcat(master_cmd_buf, config_PATH);
	strcat(master_cmd_buf, "; export TZ PATH; ");
	strcat(master_cmd_buf, cmd_buf);
	strcat(master_cmd_buf, " < ");
	strcat(master_cmd_buf, stdin_name);
	strcat(master_cmd_buf, " > ");
	strcat(master_cmd_buf, stdout_name);
	strcat(master_cmd_buf, " 2> ");
	strcat(master_cmd_buf, stderr_name);

	st = system(master_cmd_buf);	/* perform the command */

	if( st == 127 ) {
		fprintf(stderr, "Can't execute shell with 'system'.\n");
		fprintf(stderr, "Command was: %s\n", master_cmd_buf);
		goto test_aborted;
	}

	/*
	 * Now check the results.  Standard output and standard error
	 * should match exactly what we expected.
	 */
	stdoutfp = fopen(stdout_name, "r");
	if( stdoutfp == NULL ) {
		++int_err;
		fprintf(stderr, "Can't reopen test file %s\n", stdout_name);
		goto test_aborted;
	}

	stderrfp = fopen(stderr_name, "r");
	if( stderrfp == NULL ) {
		++int_err;
		fprintf(stderr, "Can't reopen test file %s\n", stderr_name);
		goto test_aborted;
	}

	/*
	 * Next, read the files into their buffers and compare them.
	 * Log the results.
	 */
	len = fread(master_stdout_buf, sizeof(char), SIZE, stdoutfp);
	if( len >= SIZE ) {
		++anyfail;
		fprintf(stderr, "Test file %s too long\n", stdout_name);
		master_stdout_buf[SIZE-1] = '\0';
					/* null terminate the string */
	} else if( strcmp(stdout_buf, master_stdout_buf) != 0 ) {
		++anyfail;
		fprintf(stderr, "FAILED %s TEST %2d FILE STDOUT\n",
				myname, testcase);
		fprintf(stderr, "Expected len=%d\n'%s'\n",
				strlen(stdout_buf), stdout_buf);
		fprintf(stderr, "Got len=%d\n'%s'\n",
				strlen(master_stdout_buf), master_stdout_buf);
	} else {
		printf("PASSED %s TEST %2d FILE STDOUT\n",
				myname, testcase);
	}

	len = fread(master_stderr_buf, sizeof(char), SIZE, stderrfp);
	if( len >= SIZE ) {
		++anyfail;
		fprintf(stderr, "Test file %s too long\n", stderr_name);
		master_stderr_buf[SIZE-1] = '\0';
					/* null terminate the string */
	} else if( strcmp(stderr_buf, master_stderr_buf) != 0 ) {
		++anyfail;
		fprintf(stderr, "FAILED %s TEST %2d FILE STDERR\n",
				myname, testcase);
		fprintf(stderr, "Expected len=%d\n'%s'\n",
				strlen(stderr_buf), stderr_buf);
		fprintf(stderr, "Got len=%d\n'%s'\n",
				strlen(master_stderr_buf), master_stderr_buf);
		fflush(stderr);
	} else {
		printf("PASSED %s TEST %2d FILE STDERR\n",
				myname, testcase);
	}

	if( anyfail ) {
		p = strrchr(cmd_buf, ';');
		if( p != NULL )
			p += 2;
		else
			p = cmd_buf;
		fprintf(stderr, "FAILING COMMAND WAS: '%s'\n", p);
		fflush(stderr);
		exit(1);
	}
}
