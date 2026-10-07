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
 
/*++ nmap.h -
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nmap.h,v $
 *
 * DESCRIPTION:
 *	Nmap.h:  This header file contains the definitions of the
 *		 data types and return codes used in the Network
 *		 Mapping procedures in the Nmap library.
 *
 * STANDARDS VIOLATIONS:
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:44:00 $ $State: Exp $)
 * $Log: nmap.h,v $
 * Revision 1.4  1994/11/19  02:44:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:05:58  mwan
 * R1.2 mods.
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:52  root
 * Version 2.0 Source
 * 
 * Revision 2.3  87/08/25  16:56:08  hender
 * uid_t gid_t now defined in /usr/netinclude/sys/types.h (4.3)
 * 
 * Revision 2.2  87/04/22  14:59:06  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nmap.h	1.4 (nmap.h OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
#endif

#ifndef _NQS_NMAPH_
#define _NQS_NMAPH_

/*
 *
 *	Data types:
 */
#if OSF
typedef long mid_t;             /* Machine-id type.             */
/* conflicting with  <sys/types.h> !!! */
#else
typedef	long mid_t;		/* Machine-id type.		*/
#endif

#if	BSD42 | SYS5 | SYS52 | ULTRIX | UNICOS  
/* the following are defined in /usr/netinclude/sys/typesh. (4.3 level) */
typedef unsigned short uid_t;	/* User-id type.		*/
typedef unsigned short gid_t;	/* Group-id type.		*/
#else
#if	BSD43 | UTS |  SGI | OSF	/* uid_t and gid_t defined in <sys/types.h> */
#else
BAD SYSTEM TYPE
#endif
#endif

/*
 *
 *	Return codes:
 */
#define	NMAP_SUCCESS	 0	/* Success!			*/
#define	NMAP_DEFMAP	 1	/* No uid(gid) mapping existed.	*/
				/* The default uid(gid) mapping	*/
				/* for the specified machine has*/
				/* been returned.		*/
#define	NMAP_EUNEXPECT	-1	/* Serious major error; like	*/
				/* EMFILE or ENFILE, or one of	*/
				/* mapping files is screwed up;	*/
				/* Something that should have	*/
				/* worked, did not.		*/
#define NMAP_ENOPRIV	-2	/* Insufficient privilege to	*/
				/* access or update mapping	*/
				/* database.			*/
#define	NMAP_ECONFLICT	-3	/* Machine already exists or an	*/
				/* attempt was made to add a	*/
				/* uid(gid) mapping which	*/
				/* already existed on the target*/
				/* machine and the new mapping	*/
				/* differed from the existing	*/
				/* mapping.			*/
#define	NMAP_ENOMAP	-4	/* No mapping exists.		*/
#define	NMAP_ENOMID	-5	/* No machine as specified by	*/
				/* the mid parameter in the call*/
				/* exists.			*/
#define	NMAP_EBADNAME	-6	/* Bad name specified.		*/
/*
 *
 *	nmap_ctl opcodes:
 */
#define	NMAP_OPNOFD	0	/* Set number of file descriptors */
#define	NMAP_OPUIDC	1	/* Set uid cache size */
#define	NMAP_OPGIDC	2	/* Set gid cache size */
#define	NMAP_OPNAMC	3	/* Set name cache size */
#endif	/* _NQS_NMAPH_ */
