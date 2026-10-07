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
 * $Log: set_tnc_port.call.c,v $
 * Revision 1.4  1994/11/18  21:08:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  01:49:03  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.2  93/06/09  17:24:20  yazz
 * Regularize VSTNC output.
 * 
 * Revision 3.1  93/06/03  18:56:10  yazz
 * [Bug #0216] Make case 4, a "send right" test case, use a plain send
 * 	right (mach_task_self()) instead of a combined send/receive
 * 	right.
 * 
 * Revision 3.0  92/07/22  16:53:47  jpaul
 * Initial Checkin
 * 
 */
#include <errno.h>
#include <stdio.h>
#include <mach.h>
#include <mach/message.h>
#include "../common/vstnc.h"

#define DEBUG 1
#undef DEBUG

#define MAXBUF 255
#define PORT_ID	1717

/*
 * Number of tests.
 */
int ntests = 11;

char casedescript[MAXBUF] = { "" };	/* description of this testcase */

int int_err = FALSE;

char *myname;

/****************************************************************
 *
 * Module:	set_tnc_port.call
 *
 * Purpose:	This module tests the TNC function
 *		set_tnc_port().  Following TNC testing templates,
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
		fflush(stderr);
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
	int		ret, expected_errno, actual_errno;
	mach_port_t	port_to_use, tmpport;
	mach_port_t	my_receive1, my_receive2, my_sendonce,
			 my_deadname, my_portset;

	/*
	 * Set up initial conditions.  Clear out any pre-existing
	 * port that's registered with our id.  Allocate rights that
	 * the tests can use.
	 */
	(void)set_tnc_port(PORT_ID, MACH_PORT_NULL);	/* clear any prev */

	ret = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
	 &my_receive1);
	if( ret != 0 ) {
		++int_err;
		fprintf(stderr, "can't alloc rcv rt #1 ret=%d\n", ret);
		fflush(stderr);
		return( -1 );
	}
	ret = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
	 &my_receive2);
	if( ret != 0 ) {
		++int_err;
		fprintf(stderr, "can't alloc rcv rt #2 ret=%d\n", ret);
		fflush(stderr);
		return( -1 );
	}

	ret = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
	 &tmpport);			/* just get this port's name */
	if( ret != 0 ) {
		++int_err;
		fprintf(stderr, "can't alloc rcv rt tmpport ret=%d\n", ret);
		fflush(stderr);
		return( -1 );
	}
	ret = mach_port_destroy(mach_task_self(), tmpport);
	if( ret != 0 ) {
		++int_err;
		fprintf(stderr, "can't destroy rcv rt tmpport ret=%d\n", ret);
		fflush(stderr);
		return( -1 );
	}
	my_sendonce = tmpport;	/* name now free so use for send once rt */
	ret = mach_port_insert_right(mach_task_self(), my_sendonce,
	 my_receive1, MACH_MSG_TYPE_MAKE_SEND_ONCE);
	if( ret != 0 ) {
		++int_err;
		fprintf(stderr, "can't make send once right ret=%d\n", ret);
		fflush(stderr);
		return( -1 );
	}

	ret = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_DEAD_NAME,
	 &my_deadname);
	if( ret != 0 ) {
		++int_err;
		fprintf(stderr, "can't alloc deadname rt ret=%d\n", ret);
		fflush(stderr);
		return( -1 );
	}

	ret = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_PORT_SET,
	 &my_portset);
	if( ret != 0 ) {
		++int_err;
		fprintf(stderr, "can't alloc portset rt ret=%d\n", ret);
		fflush(stderr);
		return( -1 );
	}

	switch( testcase ) {

		case 1:		
		{	/* using own receive port, no error expected */
			strcat(casedescript, "Using own recieve port.\n");
			port_to_use = my_receive1;
			expected_errno = 0;
			break;
		}
	
		case 2:
		{	/* set and destroy port, no error expected. */
			strcat(casedescript, 
				"Testing set and destroy recieve port.\n");
			ret = set_tnc_port(PORT_ID, my_receive2);
			if( ret != 0 ) {
				++int_err;
				fprintf(stderr,
			 	"can't set_tnc_port my_receive2 ret=%d\n", ret);
				fflush(stderr);
				return( -1 );
			}
			ret = mach_port_destroy(mach_task_self(), my_receive2);
			if( ret != 0 ) {
				++int_err;
				fprintf(stderr,
			 	"can't destroy my_receive2 ret=%d\n", ret);
				fflush(stderr);
				return( -1 );
			}	/* now, server's held send rt is a dead name */
			port_to_use = my_receive1;
			expected_errno = 0;	/* should work fine */
			break;
		}
	
		case 3:
		{	/* set rec port, slot taken, EEXIST error expected */
			strcat(casedescript, "Setting receive port, slot is already taken, expecting EEXIST.\n");
			ret = set_tnc_port(PORT_ID, my_receive2);
			if( ret != 0 ) {
				++int_err;
				fprintf(stderr,
			 	"can't set_tnc_port my_receive2 ret=%d\n", ret);
				fflush(stderr);
				return( -1 );
			}
			port_to_use = my_receive1;
			expected_errno = EEXIST;	/* id slot already taken */
			break;
		}
	
		case 4:
		{	/* using own send port, EINVAL expected.  */
			strcat(casedescript, 
				"Using own send port, EINVAL expected.\n");
			port_to_use = mach_task_self();
			expected_errno = EINVAL;	/* a send rt isn't OK */
			break;
	}
	
		case 5:
		{	/* own send once port, EINVAL expected */
			strcat(casedescript, 
				"Using own send once port, EINVAL expected.\n");
			port_to_use = my_sendonce;
			expected_errno = EINVAL; /* a sendonce rt isn't OK */
			break;
		}
	
		case 6:
		{	/* dead port, EINVAL expected */
			strcat(casedescript, 
				"Using dead port, EINVAL expected.\n");
			port_to_use = my_deadname;
			expected_errno = EINVAL; /* a deadname rt isn't OK */
			break;
		}
	
		case 7:
		{	/* dead port, EINVAL expected */
			strcat(casedescript, 
				"Using dead port, EINVAL expected.\n");
			port_to_use = MACH_PORT_DEAD;
			expected_errno = EINVAL; /* this deadname rt isn't OK */
			break;
		}
	
		case 8:
		{	/* own portset, EINVAL expected. */
			strcat(casedescript, 
				"Using own portset, EINVAL expected.\n");
			port_to_use = my_portset;
			expected_errno = EINVAL; /* a portset rt isn't OK */
			break;
		}
	
		case 9:
		{	/* null port, ENOENT expected */
			strcat(casedescript, 
				"Using null port, ENOENT expected.\n");
			port_to_use = MACH_PORT_NULL;
			expected_errno = ENOENT; /* no port is publicized */
			break;
		}
	
		case 10:
		{	/* set port, use null port, should clear slot */
			strcat(casedescript, 
				"Using null port to set, should clear slot.\n");
			ret = set_tnc_port(PORT_ID, my_receive2);
			if( ret != 0 ) {
				++int_err;
				fprintf(stderr,
			 	"can't set_tnc_port my_receive2 ret=%d\n", ret);
				return( -1 );
			}
			port_to_use = MACH_PORT_NULL;
			expected_errno = 0;		/* should clear slot */
			break;
		}

		case 11:
		{	/* no permission on port, expecting EPERM */
			strcat(casedescript, "Using port without permission granted, EPERM expected.\n");
			ret = setuid(config_userid1);
			if( ret != 0 ) {
				++int_err;
				fprintf(stderr,
			 	"can't setuid() ret=%d\n", ret);
				return( -1 );
			}
			port_to_use = my_receive1;
			expected_errno = EPERM;
			break;
		}
	
		default:
		{
			strcat(casedescript, 
				"Default case reached, internal error.\n");
			++int_err;
			fprintf(stderr, "Invalid test case %d must be 1 - %d\n",
		 	testcase, ntests);
			fflush(stderr);
			return( -1 );
		}
	}

	/* tell user what the testcase is testing.  */
	printf("%s", casedescript);
	fflush(stdout);

	/*
	 * Using what the test case set up, perform the test.
	 */
	errno = 0;
	ret = set_tnc_port(PORT_ID, port_to_use);
	actual_errno = errno;

	/*
	 * Now check the results against what was expected.
	 */
	if( expected_errno  == actual_errno ) {
		printf("PASSED %s TEST %2d\n", myname, testcase);
	} else {
		fprintf(stderr,
		 "FAILED %s TEST %2d: expected %d got errno %d\n",
		 myname, testcase, expected_errno, actual_errno);
		fflush(stderr);
	}

	return(0);		/* no internal error */
}
