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
 * $Log: frog.c,v $
 * Revision 1.4  1994/11/18  20:57:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:54:18  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.2  1992/11/30  23:04:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.1.2  1993/07/01  21:21:57  cfj
 * Adding new code from vendor
 *
 * Revision 1.1.2.1  1992/11/06  00:15:04  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:06:09  cfj
 * Bump major revision number.
 *
 * Revision 3.0  1992/07/21  12:06:35  chrisp
 * First appearance
 *
 */
#include <sys/types.h>
#include <stdio.h>

#define MAX_NODE_LIST 100

extern void exit(int);
extern int node_self();

extern int errno;

int this_pid;
char *our_name;

void timestamp()
{
	time_t		time_secs;
	char		timestring[20];

	time(&time_secs);
	strftime(timestring, sizeof(timestring), "%T", localtime(&time_secs));
	printf("%s [%d.%d] %s: ", timestring, node_self(), this_pid, our_name);
}


main(int argc, char *argv[])
{
	extern char *optarg;
	extern int optind;
	char	ch;
	int	node_list[MAX_NODE_LIST];
	int	node_list_len = 0;
	int	loop_count = 1;
	int	error = 0;
	int	i, loop;

	our_name = argv[0];
	this_pid = getpid();

	while ((ch = getopt(argc, argv, "n:")) != EOF)
		switch (ch) {
		case 'n':
			loop_count = atoi(optarg);
			break;
		case '?':
		default:
			error++;
		}
	if (optind < argc) {
		int node;
		for (;optind < argc; optind++) {
			node = atoi(argv[optind]);
			if (node < 0 || node > 7) {
				printf("node %d - must be >0 and <8\n", node);
				error++;
			}
			node_list[node_list_len++] = node;
		}
	} else
		error++;
	if (error) {
		printf("usage: frog [-n count] node [node ...]\n");
		exit(1);
	}

	timestamp();
	printf("hopping to node%s", node_list_len==1? "" : "s");
	for (i = 0; i < node_list_len; i++ )
		printf(" %d", node_list[i] );
	if (loop_count == 1)
		printf(", once\n");
	else
		printf(", %d times\n", loop_count);

	for (loop = 1; loop <= loop_count; loop++) {
		for (i = 0; i < node_list_len; i++) {
			timestamp();
			printf("%d/%d off to %d\n",
				loop, loop_count,
				node_list[i]);
			migrate(node_list[i]);
			if (errno)
				printf("migrate() error %d\n", errno);
		}
	}
}
