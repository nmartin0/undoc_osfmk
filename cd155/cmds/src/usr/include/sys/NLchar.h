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
/* @(#)NLchar.h	1.24  com/inc/sys,3.1,9013 2/27/90 22:12:43 */
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

#ifndef _SYS_NLCHAR_H_
#define _SYS_NLCHAR_H_


#ifdef _KJI
#include <jctype.h>
#endif   /* _KJI */
#include <sys/types.h>
#include <sys/localedef.h> 

extern loc_t *_locp;

/*  BASIC DEFINITIONS FOR USING NLCHARS.
 */

/*  The big type itself.
 */
typedef long NLchar;

#ifdef _NO_PROTO
extern char *NLstrcpy();

extern unsigned char
	*NLstrncpy(),
	*NLstrcat(),
	*NLstrncat(),
	*NLstrchr(),
	*NLstrrchr(),
	*NLstrpbrk(),
	*NLstrtok();

extern NLchar
	*NCstrcpy(),
	*NCstrncpy(),
	*NCstrcat(),
	*NCstrncat(),
	*NCstrchr(),
	*NCstrrchr(),
	*NCstrpbrk(),
	*NCstrtok();

#else /* _NO_PROTO */

extern char *NLstrcpy(char *, char *);

extern unsigned char
        *NLstrncpy(unsigned char *, unsigned char *, int),
        *NLstrcat(unsigned char *, unsigned char *),
        *NLstrncat(unsigned char *, unsigned char *, int),
        *NLstrchr(unsigned char *, NLchar),
        *NLstrrchr(unsigned char *, NLchar),
        *NLstrpbrk(unsigned char *, char *),
        *NLstrtok(unsigned char *, unsigned char *);

extern NLchar
        *NCstrcpy(NLchar *, NLchar *),
        *NCstrncpy(NLchar *, NLchar *, int),
        *NCstrcat(NLchar *, NLchar *),
        *NCstrncat(NLchar *, NLchar *, int),
        *NCstrchr(NLchar *, NLchar),
        *NCstrrchr(NLchar *, NLchar),
        *NCstrpbrk(NLchar *, char *),
        *NCstrtok(NLchar *, unsigned char *);
#endif /* _NO_PROTO */

/*  Number of distinct NLchars.
 */
#define NLCOLMAX        16384     /* maximum # elements in collation table */
#define NLCHARMAX       257

#ifdef lint
extern NLchar NCdechr();
#else

/*  Single-shift character definition (both NLchar and char representation).
 */

#define _NCtop(nlc)     ((nlc) >> 8 & 0xff)
#define _NCbot(nlc)     ((nlc) & 0xff)

/*  SINGLE-CHARACTER CONVERSION MACROS.
 */

/*  Internal macro to test for multi-byte NLS code point.
 */
#define _NCis2(c0,c1)   NCisshift(c0)

/*  Internal macro to convert multi-byte NLS code point to NLchar.
 */
#define _NCd2(c0,c1)    (((unsigned char)(c0) << 8) | (unsigned char)(c1))

/*  Internal macro to convert NLchar to NLS code point.
 */

#define _NCe2(nlc, c0, c1)      (((nlc) > 0xff) ? \
					((c0) = _NCtop(nlc), \
						(c1) = _NCbot(nlc), 2) : \
					((c0) = (nlc), 1))

/*  Convert c0 (and c1, if need be) into NLchar nlc; return # chars used.
 *  This is the only conversion macro that accepts an NLchar, rather than
 *  a pointer to one, as an argument.
 */
#define _NCdec2(c0, c1, nlc) ((_NCis2((c0), (c1))) ? \
				((nlc) = _NCd2((c0), (c1)), 2) : \
				((nlc) = (unsigned)(c0), 1))

/*  Defs for accessing collating data.
 */
#ifndef _KJI
#define _NCmap(nlc)	( (nlc) )
#define _NCunmap(nlc)	( (nlc) )
#else
#define _NCmap(nlc)     ( ((nlc) <= 0xff) ? (nlc) : \
				( (nlc) < 0xdfff ? ( (nlc) - 0x8000 ) : \
						   ( (nlc) - 0xc000 ) ))
#define _NCunmap(nlc)   ( ((nlc) <= 0xff)? (nlc) : \
				( (nlc) < 0x2000 ? ( (nlc) + 0x8000 ) : \
						   ( (nlc) + 0xc000 ) ))
#endif  /* _KJI */

/* 
 * NCeqvmap is no longer supported because the definition of an equivalence 
 * class is changed to mean "all characters or collating elements with the 
 * same primary collation value". If used, the macro will always return 1 
 * ("no equivalence class").
 */

#define NCeqvmap(ucval) ( 1 )
#endif /* lint */

/*  MISCELLANY.
 */

/*  Internal defs for string(3) macros using charsets.
 */
#define NLCSETMAX	33	/* malloc charsets over this length - 1 */

#ifdef lint
extern NLchar *_NCbufstr();
extern void _NCfreebuf();
#else

/*  Decode a string into an NLchar array big enough to hold it.  Use array
 *  s if possible, else malloc one with _NLgetbuf.  First element flags
 *  whether malloc was used.
 */
#define _NCbufstr(s, d, l)	((d)[0] = 0, ((l) - 2 <= \
					NCdecstr((s), &(d)[1], (l) - 1)) ? \
					_NCgetbuf((s), (l)) : &(d)[1])


/*  Free a buffer filled by _NCbufstr, if it was malloc'd originally.
 */
#ifndef _KERNEL
#define _NCfreebuf(s)		if (*((s) - 1)) free((void *)((s) - 1));
#endif
#endif

extern NLchar *_NCgetbuf();
#ifndef _KERNEL
#ifdef   _NO_PROTO
extern void free();
#else  /*_NO_PROTO */
extern void free(void *);
#endif  /*_NO_PROTO */
#endif	/* _KERNEL */

#endif /* _SYS_NLCHAR_H_ */ 
