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
 * $Log: rexec.call.remote.c,v $
 * Revision 1.4  1994/11/18  21:08:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  01:47:12  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.1  93/06/09  17:21:35  yazz
 * Regularize VSTNC output.
 * 
 * Revision 3.0  92/07/22  16:52:39  jpaul
 * Initial Checkin
 * 
 */
/*******************************************************************************

Module:		rexec.call.remote.c

Purpose:	This module is part of a test of the TNC function
		rexec().  This piece runs on the remote node.  First,
		it checks to see if a negative test case somehow caused
		it to exist, in which case it reports an error.  Then,
		it verifies that the system put it on the correct node.  


*******************************************************************************/


#include "rexec.call.h"	/* constants, etc., for this test */

/* Limits, boundaries and error codes of the test programs */
int ntests = 11;		/* Number of test cases */


main(argc, argv, envp)
int argc;
char *argv[], *envp[];

{

int test_case, req_node;      /* Test case and node requested from local */
int node;                     /* Node the program is running on */
bool positive = TRUE;         /* Test case type is usually positive */
void log_results();           /* Write test results to log file */

/* Decode the test case number from argument one passed via rexec. */
sscanf(argv[1],"%d", &test_case);

node = chk_test(test_case, argv, &req_node, &positive);

/* Handle internal bugs if found. */
if (int_err) {
	printf("Internal error:  test case (%d) or node_lookup().\n",
		test_case);
	fprintf(stderr, "Internal error:  test case (%d) or node_lookup().\n",
		test_case);
	fflush(stderr);
	exit(EINTERR);
}

/* Write the results to the log file, whatever they were. */
log_results(positive, test_case, node, req_node);

exit(OK);

}


/*******************************************************************************

Function:	chk_test

Returns:	the node the program is running on

Parameters:	test_case, the test case number, valid between 1 and ntests.

		argv, a pointer to the first element in a list of strings
		that the local program passed as arguments to the remote 
		program, the second element of which is the node requested
		in positive test cases.

		ptr_req_node, a pointer to a integer storing the node
		the local program requested this program to run on.

		ptr_positive, a pointer to a flag the routine sets if
		the test cases is a positive one.

Global:		int_err, a flag that indicates an internal error.

Purpose:	This function checks the test case and records some
		information about the expected condition.  


*******************************************************************************/

int chk_test(test_case, argv, ptr_req_node, ptr_positive)
int test_case;
char *argv[];
int *ptr_req_node;
bool *ptr_positive;

{
int node;      /* Stores and gets a node */

/* Find out what node we're running on */
node = node_self();

/* Decode the requested node if positive, or set flag if negative */
switch (test_case) {

	case 1:	/* Positive test: local node */
	case 2:	/* Positive test: remote node */
	case 4:	/* Positive test: node zero */
	case 7:	/* Positive test: rexecl library on remote node */
	case 8:	/* Positive test: rexecle library on remote node */
	case 9:	/* Positive test: rexeclp library on remote node */
	case 10:	/* Positive test: rexecv library on remote node */
	case 11:	/* Positive test: rexecvp library on remote node */
		sscanf(argv[2],"%d", ptr_req_node);
		break;

	case 3:	/* Negative test: invalid node */
	case 5:	/* Negative test: node negative one */
	case 6:	/* Negative test: node other negative */
		*ptr_positive = FALSE;
		break;

	default:	/* Internal error if passed a bad argument */
		int_err = TRUE;
		break;
}

return(node);
}

/*******************************************************************************

Function:	log_results

Returns:	void

Parameters:	positive, a flag the chk_test function clears iff the test 
		case is a negative one.

		test_case, the test case number, valid between 1 and ntests.

		node, the node executing this program

		req_node, the node the local program asked to execute 
		this program

Purpose:	This function reports tests results.  If the test case 
		was positive, then we should get here.  On the other 
		hand, a negative test case had no business invoking a
		program, either remotely or locally.

*******************************************************************************/

void log_results(positive, test_case, node, req_node)
bool positive;
int test_case, node, req_node;

{

	if ((positive) && (node == req_node)) {
		printf("PASSED rexec.call.remote TEST %2d\n", test_case);
	} else if ((positive) && (node != req_node)) {
		fprintf(stderr, "FAILED rexec.call.remote TEST %2d\n",
		 test_case);
		fprintf(stderr, "started program on node %d, expected %d.\n",
		 node, req_node);
		fflush(stderr);
	} else {
		fprintf(stderr, "FAILED rexec.call.remote TEST %2d\n",
		 test_case);
		fprintf(stderr, "incorrectly started program on node %d.\n",
		 node);
		fflush(stderr);
	}

	return;
}
