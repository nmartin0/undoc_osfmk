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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

main(argc, argv)
	char *argv[];
{
	struct stat buf;

	if (argc < 2) {
		(void)fprintf(stderr, "usage: %s <file-name>\n", argv[0]);
		exit(1);
	}
	if (stat(argv[1], &buf) == -1) {
		(void)fprintf(stderr, "%s: stat(2) failed on file \"%s\": ",
			argv[0], argv[1]);
		perror((char *)0);
		exit(2);
	}
#ifdef	SETUID
	if (buf.st_mode & S_ISUID)
		(void)printf("%d\n", buf.st_uid);
	else {
		(void)fprintf(stderr, "%s: file \"%s\" is not set uid\n",
			argv[0], argv[1]);
		exit(3);
	}
#else
	if (buf.st_mode & S_ISGID)
		(void)printf("%d\n", buf.st_gid);
	else {
		(void)fprintf(stderr, "%s: file \"%s\" is not set gid\n",
			argv[0], argv[1]);
		exit(3);
	}
#endif
	exit(0);
}
