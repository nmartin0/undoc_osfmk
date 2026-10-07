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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
/* @(#)$RCSfile: sed.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:42 $ */
/*
 * COMPONENT_NAME: (CMDEDIT) sed.h
 *
 * FUNCTIONS: none
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1984, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright 1975 Bell Telephone Laboratories, Incorporated
 *
 * sed.h   1.4  com/cmd/edit,3.1.2,9038312 8/21/90 08:32:19
 */


#include <NLchar.h>
#ifdef lint
#define NLsgetc(s)      (*(s)++)
#define NLsputc(s, c)   (*(s)++ = (c))
#else
#define NLsgetc(s)      (_NCis2((s)[0], (s)[1]) ? (s)+=2, _NCd2((s)[-2], \
			 (s)[-1]) : (*(s)++ & 0xff))
#ifdef KJI
#define NLsputc(s, c)   ((c) <= 0xff ? (*(s)++ = (c)) : \
			 (*(s)++ = (c)>>8, *(s)++ = (c)))
#else
#define NLsputc(s, c)   ((c) <= 0xff ? (*(s)++ = (c)) : \
			 (*(s)++ = 0x21 - ((c)>>7), *(s)++ = (c)|0x80))
#endif
#endif

/*
 * define some macros for rexexp.h
 */

#define INIT    extern char *cp, *badp; /* cp points to RE string */\
		register char *sp = cp;
#define GETC()          (*sp++)
#define PEEKC()         (*sp)
#define UNGETC(c)       (--sp)
#define RETURN(c)       cp = sp; return((char *)ep);
#define ERROR(c)        { cp = sp; return(badp); }

#define CEND    16
#define CLNUM   14

#define NLINES  256
#define DEPTH   20
#define PTRSIZE 100
			/* #define RESIZE  5000 */
#define RESIZE  25000   /* NLS and KJI regexp buffer needs to be large */
			/* to accommodate the greater range of characters */

#define ABUFSIZE        20
#define LBSIZE  4000
#define ESIZE   256
#define LABSIZE 50
#define GLOBAL_SUB -1	/* global substitution */

extern union reptr     *abuf[];
extern union reptr **aptr;
extern char    genbuf[];
extern char    *lbend;
extern char     *lcomend;
extern char    *hend;
extern long    lnum;
extern char    linebuf[];
extern char    holdsp[];
extern char    *spend;
extern char    *hspend;
extern int     nflag;
extern long    tlno[];

#define ACOM    01
#define BCOM    020
#define CCOM    02
#define CDCOM   025
#define CNCOM   022
#define COCOM   017
#define CPCOM   023
#define DCOM    03
#define ECOM    015
#define EQCOM   013
#define FCOM    016
#define GCOM    027
#define CGCOM   030
#define HCOM    031
#define CHCOM   032
#define ICOM    04
#define LCOM    05
#define NCOM    012
#define PCOM    010
#define QCOM    011
#define RCOM    06
#define SCOM    07
#define TCOM    021
#define WCOM    014
#define CWCOM   024
#define YCOM    026
#define XCOM    033


union   reptr {
	struct reptr1 {
		char    *ad1;
		char    *ad2;
		char    *re1;
		NLchar    *rhs;
		FILE    *fcode;
		short   gfl;
		char    command;
		char    pfl;
		char    inar;
		char    negfl;
	} r1;
	struct reptr2 {
		char    *ad1;
		char    *ad2;
		union reptr     *lb1;
		NLchar    *rhs;
		FILE    *fcode;
		char    command;
		char    gfl;
		char    pfl;
		char    inar;
		char    negfl;
	} r2;
};
extern union reptr ptrspace[];



struct label {
	char    asc[9];
	union reptr     *chain;
	union reptr     *address;
};



extern int     eargc;

extern union reptr     *pending;
extern char    *badp;
char    *compile();
/*** char    *ycomp(); ***/
char    *address();
char    *text();
char    *compsub();
struct label    *search();
char    *gline();
char    *place();

