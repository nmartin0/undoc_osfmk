/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Output the device service node number. Node is located in the device's inode.
 * This node number is NOT where the device is physically attached but the node
 * where the server provides high-level device service. High-level imples 
 * functions like ufs fileservice, tcp/ip processing, etc.
 *
 *	akp [ Wed Mar 31 09:39:53 PST 1993 ]
 * HISTORY:
 * $Log: devstat.c,v $
 * Revision 1.4  1994/11/21  16:41:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/10/23  21:12:28  stans
 *    Added "-v" switch to output device name and node #.
 *
 */

#define OSF1_ADFS	1
#include <sys/stat.h>
#include <stdio.h>
#include <mach/boolean.h>

boolean_t	verbose=FALSE;

main(argc, argv)
int	argc;
char	**argv;
{
	struct devstat d;
	int	cc, errs=0;

	argc--;
	argv++;

	if ( strcmp(argv[0],"-v") == 0 ) {
		verbose = TRUE;
		argc--;
		argv++;
	}

	while (argc--) {
		if ((cc = devstat(*argv, &d)) < 0) {
			perror(*argv);
			errs++;
		}
		if ( verbose )
			(void) printf("%s @ node %d\n", *argv, d.dst_node);
		else
			(void) printf("%d\n", d.dst_node);
		argv++;
	}

	exit(errs != 0);
}
