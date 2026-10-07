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
 
/*++ nqsxdirs.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nqsxdirs.h,v $
 *
 * DESCRIPTION:
 *
 *	NQS daemon external directory and file definitions.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:44:15 $ $State: Exp $)
 * $Log: nqsxdirs.h,v $
 * Revision 1.3  1994/11/19  02:44:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:18:17  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:56  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:45:03  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)nqsxdirs.h	1.3 (nqsxdirs.h OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#if	BSD42 | BSD43
#else
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
extern char *Nqs_fifo;			/* NQS interprocess FIFO pipe */
					/* relative pathname */
extern char *Nqs_ffifo;			/* NQS interprocess FIFO pipe */
					/* full pathname */
#else
BAD SYSTEM TYPE
#endif
#endif
extern char *Nqs_requests;		/* NQS new request directory */
extern char *Nqs_root;			/* NQS root directory */
extern char *Nqs_scripts;		/* NQS shell script links directory */
/*
 *	All of the following directories/files are relative to Nqs_root.
 */
extern char *Nqs_control;		/* NQS control directory */
extern char *Nqs_data;			/* NQS data directory */
extern char *Nqs_devices;		/* NQS device status file */
extern char *Nqs_forms;			/* NQS forms file */
extern char *Nqs_netqueues;		/* NQS network queue/status file */
extern char *Nqs_qcomplex;		/* NQS queue complex status file */
extern char *Nqs_queues;		/* NQS non-network queue/status file */
extern char *Nqs_qmaps;			/* NQS queue/device/destination file */
extern char *Nqs_pipeto;		/* NQS pipe destination queues */
extern char *Nqs_params;		/* NQS general parameters file */
extern char *Nqs_mgracct;		/* NQS manager account name file */
extern char *Nqs_seqno;			/* NQS request seq# file */
extern char *Nqs_qaccess;		/* NQS queue access directory */
extern char *Nqs_qorder;		/* NQS queue ordering directory */
extern char *Nqs_failed;		/* NQS failed directory */
extern char *Nqs_inter;			/* NQS inter-process comm. dir. */
extern char *Nqs_output;		/* NQS temporary output file dir */
extern char *Nqs_transact;		/* NQS transaction directory */
