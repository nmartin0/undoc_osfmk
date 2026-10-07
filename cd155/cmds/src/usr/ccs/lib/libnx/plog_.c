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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance
 *	with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/plog_.c,v 1.3 1994/11/19 02:32:22 mtm Exp $
 *
 * FORTRAN interface to _plogon and _plogoff
 * 
 */


#include <stdio.h>

extern char *_plogon();


plogon_(append, cubeonly, inquiry, filename, length)
int append,cubeonly, inquiry;
char *filename;

{
	char newfilename[130];
	char *logname;

	_copyname(filename, newfilename, length, sizeof(newfilename));
	if ((logname=_plogon((short)append,(short)cubeonly,
			     (short)inquiry,newfilename)) == 0) {
		nx_perror("plogon");
		exit(1);
	} else {
		printf( "plogon: Currently logging to file %s\n", logname);
		free(logname);
	}

	return(0);
}


plogoff_()

{
	return(plogoff());
}
