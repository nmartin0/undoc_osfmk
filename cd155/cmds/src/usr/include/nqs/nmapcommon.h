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
 
/*++ nmapcommon.h -
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nmapcommon.h,v $
 *
 * DESCRIPTION:
 *	Nmapcommon.h:  Header file containing definitions for use with nmap
 *
 * STANDARDS VIOLATIONS:
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:44:01 $ $State: Exp $)
 * $Log: nmapcommon.h,v $
 * Revision 1.4  1994/11/19  02:44:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:06:02  mwan
 * R1.2 mods.
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:55  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:59:41  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nmapcommon.h	1.2 (nmapcommon.h OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
#endif

#if OSF
#include "sys_types_OSF.h"
#else
#include <sys/types.h> /* in OSF sys_types_OSF.h */
#endif

#if	SGI | SYS5 | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif

#include <errno.h>
#include NETDB			/* Berkeley network database */

#include NMAP			/* Network mapping headers */
#include IOBLKSIZ
#include <nmapdb.h>
#if	NEWCASTLE
#include "syscall.h"		/* Non-Newcastle system calls */
#define gethostbyname _gthstbynm  /* Newcastle version sans stdio */
#else
#define	CLOSE	close
#define	LSEEK	lseek
#define	OPEN	open
#define	READ	read
#define	WRITE	write
#endif

extern int errno;		/* In case not declared in errno.h */

/*
 *	Mapping modes:
 */
#define	BY_NAM	0001		/* Request by name */
#define	BY_UID	0002		/* Request by user-id */
#define	BY_GID	0004		/* Request by group-id */
#define	ADD_OP	0010		/* Add mapping */
#define	DEL_OP	0020		/* Delete mapping */
#define	GET_OP	0040		/* New map request */
#define	ADD_UID	(BY_UID|ADD_OP)
#define	ADD_GID	(BY_GID|ADD_OP)
#define	ADD_NAM	(BY_NAM|ADD_OP)
#define	DEL_UID	(BY_UID|DEL_OP)
#define	DEL_GID	(BY_GID|DEL_OP)
#define	DEL_NAM	(BY_NAM|DEL_OP)
#define	GET_UID	(BY_UID|GET_OP)
#define	GET_GID	(BY_GID|GET_OP)
#define	GET_MID	(BY_NAM|GET_OP)

extern int name_mapfile;	/* File descriptor for machines		*/
				/* database: -1 if closed, > -1 if open.*/
extern int machine_file;	/* File descriptor for machine file	*/
				/* -1 if closed, > -1 if open.		*/
extern int nfds;		/* This variable determines the number	*/
				/* file descriptors that the network	*/
				/* mapping software will be allowed to	*/
				/* keep open between calls (0, 1, 2).	*/
