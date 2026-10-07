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
/*******************************************************************************

Module:		migrate.call.c


Purpose:	This module tests the TNC call migrate().
		This piece runs on the local node and migrates()s over to
		the remote node.  Note that the shell, not the program,
		contains the main test loop to avoid wierd dependencies
		or influences from previous states.


*******************************************************************************/


#include "../common/vstnc.h"
#include <sys/table.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>

#define DEBUG 1
#undef DEBUG

#define	MIGOK_NODEOK	0	/* child exit status: migrate OK, new node OK */
#define MIGOK_NODEBAD	1	/* migrate status OK but new node is wrong */
#define MIGBAD		2	/* migrate failed */

#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

int ntests = 5;		/* number of test cases */

char *myname;
char casedescript[MAXBUF] = { "" };	/* description of this testcase  */
int targetnode;		/* target node 		*/
int int_err;		/* internal error? 	*/
int expected;		/* Each test case sets this for itself */
int table_nodecnt;
int table_nodelist[MAXNODES];

int do_test(int);
void log_results(int,int);

main(
	int argc, 
	char *argv[])
{

	int test_case;		/* The test case number */
	int retvalue;		/* do_test() returns 0 for success else fail */

	myname = argv[0];

	/* First, find out what test the shell asked us to run, checking
	   the validity of the request as well.
	*/
	if ((argc != 2) || ((test_case = conv_arg(argv[1])) == 0)) {
		fprintf(stderr, "usage:  %s [1 - %d]\n", myname, ntests);
		fflush(stderr);
		exit(1);
	}


	init_config_globals();  /* read config file. */

	/* This routine executes the test conditions. */
	retvalue = do_test(test_case);

	/* Handle internal bugs if found. */
	if (int_err) {
		fprintf(stderr,"Internal error: test case (%d)\n\n",test_case);
		fflush(stderr);
		exit(1);
	}

	/* Log the results. */
	log_results(test_case, retvalue);

	exit(0);
}

/*******************************************************************************

Function:	do_test

Returns:	0 for success, 1 for failure, -1 for internal error.

Parameters:	test_case, the test case number, valid between 1 and ntests.


Purpose:	This function executes just one test.  The test cases 
		correspond to the FV plan.


*******************************************************************************/


