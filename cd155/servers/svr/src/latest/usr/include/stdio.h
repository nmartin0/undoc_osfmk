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
 * $Log: stdio.h,v $
 * Revision 1.3  1994/11/18  21:10:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:09  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:59  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:18:43  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:11  nandy
 * Initial revision
 *
 * Revision 2.11  92/04/13  14:39:27  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.11.4.3  91/03/11  15:02:20  sgrainge
 * 	Bring definition of va_list into XPG3 compliance.
 * 	[91/03/11  15:01:06  sgrainge]
 * 
 * Revision 2.11.4.2  91/01/07  20:12:36  dwm
 * 	no change
 * 	[91/01/07  20:11:09  dwm]
 * 
 * Revision 2.11.2.2  90/12/06  08:53:15  dwm
 * 	sprintf.
 * 	[90/12/04  11:48:36  dwm]
 * 
 * 	Upgrade of sprintf to post-BSD4.3 semantics.
 * 	[90/12/04  10:11:05  dwm]
 * 
 * Revision 2.11  90/10/31  15:15:56  devrcs
 * 	Moved P_tmpdir from _OSF_SOURCE to _XOPEN_SOURCE.
 * 	[90/10/12  16:01:10  rabin]
 * 
 * 	Fixed ANSI C name-space pollution caused by P_tmpdir.
 * 	[90/10/05  08:51:48  rabin]
 * 
 * Revision 2.10  90/10/07  20:36:58  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:59:38  gm]
 * 
 * 	Add _BSD version of sprintf
 * 	[90/09/24  16:31:14  lehotsky]
 * 
 * Revision 2.9  90/09/13  12:32:32  devrcs
 * 	Fixed missed __va_list_synonym prototype.
 * 	[90/09/03  09:16:21  gm]
 * 
 * 	Increased BUFSIZ to 8K.  It is now determined
 * 	dynamically by _findbuf in the library.
 * 	va_list again: added a secret synonym for va_list,
 * 	so we still have correct prototypes.
 * 	[90/08/31  15:45:10  tom]
 * 
 * 	Fixed the fix of a fix: added non-prototyped entries for routines
 * 	that use va_list when it hasn't been defined.
 * 	[90/08/30  09:44:41  tom]
 * 
 * Revision 2.8  90/08/24  13:53:27  devrcs
 * 	Removed some va_list code.
 * 	[90/08/18  13:20:10  gm]
 * 
 * 	Updated to IBM golden drop:
 * 	  Move va_list decl to OSF_SOURCE.
 * 	  Move include of limits.h to OSF_SOURCE.
 * 	  move _NRWS to OSF_SOURCE.
 * 	  Change FOPEN_MAX to match OPEN_MAX.
 * 	[90/08/17  16:19:06  tom]
 * 
 * 	Changed more locked/unlocked names for 1003.4a
 * 	[90/08/10  15:46:31  sp]
 * 
 * 	Change all the locked/unlocked calls to be consistent with the 1003.4a
 * 	[90/08/10  14:57:51  sp]
 * 
 * Revision 2.7  90/08/09  14:28:28  devrcs
 * 	Changes for reentrant C library
 * 	[90/08/02  09:56:46  sp]
 * 
 * 	Really move getw, putw, tempnam, popen, pclose to XOPEN_SOURCE.
 * 	[90/07/25  15:38:51  tom]
 * 
 * Revision 2.6  90/06/29  14:14:07  devrcs
 * 	Cleanup for standards:
 * 	 Move getw, putw, tempnam, popen, pclose to XOPEN.
 * 	 Move ctermid and cuserid to unistd.h.
 * 	[90/06/21  12:41:39  tom]
 * 
 * Revision 2.5  90/04/27  23:07:22  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/17  12:36:57  tom]
 * 
 * Revision 2.4  90/03/13  21:23:33  mbrown
 * 	change number of _NIOBRW to 8 from 20
 * 	[90/02/25  14:34:06  mbrown]
 * 
 * 	AIX merge first cut.
 * 	[90/02/12  18:01:29  tom]
 * 
 * $EndLog$
 */
