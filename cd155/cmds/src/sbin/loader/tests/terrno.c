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
/* terrno.c
 * Test loader errno printing
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <loader.h>

#include "ldr_types.h"
#include "ldr_malloc.h"
#include "ldr_sys_int.h"
#include "ldr_errno.h"


main(argc, argv)
int argc;
char **argv;
{
	char	ch = 'Z';

	ldr_heap_init();

	ldr_log("This is the log message, %s, %d, 0x%x, %c %c\n",
		"a string", 69, 0x69, (char)'Q', ch);

	ldr_msg("This should appear on %s\nldr_msg status %E\n%B\n",
		  "/dev/tty", LDR_EEXIST);

	ldr_msg("Test of bad errnos\nZero: %E\nPositive: %E\nToo small: %E\n",
		  0, 10, -200);

	ldr_log("Some funny numbers: 0x%0x%20d %020d\n",
		69, 696969, 69696969);

	ldr_msg("Printing funny numbers from the log\n%B\n");

	exit(0);
}
