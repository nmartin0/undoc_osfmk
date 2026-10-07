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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: remove.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:43 $";
#endif
/*
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: remove 
 *
 * ORIGINS: 3, 27 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * remove.c	1.9  com/lib/c/io,3.1,9013 2/16/90 08:28:07
 */

/*                                                                    
 * FUNCTION: The remove() function causes the file, whose name is pointed
 *	     to by filename, to be removed
 *
 * PARAMETERS: char *filename - file to be removed
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	      0 if successful
 *	      non-0 if not successful
 *
 */
/* The remove() function includes all the POSIX requirements */
#include <sys/stat.h>
int	
remove(const char *filename)
{
	struct stat	stbuf;

	if (lstat((char *)filename, &stbuf) < 0)
		return (-1);
	if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
		return(rmdir(filename));
	}
	else {
		return(unlink(filename));
	}
}
