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
 
/*++ destdev.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/destdev.c,v $
 *
 * DESCRIPTION:
 *
 *	Return a pointer to the device-name portion of a remote
 *	device specification.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 15, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:30 $ $State: Exp $)
 * $Log: destdev.c,v $
 * Revision 1.3  1994/11/19  02:26:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:47  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:25  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:47:49  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"			/* NQS types and definitions */

/*** destdev
 *
 *
 *	char *destdev():
 *
 *	Return a pointer to the device-name portion of a remote
 *	device specification.
 */
char *destdev (remote_device)
register char *remote_device;		/* Remote device specification text */
{
	static char devname [MAX_DEVNAME+2];
					/* Remote device name of one */
					/* extra byte so that mgr_scan.c */
					/* can tell a remote device */
					/* name that is too long. */
	register char *copyto;		/* Copyto pointer */
	register unsigned size;		/* Copy size */

	size = 0;			/* Nothing copied yet */
	copyto = devname;		/* Where to copy */
	while (*remote_device && *remote_device != '@' &&
	       size < MAX_DEVNAME+1) {
		*copyto++ = *remote_device++;
		size++;			/* One more character copied */
	}				/* specification */
	*copyto = '\0';			/* Null terminate */
	return (devname);		/* Return pointer to device name */
}
