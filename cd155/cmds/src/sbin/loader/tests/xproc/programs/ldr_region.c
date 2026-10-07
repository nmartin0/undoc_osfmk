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
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <loader.h>

char *program;

extern void cmd_info(pid_t);
extern void dump_minfo(ldr_module_info_t *);
extern void dump_rinfo(ldr_module_t, ldr_region_info_t *);

main(argc, argv)
	char *argv[];
{
	pid_t         pid;
	ldr_process_t process;
	int           rc;

	program = argv[0];

	if (argc < 2) {
		fprintf(stderr, "usage: %s <pid>\n", program);
		exit(1);
	}

	pid = (pid_t)atoi(argv[1]);

	cmd_info(pid);

	exit(0);
}
