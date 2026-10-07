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

extern int _ldr_crt0_request;
static int *_lcrp = &_ldr_crt0_request;
int _ldr_present = 1;

void auxv_init();
char *auxv_get_exec_loader_filename();

main(argc, argv)
	char *argv[];
{
	char *exec_loader_filename;

	auxv_init();
	if ((exec_loader_filename = auxv_get_exec_loader_filename()) == (char *)-1) {
		(void)fprintf(stderr, "%s: auxv_get_exec_loader_filename() failed\n",
			argv[0]);
		exit(1);
	}
	(void)printf("%s\n", exec_loader_filename);
	exit(0);
}
