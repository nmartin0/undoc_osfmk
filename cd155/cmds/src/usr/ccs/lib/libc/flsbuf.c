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
static char rcsid[] = "@(#)$RCSfile: flsbuf.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:04:52 $";
#endif
/*
 * FUNCTIONS: _cleanup, fclose, fflush, _flsbuf, _xflsbuf, _wrtchk, _findbuf,
 *            _bufsync 
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
 * flsbuf.c	1.18  com/lib/c/io,3.1,9021 5/4/90 18:59:22
 */

/*LINTLIBRARY*/
#include "stdiom.h"
#include <errno.h>
#include <stdio.h>
#ifdef _THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef _THREAD_SAFE
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

extern FILE *_iobptr[];
extern unsigned char *_stdbuf[];
extern unsigned char _smbuf[][_SBFSIZ];

/*
 * Flush buffers on exit
 */

void
_cleanup()
{
	register FILE *iop, *_lastbuf;
	register int i;
	int nrws = _NRWS;

	for(i=0; i < nrws && _iobptr[i] != 0; i++ ) {
		_lastbuf = _iobptr[i] + _NIOBRW;
		for(iop = _iobptr[i]; iop < _lastbuf; iop++)
#ifdef _THREAD_SAFE
			(void) unlocked_fclose(iop);
#else
			(void) fclose(iop);
#endif
	}
	for(i=nrws-1; i > 0 ; i-- ) 
		if ( _iobptr[i] != 0 ) 
			free((void *)_iobptr[i]);
	
}
/*
	fclose() will flush (output) buffers for a buffered open
	FILE and then issue a system close on the _fileno.  The
	_base field will be reset to NULL for any but stdin and
	stdout, the _ptr field will be set the same as the _base
	field. The _flags and the _cnt field will be zeroed.
	If buffers had been obtained via malloc(), the space will
	be free()'d.  In case the FILE was not open, or fflush()
	or close() failed, an EOF will be returned, otherwise the
	return value is 0.
 */

int 	
#ifdef  _THREAD_SAFE
unlocked_fclose(FILE *stream)
#else
fclose(FILE *stream)
#endif
{
	int rtn=EOF;

	if(stream == NULL)
		return(rtn);
	if(stream->_flag & (_IOREAD | _IOWRT | _IORW)) {
#ifdef  _THREAD_SAFE
		rtn = (stream->_flag & _IONBF)? 0: unlocked_fflush(stream);
#else
		rtn = (stream->_flag & _IONBF)? 0: fflush(stream);
#endif
		if(close((int)fileno(stream)) < 0) {
			rtn = EOF;
		}
	}
	if(stream->_flag & _IOMYBUF) {
		free((void *)stream->_base);
		stream->_base = NULL;
	}
	stream->_flag = 0;
	stream->_cnt = 0;
	stream->_ptr = stream->_base;
	return(rtn);
}

#ifdef  _THREAD_SAFE
fclose(FILE *stream)
{
	int rc;
	register filelock_t filelock;

	filelock = _flockfile(stream);
	rc = unlocked_fclose(stream);
	_funlockfile(filelock);
	return(rc);
}
#endif

/*
	The fflush() routine must take care because of the
	possibility for recursion. The calling program might
	do IO in an interupt catching routine that is likely
	to interupt the write() call within fflush()
 */

int 	
#ifdef  _THREAD_SAFE
unlocked_fflush(FILE *stream)
#else
fflush(FILE *stream)
#endif
{
	register FILE *iop, *_lastbuf;
	register int i;
	int nrws = _NRWS;
	int rval = 0;

	/*
	 * Flush all buffers recusively.
	 * This should do the right thing for thread safe also.
	 */
	if (stream == NULL) {
		for(i=0; i < nrws && _iobptr[i] != 0; i++ ) {
			_lastbuf = _iobptr[i] + _NIOBRW;
			for(iop = _iobptr[i]; iop < _lastbuf; iop++) {
				if (iop) 
					if(fflush(iop) != 0) rval = EOF;
			}
		} 
		return(rval);
	}

	if (!(stream->_flag & _IOWRT)) {
	  	/*
		 * If we have been doing buffered reads, adjust file-pointer, so
		 * that reads from a dup(2)-ed descriptor will get the unprocessed
		 * bytes
		 */
	  	if((stream->_flag & (_IONBF|_IOREAD))==_IOREAD)
		    lseek( (int) fileno(stream), -(stream->_cnt), SEEK_CUR);

		stream->_cnt = 0;
		return(0);
	}
	while(!(stream->_flag & _IONBF) && (stream->_flag & _IOWRT) &&
			(stream->_base != NULL) && (stream->_ptr > stream->_base) )
		(void) _xflsbuf(stream);

	return(ferror(stream) ? EOF : 0);
}

