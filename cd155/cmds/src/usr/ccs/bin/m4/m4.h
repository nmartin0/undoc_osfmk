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
/*   @(#)$RCSfile: m4.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:54:02 $ */
/*
 * COMPONENT_NAME: (CMDAS) Assembler and Macroprocessor 
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 3, 27
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
 */
/* m4.h	1.7  com/cmd/as/m4,3.1,9013 12/16/89 17:51:06 */
#include	<ctype.h>
#include	"m4_msg.h"

#define EOS	'\0'
#define LOW7	0177
#define MAXSYM	5
#define PUSH	1
#define NOPUSH	0
#define OK	0
#define NOT_OK	1

#define	putbak(c)	(ip < ibuflm? (*ip++=(c)): error2(PBMB,pbmsg,bufsize))
#define	stkchr(c)	(op < obuflm? (*op++=(c)): error2(AUGT,aofmsg,bufsize))
#define sputchr(c,f)	(putc(c,f)=='\n'? lnsync(f): 0)

#ifdef   XASLINE
#define putchr(c)	(Cp? stkchr(c): cf? (sflag?sputchr(c,cf):\
			(xsflag? sputchr(c,cf): putc(c,cf))): 0)
#else   XASLINE
#define putchr(c)	(Cp?stkchr(c):cf?(sflag?sputchr(c,cf):putc(c,cf)):0)
#endif  XASLINE

struct bs {
	int	(*bfunc)();
	char	*bname;
};

struct	call {
	char	**argp;
	int	plev;
};

struct	nlist {
	char	*name;
	char	*def;
	char	tflag;
	struct	nlist *next;
};

extern FILE	*cf;
extern FILE	*ifile[];
extern FILE	*ofile[];
extern FILE	*xfopen();
extern char	**Ap;
extern char	**argstk;
extern char	*Wrapstr;
extern char	*astklm;
extern char	*inpmatch();
extern char	*chkbltin();
extern char	*calloc();
extern char	*xcalloc();
extern char	*copy();
extern char	*mktemp();
extern char	*strcpy();
extern char	*fname[];
extern char	*ibuf;
extern char	*ibuflm;
extern char	*ip;
extern char	*ipflr;
extern char	*ipstk[10];
extern char	*obuf;
extern char	*obuflm;
extern char	*op;
extern char	*procnam;
extern char	*tempfile;
extern char	*token;
extern char	*toklm;

/* This define fixes a collision between m4 and the setlocale() stuff */
#define C	_C_

extern int	C;
extern int	getchr();
extern char	aofmsg[];
extern char	astkof[];
extern char	badfile[];
extern char	fnbuf[];
extern char	lcom[];
extern char	lquote[];
extern char	nocore[];
extern char	nullstr[];
extern char	pbmsg[];
extern char	rcom[];
extern char	rquote[];
extern char	type[];
extern int	bufsize;
extern int	fline[];
extern int	hshsize;
extern int	hshval;
extern int	ifx;
#ifdef XASLINE
extern int	xsflag;
#endif XASLINE
extern int	nflag;
extern int	ofx;
extern int	sflag;
extern int	stksize;
extern int	sysrval;
extern int	toksize;
extern int	trace;
extern long	ctol();
extern struct bs	barray[];
extern struct call	*Cp;
extern struct call	*callst;
extern struct nlist	**hshtab;
extern struct nlist	*install();
extern struct nlist	*lookup();
