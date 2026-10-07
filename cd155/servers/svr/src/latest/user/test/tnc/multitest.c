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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: multitest.c,v $
 * Revision 1.10  1995/02/02  00:58:19  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.9  1994/11/18  21:04:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/06/02  22:38:39  chrisp
 * Test program enhanced to call waitmulti() if -m option seen;
 * waitmulti(..,WNOWAIT) if -M option seen. Also -e <secs> specifies
 * lifetime of spinning children or -s <secs> specifies lifetime of
 * sleeping children.
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Modules: multitest.c
 *
 * Revision 1.7  1993/10/21  23:37:58  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.6  1993/07/14  18:54:44  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.5  1993/05/06  19:33:13  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:22:37  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/01/22  19:17:40  stefan
 * Merged Locus 01-20-93 code drop.
 *
 * Revision 1.3  1992/12/11  03:09:53  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  23:04:53  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:50:38  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 1.1.1.1  1993/05/03  17:59:10  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.4  93/09/16  09:34:23  chrisp
 * [SPE 0030] Generic Spanning Trees: add a -s <secs to sleep> option so that
 * 	remote rforkmulti()ed children linger before expiring ... this
 * 	gives time to signal the process group to test signal spanning
 * 	trees.
 * 
 * Revision 3.3  93/01/05  09:46:13  chrisp
 * Add "-n" option so that the parent doesn't wait for its rfork'ed children
 * 	to exit before it exits itself. This is a useful stress test, indeed.
 * 
 * Revision 3.2  92/11/12  14:19:05  chrisp
 * Wait for all children to exit before exiting the parent.
 * Print information for all array entries after rforkmulti() returns -
 * 	previously the full count was being clobbered by the returned
 * 	success count.
 * 
 * Revision 3.1  92/04/14  15:03:55  klh
 * Fix RCS Log.
 * 
 * $EndLog$
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <sys/errno.h>

int	aborted;

void sigalarm_trap(int sig, int code)
{
	aborted = TRUE;
}

main(int argc, char *argv[])
{
	extern char *optarg;
	extern int optind;
	char	ch;
	struct	itimerval timer;
	struct	sigvec sigalarm = {
                	(void(*))sigalarm_trap, 0, 0};
	int i, j, k;
	int count;
	int ret_count;
	int node_array[200], rval_array[200], pid_array[200];
	int ret;
	int no_wait = 0;
	int multi_wait = 0;
	int multi_nowait = 0;
	int sleeping = 0;
	int delay = 0;
	int this_pid = getpid();

	while ((ch = getopt(argc, argv, "e:Mmns:")) != EOF)
		switch (ch) {
		case 'e':
			delay = atoi(optarg);
			break;
		case 'M':
			multi_nowait++;
		case 'm':
			multi_wait++;
			break;
		case 'n':
			no_wait++;
			break;
		case 's':
			delay = atoi(optarg);
			sleeping++;
			break;
		}

	ret_count = count = argc-optind;
	if (count <= 0) {
		printf("No nodes specified\n");
		exit(0);
	}

	printf("[%d] List of nodes: ", this_pid);
	for (i = 0; i < count; optind++, i++) {
		node_array[i] = atoi(argv[optind]);
		printf("%d ", node_array[i]);
	}
	printf("\n");

	ret = rforkmulti(&ret_count, node_array, rval_array, pid_array);
	if (ret == 0) {
		this_pid = getpid();
		if (delay) {
			if (sigvec(SIGALRM, &sigalarm, NULL)) {	
				printf("sigvec gives errno=%d\n", errno);
				exit(1);
			}
			printf("[%d] Child %sing for %d secs\n",
				this_pid, sleeping ? "sleep" : "eat", delay);
			timer.it_interval.tv_sec = 0;
			timer.it_interval.tv_usec = 0;
			timer.it_value.tv_sec = delay;
			timer.it_value.tv_usec =0;
			aborted = FALSE;
			setitimer(ITIMER_REAL, &timer, 0);
			if (sleeping)
				pause();
			else {
				while (!aborted)
					;
			}
		}
		printf("[%d] Child exiting\n", this_pid);
		exit(0);
	}
	printf("[%d] Return values %d    count %d\n", this_pid, ret, ret_count);
	for (i = 0; i < count; i++)
		printf("\tnode:%4d\trval:%4d\tpid:%8d\n",node_array[i],
			rval_array[i],pid_array[i]);

	if (no_wait)
		exit();
	if (multi_wait) {
		int status_array[200];
		int wait_count = 0;
		ret = waitmulti(&wait_count,
				 &pid_array, &status_array, 0);
		printf("[%d] waiting for %d children...\n",
			this_pid, ret);
		do {
			wait_count = count;
			ret = waitmulti(&wait_count,
					&pid_array,
					&status_array,
					multi_nowait ? WNOWAIT : 0);
			if (ret > 0) {
				printf("[%d] waitmulti(%s) returns %d\n",
					this_pid,
					multi_nowait ? "WNOWAIT" : "",
					wait_count); 
				if (multi_nowait) {
					if (wait_count != count) {
						printf("[%d] sleeping...\n",
							this_pid);
						sleep(1);
					} else
						multi_nowait = 0;
					continue;
				} 
				for (i = 0; i < wait_count; i++)
					printf("\t[%d] status %d\n", 
						pid_array[i],
						status_array[i]);
			}
		} while (ret > 0 || errno != ECHILD);
	} else {
		do
			ret = wait(NULL);
		while (ret == 0 || errno != ECHILD);
	}
	printf("[%d] All children terminated\n", this_pid);
}
