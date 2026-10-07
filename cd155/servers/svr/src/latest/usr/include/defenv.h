/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: defenv.h,v $
 * Revision 1.3  1994/11/18  21:09:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:13:44  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:01  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:17  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:00  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:35:01  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:26:46  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:44:25  gm]
 * 
 * Revision 1.2  90/03/13  21:21:59  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:08:34  tom]
 * 
 * $EndLog$
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

