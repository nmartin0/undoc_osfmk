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
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * File:	mactalk.c
 *
 * Abstract:	This file contains initialization and exit routines
 *		for a communication session with MACD.
 */

#include <stdio.h>
#include <sys/types.h>
#include "mac.h"

void macinit();
void macexit();
void is_macwatch();

/*================================================================
 *
 * macinit()
 *
 * Abstract:	This routine establishes a connection with MACD.
 *
 * Arguments:	None
 *
 * Return value:	None
 *
 * Note:
 *
 ==============================================================*/
void macinit()
{
	int talkerr;
#ifndef IPSC
	if ((talkerr=seteuid(0))==-1) {
		fprintf(stderr,"\n Error acquiring sufficient privileges \n");
		macexit();
	}
#endif
       
	if ((talkerr=cpuctl(C_OPEN,0))==-1) {
		fprintf(stderr,"\n Error opening connection to MACD\n"); 
		macexit();
	}	
}

/*================================================================
 *
 * macexit()
 *
 * Abstract:	This routine closes a connection with MACD and exit.
 *
 * Arguments:	None
 *
 * Return value:	None
 *
 * Note:
 *
 ==============================================================*/
void macexit()
{
	int talkerr;
	if((talkerr=cpuctl(C_CLOSE,0))==-1) {
		fprintf(stderr,"\n Error closing connection to MACD\n");
	}
#ifndef IPSC
	if((talkerr=seteuid(getuid())) == -1) {
		fprintf(stderr,"\n Error restoring privileges \n\n");
		macexit();
	}
#endif
	exit(0);
}

/*================================================================
 *
 * is_macwatch()
 *
 * Abstract:	Determines whether or not MACS is in "macwatch" mode.
 *              If so, simply returns.  If not (or if the MACD cannot be
 *              contacted for some reason), prints an error message
 *              and calls exit().
 *
 * Arguments:	None
 *
 * Return value:	None
 *
 * Note:
 *
 ==============================================================*/
void is_macwatch()
{
	macinit();

	if (cpuctl(C_ISMACWATCH, 0) != 0) {
		macerr();
		(void) fprintf(stderr, "\n");
		exit(1);
	}

	(void) c_close();
}

