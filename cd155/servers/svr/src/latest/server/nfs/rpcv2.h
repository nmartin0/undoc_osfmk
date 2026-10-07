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
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: rpcv2.h,v $
 * Revision 1.5  1994/11/18  20:37:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:17:09  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:39:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:57  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:36:11  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:33:03  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:31:03  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:25:27  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:54:46  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:41:18  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:40:19  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:23:15  gm]
 * 
 * Revision 1.4  90/05/13  18:45:36  devrcs
 * 	Add AUTH sizes.
 * 	[90/05/04  13:55:10  tmt]
 * 
 * Revision 1.3  90/01/02  20:22:42  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:23:02  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)rpcv2.h	7.1 (Berkeley) 7/5/89
 */

/*
 * Definitions for Sun RPC Version 2, from
 * "RPC: Remote Procedure Call Protocol Specification" RFC1057
 */

/* Version # */
#define	RPC_VER2	2

/* Authentication Flavours */
#define	RPCAUTH_NULL	0
#define	RPCAUTH_UNIX	1
#define	RPCAUTH_SHORT	2
#define	RPCAUTH_MAXSIZ	400
#define	RPCAUTH_UNIXGIDS 16

/* Rpc Constants */
#define	RPC_CALL	0
#define	RPC_REPLY	1
#define	RPC_MSGACCEPTED	0
#define	RPC_MSGDENIED	1
#define	RPC_PROGUNAVAIL	1
#define	RPC_PROGMISMATCH	2
#define	RPC_PROCUNAVAIL	3
#define	RPC_GARBAGE	4		/* I like this one */
#define	RPC_MISMATCH	0
#define	RPC_AUTHFAIL	1

/* Authentication failures */
#define	AUTH_BADCRED	1
#define	AUTH_REJECTCRED	2
#define	AUTH_BADVERF	3
#define	AUTH_REJECTVERF	4
#define	AUTH_TOOWEAK	5		/* Give em wheaties */

/* Sizes of rpc header parts */
#define	RPC_SIZ		24
#define	RPC_REPLYSIZ	28

/* RPC Prog definitions */
#define	RPCPROG_MNT	100005
#define	RPCMNT_VER1	1
#define	RPCMNT_MOUNT	1
#define	RPCMNT_DUMP	2
#define	RPCMNT_UMOUNT	3
#define	RPCMNT_UMNTALL	4
#define	RPCMNT_EXPORT	5
#define	RPCMNT_NAMELEN	255
#define	RPCMNT_PATHLEN	1024
#define	RPCPROG_NFS	100003
