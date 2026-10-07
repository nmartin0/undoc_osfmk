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
 * $Log: sigmig.feature.c,v $
 * Revision 1.4  1994/11/18  21:08:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  01:49:40  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.3  93/06/09  17:25:25  yazz
 * Regularize VSTNC output.
 * 
 * Revision 3.2  93/03/17  14:50:45  jpaul
 * Get rid of pesky messages.
 * 
 * Revision 3.1  93/03/01  14:25:08  jpaul
 * Comment out DEBUG= line, setting it to zero is not enough.
 * 
 * Revision 3.0  92/07/22  16:54:04  jpaul
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
#endif

#ifndef TRUE
#define TRUE  1
#endif

#define DEBUG 1
#undef DEBUG

#define MAXBUF 255

/*
 * Number of tests.
 */
int ntests = 24;

char casedescript[MAXBUF] = { "" };	/* description of this testcase  */

char *myname = "";

/*
 * Amount of time parent and child should sleep before either
 * delivering the signal or timing out waiting for a signal.
 */
#define PARENT_SLEEP_TIME 2
#define CHILD_SLEEP_TIME 10

/*
 * For testing sigmigrate, we expect only two positive conditions:
 *
 *	case 1:	process catches the signal and verifies that the
 *		argument passed to it is correct.
 *	case 2:	process either chooses to ignore the signal, or migrates
 *		to the specified node.
 */
#define COND_1 0
#define COND_2 2

typedef short bool_t;

int int_err;
bool_t CAUGHT_SIG;

#define SIGMIGRATE 32

/*
 * Node the process resides on awaiting the migrate signal.
 */
int process_node;

/*
 * Correct node to be on after the kill3() call.
 */
int expected_node;

/*
 * Node kill3() tells the process to migrate to.
 */
int dest_node;

/****************************************************************/
/*								*/
/* Module:	sigmig.feature.c
/*								*/
/* Purpose:	This module is part of a test of the TNC 	*/
/*		function kill3().  Following TNC testing	*/
/*		templates, the main program just calls the	*/
/*		test subroutines with the correct test cases	*/
/*		and logs the results.				*/
/*								*/
/*								*/
/****************************************************************/

main(argc, argv, envp)
int argc;
char *argv[], *envp[];
{
	int test_case, return_val;    /* The test case number and return */
	bool_t positive = 0;         /* Test case type is usually positive */
	void log_results();           /* Write test results to log file */

	myname = argv[0];

	int_err = FALSE;

	/* 
	 * First, find out what test the shell asked us to run, 
	 * checking the validity of the request as well. 
	 */
	if ((argc != 2) || ((test_case = conv_arg(argv[1])) == 0)) {
		fprintf(stderr, "usage: sigmig.feature [1 - %d]\n", ntests);
		fflush(stderr);
		exit(1);
	}

	init_config_globals();

	/*
	 * Decide where the target process should reside.
	 * For case 1-12, the child process resides on the host node, 
	 * and for case 13-24, the child resides on a remote node.
	 */
	if (test_case <= 12)  
		process_node = node_self();
	else
		process_node = config_goodnode;
	/* The rubber hits the road here.  This routine executes the tests. */
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
/* Returns:	the results of calling test_migrate().		*/
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
	extern int test_migrate(void *);
	extern void sig_catcher(int, int);

	int 	return_val;

	switch (test_case) {
		/*
		 * Test condition 1 : child migrating to the same node.
		 */
		case 1:
		case 13:
		{
			strcat(casedescript, "Child migrating to same node.\n");
			*ptr_positive = COND_1;
			dest_node = node_self();
			return_val = test_migrate(sig_catcher);
			break;
		}

		/*
		 * Test condition 2: catch signal telling child to migrate to 
		 * a bad node.
		 */
		case 2:	
		case 14:
		{
			strcat(casedescript, "Catch signal telling child to migrate to a bad node.\n");
			*ptr_positive = COND_1;
			dest_node = node_self();
			return_val = test_migrate(sig_catcher);
			break;
		}

		/*
		 * Test condition 3: catch signal to a negative node.
		 */
		case 3:	
		case 15:
		{
			strcat(casedescript, 
				"Catch signal to to a negative node.\n");
			*ptr_positive = COND_1;
			dest_node = -1;
			return_val = test_migrate(sig_catcher);
			break;
		}

		/*
		 * Test condition 4: catch migrate signal to a good node 
		 * other than node_self.
		 */
		case 4:	
		case 16:
		{
			strcat(casedescript, "Catch signal telling child to migrate to a valid node.\n");
			*ptr_positive = COND_1;
			dest_node = config_goodnode;
			return_val = test_migrate(sig_catcher);
			break;
		}

		/*
		 * Test condition 5: ignoring signal to migrate to current node,
		 * no migrate, no signal caught.
		 */
		case 5:	
		case 17:
		{
			strcat(casedescript, "ignoring signal to migrate to a current node.\n");
			*ptr_positive = COND_2;
			expected_node = process_node;
			dest_node = node_self();
			return_val = test_migrate(SIG_IGN);
			break;
		}

		/*
		 * Test condition 6: ignore signal to badnode, no signal
		 * caught, no migrate.
		 */
		case 6:	
		case 18:
		{
			strcat(casedescript, 
				"ignoring signal to migrate to a bad node.\n");
			*ptr_positive = COND_2;
			expected_node = process_node;
			dest_node = config_badnode;
			return_val = test_migrate(SIG_IGN);
			break;
		}

		/*
		 * Test condition 7: ignore signal to -1, no signal caught, 
		 * no migrate.
		 */
		case 7:	
		case 19:
		{
			strcat(casedescript, "ignoring signal to node -1.\n");
			*ptr_positive = COND_2;
			expected_node = process_node;
			dest_node = -1;
			return_val = test_migrate(SIG_IGN);
			break;
		}

		/*
		 * Test condition 8: ignore signal, no signal caught, 
		 * no migrate.
		 */
		case 8:	
		case 20:
		{
			strcat(casedescript, "ignoring signal.\n");
			*ptr_positive = COND_2;
			expected_node = process_node;
			dest_node = config_goodnode;
			return_val = test_migrate(SIG_IGN);
			break;
		}

		/*
		 * Test condition 9: use default value, migrate to specified
		 * node.
		 */
		case 9:	
		case 21:
		{
			strcat(casedescript, "migrate to a specified (node_self) node.\n");
			*ptr_positive = COND_2;
			expected_node = node_self();
			dest_node = node_self();
			return_val = test_migrate(SIG_DFL);
			break;
		}

		/*
		 * Test condition 10: since specified node is bad, no 
		 * signal caught, no migrate.
		 */
		case 10:	
		case 22:
		{
			strcat(casedescript, "Migrate to a bad node, should not catch signal or migrate.\n");
			*ptr_positive = COND_2;
			expected_node = process_node;
			dest_node = config_badnode;
			return_val = test_migrate(SIG_DFL);
			break;
		}

		/*
		 * Test condition 11: since specified node is bad, no 
		 * signal caught, no migrate.
		 */
		case 11:	
		case 23:
		{
			strcat(casedescript, "Migrate to a bad node (-1), should not catch signal or migrate.\n");
			*ptr_positive = COND_2;
			expected_node = process_node;
			dest_node = -1;
			return_val = test_migrate(SIG_DFL);
			break;
		}

		case 12:	
		case 24:
		{
		/*
		 * Test condition 12: use default value, migrate to 
		 * specified node.
		 */
			strcat(casedescript, "Migrate to good node, should catch signal and migrate.\n");
			*ptr_positive = COND_2;
			expected_node = config_goodnode;
			dest_node = config_goodnode;
			return_val = test_migrate(SIG_DFL);
			break;
		}

		default: /* Internal error if passed a bad flag */
		{
			strcat(casedescript, "Default reached, internal error.\n");
			int_err = TRUE;
			break;
		}
	}

	/* tell user what the testcase is testing.  */
	printf("%s", casedescript);
	fflush(stdout);

	return(return_val);
}

