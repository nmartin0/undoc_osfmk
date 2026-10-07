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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: data.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:56:25 $";
#endif
/*
 * FUNCTIONS: 
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
 * data.c	1.6  com/lib/c/io,3.1,8943 9/13/89 09:13:03
 */

/*LINTLIBRARY*/
#include <stdio.h>

/* some slop is allowed at the end of the buffers in case an upset in
 * the synchronization of _cnt and _ptr (caused by an interrupt or other
 * signal) is not immediately detected.
 */
unsigned char _sibuf[BUFSIZ+8], _sobuf[BUFSIZ+8];
/*
 * Ptrs to start of preallocated buffers for stdin, stdout.
 */
unsigned char *_stdbuf[] = { _sibuf, _sobuf };


/*
 * Incorporate modifications made for the iPSC/860 to handle
 * I/O to and from multiple nodes.
 */
#define LINEBUFSIZE 256
unsigned char _smbuf[3+1][LINEBUFSIZE];

#ifdef _THREAD_SAFE
#include "rec_mutex.h"
extern struct rec_mutex _stdio_buf_rmutex[3];

FILE _iob[_NIOBRW] = {
	{0, NULL, NULL, 0, _IOREAD, 0,0,0,&_stdio_buf_rmutex[0], NULL},
	{0,_smbuf[1],_smbuf[1],LINEBUFSIZE,_IOWRT+_IOLBF,1,0,0,
		&_stdio_buf_rmutex[1], _smbuf[1]+LINEBUFSIZE},
        {0,_smbuf[2],_smbuf[2], LINEBUFSIZE, _IOWRT+_IOLBF,2,0,0,
                &_stdio_buf_rmutex[2],_smbuf[2]+LINEBUFSIZE}
};
#else /* _THREAD_SAFE */
FILE _iob[_NIOBRW] = {
	{0, NULL, NULL, 0, _IOREAD, 0,0,0,0, NULL},
	{0,_smbuf[1],_smbuf[1],LINEBUFSIZE,_IOWRT+_IOLBF,1,0,0,0,
		_smbuf[1]+LINEBUFSIZE},
#ifdef NX
        {0,_smbuf[2],_smbuf[2],LINEBUFSIZE,_IOWRT+_IOLBF,2,0,0,0,
                _smbuf[2]+LINEBUFSIZE}
#else /* NX */
	{0,_smbuf[2],_smbuf[2],LINEBUFSIZE,_IOWRT+_IONBF,2,0,0,0,
		_smbuf[2]+LINEBUFSIZE}
#endif /* NX */
};
#endif /* _THREAD_SAFE */

FILE *_iobptr[_NRWS] = { _iob,0,0,0,0,0,0,0,0};
/*
 * Ptr to end of io control blocks
 */

