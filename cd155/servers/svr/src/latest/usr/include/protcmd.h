/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#if SEC_BASE
#ifndef __PROTCMD__
#define __PROTCMD__

/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: protcmd.h,v $
 * Revision 1.3  1994/11/18  21:10:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:33  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:29  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:18:05  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:38  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:38:35  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:33:20  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:54:08  gm]
 * 
 * Revision 1.2  90/06/22  21:52:17  devrcs
 * 	Initial version from SecureWare
 * 	[90/06/01  08:15:59  staffan]
 * 
 * $EndLog$
 */

/* Copyright (c) 1988, SecureWare, Inc.
 *   All rights reserved
 *
 * @(#)protcmd.h	2.1 16:06:32 4/20/90
 * Based on:
 *   @(#)protcmd.h	2.5 10:47:19 9/22/89 SecureWare
 */

/*
 * This Module contains Proprietary Information of SecureWare, Inc.
 * and should be treated as Confidential.
 */

/*
 * Definitions for security-relevant routines of enhanced UNIX commands
 * that must be protected.
 */

#define AT_SEC_WRITEMODE	0220	/* Secure mode for creating files */
#define AT_SEC_ATMODE		06040	/* Secure mode for using files */
#define CRONTAB_SEC_CRMODE	0440	/* Secure mode for creating crontabs. */
#define	INIT_DATA_FILE_MODE	0600
#define LOGIN_PROGRAM		"/tcb/lib/login"
#define PASSWD_PROGRAM		"/tcb/bin/passwd"
#define INITCOND_PROGRAM	"/tcb/lib/initcond"
#define	DEV_LEADER		"/dev/"
#define	INIT_INITTAB_LOCATION	"/tcb/files/inittab"
#define	INIT_SU_LOCATION	"/tcb/bin/su"

#if SEC_MAC || SEC_ILB
#define	LP_LABEL_PREFIX		'L'
#define	LP_FILTER_PREFIX	'f'
#endif

#endif /* __PROTCMD__ */
#endif /* SEC_BASE */
