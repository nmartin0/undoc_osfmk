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
 * $Log: kill3.call.c,v $
 * Revision 1.4  1994/11/18  21:06:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  01:43:52  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.2  93/07/12  17:25:52  yazz
 * Regularized output.
 * Made test use its own name, instead of some other test's name.
 * 
 * Revision 3.1  93/06/09  17:18:30  yazz
 * Regularize VSTNC output.
 * 
 * Revision 3.0  92/07/22  16:51:03  jpaul
 * Initial Checkin
 * 
 */
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/signal.h>
#include "../common/vstnc.h"

#ifndef FALSE
#define FALSE 0
#endif /* !FALSE */
#ifndef TRUE 
#define TRUE  1
#endif /* !TRUE */

#define DEBUG 1
#undef DEBUG

#define MAXBUF 255

/*
 * Number of tests.
 */
int ntests = 8;

/*
 * An arbitrary node number for testing kill3 argument passing.
 */
#define TEST_NODE 500

/*
 * Amount of time parent and child should sleep before either
 * delievering the signal or time out waiting for a signal.
 */
#define PARENT_SLEEP_TIME 5
#define CHILD_SLEEP_TIME 10

typedef short bool_t;

char casedescript[MAXBUF] = { "" };	/* description of this testcase  */

int do_test(int, bool_t *);
int int_err;
bool_t CAUGHT_SIG;

#ifndef SIGMIGRATE
#define SIGMIGRATE 32
#endif /* !SIGMIGRATE */

/****************************************************************/
/*								*/
/* Module:	kill3.call					*/
/*								*/
/* Purpose:	This module is part of a test of the TNC 	*/
/*		function kill3().  Following TNC testing	*/
/*		templates, the main program just calls the	*/
/*		test subroutines with the correct test cases	*/
/*		and logs the results.				*/
/*								*/
/* Modification History:					*/
/*								*/
/* Date		Reason						*/
/* --------	------						*/
/*								*/
/* 03/30/92	Began coding.					*/
/* 06/23/92	Document testcases -- jph			*/
/*								*/
/* See Also:							*/
/*								*/
/****************************************************************/

main(argc, argv, envp)
int argc;
char *argv[], *envp[];
{
	int test_case, return_val;	/* The test case number and return */
	bool_t positive = TRUE;		/* Test case type is usually positive */
	void log_results();		/* Write test results to log file */
	char *myname = argv[0];

	int_err = FALSE;

	/* 
	 * First, find out what test the shell asked us to run, 
	 * checking the validity of the request as well. 
	 */
	if ((argc != 2) || ((test_case = conv_arg(argv[1])) == 0)) {
		fprintf(stderr, "usage:  %s [1 - %d]\n", myname, ntests);
		fflush(stderr);
		exit(1);
	}

	init_config_globals();

	/* This routine executes the tests. */
	return_val = do_test(test_case, &positive);

	/* Handle internal bugs if found. */
	if (int_err) {
		fprintf(stderr, "Internal error:  test case (%d).\n",test_case);
		fflush(stderr);
		exit(1);
	}

	/* Write the results to the log file, whatever they were. */
	log_results(positive, test_case, return_val);

	exit(0);
}


/****************************************************************/
/*								*/
/* Function:	do_test						*/
/*								*/
/* Returns:	the results of calling either test_simple() or	*/
/*		test_fancy() depend on test case.		*/
/*								*/
/* Parameters:	test_case, the test case number, valid between  */
/*		1 and ntests.					*/
/*								*/
/*		ptr_positive, a pointer to a flag the routine   */
/*		clears if the test cases is a negative one.	*/
/*		In this case, they all should be postive.	*/
/*								*/
/* Purpose:	This function executes the tests.  The test 	*/
/*		cases correspond to the cases in section 3 of   */
/*		the FV plan. 					*/
/*								*/
/*								*/
/****************************************************************/

