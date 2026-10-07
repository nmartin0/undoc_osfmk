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
 * $Log: rfork.call.c,v $
 * Revision 1.4  1994/11/18  21:08:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/03/14  17:57:47  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 3.2  93/06/20  18:31:07  yazz
 * [ ad1.04 merge ]
 * 	Zero errno explicitly, lest an inapropriate test failure result.
 * 
 * Revision 3.1  93/06/09  17:22:19  yazz
 * Regularize VSTNC output.
 * Briefened some unnecessarily lengthy sleep delays.
 * 
 * Revision 3.0  92/07/22  16:52:53  jpaul
 * Initial Checkin
 * 
 */
/*******************************************************************************

Module:		rfork.call.c

Purpose:	This module tests the TNC function rfork().  This piece 
		runs on the local node and rfork()s over to the remote
		node.  Note that the shell, not the program, contains
		the main test loop to avoid wierd dependencies or 
		influences from previous states.


*******************************************************************************/

/* Include files */
#include <stdio.h>        /* The usual stuff */
#include <ctype.h>        /* For operating on characters */
#include <errno.h>        /* To check the return */
#include <string.h> 	  /* For string comparisons */
#include <setjmp.h>       /* More signal handling */
#include <sys/uio.h>      /* Can't do read() w/o this!?! */
#include <sys/types.h>    /* For various system types */
#include <sys/wait.h>     /* For using the wait call */
#include <unistd.h>       /* Standard types and macros */
#include <sys/signal.h>   /* For signal handling */
#include "../common/vstnc.h"

/*	Constants	*/

#undef DEBUG
#define DEBUG 1

#define MAXBUF 255


#define NODE_ZERO	0  /* The system may have a valid node zero */
#define NODE_NEG_ONE   -1  /* The system may not have a valid negative node */	
#define NODE_NEG_OTH -999  /* The system may not have a valid negative node */
#define WIERD_NUM -696969  /* This unusual number used to initialize values
			      to obviously invalid numbers. */

#define WAIT_REM_PROC 30   /* Wait at most thirty seconds for the child proc 
			      to start */

#define WAIT_CHILD_LOG 10  /* Give the child ten seconds to write to the log
			      file before the parent writes */

#define RFORK_ERR_CODE -1  /* rfork returns this if it fails */

#define OFF             0  /* Turns off alarm */

/* Various internal and exit status codes */
#define OK              0  /* Status okay */
#define ENOTEST         1  /* Argument is invalid. */
#define ENOGREETING     2  /* Parent did not receive greeting from child. */
#define EMUNGGREETING   3  /* Parent received trashed gretting. */
#define EDISCO          4  /* Pipe lost, closed, or not read. */
#define EPLUGGEDPIPE    5  /* The child can't write pipe. */
#define EPARTIAL        6  /* The child can only partially write pipe. */
#define EINTERR       999  /* An internal error occured. */

/* The logical constants */
#ifndef FALSE
#define FALSE           0
#endif /* !FALSE */
#ifndef TRUE
#define TRUE            1
#endif /* !TRUE */

/* Communication string for child to send to parent */
#define CHILD_MSG       "`Hello, Dad?  I'm in jail!'  -- Don Was."
#define CHILD_MSG_BYTES sizeof(CHILD_MSG)

/*      Structures and Types */

/* The basic true/false type */
typedef short bool;


/*	Globals		*/

/* Internal error flag */

bool int_err = FALSE;

/* Jumping context--coded on Feb. 29, Leap Year, BTW :-)  */
jmp_buf context;

/* 
 * Macros for nodes used in test cases
 */
#define	SELF			0
#define	ONE_VALID_NODE		1
#define	ONE_INVALID_NODE	2

int ntests = 6;

char casedescript[MAXBUF] = { "" };	/* description of this testcase */

