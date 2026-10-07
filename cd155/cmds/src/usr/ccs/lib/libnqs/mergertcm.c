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
 
/*++ mergertcm.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/mergertcm.c,v $
 *
 * DESCRIPTION:
 *
 *	Merge a TCMx_ code with an RCM_ code.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 28, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:15 $ $State: Exp $)
 * $Log: mergertcm.c,v $
 * Revision 1.3  1994/11/19  02:27:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:18  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:11  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:51:29  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "informcc.h"			/* Completion code information bits */

/*** mergertcm
 *
 *
 *	long mergertcm():
 *	Merge TCMx_ code with RCM_code.
 */
long mergertcm (rcm_original, tcm_original)
long rcm_original;
long tcm_original;
{
	return (rcm_original | ((tcm_original & XCI_FULREA_MASK) << 10));
}
