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
 
/*++ telldb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/telldb.c,v $
 *
 * DESCRIPTION:
 *
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:28:01 $ $State: Exp $)
 * $Log: telldb.c,v $
 * Revision 1.3  1994/11/19  02:28:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  21:59:58  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:15  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:56:46  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"

/*** telldb
 *
 *
 *	long telldb():
 *
 *	Returns:
 *		The file offset position of the last descriptor
 *		read from the specified NQS database/configuration
 *		file.  A value of -1 indicates that no readdb()
 *		call has ever been performed on the specified file.
 */
long telldb (file)
register struct confd *file;
{
	return (file->lastread);
}
