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
 
/*++ nqsdeflim.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nqsdeflim.h,v $
 *
 * DESCRIPTION:
 *
 *	Default batch queue resource limits are defined here.
 *
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	February 13, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:44:05 $ $State: Exp
$)
 * $Log: nqsdeflim.h,v $
 * Revision 1.4  1994/11/19  02:44:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/12/16  23:23:33  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  22:17:57  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:46  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:44:19  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqsdeflim.h	1.3 (nqsdeflim.h OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
#endif

#if	BSD42 | BSD43 | ULTRIX
#define DLIM_PPCOREUNITS	QLM_MBYTES
#define DLIM_PPCORECOEFF	1L
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#define DLIM_PPCOREUNITS	QLM_BYTES
#define DLIM_PPCORECOEFF	0L
#else
BAD SYSTEM TYPE
#endif
#endif

#if	BSD42 | BSD43 | ULTRIX
#define DLIM_PPDATAUNITS	QLM_MBYTES
#define DLIM_PPDATACOEFF	1L
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#define DLIM_PPDATAUNITS	QLM_BYTES
#define DLIM_PPDATACOEFF	0L
#else
BAD SYSTEM TYPE
#endif
#endif

#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | OSF
#define DLIM_PPPFILEUNITS	QLM_MBYTES
#ifdef SDSC
#define DLIM_PPPFILECOEFF	0L
#else
#define DLIM_PPPFILECOEFF	1L
#endif
#else
#if	UNICOS
#define DLIM_PPPFILEUNITS	QLM_MBYTES
#define DLIM_PPPFILECOEFF	100L
#else
#if	UTS
#define DLIM_PPPFILEUNITS	QLM_MBYTES
#define DLIM_PPPFILECOEFF	100L
#else
BAD SYSTEM TYPE
#endif
#endif
#endif

#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UNICOS | UTS | OSF
#define DLIM_PRPFILEUNITS	QLM_BYTES
#define DLIM_PRPFILECOEFF	0L
#else
BAD SYSTEM TYPE
#endif

#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UNICOS | UTS | OSF
#define DLIM_PPQFILEUNITS	QLM_BYTES
#define DLIM_PPQFILECOEFF	0L
#else
BAD SYSTEM TYPE
#endif

#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UNICOS | UTS | OSF
#define DLIM_PRQFILEUNITS	QLM_BYTES
#define DLIM_PRQFILECOEFF	0L
#else
BAD SYSTEM TYPE
#endif

#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UNICOS | UTS | OSF
#define DLIM_PPTFILEUNITS	QLM_BYTES
#define DLIM_PPTFILECOEFF	0L
#else
BAD SYSTEM TYPE
#endif

#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UNICOS | UTS | OSF
#define DLIM_PRTFILEUNITS	QLM_BYTES
#define DLIM_PRTFILECOEFF	0L
#else
BAD SYSTEM TYPE
#endif

#if	UNICOS
#define DLIM_PPMEMUNITS		QLM_MWORDS
#define DLIM_PPMEMCOEFF		256L
#else
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
#define DLIM_PPMEMUNITS		QLM_BYTES
#define DLIM_PPMEMCOEFF		0L
#else
BAD SYSTEM TYPE
#endif
#endif

#if	UNICOS
#define DLIM_PRMEMUNITS		QLM_MWORDS
#define DLIM_PRMEMCOEFF		256L
#else
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
#define DLIM_PRMEMUNITS		QLM_BYTES
#define DLIM_PRMEMCOEFF		0L
#else
BAD SYSTEM TYPE
#endif
#endif

#if	BSD42 | BSD43 | ULTRIX
#define DLIM_PPSTACKUNITS	QLM_MBYTES
#define DLIM_PPSTACKCOEFF	1L
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#define DLIM_PPSTACKUNITS	QLM_BYTES
#define DLIM_PPSTACKCOEFF	0L
#else
BAD SYSTEM TYPE
#endif
#endif

#if	BSD42 | BSD43 | ULTRIX
#define DLIM_PPWORKUNITS	QLM_MBYTES
#define DLIM_PPWORKCOEFF	1L
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#define DLIM_PPWORKUNITS	QLM_BYTES
#define DLIM_PPWORKCOEFF	0L
#else
BAD SYSTEM TYPE
#endif
#endif

#if	BSD42 | BSD43 | ULTRIX
#define DLIM_PPCPUSECS		36000
#define DLIM_PPCPUMS		0
#else
#if	UNICOS
#define DLIM_PPCPUSECS		720000
#define DLIM_PPCPUMS		0
#else
#if	SGI | SYS52 | UTS | OSF
#define DLIM_PPCPUSECS		0
#define DLIM_PPCPUMS		0
#else
BAD SYSTEM TYPE
#endif
#endif
#endif

#if	UNICOS
#define DLIM_PRCPUSECS		720000
#define DLIM_PRCPUMS		0
#else
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
#define DLIM_PRCPUSECS		0
#define DLIM_PRCPUMS		0
#else
BAD SYSTEM TYPE
#endif
#endif

#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UNICOS | UTS | OSF
#define DLIM_PPNICE		0
#else
BAD SYSTEM TYPE
#endif

#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UNICOS | UTS | OSF
#define DLIM_PRDRIVES		0
#else
BAD SYSTEM TYPE
#endif

#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UNICOS | UTS | OSF
#ifdef SDSC
#define DLIM_PRNCPUS		1
#else
#define DLIM_PRNCPUS		0
#endif
#else
BAD SYSTEM TYPE
#endif

