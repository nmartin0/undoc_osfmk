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
static char rcsid[] = "@(#)$RCSfile: getwc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:59 $";
#endif
/*
 * FUNCTIONS: getwc 
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * getwc.c	1.4  com/lib/c/io,3.1,8943 9/12/89 18:33:35
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <NLchar.h>

/*
 * FUNCTION:	
 * Get a one or two byte character and return it as a wide char.
 *
 * RETURN VALUE DESCRIPTION:	
 * Returns EOF if getc returns EOF on either the first or second byte read.
 *
 */  

int
#ifdef	_THREAD_SAFE
unlocked_getwc(FILE *stream)
#else
getwc(FILE *stream)
#endif
{
        register int wc, lowbyte;
	char ar[2];
	NLchar nlc;

	if (EOF != (wc = getc(stream))) wc &= 0xff;
	/*
	 *   If this is not a two byte shift character, return it
	 */
	if ( ! NCisshift(wc) ) return(wc);

	if (EOF != (lowbyte = getc(stream)))
		lowbyte &= 0xff;
	else
		return(EOF);
	/*
	 *   The first character was an NC shift character and
	 *   the second was not EOF so convert it to an NLchar
	 *   and return that.
	 */
	ar[0] = (char) wc;
	ar[1] = (char) lowbyte;
	NCdecode(ar,&nlc);
	return(nlc);
}

#ifdef	_THREAD_SAFE
int
getwc(FILE *stream)
{
        register int wc, lowbyte;
	char ar[2];
	NLchar nlc;
	register filelock_t filelock;

	filelock = _flockfile(stream);
	if (EOF != (wc = unlocked_getc(stream))) wc &= 0xff;
	/*
	 *   If this is not a two byte shift character, return it
	 */
	if ( ! NCisshift(wc) ) {
		_funlockfile(filelock);
		return(wc);
	}

	if (EOF != (lowbyte = unlocked_getc(stream)))
		lowbyte &= 0xff;
	else {
		_funlockfile(filelock);
		return(EOF);
	}
	/*
	 *   The first character was an NC shift character and
	 *   the second was not EOF so convert it to an NLchar
	 *   and return that.
	 */
	ar[0] = (char) wc;
	ar[1] = (char) lowbyte;
	NCdecode(ar,&nlc);
	_funlockfile(filelock);
	return(nlc);
}
#endif
