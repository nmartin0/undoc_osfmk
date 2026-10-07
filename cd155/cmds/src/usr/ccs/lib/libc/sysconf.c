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
static char rcsid[] = "@(#)$RCSfile: sysconf.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 02:08:48 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: sysconf
 *
 * ORIGINS: 27
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
 * sysconf.c	1.6  com/lib/c/gen,3.1,8943 10/27/89 15:54:19
 */

#include <unistd.h>
#include <time.h>
#include <sys/param.h>
#include <limits.h>
#include <errno.h>
#include <userpw.h>

#ifdef  _THREAD_SAFE
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

/*
 * NAME: sysconf
 *
 * FUNCTION: The sysconf() function provides a method for the application to 
 *	determine the current value of a configurable system limit or option.
 *      The 'name' argument represents the system variable to be queried.
 *	These variables are found in <limits.h> or <unistd.h>.
 *
 * EXECUTION ENVIRONMENT:
 *
 * DATA STRUCTURES:  none
 *
 * RETURNS:
 * 	If 'name' is an invalid value, sysconf() returns a -1; otherwise,
 *      the sysconf() function will return the current variable value on 
 * 	the system.
 *
 * ERRORS:
 *	EINVAL		The value of the 'name' argument is invalid.
 *
 */
long
sysconf(int name)
{

	switch (name)
	{
		case _SC_AES_OS_VERSION:
#ifndef _AES_OS_VERSION
	  		return(-1);
#else
			return(_AES_OS_VERSION);
#endif

	  	case _SC_ATEXIT_MAX:
#ifndef ATEXIT_MAX
	  		return(-1);
#else
	  		return(ATEXIT_MAX);
#endif

		case _SC_PAGE_SIZE:
			return (getpagesize());

		case _SC_ARG_MAX: 
		/*
 	 	 * ARG_MAX should really be set to the value of NCARGS so that
	 	 * if ARG_MAX is undefined, a -1 can be returned (like the POSIX
	 	 * document says) and if it is defined, ARG_MAX can be returned
	 	 * (which is really the value that currently exists as NCARGS.
 	 	 */
#ifndef ARG_MAX
			return(-1);
#else
			return(ARG_MAX);
#endif

		case _SC_CHILD_MAX:		 
#ifndef CHILD_MAX
			return(-1);
#else
			/*
 		 	 * root has no limitation of the number of child
 		 	 * processes, so we are returning the number of
			 * process table slots.
 			 */
			if (geteuid() == 0)
				return(CHILD_MAX);
			else
				return(CHILD_MAX);	
#endif

		case _SC_CLK_TCK:
#ifndef CLK_TCK
			return(-1);
#else
			return(CLK_TCK);
#endif

		case _SC_NGROUPS_MAX:
#ifndef NGROUPS_MAX
			return(-1);
#else
			return(NGROUPS_MAX);
#endif

		case _SC_OPEN_MAX:
#ifndef OPEN_MAX
			return(-1);
#else
			return( getdtablesize());
#endif

		case _SC_PASS_MAX:
#ifdef PASS_MAX
			return(PASS_MAX);
#else
			return(MAX_PASS);
#endif

		case _SC_JOB_CONTROL:
#ifdef _POSIX_JOB_CONTROL
			return(TRUE);
#else	
			return(-1);
#endif

		case _SC_SAVED_IDS:
#ifdef _POSIX_SAVED_IDS
			return(TRUE);
#else
			return(-1);
#endif

		case _SC_VERSION:
#ifndef _POSIX_VERSION
			return(-1);
#else
			return(_POSIX_VERSION);
#endif

		case _SC_XOPEN_VERSION:
#ifndef	_XOPEN_VERSION
			return(-1);
#else
			return(_XOPEN_VERSION);
#endif

		default:
			SETERR(EINVAL);
			return(-1);
	}

}  /* end sysconf */
