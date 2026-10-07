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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/parsesched.c,v 1.6 1995/01/05 00:45:46 sdh Exp $
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/allocator.h>
#include <nx/bitmap.h>

#define	MAX_RQ		(24 * 60 * 60 * 1000) /* 24 hours in milliseconds */

/********************************  parse_rq ************************
 *
 *      Calling Sequence:
 *             parse_rq(rq,rollin_q);
 *
 *      Description:
 *            calculates rollin quantum from the argument to -rq.
 *
 *      Parameters:
 *            char pointer to argument to rq
 *            integer pointer to argument rollin_q
 *      
 *      Returns:
 *               0 == all is ok
 *		-1 == error condition, errno is set
 *
 *
 */

int
parse_rq(char *rq,unsigned long  *rollin_q)
{
	char	*tmpptr;	/* Temporary pointer */
	int	in_millisecs;

	/*
	 * First check that the string has valid characters only
	 */

	tmpptr = rq;

	while( *tmpptr != '\0' ) {
		if(( *tmpptr < '0' || *tmpptr > '9') && (*tmpptr != 's')  &&
			(*tmpptr != 'm') && (*tmpptr != 'h') ) {
			errno = EPINVALSCHED;
			return(-1);
		}
		tmpptr++;
	}

	in_millisecs = 0;
	if(( tmpptr = strpbrk(rq,"smh")) != NULL ) {

		switch(*tmpptr) {

			case 's':
				*tmpptr = '\0';
				*rollin_q = atoi(rq) * 1000;
				break;

			case 'h':
				*tmpptr = '\0';
				*rollin_q = atoi(rq) * 3600000;
				break;

			case 'm':
				/* Can be minutes (m) or milliseconds (ms) */
				*tmpptr = '\0';
				if (*(tmpptr+1) == 's') {
					*rollin_q = atoi(rq);
					in_millisecs = 1;
				} else {
					*rollin_q = atoi(rq) * 60000;
				}
				break;
			default:
				errno = EPINVALSCHED;
				return(-1);
		}

	} else {
		*rollin_q = atoi(rq);
		in_millisecs = 1;
	}

	if (in_millisecs == 1) {
		/*
		 * Round up rollin quantum to the next higher multiple of 100
		 */
		if (*rollin_q != 0) {
			*rollin_q = ((*rollin_q + 99) / 100) * 100;
		}
	}
	return(0);
}
