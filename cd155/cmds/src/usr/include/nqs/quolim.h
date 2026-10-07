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
 
/*++ quolim.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/quolim.h,v $
 *
 * DESCRIPTION:
 *
 *
 *	Quota structure definitions file.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:44:16 $ $State: Exp $)
 * $Log: quolim.h,v $
 * Revision 1.3  1994/11/19  02:44:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:18:21  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:58  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:45:12  hender
 * Sterling version 4/22/87
 * 
 *
 */

/*
 *	Quota limit units:
 */
#define	QLM_BYTES	011	/* Resource limit in bytes */
#define	QLM_WORDS	012	/* Resource limit in words */
#define	QLM_KBYTES	021	/* Resource limit in kilobytes */
#define	QLM_KWORDS	022	/* Resource limit in kilowords */
#define	QLM_MBYTES	031	/* Resource limit in megabytes */
#define	QLM_MWORDS	032	/* Resource limit in megawords */
#define	QLM_GBYTES	041	/* Resource limit in gigabytes */
#define	QLM_GWORDS	042	/* resource limit in gigawords */


/*
 *	Quota structures:
 */
struct cpulimit {
	unsigned long max_seconds;	/* Maximum CPU time in seconds */
	unsigned long warn_seconds;	/* Warning CPU time in seconds */
	short max_ms;			/* + max_ms (milliseconds) */
	short warn_ms;			/* + warn_ms (milliseconds) */
};


struct quotalimit {
	unsigned long max_quota;	/* Max quota limit measured in */
	unsigned long warn_quota;	/* Warn quota limit measured in */
	short max_units;		/* units of type: max_units */
	short warn_units;		/* units of type: warn_units */
};
