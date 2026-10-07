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
 * $Log: fast.cmd.c,v $
 * Revision 1.5  1994/11/18  21:06:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/03/14  17:54:33  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 1.3  1994/02/04  01:42:23  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.2  93/06/09  17:17:58  yazz
 * Regularize VSTNC output.
 * 
 * Revision 3.1  93/03/17  11:52:33  jpaul
 * Use shared memory more logically and move all creation/deletion
 * code to common.c.  Don't try to detach from not attached memory.
 * Various fixes for failing testcases.
 * 
 * Revision 3.0  92/07/22  16:49:45  jpaul
 * Initial Checkin
 * 
 */
/*******************************************************************************

Module:		fast.cmd.c

Purpose:	This module tests the TNC command fast.  Sample input,
		both correct and erroneous, is given to the command.  
		The command's standard output and standard error are then
		compared against expected values.  The fast command
		processes input from a shared memory segment,
		where X is an int value of the node number.  We
		supply sample input for this file in standard input, and
		then it is written into shared memory as part of the 
		test execution.


*******************************************************************************/

#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

#include "../common/vstnc.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/table.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
#define DEBUG 1
*/

int ntests = 13;		/* number of test cases */

char key_file[1024];

int setup_shared();
int shmemdestroy();

int int_err;
char *myname;

struct load_vec_info
{
	int node; /* node number */
	double lm; /* load measure */
};

struct load_info
{
   	int num_elements;
	struct load_vec_info load_vector[200];
} *load_i_ptr;

int do_test(int);
int want_detach = 0;

char casedescript[MAXBUF] = { "" }; /* description of this testcase  */

