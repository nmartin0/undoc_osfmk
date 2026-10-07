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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ptrace.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:37:09 $";
#endif

/*
 * FUNCTIONS: ptrace
 *
 * DESCRIPTION:
 *	SVID2 compliant ptrace()
 */

#include	<sys/syscall.h>
#include	<errno.h>


int
ptrace(int req, int pid, int *addr, int data)
{
	int	ret_val;
	int	old_err = errno;

	/* need to detect errors with errno since -1 is a valid return */
	errno = 0;
	if ((ret_val = syscall(SYS_ptrace, req, pid, addr, data)) == -1 &&
			errno)
		return (-1);	/* a real error, errno is set */

	/* no error, restore errno */
	errno = old_err;
	switch (req) {
	case 4	:	/* WRITE_I */
			/* FALLTHROUGH */
	case 5	:	/* WRITE_D */
		/* on success return the data written */
		return (data);
	case 7	:	/* CONTINUE */
		/* on success the value of data is returned */
		return (data);
	default	:	/* other requests behave as expected */
		;
	}
	return (ret_val);
}
