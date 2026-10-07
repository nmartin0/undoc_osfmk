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
 * $Log: stdlib.h,v $
 * Revision 1.3  1994/11/18  21:11:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:12  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:02  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:18:48  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:12  nandy
 * Initial revision
 *
 * Revision 1.7  92/04/13  14:39:29  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.7  90/10/31  15:16:02  devrcs
 * 	Changed the definition of MB_CUR_LEN to call a libc function.
 * 	[90/10/17  12:47:48  tom]
 * 
 * 	Replace deleted #else for _NOPROTO around environment calls (putenv etc)
 * 	[90/10/11  15:13:39  sp]
 * 
 * Revision 1.6  90/10/07  20:37:02  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:59:45  gm]
 * 
 * Revision 1.5  90/08/09  14:28:32  devrcs
 * 	Changes for reentrant C library
 * 	[90/08/02  09:57:07  sp]
 * 
 * 	add clearenv
 * 	[90/08/01  16:14:42  mbrown]
 * 
 * Revision 1.4  90/04/27  23:07:25  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/16  13:49:32  tom]
 * 
 * Revision 1.3  90/03/27  20:08:15  gm
 * 	Add getpass() for XPG3.
 * 	[90/03/15  16:22:37  tom]
 * 
 * Revision 1.2  90/03/13  21:23:36  mbrown
 * 	Add putenv() for XPG3.
 * 	[90/02/16  17:14:42  tom]
 * 
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:37:47  tom]
 * 
 * $EndLog$
 */
/* stdlib.h 1.20  com/inc,3.1,9013 2/27/90 22:58:14 */
/*
 * COMPONENT_NAME: stdlib.h
 *
 * Copyright International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <standards.h>

/*
 *
 *      The ANSI standard requires that certain values be in stdlib.h.
 *      It also requires if _ANSI_C_SOURCE is defined then ONLY these
 *      values are present. This header includes all the ANSI required entries.
 *
 */
#ifdef _ANSI_C_SOURCE

/*
 *      The following 3 definitions are included in <sys/types.h>.  They are
 *      also included here to comply with ANSI standards.
 */

#ifndef NULL
#define NULL    (void *)0
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long   size_t;
#endif

#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned short  wchar_t;
#endif

typedef struct div_t  {			/* struct returned from div	*/
	int quot;			/* quotient			*/
	int rem; } div_t;			/* remainder			*/

typedef struct ldiv_t  {		/* struct returned from ldiv	*/
	long int quot;			/* quotient			*/
	long int rem; } ldiv_t;		/* remainder			*/

#define EXIT_FAILURE   (1)		/* exit function failure	*/
#define EXIT_SUCCESS	0		/* exit function success	*/
#define RAND_MAX	32767		/* max value returned by rand	*/

extern int __getmbcurmax();
#define MB_CUR_MAX	__getmbcurmax()	/* max bytes in multibyte char	*/

#ifdef   _NO_PROTO
extern double 	atof();
extern int 	atoi();
extern long int atol();
extern double 	strtod();
extern long int strtol();
extern unsigned long int strtoul();
extern int 	rand();
extern void	srand();
extern void 	*calloc();
extern void	free();
extern void	*malloc();
extern void 	*realloc();
extern void	abort();
extern int	atexit();
extern void	exit();
extern char	*getenv();
extern int 	system();
extern void 	*bsearch();
extern void 	qsort();
extern int 	abs();
extern struct div_t	div();
extern long int	labs();
extern struct ldiv_t 	ldiv();
extern int 	mblen();
extern int 	mbtowc();
extern int 	wctomb();
extern size_t 	mbstowcs();
extern size_t 	wcstombs();
#else  /*_NO_PROTO */
extern double 	atof(const char *);
extern int 	atoi(const char *);
extern long int atol(const char *);
extern double 	strtod(const char *, char **);
extern long int strtol(const char *, char **, int );
extern unsigned long int strtoul(const char *, char **, int );
extern int 	rand(void);
extern void	srand(unsigned int );
extern void 	*calloc(size_t , size_t );
extern void	free(void *);
extern void	*malloc(size_t );
extern void 	*realloc(void *, size_t );
extern void	abort(void);
extern int	atexit(void (*func)(void));
extern void	exit(int );
extern char	*getenv(const char *);
extern int 	system(const char *);
extern void 	*bsearch(const void *, const void *, size_t , size_t , int(*compar)(const void *, const void *));
extern void 	qsort(void *, size_t , size_t ,int(*compar)(const void *, const void *));
extern int 	abs(int );
extern struct div_t	div(int , int );
extern long int	labs(long int );
extern struct ldiv_t 	ldiv(long int , long int );
extern int 	mblen(const char *, size_t );
extern int 	mbtowc(wchar_t *, const char *, size_t );
extern int 	wctomb(char *, wchar_t );
extern size_t 	mbstowcs(wchar_t *, const char *,size_t );
extern size_t 	wcstombs(char *, const wchar_t *,size_t );

#endif /*_NO_PROTO */
#endif /*_ANSI_C_SOURCE */

#ifdef _XOPEN_SOURCE

#ifdef _NO_PROTO
extern int 	putenv();
extern int 	clearenv();
extern char 	*getpass();
extern char 	*getpass_r();		/* _POSIX_REENTRANT_FUNCTIONS */
#else
extern int 	putenv(char *);
extern int 	clearenv();
extern char 	*getpass(const char *);
extern char 	*getpass_r(const char *, char *); /* _POSIX_REENTRANT_FUNCTIONS */
#endif

#endif  /* _XOPEN_SOURCE */

/*
 *
 *   The following function prototypes are not defined for programs
 *   that are adhering to strict ANSI conformance, but are included
 *   for floating point support.
 *
 */

#ifdef _OSF_SOURCE
#include <sys/types.h>
     float    atoff();
     char     *ecvt();
     char     *fcvt();
     char     *gcvt();
     float    strtof();
     void     imul_dbl();
     void     umul_dbl();

#endif /* _OSF_SOURCE */
#endif /* _STDLIB_H_ */
