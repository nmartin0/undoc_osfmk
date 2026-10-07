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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libesize/ecmp_.c,v 1.2 1994/11/19 02:20:02 mtm Exp $
 *
 * FORTRAN interface for extended numbers compare.
 *
 *      HISTORY:
 *      $Log: ecmp_.c,v $
 * Revision 1.2  1994/11/19  02:20:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/09/28  00:52:06  brad
 * Initial revision
 *
 * Revision 1.2  1992/08/05  09:44:42  dbm
 * Added History.
 *
 *
 */

#include <errno.h>
#include <sys/types.h>
#include "nx.h"

long
ecmp_(e1, e2)
esize_t	*e1;
esize_t	*e2;
{
	return (_ecmp(*e1, *e2));
}