int
do_test(
	int test_case)
{
	int i, j;
	int st;
	int anyfail = 0;
	int nodecount;
	int waitstatus;
	int retpid;

	/* Perform the test requested. */
	switch (test_case) {

	/*
	 * Boundary condition tests 
	 *
	 * Migrate to the current node.  No error should occur.
	 * No migration to any other node should ocur.
	 */
	case 1:
	{
		int childpid;
		int retval;
		int node;
		int waitstatus;

		strcat(casedescript, "Migrating to current node.\n");
		expected = MIGOK_NODEOK;/* say what's expected */
		targetnode = node_self();

		childpid = fork();	/* try to create a child process */
		if (childpid < 0) {	/* created no child -- internal error */
			int_err = 1;	
			fprintf(stderr, "unable to fork.  Errno: %d\n",
				errno);
			fflush(stderr);
			return( -1 );
		}
		else if (childpid == 0 ) {	/* we are the child */
			retval = migrate(node_self());
			if (retval == -1) /* migrate failed */
				exit(MIGBAD);

			/* migrate succeded; check new node. */
			if (node_self() != targetnode)
				exit(MIGOK_NODEBAD);
			else
				exit(MIGOK_NODEOK);
		}
		else {				/* we are the parent */
			wait(&waitstatus);
			return( waitstatus>>8 );
		}
		break;				/* should never reach here */
	}

	/* Migrate to a negative node number.  This should fail. */
	case 2:
	{
		int childpid;
		int retval;
		int negnode = -17;
		int waitstatus;

		expected = MIGBAD;

		strcat(casedescript, 
			"Migrating to a negative node. (should fail)\n");

		childpid = fork();	/* try to create a child process */
		if (childpid < 0) {	/* created no child -- internal error */
			int_err = 1;	
			fprintf(stderr, "unable to fork.  Errno: %d\n",
				errno);
			fflush(stderr);
			return( -1 );
		}
		else if (childpid == 0 ) {	/* we are the child */
			retval = migrate(negnode);
			if (retval == -1)	/* migrate failed as expected */
				exit(MIGBAD);
			/* BAD News, we migrated. */
			exit(MIGOK_NODEBAD);
		}
		else {				/* we are the parent */
			wait(&waitstatus);
			return( waitstatus>>8 );
		}
		break;				/* should never reach here */
	}

	
	/* Migrate to a node number 1 less than smallest.  This should fail. */
	case 3:
	{
		int childpid;
		int retval;
		int toosmallnode;
		int waitstatus;

		expected = MIGBAD;

		strcat(casedescript, 
			"Migrating to a nonvalid node. (should fail)\n");
#ifdef DEBUG
		{
		int x;
		printf("config_nodecnt is %d.\n", config_nodecnt);
		for (x=0; x< config_nodecnt; x++)
			printf("config_node of %d is %d.\n",x,config_nodelist[x]);
		}
#endif /* DEBUG  *

		/* find the first node in the config table. */
		toosmallnode = config_nodelist[0] - 1;	

		childpid = fork();	/* try to create a child process */
		if (childpid < 0) {	/* created no child -- internal error */
			int_err = 1;	
			fprintf(stderr, "unable to fork.  Errno: %d\n",
				errno);
			fflush(stderr);
			return( -1 );
		}
		else if (childpid == 0 ) {	/* we are the child */
			retval = migrate(toosmallnode);
			if (retval == -1)	/* migrate failed as expected */
				exit(MIGBAD);
			/* BAD News, we migrated */
			exit(MIGOK_NODEBAD);
		}
		else {				/* we are the parent */
			wait(&waitstatus);
			return( waitstatus>>8 );
		}
		break;				/* should never reach here */
	}

	/* migrate to a node one greater than the max node.  
	 * Can't do it.
	 */
	case 4:
	{
		int childpid;
		int retval;
		int toobignode;
		int waitstatus;

		expected = MIGBAD;

		strcat(casedescript, 
			"Migrating to a invalid node. (should fail)\n");
		/* find the last node in the config table. */
		toobignode = (config_nodelist[config_nodecnt-1] + 1);	

		childpid = fork();	/* try to create a child process */
		if (childpid < 0) {	/* created no child -- internal error */
			int_err = 1;	
			fprintf(stderr, "unable to fork.  Errno: %d\n",
				errno);
			fflush(stderr);
			return( -1 );
		}
		else if (childpid == 0 ) {	/* we are the child */
			retval = migrate(toobignode);
			if (retval == -1)	/* migrate failed as expected */
				exit(MIGBAD);
			/* BAD News, we migrated */
			exit(MIGOK_NODEBAD);
		}
		else {				/* we are the parent */
			wait(&waitstatus);
			return( waitstatus>>8 );
		}
		break;				/* should never reach here */
	}

	case 5:	
	{	/* 
		 * Variable test.  Try to migrate to every node number
		 * from the lowest to the highest, including holes in
		 * the table representing invalid node numbers.
		 * Locally, we expect success or failure depending on 	
		 * the validity of each node.  We return the number of
		 * cases that did not match our expectation.
		 */
	
		int childpid;		/* forked file's pid */
		int retval; 		/* return val */
		int waitstatus;		/* wait() status */
		int node;		/* step thru node list */
		int st;			/* return stat */
		int my_expected;	/* My expected return stat */
		int nfailures = 0;	/* # times our expectation not met */

		expected = 0;		/* we expect perfection */

		strcat(casedescript, "Migrating to all nodes, including holes in list. (nonvalid nodes should fail)\n");
		/*
		 * Try to migrate to every node number from the lowest
		 * valid one to the highest valid one, including any
		 * invalid ones in between.  Set "my_expected"
		 * according to whether the node is a valid one or not.
		 * Note the clever loop processing.
		 */
		i = 0;			/* i indexes VALID nodes only */
		node = config_nodelist[0];	/* node can be valid or not */
		for( ;node <= config_nodelist[config_nodecnt-1]; ++node ) {
			if( node == config_nodelist[i+1] )
				++i;	/* node has reached next valid number */
			if( node == config_nodelist[i] )
				my_expected = MIGOK_NODEOK;
			else
				my_expected = MIGBAD;

			childpid = fork();	
			if (childpid < 0) {
				int_err = 1;	
				fprintf(stderr, "unable to fork.  Errno: %d\n",
						errno);
				fflush(stderr);
				return( -1 );
			}
			else if( childpid == 0 ) {	/* we are the child */
				st = migrate(node);
				if( st < 0 )
					exit(MIGBAD);
				if( node_self() == node )
					exit(MIGOK_NODEOK);
				exit(MIGOK_NODEBAD);
			}
			else {
				wait(&waitstatus);
				if ((waitstatus >>8 ) != my_expected) {
					++nfailures;

					/* Log that we expected bad, got good */
					if (my_expected == MIGBAD)
						fprintf(stderr, "migrate to node %d failed, but should have succeeded.\n\n.", node);
						fflush(stderr);
					/* Log that we expected good, got bad */
					if (my_expected == MIGOK_NODEOK) {
						fprintf(stderr, "migrate to node %d succeeded, but should have failed.\n\n.", node);
					fflush(stderr);
					}
				}
			}
		}  /* end clever loop  */

		return( nfailures );	/* loop done.  we expect 0 failures */

	} /* end case 5  */

	default:
		int_err = 1;	/* one of the cases forgot to do a return */
		strcat(casedescript, 
			"default case reached, internal error.\n");
		fflush(stderr);
		return( -1 );

	}	/* end switch */

	/* tell user what the testcase is testing */
	printf("%s", casedescript);
	fflush(stdout);

}  /* end module  do_test */


/*******************************************************************************

Function:	log_results

Returns:	void

Parameters:	positive, a flag cleared by do_test if a test case is
		negative (expects failure).

		test_case, the test case number, valid between 1 and ntests.

		retvalue, xxxx

Purpose:	This function reports tests results.  Good results go to
		stdout; bad results go to stderr.


*******************************************************************************/
void log_results(
	int test_case,
	int retvalue)
{
	if( retvalue == expected )
		printf("PASSED case %2d test %s\n", test_case, myname);
	else 
		{
		printf("FAILED case %2d test %s\n", test_case, myname);
		fprintf(stderr, "FAILED case %2d test %s\n", test_case, myname);
		fflush(stderr);
		}

}

/* dump out the config list.  */
dump_configlist()
{
#ifdef DEBUG_TNC
	nodeindex = 0;
	printf("There are %d nodes in the table.\n", config_nodecnt);
	while (nodearray[nodeindex] != NULL)
		printf("nodearray[%d]=%d.\n",nodeindex,nodearray[nodeindex++]);
#endif /* DEBUG_TNC */
}
