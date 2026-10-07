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
 
/*++ destacct.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/destacct.c,v $
 *
 * DESCRIPTION:
 *
 *	Return a pointer to the account-name portion of a remote
 *	account specification:  account@machine_name.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	November 5, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:30 $ $State: Exp $)
 * $Log: destacct.c,v $
 * Revision 1.3  1994/11/19  02:26:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:42  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:24  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:47:41  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"			/* NQS types and definitions */

/*** destacct
 *
 *
 *	char *destacct():
 *
 *	Return a pointer to the account-name portion of a remote
 *	account specification:  account@machine_name.
 */
char *destacct (remote_account)
register char *remote_account;		/* Remote account specification text */
{
	static char acctname [MAX_ACCOUNTNAME+2];
					/* Remote account name of one */
					/* extra byte so that caller can */
					/* tell if the account name is too */
					/* long. */
	register char *copyto;		/* Copyto pointer */
	register unsigned size;		/* Copy size */

	size = 0;			/* Nothing copied yet */
	copyto = acctname;		/* Where to copy */
	while (*remote_account && *remote_account != '@' &&
	       size < MAX_ACCOUNTNAME+1) {
		*copyto++ = *remote_account++;
		size++;			/* One more character copied */
	}				/* specification */
	*copyto = '\0';			/* Null terminate */
	return (acctname);		/* Return pointer to account name */
}
