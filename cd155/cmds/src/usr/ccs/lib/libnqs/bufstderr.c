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
 
/*++ bufstderr.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/bufstderr.c,v $
 *
 * DESCRIPTION:
 *
 *	Block buffer the output to the stderr file stream.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 27, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:22 $ $State: Exp $)
 * $Log: bufstderr.c,v $
 * Revision 1.3  1994/11/19  02:26:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:27  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:17  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:46:52  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)bufstderr.c	1.3 (bufstderr.c OSF/1 NQS2.0 GJK) 6/24/92";
#define SCCS
#endif

#include <stdio.h>


/*** bufstderr
 *
 *
 *	void bufstderr():
 *	Block buffer the output to the stderr file stream.
 */
void bufstderr()
{
	static char stderr_buffer [BUFSIZ];	/* Stderr output buffer */

#if	BSD42 | BSD43 | ULTRIX
	setbuffer (stderr, stderr_buffer, BUFSIZ);
#else
#if	SGI | SYS52 | UTS | UNICOS | OSF
	setvbuf (stderr, stderr_buffer, _IOFBF, BUFSIZ);
#else
BAD SYSTEM TYPE
#endif
#endif
}
