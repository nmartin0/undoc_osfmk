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
 * HISTORY
 * $Log: rmknod.c,v $
 * Revision 1.5  1994/11/18  20:53:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:50:13  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.3  1993/05/06  19:27:46  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:15:38  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  23:00:49  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:49:04  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:01:07  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:57:22  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.4  1992/04/07  13:48:14  pjg
 * 	Changed rmknod syscall number to 266
 *
 * Revision 2.3  92/03/01  18:53:32  pjg
 * 	Change the number of the system call to 263.
 * 
 * Revision 2.2  91/11/27  11:45:56  rabii
 * 	Initial Check-in
 * 
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#define RMKNOD 266
#define rmknod(a,b,c,d)	syscall(RMKNOD, a, b, c, d)

main(argc, argv)
	int argc;
	char **argv;
{
	extern int errno;
	u_short mode;
	char *strerror();

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
		    "mknod: node must be type 'b' or 'c'.\n");
		exit(1);
	}

	if (rmknod(argv[1], mode, makedev(atoi(argv[3]), atoi(argv[4])), atoi(argv[5])) < 0) {
		(void)fprintf(stderr,
		    "mknod: %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	exit(0);
}
