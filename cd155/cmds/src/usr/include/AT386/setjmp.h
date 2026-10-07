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
/*	setjmp.h	4.1	83/05/03	*/

#ifndef _SETJMP_H_
#define _SETJMP_H_
#include <standards.h>

/*
 *
 *      The ANSI and POSIX standards require that certain values be in setjmp.h.
 *      They also require that if _ANSI_C_SOURCE or _POSIX_SOURCE is defined 
 *      then ONLY those standard specific values are present. This header 
 *      includes all the ANSI and POSIX required entries.
 *
 */

#ifdef _ANSI_C_SOURCE
#define _JBLEN  22      /* regs, fp regs, cr, sigmask, context, etc. */

typedef int jmp_buf[_JBLEN];

#ifdef   _NO_PROTO
extern void longjmp();
extern int setjmp(); 
#else  /*_NO_PROTO */
extern void longjmp(jmp_buf , int );
extern int setjmp(jmp_buf ); 
#endif /*_NO_PROTO */

#endif /* _ANSI_C_SOURCE */

#ifdef _POSIX_SOURCE

typedef int sigjmp_buf[_JBLEN];
#define sigsetjmp(env, save)	((save) ? setjmp(env) : _setjmp(env))
#ifdef _NO_PROTO
extern void siglongjmp();
#else
extern void siglongjmp(sigjmp_buf, int);
#endif /* _NO_PROTO */
#endif /* _POSIX_SOURCE */

#ifdef _OSF_SOURCE

#ifdef _NO_PROTO
extern int _setjmp ();
extern void _longjmp ();
#else
extern int _setjmp (jmp_buf);
extern void _longjmp (jmp_buf, int);
#endif

#ifdef _REENTRANT
#ifdef setjmp
#undef setjmp
#endif
#define setjmp(buf)	_setjmp(buf)
#endif

#endif  /* _OSF_SOURCE */

#endif /* _SETJMP_H_ */
