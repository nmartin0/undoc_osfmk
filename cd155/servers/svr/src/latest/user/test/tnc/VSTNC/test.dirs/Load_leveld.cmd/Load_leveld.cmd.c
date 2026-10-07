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
 *
 * $Log: Load_leveld.cmd.c,v $
 * Revision 1.3  1994/11/18  21:05:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/02/04  01:41:13  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.6  93/07/12  17:23:13  yazz
 * Added log/endlog markers.
 * Added debug code.
 * Made test create only one copy of the load_level daemon; it had been
 * creating two entire instances.
 * Replace strcat() with strcpy() for the first string to be placed into
 * a buffer; previously we had merely hoped that the first byte of the
 * destination buffer (which is on the stack) would be null.
 * 
 * $EndLog$
 */
#include <errno.h>
#include <stdio.h>
#include <sys/signal.h>
#include <sys/table.h>
#include <sys/file.h>
#include "../common/vstnc.h"

/*
 * Number of tests.
 */
int ntests = 2;

#define	NUMHOGS		3

int int_err = FALSE;

char *myname;

/*
 * Don't specify path for load_leveld, use the PATH environment variable.
 */
char *load_leveld_cmd = { "load_leveld" };

char *my_key_dir = { "./key1" };
char *migrate_cmds = { "./migrate_commands" };
char *parameters = { "./parameters" };

/* local functions */
int chk_names();
int do_test(int);
void log_results(int, int, int);
int scan_proc_table(char *, int *, int);


/****************************************************************
 *
 * Module:	Load_leveld.cmd
 *
 * Purpose:	This module tests the TNC load_leveld command.
 *		The main program calls the test subroutines
 *		with the correct test cases and logs the results.
 *
 ****************************************************************/

main(
	int 	argc,
	char 	*argv[],
	char	*envp[])
{
	int testcase;		/* The test case number from argv[1] */
	int return_val;		
	int stat;

	myname = argv[0];

	/* Set the process group id to the process id of this process. */
	stat = setpgid(0, 0);
	if( stat != 0 ) {
		fprintf(stderr, "%s: Setpgid() call failed errno=%d\n",
			myname, errno);
		exit(1);
	}

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
	 * Check directory and file names.
	 */
	return_val = chk_names();
	if (return_val)
		exit(1);

	/*
	 * Execute the test specified, and log its results.
	 * Only ONE test case per run.
	 */
	return_val = do_test(testcase);

	/* Handle internal bugs if found. */
	if (int_err) {
		fprintf(stderr, "Internal error:  test case (%d).\n",testcase);
		exit(1);
	}

	/* Write the results to the log file, whatever they were. */
	log_results(0, testcase, return_val);

	exit(0);
}

/****************************************************************/
/*								*/
/* Function:	chk_names()					*/
/*								*/
/* Returns:	return 0 on pass, 1 on failure.			*/
/*								*/
/* Parameters:	none, global variables				*/
/*								*/
/* Author:	Brent G. Olsen					*/
/*								*/
/****************************************************************/

int
chk_names()
{
	int return_val = 0;
	int accessible;

	errno = 0;
	accessible = access(my_key_dir, F_OK);
	if (accessible == -1) {
		if (errno == ENOENT) {
			printf("Creating key directory(%s).\n", my_key_dir);
			mkdir(my_key_dir, 755);
		}
		else {
			fprintf(stderr, "Can't access key_dir(%s), errno:%d\n",
					my_key_dir, errno);
			return_val = 1;
		}
	}

	errno = 0;
	accessible = access(migrate_cmds, F_OK);
	if (accessible == -1) {
		fprintf(stderr, "Can't access migrate commands file(%s), "
		 		"errno:%d\n", migrate_cmds, errno);
		return_val = 1;
	}

	errno = 0;
	accessible = access(parameters, F_OK);
	if (accessible == -1) {
		fprintf(stderr, "Can't access parameters file(%s), "
		 		"errno:%d\n", parameters, errno);
		return_val = 1;
	}
	return(return_val);
}

/****************************************************************/
/*								*/
/* Function:	do_test()					*/
/*								*/
/* Returns:	None						*/
/*								*/
/* Parameters:	testcase, the test case number, valid between	*/
/*		1 and ntests.					*/
/* Author:	Bob Yasi					*/
/*								*/
/****************************************************************/