main(argc, argv, envp)
char *argv[], *envp[];
{
int test_case;               /* The test case number */
pid_t do_test(), child_pid;  /* The do_test routine returns the child pid. */
bool positive = TRUE;        /* Test case type is usually positive */
void log_parent_results();   /* Write parent test results to log file */
void log_child_results();    /* Write parent test results to log file */
int tell_parent();           /* The child sends a message to the parent */
int listen_child();          /* The parent listens for the child */
int talk_status = OK;        /* Status of parent child conversation */
int comm_pipe[2];            /* Pipe for parent and child */
int child_stat;		     /* Status returned by child */
int rfork_errno = WIERD_NUM; /* Get rfork's errno in negative test. */
int req_node;                /* The requested node */


/* First, find out what test the shell asked us to run, checking
   the validity of the request as well.
 */
if ((argc != 2) || ((test_case = conv_arg(argv[1])) == 0)) {
	fprintf(stderr, "usage:  rfork.call [1 - %d]\n", ntests);
	exit(ENOTEST);
}

init_config_globals();

/* Next, set up the communications pipe for the child to talk 
   to the parent. 
 */
if (pipe(comm_pipe)) {
	fprintf(stderr, "Internal error:  could not open pipe.\n");
	fflush(stderr);
	exit(EINTERR);
}

/* The rubber hits the road here.  This routine executes the test conditions. */
child_pid = do_test(test_case, &req_node, &positive, &rfork_errno);


/* Handle internal bugs if found. */
if (int_err) {
	fprintf(stderr, "Internal error:  test case (%d) or node (%d).\n",
		test_case, req_node);
	fflush(stderr);
	exit(EINTERR);
}


/* Check out remote pipe communication, then log results. */
if (child_pid == 0) {
	talk_status = tell_parent(comm_pipe[1]);
	log_child_results(positive, test_case, req_node, talk_status);
} else {

	/*
	 * If we are the parent process, listen to the child for message.
	 * Else, we just log the results.
	 */
	if (child_pid > (pid_t) 0)  {
		talk_status = listen_child(comm_pipe[0]);
	}
	log_parent_results(positive, test_case, child_pid, talk_status, 
			rfork_errno);
}

/* Return a the result code, which for the parent requires checking
   on the child. */
if (child_pid == 0) {
	/* If the child gets this far, everything went okay. */
	exit(OK);

} else if (child_pid > (pid_t)0) {
	/* The parent should check the child status and return an 
	   error if the child did.  */
	child_stat = 0;
	(void)wait(&child_stat);
	exit(child_stat);

} else
        /* No child, then no error. */
	exit(OK);

}


/*******************************************************************************

Function:	do_test

Returns:	the code returned by rfork.  If the test case was 
		negative valid iff RFORK_ERR_CODE, and if the test
		case was positive valid iff the child PID.

Parameters:	test_case, the test case number, valid between 1 and ntests.

		ptr_req_node, a pointer to storage for the node this routine 
		asks the child process to start on.

		ptr_positive, a pointer to a flag the routine clears if
		the test cases is a negative one.

		ptr_rfork_errno, a pointer to storage for the errno returned
		by rfork in negative cases.

Purpose:	This function executes the tests.  The test cases 
		correspond to the cases in section 3 of the FV plan.

*******************************************************************************/

