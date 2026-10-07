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
static char rcsid[] = "@(#)$RCSfile: filbuf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:48 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: _filbuf 
 *
 * ORIGINS: 3, 27 
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
 * sccsid[] = "filbuf.c  1.9  com/lib/c/io,3.1,9021 4/13/90 11:48:12";
 */

/*LINTLIBRARY*/
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

extern _findbuf();
extern FILE *_iobptr[];

int
_filbuf(iop)
register FILE *iop;
{
	register FILE *diop, *_lastbuf;
	register int i;
	int nrws = _NRWS;

	if (iop->_base == NULL)  /* get buffer if we don't have one */
		_findbuf(iop);

	if ( !(iop->_flag & _IOREAD) )
		if (iop->_flag & _IORW)
			iop->_flag |= _IOREAD;
		else {
			errno = EBADF;
			return(EOF);
		}

	/* if this device is a terminal (line-buffered) or unbuffered, then */
	/* flush buffers of all line-buffered devices currently writing */

	if (iop->_flag & (_IOLBF | _IONBF))
		for(i=0; i < nrws && _iobptr[i] != 0  ;  i++ ) {   
			_lastbuf = _iobptr[i] + _NIOBRW;
			for(diop = _iobptr[i]; diop < _lastbuf; diop++) 
				if (diop->_flag & _IOLBF)
					(void) fflush(diop);
		}

	iop->_ptr = iop->_base;
	iop->_cnt = read((int)fileno(iop), (char *)iop->_base,
	    (unsigned)((iop->_flag & _IONBF) ? 1 : _bufsiz(iop) ));
	if (--iop->_cnt >= 0)		/* success */
		return (*iop->_ptr++);
	if (iop->_cnt != -1)		/* error */
		iop->_flag |= _IOERR;
	else {				/* end-of-file */
		iop->_flag |= _IOEOF;
		if (iop->_flag & _IORW)
			iop->_flag &= ~_IOREAD;
	}
	iop->_cnt = 0;
	return (EOF);
}
