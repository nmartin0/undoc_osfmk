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
static char rcsid[] = "@(#)$RCSfile: mkfifo.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:11 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: mkfifo
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 * 
 * mkfifo.c	1.9  com/lib/c/gen,3.1,8943 10/13/89 15:08:26
 */

#include <sys/limits.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/errno.h> 

/*
 *
 * SYNOPSIS: mkfifo() is given a pathname and a set of permissions for
 *	a pipe to be created. We then test against system limits and
 *	pass the request on to mknod(), a system call which will create
 *	the pipe, set the mode, and set up the file inode and time
 *	stats. We pick up the return code and errno (if any), then
 *	act accordingly.
 */

/* The mkfifo() function includes all the POSIX requirements */

extern int errno; 

int 
mkfifo(char *path, mode_t mode) 
{ 

  if((strlen(path)) > PATH_MAX)
  {
    errno = ENAMETOOLONG;
    return(-1);
  }

/**********
  mknod() will do all the dirty work.
**********/
  mode |= S_IFIFO;	/* OR mode with the FIFO creation code */
  if((mknod(path, mode)) != 0)
    return(-1);

  return(0); 
}