pid_t do_test(test_case, ptr_req_node, ptr_positive, ptr_rfork_errno)
int test_case, *ptr_req_node, *ptr_rfork_errno;
bool *ptr_positive;
{
	int node_lookup(int);            /* Gets a specified node */
	pid_t child_pid;              /* The rfork return value. */

	/* Set some values based on the test case. */
	switch (test_case) {
	
		case 1:	/* Positive test: local node */
		{
			strcat(casedescript, "rforking to local node.\n");
			if ((*ptr_req_node = node_lookup(SELF)) < 0)
				int_err = TRUE;
			break;
		}
	
		case 2:	/* Positive test: remote node */
		{
			strcat(casedescript, "rforking to remote node.\n");
			if ((*ptr_req_node = node_lookup(ONE_VALID_NODE)) < 0)
				int_err = TRUE;
			break;
		}
	
		case 3:	/* Negative test: invalid node */
		{
			strcat(casedescript, "rforking to invalid node.\n");
			*ptr_positive = FALSE;
			if ((*ptr_req_node = node_lookup(ONE_INVALID_NODE)) < 0)
				int_err = TRUE;
			break;
		}
	
		case 4:	/* Positive test: node zero */
		{
			strcat(casedescript, "rforking to invalid node.\n");
			/* Here be dragons:  If the sys admin does not configure
		 	* the TNC cluster with a node numbered zero, this case
		 	* will report an error in rfork which really lies in an
		 	* invalidated assumption in the test.
		 	*/
			*ptr_req_node = NODE_ZERO;
			break;
		}
	
		case 5:	/* Negative test: node negative one */
		{
			strcat(casedescript, "rforking to node (-1).\n");
			*ptr_positive = FALSE;
			*ptr_req_node = NODE_NEG_ONE;
			break;
		}
	
		case 6:	/* Negative test: node other negative */
		{
			strcat(casedescript, 
				"rforking to invalid negative node.\n");
			*ptr_positive = FALSE;
			*ptr_req_node = NODE_NEG_OTH;
			break; 
			
		}

		default:	/* Internal error if passed a bad flag */
		{
			strcat(casedescript, 
				"default case reached, internal error.\n");
			int_err = TRUE;
			break;
		}
	}	/* end switch */
	/* tell user what the testcase is testing.  */
	printf("%s", casedescript);
	fflush(stdout);

	/* Okay, let's rfork the child and save the errno. */
	errno = 0;
	child_pid = rfork(*ptr_req_node);
	*ptr_rfork_errno = errno;
	
	return(child_pid);
}

/*******************************************************************************

Function:	log_parent_results

Returns:	void

Parameters:	positive, a flag cleared by do_test if a test case is
		negative.

		test_case, the test case number, valid between 1 and ntests.

		child_pid, returned by rfork through do_test.  

		talk_status, a status word set by listen_child().

		rfork_errno, the error code returned by rfork if it failed

Purpose:	This function reports tests results for the parent.

*******************************************************************************/

void log_parent_results(positive, test_case, child_pid, talk_status, rfork_errno)
bool positive;
pid_t child_pid;
int test_case, talk_status, rfork_errno;

{

/* Since the child may exist, give him some time to log his results */
(void)sleep(WAIT_CHILD_LOG);


if (positive) {

        /*
	 * In the positive case, the test results depend on whether the
	 * child successfully sent his greeting to the parent.
	 */
	switch (talk_status) {
		case OK:  /* Child sent greeting okay */
			printf("PASSED rfork.call TEST %2d (parent)\n",
			 test_case);
			break;

		case ENOGREETING:  /* Never heard from child */
			fprintf(stderr, "FAILED rfork.call TEST %2d (parent)\n", 
			 test_case);
			fprintf(stderr,
				"parent never received child's message.\n");
			fflush(stderr);
			break;

		case EMUNGGREETING:  /* Child's greeting got trashed */
			fprintf(stderr, "FAILED rfork.call TEST %2d (parent)\n", 
			 test_case);
			fprintf(stderr, "child message garbled.\n");
			fflush(stderr);
			break;

		case EDISCO:  /* Pipe to child went south */
			fprintf(stderr, "FAILED rfork.call TEST %2d (parent)\n",
			 test_case);
			fprintf(stderr, "pipe disappeared.\n");
			fflush(stderr);
			break;

		default: /* Should never happen */
			int_err = TRUE;
			break;
	}

	/*
	 * Do away with fancy parent-child communication stuff
	 * if rfork is supposed to fail.
	 */
} else if ((rfork_errno == EINVAL) && (child_pid == RFORK_ERR_CODE)) {

        /*
	 * In the negative case, the rfork should set errno and return
	 * RFORK_ERR_CODE, and the attempted talk should fail.  No child
	 * will be there to print a passed message so we print 2 of them,
	 * to keep the count even.
	 */
	printf("PASSED rfork.call TEST %2d (parent)\n", test_case);
	printf("PASSED rfork.call TEST %2d (no child)\n", test_case);
} else {
	fprintf(stderr, "FAILED rfork.call TEST %2d (child)\n", test_case);
	fprintf(stderr, "rfork set errno to %d and talk status is %d.\n", 
	 rfork_errno, talk_status);
	fflush(stderr);
}
return;
}

