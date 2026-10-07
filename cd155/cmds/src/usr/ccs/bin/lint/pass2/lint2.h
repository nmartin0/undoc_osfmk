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
/* @(#)lint2.h	1.2  com/cmd/prog/lint/pass2,3.1,9013 9/12/89 11:51:01 */
/*
 * COMPONENT_NAME: (CMDPROG) Programming Utilites
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 00 03 10 27 32
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/* Basic symbol table entry. */
typedef struct smtbl {
	char	*sname;		/* symbol name */
	TPTR	type;		/* type information */
	struct {
		char	*pfname;/* physical filename */
		char	*ifname;/* included filename */
		short	line;	/* line number occurance */
	} rd[2];
	short	usage;		/* usage flags */
	short	nmbrs;		/* number of member symbols */
	struct mbtbl *mbrs;	/* member symbols */
} SMTAB;

#define RUSE	0		/* reference usage */
#define DUSE	1		/* definition usage */
#define CRUSE	2		/* current reference usage */
#define CDUSE	3		/* current definition usage */
#define rpf	rd[RUSE].pfname
#define rif	rd[RUSE].ifname
#define rl	rd[RUSE].line
#define dpf	rd[DUSE].pfname
#define dif	rd[DUSE].ifname
#define dl	rd[DUSE].line

/* Member symbol table extension. */
typedef struct mbtbl {
	char	*mname;		/* member name */
	TPTR	type;		/* type information */
	char	*tagname;	/* struct tag name */
	struct mbtbl *next;	/* next member */
} MBTAB;

/* Allocation constants. */
#define MAXHASH	20		/* number of hash buckets */
#define HASHBLK	1013		/* numbers of symbols per hash bucket */
#define NAMEBLK	1024		/* string table allocation size */
#define MBRBLK	128		/* member symbol table allocation size */

/* Function code directives. */
#define LOOK	1		/* CheckSymbol() */
#define STORE	2
#define CHANGE	3
#define REJECT	4

#define GETNAME	1		/* GetName() */
#define GETMISC	2

/* Global symbols. */
SMTAB theSym;			/* current play symbol */
SMTAB *prevSym;			/* previous symbol */
SMTAB *curSym;			/* current symbol */
char *curPFname;		/* current cpp physical filename */
char *curIFname;		/* current cpp include filename */
char *prevIFname;		/* previous cpp include filename */
short curDLine;			/* current definition line number */
short curRLine;			/* current reference line number */
char sbuf[BUFSIZ];		/* temporary symbol name space */
char ibuf[BUFSIZ];		/* temporary file name space */
char *fname;			/* actual file name opened */
int markerEOF;			/* end of file marker */
int pflag;			/* extreme portability flag */
int debug;			/* debug mode status flag */

/* Function declarations. */
SMTAB *FindSymbol();
char *StoreSName();
char *StoreMName();
SMTAB *LookupSymbol();
MBTAB *MBMalloc();

char *GetName();
TPTR InType();
