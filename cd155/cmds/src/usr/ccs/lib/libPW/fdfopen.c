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
static char sccsid[] = "@(#)fdfopen.c	1.5  com/lib/PW,3.1,8943 9/11/89 09:21:38";
/*
 * COMPONENT_NAME: (LIBPW) Programmers Workbench Library
 *
 * FUNCTIONS: fdfopen
 *
 * ORIGINS: 3 27
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
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 */

# include	"stdio.h"
# include	"sys/types.h"
# include	"macros.h"

/*
 * FUNCTION: Return FILE * for file descriptor argument
 *
 * RETURN VALUE DESCRIPTIONS:
 *	Returns file pointer on success,
 *	NULL on failure (fd not open or no file structures available).
 */
/*
	Interface to stdio
	First arg is file descriptor, second is read/write mode (0/1).
*/


FILE *
fdfopen(fd, mode)
register int fd, mode;
{
	register FILE *iop;

	if (fstat(fd, &Statbuf) < 0)
		return(NULL);
	for (iop = _iob; iop->_flag&(_IOREAD|_IOWRT); iop++)
		if (iop >= &_iob[_NFILE-1])
			return(NULL);
	iop->_flag &= ~(_IOREAD|_IOWRT);
	iop->_file = fd;
	if (mode)
		iop->_flag |= _IOWRT;
	else
		iop->_flag |= _IOREAD;
	return(iop);
}
