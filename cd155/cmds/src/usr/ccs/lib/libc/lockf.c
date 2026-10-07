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
static char rcsid[] = "@(#)$RCSfile: lockf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:56 $";
#endif

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

lockf(fildes, function, size)
long size;
int fildes, function;
{
	struct flock l;
	int rv;

	l.l_whence = 1;
	if (size < 0) {
		l.l_start = size;
		l.l_len = -size;
	} else {
		l.l_start = 0L;
		l.l_len = size;
	}
	switch (function) {
	case F_ULOCK:
		l.l_type = F_UNLCK;
		rv = fcntl(fildes, F_SETLK, &l);
		break;
	case F_LOCK:
		l.l_type = F_WRLCK;
		rv = fcntl(fildes, F_SETLKW, &l);
		break;
	case F_TLOCK:
		l.l_type = F_WRLCK;
		rv = fcntl(fildes, F_SETLK, &l);
		break;
	case F_TEST:
		l.l_type = F_WRLCK;
		rv = fcntl(fildes, F_GETLK, &l);
		if (rv != -1) {
			if (l.l_type == F_UNLCK)
				return (0);
			else {
				errno = EACCES;
				return (-1);
			}
		}
		break;
	default:
		errno = EINVAL;
		return (-1);
	}
	if (rv < 0) {
		switch(errno) {
		case EMFILE:
		case ENOSPC:
			/* A deadlock error is given if we run out of resources,
			 * in compliance with /usr/group standards.
			 */
			errno = EDEADLK;
			break;
		default:
			break;
		}
	}
	return (rv);
}