int
do_test(test_case, ptr_positive)
int test_case;
bool_t *ptr_positive;
{
	extern int test_simple(int);
	extern int test_fancy(bool_t, bool_t, int);
	int 	return_val;
	int 	pid;
	int 	this_node, valid_node;

	/*
	 * Determine the node number of this node.
	 */
	this_node = node_self();

	/*
	 * Also find a valid node other than node_self().
	 */
	valid_node = config_goodnode;

	switch (test_case) {
		/*
		 * Negative test: kill3 a non-valid pid. (local)
		 */
		case 1:
		{
			strcat(casedescript, "Attempting to kill3 a non-valid local pid, should fail.\n");
			*ptr_positive = FALSE;
			if ((pid = get_bad_pid()) == -1) {
				int_err = TRUE;
				fprintf(stderr, "kill3(): fork failed.\n");
				fflush(stderr);
				break;
			}
			return_val = test_simple(pid);
			break;
		}

		/*
		 * Positive test: kill3 calling process itself. (local)
		 */
		case 2:	
		{
			strcat(casedescript, "Suicide: (kill own pid) should pass.\n");
			*ptr_positive = TRUE;
			return_val = test_simple(getpid());
			break;
		}

		/*
		 * Positive test: kill3 another process with same
		 * uid. (local)
		 */
		case 3:	
		{
			strcat(casedescript, 
			"kill3 local pid with same uid; should pass.\n");
			*ptr_positive = TRUE;
			return_val = test_fancy(FALSE, TRUE, this_node);
			break;
		}

		/*
		 * Negative test: non-root process kill3 another process 
		 * with different uid. (local)
		 */
		case 4:	
		{	
		strcat(casedescript, "Non-root process trying to kill process with another uid; should fail.\n");
			*ptr_positive = FALSE;
			return_val = test_fancy(FALSE, FALSE, this_node);
			break;
		}

		/*
		 * Positive test: kill3 a remote process with same
		 * uid. (remote)
		 */
		case 5:	
		{
			strcat(casedescript, "Non-root process trying to kill remote process with same uid; should pass.\n");
			*ptr_positive = TRUE;
			return_val = test_fancy(FALSE, TRUE, valid_node);
			break;
		}

		/*
		 * Negative test: non-root process kill3 a remote process 
		 * with different uid. (remote)
		 */
		case 6:	
		{
			strcat(casedescript, "Non-root process trying to kill remote process with different uid; should fail.\n");
			*ptr_positive = FALSE;
			return_val = test_fancy(FALSE, FALSE, valid_node);
			break;
		}

		/*
		 * Positive test: kill3 a local process with different
		 * uid as root. (local)
		 */
		case 7:	
		{
			strcat(casedescript, "Root process trying to kill local process with different uid; should pass.\n");
			*ptr_positive = TRUE;
			return_val = test_fancy(TRUE, FALSE, this_node);
			break;

		}

		/*
		 * Positive test: kill3 a remote process with different
		 * uid as root. (remote)
		 */
		case 8:	
		{
			strcat(casedescript, "Root process trying to kill remote process with different uid; should pass.\n");
			*ptr_positive = TRUE;
			return_val = test_fancy(TRUE, FALSE, valid_node);
			break;
		}

		default: /* Internal error if passed a bad flag */
		{
			strcat(casedescript, "Unknown testcase.\n");
			int_err = TRUE;
			break;
		}
	}	/* end switch */

	/* tell user what the testcase is testing.  */
	printf("%s", casedescript);
	fflush(stdout);

	return(return_val);
}

									
/************************************************************************
/*
/* Function:	test_simple()
/*
/* Returns:	1 or -1, depends on if the kill3 to the specified pid
/*		is successful.
/*
/* Parameter:	pid, the id number of the process to send the kill3 signal
/*		to.
/*
/* Purpose:	Use kill3() to send the migrate signal to a specified
/*		process, and check the results.
/* 
/* Modification History
/*
/* Date		Reason
/* ----		------
/*
/* 03/30/92	Began Coding
/*
/*************************************************************************/

int
test_simple(int pid)
{
	extern void simple_catcher(int, int);
	int status;

	/*
	 * Set the handler in case we get the signal.
	 */
	signal(SIGMIGRATE, simple_catcher);

	CAUGHT_SIG = FALSE;

	status = kill3(pid, SIGMIGRATE, TEST_NODE);
	if (status == -1) {
		printf("test_simple: status = -1.\n");
		return(-1);
	} else
		printf("test_simple: go on to wait and see if signal is delivered.\n");

	/*
	 * Wait here and see if a signal is delievered.
	 */
	sleep(CHILD_SLEEP_TIME);

	/*
	 * Return the results.
	 */
	if (CAUGHT_SIG == TRUE) 
		return(1);
	else
		return(-1);
}

/************************************************************************
/*
/* Function:	simple_catcher()
/*
/* Returns:	void
/*
/* Parameter:	sig_no, the signal number delievered us here 
/*		node_number, an arbitrary node for check paramter passing
/*
/* Purpose:	handle the migrate signal, and check the parameters.
/* 
/*
/*************************************************************************/

void
simple_catcher(int sig_no, int node_number)
{
	if (node_number == TEST_NODE && sig_no == SIGMIGRATE)
		CAUGHT_SIG = TRUE;
	else
		CAUGHT_SIG = FALSE;
	return;
}

/************************************************************************
/*
/* Function:	get_bad_pid()
/*
/* Returns:	a "dead" pid number
/*
/* Parameter:	none
/*
/*
/* Purpose:	Generate a pid that is no longer valid.
/* 
/* Modification History
/*
/*
/*************************************************************************/

