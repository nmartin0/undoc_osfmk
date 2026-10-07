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
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: libc.c,v $
 * Revision 1.4  1994/11/18  20:30:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:56:00  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:15:24  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:19:48  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:09  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:34:21  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/05  13:57:07  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  13:41:50  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:54:18  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:35:58  gm]
 * 
 * Revision 1.3  90/07/17  11:30:42  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:23:46  kevins]
 * 
 * Revision 1.5  89/04/05  12:57:59  rvb
 * 	afs needs strcmp, too.
 * 	[89/03/21            rvb]
 * 
 * Revision 1.4  89/03/09  20:02:41  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:33:44  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 0.0  89/02/16            rvb
 * 	Created
 * 
 * 	These libc functions are currently needed by afs.
 * 	[89/02/16            rvb]
 * 
 * $EndLog$
 */

#if	defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)strcpy.c	5.2 (Berkeley) 3/9/86";
#endif	LIBC_SCCS and not lint

/*
 * Copy string s2 to s1.  s1 must be large enough.
 * return s1
 */

char *
strcpy(s1, s2)
register char *s1, *s2;
{
	register char *os1;

	os1 = s1;
	while (*s1++ = *s2++)
		;
	return(os1);
}

#if	defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)strncpy.c	5.2 (Berkeley) 3/9/86";
#endif	LIBC_SCCS and not lint

/*
 * Copy s2 to s1, truncating or null-padding to always copy n bytes
 * return s1
 */

char *
strncpy(s1, s2, n)
register char *s1, *s2;
{
	register i;
	register char *os1;

	os1 = s1;
	for (i = 0; i < n; i++)
		if ((*s1++ = *s2++) == '\0') {
			while (++i < n)
				*s1++ = '\0';
			return(os1);
		}
	return(os1);
}

strcmp(s1, s2)
register char *s1, *s2;
{

	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return(0);
	return(*s1 - *--s2);
}

strncmp(s1, s2, n)
register char *s1, *s2;
register n;
{

	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);
	return(n<0 ? 0 : *s1 - *--s2);
}
