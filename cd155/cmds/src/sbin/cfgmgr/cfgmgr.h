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
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */

/*
 * @(#)$RCSfile: cfgmgr.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:12 $
 */

#define	CMGR_PID_PATHNAME	"/var/run/cfgmgr.pid"
/*
 *      CMGR:	Common global data
 */
typedef struct {
	char *		progname;       /* Program name */
	char *		database;       /* Database file name */
	int		pidfd;		/* Lock file id */
	int		maxrec;		/* Max size of record */
	int		maxatr;		/* Max number of attributes */
	int    		fflg;           /* True if option -f present */
	int    		dflg;           /* True if option -d present */
	int    		vflg;           /* True if option -v present */
	int		reset;		/* Reset cfgmgr */
	int		exitval;	/* Exit value */
} cmgr_common_t;

extern cmgr_common_t	CMGR;

