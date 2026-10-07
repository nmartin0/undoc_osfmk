#define PARAGON
/* #define DS */
/*
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/stack/bootmagic.h,v 1.1.8.1 1995/06/11 18:42:28 kat Exp $
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


