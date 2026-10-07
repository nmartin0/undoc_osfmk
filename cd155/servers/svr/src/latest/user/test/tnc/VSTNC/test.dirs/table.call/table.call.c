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
 * $Log: table.call.c,v $
 * Revision 1.4  1994/11/18  21:08:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  01:50:04  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.4  93/07/12  17:38:40  yazz
 * Correct usage message.
 * 
 * Revision 3.3  93/06/09  17:25:43  yazz
 * Regularize VSTNC output.
 * 
 * Revision 3.2  93/03/09  13:08:00  jpaul
 * use sprintf instead of strcat
 * 
 * Revision 3.1  93/01/13  16:02:50  jpaul
 * Add diagnostic error messages to print more info for failure cases.
 * 
 * Revision 3.0  92/07/22  16:54:23  jpaul
 * Initial Checkin
 * 
 */
/*******************************************************************************

Module:		table.call.c

Purpose:	This module tests the TNC call table(TBL_NODEINFO).
		This piece runs on the local node and rfork()s over to
		the remote node.  Note that the shell, not the program,
		contains the main test loop to avoid wierd dependencies
		or influences from previous states.


*******************************************************************************/


#include "../common/vstnc.h"
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/table.h>

#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

#define DEBUG

int ntests = 4;		/* number of test cases */

char casedescript[MAXBUF]; /* desctiption of this testcase  */
char errmess[MAXBUF] = ""; /* diagnostic error message */

int table_nodecnt;
int table_nodelist[MAXNODES];
int int_err;
char *myname;

int get_table_nodelist(int *, int *);
int do_test(int, int *);
void log_results(int, int, int);
int nodeinfo_compare(int, int *);

main(
	int argc, 
	char *argv[])
{

	int test_case;		/* The test case number */
	int ret_value;		/* do_test() returns 0 for success else fail */
	int positive = 1;	/* != 0 if success expected */

	myname = argv[0];

	/* First, find out what test the shell asked us to run, checking
	   the validity of the request as well.
	*/
	if ((argc != 2) || ((test_case = conv_arg(argv[1])) == 0)) {
		fprintf(stderr, "usage:  table.call [1 - %d]\n", ntests);
		exit(1);
	}

	init_config_globals();  /* read config file. */

	/* This routine executes the test conditions. */
	ret_value = do_test(test_case, &positive);

	/* Handle internal bugs if found. */
	if (int_err) {
		fprintf(stderr,"Internal error: test case (%d)\n\n",test_case);
		exit(1);
	}

	/* Log the results. */
	log_results(positive, test_case, ret_value);

	exit(0);
}

/*******************************************************************************

Function:	do_test

Returns:	0 for success, 1 for failure, -1 for internal error.

Parameters:	test_case, the test case number, valid between 1 and ntests.

		ptr_positive, a pointer to a flag the routine clears if
		the test cases is a negative one.

Purpose:	This function executes just one test.  The test cases 
		correspond to the FV plan.


*******************************************************************************/


int
do_test(
	int test_case,
	int *ptr_positive)
{
	int i, j;
	int st;
	int anyfail = 0;
	int nodecount;
	int waitstatus;
	int retpid;

	/* Perform the test requested. */
	switch (test_case) {

	case 1:	
	{	/* 
		 * Positive test: read node table on local node
		 * and compare it to node table configuration info.
		 */
		strcat(casedescript, "Reading node table on the local node and comparing to the configuration info.\n");
		printf("%s", casedescript);
		fflush(stdout);
		*ptr_positive = 1;	/* say we expect success */
		st = setuid(config_userid1);  /* get to be a mortal user */
		if( st != 0 ) {
			fprintf(stderr, "setuid(config_userid1) failed\n");
			int_err = 1;
			return(-1);
		}

		st = get_table_nodelist(&table_nodecnt, table_nodelist);
		if( st != 0 ) {
			strcat(errmess, "get_table_nodelist failed.\n");
			return( 1 );
		}

		if( nodeinfo_compare(table_nodecnt, table_nodelist) != 0 ) {
			strcat(errmess, "nodeinfo_compare failed.\n");
			return( 1 );	/* failure return */
		}

		return( 0 );			/* success return */
	}

	case 2:	
	{	/* 
		 * Positive test: read the node table on all remote
	         * nodes and compare it to the known configuration
		 * information from the config file.
		 */
		strcat(casedescript,
		 "Compare node list from each remote node with config info.\n");
		printf("%s", casedescript);
		fflush(stdout);
		*ptr_positive = 1;	/* say we expect success */

		if( setuid(config_userid1) != 0 ) {
			fprintf(stderr, "setuid(config_userid1) failed\n");
			int_err = 1;
			return(-1);
		}

		for( i=0; i<config_nodecnt; ++i ) {
			st = rfork(config_nodelist[i]);
			if( st < 0 ) {
				fprintf(stderr, "rfork to node %d failed\n",
						config_nodelist[i]);
				int_err = 1;
				return(-1);
			}
			if( st > 0 ) {		/* we are the parent */
				retpid = wait(&waitstatus);
			} else {		/* child does test on remote */
				st = get_table_nodelist(&table_nodecnt,
						table_nodelist);
				if( st != 0 ) {
					strcat(errmess, 	
						"get_table_nodelist failed.\n");
					exit( 1 );	/* exit failure */
				}

				if( nodeinfo_compare(table_nodecnt,
						table_nodelist) != 0 ) {
					strcat(errmess, 
						"nodeinfo_compare failed.\n");
					exit( 1 );	/* exit failure */
				}

				exit( 0 );		/* exit success */
			}

			if (waitstatus != 0 ) {
				++anyfail;
			}
		}

		if( anyfail != 0 ) {
			return( 1 );		/* fail */
		}
		return( 0 );			/* success on every node */
	}

	case 3:	
	{ 	/* Negative test: attempt to write to the node table as a 
		 * mortal user.  This is not allowed.
		 */ 
		strcat(casedescript, "Attempting to write to node table as mortal (uid != 0) user. (Not allowed)\n");
		printf("%s", casedescript);
		fflush(stdout);
		*ptr_positive = 0;

		if( setuid(config_userid1) != 0 ) {
			fprintf(stderr, "setuid(config_userid1) failed\n");
			int_err = 1;
			return(-1);
		}
		/* the -1 nel should trigger a write to the table
		 * which,btw is not supported at this time.
		 */
		nodecount = table(TBL_NODEINFO, 0, table_nodelist, -1, 
 				sizeof(int));
		if ( nodecount > 0 ) {
			return( 0 );
		}
		return( 1 );
	}

	case 4:	
	{	/* Negative test: attempt to write to the node table as 
		 * superuser.  This is not allowed either.
		 */ 
		strcat(casedescript, "Attempting to write to node table as superuser (uid == 0) user. (Not allowed)\n");
		printf("%s", casedescript);
		fflush(stdout);
		*ptr_positive = 0;

		/* the -1 nel should trigger a write to the table
		 * which, btw, is not supported at this time and will
		 * not work for ANY table.
		 */
		nodecount = table(TBL_NODEINFO, 0, table_nodelist, -1,
				sizeof(int));
		if (nodecount > 0) {
			return( 0 );
		}
		return( 1 );
	}

	default:	/* Internal error if passed a bad flag */
	{
		strcat(casedescript, "Default case reached, internal error.\n");
		int_err = 1;
		return( -1 );
	}
	int_err = 1;		/* one of the cases forgot to do a return */
	return( -1 );
	}	/* end switch */

}


