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
 
/*++ nqsdirs.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nqsdirs.h,v $
 *
 * DESCRIPTION:
 *
 *	NQS daemon directory and file definitions.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 2, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:44:06 $ $State: Exp $)
 * $Log: nqsdirs.h,v $
 * Revision 1.3  1994/11/19  02:44:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:17:59  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:48  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:44:23  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqsdirs.h	1.4 (nqsdirs.h OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
#endif

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqsdirs.h	1.4 (nqsdirs.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
#endif

#if	BSD42 | BSD43
#else
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
char *Nqs_fifo = "FIFO";		/* NQS interprocess FIFO pipe */
					/* relative pathname */
char *Nqs_ffifo = "//usr/spool/nqs/new/requests/FIFO";
					/* NQS interprocess FIFO pipe */
					/* full pathname */
#else
BAD SYSTEM TYPE
#endif
#endif
char *Nqs_requests = "//usr/spool/nqs/new/requests";
					/* NQS new request directory */
char *Nqs_root = "//usr/spool/nqs/private/root";
					/* NQS root directory */
char *Nqs_scripts = "//usr/spool/nqs/scripts";
					/* NQS shell-script links directory */
/*
 *	All of the following directories/files are relative to Nqs_root.
 */
char *Nqs_control = "control";		/* NQS control directory */
char *Nqs_data = "data";		/* NQS data directory */
char *Nqs_devices = "database/devices";	/* NQS device status file */
char *Nqs_forms = "database/forms";	/* NQS forms file */
char *Nqs_netqueues = "database/netqueues";
					/* NQS network queue/status file */
char *Nqs_qcomplex = "database/qcomplex";
					/* NQS queue complex status file */
char *Nqs_queues = "database/queues";	/* NQS non-network queue/status file */
char *Nqs_qmaps = "database/qmaps";	/* NQS queue/device/destination file */
char *Nqs_pipeto = "database/pipeto";	/* NQS pipe destination queues */
char *Nqs_params = "database/params";	/* NQS general parameters file */
char *Nqs_mgracct = "database/managers";/* NQS manager account name file */
char *Nqs_seqno = "database/seqno";	/* NQS request seq# file */
char *Nqs_qaccess = "database_qa";	/* NQS queue access directory */
char *Nqs_qorder = "database_qo";	/* NQS queue ordering directory */
char *Nqs_failed = "failed";		/* NQS failed directory */
char *Nqs_inter = "interproc";		/* NQS inter-process comm. dir. */
char *Nqs_output = "output";		/* NQS temporary output file dir. */
char *Nqs_transact = "transact";	/* NQS transaction directory */
