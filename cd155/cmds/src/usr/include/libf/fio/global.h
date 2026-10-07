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
 *	Copyright 1991, The Portland Group, Incorporated.
 *	All rights reserved.
 *
 *	THE PORTLAND GROUP, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with The Portland Group and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

/* global.h - Global definitions and declarations for pgftn I/O library */

#include "pgstdio.h"	/* stubbed version of stdio.h */
#include "stdinit.h"	/* declarations for stdio units */
#include "cnfg.h"	/* declarations for configuration items */

#ifndef LOCAL_DEBUG
#define LOCAL_DEBUG 0
#endif

typedef unsigned short WCHAR;

/*  declare some external library functions required:  */

#define VOID	void

extern char	*malloc(), *realloc(), *getenv();
extern VOID	free();
extern long	strtol();

/*  special versions needed for these functions:  */
#define ecvt __ecvt
#define fcvt __fcvt
#define strtod __strtod
extern char  *fcvt();
extern char  *ecvt();
extern double strtod();

#define feof(f) fio_feof(f)
#define ferror(f) fio_ferror(f)

typedef long	INT;		/* native integer at least 32 bits */
typedef unsigned long UINT;	/* unsigned 32 bit native integer */
#define isupper(cvar) ((cvar) >= 'A' && (cvar) <= 'Z')
#define ISDIGIT(c)	((c) >= '0' && (c) <= '9')

/*
 * Because of bugs in AT&T SysV R4 fwrite, it is necessary to use
 * a special version of fwrite for line-buffered files.  This
 * is defined in the makefile as BROKEN_FWRITE.
 */
#ifdef BROKEN_FWRITE
#define FWRITE __pgi_fwrite
#else
#define FWRITE fwrite
#endif

#define TRUE 1
#define FALSE 0
typedef int bool;
typedef char sbool;	/* short boolean (for use in large structs) */

/*  true and false as represented in Fortran program at runtime:  */
#define FTN_TRUE	-1
#define FTN_FALSE	0

#if LOCAL_DEBUG
#define assert(ex) {if (!(ex)) {(VOID)fprintf(__fio_stdio.err, \
	"Fio-assertion failed: file \"%s\", line %d\n", __FILE__, __LINE__);}}
#else
#define assert(ex)
#endif

#define STASH(str) ((char *) strcpy(malloc(strlen(str)+1), str) )

#define ERR_FLAG 1
#define EOF_FLAG 2

/* maximum filename length in bytes: */
#define MAX_NAMELEN 255


/* Fortran I/O error code definitions: */

#define FIO_ERROR_OFFSET 200	/* smallest error value */
		     /* 200 */
#define FIO_ESPEC	201
#define FIO_ECOMPAT	202
#define FIO_ERECLEN	203
#define FIO_EREADONLY	204
#define FIO_EDISPOSE	205
#define FIO_ESCRATCH	206
#define FIO_EOPENED	207
#define FIO_EEXIST	208
#define FIO_ENOEXIST	209
#define FIO_ENOMEM	210
#define FIO_ENAME	211
#define FIO_EUNIT	212
		     /* 213 */
		     /* 214 */
#define FIO_EFORM	215
		     /* 216 */
#define FIO_EEOF	217
		     /* 218 */
#define FIO_ETOOBIG	219
#define FIO_ETOOFAR	220
#define FIO_EFSYNTAX	221
#define FIO_EPAREN	222
#define FIO_EPT		223
#define FIO_ESTRING	224
#define FIO_ELEX	225
#define FIO_ELETTER	226
		     /* 227 */
#define FIO_ENOGROUP	228
#define FIO_ENMLEOF	229
#define FIO_ESCALEF	230
#define FIO_EERR_DATA_CONVERSION	231
		     /* 232 */
#define FIO_ETOOM	233
#define FIO_EEDITDSCR	234
#define FIO_EMISMATCH	235
#define FIO_EBIGREC	236
#define FIO_EQUAD	237
#define FIO_ETAB_VALUE_OUT_OF_RANGE	238
#define FIO_ENOTMEM	239
		     /* 240 */
		     /* 241 */
#define FIO_EDIRECT	242
#define FIO_EPNEST	243
#define FIO_ENONAME	244
#define FIO_ESYNTAX	245
#define FIO_EINFINITE_REVERSION	246
		    /* 247 */
#define FIO_ESUBSC	248
#define FIO_EFGD	249
#define FIO_EDOT	250
#define FIO_ECHAR	251
#define FIO_EEOFERR	252
#define FIO_EDREAD	253
#define FIO_EREPCNT	254

