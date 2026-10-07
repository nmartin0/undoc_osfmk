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
 * 
 */

/*
 * HISTORY
 * $Log: chkpnt_pgrp.cmd.c,v $
 * Revision 1.2  1994/11/18  21:05:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:52:32  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:14:37  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:02:23  hao
 * 	Initial check-in.
 * 
 */
/*******************************************************************************

Module:		chkpnt_pgrp.cmd

Purpose:	This module tests the TNC command chkpnt_pgrp.  Sample input,
		both correct and erroneous, is given to the command.  Then
		the command's standard output and standard error are
		compared against expected values.  

*******************************************************************************/


#include "../common/vstnc.h"
#include <sys/table.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>

int ntests = 8;		/* number of test cases */

int int_err = 0;
char *myname;

#define DEBUG 1
/*
*/

#define NODECOUNT 3
#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

char casedescript[MAXBUF]; /* desctiption of this testcase  */

int do_test(int);

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
		fprintf(stderr, "usage: chkpnt_pgrp.cmd [1 - %d]\n", ntests);
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

	pid_t	pid;
	char	dir_name[SIZE];
	int	waitstat;
	int	status;

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

	pid = fork();

	if (pid < 0) {
		fprintf(stderr, "testcase %d can't fork.\n", testcase);
		int_err++;
		return(-1);
	}

	if (pid == 0) {
		status = setpgid(0,0);		
		if (status == -1) {
			fprintf(stderr, "testcase %d can't setpgid.\n", testcase);
			exit(-1);
		}
		pause();
		exit(0);
	}

	sprintf(dir_name, "/chkpnt/pgrp.%d", pid);
	mkdir(dir_name,  S_IRWXU | S_IXGRP | S_IXOTH);

	switch( testcase ) {
	case 1: 
		sprintf(casedescript, "Chkpnt_pgrp into a non-empty directory; cmd: chkpnt_pgrp %d", pid);
		sprintf(dir_name, "/chkpnt/pgrp.%d/temp", pid);
		mkdir(dir_name,  S_IRWXU | S_IXGRP | S_IXOTH);
		sprintf(cmd_buf, "/etc/chkpnt_pgrp %d", pid);
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "checkpoint directory non-empty\n");
		break;

	case 2:	

		sprintf(casedescript, "Force the chkpnt into non-empty directory; cmd: chkpnt_pgrp -f %d", pid);
		sprintf(cmd_buf, "/etc/chkpnt_pgrp -f %d", pid);
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;

	case 3:

		sprintf(casedescript, "Kill the target process at the end of chkpnt; cmd: chkpnt_pgrp -k %d", pid);
		sprintf(cmd_buf, "/etc/chkpnt_pgrp -k %d", pid);
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;

	case 4:
		sprintf(casedescript, "Write the chkpnt images into a particular directory; cmd: chkpnt_pgrp -d /chkpnt/UseMe %d", pid);
		rmdir(dir_name);
		sprintf(dir_name, "/chkpnt/UseMe");
		mkdir(dir_name,  S_IRWXU | S_IXGRP | S_IXOTH);
		sprintf(cmd_buf, "/etc/chkpnt_pgrp -d %s %d", dir_name, pid);
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;

	case 5:
		sprintf(casedescript, "Write chkpnt images into non-existing directoriyl; cmd: chkpnt_pgrp -d /chkpnt/NoN-ExIst %d", pid); 
		sprintf(cmd_buf, "/etc/chkpnt_pgrp -d /chkpnt/NoN-ExIst %d", pid);
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "cannot access directory /chkpnt/NoN-ExIst\n");
		break;

	case 6:
		sprintf(casedescript, "Chkpnt without correct permissions; cmd: chkpnt_pgrp %d", pid);
		sprintf(cmd_buf, "/etc/chkpnt_pgrp %d", pid);
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "invalid process group specified\n");
		break;

	case 7:

		sprintf(casedescript, "chkpnt into default directory; cmd: /etc/chkpnt_pgrp %d", pid);
		sprintf(cmd_buf, "/etc/chkpnt_pgrp %d", pid);
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;

	case 8:
		sprintf(casedescript, "chkpnt a non-existing process; cmd: /etc/chkpnt_pgrp %d", pid);
		kill(-pid, SIGKILL);
		wait(&waitstat);
		sprintf(cmd_buf, "/etc/chkpnt_pgrp %d", pid);
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "invalid process group specified\n");
		break;

	default:
	{
		strcat(casedescript, "default: testcase not known.\n");
		fprintf(stderr, "Unknown test case #%d\n\n", testcase);
		goto test_aborted;
	} 
	}  /* end switch */
	
	/* tell user what the testcase is testing.  */
	fprintf(stdout, "%s\n", casedescript);
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

	if (testcase == 6) {
		chmod(stdin_name, S_IRWXU | S_IRWXG | S_IRWXO);
		chmod(stdout_name, S_IRWXU | S_IRWXG | S_IRWXO);
		chmod(stderr_name, S_IRWXU | S_IRWXG | S_IRWXO);
		setuid(config_userid1);
	}

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

	kill(pid, SIGKILL);
}
