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
 * $Log: nl_types.h,v $
 * Revision 1.3  1994/11/18  21:10:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:20  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:49  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:24  nandy
 * Initial revision
 *
 * Revision 1.4  92/04/13  14:38:16  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.4  90/10/07  20:32:56  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:53:35  gm]
 * 
 * Revision 1.3  90/04/27  23:07:05  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/16  13:48:49  tom]
 * 
 * Revision 1.2  90/03/13  21:23:13  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:25:34  tom]
 * 
 * $EndLog$
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
