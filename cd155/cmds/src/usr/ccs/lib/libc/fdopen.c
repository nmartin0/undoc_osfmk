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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: fdopen.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:39 $";
#endif
/*
 * FUNCTIONS: fdopen 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * fdopen.c	1.12  com/lib/c/io,3.1,9013 2/27/90 17:20:00
 */

/*LINTLIBRARY*/
/*
 * Unix routine to do an "fopen" on file descriptor
 * The mode has to be repeated because you can't query its
 * status
 */
/* The fdopen() function includes all the POSIX requirements */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <fcntl.h>
#include <sys/errno.h>

extern long lseek();
extern FILE *_findiop();

/*                                                                    
 * FUNCTION: routine to do an "fopen" on file descriptor
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	     - FILE * on success
 *	     - NULL on failure
 */

FILE *
fdopen(int fd, char *mode)
{
	register FILE *iop;
	int status_file;
#ifdef	_THREAD_SAFE
	register filelock_t filelock;
#endif

	if ( (status_file = fcntl(fd, F_GETFL) ) == -1)
		return(NULL);
	status_file &= O_ACCMODE;

	if((iop = _findiop()) == NULL)
		return(NULL);

#ifdef	_THREAD_SAFE
	filelock = _flockfile(iop);
#endif
	iop->_cnt = 0;
	iop->_flag = 0;
	iop->_file = fd;
	iop->_base = iop->_ptr = NULL;

	if (mode[1] == '+' || (mode[1] && mode[2] == '+')) {
		if (status_file == O_RDWR) {
			iop->_flag &= ~(_IOREAD | _IOWRT);
			iop->_flag |= _IORW;
		}
		else {
#ifdef	_THREAD_SAFE
			seterrno(EINVAL);
#else
			errno = EINVAL;
#endif
			iop = NULL;
		}
#ifdef _THREAD_SAFE
		_funlockfile(filelock);
#endif
		if (*mode == 'a') {
			(void) lseek(fd, 0L, 2);
		}

		return(iop);
        }

	switch(*mode) {

		case 'r':
			if (!( (status_file == O_RDONLY) || (status_file == O_RDWR) ) ) {
				iop->_flag = 0;
#ifdef	_THREAD_SAFE
				seterrno(EINVAL);
				_funlockfile(filelock);
#else
				errno = EINVAL;
#endif
				return(NULL);
			}
			iop->_flag |= _IOREAD;
			break;
		case 'a':
			if (!( (status_file == O_WRONLY) || (status_file == O_RDWR) ) ) {
				iop->_flag = 0;
#ifdef	_THREAD_SAFE
				seterrno(EINVAL);
				_funlockfile(filelock);
#else
				errno = EINVAL;
#endif
				return(NULL);
			}
			(void) lseek(fd, 0L, 2);
			/* No break */
		case 'w':
			if (!( (status_file == O_WRONLY) || (status_file == O_RDWR) ) ){
				iop->_flag = 0;
#ifdef	_THREAD_SAFE
				seterrno(EINVAL);
				_funlockfile(filelock);
#else
				errno = EINVAL;
#endif
				return(NULL);
			}
			iop->_flag |= _IOWRT;
			break;
		default:
			iop->_flag = 0;
#ifdef	_THREAD_SAFE
			_funlockfile(filelock);
#endif
			return(NULL);
	}

#ifdef	_THREAD_SAFE
	_funlockfile(filelock);
#endif
	return(iop);
}
