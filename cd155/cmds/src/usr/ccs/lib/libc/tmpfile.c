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
static char rcsid[] = "@(#)$RCSfile: tmpfile.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:09 $";
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
 * FUNCTIONS: tmpfile  
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
 * sccsid[] = "tmpfile.c 1.11  com/lib/c/io,3.1,9021 3/9/90 11:37:46";
 */

#include <stdio.h>
#include <fcntl.h>

extern int unlink();

/*                                                                    
 * FUNCTION: tmpfile - return a pointer to an update file that can be
 *	     used for scratch. The file will automatically
 *	     go away if the program using it terminates.
 *
 * RETURN VALUE DESCRIPTIONS:
 *           returns a pointer to the stream of the file that it created
 *	     If the file cannot be created, the tmpfile function returns
 *	     a null pointer.
 */

FILE  *
tmpfile(void)
{
	char	tfname[L_tmpnam];
	FILE	*p = NULL;
	int fd;

	(void) tmpnam(tfname);
	if ((fd = open(tfname, O_RDWR|O_EXCL|O_CREAT, 0666)) != -1) {
		p = fdopen(fd, "wb+");
		(void) unlink(tfname);
	}
	return(p);
}
