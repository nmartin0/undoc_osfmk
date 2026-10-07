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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/common_nx_c/lestat.c,v 1.2 1994/11/19 01:56:34 mtm Exp $
 *
 * Get the status of an extended file.  Don't follow links.
 *
 * HISTORY:
 * $Log: lestat.c,v $
 * Revision 1.2  1994/11/19  01:56:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/09/28  01:17:11  brad
 * Initial revision
 *
 * Revision 1.1  1993/06/10  18:35:54  brad
 * Initial revision
 *
 * Revision 1.3  1992/08/05  10:22:54  dbm
 * added History.
 */

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include "sys/estat.h"
#include "nx.h"

/*
 * lestat.c
 *
 * Description:
 *		This function is used to return the status of an
 *		extended file.  The status is returned in the 'buf'
 *		parameter. This function will return a zero on 
 *		success.  On failure, the error message will be 
 *		written to standard error and the calling process
 *		will be terminated with a non zero exit code.
 *	
 * Parameters:	
 *		path	File pathname.
 *		buf	Pointer to an estat structure in which the 
 *			status information is placed.
 *
 * Returns:
 *		0 if successful.
 */
long
lestat(path, buf)
char		*path;	/* File pathname. */
struct estat	*buf;	/* Returned estat structure. */
{
	long status;

	status = _lestat(path, buf);
	if (status == -1) {
		nx_perror("lestat");
		exit(1);
	}

	return(status);
}
