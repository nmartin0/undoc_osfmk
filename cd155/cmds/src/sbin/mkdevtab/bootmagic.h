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
**
** Global bootmesh variables
**
*/

/*
 * buffers for bootmagic file
 */
#define	BOOTMAGIC_MAX	(2 * 4096)
extern	char 	*bootmagicfile;
extern	char 	*bootmagic;		/* external newline-terminated format */
extern	char 	*in_bootmagic;		/* internal null-terminated format */


char err[1024];				/* error message buffer */
extern char *progname;
#define ERR(str) \
	{ \
	printf("%s: Error: %s\n",progname,str); \
	return(-1); \
	}
