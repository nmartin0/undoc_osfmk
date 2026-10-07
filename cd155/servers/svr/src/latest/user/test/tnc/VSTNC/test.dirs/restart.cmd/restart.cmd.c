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
 * $Log: restart.cmd.c,v $
 * Revision 1.2  1994/11/18  21:07:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:57:17  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:18:56  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:35:46  hao
 * 	Initial check-in.
 * 
 */
/*******************************************************************************

Module:		restart.cmd.c

Purpose:	This module tests the TNC command restart.  Sample input,
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
#include <sys/fcntl.h>
#include <sys/mode.h>

int ntests = 10;		/* number of test cases */

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

int do_test();

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
		fprintf(stderr, "usage: %s [1 - %d]\n", myname, ntests);
		fflush(stderr);
		exit(1);
	}

	init_config_globals();  /* read config file. */

	/*
	 * This routine executes the test and logs the results.
	 */
	ret_value = do_test(testcase, atoi(argv[2]));

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
do_test(int testcase, pid_t restart_pid)
{
	FILE	*stdinfp, *stdoutfp, *stderrfp;	/* not ours, the test's */
	FILE	*test_out_fp;
	char	stdin_name[100], stdout_name[100], stderr_name[100];
	char	tempin_name[100], tempout_name[100], temperr_name[100];
#	define	SIZE 4096

	char	cmd_buf[SIZE];		/* 4 bufs for setup of test cases */
	char	stdin_buf[SIZE];
	char	stdout_buf[SIZE];
	char	stderr_buf[SIZE];
	char	test_out_buf[SIZE];

	char	master_cmd_buf[SIZE];	/* 4 more for performing the tests */
	char	master_stdin_buf[SIZE];
	char	master_stdout_buf[SIZE];
	char	master_stderr_buf[SIZE];
	char	master_out_buf[SIZE];

	int	len;
	int	st;
	char	*p;
	int	i;
	int	anyfail = 0;

	pid_t	pid, old_pid; 
	char	dir_name[SIZE];
	char	bogus_entry_name[SIZE];
	int	waitstat;
	char	cwd[100];
	char	*test_out_name = "simp_out";
	int	count;

	sprintf(dir_name, "/chkpnt/pgrp.%d", restart_pid);

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

	if (pid != 0) {
	if (testcase == 9) {
		count = 0;
		for (;;) {
			sleep(1);
			waitpid(restart_pid, &waitstat, WUNTRACED);
			if (WIFSTOPPED(waitstat)) {
				kill(-restart_pid, SIGCONT);
				break;
			}
			if (count++ == 20) {
				fprintf(stderr, "FAILED %s resume TEST %2d\n", myname, testcase);
				exit(-1);
			}
		}
	}
		wait(waitstat);
		exit(0);
	}


	switch( testcase ) {

	case 1: 
		sprintf(casedescript, "restart from default dir; cmd: restart");
		sprintf(cmd_buf, "/etc/restart");
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		sprintf(test_out_buf, "pgrp %d\n", getpgrp());
		getwd(cwd);
		sprintf(tempin_name, "%s/%s", cwd, stdin_name);
		sprintf(stdin_name, tempin_name);
		sprintf(tempout_name, "%s/%s", cwd, stdout_name);
		sprintf(stdout_name, tempout_name);
		sprintf(temperr_name, "%s/%s", cwd, stderr_name);
		sprintf(stderr_name, temperr_name);
		chdir(dir_name);
		break;

	case 2:	
		sprintf(casedescript, "restart from specified dir; cmd: restart  %s", dir_name);
		sprintf(cmd_buf, "/etc/restart %s", dir_name);
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		sprintf(test_out_buf, "pgrp %d\n", getpgrp());
		printf("exected test buf output is %s\n", test_out_buf);
		break;

	case 3:
		sprintf(casedescript, "restart with incorrect permission; cmd: restart %s", dir_name);
		sprintf(cmd_buf, "/etc/restart %s", dir_name);
		strcpy(stdout_buf, "");
		sprintf(stderr_buf, "restart failed: No child processes\n");
		strcpy(test_out_buf, "");
		chmod(stdin_name, S_IRWXU | S_IRWXG | S_IRWXO);
		chmod(stdout_name, S_IRWXU | S_IRWXG | S_IRWXO);
		chmod(stderr_name, S_IRWXU | S_IRWXG | S_IRWXO);
		chmod("/dev/tty", S_IRWXU | S_IRWXG | S_IRWXO);
		setuid(config_userid1);
		break;
	case 4:
		sprintf(casedescript, "restart with incomplete images; cmd: rstart %s", dir_name); 
		sprintf(bogus_entry_name, "%s/0.%d.%d.0.stat", dir_name, restart_pid, restart_pid);
		creat(bogus_entry_name, O_RDWR);
		sprintf(cmd_buf, "/etc/restart %s", dir_name);
		strcpy(stdout_buf, "");
		strcpy(test_out_buf, "");
		sprintf(stderr_buf, "restart failed: Invalid argument\n");
		break;
	case 5:
		sprintf(casedescript, "restart processes with pids already in use; cmd: restart %s", dir_name);
		sprintf(cmd_buf, "/etc/restart %s", dir_name);
		old_pid = forkfamily(1, &restart_pid);

		if (old_pid == 0) {
			pause();
			exit(0);
		}

		if (old_pid < 0) {
			fprintf(stderr, "Can't forkfamily with pid %d.\n", restart_pid);
			goto test_aborted;
		}

		strcpy(stdout_buf, "");
		sprintf(stderr_buf, "restart: process %d already exists\n", restart_pid);
		break;

	case 6:
		sprintf(casedescript, "force restart with pids aleady in use; cmd: restart -f %s", dir_name);
		sprintf(cmd_buf, "/etc/restart -f %s", dir_name);

		old_pid = forkfamily(1, &restart_pid);

		if (old_pid == 0) {
			sleep(50);
			exit(0);
		}

		if (old_pid < 0) {
			fprintf(stderr, "Can't forkfamily with pid %d.\n", restart_pid);
			goto test_aborted;
		}

		strcpy(test_out_buf, "");
		strcpy(stdout_buf, "");
		sprintf(stderr_buf, "restart: killing process %d\n"
		       "restart: process not killed\n", restart_pid);
		break;

	case 7:
		sprintf(casedescript, "restart processes with the pgid of the restart command; cmd: /etc/restart -g %s", dir_name);
		sprintf(test_out_buf, "pgrp %d\n", getpgrp());
		sprintf(cmd_buf, "/etc/restart -gp %s", dir_name);
		strcpy(stdout_buf, "");
		sprintf(stderr_buf, "restart: process group id %d (2 processes)\n", getpgrp());
		break;

	case 8:
		sprintf(casedescript, "restart processes as distinct processes; cmd: restart -p %s", dir_name);
		sprintf(cmd_buf, "/etc/restart -p %s", dir_name);
		sprintf(stderr_buf, "restart: process group id %d (2 processes)\n", restart_pid);
		sprintf(test_out_buf, "pgrp %d\n", restart_pid);
		strcpy(stdout_buf, "");
		break;

	case 9:
		sprintf(casedescript, "restart all processes with stopped state; cmd: /etc/restart -s %s", dir_name);
		sprintf(cmd_buf, "/etc/restart -s %s", dir_name);
		strcpy(stdout_buf, "");
		strcpy(stderr_buf, "");
		break;

	case 10:
		sprintf(casedescript, "restart command waits for all processes to terminate; cmd: restart -pw %s", dir_name);
		sprintf(cmd_buf, "/etc/restart -pw %s", dir_name);
		strcpy(stdout_buf, "");
		sprintf(stderr_buf, "restart: process group id %d (2 processes)\nrestart: waiting for all children to exit\nrestart: all children exited\n", restart_pid);
		sprintf(test_out_buf, "pgrp %d\n", restart_pid);
		
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

	st = system(master_cmd_buf);	/* perform the command */

	if( st == 127 ) {
		fprintf(stderr, "Can't execute shell with 'system'.\n");
		fprintf(stderr, "Command was: %s\n", master_cmd_buf);
		goto test_aborted;
	}

	if (testcase == 1)
		chdir(cwd);

	if (testcase == 5)
		kill(restart_pid, SIGKILL);

	/*
	 * Since child are off on their own, we will need to kludge
	 * a wait here.
	 */
	if (testcase == 7 || testcase == 8)
		sleep(15);
		
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

	test_out_fp = fopen(test_out_name, "r");
	if( test_out_fp == NULL ) {
		++int_err;
		fprintf(stderr, "Can't open test file output %s\n", test_out_name);
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

	len = fread(master_out_buf, sizeof(char), SIZE, test_out_fp);
	if( len >= SIZE ) {
		++anyfail;
		fprintf(stderr, "Test file %s too long\n", test_out_name);
		master_stderr_buf[SIZE-1] = '\0';
					/* null terminate the string */
	} else if( strcmp(test_out_buf, master_out_buf) != 0 ) {
		++anyfail;
		fprintf(stderr, "FAILED %s TEST %2d FILE TEST_OUT_FILE\n",
				myname, testcase);
		fprintf(stderr, "Expected len=%d\n'%s'\n",
				strlen(test_out_buf), test_out_buf);
		fprintf(stderr, "Got len=%d\n'%s'\n",
				strlen(master_out_buf), master_out_buf);
		fflush(stderr);
	} else {
		printf("PASSED %s TEST %2d FILE TEST_OUT_FILE\n",
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
