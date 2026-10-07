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
 
/* Function prototypes for nx_getopt.c
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/nx_getopt.h,v 1.2 1994/11/19 02:44:48 mtm Exp $
*/

/* History
 * $Log: nx_getopt.h,v $
 * Revision 1.2  1994/11/19  02:44:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/10/08  19:32:49  carbajal
 * New .h files for function prototypes
 *
*/

/* External data declarations
/* External data declarations 
*/
extern int	nx_opterr;
extern int	nx_optind;
extern char	*nx_optopt;
extern char	*nx_optarg;

extern char *nx_getopt(int argc,char **argv, char *opts);

