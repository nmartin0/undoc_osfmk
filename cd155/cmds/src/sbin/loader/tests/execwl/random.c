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

#include <sys/ldr_exec.h>

main()
{
	int value;

	srandom((int)time((long *)0));
	value = random() & LDR_EXEC_USER_MASK;
#ifdef	HEX
	(void)printf("0x%x\n", value);
#else
	(void)printf("%d\n", value);
#endif
	exit(0);
}
