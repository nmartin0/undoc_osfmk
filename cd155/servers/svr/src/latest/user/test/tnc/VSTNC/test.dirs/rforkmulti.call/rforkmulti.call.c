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
 * $Log: rforkmulti.call.c,v $
 * Revision 1.6  1994/12/01  02:22:18  yazz
 *  Reviewer: slk
 *  Risk: Lo
 *  Benefit or PTS #: 11632
 *  Testing: VSTNC test suite
 *  Module(s): user/test/tnc/VSTNC/test.dirs/rforkmulti.call/rforkmulti.call.c
 * If NX is defined, expect NX semantics for the rforkmulti() call.
 *
 * Revision 1.5  1994/11/18  21:08:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/02/04  01:48:43  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.5  93/07/12  17:37:35  yazz
 * Add missing linefeed to debug output.
 * 
 * Revision 3.4  93/06/09  17:23:50  yazz
 * Regularize VSTNC output.
 * 
 * Revision 3.3  93/03/01  14:24:38  jpaul
 * Turn off debug messages, need to comment out DEBUG=
 * rather than setting it to 0, most #ifdefs just check to
 * see if it's set, not the value, so even a 0 value will be
 * picked up.
 * 
 * Revision 3.2  92/12/02  16:40:24  jpaul
 * Turn debugging off and isolate printf for debugging only.
 * 
 * Revision 3.1  92/11/09  13:20:00  yazz
 * String constant fixup for i860 compiler.
 * 
 * Revision 3.0  92/07/22  16:53:16  jpaul
 * Initial Checkin
 * 
 */
#include <errno.h>
#include <stdio.h>
#include "../common/vstnc.h"

/*
 * Number of tests.
 */
int ntests = 8;

#define	CHILDMAX 17
#undef DEBUG

#ifndef MAXBUF
# define MAXBUF 255
#endif /* !MAXBUF */

int do_test_end();

int int_err = FALSE;

char casedescript[MAXBUF]; /* desctiption of this testcase  */

char *myname;

/****************************************************************
 *
 * Module:	rforkmulti.call
 *
 * Purpose:	This module tests the TNC function
 *		rforkmulti().  Following TNC testing templates,
 *		the main program just calls the test subroutines
 *		subroutines with the correct test cases and logs
 *		the results.
 *
 ****************************************************************/

main(argc, argv, envp)
int argc;
char *argv[], *envp[];
{
	int testcase;		/* The test case number from argv[1] */

	myname = argv[0];

	/*
	 * First, find out what test the shell asked us to run,
	 * checking the validity of the request as well.
	 */
	if( argc != 2 || (testcase = conv_arg(argv[1])) == 0 ) {
		fprintf(stderr, "usage: %s [ 1 - %d ]\n", myname, ntests);
		exit(1);
	}

	init_config_globals();

	/*
	 * Execute the test specified, and log its results.
	 * Only ONE test case per run.
	 */
	(void)do_test(testcase);

	/* Handle internal bugs if found. */
	if (int_err) {
		fprintf(stderr, "Internal error:  test case (%d).\n",testcase);
		exit(1);
	}

	exit(0);
}

/****************************************************************/
/*								*/
/* Function:	do_test()					*/
/*								*/
/* Returns:	None						*/
/*								*/
/* Parameters:	testcase, the test case number, valid between	*/
/*		1 and ntests.					*/
/****************************************************************/