/*******************************************************************************

Function:	log_child_results

Returns:	void

Parameters:	positive, a flag the do_test function clears iff the test 
		case is a negative one.

		test_case, the test case number, valid between 1 and ntests.

		req_node, the node the parent program asked to execute 
		this program

		talk_status, a status word indicating how the write to
		the parent went

Purpose:	This function reports child tests results.  If the test 
		case was positive, then we should get here.  On the other 
		hand, a negative test case has no business invoking a
		program, either remotely or locally.

*******************************************************************************/

void log_child_results(positive, test_case, req_node, talk_status)
	bool positive;
	int test_case, req_node, talk_status;
{
	int node_lookup(int);
	int node;			/* Stores the execution node */

	/* Look up the node number the program runs on */
	if ((node = node_lookup(SELF)) < 0)
		int_err = TRUE;

	/* Decide if the right thing happened to the child */
	if (positive) {

		/* This is the success condition for positive test cases. */
		if ((node == req_node) && (talk_status == OK)) {
			printf("PASSED rfork.call TEST %2d (child)\n", test_case);

		} else {
		/* At least one thing went wrong, so check and log the error(s). */

			/* Check to see if we spawned on the wrong node. */
			if (node != req_node) {
				fprintf(stderr, "FAILED rfork.call TEST %2d\n", 
				 test_case);
				fprintf(stderr, "child on node %d, requested %d.\n",
				 node, req_node);
				fflush(stderr);
			}

			/* Check if the message to parent was blown. */
			switch (talk_status) {
				case OK:  /* Child sent greeting okay */
				printf("PASSED rfork.call TEST %2d\n", 
					 test_case);
				break;

				case EPLUGGEDPIPE:  /* Couldn't write pipe */
					fprintf(stderr, 
					 "FAILED rfork.call TEST %2d\n", test_case);
					fprintf(stderr, "child's pipe write failed.\n");
					break;
	
				case EPARTIAL:  /* Only some of message got written */
					fprintf(stderr, "FAILED rfork.call TEST %2d\n",
					 test_case);
					fprintf(stderr, "message partially lost.\n");
					break;

				case EDISCO:  /* Pipe to parent went south */
					fprintf(stderr, "FAILED rfork.call TEST %2d\n",
					 test_case);
					fprintf(stderr, "parent-child pipe lost.\n");
					break;
	
				default: /* Should never happen */
					int_err = TRUE;
					break;
			}
		}
	} else {
		/* Negative test case means no child should run */
		fprintf(stderr, "FAILED rfork.call TEST %2d (child exists!)\n",
		 test_case);
		fprintf(stderr, "unexpected child running on node %d.\n", node);
	}
	fflush(stderr);
	return;
}

/*******************************************************************************

Function:	tell_parent

Returns:	OK if write to pipe succeeded
		EPLUGGEDPIPE if it can't write the pipe 
		EPARTIAL if only some of the data went into the pipe
		EDISCO if the pipe disconnects

Parameters:	parent_pipe, the file descriptor of the writing end
		of the parent-child communication pipe

Purpose:	Sends a message to the parent process to tell the parent
		the child's alive.


*******************************************************************************/

int tell_parent(parent_pipe)
int parent_pipe;

{
void no_reader_on_pipe();      /* If the parent won't read the pipe */
int ret_val = OK;              /* To return status word */
int nbytes;                    /* Count of bytes actually written */
sigset_t block_sigs;	       /* Mask for signals */

	/* Write the string to the pipe and check the bytes written */
	if ((nbytes = write(parent_pipe, CHILD_MSG, CHILD_MSG_BYTES)) == 0)
	    	ret_val = EPLUGGEDPIPE;

	else if (nbytes != CHILD_MSG_BYTES)
		ret_val = EPARTIAL;

	return(ret_val);
}