int
do_test(
	int	testcase)
{
	int	i, n, ret, ret_val, actual_errno;
	char	*hogprog = "forever_n";
	char	*hogprog2migrate = "forever_y";
#	define	SIZE 4096
	char	master_cmd_buf[SIZE];
	char	cmd_buf[SIZE];
	int	existing_ll_pid = -1;
	int	proc_pid, proc_was_found;
	/*
	 * First start the load_level daemon, and wait a few seconds
	 * for it to get established.  We start it with special test
	 * files and special test options so as not to conflict with
	 * the existing load_leveld.  We also send a signal to the
	 * existing load_leveld so that it will go dormant for a few
	 * minutes while this test runs.
	 */
	proc_was_found = scan_proc_table("load_leveld", &proc_pid, -1);
	if (proc_was_found) {
		existing_ll_pid = proc_pid; 
		errno = 0;
		ret = kill(existing_ll_pid, SIGUSR1);
		actual_errno = errno;
		if((ret != 0)  &&  (errno != ESRCH)) {
			++int_err;
			fprintf(stderr,
		 	"Can't send SIGUSR1 to load_leveld; errno=%d\n",
			actual_errno);
			return -1;
		}

		/*
		 * Sleep for 10 seconds to allow enough time for the already 
		 * running load_leveld to receive the necessary signals and go
		 * to sleep. 
		 */
		sleep(10);
	}

	/*
	 * First, start ONE *MIGRATABLE* copy of a cpu hog on goodnode.
	 * Sleep briefly so it can run up some CPU seconds -- processes
	 * that haven't run up much CPU are not candidates for migration.
	 */
	proc_pid = fork();
	if (proc_pid < 0) {
		++int_err;
		fprintf(stderr, "fork fail err=%d\n", errno);
		return -1;
	}
	if (proc_pid == 0) {
		ret = rexeclp(hogprog2migrate, hogprog2migrate, (char *)0,
	 		      config_goodnode);
		actual_errno = errno;
		if( ret < 0 ) {
			++int_err;
			fprintf(stderr, "rexeclp fail %s node=%d errno=%d",
		 		hogprog2migrate, config_goodnode, 
				actual_errno);
			return -1;
		}
	}

	sleep(5);


	/*
	 * Next we start SEVERAL *UNmigratable* copies of the forever
	 * loop program running on ALL nodes except our own.  We intend
	 * that our own node be the ONLY underloaded node.
	 */

	for( n = 0; n < config_nodecnt; ++n ) {
		if (config_mynode == config_nodelist[n])
			continue;
		for( i = 0; i < NUMHOGS; ++i ) {
			proc_pid = fork();
			if (proc_pid > 0)
				continue;
			if (proc_pid < 0) {
				++int_err;
				fprintf(stderr, "fork fail err=%d\n", errno);
				return -1;
			}
			ret = rexeclp(hogprog, hogprog, (char *)0, 
				      config_nodelist[n]);
			if( ret < 0 ) {
				++int_err;
				fprintf(stderr, "rexeclp fail err=%d\n",
					errno);
				return -1;
			}
		}
	}

	/*
	 * Now start up some EXTRA *UNmigratable* copies of the forever loop
	 * program running on "goodnode".  "goodnode" is a node different
	 * from the one we're running on.  This is done to assure that 
	 * "goodnode" is our targeted overloaded node.
	 */
	for( i = 0; i < NUMHOGS; ++i ) {
		proc_pid = fork();
		if (proc_pid > 0)
			continue;
		if (proc_pid < 0) {
			++int_err;
			fprintf(stderr, "fork fail err=%d\n", errno);
			return -1;
		}
		ret = rexeclp(hogprog, hogprog, (char *)0, config_goodnode);
		if( ret < 0 ) {
			++int_err;
			fprintf(stderr, "rexeclp fail err=%d\n", errno);
			return -1;
		}
	}

	switch(testcase) {

		case 1:
			strcpy(cmd_buf, load_leveld_cmd);
			break;

		case 2:
			strcpy(cmd_buf, load_leveld_cmd);
			strcat(cmd_buf, " -n");
			break;

		default:
			++int_err;
			fprintf(stderr, 
				"Invalid test case %d must be 1 - %d\n",
		 		testcase, ntests);
			return -1;
	}
	strcpy(master_cmd_buf, "TZ=");
	strcat(master_cmd_buf, config_TZ);
	strcat(master_cmd_buf, "; PATH=");
	strcat(master_cmd_buf, config_PATH);
	strcat(master_cmd_buf, "; export TZ PATH; ");
	strcat(master_cmd_buf, cmd_buf);
	strcat(master_cmd_buf, " -I 1");
	strcat(master_cmd_buf, " -K ");
	strcat(master_cmd_buf, my_key_dir);
	strcat(master_cmd_buf, " -c ");
	strcat(master_cmd_buf, migrate_cmds);
	strcat(master_cmd_buf, " -p ");
	strcat(master_cmd_buf, parameters);
	strcat(master_cmd_buf, " &");

	printf("Full command is: %s\n", master_cmd_buf);

	/*
	 * Next start our own copy of the load leveler.  In the normal
	 * case we expect it to migrate the "forever_y" process from
	 * the very overloaded node "goodnode" to our own node, which
	 * we've kept underloaded.  (In the "-n" testcase, we expect
	 * that the "forever_y" process will NOT migrate to our node.)
	 */



	ret = system(master_cmd_buf);	/* start daemon up */



	if (ret == 127) {
		++int_err;
		fprintf(stderr, "failed to invoke 'system' function err=%d\n",
			errno);
		return -1;
	}

	/*
	 * Sleep for 1 minute to give the load leveler daemon time to init
	 * itself and then to migrate the "forever_y" process to our node,
	 * which is the underloaded node.
	 *
	 * If the process hasn't migrated in 2 minutes we conclude it isn't
	 * going to.  (To move the test along we check before the whole time
	 * interval has expired.)
	 */

#ifdef DEBUG
	printf("\nBEFORE SLEEPING:");
	printf("\nps lawwx -- systemwide"); (void)system("ps lawwx");
	printf("\nvs on node 0:\n"); (void)system("onnode 0 vs");
	printf("\nvs on node 1:\n"); (void)system("onnode 1 vs");
	printf("\nvs on node 3:\n"); (void)system("onnode 3 vs");
#endif

	sleep(1*60);

#ifdef DEBUG
	printf("\AFTER SLEEPING 1 MINUTE:");
	printf("\nvs on node 0:\n"); (void)system("onnode 0 vs");
	printf("\nvs on node 1:\n"); (void)system("onnode 1 vs");
	printf("\nvs on node 3:\n"); (void)system("onnode 3 vs");
#endif

	proc_was_found = scan_proc_table(hogprog2migrate, &proc_pid, -1);
	if (!proc_was_found) {

		/*
		 * Allow an additional minute for the process to migrate.
		 */
		sleep(1*60);
#ifdef DEBUG
	printf("\AFTER SLEEPING 2 MINUTES:");
	printf("\nvs on node 0:\n"); (void)system("onnode 0 vs");
	printf("\nvs on node 1:\n"); (void)system("onnode 1 vs");
	printf("\nvs on node 3:\n"); (void)system("onnode 3 vs");
#endif


		proc_was_found = scan_proc_table(hogprog2migrate,
						 &proc_pid, -1);
	}

	switch(testcase) {

		case 1:
			if (proc_was_found)
				ret_val = 0;
			else
				ret_val = -1;
			break;

		case 2:
			if (proc_was_found)
				ret_val = -1;
			else
				ret_val = 0;
			break;

		default:
			++int_err;
			fprintf(stderr, 
				"Invalid test case %d must be 1 - %d\n",
				testcase, ntests);
			ret_val = -1;
			break;
	}

	/* kill the load_leveld which was invoked for the test */
	proc_was_found = scan_proc_table("load_leveld", &proc_pid, 
					 existing_ll_pid);
	if (proc_was_found) {
		errno = 0;
		ret = kill(proc_pid, SIGTERM);
		actual_errno = errno;
		if ((ret != 0) && (errno != ESRCH)) {
			++int_err;
			fprintf(stderr,
		 	"Can't send SIGTERM to load_leveld; errno=%d\n",
			actual_errno);
			return -1;
		}
	}

	/*
	 * Kill off all those forever procs we created.
	 */
	signal(SIGTERM, SIG_IGN);	/* ignore sig we are about to send */
	kill(0, SIGTERM);		/* clean up our whole process grp */

	return (ret_val);		/* so we terminate normally */
}

