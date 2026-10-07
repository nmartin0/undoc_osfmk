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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: getopt.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:47 $";
#endif

/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: getopt
 *
 * ORIGINS: 3 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * sccsid[] = "getopt.c  1.9  com/lib/c/gen,3.1,9021 2/16/90 16:14:26";
 */

#include <stdio.h>
#include <string.h>

#ifdef MSG
#include "libc_msg.h"
#endif

#define ERR(s, c)	if(opterr){\
	(void) fputs(argv[0], stderr);\
	(void) fputs(s, stderr);\
	(void) fputc(c, stderr);\
	(void) fputc('\n', stderr);}

int	opterr = 1;		/* print errors?			*/
int	optind = 1;		/* index of next argv			*/
int	optopt;			/* current option we're looking at	*/
char	*optarg;		/* argument for current option		*/


/*
 * NAME:	getopt
 *                                                                    
 * FUNCTION:	get flag letters from the argument vector
 *                                                                    
 * NOTES:	Getopt scans the command line looking for specified flags.
 *		The flags may or may not require arguments.
 *
 * RETURN VALUE DESCRIPTION:	returns '?' on an error (unrecognized flag or
 *		a recognized flag requiring an argument and no argument given),
 *		EOF on successful completion of the scan of the command line,
 *		else the recognized flag just encountered
 */  

int
getopt(argc, argv, optstring)
int	argc;			/* number of command line arguments	*/
char	**argv;			/* pointer to command line arguments	*/
char	*optstring;		/* string describing valid flags	*/
{
	static int sp = 1;
	int c;
	char *cp;

	if(sp == 1)
		if(optind >= argc ||
		   argv[optind][0] != '-' || argv[optind][1] == '\0')
			return(EOF);
		else if(strcmp(argv[optind], "--") == NULL) {
			optind++;
			return(EOF);
		}
	optopt = c = argv[optind][sp];
	if(c == ':' || (cp=strchr(optstring, c)) == NULL) {
#ifdef MSG
		ERR(NLgetamsg (MF_LIBC, MS_LIBC, M_OPTILL,
		    ": illegal option -- "), c);
#else
		ERR(": illegal option -- ", c);
#endif
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
#ifdef MSG
			ERR(NLgetamsg (MF_LIBC, MS_LIBC, M_OPTARG,
			    ": option requires an argument -- "), c);
#else
			ERR(": option requires an argument -- ", c);
#endif
			sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}
