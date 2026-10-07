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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_pri.c,v 1.9 1994/11/19 02:32:00 mtm Exp $
 *
 */


#include <sys/types.h>
#include <errno.h>
#include <nx/defines.h>

int
nx_pri( pgroup, priority )
pid_t	pgroup;
int priority;
{
	if ( (priority < MINPRI) || (priority > MAXPRI)){
		errno = EPINVALPRI;
		return -1;
	}
	else{
		if (pgroup == 0) 
			return (syscall(314, getpgrp(), priority));
		else
		if (pgroup > 0)
			return (syscall(314, pgroup, priority));
		else{
			errno = EANOEXIST;
			return -1;
		}
	}
}
