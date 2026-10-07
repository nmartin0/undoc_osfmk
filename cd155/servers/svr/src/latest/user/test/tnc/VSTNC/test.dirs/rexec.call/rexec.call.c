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
 * $Log: rexec.call.c,v $
 * Revision 1.4  1994/11/18  21:08:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  01:47:07  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.1  93/06/09  17:21:21  yazz
 * Regularize VSTNC output.
 * 
 * Revision 3.0  92/07/22  16:52:27  jpaul
 * Initial Checkin
 * 
 */
/*******************************************************************************

Module:		rexec.call.c

Purpose:	This module is part of a test of the TNC function
		rexec().  This piece runs on the local node.  Other
		than getting a list of valid and invalid node numbers,
		then firing off the remote process, it doesn't do much.
		The main logic lies in the switch structure, which uses
		the command argument to decide what test condition to
		execute.  Note that the shell, not the program, contains
		the main test loop.  Why?  Because rexec() won't return
		if it succeeds, so, to avoid wierd dependencies, this 
		program doesn't try to loop.


*******************************************************************************/


#include "rexec.call.h"	/* constants, etc., for this test */

#define DEBUG 1
#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */


/* Limits, boundaries and error codes of the test programs */
int ntests = 11;  	      /* Number of test cases */

char tmp_array[10];

char casedescript[MAXBUF]; /* desctiption of this testcase  */

main(argc, argv, envp)
int argc;
char *argv[], *envp[];

{

int testcase, return_val;    /* The test case number and the rexec return */
bool positive = TRUE;         /* Test case type is usually positive */
void log_results();           /* Write test results to log file */

/* First, find out what test the shell asked us to run, checking
   the validity of the request as well, then encode the test case 
   number in the second argument passed via rexec to the remote process.
 */
if ((argc != 2) || ((testcase = conv_arg(argv[1])) == 0)) {
	fprintf(stderr, "usage:  rexec.call [1 - %d]\n", ntests);
	exit(ENOTEST);
}

init_config_globals();  /* read config file. */

return_val = do_test(testcase, envp, &positive);


/* Handle internal bugs if found. */
if (int_err) {
	fprintf(stderr, "Internal error:  test case (%d) or node_lookup().\n",
		testcase);
	exit(EINTERR);
}
/* In the case of a positive test case, we should never reach this
   point.  For a negative test case, we always should.
 */


/* Write the results to the log file, whatever they were. */
log_results(positive, testcase, return_val);

/* Ah-deeba-deeba-deeba-dat's all folks! */
exit(OK);

}


/*******************************************************************************

Function:	do_test

Returns:	the error code returned by rexec, if any, correct iff the
		test case was negative and the value is REXEC_ERR_CODE.

Parameters:	testcase, the test case number, valid between 1 and ntests.

		envp, a pointer to the first element in a list of strings
		that rexec should pass as environment to the remote program.
		This list is passed from the main function.

		ptr_positive, a pointer to a flag the routine clears if
		the test cases is a negative one.

Global: 	rem_arg, a pointer to the first element in a list of strings
		that rexec should pass as arguments to the remote program.
		This list is set up in the module include file and the main
		function, and do_test encodes the node number in positive
		test cases so that the remote program can decide if the
		system created it in the right place.

Purpose:	This function executes the tests.  The test cases 
		correspond to the cases in section 3 of the FV plan, 
		except that the last five cases run through the other 
		library entries.  


*******************************************************************************/

int do_test(testcase, envp, ptr_positive)
int testcase;
char *envp[];
bool *ptr_positive;