int
do_test(testcase)
int testcase;
{
	int		i, anyfail, print_arrays_flag = 0;
	int		ret, expected_ret;
	int		actual_ret, actual_errno;
	int		orig_count, expected_count, actual_count, tmpcount;
	int		expected_errno;
	int		node_array[CHILDMAX];
	int		errno_array[CHILDMAX], *errno_arrayp;
	int		pid_array[CHILDMAX];
	int		expected_errno_array[CHILDMAX];
	int		child_sum, parent_sum;
	int		waitstat;
	char		*p;

	/*
	 * Set up initial conditions.  Zero out the arrays and let
	 * the individual test cases change what needs changing.
	 * Calculate the sum of the bytes in this, the do_test() routine.
	 */

	for( i=0; i<CHILDMAX; ++i ) {
		node_array[i] = 0;
		errno_array[i] = 0;
		pid_array[i] = 0;
		expected_errno_array[i] = 0;
	}
	orig_count = 0;
	expected_count = 0;
	expected_ret = 0;
	expected_errno = 0;
	errno_arrayp = &errno_array[0];

	for( parent_sum=0,p=(char *)do_test; p<(char *)do_test_end; ++p ) {
		parent_sum += *p;
	}

#ifdef DEBUG
	printf("parent_sum is %d\n", parent_sum);
#endif

	switch( testcase ) {

	case 1:
			/* rforkmulti (1 fork) on own node */
		strcat(casedescript, "rforkmulti (1 fork) to own node.\n");
		orig_count = 1;
		expected_ret = 1;
		expected_count = 1;
		node_array[0] = config_mynode;
		break;

	case 2:
		/* rforkmulti (1 fork) to valid node */
		strcat(casedescript, "rforkmulti (1 fork) to remote node.\n");
		orig_count = 1;
		expected_ret = 1;
		expected_count = 1;
		node_array[0] = config_goodnode;
		break;

	case 3:
		/* rforkmulti to bad node */
#ifdef NX
		strcat(casedescript, "NX: rforkmulti (1 fork) to bad node, "
				"EINVAL for entire call expected.\n");
		expected_ret = -1;
		expected_errno = EINVAL;
#else
		strcat(casedescript, "rforkmulti (1 fork) to bad node, EINVAL "
				"for just one node expected.\n");
		expected_ret = -2;
		expected_errno_array[0] = EINVAL;
#endif
		expected_count = 0;
		orig_count = 1;
		node_array[0] = config_badnode;
		break;

	case 4:
		/* rforkmulti to 3 valid nodes */
		strcat(casedescript, "rforkmulti (3 forks) to valid nodes.\n");
		orig_count = 3;
		expected_ret = 1;
		expected_count = 3;	/* all 3 requests should work OK */
		node_array[0] = config_mynode;
		node_array[1] = config_goodnode;
		node_array[2] = config_goodnode;
		break;

	case 5:
		/* rforkmulti to 2 good nodes plus 1 bad node */
#ifdef NX
		strcat(casedescript, "NX: rforkmulti to 2 good nodes and 1 "
				"bad node, EINVAL for entire call expected.\n");
		expected_ret = -1;
		expected_count = 0;
		expected_errno = EINVAL;
#else
		strcat(casedescript, "rforkmulti to 3 different nodes, 2 "
				"should pass, one fail.\n");
		expected_ret = -2;
		expected_count = 2;	/* 2 of the 3 requests should work OK */
		expected_errno_array[1] = EINVAL;
#endif
		orig_count = 3;
		node_array[0] = config_mynode;
		node_array[1] = config_badnode;
		node_array[2] = config_goodnode;
		break;

	case 6:
		/* bad address on node array, should get EFAULT */
		strcat(casedescript, "rforkmulti with bad node array address, "
				"EFAULT for entire call expected.\n");
		orig_count = 1;
		expected_count = 0;
		errno_arrayp = config_badaddr;
		expected_ret = -1;
		expected_errno = EFAULT;
		break;

	case 7:
		/* 3 bad nodes, all should return EINVAL */
#ifdef NX
		strcat(casedescript, "NX: rforkmulti to 3 bad nodes, EINVAL "
				"for entire call expected.\n");
		expected_ret = -1;
		expected_errno = EINVAL;
#else
		strcat(casedescript, "rforkmulti to 3 bad nodes, all should "
				"fail with EINVAL.\n");
		expected_ret = -2;
#endif
		expected_count = 0;	/* none of the 3 requests should work */
		orig_count = 3;
		node_array[0] = config_badnode;
		node_array[1] = config_hinode + 1;
		node_array[2] = config_lonode - 1;
		expected_errno_array[0] = EINVAL;
		expected_errno_array[1] = EINVAL;
		expected_errno_array[2] = EINVAL;
		break;

	case 8:
		/* alternate between good and bad nodes. */
#ifdef NX
		strcat(casedescript, "NX: rforkmulti alternate good and bad "
				"nodes, expect EINVAL on entire call.\n");
		expected_ret = -1;
		expected_errno = EINVAL;
#else
		strcat(casedescript, "rforkmulti alternate good and bad "
				nodes.\n");
		expected_ret = -2;
#endif
		orig_count = CHILDMAX;
		for( i=0; i<CHILDMAX; ++i ) {
			switch( i % 2 ) {
				case 0:
				{
					node_array[i] = config_goodnode;
					++expected_count;
					break;
				}
				case 1:
				{
					node_array[i] = config_badnode;
					expected_errno_array[i] = EINVAL;
					break;
				}
			}
		}
		break;

	default:
		strcat(casedescript, 
			"default case reached, internal error.\n");
		++int_err;
		fprintf(stderr, "Invalid test case %d must be 1 - %d\n",
	 	testcase, ntests);
		return( -1 );
	}

	/* tell user what the testcase is testing.  */
	printf("%s", casedescript);
	fflush(stdout);

	/*
	 * Using what the test case set up, perform the test.
	 */
	tmpcount = orig_count;	/* this is both an input and output param */
#ifdef DEBUG
	printf("before rforkmulti.\n");
#endif  /* DEBUG */
	ret = rforkmulti(&tmpcount, node_array, errno_arrayp, pid_array);
	actual_ret = ret;
	actual_errno = errno;
	actual_count = tmpcount;
	if( ret == 0 ) {	/* we are a child */
		for( child_sum=0,p=(char *)do_test; p<(char *)do_test_end;
		 ++p ) {
			child_sum += *p;
		}
#ifdef DEBUG 
		printf("\nIn child after rforkmulti, child_sum %d parent_sum "
				"%d.\n\n", child_sum, parent_sum);
#endif /* DEBUG  */
		if( child_sum != parent_sum )
			exit(1);
		exit(0);		/* good sum means 0 exit */
	}
#ifdef DEBUG 
	printf("made it to parent code\n");
#endif  /* DEBUG */
	/* we are the parent */

	if( actual_ret == expected_ret ) {
		printf("PASSED %s retcode TEST %2d\n", myname, testcase);
	} else {
		fprintf(stderr,
		 "FAILED %s retcode TEST %2d: expected %d got %d\n\n",
		 myname, testcase, expected_ret, actual_ret);
		fflush(stderr);
	}

	if( actual_errno == expected_errno ) {
		printf("PASSED %s global errno TEST %2d\n", myname, testcase);
	} else {
		fprintf(stderr,
		 "FAILED %s global errno TEST %2d: expected %d got %d\n",
		 myname, testcase, expected_errno, actual_errno);
		fflush(stderr);
	}

	if (expected_ret == -1) {	/* these tests automatically pass */
		printf("PASSED %s count TEST %2d\n", myname, testcase);
		printf("PASSED %s array TEST %2d\n", myname, testcase);
		return(0);
	}

	if( actual_count == expected_count ) {
		printf("PASSED %s count TEST %2d\n", myname, testcase);
	} else {
		fprintf(stderr,
		 "FAILED %s count TEST %2d: expected %d got %d\n",
		 myname, testcase, expected_count, actual_count);
		fflush(stderr);
	}

	for( anyfail=0,i=0; i<orig_count; ++i ) {
		if( (expected_errno_array[i] != errno_array[i])  ||
		 (pid_array[i] != -1 && pid_array[i] <= 0)  ||
		 (pid_array[i] > 0 && errno_array[i] != 0)  ||
		 (pid_array[i] == -1 && errno_array[i] <= 0) ) {
			++anyfail;
			++print_arrays_flag;
			fprintf(stderr, "FAILED %s array TEST %2d, slot %d\n",
			 myname, testcase, i);
			fflush(stderr);
		}
	}
	if( anyfail == 0 ) {
		printf("PASSED %s array TEST %2d\n", myname, testcase);
	}

	for( anyfail=0,ret=0; (ret=wait(&waitstat)) !=-1;) {
		if( waitstat != 0 ) {
			++anyfail;
			++print_arrays_flag;
			fprintf(stderr, "FAILED %s checksum TEST %2d pid=%d\n",
			 myname, testcase, ret);
			fflush(stderr);
		}
	}

	if( print_arrays_flag ) {
		fprintf(stderr, "\nFull info printed due to error:\n");
		fprintf(stderr, "count_in=%d count_out=%d ret=%d errno=%d\n\n",
		 orig_count, actual_count, actual_ret, actual_errno);
		fprintf(stderr, "slot node       pid errno expected_errno\n");
		for( i=0; i<orig_count; ++i ) {
			fprintf(stderr, "%4d %4d %9d %5d %5d\n",
			 i, node_array[i], pid_array[i], errno_array[i],
			 expected_errno_array[i]);
		}
		fflush(stderr);
	}

	return(0);		/* no internal error */
}

do_test_end()
{
	return(0);		/* dummy routine marks end of do_test() */
}
