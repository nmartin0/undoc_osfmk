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
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_perror.c,v 1.3 1994/11/19 02:31:54 mtm Exp $
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

extern char *sys_errlist[];
extern int sys_nerr;


/***************************  nx_perror  ******************************
 *
 *      Description:
 *            Perror which identifies a process with node and ptype.
 *
 *      Calling Sequence:
 *            nx_perror(msg);
 *
 *      Parameters:
 *            msg:      Identification information
 *
 *      Returns:
 *            none
 *
 *      Called by:
 *            any
 */

void nx_perror(const char *msg)
{
        char    *err;

	err = (char *)malloc(80);

	if(errno >= 0 && errno < sys_nerr)
                err = sys_errlist[errno];
        else
                err = "Unknown error";

        fprintf(stderr, "(node %d, ptype %d) %s: %s\n",
                _mynode(), _myptype(), msg, err);
}


