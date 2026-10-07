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
 * $Log: node_self.c,v $
 * Revision 1.4  1994/11/18  20:57:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:54:21  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.2  1992/11/30  23:04:37  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.1.2  1993/07/01  21:22:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.1.2.1  1992/11/06  00:15:06  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:06:12  cfj
 * Bump major revision number.
 *
 * Revision 3.0  1992/07/21  12:06:44  chrisp
 * First time out
 *
 */
#include <sys/types.h>
#include <stdio.h>

extern void exit(int);
extern int node_self();

extern int errno;

main(int argc, char *argv[])
{
	extern char *optarg;
	extern int optind;
	char	ch;
	int	node = node_self();
	int	error = 0;

	while ((ch = getopt(argc, argv, "o")) != EOF)
		switch (ch) {
		case 'o':
			node = getpid() >> 16;
			break;
		case '?':
		default:
			error++;
		}
	if (error) {
		printf("usage: node_self [-o]\n");
		exit(1);
	}

	printf( "%d\n", node );
	exit(0);

}
