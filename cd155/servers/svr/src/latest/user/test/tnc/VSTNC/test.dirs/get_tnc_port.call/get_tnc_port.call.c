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
 * $Log: get_tnc_port.call.c,v $
 * Revision 1.4  1994/11/18  21:06:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  01:43:21  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.2  93/06/09  17:18:16  yazz
 * Regularize VSTNC output.
 * 
 * Revision 3.1  93/03/01  14:19:58  jpaul
 * Set size before calling mach routines.
 * 
 * Revision 3.0  92/07/22  16:50:47  jpaul
 * Initial Checkin
 * 
 */
#include <errno.h>
#include <stdio.h>
#include <mach.h>
#include <mach/message.h>
#include "../common/vstnc.h"

#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

/* Number of tests.  */
int ntests = 10;
int int_err = FALSE;

#define PORT_ID		1717
char casedescript[MAXBUF]; /* desctiption of this testcase  */


char *myname;

/****************************************************************
 *
 * Module:	get_tnc_port.call
 *
 * Purpose:	This module tests the TNC function
 *		get_tnc_port().  Following TNC testing templates,
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
		fflush(stderr);
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
	int		ret;
	int		expected_errno, actual_errno;
	int		forknode, node_to_get;
	int		setflag, useridflag;
	int		waitstat;
	mach_port_t	port_to_set, port_to_get, *port_to_getp;

	/*
	 * Set up initial conditions.  Clear out any pre-existing
	 * port that's registered with our id.  Allocate rights that
	 * the tests can use.  The parent will publicize a port with
	 * set_tnc_port(); the child will use get_tnc_port() and also
	 * log the results.
	 */
	(void)set_tnc_port(PORT_ID, MACH_PORT_NULL);	/* clear any prev */

	ret = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
	 &port_to_set);
	if( ret != 0 ) {
		++int_err;
		fprintf(stderr, "can't alloc rcv rt #1 ret=%d\n", ret);
		fflush(stderr);
		return( -1 );
	}

	/*
	 * Specify the default values for the parameters that are varies
	 * by the test cases.  "get_()" is short for "get_tnc_port()".
	 */
	forknode = node_self();		/* child forks on our node */
	setflag = 1;			/* parent WILL call set_tnc_port() */
	useridflag = 0;			/* child will NOT change its userid */
	node_to_get = node_self();/* child uses this node for get_() */
	expected_errno = 0;		/* no error expected */
	port_to_getp = &port_to_set;	/* address used in get_() */

	switch( testcase ) {

	case 1:				/* most normal case */
	{
		strcat(casedescript, "Default params\n");
		break;
	}

	case 2:
	{
		strcat(casedescript, 
			"forknode = config_goodnode (should pass)\n");
		forknode = config_goodnode;
		break;
	}

	case 3:
	{
		strcat(casedescript, 
		  "node_to_get= config_badnode;  (should fail with EINVAL)\n");
		node_to_get = config_badnode;
		expected_errno = EINVAL;
		break;
	}

	case 4:
	{
		strcat(casedescript, 
			"forknode = config_goodnode, node_to_get = config_badnode, (should fail with EINVAL)\n");
		forknode = config_goodnode;
		node_to_get = config_badnode;
		expected_errno = EINVAL;
		break;
	}

	case 5:
	{
		strcat(casedescript, 
			"no call to get_tnc_port; (should fail with ENOENT)\n");
		setflag = 0;
		expected_errno = ENOENT;
		break;
	}

	case 6:
	{
		strcat(casedescript, "no call to get_tnc_port, forknode = config_goodnode; (should fail with ENOENT)\n");
		setflag = 0;
		forknode = config_goodnode;
		expected_errno = ENOENT;
		break;
	}

	case 7:
	{
		strcat(casedescript, "calling get_tnc_port, bad port address; (should fail with EFAULT)\n");
		port_to_getp = (mach_port_t *)config_badaddr;
		expected_errno = EFAULT;
		break;
	}

	case 8:
	{
		strcat(casedescript, "calling get_tnc_port, good node to fork to, bad port address; (should fail with EFAULT)\n");
		forknode = config_goodnode;
		port_to_getp = (mach_port_t *)config_badaddr;
		expected_errno = EFAULT;
		break;
	}

	case 9:
	{
		strcat(casedescript, "calling get_tnc_port, unathorized userid; (should fail with EPERM)\n");
		useridflag = 1;
		expected_errno = EPERM;
		break;
	}

	case 10:
	{
		strcat(casedescript, "calling get_tnc_port, unathorized userid, good node to fork to; (should fail with EPERM)\n");
		useridflag = 1;
		forknode = config_goodnode;
		expected_errno = EPERM;
		break;
	}

	default:
	{
		strcat(casedescript, "Unknown testcase\n");
		++int_err;
		fprintf(stderr, "Invalid test case %d must be 1 - %d\n",
		 testcase, ntests);
		fflush(stderr);
		return( -1 );
	}
	} /* end switch */

	/* tell user what the testcase is testing.  */
	printf("%s", casedescript);
	fflush(stdout);

	/*
	 * Using what the test case set up, perform the test.
	 */
	if( setflag != 0 ) {
		ret = set_tnc_port(PORT_ID, port_to_set);
		if( ret != 0 ) {
			++int_err;
			fprintf(stderr, "can't Set_tnc_port() ret=%d\n", ret);
			fflush(stderr);
			return( -1 );
		}
	}
	ret = rfork(forknode);
	if( ret < 0 ) {
		++int_err;
		fprintf(stderr, "can't rfork() to node %d ret=%d\n", forknode,
		 ret);
		fflush(stderr);
		return( -1 );
	}
	if( ret != 0 ) {	/* we are the parent -- wait for child */
		ret = wait(&waitstat);
		if( ret < 0 ) {
			++int_err;
			fprintf(stderr, "wait() failed ret=%d\n", ret);
			fflush(stderr);
			return( -1 );
		}
	} else {		/* we are child -- do test & log results */
		if( useridflag != 0 ) {
			ret = setuid(config_userid1);
			if( ret != 0 ) {
				++int_err;
				fprintf(stderr, "can't setuid() errno=%d\n",
				 errno);
				fflush(stderr);
				return( -1 );
			}
		}

		errno = 0;  /* ??? */
		ret = get_tnc_port(PORT_ID, node_to_get, port_to_getp);
		actual_errno = errno;
		if( actual_errno == expected_errno ) {
			printf("PASSED %s errno TEST %2d\n", myname, testcase);
			if( expected_errno == 0 ) {
				ret = do_send(*port_to_getp);
				if( ret != 0 ) {
					exit(1);	/* abnormal exit */
				}
			}
		} else {
			fprintf(stderr,
			 "FAILED %s errno TEST %2d: expected %d got %d\n",
			 myname, testcase, expected_errno, actual_errno);
			fflush(stderr);
			exit(1);	/* abnormal exit */
		}

		exit(0);	/* child terminates */
	}

	/*
	 * Now we're back in the parent and the child is done.  If the
	 * testcase called for the child to succeed with its get_tnc_port()
	 * call, and it did indeed succeed, then a message was sent.  So
	 * we verify that the child's send right was connected to our
	 * (the parent's) receive port by receiving that message.
	 */
	if( expected_errno == 0 && waitstat == 0 ) {
		ret = do_receive(port_to_set);
		if( ret != 0 ) {
			fprintf(stderr,
			 "FAILED %s port connectivity TEST %2d: err=0x%x\n",
			 myname, testcase, ret);	/* ret=mach error */
			fflush(stderr);
			exit(1);	/* abnormal exit */
		}
	}

	return(0);		/* no internal error */
}

