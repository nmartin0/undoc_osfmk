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
 * $Log: kill3.cmd.c,v $
 * Revision 1.5  1994/11/18  21:06:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/02/04  01:45:09  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.3  93/06/09  17:19:58  yazz
 * Regularize VSTNC output.
 * Also fixed bug in send-to-own-pgroup case, which was resulting in the
 * untimely termination of the test program itself, before any output logged.
 * 
 * Revision 3.2  93/03/01  14:20:20  jpaul
 * Reorder command line args.
 * 
 * Revision 3.1  92/12/02  16:38:06  jpaul
 * Sleep longer so interrupt will be caught.
 * 
 * Revision 3.0  92/07/22  16:51:19  jpaul
 * Initial Checkin
 * 
 */
/*******************************************************************************

Module:		kill3.cmd.c

Purpose:	This module tests the TNC command kill3.  Sample input,
		both correct and erroneous, is given to the command.  Then
		the command's standard output and standard error are
		compared against expected values.  Output from the command
		executed by kill3 is included in this checking.  Child tasks
		are forked to catch signals sent by these tests.


*******************************************************************************/

#define TESTKEYFILE "/tmp/fastkeytest"
#define DEBUG 1
#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

#include "../common/vstnc.h"
#include <sys/table.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#ifndef SIGMIGRATE
# define SIGMIGRATE 32
#endif

char casedescript[MAXBUF]; /* desctiption of this testcase  */

int ntests = 21;			/* number of test cases */

int expected_sig = SIGMIGRATE;	/* does not change for any test */
long expected_child_arg;
int expected_self_sig = 0;	/* not expecting signal to self */
int actual_self_sig = 0;	/* didn't get one yet */
#define MAXPIDS	17
int pidlist[MAXPIDS];		/* entry is 0 if no child proc at all */
int expected_exit[MAXPIDS];
int actual_exit[MAXPIDS];

#define SIGCAUGHT_ARGOK		(0 << 8)
#define SIGCAUGHT_ARGBAD	(1 << 8)
#define NOSIGCAUGHT		(2 << 8)

char *statnames[] = {
	"SIGCAUGHT_ARGOK",
	"SIGCAUGHT_ARGBAD",
	"NOSIGCAUGHT"
};


int int_err;
char *myname;
int testcase;		/* The test case number */
int test_driver_pid;

int do_test();
void sig_catcher();
void abort_test();

