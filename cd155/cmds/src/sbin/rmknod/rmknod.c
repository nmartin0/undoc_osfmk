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
 * $Id: rmknod.c,v 2.5 1994/11/21 17:54:50 mtm Exp $
 *
 * HISTORY
 * $Log: rmknod.c,v $
 * Revision 2.5  1994/11/21  17:54:50  mtm
 * Copyright additions/changes
 *
 * Revision 2.4  1994/11/21  17:49:43  bradf
 * Getting rid of RCS lock
 *
 * Revision 2.3  1992/05/11  14:40:09  stans
 * code clean up, syscall is now in syscall.h & libc.a
 *
 * Revision 2.2  92/02/10  12:37:57  stans
 * initial checkin
 * 
 * Revision 2.2  91/11/27  11:45:56  rabii
 * 	Initial Check-in
 * 
 *
 * just like mknod except it stores a node number in the inode.
 *
 *	rmknod name [ c ] [ b ] major minor node
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

static	char *rmknod_id[]="$Id: rmknod.c,v 2.5 1994/11/21 17:54:50 mtm Exp $";

main(argc, argv)
	int argc;
	char **argv;
{
	extern int	errno;
	unsigned	maj, min, nodeAdrs;
	u_short		mode;
	char		*strerror();

	if (argc != 6) {
		(void)fprintf(stderr,
		    "usage: rmknod name [b | c] major minor node\n");
		exit(1);
	}

	mode = 0666;

	if (argv[2][0] == 'c')
		mode |= S_IFCHR;
	else if (argv[2][0] == 'b')
		mode |= S_IFBLK;
	else {
		(void)fprintf(stderr,
		    "rmknod: node must be type 'b' or 'c'.\n");
		exit(1);
	}

	maj = (unsigned)atoi(argv[3]);
	min = (unsigned)atoi(argv[4]);
	nodeAdrs = atoi(argv[5]);

	if (rmknod(argv[1], mode, makedev( maj, min ), nodeAdrs) < 0) {
		(void)fprintf(stderr,
		    "rmknod: %s: %s\n", argv[1], strerror(errno));
		exit(errno);
	}
	exit(0);
}