#if defined(ISC)
/* CFS i/o error codes */
#define FIO_MIXED_IO    260
#define FIO_EINTERN     261
#endif

/* Fortran I/O file control block struct */

typedef struct fcb {
    struct fcb	*next;	/* pointer to next fcb in avail or allocd list */
    FILE	*fp;	/* UNIX file pointer from fopen().  Note that a non-
			 * NULL value for this field is what indicates that
			 * a particular FCB is in use. */
    int		unit;		/* unit number */
    char	*name;		/* file name */
    int		reclen;	/* record length in bytes or words- ??? I
			 * assume 0 means variable length */
    int		wordlen;	/* length of words in bytes */
    int		nextrec;	/* record number of next record */
    int		maxrec;	/* maximum record number (direct access only) */
    short	status;		/* FIO_OLD or FIO_SCRATCH */
    short	dispose;	/* KEEP, DELETE or SAVE */
    short	acc;		/* FIO_DIRECT or FIO_SEQUENTIAL (never APPEND)*/
    short	blank;		/* FIO_NULL or FIO_ZERO */
    short	form;		/* FIO_FORMATTED or FIO_UNFORMATTED */
    short	coherent;	/* coherency check for read & write (e.g. write
				 * followed by read needs a seek):
				 *   0 = no seek necessary for read/write
				 *   1 = coherent only if write.
				 *   2 = coherent only if read.
				 */
    sbool	eof_flag; /* indicates that (imaginary) eof record has
			been read.  Initially FALSE, set by ENDFILE or
			read past endoffile; cleared by REWIND and BACKSPACE */
    sbool	named;		/* whether file is named or not */
    sbool	readonly;	/* whether file is read only */
    sbool	stdunit;	/* FCB connected to stdin/stderr/stdout */
    sbool	truncflag;	/* for sequential files only.  If write
			stmt occurs, file must be truncated if necessary */
#ifdef BUFFER_MANAGER
    char *      vbuf;           /* for sequential files, the argument to setvbuf() 
                                   in __fio_open() */
#endif
} FIO_FCB;


/* FIO_FCB flags for access, dispose, blank, status and format fields:  */

#define FIO_OLD		1	/* values for status */
#define FIO_NEW		2
#define FIO_UNKNOWN	3
#define FIO_SCRATCH	4

#define FIO_KEEP	5	/* values for dispose */
#define FIO_DELETE	7

#define FIO_DIRECT	8	/* values for acc(ess) */
#define FIO_SEQUENTIAL	9
#define FIO_APPEND	10

#define FIO_FORMATTED	11	/* values for form(at) */
#define FIO_UNFORMATTED	12

#define FIO_ZERO 13		/* values for blank */
#define FIO_NULL 14

#define FIO_TRUNC 99		/* value for truncating a permanent file --
				 * __fio_rwinit() && __fio_open()
				 */

/*  declare global variables for Fortran I/O:  */

typedef struct {
    FIO_FCB	*fcbs;		/* pointer to list of allocated fcbs */
    INT		*enctab;	/* pointer to buffer w encoded format */
    char	*fname;		/* file name for OPEN error messages */
    int		 fnamelen;
    bool	 error;
    bool	 eof;
} PGI_FIO;

/*  declare external variables/arrays used by Fortran I/O:  */

extern int	errno;

extern PGI_FIO	pgi_fio;
extern short	__fio_type_size[];

/*  declare external functions local to Fortran I/O:  */

extern int	 __fio_assign();	/* assign.c */

extern char	*__fio_default_convert();	/* fmtconv.c */
extern char	*__fio_fmt_i();
extern char	*__fio_fmt_g();
extern char	*__fio_fmt_f();
extern char	*__fio_fmt_e();
extern char	*__fio_fmt_d();

extern int	 __fio_getnum();	/* fmtgetnum.c */

extern int	 __fio_open();		/* open.c */
extern int	 __fio_close();		/* close.c */

extern int	 __fio_error();		/* error.c */
extern int	 __fio_eoferr();
extern VOID	 __fio_errinit();

extern FIO_FCB	*__fio_alloc_fcb();	/* utils.c */
extern VOID	 __fio_free_fcb();
extern FIO_FCB	*__fio_rwinit();
extern FIO_FCB	*__fio_find_unit();
extern int	 __fio_zeropad();
extern bool	 __fio_eq_str();
#ifdef KANJI
extern int	 __fio_wchar2euc();
extern int	 __fio_euc2wchar();
extern VOID	 __fio_putwchar();
#endif
extern VOID	 __fio_swap_bytes();
