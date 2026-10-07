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
 *		is strictly prohibited.  Locus makes no warrantee,
 *		explicit or implicit, on the functionality of this code.
 *
 * $Log: node_self.call.c,v $
 * Revision 1.5  1994/11/18  21:07:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/02/04  01:45:55  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.4  93/07/12  17:37:01  yazz
 * Regularize output & correct comment blocks.
 * 
 * Revision 3.3  93/06/09  17:20:38  yazz
 * Regularize VSTNC output.
 * Undefined DEBUG.
 * 
 * Revision 3.2  93/03/01  14:22:29  jpaul
 * Do not blindly set TBL_NODEINFO.  It should be picked up from the
 * .h files.  If it is not, there is a make error.
 * 
 * Revision 3.1  92/11/09  13:17:46  yazz
 * String constant fixups for i860 compiler.
 * 
 * Revision 3.0  92/07/22  16:51:48  jpaul
 * Initial Checkin
 * 
 */
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/table.h>
#include "../common/vstnc.h"

#define DEBUG 1
#undef DEBUG

#ifndef FALSE
#define FALSE 0
#endif /* !FALSE */
#ifndef TRUE 
#define TRUE  1
#endif /* !TRUE */

#define MAXBUF 255

/*
 * Number of tests.
 */
int ntests = 2;

char casedescript[MAXBUF]; /* desctiption of this testcase  */

int int_err;
typedef short bool_t;

/*******************************************************************************

Module:		node_self.call

Purpose:	This module is part of a test of the TNC function
		node_self().

*******************************************************************************/

main(argc, argv, envp)
int argc;
char *argv[], *envp[];
{
	int test_case, return_val;    /* The test case number and return */
	bool_t positive = TRUE;         /* Test case type is usually positive */
	void log_results();           /* Write test results to log file */

	int_err = FALSE;

	/* 
	 * First, find out what test the shell asked us to run, 
	 * checking the validity of the request as well. 
	 */
	if ((argc != 2) || ((test_case = conv_arg(argv[1])) == 0)) {
		fprintf(stderr, "usage:  node_self.call [1 - %d]\n", ntests);
		exit(1);
	}

	init_config_globals();

	/* This routine executes the tests. */
	return_val = do_test(test_case);

	/* Handle internal bugs if found. */
	if (int_err) {
		fprintf(stderr, "Internal error:  test case (%d).\n",test_case);
		exit(1);
	}

	/* Write the results to the log file, whatever they were. */
	log_results(positive, test_case, return_val);

	exit(0);
}

/*******************************************************************************

Function:	do_test

Returns:	1 for success, 0 for failure

Parameters:	test_case, the test case number, valid between 1 and ntests.

*******************************************************************************/
int
do_test(test_case)
int test_case;
{
	int return_val;

	switch (test_case) {

		case 1:	/* Test node_self() on the local node */
			strcat(casedescript, 
				"Calling node_self on local node.\n");
			return_val = do_local();
			break;

		case 2:	/* Test node_self() on remote machines */
			strcat(casedescript, 
				"Calling node_self on remote nodes.\n");
			return_val = do_remote();
			break;

		default: /* Internal error if passed a bad flag */
			strcat(casedescript, 
				"Default case, internal error.\n");
			int_err = TRUE;
			break;
	}
	/* tell user what the testcase is testing.  */
	printf("%s", casedescript);
	fflush(stdout);
	return(return_val);
}
/*******************************************************************************

Function:	log_results

Returns:	void

Parameters:	positive, true if the test should succeed, false if the test
		is a negative test (failure expected).

		test_case, the test case number, valid between 1 and ntests.

		return_val, the value do_test returns

*******************************************************************************/
void 
log_results(positive, test_case, return_val)
bool_t positive;
int test_case, return_val;
{

	/* 
	 * All the test cases for node_self() should be positive.
	 * We just have to check the return_vals.
	 */
	if (positive) {
		if (return_val) 
			printf("PASSED node_self.call TEST %2d\n", test_case);
		else 
			fprintf(stderr, "FAILED node_self.call TEST %2d\n",
			 test_case);
	}

	/*
	 * We should never reach here.  Regard this as internal errnor.
	 */
	else 	
		fprintf(stderr, "FAILED node_self.call internal error, "
			"non postive test case %2d.\n", test_case);
	fflush(stderr);
	return;
}

int
do_remote()
{
	int this_node, remote_node; 
	int status, pid, index;
	int dead_child;
	int *node_number_table;		/* holds all valid nodes     */
	int number_of_nodes;		/* number of all valid nodes */

	/*
	 * Use table() system call to obtain an array of valid node
	 * numbers.
	 */
	number_of_nodes = table(TBL_NODEINFO, 0, (char *) 0, 32767, 0);
	if (number_of_nodes == -1){
		fprintf(stderr, "table(): failed to obtain number of nodes\n");
		fflush(stdout);
		exit(0);
	}

	/*
	 * Allocate the space needed for the table.
	 */
	node_number_table = (int *) calloc(number_of_nodes, sizeof(int));
	if (node_number_table == NULL){
		fprintf(stderr, "calloc(): failed to allocate node table.\n");
		fflush(stdout);
		exit(0);
	}

	/*
	 * Fill the table with valid node numbers.
	 */
	status = table(TBL_NODEINFO, 0, (char *) node_number_table, 
		       number_of_nodes, sizeof(int));
	if (status == -1){
		fprintf(stderr, "table(): failed to read in node array.\n");
		fflush(stdout);
		exit(0);
	}

	/*
	 * Now try to verify the node_self() call on all
	 * the remote nodes.
	 */
	for (index=0; index<number_of_nodes; index++){
		
		/*
		 * Find a valid node number first, then rfork to it.
		 */
		remote_node = node_number_table[index];

#ifdef DEBUG
		printf("rforking to node %d\n", remote_node);
#endif

		pid = rfork(remote_node);
		if (pid < 0){
			fprintf(stderr,"do_remote(): rfork failed\n");
			fflush(stderr);
			exit(0);
		}

		/*
		 * If this is the child, run the node_self() test and
		 * log the results.
		 */
		if (pid == 0){
			this_node = node_self();
			if (this_node == remote_node) 
				exit(0);
			else
				exit(-1);
		}

		/*
		 * Parent waits for the child to exit and collects
		 * the status.
		 */
		dead_child = wait(&status);

		/*
		 * If one of the remote node_self() call fails, the
		 * second test case fails also.
		 */
		if (status == -1)
			return(0);
	}

	/* 
	 * If we reach here, all the remote node_self() calls 
	 * have succeded.
	 */
	return(1);
}

int 
do_local()
{
	int this_node, real_node_number; 
	FILE *fd;

	/*
	 * Call node_self() and check errno.
	 */
	errno = 0;
	this_node = node_self();
	if (errno != 0) {
		fprintf(stderr, "node_self.call: node_self() failure, "
			"errno=%d\n", errno);
		fflush(stderr);
	}

	if (this_node == config_mynode)
		return(1);
	else
		return(0);
}
