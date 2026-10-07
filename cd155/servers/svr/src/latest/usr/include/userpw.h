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
 * HISTORY
 * $Log: userpw.h,v $
 * Revision 1.3  1994/11/18  21:11:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:43  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:37  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:19:39  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:54  nandy
 * Initial revision
 *
 * Revision 1.4  92/04/13  14:40:13  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.4  90/10/07  20:38:56  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:02:37  gm]
 * 
 * Revision 1.3  90/04/27  23:08:14  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/16  13:50:21  tom]
 * 
 * Revision 1.2  90/03/13  21:24:03  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:40:14  tom]
 * 
 * $EndLog$
 */
/* @(#)userpw.h	1.6  com/inc,3.1,8943 10/2/89 23:10:02 */
/*
 * COMPONENT_NAME: (userpw.h) header to access user passwd info 
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _USERPW_H_    
#define _USERPW_H_

#include <standards.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/limits.h>

/* function declarations	*/
#ifndef _NO_PROTO
	extern	struct userpw  	*getuserpw (char *);
	extern	int		putuserpw (struct userpw *);
	extern	int		setpwdb (int);
	extern	int		endpwdb (void);
#else	
	extern	struct userpw  	*getuserpw ();
	extern	int		putuserpw ();
	extern	int		setpwdb ();
	extern	int		endpwdb ();
#endif


/* manifest constants and limits	*/
#define	MAX_PASS	PASS_MAX

/* flags for user password */
#define	PW_NOCHECK	0x1
#define	PW_ADMCHG	0x2
#define	PW_ADMIN	0x4

#define PW_PASSLEN	8		/* maximum length of a password */
#define PW_NAMELEN	32		/* max length of a user's name */
#define PW_CRYPTLEN	16		/* length of the encrypted password */

struct	userpw
{
	char	upw_name[PW_NAMELEN];	/* user's name */
	char	*upw_passwd;		/* user's passwd */
	ulong	upw_flags;		/* flags of restrictions */
	ulong	upw_lastupdate;		/* date of last passwd update */
};

#endif /* _USERPW_H_ */
