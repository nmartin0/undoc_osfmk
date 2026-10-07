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
/* 
 * HISTORY
 * $Log: getprocinfo.c,v $
 * Revision 1.2  1994/11/18  21:06:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/16  20:25:08  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Built
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:16:38  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.1  93/08/27  14:22:05  hao
 * 	Initial check-in.
 * 
 */
#include <errno.h>
#include <stdio.h>
#include "../common/vstnc.h"

/*
 * Number of tests.
 */
int ntests = 7;

#define	CHILDMAX 10

#define DEBUG 0
#ifndef MAXBUF
#define MAXBUF 255
#endif /* !MAXBUF */

int int_err = FALSE;
int compare_array(int *, int *);
void print_pid_array(int *);

char casedescript[MAXBUF]; /* desctiption of this testcase  */

char	*CONFIG_GOODPATH = "/chkpnt/proc.1000";
char	*CONFIG_BADPATH = "/chkpnt/proc.bad";
char	*CONFIG_GOODFILE = "/chkpnt/goodfile";
char	*CONFIG_BAD_DATA_PATH = "/chkpnt/proc.incomplete";

int	config_nproc = 4;
int	config_pid_list[4]={1, 111111, 200, 500};
int	config_ppid_list[4]={1, 20000, 1, 1000};
int	config_node_list[4]={0, 2, 0, 1};
int	config_id = 10;

char *myname;
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
	int 		nproc, id, count, status;
	int		expected_return, expected_errno;
	int		actual_return;
	pid_t		*pid_list, *ppid_list;
	int		*node_list;
	char		*checkpoint_dir_path;

	errno = 0;

	switch(testcase) {

		case 1:
		{
			expected_return = 0;
			expected_errno = 0;
			checkpoint_dir_path = CONFIG_GOODPATH;
			break;
		}

		case 2:
		{
			expected_return = -1;
			expected_errno = EACCES;
			checkpoint_dir_path = CONFIG_GOODPATH;
			if( setuid(config_userid1) != 0) {
				fprintf(stderr, "can't setuid() for testcase 2.\n");
				int_err = 1;
				return(-1);
			}
			break;
		}

		case 3:
		{
			expected_return = -1;
			expected_errno = ENOENT;
			checkpoint_dir_path = CONFIG_BADPATH;
			break;
		}
	
		case 4:
		{
			expected_return = -1;
			expected_errno = ENOTDIR;
			checkpoint_dir_path = CONFIG_GOODFILE;
			break;
		}
	
		case 5:
		{
			expected_return = -1;
			expected_errno = EINVAL;
			checkpoint_dir_path = CONFIG_BAD_DATA_PATH;
			break;
		}
	
		default:
		{
			strcat(casedescript, 
				"default case reached, internal error.\n");
			++int_err;
			fprintf(stderr, "Invalid test case %d must be 1 - %d\n",
	 		testcase, ntests);
			return( -1 );
		}
	}

	actual_return = chkpnt_getprocinfo(checkpoint_dir_path, &id, &nproc, &pid_list, &ppid_list, &node_list);

	if( actual_return == expected_return ) {
		printf("PASSED %s retcode TEST %2d\n", myname, testcase);
	} else {
		fprintf(stderr,
		 "FAILED %s retcode TEST %2d: expected %d got %d\n\n",
		 myname, testcase, expected_return, actual_return);
		fflush(stderr);
	}

	if( errno == expected_errno ) {
		printf("PASSED %s global errno TEST %2d\n", myname, testcase);
	} else {
		fprintf(stderr,
		 "FAILED %s global errno TEST %2d: expected %d got %d\n",
		 myname, testcase, expected_errno, errno);
		fflush(stderr);
	}

	if (expected_return == -1)
		return(0);

	if (nproc != config_nproc) 
		goto out;
	if (id != config_id)
		goto out;
	if (!compare_array((int *)pid_list, (int *)config_pid_list))
		goto out;
	if (!compare_array((int *)ppid_list, (int *)config_ppid_list))
		goto out;
	if (!compare_array(node_list, config_node_list))
		goto out;

	printf("PASSED %s functaional TEST %2d\n", myname, testcase);
	goto out2;
out:
	fprintf(stderr, "FAILED %s global functional TEST %2d\n", myname, testcase);
out2:	
	free(pid_list);
	free(ppid_list);
	free(node_list);
	return(0);		/* no internal error */
}

void
print_pid_array(int pid_array[])
{
	int i;
	for (i=0; i<config_nproc; i++) 
		printf("pid_array[%d] = %d.\n", i, pid_array[i]);
}

int
compare_array(int array1[], int array2[])
{
	int count;
	for (count=0; count<config_nproc; count++) {
		if (array1[count] != array2[count])
			return(0);
	}
	return(1);
}
