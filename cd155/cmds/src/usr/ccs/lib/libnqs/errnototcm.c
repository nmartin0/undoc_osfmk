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
 
/*++ errnototcm.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/errnototcm.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Convert a file access related errno code to its corresponding
 *	transaction code.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 27, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:36 $ $State: Exp $)
 * $Log: errnototcm.c,v $
 * Revision 1.3  1994/11/19  02:26:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:55  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:28  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:48:00  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)errnototcm.c	1.2 (errnototcm.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#include <errno.h>		/* Errno definitions */
#include "transactcc.h"		/* NQS transaction completion codes */

extern int errno;		/* In case not declared in errno.h */

/*** errnototcm
 *
 *
 *	long errnototcm():
 *	Convert errno code to transaction completion code.
 *
 *	Returns:
 *		The transaction code corresponding to the
 *		errno condition.
 */
long errnototcm ()
{
	switch (errno) {
	case EACCES:
		return (TCML_EACCESS);
	case EFBIG:
		return (TCML_EFBIG);
	case EISDIR:
		return (TCML_EISDIR);
#if	BSD42 | BSD43 | ULTRIX
	case ELOOP:
		return (TCML_ELOOP);
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
	case ENFILE:
		return (TCML_ENFILE);
#if	NETWORKED
	case ENOBUFS:
		return (TCML_ENOBUFS);
#endif
	case ENOENT:
		return (TCML_ENOENT);
	case ENOMEM:
		return (TCML_ENOMEM);
	case ENOSPC:
		return (TCML_ENOSPC);
	case ENOTDIR:
		return (TCML_ENOTDIR);
	case ENXIO:
		return (TCML_ENXIO);
	case EPERM:
		return (TCML_EPERM);
	case EPIPE:
		return (TCML_EPIPE);
	case EROFS:
		return (TCML_EROFS);
#if	NETWORKED
	case ETIMEDOUT:
		return (TCML_ETIMEDOUT);
#endif
	case ETXTBSY:
		return (TCML_ETXTBSY);
	}
	return (TCML_FATALABORT);
}
