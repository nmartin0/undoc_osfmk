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
 * $Log: ldr_exec.h,v $
 * Revision 1.4  1994/11/18  20:40:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:25:29  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:19  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:42:25  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:55  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:07:29  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:43:23  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:51:34  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:41:14  gm]
 * 
 * Revision 1.2  90/06/22  20:53:17  devrcs
 * 	Initial Revision.
 * 	[90/06/07  16:41:29  kwallace]
 * 
 * $EndLog$
 */

/*
 * Default Loader File Name
 */
#define	LDR_EXEC_DEFAULT_LOADER	"/sbin/loader"

/*
 *  AT_EXEC_LOADER_FLAGS Auxiliary Vector Entry Value
 */
#define	LDR_EXEC_SYSTEM_MASK	0xffff0000
#define	LDR_EXEC_USER_MASK	0x0000ffff

#define	LDR_EXEC_SETUID_F	0x80000000
#define	LDR_EXEC_SETGID_F	0x40000000
#define	LDR_EXEC_PTRACE_F	0x20000000