/* @(#)$RCSfile: stdio.h,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/18 21:10:57 $ */
/*
 * COMPONENT_NAME: stdio.h
 *                                                                    
 * ORIGIN: IBM
 *
 * Copyright International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 * 
 * stdio.h  1.40  com/inc,3.1,9021 5/12/90 06:54:44
 */                                                                   

#ifndef _STDIO_H_
#define _STDIO_H_

#include <standards.h>

/*
 *
 *      The ANSI and POSIX standards require that certain values be in stdio.h.
 *      It also requires that if _ANSI_C_SOURCE or _POSIX_SOURCE is defined
 *	then ONLY those values are present. This header includes all the ANSI 
 *	and POSIX required entries.
 *
 */

#ifdef _ANSI_C_SOURCE

/*
 *      The following definitions are included in <sys/types.h>.  They
 *      are also included here to comply with ANSI standards.
 */

#ifndef NULL
#define NULL    0
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long   size_t;
#endif

#ifndef _FPOS_T
#define _FPOS_T
typedef long    fpos_t;
#endif

/*
 *      The definition of TMP_MAX is included in <sys/limits.h>.  It is
 *      also defined here to comply with ANSI standards.
 */

#ifndef TMP_MAX
#define TMP_MAX         16384
#endif


#define FOPEN_MAX 	64
#define FILENAME_MAX 	255
#define BUFSIZ		8192
#define _P_tmpdir       "/tmp/"
#define L_tmpnam	(sizeof(_P_tmpdir) + 15)

/*
 * _IOLBF means that a file's output will be buffered line by line
 * In addition to being flags, _IONBF, _IOLBF and _IOFBF are possible
 * values for "type" in setvbuf.
 */
#define _IOFBF		0000
#define _IOLBF		0200
#define _IONBF		0004


#ifndef EOF
#define EOF		(-1)
#endif

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

typedef struct {
	int	_cnt;
	unsigned char	*_ptr;
	unsigned char	*_base;
	int	_bufsiz;
	short	_flag;
	short	_file;
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
	int	_unused[2];
	void	*_lock;			/* lock for thread safe library */
#else
	int	_unused[3];
#endif
	unsigned char   *_bufendp;
} FILE;

#define _NIOBRW		8
extern FILE	_iob[_NIOBRW];

#define _IOEOF		0020
#define _IOERR		0040

#define stdin		(&_iob[0])
#define stdout		(&_iob[1])
#define stderr		(&_iob[2])

#ifdef  _NONSTD_TYPES
extern int      fread();
extern int      fwrite();
#else
#if   defined(_NO_PROTO)
extern size_t   fread();
extern size_t   fwrite();
#else   /* _NO_PROTO */
extern size_t   fread(void *, size_t, size_t, FILE *);
extern size_t   fwrite(const void *, size_t, size_t,FILE *);
#endif  /* _NO_PROTO */
#endif  /* _NONSTD_TYPES */

#ifdef _NO_PROTO
extern int _flsbuf();
extern int _filbuf();
extern int	ferror();
extern int	feof();
extern void	clearerr();
extern int	putchar();
extern int	getchar();
extern int	putc();
extern int	getc();
extern int	remove();
extern int	rename();
extern FILE 	*tmpfile();
extern char 	*tmpnam();
extern int 	fclose();
extern int 	fflush();
extern FILE	*fopen();
extern FILE 	*freopen();
extern void 	setbuf();
extern int 	setvbuf();
extern int	sprintf();
extern int	fprintf(); 
extern int	fscanf();
extern int	printf();
extern int	scanf();
extern int	sscanf(); 
extern int	vfprintf();
extern int	vprintf(); 
extern int	vsprintf();
extern int 	fgetc();
extern char 	*fgets();
extern int 	fputc();
extern int 	fputs();
extern char 	*gets();
extern int 	puts();
extern int	ungetc();
extern int	fgetpos();
extern int 	fseek();
extern int	fsetpos();
extern long     ftell();
extern void	rewind();
extern void 	perror(); 
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
extern void	flockfile();
extern void	funlockfile();
#endif
#else			/* use ANSI C required prototypes */