main(
	int argc,
	char *argv[])
{

	int testcase;		/* The test case number */
	int ret_value;		/* do_test() returns 0 for success else fail */
	int shret;		/* return from shared mem calls */
	char *shmid;		/* shared mem id */


	myname = argv[0];

	/*
	 * First, find out what test the shell asked us to run, checking
	 * the validity of the request as well.
	 */
	if ((argc < 2) || ((testcase = conv_arg(argv[1])) == 0)) {
		fprintf(stderr, "usage: fast.cmd [-K keyfile]  [1 - %d]\n", ntests);
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
do_test(
	int testcase)
{

#define	SIZE 4096
#define NODECOUNT 3

	FILE	*stdinfp, *stdoutfp, *stderrfp;	/* not ours, the test's */
	char	stdin_name[17], stdout_name[17], stderr_name[17];

	char	cmd_buf[SIZE];		/* 4 bufs for setup of test cases */
	char	stdin_buf[SIZE];
	char	stdout_buf[SIZE];
	char	stderr_buf[SIZE];

	char	master_cmd_buf[SIZE];	/* 4 more for performing the tests */
	char	master_stdin_buf[SIZE];
	char	master_stdout_buf[SIZE];
	char	master_stderr_buf[SIZE];

	char	tmp_buf[SIZE];
	int	len;
	int	st;
	char	*p;
	int	i;
	struct  load_vec_info node_n_load[3];	/* what to fill shmem with */
	int	anyfail = 0;
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
	bzero(tmp_buf, SIZE);
	bzero(stdin_buf, SIZE);
	bzero(stdout_buf, SIZE);
	bzero(stderr_buf, SIZE);

	switch( testcase ) {

	case 1:		/* check usage message error handling */
	{
		strcat(casedescript, 
			"Checking usage message error handling, cmd: fast -K /tmp\n");
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}

		want_detach++;
		
		strcpy(cmd_buf, "fast -K /tmp");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "usage: fast [ -v ] command [ arg ] ...\n");
		break;
	}

	case 2: /* check usage message error handling */
	{
		strcat(casedescript, 
			"Checking argument error handling, cmd: fast -x -K /tmp\n");
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;

		strcpy(cmd_buf, "fast -x -K /tmp");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "usage: fast [ -v ] command [ arg ] ...\n");
		break;
	}

	case 3: /* check usage message error handling */
	{
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;
		strcat(casedescript, 
			"Checking argument error handling, cmd: fast -v -K /tmp\n");
		strcpy(cmd_buf, "fast -v -K /tmp");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "usage: fast [ -v ] command [ arg ] ...\n");
		break;
	}

	case 4: /* check usage message error handling */
	{
		
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		
		want_detach++;

		strcat(casedescript, "Checking argument error handling with valid executable, cmd: fast -x -K /tmp /bin/hostname\n");
		strcpy(cmd_buf, "fast -x -K/ tmp /bin/hostname");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "usage: fast [ -v ] command [ arg ] ...\n");
		break;
	}

	case 5:		/* node 0 is least loaded, expect that.
			 * use undocumented -K option to avoid complications
			 * with any load leveler running. 
			 */
	{	
		strcat(casedescript, "Expecting node 0, nonverbose, cmd: fast -K /tmp fast.cmd.remote\n");


		node_n_load[0].node = 0;
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}

		want_detach++;

		strcat(cmd_buf, "fast -K /tmp fast.cmd.remote");

		sprintf(stdout_buf, "stdout: my node is 0\n");
		strcat(stdout_buf, "stdout: argc is 1\n");
		strcat(stdout_buf, "stdout: arg 0 is fast.cmd.remote\n");

		sprintf(stderr_buf, "stderr: my node is 0\n");
		strcat(stderr_buf, "stderr: argc is 1\n");
		strcat(stderr_buf, "stderr: arg 0 is fast.cmd.remote\n");

		break;
	}

	case 6:		/* node 0 is again expected  */
	{
		strcat(casedescript, "Expecting node 0, verbose, cmd: fast -v -K /tmp fast.cmd.remote\n");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}

		want_detach++;

		strcat(cmd_buf, "fast -v -K /tmp fast.cmd.remote");

		strcpy(stdout_buf, "fast node 0\n");
		strcat(stdout_buf, "stdout: my node is 0\n");
		strcat(stdout_buf, "stdout: argc is 1\n");
		strcat(stdout_buf, "stdout: arg 0 is fast.cmd.remote\n");

		strcpy(stderr_buf, "stderr: my node is 0\n");
		strcat(stderr_buf, "stderr: argc is 1\n");
		strcat(stderr_buf, "stderr: arg 0 is fast.cmd.remote\n");

		break;
	}

	case 7:	/* test multiple -v arguments parsed OK.  */
	{
		
		strcat(casedescript, 
			"Checking multiple -v handling, cmd: fast -v -v -K /tmp fast.cmd.remote\n");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}

		want_detach++;

		strcat(cmd_buf, "fast -v -v -K /tmp fast.cmd.remote");
		strcpy(stdout_buf, "fast node 0\n");
		strcat(stdout_buf, "stdout: my node is 0\n");
		strcat(stdout_buf, "stdout: argc is 1\n");
		strcat(stdout_buf, "stdout: arg 0 is fast.cmd.remote\n");

		sprintf(stderr_buf, "stderr: my node is 0\n");
		strcat(stderr_buf, "stderr: argc is 1\n");
		strcat(stderr_buf, "stderr: arg 0 is fast.cmd.remote\n");

		break;
	}

	case 8:		/* try to execute non-existant file */
	{
		strcat(casedescript, 
			"Attempting to execute nonexistant file, nonverbose; cmd: fast -K /tmp ./not_found\n");
		node_n_load[0].node = 0;
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid < 0) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		want_detach++;
		system("rm -f ./not_found > /dev/null 2>&1");

		strcat(cmd_buf, "fast -K /tmp ./not_found");

		strcpy(stdout_buf, "");

		strcpy(stderr_buf, "fast: ./not_found not found.\n");
		break;
	}

	case 9:		/* vflag set, 0 fastest node, exec nonexistant file */
	{
		strcat(casedescript, 
			"Attempting to execute nonexistant file, verbose; cmd: fast -K /tmp ./not_found\n");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		setup_shared(NODECOUNT, node_n_load);
		if (shmid == -1) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}

		want_detach++;

		system("rm -f ./not_found > /dev/null 2>&1");

		strcat(cmd_buf, "fast -v -K /tmp ./not_found");

		strcpy(stdout_buf, "fast node 0\n");

		strcpy(stderr_buf, "fast: ./not_found not found.\n");
		break;
	}

	case 10:	/* execute permission turned off, nonverbose.  */
	{
		strcat(casedescript, 
			"Attempting to execute non-executable file, nonverbose (file permissions test); cmd: fast -K /tmp ./no_exec\n");

		node_n_load[0].node = 0;
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid == -1) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}

		want_detach++;

		system("rm -f no_exec; touch ./no_exec; chmod a-x no_exec");

		strcat(cmd_buf, "fast -K /tmp ./no_exec");

		strcpy(stdout_buf, "");

		strcpy(stderr_buf, "fast: can't execute ./no_exec.\n");
		break;
	}

	case 11:	/*  verbose, non-executable file.  */
	{
		strcat(casedescript, 
			"Attempting to execute nonexistant file, verbose (file permissions test); cmd: fast -K /tmp ./not_found\n");
		node_n_load[0].node = 0;
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid == -1) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}

		want_detach++;

		system("rm -f no_exec; touch ./no_exec; chmod a-x no_exec");

		strcat(cmd_buf, "fast -v -K /tmp ./no_exec");

		strcpy(stdout_buf, "fast node 0\n");

		strcpy(stderr_buf, "fast: can't execute ./no_exec.\n");
		break;
	}

	case 12:	/* try lots of args.  0 is fastest. */
	{
		strcat(casedescript, 
			"Testing argument handling with lots of args, nonverbose; cmd: fast -K /tmp fast.cmd.remote arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9\n");
		node_n_load[0].node = 0;
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid == -1) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}

		want_detach++;

		strcat(cmd_buf, "fast -K /tmp fast.cmd.remote");
		strcat(cmd_buf," arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9");

		sprintf(stdout_buf, "stdout: my node is 0\n");
		strcat(stdout_buf, "stdout: argc is 10\n");
		strcat(stdout_buf, "stdout: arg 0 is fast.cmd.remote\n");
		strcat(stdout_buf, "stdout: arg 1 is arg1\n");
		strcat(stdout_buf, "stdout: arg 2 is arg2\n");
		strcat(stdout_buf, "stdout: arg 3 is arg3\n");
		strcat(stdout_buf, "stdout: arg 4 is arg4\n");
		strcat(stdout_buf, "stdout: arg 5 is arg5\n");
		strcat(stdout_buf, "stdout: arg 6 is arg6\n");
		strcat(stdout_buf, "stdout: arg 7 is arg7\n");
		strcat(stdout_buf, "stdout: arg 8 is arg8\n");
		strcat(stdout_buf, "stdout: arg 9 is arg9\n");

		sprintf(stderr_buf, "stderr: my node is 0\n");
		strcat(stderr_buf, "stderr: argc is 10\n");
		strcat(stderr_buf, "stderr: arg 0 is fast.cmd.remote\n");
		strcat(stderr_buf, "stderr: arg 1 is arg1\n");
		strcat(stderr_buf, "stderr: arg 2 is arg2\n");
		strcat(stderr_buf, "stderr: arg 3 is arg3\n");
		strcat(stderr_buf, "stderr: arg 4 is arg4\n");
		strcat(stderr_buf, "stderr: arg 5 is arg5\n");
		strcat(stderr_buf, "stderr: arg 6 is arg6\n");
		strcat(stderr_buf, "stderr: arg 7 is arg7\n");
		strcat(stderr_buf, "stderr: arg 8 is arg8\n");
		strcat(stderr_buf, "stderr: arg 9 is arg9\n");

		break;
	}

	case 13:	/* error condition, shared mem not setup.  */
	{
		char temp[MAXBUF];
		bzero(temp, MAXBUF);

		sprintf(casedescript, "Testing error condition: shared memory not setup or accessible, \nverbose: should default to this_node(%d); cmd: fast -K /tmp -v fast.cmd.remote\n", node_self());

		/* 
		 * DON'T set up shared mem and check behavior.
		node_n_load[0].node = 0;
		node_n_load[0].lm = 0.0;
		node_n_load[1].node = 1;
		node_n_load[1].lm = 1.1;
		node_n_load[2].node = 2;
		node_n_load[2].lm = 2.2;
		shmid = setup_shared(NODECOUNT, node_n_load);
		if (shmid == -1) {
			++int_err;
			fprintf(stderr, "unable to setup shared memory\n");
			goto test_aborted;
		}
		*/

		want_detach = 0;
		strcat(cmd_buf, "fast -v -K /tmp fast.cmd.remote");

		strcpy(stdin_buf, "");	/* no valid data */

		sprintf(stdout_buf, "fast node %d\nstdout: my node is %d\n", 
				     node_self(), node_self());
		strcat(stdout_buf, "stdout: argc is 1\n");
		strcat(stdout_buf, "stdout: arg 0 is fast.cmd.remote\n");

		strcat(stderr_buf, "Load information not available; using local node.\n");
		sprintf(temp, "stderr: my node is %d\n", node_self());
		strcat(stderr_buf, temp);
		strcat(stderr_buf, "stderr: argc is 1\n");
		strcat(stderr_buf, "stderr: arg 0 is fast.cmd.remote\n");
	

		break;
	}

	default:  
	{
		strcat(casedescript, "Default case -- unknown\n");	
		fprintf(stderr, "Unknown test case #%d\n\n", testcase);
		goto test_aborted;
	}
	}	/* end switch */

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
		fflush(stderr);
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
	}
}

