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

main(argc, argv)
	char *argv[];
{
	int exec_loader_flags;

	auxv_init();
	if (auxv_get_exec_loader_flags(&exec_loader_flags) == -1) {
		(void)fprintf(stderr, "%s: auxv_get_exec_loader_flags() failed\n",
			argv[0]);
		exit(1);
	}
#ifdef	HEX
	(void)printf("0x%x\n", exec_loader_flags);
#else
	(void)printf("%d\n", exec_loader_flags);
#endif
	exit(0);
}
