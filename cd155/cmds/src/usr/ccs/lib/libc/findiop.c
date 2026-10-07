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
static char rcsid[] = "@(#)$RCSfile: findiop.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:50 $";
#endif
/*
 * FUNCTIONS: _findiop 
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
 * findiop.c	1.8  com/lib/c/io,3.1,8943 10/18/89 10:52:42
 */

#include <stdio.h>
#include <malloc.h>
#include <memory.h>

#ifdef _THREAD_SAFE
#include "stdio_lock.h"
#include "rec_mutex.h"
#include <errno.h>

extern struct rec_mutex	_findiop_rmutex;

#define inuse(iop)	((iop)->_flag & (_IOINUSE|_IOREAD|_IOWRT|_IORW))
#endif /* _THREAD_SAFE */

extern FILE *_iobptr[];

FILE *
_findiop()
{
	register FILE *iop, *_lastbuf;
	register int i, size;
	int nrws = _NRWS;
#ifdef _THREAD_SAFE
	register filelock_t filelock;

	rec_mutex_lock(&_findiop_rmutex);
#endif /* _THREAD_SAFE */

	for (i=0; i < nrws && _iobptr[i] != 0  ;  i++ ) {   
		_lastbuf = _iobptr[i] + _NIOBRW;
		for (iop = _iobptr[i]; iop < _lastbuf; iop++)
#ifdef _THREAD_SAFE
		{
			if (!inuse(iop)) {
				/* make sure next thread through doesn't use
				 * this iob.   Check the file lock. If null,
				 * allocate it.  If it's non-null, testlock().  
				 * If we get it, it's ours.
				 */
				 if (iop->_lock == NULL) {
					(void *)rec_mutex_alloc(&iop->_lock);
				  }
				  if (filelock = _ftestfile(iop)) {	
					  if (!inuse(iop)) { /* found one */
						  iop->_flag = _IOINUSE;
						  rec_mutex_unlock(&_findiop_rmutex);
						  _funlockfile(filelock);
						  return (iop);
					  } else { /* not free, give it back */
						  _funlockfile(filelock);
					  }
				  }	/* lock failed, must be in use */
			  }
		}
#else	/* _THREAD_SAFE */
			if ( (iop->_flag & (_IOREAD | _IOWRT | _IORW)) == 0 )
				return(iop);
#endif	/* _THREAD_SAFE */
	}

#ifdef _THREAD_SAFE
	if ( i < nrws && 
	    (_iobptr[i] = (FILE*)malloc(size = sizeof(FILE)*_NIOBRW))) {
		memset((void *)_iobptr[i], (int)NULL, (size_t)size);
		_iobptr[i]->_flag = _IOINUSE;
		(void *)rec_mutex_alloc(&_iobptr[i]->_lock);
		rec_mutex_unlock(&_findiop_rmutex);
		return (_iobptr[i]);
	} else {
		rec_mutex_unlock(&_findiop_rmutex);
		return (NULL);
	}
#else /* _THREAD_SAFE */
	if ( i < nrws && 
	    (_iobptr[i] = (FILE*)malloc(size = sizeof(FILE)*_NIOBRW))) 
	  	return((FILE*)memset((void *)_iobptr[i], (int)NULL, (size_t)size) );
	else
		return(NULL);
#endif /* _THREAD_SAFE */

}