extern int _flsbuf(unsigned int, FILE *);
extern int _filbuf(FILE *);
extern int 	ferror(FILE *);
extern int 	feof(FILE *);
extern void 	clearerr(FILE * );
extern int 	putchar(int );
extern int 	getchar(void);
extern int 	putc(int , FILE *);
extern int 	getc(FILE *);
extern int	remove(const char *);
extern int	rename(const char *, const char *);
extern FILE 	*tmpfile(void);
extern char 	*tmpnam(char *);
extern int 	fclose(FILE *);
extern int 	fflush(FILE *);
extern FILE	*fopen(const char *, const char *);
extern FILE 	*freopen(const char *, const char *, FILE *);
extern void 	setbuf(FILE *, char *);
extern int 	setvbuf(FILE *, char *, int , size_t );
extern int	fprintf(FILE *, const char *, ...); 
extern int	fscanf(FILE *, const char *, ...);
extern int	printf(const char *, ...); 
extern int	scanf(const char *, ...); 
extern int	sprintf(char *, const char *, ...); 
extern int	sscanf(const char *, const char *, ...); 

#ifndef _VA_LIST
#define _VA_LIST
typedef char * __va_list;
extern int  vfprintf(FILE *, const char *, __va_list );
extern int  vprintf(const char *, __va_list );
extern int  vsprintf(char *, const char *, __va_list );
#else
extern int  vfprintf(FILE *, const char *, va_list );
extern int  vprintf(const char *, va_list );
extern int  vsprintf(char *, const char *, va_list );
#endif

extern int 	fgetc(FILE *);
extern char 	*fgets(char *, int , FILE *);
extern int 	fputc(int , FILE *);
extern int 	fputs(const char *, FILE *);
extern char 	*gets(char *);
extern int 	puts(const char *);
extern int	ungetc(int , FILE *);
extern int	fgetpos(FILE *, fpos_t *);
extern int 	fseek(FILE *, long , int );
extern int	fsetpos(FILE *, const fpos_t *);
extern long    	ftell(FILE *);
extern void	rewind(FILE *);
extern void 	perror(const char *); 
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
extern void	flockfile(FILE *);
extern void	funlockfile(FILE *);
#endif

#endif /* _NO_PROTO */

#if	defined(_REENTRANT) || defined(_THREAD_SAFE)
/*
 * The default for getc and putc are locked for compatibility with 
 * Posix P1003.4a
 * By defining _STDIO_UNLOCK_CHAR_IO before including this
 * file, the default action is changed to unlocked putc and getc.
 * A file lock can still be placed around a block of putc's or getc's
 * regardless of the locking mode, and invoking the locked or
 * unlocked version directly always overrides the default action.
 */

#define unlocked_getc(p)	(--(p)->_cnt < 0 ? _filbuf(p) : (int) *(p)->_ptr++)
#define unlocked_getchar()	unlocked_getc(stdin)
#define locked_getc(p)		fgetc(p)
#define locked_getchar()	locked_getc(stdin)
#define unlocked_putc(x, p)	(--(p)->_cnt < 0 ? \
				_flsbuf((unsigned char) (x), (p)) : \
				(int) (*(p)->_ptr++ = (unsigned char) (x)))
#define unlocked_putchar(x)	unlocked_putc(x,stdout)
#define locked_putc(x, p)	fputc(x, p)
#define locked_putchar(x)	locked_putc(x,stdout)

#ifndef _STDIO_UNLOCK_CHAR_IO
#define getc(p)			locked_getc(p)
#define putc(x, p)		locked_putc(x, p)
#else
#define getc(p)			unlocked_getc(p)
#define putc(x, p)		unlocked_putc(x, p)

/*
 * if _STDIO_UNLOCK_CHAR_IO is not defined, these macros will not be defined
 * and become functions.
 */
