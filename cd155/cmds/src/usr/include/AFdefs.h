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
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1985, 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */

/*
 * NAME:	AFdefs.h
 * FUNCTION:	Definitions to be included by programs that use "Attribute 
 *		Files".  
 * NOTES:	<stdio.h> must be included before this include file.
 */

#ifndef _AFDEFS_H
#define	_AFDEFS_H

#define	AF_OK 		000		/* No input record parse errors */
#define	AF_SYNTAX	001		/* Input record parse syntax error */
#define	AF_ERRCBUF	002		/* Input record exceeds AF_rsiz */
#define	AF_ERRCATR	004		/* Input record attr exceeds AF_natr */

struct ATTR {       
	char *  	AT_name;	/* Attribute name */
	char *  	AT_value;	/* Attribute value list */
	char *  	AT_nvalue;	/* Attribute value next in list */
};
typedef struct ATTR *	ATTR_t;

struct ENT {       
	char *  	EN_name;	/* Entry object name */
	char *  	EN_cbuf;	/* Entry input buffer */
	ATTR_t  	EN_catr;	/* Entry attribute list */
        ATTR_t  	EN_natr;	/* Entry next attribute in list */
};
typedef struct ENT *	ENT_t;

struct AFILE {       
	FILE *  	AF_iop;		/* File pointer */
	int     	AF_maxsiz;	/* Max input buffer size (in bytes) */
	int     	AF_maxatr;	/* Max attr list size (in elements) */
	int     	AF_errs;	/* Record input errors */
	char *		AF_dflt;	/* Default name */
	struct ENT	AF_cent;	/* Current entry structure */
	struct ENT	AF_dent;	/* Default entry structure */
};
typedef struct AFILE *	AFILE_t;


#define AFentname(x)    ( (x)->EN_name )
#define AFatrname(x)    ( (x)->AT_name )

#ifdef _NO_PROTO

extern AFILE_t	AFopen();
extern void	AFclose();
extern void	AFrewind();
extern void	AFsetdflt();
extern ENT_t	AFgetent();
extern ENT_t	AFnxtent();
extern ATTR_t	AFgetatr();
extern ATTR_t	AFnxtatr();
extern char *	AFgetval();
extern char *	AFnxtval();

#else /* _NO_PROTO */

extern AFILE_t	AFopen(char * filename, int maxrecsiz, int maxnumatr);
extern void	AFclose(AFILE_t af);
extern void	AFrewind(AFILE_t af);
extern void	AFsetdflt(AFILE_t af, char * dflt);
extern ENT_t	AFgetent(AFILE_t af, char * name);
extern ENT_t	AFnxtent(AFILE_t af);
extern ATTR_t	AFgetatr(ENT_t entry, char * name);
extern ATTR_t	AFnxtatr(ENT_t entry);
extern char *	AFgetval(ATTR_t attribute);
extern char *	AFnxtval(ATTR_t attribute);
#endif	/* _NO_PROTO */

#endif	/* _AFDEFS_H */