/*******************************************************************************

Function:	log_results

Returns:	void

Parameters:	positive, a flag the do_test function clears iff the test 
		case is a negative one.

		test_case, the test case number, valid between 1 and ntests.

		return_val, the values do_test got from either the
		local test or the romote test.

Author:		Mike Barnett

Purpose:	This function reports tests results from the sub-tests.

Modification History:

Date		Reason
--------	------

06/18/92	Began coding.

*******************************************************************************/

void 
log_results(
	int	positive,
	int	test_case, 
	int	ret_val)
{

	/*
	 * Check the results with their expected value.
	 */
	if (positive == ret_val)
		printf("PASSED load_leveld.cmd TEST %2d\n", test_case);
	else {
		fprintf(stderr, "FAILED Load_leveld.cmd TEST %2d\n",
		 test_case);
	}
}

/*******************************************************************************

Function:	scan_proc_table

Returns:	1 if specified command is an active process 
		0 if specified command is not an active process

Parameters:	cmd_name, the short command name of the process to scan for.
		pid_ptr, pointer to a variable which is to contain the pid
		         of the process if found in the proc info table.
		exclude_pid, exclude any process found during scan if pid of
			     process = exclude_pid

Author:		Mike Barnett

Purpose:	This function searches the proc info table for the passed in
		command name.

Modification History:

Date		Reason
--------	------

06/18/92	Began coding.

*******************************************************************************/

int
scan_proc_table(
	char	*cmd_name,
	int	*pid_ptr,
	int	exclude_pid)
{
	int nproc, i, found, ret;
	struct tbl_procinfo pi;

	nproc = table_node(node_self(), TBL_PROCINFO, 0, NULL, 32767, 0);
	if (nproc == -1)
		nproc = 0;
	found = FALSE;
	for (i = 0; (i < nproc) && (!found); i++) {
		(void)table_node(node_self(), TBL_PROCINFO, i, (char *)&pi, 1, 
			    sizeof(pi));
		if (pi.pi_status != PI_ACTIVE)
	        	continue;
		if ((strcmp(cmd_name, pi.pi_comm) == 0) &&
		 (exclude_pid != pi.pi_pid)) {
			*pid_ptr = pi.pi_pid;
	        	found = TRUE;
		}

#ifdef DEBUG
		printf("node=%2d pid=%5d  cmd=%s\n",
		 node_self(), pi.pi_pid, pi.pi_comm);
#endif

	}
	return (found);
}
