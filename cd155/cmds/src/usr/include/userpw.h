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
