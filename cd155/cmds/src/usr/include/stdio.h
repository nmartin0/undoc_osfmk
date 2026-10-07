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
/* @(#)$RCSfile: stdio.h,v $ $Revision: 1.6 $ (OSF) $Date: 1994/11/19 02:40:57 $ */
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

#if   __PGC__
#if     defined(__i860__) && !defined(_VA_LIST)
#include <va_list.h>
#endif  /* i860 !_VA_LIST */

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
#endif     /* __PGC__ */


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
#define unlocked_fileno(p)	((p)->_file)

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

#if (!defined(_REENTRANT) && !defined(_THREAD_SAFE)) || defined(_STDIO_UNLOCK_CHAR_IO)
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