{
int node_lookup();            /* Stores and gets a list of nodes */
int return_val;               /* Holds the return value */
int node = WIERD_NUM;         /* The node to run on */

char testcase_string[10];
sprintf(testcase_string,"%d", testcase);
rem_arg[1] = testcase_string;


	switch (testcase) {

		case 1:	/* Positive test: local node */
		{
			strcat(casedescript, "rexec on local node.\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			sprintf(tmp_array, "%d", node_self());	
			rem_arg[2] = tmp_array;
			return_val = rexecve(REMOTE_PROG, rem_arg, envp, node_self());	
			break;
		}
	
		case 2:	/* Positive test: remote node */
		{
			strcat(casedescript, "rexec on remote node.\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			sprintf(tmp_array, "%d", config_goodnode);	
			rem_arg[2] = tmp_array;
			return_val = rexecve(REMOTE_PROG, rem_arg, envp,
					config_goodnode);
			break;
		}
	
		case 3:	/* Negative test: invalid node */
		{
			strcat(casedescript, "rexec on invalid node.\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			*ptr_positive = FALSE;
			return_val = rexecve(REMOTE_PROG, rem_arg, envp, 
					config_badnode);	
			break;
		}
	
		case 4:	/* Positive test: node zero */
		{
			/* Warning:  If the sys admin does not configure
		   	the TNC cluster with a node numbered zero, this case
		   	will report an error in rexec which really lies in an
		   	invalidated assumption in the test!  
		  	*/
			strcat(casedescript, "rexec on node 0.\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			sprintf(tmp_array, "%d", NODE_ZERO);	
			rem_arg[2] = tmp_array;
			return_val = rexecve(REMOTE_PROG, rem_arg, envp, NODE_ZERO);	
			break;
		}
	
		case 5:	/* Negative test: node negative one */
		{
			strcat(casedescript, "rexec on node -1.\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			*ptr_positive = FALSE;
			return_val = rexecve(REMOTE_PROG, rem_arg, envp, NODE_NEG_ONE);	
			break;
		}
	
		case 6:	/* Negative test: node other negative */
		{
			strcat(casedescript, "rexec on negative node (not -1).\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			*ptr_positive = FALSE;
			return_val = rexecve(REMOTE_PROG, rem_arg, envp, NODE_NEG_OTH);	
			break; 
		}
			
		case 7:	/* Positive test: rexecl library on remote node */
		{
			strcat(casedescript, "rexecl on remote node.\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			sprintf(tmp_array, "%d", config_goodnode);	
			rem_arg[2] = tmp_array;
			return_val = rexecl(REMOTE_PROG, rem_arg[0], rem_arg[1],
					rem_arg[2], rem_arg[3], rem_arg[4], (char *)0,
					config_goodnode);
			break;
		}
	
		case 8:	/* Positive test: rexecle library on remote node */
		{
			strcat(casedescript, "rexecle on remote node.\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			sprintf(tmp_array, "%d", config_goodnode);	
			rem_arg[2] = tmp_array;
			return_val = rexecle(REMOTE_PROG, rem_arg[0], rem_arg[1],
					rem_arg[2], rem_arg[3], rem_arg[4], (char *)0,
					envp, config_goodnode);
			break;
		}
	
		case 9:	/* Positive test: rexeclp library on remote node */
		{
			strcat(casedescript, "rexeclp on remote node.\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			sprintf(tmp_array, "%d", config_goodnode);	
			rem_arg[2] = tmp_array;
			return_val = rexeclp(REMOTE_PROG, rem_arg[0], rem_arg[1],
					rem_arg[2], rem_arg[3], rem_arg[4], (char *)0,
					config_goodnode);
			break;
		}
	
		case 10:	/* Positive test: rexecv library on remote node */
		{
			strcat(casedescript, "rexecv on remote node.\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			sprintf(tmp_array, "%d", config_goodnode);	
			rem_arg[2] = tmp_array;
			return_val = rexecv(REMOTE_PROG, rem_arg, config_goodnode);
			break;
		}
	
	
		case 11:	/* Positive test: rexecvp library on remote node */
		{
			strcat(casedescript, "rexecvp on remote node.\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			sprintf(tmp_array,"%d", config_goodnode);	
			rem_arg[2] = tmp_array;
			return_val = rexecvp(REMOTE_PROG, rem_arg, config_goodnode);
			break;
		}
	
		default:	/* Internal error if passed a bad flag */
		{
			strcat(casedescript, "dedault reached, internal error.\n");
			/* tell user what the testcase is testing.  */
			printf("%s", casedescript);
			fflush(stdout);
			int_err = TRUE;
			break;
		}
	}  /* end switch */
	
	/* If we get here, either we passed--to some extent--a 
	 * negative test case or failed a positive one. 
 	 */
	return(return_val);
}

/*******************************************************************************

Function:	log_results

Returns:	void

Parameters:	positive, a flag the do_test function clears iff the test 
		case is a negative one.

		testcase, the test case number, valid between 1 and ntests.

		return_val, the value do_test got from a (presumably)
		negative test of rexec.  This number is valid iff EINVAL
		and !positive.

Purpose:	This function reports tests results provided the
		test was negative or provided it was positive but
		failed.  In other words, if the test case was negative, 
		then we should get here.  On the other hand, a positive 
		test case that succeeded would never reach this function.
		Test results other than those meeting these conditions
		are reported from the remote program's version of this
		routine.


*******************************************************************************/

void log_results(positive, testcase, return_val)
bool positive;
int testcase, return_val;

{

	if (positive) {

		/*
		 * At this point, we should sleep to allow the process to
		 * start remotely anyway, just on the off chance it will.
		 * If it does, it will log the error and exit.  The sleep
		 * just gives the program time to log the result before
		 * we go forward and advance the file pointer for stderr.
		 */
		(void) sleep(WAIT_REM_PROC);

		fprintf(stderr, "FAILED rexec.call TEST %2d", testcase);
		fprintf(stderr, "rexec returned %d and errno is %d.\n", 
		 return_val, errno);
		fflush(stderr);

	} else if ((errno == EINVAL) && (return_val == REXEC_ERR_CODE)) {
		printf("PASSED rexec.call TEST %2d\n", testcase);
	} else {
		fprintf(stderr, "FAILED rexec.call TEST %2d\n", testcase);
		fprintf(stderr, "rexec returned %d and errno is %d.\n", 
		 return_val, errno);
		fflush(stderr);
	}
	return;
}
