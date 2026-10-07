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
 *	Copyright (c) Locus Computing, 1991-92
 * 	This is UNPUBLISHED source code that is
 * 	the property of Locus Computing, containing
 *	proprietary secrets of LCC.  Any disclosure
 *	is strictly prohibited.  Locus makes no warantee,
 *	explicit or implicit, on the functionality of this code.
 */
/*
 * HISTORY
 * $Log: bunny.c,v $
 * Revision 1.5  1994/11/18  20:57:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/03/14  17:50:18  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 1.3  1993/07/14  18:54:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.2  1992/11/30  23:04:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.1.2  1993/07/01  21:21:47  cfj
 * Adding new code from vendor
 *
 * Revision 1.1.2.1  1992/11/06  00:15:02  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:06:05  cfj
 * Bump major revision number.
 *
 * Revision 3.0  1992/07/21  12:06:24  chrisp
 * First appearance
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/table.h>
#include <sys/time.h>
#include <time.h>

#define SIGALRM		14
#define SIGMIGRATE	32
#define FALSE		0
#define TRUE		1
#define MAX_NODE_LIST 100

extern void exit(int);
extern int node_self();

extern int errno;

int	aborted;
int	this_pid;
int	i, loop;
int	node_list[MAX_NODE_LIST];
int	migrate_in_handler = FALSE;
int	ignore_sigmigrate = FALSE;
char	*our_name;

void timestamp()
{
	time_t		time_secs;
	char		timestring[20];

	time(&time_secs);
	strftime(timestring, sizeof(timestring), "%T", localtime(&time_secs));
	printf("%s [%d.%d] %s: ", timestring, node_self(), this_pid, our_name);
}

void migrate_trap(int sig, int code)
{
	int	error;
	timestamp();
	printf("SIGMIGRATE, code %d ... ", code);
	if (ignore_sigmigrate)
		printf("ignoring\n");
	else {
		error = migrate(code);
		if (error < 0)
			printf("%s() failed with errno %d\n",
				(code<0) ? "checkpoint" : "migrate", errno);
		else if (error > 0)
			printf("restarted from migrate(%d)\n", -error);
		else
			printf("%s\n", (code<0) ? "checkpointed" : "migrated");
	}
}

void sigalarm_trap(int sig, int code)
{
	if (migrate_in_handler) {
		if (migrate(node_list[i]))
			printf("migrate() error %d\n", errno);
	}
	aborted = TRUE;
}

main(int argc, char *argv[])
{
	extern char *optarg;
	extern int optind;
	char	ch;
	int	node_list_len = 0;
	int	loop_count = 1;
	int	delaying = FALSE;
	int	sleeping = FALSE;
	int	randomize_nodes = FALSE;
	int	randomize_delay = FALSE;
	int	random_seed;
	int	delay_time = 0;
	int 	numnodes;
	struct	itimerval timer;
	int	error = 0;
	struct	sigvec sigalarm = {
                	(void(*))sigalarm_trap, 0, 0};
        struct	sigvec sigmig = {
                	(void(*))migrate_trap, 0, 0};

	our_name = argv[0];
	this_pid = getpid();
	random_seed = this_pid;

	while ((ch = getopt(argc, argv, "E:e:hN:n:qr:S:s:Vv")) != EOF)
		switch (ch) {
		case 'e':
			randomize_delay = TRUE;
		case 'E':
			if (delaying)
				error++;
			delay_time = atoi(optarg);
			delaying = TRUE;
			sleeping = FALSE;
			break;
		case 'h':
			migrate_in_handler = TRUE;
			break;
		case 'N':
			randomize_nodes = TRUE;
			if ((node_list_len = atoi(optarg)) >= MAX_NODE_LIST)
				error++;
			break;
		case 'n':
			loop_count = atoi(optarg);
			break;
		case 'r':
			random_seed = atoi(optarg);
			break;
		case 's':
			randomize_delay = TRUE;
		case 'S':
			if (delaying)
				error++;
			delay_time = atoi(optarg);
			delaying = TRUE;
			sleeping = TRUE;
			break;
		case 'V':
			ignore_sigmigrate = TRUE;
		case 'v':
			if (sigvec(SIGMIGRATE, &sigmig, NULL)) {
				printf("sigvec gives errno=%d\n", errno);
				exit(1);
			}
			break;
		case '?':
		default:
			error++;
		}
	srand(random_seed);
	if (optind < argc) {
		if (randomize_nodes) {
			int max_node = atoi(argv[optind]);

			numnodes = table(TBL_NODEINFO, 0, (char *)0, 32767, 0);
			printf("table returned %d for numnodes\n", numnodes);

			if (max_node < 0 || max_node > numnodes) {
				printf("node must be between 0 and %d\n", 
					numnodes);
				error++;
			} else {
				max_node++;
				for (i=0; i < node_list_len; i++)
					node_list[i] = rand() % max_node;
			}
		} else {
			for (;optind < argc; optind++) {
				int node = atoi(argv[optind]);
				numnodes = table(TBL_NODEINFO, 0, (char *)0, 32767, 0);
				if (node < 0 || node > numnodes) {
					printf("node must be between 0 and %d\n",
						numnodes);
					error++;
				}
				node_list[node_list_len++] = node;
			}
		}
	} else
		error++;
	if (randomize_delay && !delaying)
		error++;
	if (error) {
		printf("usage: %s [-n count] node [node ...]\n"
		       "             [-N len] generates a random node list\n"
		       "             [-E|-S secs] eat or sleep delay time\n"
		       "             [-s|-s secs] eat or sleep random time\n"
		       "             [-h] calls migrate() from signal catcher\n"
		       "             [-v] sets a SIGMIGRATE signal handler\n"
			,our_name);
		exit(1);
	}

	timestamp();
	printf("hopping to %snode%s",
		randomize_nodes ? "(randomly generated) " : "",
		node_list_len == 1 ? "" : "s");
	for (i = 0; i < node_list_len; i++ )
		printf(" %d", node_list[i] );
	if (loop_count == 1)
		printf(", once\n");
	else
		printf(", %d times\n", loop_count);
	if (delaying) {
		printf("\t\t%s for %s%d seconds on each node\n",
			sleeping? "sleeping" : "eating",
			randomize_delay? "a random time < " : "", delay_time);
		if (migrate_in_handler)
			printf("\t\tmigrate() called from timer handler\n");
		if (sigvec(SIGALRM, &sigalarm, NULL)) {
			printf("sigvec gives errno=%d\n", errno);
			exit(1);
		}
	}
	
	for (loop = 1; loop <= loop_count; loop++) {
		for (i = 0; i < node_list_len; i++) {
			if (delaying) {
				int this_delay_time = randomize_delay ?
					(rand() % delay_time) + 1 : delay_time;
				timestamp();
				printf("%s on node %d for %d secs...\n",
					sleeping ? "sleeping" : "eating",
					node_self(), this_delay_time);
				timer.it_interval.tv_sec = 0;
				timer.it_interval.tv_usec = 0;
				timer.it_value.tv_sec = this_delay_time;
				timer.it_value.tv_usec =0;
				aborted = FALSE;
				setitimer(ITIMER_REAL, &timer, 0);
				if (sleeping)
					pause();
				else {
					while (!aborted)
						;
				}
			} else {
				timestamp();
				printf("visiting node %d...\n", node_self());
			}
			if (!migrate_in_handler) {
				if (migrate(node_list[i]))
					printf("migrate() error %d\n", errno);
			}
		}
	}
	timestamp();
	printf("done on node %d\n", node_self());
}
