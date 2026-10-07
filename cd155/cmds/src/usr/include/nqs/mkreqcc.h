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
 
/*++ mkreqcc.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/mkreqcc.h,v $
 *
 * DESCRIPTION:
 *
 *	Mkreq completion code definitions.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:43:57 $ $State: Exp $)
 * $Log: mkreqcc.h,v $
 * Revision 1.3  1994/11/19  02:43:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:17:38  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:38  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:42:45  hender
 * Sterling version 4/22/87
 * 
 *
 */

#define	MKREQ_SUCCESS	 0	/* Control file successfully created */
#define	MKREQ_NOCREATE	 (-1)	/* Unable to open/create control file */
#define	MKREQ_NOMID	 (-2)	/* Unable to get machine-id of local host */
#define	MKREQ_BADREQTYPE (-3)	/* Bad request type specified */
#define	MKREQ_NOUSERNAME (-4)	/* Could not get username for user-id */
#define	MKREQ_NOCWD	 (-5)	/* Unable to get current working directory */
#define	MKREQ_CWDNEWLINE (-6)	/* Current working directory contained */
				/* a new line character */
#define	MKREQ_NOCHDIRNEW (-7)	/* Unable to chdir() to the NQS new request */
				/* directory */
#define	MKREQ_NOLOCALDAE (-8)	/* Unable to get pipe to local daemon */
#define	MKREQ_NOSETUGID	 (-9)	/* Unable to set [uid,gid] */
#define	MKREQ_NOPARMFILE (-10)	/* Unable to open NQS parameters file */
#define MKREQ_INVALIDACC (-11)	/* Invalid account ID */