/*
 *		Function:	nodeinfo_compare
 *
 *		Returns:	0 for equal match, 1 otherwise.
 *
 *		Parameters:     nodecnt1_ptr, nodecnt2_ptr: 
 *		int pointers to the number of
 * 		node numbers in nodelist1 and nodelist2, respectively.
 *		nodelist1, nodelist2: arrays of ints that 
 *		contain the node numbers to be compared.
 *
 *		Purpose:	Compare two nodelists and node counts 
 * 				for being identical.
 *
 */
int
nodeinfo_compare(
	int	table_nodecount,
	int	table_nodelist[])
{
	int i;

	if( config_nodecnt != table_nodecnt ) {
		sprintf(errmess, "config_nodecnt(%d)!= table_nodecnt(%d)!\n",
			config_nodecnt, table_nodecnt);
		return( 1 );	/* failure return */
	}
	for( i=0; i<config_nodecnt; ++i ) {
		if( table_nodelist[i] != config_nodelist[i] ) {
			sprintf(errmess, 
			  "table_nodelist[%d]!= config_nodelist[%d]!\n", i, i);
			return( 1 );	/* failure return */
		}
	}
	return( 0 );			/* success return */
}

/*******************************************************************************

Function:	log_results

Returns:	void

Parameters:	positive, a flag cleared by do_test if a test case is
		negative (expects failure).

		test_case, the test case number, valid between 1 and ntests.

		ret_value, xxxx

Purpose:	This function reports tests results.  Good results go to
		stdout; bad results go to stderr.


*******************************************************************************/


void log_results(
	int positive,
	int test_case,
	int ret_value)
{
	if ((positive && ret_value == 0) || (!positive && ret_value != 0))
		printf("PASSED %s TEST %2d\n", myname, test_case);
	else {
		fprintf(stderr, "FAILED %s TEST %2d\n", myname, test_case);
		fprintf(stderr, "%s", errmess);
		fflush(stderr);
	}

}

/*******************************************************************************

Function:	get_table_nodelist

Returns:	0 for success, -1 for error

Parameters:	nodenum_ptr, address of an int that is set to the
		number of nodes found in the table

		nodelist, address of 0th element of an int array into
		which the list of valid node numbers is placed

Purpose:	Reports the info provided by the table(TBL_NODEINFO) call


*******************************************************************************/


get_table_nodelist(
	int	*nodecount_ptr,
	int	nodelist[])
{
	int nodecount;

	/* get the count of the nodes from the table. */

	nodecount = table(TBL_NODEINFO, 0, nodelist, MAXNODES, sizeof(int));

	*nodecount_ptr = nodecount;

	if( nodecount >= MAXNODES  ||  nodecount <= 0 ) {
		fprintf(stderr, "TEST ABORTED: MAXNODES=%d, table says %d\n\n",
				MAXNODES, nodecount);
		fflush(stderr);
		return(-1);
	}

	return(0);
}

/* dump out the config list.  */
dump_configlist()
{
#ifdef DEBUG
	int nodeindex = 0;
	printf("There are %d nodes in the table.\n", table_nodecnt);
	while (table_nodelist[nodeindex] != NULL)
		printf("nodelist[%d]=%d.\n", nodeindex, table_nodelist[nodeindex++]);
#endif /* DEBUG */
}


/* dump node table */
dump_nodetable() 
{
#ifdef DEBUG
	int i;
	printf("There are %d nodes in the table.\n", table_nodecnt);
	for (i = 0; i < table_nodecnt; i++)
		printf("nodelist[%d]=%d.\n",i, table_nodelist[i++]);
#endif /* DEBUG */
}