#ifdef  _THREAD_SAFE

/*
 * 	In the null case, we call unlocked_fflush, which
 * 	will call fflush again, so locking should be ok.
 */

fflush(FILE *stream)
{
	register filelock_t filelock;
	int rc;

	if (stream == NULL)
		return( unlocked_fflush(NULL) );
	filelock = _flockfile(stream);
	rc = unlocked_fflush(stream);
	_funlockfile(filelock);
	return(rc);
}
#endif

/* The routine _flsbuf may or may not actually flush the output buffer.  If
 * the file is line-buffered, the fact that iop->_cnt has run below zero
 * is meaningless: it is always kept below zero so that invocations of putc
 * will consistently give control to _flsbuf, even if the buffer is far from
 * full.  _flsbuf, on seeing the "line-buffered" flag, determines whether the
 * buffer is actually full by comparing iop->_ptr to the end-of-buffer pointer
 * _bufend(iop).  If it is full, or if an output line is completed (with a
 * newline), the buffer is flushed.  (Note: the character argument to _flsbuf
 * is not flushed with the current buffer if the buffer is actually full--
 * it goes into the buffer after flushing.)
 */

int
_flsbuf(c, iop)
unsigned int c;
register FILE *iop;
{
    unsigned char c1;

    do {
	if ( (iop->_flag & (_IOLBF | _IOWRT | _IOEOF)) == (_IOLBF | _IOWRT) ) {
		if ( iop->_ptr >= _bufend(iop) )  /* if buffer full, */
			break;		    /* exit do-while, and flush buf. */
		if ( (*iop->_ptr++ = c) != '\n' )
			return(c);
		return(_xflsbuf(iop) == EOF ? EOF : c);
	}
	if ( (iop->_flag & (_IONBF | _IOWRT | _IOEOF)) == (_IONBF | _IOWRT) ) {
		iop->_cnt = 0;
		c1 = c;
		if (write((int)fileno(iop), (char *)&c1, (unsigned)1) == 1)
			return(c);
		iop->_flag |= _IOERR;
		return(EOF);
	}
	/* The _wrtchk call is here rather than at the top of _flsbuf to re- */
	/* duce overhead for line-buffered I/O under normal circumstances.  */

	if (_WRTCHK(iop)) { /* is writing legitimate? */
		iop->_flag |= _IOERR;
		return(EOF);
	}
    } while ( (iop->_flag & (_IONBF | _IOLBF)) );


    (void) _xflsbuf(iop);   /* full buffer:  flush buffer */
    (void) putc((char) c, iop);  /* then put "c" in newly emptied buf */
			/* (which, because of signals, may NOT be empty) */
    return( ferror(iop) ? EOF : c);
}

/* The function _xflsbuf writes out the current contents of the output
 * buffer delimited by iop->_base and iop->_ptr.
 * iop->_cnt is reset appropriately, but its value on entry to _xflsbuf
 * is ignored.
 *
 * The following code is not strictly correct.  If a signal is raised,
 * invoking a signal-handler which generates output into the same buffer
 * being flushed, a peculiar output sequence may result (for example,
 * the output generated by the signal-handler may appear twice).  At
 * present no means has been found to guarantee correct behavior without
 * resorting to the disabling of signals, a means considered too expensive.
 * For now the code has been written with the intent of reducing the
 * probability of strange effects and, when they do occur, of confining
 * the damage.  Except under extremely pathological circumstances, this
 * code should be expected to respect buffer boundaries even in the face
 * of interrupts and other signals.
 */

