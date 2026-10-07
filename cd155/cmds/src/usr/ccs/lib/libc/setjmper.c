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
static char rcsid[] = "@(#)$RCSfile: setjmper.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:04 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: longjmperror
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
 * setjmper.c	1.6  com/lib/c/gen,3.1,8943 10/11/89 11:06:08
 */

/*
 * NAME: longjmperror()		(BSD)
 *
 * FUNCTION: writes a message to file descriptor 2 (nominally stderr)
 *	     that longjmp() failed.
 *
 * NOTES: 
 * 	This routine is called from longjmp() when an error occurs.
 * 	Programs that wish to exit gracefully from this error may
 * 	write their own versions.
 * 	If this routine returns, the program is aborted.
 *
 * RETURN VALUES: 
 *	NONE
 */

	/* message for longjmp error */
#define ERRMSG	"longjmp or siglongjmp function used outside of saved context\n"
#ifdef MSG
#include "libc_msg.h"
#define E_ERRMSG NLgetamsg(MF_LIBC, MS_LIBC, M_SETJMPER, ERRMSG)
#else
#define E_ERRMSG ERRMSG
#endif /* MSG */

/*
 * This routine is called from longjmp() when an error occurs.
 * Programs that wish to exit gracefully from this error may
 * write their own versions.
 * If this routine returns, the program is aborted.
 */
void longjmperror()
{
	extern	int	write();	/* system call to write data */
	char	*errmsg = E_ERRMSG;	/* pointer to message text */

	/* write longjmp error message to file descriptor 2, hoping that
 	   file descriptor 2 is stderr */
	write(2, errmsg, NLstrlen(errmsg));
}