int
get_bad_pid()
{
	int pid;

	pid = fork();
	/*
	 * If this is the child pid, just exit.
	 */
	if (pid == 0) {
		printf("get_bad_pid: this is the child pid.\n");
		exit(0);
	}

	/*
	 * If this is the parent, wait for the child to exit, then
	 * return the pid.
	 */
	if (pid > 0) {
		printf("get_bad_pid: this is the parent, wait for child.\n");
		wait(0);
		printf("get_bad_pid: this is the partnt, return pid %d.\n",pid);
		return(pid);
	}

	/* 
	 * Fork failed!  Log error.
	 */
	return(-1);
}

/************************************************************************
/*
/* Function:	test_fancy()
/*
/* Returns:	1 or -1, depends on if the kill3 are correctly delieverd
/*		and received by various processes.
/*
/* Parameter:	is_root, this test should be run as root
/*		same_uid, test the two processes with same uid
/*		node_num, the node to run the child process on
/*
/* Purpose:	Use kill3() to send the migrate signal to a various
/*		processes, and check the correctness of delievering
/*		and receiving.
/*
/*************************************************************************/

int
test_fancy(bool_t is_root, bool_t same_uid, int node_num)
{
	extern void migrate_catcher(int, int);
	int pid;
	int status, child_status;

	/*
	 * If we are not testing as root and we do not need to
	 * set other uids, setuid to something else.
 	 */
	if (same_uid == TRUE) {
		status = setuid(config_userid1);
		if (status == -1) {
			int_err = TRUE;
			fprintf(stderr, "kill3(): setuid failed.\n");
			fflush(stderr);
			return(-1);
		}
	}

	/* 
	 * Create a new child process to send signal to.
	 */
	pid = rfork(node_num);

	/*
	 * If this is the child, setup migrating handler, and wait
	 * for the parent to send the kill3 signal.  If sleep time
	 * expires, it is assumed that no migrate signal was 
	 * delivered.
	 */
	if (pid == 0) {
		/*
		 * If we are testing kill3 to different uids, setup
		 * new uid here.
		 */
		if (same_uid == FALSE) {
			status = setuid(config_userid2);
			if (status == -1) {
				int_err = TRUE;
				fprintf(stderr, "kill3(): setuid failed.\n");
				fflush(stderr);
				return(-1);
			}
		}
		signal(SIGMIGRATE, migrate_catcher);
		sleep(CHILD_SLEEP_TIME);
		exit(1);
	}

	/* 
	 * The parent process issues the kill3 call and waits for the
	 * child to exit.
	 */
	if (pid > 0) {
		/*
		 * If we are not testing as root and we need send kill
		 * to a different uid, setuid first.
	 	 */
		if (is_root == FALSE && same_uid == FALSE) {
			status = setuid(config_userid1);
			if (status == -1) {
				int_err = TRUE;
				fprintf(stderr, "kill3(): setuid failed.\n");
				fflush(stderr);
				return(-1);
			}
		}
		sleep(PARENT_SLEEP_TIME);
		status = kill3(pid, SIGMIGRATE, TEST_NODE);
		if (status == -1){
			return(-1);
		}

		wait(&child_status);
		if (child_status != 0)
			return(-1);
		else
			return(1);
	}	

	/* 
	 * If we reach here, internal error.
	 */
	int_err = TRUE;
	fprintf(stderr, "kill3(): rfork failed.\n");
	fflush(stderr);
	return(-1);
}

/************************************************************************
/*
/* Function:	migrate_catcher()
/*
/* Returns:	void
/*
/* Parameter:	sig_no, the signal number delievered us here 
/*		node_number, an arbitrary node for check paramter passing
/*
/* Purpose:	handle the migrate signal, and check the parameters.
/* 
/* Modification History
/*
/*
/*************************************************************************/

void
migrate_catcher(int sig_no, int node_number)
{
	if ((sig_no == SIGMIGRATE) && (node_number == TEST_NODE)) 
		exit(0);
	else
		exit(1);
}


/*******************************************************************************

Function:	log_results

Returns:	void

Parameters:	positive, a flag the do_test function clears iff the test 
		case is a negative one.

		test_case, the test case number, valid between 1 and ntests.

		return_val, the values do_test got from either the
		local test or the romote test.

Purpose:	This function reports tests results from the sub-tests.


*******************************************************************************/

void 
log_results(positive, test_case, return_val)
bool_t positive;
int test_case, return_val;
{

	/* 
	 * All the test cases for node_self() ??? should be positive.
	 * We just have to check the return_vals.
	 */
	if (positive) {
		if (return_val == 1) 
			printf("PASSED kill3.call TEST %d\n", 
				test_case);
		else 
			fprintf(stderr, "FAILED kill3.call TEST %d\n",
				test_case);
			fflush(stderr);
	}

	/*
	 * We should never reach here.  Regard this as internal errnor.
	 */
	else {
		if ((return_val == -1) &&
				((errno == EPERM) || (errno == ESRCH))) {
			printf("PASSED kill3.call TEST %d\n", 
				test_case);
		}
		else 
			fprintf(stderr, "FAILED kill3.call TEST %d\n",
				test_case);
			fflush(stderr);
	}
	return;
}
