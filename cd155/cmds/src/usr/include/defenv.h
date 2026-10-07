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
/* @(#)defenv.h	1.7  com/inc,3.1,8943 9/25/89 19:09:33 */
#ifndef _DEFENV_H_
#define _DEFENV_H_

/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

/* Default environment definitions */

#define DEFCTAB	"/etc/nls/ctab/default"
#define NDATVAR	"NLDATE"
#define NFILVAR	"NLFILE"
#define NLDTVAR "NLLDATE"
#define NTIMVAR "NLTIME"
#define NLCTVAR "NLCTAB"

/*  Table of default parameter values
 */
typedef struct nppair {
	char *np_name;
	char *np_val;
	} NPPAIR;

extern NPPAIR deftable[]; /* defined in NLgetenv() */
extern int deftsize;      /* defined in NLgetenv() */

struct envpair {
	char *name;			/* Name of param */
	char *def;			/* Value of param */
	char parsed;			/* Bool:  value parsed? */
	struct envpair *next;		/* Next param struct */
};
extern struct envpair *__paramtab; 

extern char *__filbuf; /* NLgetfile(), NLgetenv() and setlocale()  */
extern char *__bufp;   /* are using __filbuf, __bufp and __ovflg   */
extern char __ovflg;   /* They are all defined in NLgetenv()       */

#endif /* _DEFENV_H_ */