/************************************************************************
/*
/* Function:	test_migrate()
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
/*
/*************************************************************************/

int
test_migrate(void (*local_catcher)())
{
	int pid;
	int status, child_status;
	int temp_node;

	/*
	 * Setup a handler before forking off new processes.
	 */
	signal(SIGMIGRATE, local_catcher);
#ifdef DEBUG 
	printf("rforking to %d\n", process_node);
#endif /* DEBUG  */

	/* 
	 * Create a new child process to send signal to.
	 */
	pid = rfork(process_node);

	/*
	 * If this is the child, wait for the parent to send the kill3 
	 * signal.  If sleep time expires, check and see if we have
	 * migrated or not.
	 */
	if (pid == 0) {
#ifdef DEBUG 
	printf("child going to sleep.\n");
#endif /* DEBUG  */
		sleep(CHILD_SLEEP_TIME);
		/*
		 * Get the current running node, and compare it with
		 * the expected results.
		 */
		if (expected_node == node_self()){
#ifdef DEBUG 
	printf("child exiting with status 2.\n");
#endif /* DEBUG  */
			exit(2);
		}
		else
#ifdef DEBUG 
	printf("child exiting with status 3.\n");
#endif /* DEBUG  */
			exit(3);
	}

	/* 
	 * The parent process issues the kill3 call and waits for the
	 * child to exit.
	 */
	if (pid > 0) {
		sleep(PARENT_SLEEP_TIME);
		/*
		 * kill3() is allowed to fail here.
		 */
#ifdef DEBUG 
	printf("parent sending signal to child %d to migrate to node %d.\n", pid, dest_node);
#endif /* DEBUG  */
		status = kill3(pid, SIGMIGRATE, dest_node);
		if (status == -1){
			printf("kill3() failed.\n");
		}
		wait(&status);
#ifdef DEBUG 
	printf("raw status 0x%x\n", status);
#endif /* DEBUG  */
		child_status = (status & 0xff00) >> 8;	
#ifdef DEBUG 
	printf("child exited with status 0x%x.\n", child_status);
#endif /* DEBUG  */
		return(child_status);
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
/*
/*************************************************************************/

void
sig_catcher(int sig_no, int node_number)
{

#ifdef DEBUG
	printf("child caught signal.\n");
#endif

	if ((sig_no == SIGMIGRATE) && (node_number == dest_node)) 
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
	 * Check the results with their expected value.
	 */
	if (positive == return_val) {
		printf("PASSED %s TEST %2d\n", myname, test_case);
	} else {
		fprintf(stderr, "FAILED %s TEST %2d\n", myname, test_case);
		fflush(stderr);
	}
}

/************************************************************************
/*
/* Function:	find_valid_node()
/*
/* Returns:	a valid node number in the cluster
/*
/* Parameter:	none
/*
/* Purpose:	Use table() system call to get a list of valid node
/*		numbers, and return one other than the host node.
/* 
/*
/*************************************************************************/
