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
 * Mach Operating System
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 * HISTORY
 * $Log: pfs_server.c,v $
 * Revision 1.3  1994/11/18  20:48:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/04/03  03:12:30  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1  1993/02/09  21:51:42  brad
 * Added include of pfs2_user.c, so server-initiated RPC's (forwarding)
 * can be performed.
 *
 * Revision 1.1  1992/11/25  22:26:25  brad
 * Initial revision
 *
 * 
 */

/*
 * Wrapper to compile bsd servers without KERNEL defined.
 */
#include <sys/time.h>

#undef	KERNEL
#ifdef	PFS
#include <uxkern/pfs2_server.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <uxkern/pfs2_user.c>
#endif
