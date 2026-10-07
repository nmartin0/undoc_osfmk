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
 
/*++ setpeertcm.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/setpeertcm.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Return TCMx_ code with peer origin bit set.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	May 21, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:51 $ $State: Exp $)
 * $Log: setpeertcm.c,v $
 * Revision 1.3  1994/11/19  02:27:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:57  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:59  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:55:40  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "informcc.h"		/* NQS information completion codes */

/*** setpeertcm
 *
 *
 *	long setpeertcm():
 *	Return TCMx_ code with peer origin bit set.
 */
long setpeertcm (code)
register long code;			/* Completion code */
{
	return (code | XCI_PEER_MASK);	/* Return TCMx_ code with peer bit */
}