#define clearerr(p)		((void) ((p)->_flag &= ~(_IOERR | _IOEOF)))
#define feof(p)			((p)->_flag & _IOEOF)
#define ferror(p)		((p)->_flag & _IOERR)
#endif /* _STDIO_UNLOCK_CHAR_IO */

#define unlocked_clearerr(p)	((void) ((p)->_flag &= ~(_IOERR | _IOEOF)))
#define unlocked_feof(p)	((p)->_flag & _IOEOF)
#define unlocked_ferror(p)	((p)->_flag & _IOERR)
#define unlocked_fileno(p)	(p)->_file

#else /* _REENTRANT */

#define getc(p)		(--(p)->_cnt < 0 ? _filbuf(p) : (int) *(p)->_ptr++)

#define putc(x, p)	(--(p)->_cnt < 0 ? \
			_flsbuf((unsigned char) (x), (p)) : \
			(int) (*(p)->_ptr++ = (unsigned char) (x)))

#define clearerr(p)	((void) ((p)->_flag &= ~(_IOERR | _IOEOF)))
#define feof(p)		((p)->_flag & _IOEOF)
#define ferror(p)	((p)->_flag & _IOERR)

#endif /* _REENTRANT */

#define getchar()	getc(stdin)
#define putchar(x)	putc((x), stdout)
#endif /*_ANSI_C_SOURCE */


/*
 *   The following are values that have historically been in stdio.h.
 *
 *   They are a part of the POSIX defined stdio.h and therefore are
 *   included when _POSIX_SOURCE and _XOPEN_SOURCE are defined.
 *
 */

#ifdef _POSIX_SOURCE
#include <sys/types.h>

#define L_ctermid	9
#define L_cuserid	9

#ifdef _NO_PROTO
extern int 	fileno();
extern FILE	*fdopen();

#else

extern int 	fileno(FILE *);
extern FILE 	*fdopen(int , char *);

#endif /* _NO_PROTO */

#if !defined(_REENTRANT) || !defined(_STDIO_UNLOCK_CHAR_IO)
#define fileno(p)	(p)->_file     
#endif

#endif /* _POSIX_SOURCE */


#ifdef _XOPEN_SOURCE

#define P_tmpdir	_P_tmpdir

#ifdef _NO_PROTO
extern int 	getw();
extern int 	pclose();
extern int 	putw();
extern FILE 	*popen();
extern char 	*tempnam();
#else
extern int 	getw(FILE*);
extern int 	pclose(FILE *);
extern int 	putw(int, FILE*);
extern FILE 	*popen(char *, char *);
extern char 	*tempnam(char*,char*);
#endif _NO_PROTO

#endif /* _XOPEN_SOURCE */


#ifdef _OSF_SOURCE

#include <sys/limits.h>	/* limits.h not allowed by Posix.1a.  Must be here */

#ifdef OPEN_MAX
#define _NFILE		OPEN_MAX
#else
#define _NFILE		64	/* should be a multiple of _NIOBRW */
#endif

#define _NRWS           (_NFILE / _NIOBRW )+1
                        /* Number of iob rows should be equal           */
                        /* to (_NFILE / _NIOBRW ) + 1.  The extra       */
                        /* row ensures insures that the _NFILE+1th      */
                        /* fopen fails because of open().               */


/* buffer size for multi-character output to unbuffered files */
#define _SBFSIZ 8

#define _IOREAD		0001
#define _IOWRT		0002
#define _IOMYBUF	0010
#define _IOSTRG		0100
#define _IONOFD		_IOSTRG		/* strange or no file descriptor */
#define _IORW		0400
#define _IOUNGETC	01000
#define _IOINUSE	02000		/* new flag for _THREAD_SAFE */

#define _bufend(p)	((p)->_bufendp)
#define _bufsiz(p)	(_bufend(p) - (p)->_base)


#ifdef _NO_PROTO
extern void 	setbuffer();
extern void 	setlinebuf();
#else
extern void 	setbuffer(FILE*, char*, int);
extern void 	setlinebuf(FILE*);
#endif /* _NO_PROTO */

#endif /* _OSF_SOURCE */
#endif /* _STDIO_H_ */
