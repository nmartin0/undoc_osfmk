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
 
/*++ ioblksiz.h - optimal block size for indivisible file system I/O operations
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/ioblksiz.h,v $
 *
 * DESCRIPTION:
 *
 *	This header file defines the optimal block size for indivisible
 *	file system I/O operations.  This value defines the largest
 *	number of bytes that can be read/written from/to a file in a
 *	single indivisible I/O operation (ATOMICBLKSIZ).  It is also
 *	permissible for this value to be an exact divisor of the true
 *	"atomic block size."  In either case, this value MUST satisfy
 *	these constraints for the supporting system.
 *
 *
 *	WARNING:
 *	  Seven system types are defined:  BSD42, BSD43, SGI, SYS52,
 *	  ULTRIX, UNICOS, and UTS.  Accordingly, the flag: SYSTEM_TYPE
 *	  in the Makefile must be appropriately defined.
 *
 *	  The definition of ATOMICBLKSIZ must NOT be changed without
 *	  careful forethought.  If the value of ATOMICBLKSIZ is changed,
 *	  then the database software that is dependent upon the correct
 *	  definition of ATOMICBLKSIZ can be easily broken.  Examples of
 *	  such software include the nmap (network mapping) database
 *	  routines, and the Network Queueing System (NQS).
 *
 *	  Be careful.
 *
 *
 *	Author(s):
 *	----------
 *	Brent A. Kingsbury, and Kate R. Rosenbloom.
 *	Sterling Software Incorporated.
 *
 *
 * STANDARDS VIOLATIONS:
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:43:54 $ $State: Exp $)
 * $Log: ioblksiz.h,v $
 * Revision 1.4  1994/11/19  02:43:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:05:54  mwan
 * R1.2 mods.
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:50  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:58:58  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)ioblksiz.h	1.2 (ioblksiz.h OSF/1 NQS2.0 GJK) 5/20/92";
#endif

#if	IO512
#define ATOMICBLKSIZ	512		/* Old style */
#else

#if	BSD42 | BSD43 | ULTRIX		/* Berkeley UNIX */
#define	ATOMICBLKSIZ	4096		/* (On some Berkeley systems, it */
					/* might be 8192) */
#else
#if	SGI				/* Silicon Graphics Iris UNIX */
#define	ATOMICBLKSIZ	1024
#else
#if	SYS5 | SYS52			/* AT&T garden variety UNIX */
#define	ATOMICBLKSIZ	1024
#else
#if	UTS				/* Amdahl Unix */
#define	ATOMICBLKSIZ	4096
#else
#if	UNICOS				/* Cray UNIX */
#define	ATOMICBLKSIZ	4096
#else
#if     OSF                          	/* OSF/1 MACH */
#define ATOMICBLKSIZ    4096
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#endif
#endif
#endif
#endif