/****************************************************************
 *
 * Function:	do_send()
 *
 * Returns:	0 for success, non-zero for failure
 *
 * Parameters:	port to send to
 *
 * Notes:	Called by the child process
 *
 ****************************************************************/

do_send(sendport)
mach_port_t	sendport;
{
	int			ret;
	mach_msg_header_t	snd_h;

	snd_h.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_PORT_SEND,0);
	snd_h.msgh_size = sizeof(snd_h);/* no items are sent, just msgh_id */
	snd_h.msgh_remote_port = sendport;
	snd_h.msgh_local_port = MACH_PORT_NULL;
	snd_h.msgh_id = PORT_ID;	/* value receiver expects */

	ret = mach_msg(&snd_h,
	 MACH_SEND_MSG|MACH_SEND_TIMEOUT,
	 snd_h.msgh_size,
	 0,
	 MACH_PORT_NULL,
	 10000,
	 MACH_PORT_NULL);
	return( ret );
}

/****************************************************************
 *
 * Function:	do_recv()
 *
 * Returns:	0 for success, non-zero for failure
 *
 * Parameters:	port to receive from
 *
 * Notes:	Called by the parent process
 *
 ****************************************************************/

do_receive(recvport)
mach_port_t	recvport;
{
	int			ret;
	mach_msg_header_t	rcv_h;

	rcv_h.msgh_id = ~PORT_ID;	/* NOT the value we expect */
	rcv_h.msgh_size = sizeof(rcv_h);/* no items are sent, just msgh_id */

	ret = mach_msg(&rcv_h,
	 MACH_RCV_MSG|MACH_RCV_TIMEOUT,
	 0,
	 rcv_h.msgh_size,
	 recvport,
	 10000,
	 MACH_PORT_NULL);

	if( ret != MACH_MSG_SUCCESS )
		return( ret );
	if( rcv_h.msgh_id != PORT_ID )
		return( -1 );
	return( 0 );
}