int
_xflsbuf(iop)
register FILE *iop;
{
	register unsigned char *base;
	register int n;
	register int nwritten;
	register int offset;

	n = iop->_ptr - (base = iop->_base);
	iop->_ptr = base;
	iop->_cnt = (iop->_flag &(_IONBF | _IOLBF)) ? 0 : _bufsiz(iop);
	_BUFSYNC(iop);

	offset = 0;
	while (n > 0) {
		nwritten = write((int)fileno(iop),(char*)(base+offset),(unsigned)n);
		if (nwritten > 0) {
			n -= nwritten;
			offset += nwritten;
		} else {
			iop->_flag |= _IOERR;
			return(EOF);
		}
	}

	return(0);
}

/* The function _wrtchk checks to see whether it is legitimate to write
 * to the specified device.  If it is, _wrtchk sets flags in iop->_flag for
 * writing, assures presence of a buffer, and returns 0.  If writing is not
 * legitimate, EOF is returned.
 */

int
_wrtchk(iop)
register FILE *iop;
{
	if ( (iop->_flag & (_IOWRT | _IOEOF)) != _IOWRT ) {
		if (!(iop->_flag & (_IOWRT | _IORW))) {
			SETERR(EBADF);
			return(EOF);  /* bogus call--read-only file */
		}
		iop->_flag = iop->_flag & ~_IOEOF | _IOWRT; /* fix flags */
	}
	if (iop->_base == NULL)    /* this is first I/O to file--get buffer */
		_findbuf(iop);
	if (iop->_ptr == iop->_base && !(iop->_flag & (_IONBF | _IOLBF)) )  {
		iop->_cnt = _bufsiz(iop); /* first write since seek--set cnt */
		_BUFSYNC(iop);
	}
	return(0);
}

/*
 * _findbuf, called only when iop->_base == NULL, locates a predefined buffer
 * or allocates a buffer using malloc.  If a buffer is obtained from malloc,
 * the _IOMYBUF flag is set in iop->_flag.
 */

_findbuf(iop)
register FILE *iop;
{
	register int fno = fileno(iop); /* file number */
	int size;
	struct stat stbuf;
	int saved_errno;

	if (fno < 2)  /* use existing bufs for stdin, stdout */
		_bufend(iop) = (iop->_base = _stdbuf[fno]) + BUFSIZ;
	else
	{
		if (fstat(fno, &stbuf) < 0 || stbuf.st_blksize <= NULL)
			size = BUFSIZ;
		else
			size = stbuf.st_blksize;

		if ((iop->_base = (unsigned char *)malloc(size)) != NULL) {
			iop->_flag |= _IOMYBUF;
			_bufend(iop) = iop->_base + size;
		}
		else {		/* try for a real small buffer */
			iop->_base = 
				(unsigned char *) malloc((size_t)(_SBFSIZ+8));
			iop->_flag |= _IOMYBUF;
			_bufend(iop) = iop->_base + _SBFSIZ;
		}
	}

	iop->_ptr = iop->_base;
	saved_errno = errno;
	if ( isatty(fno) && !(iop->_flag & _IONBF) )
		iop->_flag |= _IOLBF;
	SETERR(saved_errno);
}

/* The function _bufsync is called because interrupts and other signals
 * which occur in between the decrementing of iop->_cnt and the incrementing
 * of iop->_ptr, or in other contexts as well, may upset the synchronization
 * of iop->_cnt and iop->ptr.  If this happens, calling _bufsync should
 * resynchronize the two quantities (this is not always possible).  Resyn-
 * chronization guarantees that putc invocations will not write beyond
 * the end of the buffer.  Note that signals during _bufsync can cause
 * _bufsync to do the wrong thing, but usually with benign effects.
 */

_bufsync(iop)
register FILE *iop;
{
	register int spaceleft;

	if ((spaceleft = _bufend(iop) - iop->_ptr) < 0)
		iop->_ptr = _bufend(iop);
	else if (spaceleft < iop->_cnt)
		iop->_cnt = spaceleft;
}
