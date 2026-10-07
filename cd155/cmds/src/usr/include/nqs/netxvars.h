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
 
/*++ netxvars.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/netxvars.h,v $
 *
 * DESCRIPTION:
 *
 *
 *	NQS network daemon global external variable definitions file.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	December 23, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:43:59 $ $State: Exp $)
 * $Log: netxvars.h,v $
 * Revision 1.3  1994/11/19  02:43:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:17:45  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:42  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:44:03  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqsxdirs.h"			/* NQS global directory defintions */

extern char *Argv0;			/* Pointer to argv[0] string for */
					/* the NQS daemon */
extern int Argv0size;			/* Size of the argv[0] space for */
					/* the NQS daemon */
extern int Debug;			/* NQS debug level */
extern mid_t Locmid;			/* Machine-id of local host */
extern FILE *Logfile;			/* Log process stream */
extern struct confd *Devicefile;	/* Device descriptor file */
extern struct confd *Netqueuefile;	/* Network queue descriptor file */
extern struct confd *Qcomplexfile;	/* Queue complex descriptor file */
extern struct confd *Queuefile;		/* Non-network queue descriptor file */
extern struct confd *Qmapfile;		/* Queue-device mapping file */
extern struct confd *Pipeqfile;		/* Pipe queue destination file */
extern struct confd *Paramfile;		/* General parameters file */
extern struct confd *Mgrfile;		/* NQS manager access list file */
extern struct confd *Formsfile;		/* NQS forms list file */
