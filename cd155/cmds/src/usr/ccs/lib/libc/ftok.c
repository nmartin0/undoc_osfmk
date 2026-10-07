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
static char rcsid[] = "@(#)$RCSfile: ftok.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:16 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: ftok
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
 * ftok.c	1.6  com/lib/c/gen,3.1,8943 9/8/89 08:47:54
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
/* #include <sys/sysmacros.h> */

/*
 * NAME:	ftok
 *                                                                    
 * FUNCTION:	generate a standard ipc key
 *                                                                    
 * NOTES:	Ftok generates a key based on 'path' and 'id'.  'Path'
 *		is the pathname of a currently existing file that is
 *		accessible to the process.  'Id' is a character that
 *		can cause different keys to be generated for the same
 *		'path'.
 *
 * RETURN VALUE DESCRIPTION:	-1 if the file does not exist or is
 *		not accessible to the process, -1 if 'id' is '\0',
 *		else the key
 */  

key_t
ftok(path, id)
char *path;			/* pathname to base key on */
char id;			/* unique id per application */
{
	struct stat st;

	/* Leave keys 0 to (2^24-1) for SNA, other use */
	if(id == '\0')
		return((key_t)-1);

	/*
	 * if the file is not accessible or doesn't exist, return -1.
	 * else use a combo of 'id', the minor device number of the
	 * device the file lives on, and the file's inode number to
	 * compute the key...
	 */
	return(stat(path, &st) < 0 ? (key_t)-1 :
	    (key_t)((key_t)id << 24 | ((long)(unsigned)minor(st.st_dev)) << 16 |
		(unsigned)st.st_ino));
}