/*******************************************************************************

Function:	listen_child

Returns:	OK if read from pipe succeeded
		ENOGREETING if the child doesn't write the pipe
		EMUNGGREETING if the pipe contains trash
		EDISCO if the pipe is closed

Parameters:	child_pipe, the file descriptor of the reading end
		of the parent-child communication pipe

Purpose:	Waits for at most five minutes for a message from the 
		child process telling the parent the child's alive.



*******************************************************************************/

int listen_child(child_pipe)
int child_pipe;

{
void get_awoken();                  /* Handles alarm if heard */
char buffer[CHILD_MSG_BYTES];       /* To hold the pipe data */
int ret_val = OK;                   /* Store the return value */
sigset_t block_sigs;		    /* Mask for signals */
extern int errno;

/* Tell the system about the action to take. */
(void)signal(SIGALRM, get_awoken);

/* If setjmp returns non-zero here, the alarm woke it up */
if (!setjmp(context)) {

	/* Wait at most five minutes for the child to write the pipe */
	(void)alarm(WAIT_REM_PROC);

	/* Read the string from the pipe and check it */
	if (((read(child_pipe, buffer, CHILD_MSG_BYTES)) != CHILD_MSG_BYTES) || 
		(strcmp(buffer, CHILD_MSG)))
		ret_val = EMUNGGREETING;

	/* Verify that one is not mistaking a broken pipe for a munged
	   greeting, which is possible given the logic above. */
	if (errno == EBADF)
		ret_val = EDISCO;

} else 
	ret_val = ENOGREETING;

/* Set up to ignore the alarm here */
(void)alarm(OFF);

/* Return to default alarm signal handling */
(void)signal(SIGALRM, SIG_DFL);

return(ret_val);
}

/*******************************************************************************

Function:	get_awoken

Returns:	ENOGREETING through longjmp, indicating that the
		parent waited five minutes but received no message
		from the child.

Parameters:	None

Purpose:	Signal handler for the alarm clock


See Also:	

~rex/tstone/fvplans	This directory holds all the FV plans, including
		rfork.doc, which one should read before running or modifying
		this program.

*******************************************************************************/

void get_awoken()

{

/* There's nothing to do but go back to listen_child */
longjmp(context, ENOGREETING);

/*NOTREACHED*/
return;

}

/*******************************************************************************

Function:	no_reader_on_pipe

Returns:	EDISCO through longjmp to indicate that the pipe
		connection went away.

Parameters:	None

Purpose:	Signal handler for the SIGPIPE signal, which one
		receives if the pipe has no open read file 
		descriptors and one attempts to write it.

*******************************************************************************/

void no_reader_on_pipe()

{

/* There's nothing to do but go back to tell_parent. */
longjmp(context, EDISCO);

/*NOTREACHED*/
return;

}
 
/*******************************************************************************

Function:	node_lookup

Returns:	the host node number if the caller asks for SELF
		a valid node number if passed ONE_VALID_NODE
		an invalid node number if passed ONE_INVALID_NODE

Parameters: 	node_type, what kind of node do we what.	

Purpose:	generate valid and invalid node numbers for the calling routine 
		using either the node table or the configuration file.


******************************************************************************/

int 
node_lookup(int node_type)
{
	int	this_node, index; 
	int 	status, temp_node;
	int	*node_number_table;
	int	number_of_nodes;
	FILE	*fd;

	this_node = node_self();

	/*
	 * If looking for host node itself, return node_self().
	 */
	if (node_type == SELF) 
		return(this_node);
	/*
	 * Else, get a list of valid node numbers, so we can return
	 * either a valid or invalid node nubmer.
	 */
	else { 
		if (node_type == ONE_VALID_NODE) {
			/* 
			 * return a valid node other than node_self().
			 */
			return(config_goodnode);
		}

		/* 
		 * Use configuration file for an invalid node number.
		 */
		if (node_type == ONE_INVALID_NODE) {
			return(config_badnode);
		}

		/*
		 * We should never reach here.  Log error.
		 */
		fprintf(stderr, "invalid option to node_lookup().\n");
		fflush(stderr);
		int_err = TRUE;
		return(-1);
	}
}
