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
 * $Log: onnode.cmd.c,v $
 * Revision 1.5  1994/11/18  21:07:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/02/04  01:46:18  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.3  93/06/09  17:20:46  yazz
 * Regularize VSTNC output.
 * 
 * Revision 3.2  93/03/01  14:23:06  jpaul
 * Remove ping regression testcase from onnode, There is
 * now a ping test suite.
 * 
 * Revision 3.1  92/12/18  11:50:41  jpaul
 * Add ping regression testcase for VSTNC onnode test.
 * 
 * Revision 3.0  92/07/22  16:52:03  jpaul
 * Initial Checkin
 * 
 */
/*******************************************************************************

Module:		onnode.cmd.c

Purpose:	This module tests the TNC command onnode.  Sample input,
		both correct and erroneous, is given to the command.  Then
		the command's standard output and standard error are
		compared against expected values.  Output from the command
		executed by onnode is included in this checking.


*******************************************************************************/


#include "../common/vstnc.h"
#include <sys/table.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define DEBUG 1
#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

char casedescript[MAXBUF]; /* desctiption of this testcase  */
extern char *config_goodhost;

int ntests = 16;		/* number of test cases */

int int_err;
char *myname;

int do_test(int);

main(
	int argc,
	char *argv[])
{

	int testcase;		/* The test case number */
	int ret_value;		/* do_test() returns 0 for success else fail */

	myname = argv[0];

	/*
	 * First, find out what test the shell asked us to run, checking
	 * the validity of the request as well.
	 */
	if ((argc != 2) || ((testcase = conv_arg(argv[1])) == 0)) {
		fprintf(stderr, "usage: onnode.cmd [1 - %d]\n", ntests);
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

	switch( testcase ) {	/* negative node */
	case 1:
	{
		strcat(casedescript, "Negative node. (Should fail).\n");
		strcpy(cmd_buf, "onnode -5 hostname");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "onnode: -5 is not a valid node number.\n");
		break;
	}

	case 2:
	{		/* out of range node */
		strcat(casedescript, "Out-of-range node. (Should fail).\n");
		sprintf(cmd_buf, "onnode %d hostname", config_hinode+1);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		sprintf(stderr_buf, "onnode: %d is not a valid node number.\n",
				config_hinode+1);
		break;
	}

	case 3:
	{		/* out of range node */
		strcat(casedescript, "Out-of-range node. (Should fail).\n");
		sprintf(cmd_buf, "onnode %d hostname", config_lonode-1);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		sprintf(stderr_buf, "onnode: %d is not a valid node number.\n",
				config_lonode-1);
		break;
	}

	case 4:
	{		/* in range node.  */
		strcat(casedescript, "Valid node.\n");
		sprintf(cmd_buf, "onnode %d onnode.cmd.remote", config_lonode);

		strcpy(stdin_buf, "");

		sprintf(stdout_buf, "stdout: my node is %d\n", config_lonode);
		strcat(stdout_buf, "stdout: argc is 1\n");
		strcat(stdout_buf, "stdout: arg 0 is onnode.cmd.remote\n");

		sprintf(stderr_buf, "stderr: my node is %d\n", config_lonode);
		strcat(stderr_buf, "stderr: argc is 1\n");
		strcat(stderr_buf, "stderr: arg 0 is onnode.cmd.remote\n");

		break;
	}

	case 5:
	{		/* valid node */
		sprintf(cmd_buf, "onnode %d onnode.cmd.remote", config_hinode);
		strcat(casedescript, "Valid node.\n");

		strcpy(stdin_buf, "");

		sprintf(stdout_buf, "stdout: my node is %d\n", config_hinode);
		strcat(stdout_buf, "stdout: argc is 1\n");
		strcat(stdout_buf, "stdout: arg 0 is onnode.cmd.remote\n");

		sprintf(stderr_buf, "stderr: my node is %d\n", config_hinode);
		strcat(stderr_buf, "stderr: argc is 1\n");
		strcat(stderr_buf, "stderr: arg 0 is onnode.cmd.remote\n");

		break;
	}

	case 6:
	{		/* bad node argument */
		strcat(casedescript, "Invalid node (alphanumeric).\n");
		strcpy(cmd_buf, "onnode abc123 hostname");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf,
				"onnode: abc123 is not a valid node number.\n");
		break;
	}

	case 7:
	{		/* bad node argument */
		strcat(casedescript, "Invalid node (alpha chars only).\n");
		strcpy(cmd_buf, "onnode Hello hostname");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "onnode: Hello is not a valid node number.\n");
		break;
	}

	case 8:
	{		/* bad node argument */
		strcat(casedescript, "Invalid node (alphanumeric).\n");
		strcpy(cmd_buf, "onnode ABC123 hostname");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "onnode: ABC123 is not a valid node number.\n");
		break;
	}

	case 9:
	{		/* bad node argument */
		strcat(casedescript, "Invalid node (alphanumeric).\n");
		strcpy(cmd_buf, "onnode 123ABC1 hostname");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "onnode: 123ABC1 is not a valid node number.\n");
		break;
	}

	case 10:
	{		/* octal node representation */
		strcat(casedescript, "Octal node representation.\n");
		sprintf(cmd_buf, "onnode 0%o onnode.cmd.remote", config_goodnode);
		strcpy(stdin_buf, "");
		sprintf(stdout_buf, "stdout: my node is %d\n", config_goodnode);
		strcat(stdout_buf, "stdout: argc is 1\n");
		strcat(stdout_buf, "stdout: arg 0 is onnode.cmd.remote\n");
		sprintf(stderr_buf, "stderr: my node is %d\n", config_goodnode);
		strcat(stderr_buf, "stderr: argc is 1\n");
		strcat(stderr_buf, "stderr: arg 0 is onnode.cmd.remote\n");
		break;
	}

	case 11:
	{		/* small hex node  */
		sprintf(cmd_buf, "onnode 0x%x onnode.cmd.remote", config_goodnode);
		strcat(casedescript, "Lower case hex node representation.\n");

		strcpy(stdin_buf, "");

		sprintf(stdout_buf, "stdout: my node is %d\n", config_goodnode);
		strcat(stdout_buf, "stdout: argc is 1\n");
		strcat(stdout_buf, "stdout: arg 0 is onnode.cmd.remote\n");
		sprintf(stderr_buf, "stderr: my node is %d\n", config_goodnode);
		strcat(stderr_buf, "stderr: argc is 1\n");
		strcat(stderr_buf, "stderr: arg 0 is onnode.cmd.remote\n");
		break;
	}

	case 12:
	{		/* big hex node  */
		sprintf(cmd_buf, "onnode 0X%x onnode.cmd.remote", config_goodnode);
		strcat(casedescript, "Upper case hex node representation.\n");

		strcpy(stdin_buf, "");

		sprintf(stdout_buf, "stdout: my node is %d\n", config_goodnode);
		strcat(stdout_buf, "stdout: argc is 1\n");
		strcat(stdout_buf, "stdout: arg 0 is onnode.cmd.remote\n");

		sprintf(stderr_buf, "stderr: my node is %d\n", config_goodnode);
		strcat(stderr_buf, "stderr: argc is 1\n");
		strcat(stderr_buf, "stderr: arg 0 is onnode.cmd.remote\n");
		break;
	}

	case 13:
	{
			/* no arguments  */
		strcat(casedescript, "No arguments.\n");
		strcpy(cmd_buf, "onnode");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "usage: onnode node command [ arg ] ...\n");
		break;
	}
	
	case 14:
	{		/* nonexistant file to execute */
		strcat(casedescript, "Attempt to execute non-existant file.\n");
		system("rm -f not_found");
		sprintf(cmd_buf, "onnode %d not_found", config_goodnode);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "onnode: not_found not found.\n");
		break;
	}

	case 15:
	{		/* test permissions -- non-executable file */
		strcat(casedescript, "Attempt to execute non-executable file (file permissions test).\n");
		system("rm -f no_exec; touch ./no_exec; chmod a-x no_exec");
		sprintf(cmd_buf, "onnode %d ./no_exec", config_goodnode);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "onnode: can't execute ./no_exec.\n");
		break;
	}

	case 16:
	{		/*  argument list passing  */
		strcat(casedescript, 
			"Test argument list passing (many args).\n");
		sprintf(cmd_buf, "onnode %d onnode.cmd.remote", config_lonode);
		strcat(cmd_buf," arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9");

		strcpy(stdin_buf, "");

		sprintf(stdout_buf, "stdout: my node is %d\n", config_lonode);
		strcat(stdout_buf, "stdout: argc is 10\n");
		strcat(stdout_buf, "stdout: arg 0 is onnode.cmd.remote\n");
		strcat(stdout_buf, "stdout: arg 1 is arg1\n");
		strcat(stdout_buf, "stdout: arg 2 is arg2\n");
		strcat(stdout_buf, "stdout: arg 3 is arg3\n");
		strcat(stdout_buf, "stdout: arg 4 is arg4\n");
		strcat(stdout_buf, "stdout: arg 5 is arg5\n");
		strcat(stdout_buf, "stdout: arg 6 is arg6\n");
		strcat(stdout_buf, "stdout: arg 7 is arg7\n");
		strcat(stdout_buf, "stdout: arg 8 is arg8\n");
		strcat(stdout_buf, "stdout: arg 9 is arg9\n");

		sprintf(stderr_buf, "stderr: my node is %d\n", config_lonode);
		strcat(stderr_buf, "stderr: argc is 10\n");
		strcat(stderr_buf, "stderr: arg 0 is onnode.cmd.remote\n");
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

	default:
	{
		strcat(casedescript, "Default reached, internal error.\n");
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
	}
}
