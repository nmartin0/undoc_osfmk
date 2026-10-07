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
 
#define PARAGON
/* #define DS */
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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/cbs/bootmagic.h,v 1.4 1994/11/21 16:40:02 mtm Exp $
 *
 *
 */

/*
**
** Global bootmesh variables
**
*/

/*
 * generic control variables
 */
extern	char 	*progname;
extern	int	debug;
extern  int     whitespace;             /* for whitespace in .._LIST values */

/*
 * buffers for bootmagic file
 */

#define	BOOTMAGIC_MAX	(2 * 4096)
extern	char 	*bootmagicfile;
extern	char 	*bootmagic;		/* external newline-terminated fmt */
extern	char 	*in_bootmagic;		/* internal null-terminated format */
extern  char    *whitespace_buf;        /* internal, lists with spaces */

char err[1024];			/* error message buffer */
#define ERR(str) \
	{ \
	printf("%s: Error: %s\n",progname,str); \
	return(-1); \
	}
#define WARN(str) \
	{ \
	if (warn) \
	printf("%s: Warning: %s\n", progname,str); \
	}


