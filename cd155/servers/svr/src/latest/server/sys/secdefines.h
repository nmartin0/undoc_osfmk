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
 
#ifndef __SECDEFINES__
#define __SECDEFINES__

/*
 * @OSF_COPYRIGHT@
 */
/*
 * Copyright (c) 1990 SecureWare, Inc.  All Rights Reserved.
 *
 * @(#)secdefines.h	2.1 16:06:16 4/20/90 SecureWare
 *
 * This header file configures the conditional compilation symbols
 * used to control the inclusion of security features.
 */
/*
 * HISTORY
 * $Log: secdefines.h,v $
 * Revision 1.4  1994/11/18  20:41:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:27:01  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:36:08  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:06  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:02  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:10:59  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:44:43  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:53:24  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:08:55  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:44:01  gm]
 * 
 * Revision 1.2  90/06/22  20:53:39  devrcs
 * 	Added from SecureWare.
 * 	[90/06/09  23:58:46  seiden]
 * 
 * $EndLog$
 */

#include <standards.h>

#if defined(KERNEL) && defined(_OSF_SOURCE)
#include <sec_base.h>
#include <sec_priv.h>
#include <sec_mac_ob.h>
#include <sec_cmw.h>
#include <sec_shw.h>
#include <sec_acl_posix.h>
#include <sec_acl_sware.h>
#include <sec_ncav.h>
#endif /* KERNEL && _OSF_SOURCE */

#if SEC_BASE

/*
 * Resolve mutually exclusive base conditionals
 */

#if SEC_CMW
#undef SEC_MAC_OB
#undef SEC_SHW
#endif

#if SEC_SHW
#undef SEC_MAC_OB
#endif

#if SEC_ACL_POSIX
#undef SEC_ACL_SWARE
#endif

#if defined(_OSF_SOURCE) || defined(AUX)

#define	SEC_AUDIT_SYMLINKS	1	/* system supports symbolic links */
#define	SEC_PTY			1
#define	SEC_SOCKET		1

#include <sys/param.h>
#if NGROUPS
#define	SEC_GROUPS	NGROUPS
#else
#undef	SEC_GROUPS
#endif

#endif /* _OSF_SOURCE || AUX */

#ifdef SYSV_3

#undef	SEC_AUDIT_SYMLINKS
#undef	SEC_PTY
#undef	SEC_SOCKET
#undef	SEC_GROUPS

#endif /* SYSV_3 */


/*
 * The following definitions derive various conditional compilation
 * symbols from the base conditionals that are specified on the
 * compiler command line.
 */

#if SEC_MAC_OB || SEC_SHW || SEC_CMW
#define	SEC_MAC	1
#else
#undef	SEC_MAC
#endif

#if SEC_ACL_SWARE || SEC_ACL_POSIX
#define	SEC_ACL	1
#else
#undef	SEC_ACL
#endif

#if SEC_SHW || SEC_CMW
#define	SEC_ILB	1
#define	SEC_ENCODINGS	1
#else
#undef	SEC_ILB
#undef	SEC_ENCODINGS
#endif

#if SEC_ACL || SEC_MAC || SEC_NCAV
#define	SEC_ARCH	1
#else
#undef	SEC_ARCH
#endif

#if SEC_ARCH || SEC_PRIV
#define	SEC_FSCHANGE	1
#else
#undef	SEC_FSCHANGE
#endif

#else /* !SEC_BASE */

#undef	SEC_BASE
#undef	SEC_PRIV
#undef	SEC_MAC_OB
#undef	SEC_SHW
#undef	SEC_CMW
#undef	SEC_ACL_SWARE
#undef	SEC_ACL_POSIX
#undef	SEC_NCAV
#undef	SEC_ACL
#undef	SEC_MAC
#undef	SEC_ILB
#undef	SEC_ENCODINGS
#undef	SEC_ARCH
#undef	SEC_FSCHANGE
#undef	SEC_PTY
#undef	SEC_SOCKET
#undef	SEC_GROUPS
#undef	SEC_AUDIT_SYMLINKS

#endif /* !SEC_BASE */
#endif /* __SECDEFINES__ */
