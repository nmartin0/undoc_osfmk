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
/* @(#)nl_types.h       1.19  com/inc,3.1,9013 3/4/90 19:29:35 */
/*
 * COMPONENT_NAME: INC
 *
 * FUNCTIONS: nl_types.h
 *
 * ORIGINS: 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 */

#ifndef _NL_TYPES_H_
#define _NL_TYPES_H_

#include <standards.h>

#ifdef _XOPEN_SOURCE
typedef int nl_item;
typedef struct catalog_descriptor *nl_catd;

#define NL_SETD 	1  				

#ifdef _NO_PROTO
extern nl_catd catopen();
extern char  *catgets();
extern int catclose();
#else
extern nl_catd catopen(char *, int );
extern char  *catgets(nl_catd , int , int , char *);
extern int catclose(nl_catd );
#endif /* _NO_PROTO */
#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE
typedef struct catalog_descriptor CATD;

#ifdef	_NO_PROTO
nl_catd NLcatopen();
char *	NLgetamsg();
#else
nl_catd NLcatopen(char *, int);
extern char *NLgetamsg(char*, int, int, char*);
#endif	/* _NO_PROTO */


#ifdef _CAT_MACRO
#ifdef _NO_PROTO
char *fcatgets();
char *NLfcatgets();
char *fcatgetmsg();
#else
extern char   *fcatgets(nl_catd,int,int,char*);
extern char *NLfcatgets(nl_catd,int,int,char*);
extern char *fcatgetmsg(nl_catd,int,int,char*,int);
#endif	/* _NOPROTO */
#else
#ifdef _NO_PROTO
char *catgets();
char *NLcatgets();
char *catgetmsg();
#else
extern char   *catgets(nl_catd, int, int, char *);
extern char *NLcatgets(nl_catd, int, int, char *);
extern char *catgetmsg(nl_catd, int, int, char*, int);
#endif
#endif

#include <mesg.h>
#endif /* _OSF_SOURCE */

#endif /* _NL_TYPES_H_ */
