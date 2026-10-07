/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: misc_routines.c,v $
 * Revision 1.4  1994/11/18  20:31:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:56:21  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:15:44  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:19:58  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:24  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:35:19  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/04/12  14:06:17  condict
 * Comment out "min" and "imin" funcs.
 * 
 * Revision 3.0  91/01/17  12:10:17  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/05/21  13:53:19  dbg
 * 	Created.
 * 	[90/05/03            dbg]
 * 
 */

/*
 *	Object:
 *		ovbcopy				EXPORTED function
 *
 *		Overlapped byte copy
 *
 */
ovbcopy(from,to,len)
	register char *from,*to;
	register len;
{
	if (from < to) {
		from += len;
		to += len;
		while (len--)
			*--to = *--from;
	} else {
		while (len--)
			*to++ = *from++;
	}
}

#ifndef	OSF1_SERVER
unsigned min(a,b)
register unsigned a,b;
{return (a < b) ? a : b;}

imin(a,b){return (a > b) ? b : a;}
#endif	/* OSF1_SERVER */