main(
	int argc,
	char *argv[])
{

	int ret_value;		/* do_test() returns 0 for success else fail */

	myname = argv[0];
	test_driver_pid = getpid();

	/*
	 * First, find out what test the shell asked us to run, checking
	 * the validity of the request as well.
	 */
	if ((argc != 2) || ((testcase = conv_arg(argv[1])) == 0)) {
		fprintf(stderr, "usage: kill3.cmd [1 - %d]\n", ntests);
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

	char	tmp_buf[SIZE];	

	int	len;
	int	st;
	char	*p;
	int	n;
	int	anyfail = 0, exitstatusfailed = 0;

	for( n=0; n<MAXPIDS; ++n )
		{ pidlist[n] = 0; }

	/*
	 * Generate stdin, stdout and stderr filenames and
	 * create all three files for writing.
	 */
	sprintf(stdin_name, "test.%02d.stdin", testcase);
	stdinfp = fopen(stdin_name, "w+");
	if( stdinfp == NULL ) {
		++int_err;
		fprintf(stderr, "Can't create test file %s\n", stdin_name);
		abort_test();
	}

	sprintf(stdout_name, "test.%02d.stdout", testcase);
	stdoutfp = fopen(stdout_name, "w+");
	if( stdoutfp == NULL ) {
		++int_err;
		fprintf(stderr, "Can't create test file %s\n", stdout_name);
		abort_test();
	}

	sprintf(stderr_name, "test.%02d.stderr", testcase);
	stderrfp = fopen(stderr_name, "w+");
	if( stderrfp == NULL ) {
		++int_err;
		fprintf(stderr, "Can't create test file %s\n", stderr_name);
		abort_test();
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

	/*
	 * Set up to catch signals.  We don't usually catch any but our
	 * child tasks often do.
	 */
	signal(SIGMIGRATE, sig_catcher);

	switch( testcase ) {
	case 1:		/* pid: decimal */
	{
		strcat(casedescript, 
			"Catching Signal: SIGMIGRATE, pid in decimal.\n");

		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();	/* child gets its own pgrp */

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 -32 %d %d", pidlist[0], pidlist[1]);
					/* Yes: pidlist[0] is the ARG value */
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 2:		/* pid: octal */
	{
		strcat(casedescript, 
			"Catching Signal: SIGMIGRATE, pid in octal.\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 -32 0%o 0%o", pidlist[0], pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 3:		/* pid: hex, small x in 0x */
	{
		strcat(casedescript, 
			"Catching Signal: SIGMIGRATE, pid in hex (0x).\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 -32 0x%x 0x%x", pidlist[0], pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 4:		/* pid: hex, big X in 0X */
	{
		strcat(casedescript, 
			"Catching Signal: SIGMIGRATE, pid in hex (0X).\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 -32 0X%X 0X%X", pidlist[0], pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 5:		/* zero means test driver's whole process group */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, pid is 0 test prog's pgrp.\n");
		expected_exit[0] = NOSIGCAUGHT;	/* child in different pgrp */
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[1] = make_child_proc();

		expected_self_sig = 1;		/* we DO expect to get a sig */
		sprintf(cmd_buf, "kill3 -32 %d 0", pidlist[0]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 6:		/* pgrp: decimal */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, process group, decimal.\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();	/* child gets its own pgrp */

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = -pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 -32 -%d -%d", pidlist[0], pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 7:		/* pgrp: octal */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, process group, octal.\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = -pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 -32 -0%o -0%o", pidlist[0], pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 8:		/* pgrp: hex, small x in 0x */
	{
		strcat(casedescript, "Catching Signal: SIGMIGRATE, process group, small hex (0x).\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = -pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 -32 -0x%x -0x%x", pidlist[0],
				pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 9:		/* pgrp: hex, big X in 0X */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, process group, big hex (0X).\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = -pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 -32 -0X%X -0X%X", pidlist[0],
				pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 10:	/* pid: "-SIGMIGRATE" */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, process id -SIGMIGRATE.\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = pidlist[0];
		pidlist[1] = make_child_proc();
		sprintf(cmd_buf, "kill3 -SIGMIGRATE %d %d", pidlist[0],
				pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 11:	/* pid: "-MIGRATE" */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, process id -MIGRATE.\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 -MIGRATE %d %d", pidlist[0],
				pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 12:	/* pid: no signo arg */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, no signo argument.\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 %d %d", pidlist[0], pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 13:	/* pgrp: "-SIGMIGRATE" */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, process group id -SIGMIGRATE.\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = -pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 -SIGMIGRATE -%d -%d", pidlist[0],
				pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 14:	/* pgrp: "-MIGRATE" */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, process group id -MIGRATE.\n");
		expected_exit[0] = NOSIGCAUGHT;
		/* expected_child_arg does not matter with NOSIGCAUGHT */
		pidlist[0] = make_child_proc();

		expected_exit[1] = SIGCAUGHT_ARGOK;
		expected_child_arg = -pidlist[0];
		pidlist[1] = make_child_proc();

		sprintf(cmd_buf, "kill3 -MIGRATE -%d -%d", pidlist[0],
				pidlist[1]);
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 15:	/* Bogus pid/pgrp string */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, bogus pid/pgrp string.\n");
		strcpy(cmd_buf, "kill3 -32 -1 abc123");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf,
		 "kill3: non-numeric process_ID abc123 skipped.\n");
		break;
	}

	case 16:	/* Bogus pid/pgrp string */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, bogus pid/pgrp string.\n");
		strcpy(cmd_buf, "kill3 -32 -1 123abc");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf,
		 "kill3: non-numeric process_ID 123abc skipped.\n");
		break;
	}

	case 17:	/* Bogus arg string */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, bogus arg string.\n");
		strcpy(cmd_buf, "kill3 -32 abc123 0");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "kill3: abc123 is not a numeric arg.\n");
		break;
	}

	case 18:	/* Bogus arg string */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, bogus arg string.\n");
		strcpy(cmd_buf, "kill3 -32 123abc 0");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "kill3: 123abc is not a numeric arg.\n");
		break;
	}

	case 19:	/* multi-pid test */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, multi-pid.\n");
		expected_child_arg = getpid();
		sprintf(cmd_buf, "kill3 -MIGRATE %d", expected_child_arg);

		for( n=0; n<MAXPIDS; ++n ) {
			expected_exit[n] = SIGCAUGHT_ARGOK;
			pidlist[n] = make_child_proc();
			sprintf(tmp_buf, " %d", pidlist[n]);
			strcat(cmd_buf, tmp_buf);
		}

		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 20:	/* multi-pgrp test */
	{
		strcat(casedescript, 
		"Catching Signal: SIGMIGRATE, multi-pgrp.\n");
		expected_child_arg = getpid();
		sprintf(cmd_buf, "kill3 -MIGRATE %d", expected_child_arg);

		for( n=0; n<MAXPIDS; ++n ) {
			expected_exit[n] = SIGCAUGHT_ARGOK;
			pidlist[n] = make_child_proc();
			sprintf(tmp_buf, " -%d", pidlist[n]);
			strcat(cmd_buf, tmp_buf);
		}

		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;
	}

	case 21:
	{
		strcat(casedescript, 
		"Catching Signal: no arguments.\n");
		strcpy(cmd_buf, "kill3");
		strcpy(stdin_buf, "");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "usage: kill3 [ -signal_name | -signal_number ] arg process_ID ...\n");
		break;
	}

	default:
	{
		strcat(casedescript, 
		"Catching Signal: default.\n");
		fprintf(stderr, "Unknown test case #%d\n\n", testcase);
		abort_test();
	}
	}	/* end switch */
	
	/* tell user what the testcase is testing.  */
	printf("%s", casedescript);
	fflush(stderr);

	/*
	 * Write our standard input string to the standard input file.
	 * Multiple lines of input can be placed in a single string.
	 */
	strcpy(master_stdin_buf, stdin_buf);
	len = strlen(master_stdin_buf);
	st = fwrite(master_stdin_buf, sizeof(char), len, stdinfp);
	if( st != len ) {
		fprintf(stderr, "Can't write into test file %s\n", stdin_name);
		abort_test();
	}

	/*
	 * Now close all three files.  Standard output and error should
	 * be zero length.
	 */
	if( fclose(stdinfp) == EOF ) {
		fprintf(stderr, "Can't close test file %s errno=%d\n",
				stdin_name, errno);
		abort_test();
	}
	if( fclose(stdoutfp) == EOF ) {
		fprintf(stderr, "Can't close test file %s errno=%d\n",
				stdout_name, errno);
		abort_test();
	}
	if( fclose(stderrfp) == EOF ) {
		fprintf(stderr, "Can't close test file %s errno=%d\n",
				stderr_name, errno);
		abort_test();
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

	/*********************************************************/

	st = system(master_cmd_buf);	/* perform the command */

	/*********************************************************/

	if( st == 127 ) {
		fprintf(stderr, "Can't execute shell with 'system'.\n");
		fprintf(stderr, "Command was: %s\n", master_cmd_buf);
		abort_test();
	}

	/*
	 * Wait for our child processes to terminate and compare each
	 * one's actual versus expected status.
	 */
	for( n=0; n<MAXPIDS; ++n ) {
		if( pidlist[n] != 0 ) {
			st = waitpid(pidlist[n], &actual_exit[n], 0);
			if( st != pidlist[n] ) {
				++int_err;
				fprintf(stderr, "waitpid fail n=%d p=%d e=%d\n",
						n, pidlist[n], errno);
				abort_test();
			}
			if( actual_exit[n] != expected_exit[n] ) {
				++anyfail;
				++exitstatusfailed;
				fprintf(stderr,
				 "FAILED %s TEST %2d EXIT STATUSES\n",
				 myname, testcase);
				fprintf(stderr,
				 "Expected pid %d status 0x%x (%s) got 0x%x\n",
				 pidlist[n], expected_exit[n],
				 statnames[expected_exit[n]>>8],
				 actual_exit[n]);
				fflush(stderr);
			}
		}
	}
	if( exitstatusfailed == 0 ) {
		printf("PASSED %s TEST %2d EXIT STATUSES\n",
				myname, testcase);
	}

	/*
	 * Now check the file results.  Standard output and standard error
	 * should match exactly what we expected.
	 */
	stdoutfp = fopen(stdout_name, "r");
	if( stdoutfp == NULL ) {
		++int_err;
		fprintf(stderr, "Can't reopen test file %s\n", stdout_name);
		abort_test();
	}

	stderrfp = fopen(stderr_name, "r");
	if( stderrfp == NULL ) {
		++int_err;
		fprintf(stderr, "Can't reopen test file %s\n", stderr_name);
		abort_test();
	}

	/*
	 * Next, read the files into their buffers and compare them.
	 * Log the results.
	 */
	len = fread(master_stdout_buf, sizeof(char), SIZE, stdoutfp);
	if( len >= SIZE ) {
		++anyfail;
		fprintf(stderr, "Test file %s too long\n", stdout_name);
		fflush(stderr);
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
		fflush(stderr);
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

	/*
	 * Last we make sure we did/did not get a signal ourselves,
	 * as expected/not expected.
	 */
	if( expected_self_sig == actual_self_sig ) {
		printf("PASSED %s TEST %2d SELF_SIG\n", myname, testcase);
	} else {
		fprintf(stderr, "FAILED %s TEST %2d SELF_SIG\n",
		 myname, testcase);
		if( expected_self_sig ) {
			fprintf(stderr,
			 "Expected to get a signal but did not get one.\n");
		} else {
			fprintf(stderr,
			 "Expected no signal but caught one nonetheless.\n");
		}
		fprintf(stderr, "FAILING COMMAND WAS: '%s'\n", p);
		fflush(stderr);
	}
}

/*******************************************************************************

Function:	sig_catcher

Returns:	None.  Exits with 0 if expected sig and arg caught and
		exits with 1 if unexpected sig or unexpected arg are caught.
		However, if we find we are in fact the test driver (i.e. our
		pid matches) then we set a flag and merely return.

Parameters:	signo and arg, passed by the signal catching mechanism

*******************************************************************************/

void
sig_catcher(
	int	sig,
	long	arg)
{
	if( getpid() == test_driver_pid ) {
		actual_self_sig = 1;	/* say we got a sig ourselves */
		return;
	}

	if( sig == expected_sig  &&  arg == expected_child_arg )
		{ exit(0); }
	exit(1);
}

/*******************************************************************************

Function:	do_child

Returns:	None.  Exits with 2 if no signal is caught in a specified
		number of seconds.  If a signal is caught, the sig_catcher
		routine catches it and exits itself.

Parameters:	???

*******************************************************************************/

#define SLEEP_TIME 17

do_child()
{
	sleep(SLEEP_TIME);	/* if a sig coming expect it in this time */

	exit(2);		/* no signal came */
}

/*******************************************************************************

Function:	make_child_proc

Description:	Forks one child process locally, and has it call the
		do_child() routine.  If the fork fails we abort the test.
		Thus, only the parent is returned to from here.  The
		child task is made its own process group leader as well.

Returns:	Child's pid for success, else we abort test and don't return

Parameters:	None.

*******************************************************************************/


make_child_proc()
{
	int pid;
	int st;

	pid = fork();
	if( pid == -1 ) {
		++int_err;
		fprintf(stderr, "Can't fork child for test %d\n", testcase);
		abort_test();
	}
	else if( pid == 0 )
		{ do_child(); }	/* does not return */

	st = setpgrp(pid, pid);
	if( st != 0 ) {
		fprintf(stderr, "Can't setpgrp child for test %d\n", testcase);
		abort_test();
	}
	return( pid );
}

/*******************************************************************************

Function:	abort_test

Returns:	None.  Exits.

Parameters:	None.

*******************************************************************************/

void
abort_test()
{
	fprintf(stderr, "\nTEST ABORTED\n\n");
	fflush(stderr);
	exit( -1 );
}
