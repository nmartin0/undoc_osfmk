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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log: subr_xxx.c,v $
 * Revision 1.5  1994/11/18  20:27:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:49:13  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:49:33  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:05:32  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:14  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.9  1993/04/24  18:44:50  klh
 * 	Revision 2.9  93/01/14  13:10:49  loverso
 * 		Replace atob() with routines that exactly match libc/POSIX/ANSI-C.
 * 		(isspace, isalpha, isdigit, isupper, strtol, strtoul, strspn,
 * 		strpbrk, strtok)
 *
 * 	Revision 2.7  92/05/31  18:58:28  loverso
 * 		Don't need to include ncpus.h anymore (pjg).
 *
 * 	Revision 2.6  92/05/27  20:05:30  pjg
 * 		Include cpus.h and mach_ldebug.h.
 *
 * Revision 2.8  92/07/07  15:12:39  roman
 * Minor format corrections to match OSF code exactly.
 * 
 * Revision 2.7  92/06/18  12:11:27  roman
 * Change isspace() to return true for all characters specified by ANSI C.
 * 
 * Revision 2.6  92/06/05  18:18:53  roman
 * Change isspace() to use an int parameter rather than a pointer, the
 * 	was K&R intended.
 * 
 * Revision 2.5  92/02/26  08:15:33  roman
 * Add new routine isspace().
 * 
 * Revision 2.4  92/01/14  11:06:54  roy
 * 	Added digit and atob routines (durriya).
 * 
 * Revision 2.3  91/11/13  12:53:50  rabii
 * 	i860 support
 * 
 * Revision 2.2  91/08/31  13:23:20  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/05  13:54:32  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  13:19:03  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:59:21  gm]
 * 
 * 	Eliminate extraneous dependency on <sys/buf.h>
 * 	[90/09/23  21:33:56  jeffc]
 * 
 * Revision 1.7  90/09/23  15:43:32  devrcs
 * 	added 386 define for ffs,bcmp,strlen
 * 	[90/09/12  16:33:42  kevins]
 * 
 * Revision 1.6  90/08/24  11:18:19  devrcs
 * 	Change errsys and nullsys to use new system call interface.
 * 	[90/08/17  17:38:51  nags]
 * 
 * 	HP/Apollo M68K
 * 	[90/08/13  17:36:08  mcg]
 * 
 * Revision 1.5  90/06/22  20:06:52  devrcs
 * 	Add includes and ifdef SER_COMPAT from alan@encore.com
 * 	[90/06/13  15:26:38  brezak]
 * 
 * 	Lastest version from nags.
 * 
 * 	Compressed history (reverse chronology):
 * 	Updated for OSF/1 parallelization.		nags@encore.com
 * 	Fixes for first snapshot.			gm@osf.org
 * 	BSD4.4 changes.					noemi@osf.org
 * 	Moved index here, from afs/afs_whatchamacallit.	af@cmu.edu
 * 	Unconditional errsys subroutine returns EINVAL.	gm0w@cmu.edu
 * 	Changes for cleanup.				gm0w@cmu.edu
 * 	Code cleanup cataclysm.				mwyoung@cmu.edu
 * 	Mips-specific code, errsys for bad syscalls.	af@cmu.edu
 * 	Changes for I386.				rvb@cmu.edu
 * 	MACH: Don't include "sys/vm.h".			mwyoung@cmu.edu
 * 	Panic on strlen(NULL).				mja@cmu.edu
 * 	Fix strlen to not fault on null pointer.	jjc@cmu.edu
 * 	Fixes for balance.				beck@sequent.com
 * 	Sun needs imin, min, etc.			jjc@cmu.edu
 * 	(Negatively) Conditionalized include of pte.h	bolosky@cmu.edu
 * 	New calloc routine using VM routines.		avie@cmu.edu
 * 	Upgraded to 4.3.				avie@cmu.edu
 * 	CMUCS:  Fixed off by one error in ffs.		avie@cmu.edu
 * 	[90/06/13  13:55:25  brezak]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)subr_xxx.c	7.1 (Berkeley) 6/5/86
 */

#include <mach_ldebug.h>

#include <cputypes.h>
#include <ser_compat.h>

#include <uxkern/import_mach.h>
#include <kern/parallel.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/uio.h>

/*
 * Routine placed in illegal entries in the bdevsw and cdevsw tables.
 */
nodev()
{

	return (ENODEV);
}

/*
 * Null routine; placed in insignificant entries
 * in the bdevsw and cdevsw tables.
 */
nulldev()
{

	return (0);
}

/*
 * Null system calls, invalid syscall, or just not configured.
 */
errsys(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;

{

	return (EINVAL);
}

nullsys(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{

	return (0);
}

#if	defined(ibmrt) || defined(ns32000) || defined(sun) || defined(i386) || defined(mips) || defined(__hp_osf) || defined(i860)
imin(a, b)
{

	return (a < b ? a : b);
}

imax(a, b)
{

	return (a > b ? a : b);
}

unsigned
min(a, b)
	u_int a, b;
{

	return (a < b ? a : b);
}

unsigned
max(a, b)
	u_int a, b;
{

	return (a > b ? a : b);
}
#endif	/* ibmrt || ns32000 || sun || i386 || mips || __hp_osf || i860 */

#ifdef	balance
/*
 * Sequent Balance kernel uses calloc() differently.  See sqt/startup.c.
 */
#else
#ifndef	OSF1_SERVER
caddr_t calloc(size)
	int size;
{
	return((caddr_t)kmem_alloc(kernel_map, (vm_offset_t)size));
}
#endif	/* OSF1_SERVER */
#endif

#ifdef	GPROF
/*
 * Stub routine in case it is ever possible to free space.
 */
cfreemem(cp, size)
	caddr_t cp;
	int size;
{
	printf("freeing %x, size %d\n", cp, size);
}
#endif


#if	 !defined(vax) && !defined(balance) && !defined(mips)

ffs(mask)
	register long mask;
{
	register int i;

	for(i = 1; i <= NSIG; i++) {
		if (mask & 1)
			return (i);
		mask >>= 1;
	}
	return (0);
}

bcmp(s1, s2, len)
	register char *s1, *s2;
	register int len;
{

	while (len--)
		if (*s1++ != *s2++)
			return (1);
	return (0);
}

strlen(s1)
	register char *s1;
{
	register int len;

	if (s1 == 0)
		panic("strlen(NULL)");
	for (len = 0; *s1++ != '\0'; len++)
		/* void */;
	return (len);
}
#endif	/* !defined(vax) && !defined(balance) && !defined(mips) */


char *
index(strp, c)
register char *strp;
register char c;
{
	for (; *strp; strp++)
		if (*strp == c)
			return(strp);
	return(NULL);
}

rndup(size, minsize)
unsigned size, minsize;
{
	while (minsize < size)
		minsize <<= 1;
	return(minsize);
}

#if     SER_COMPAT
ser_funnel(f)
int f;
{
	unix_master();
}

ser_unfunnel(f)
int f;
{
	unix_release();
}
#endif

int
isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v'
		|| c == '\n');
}

int
isalpha(int c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int
isupper(int c)
{
	return (c >= 'A' && c <= 'Z');
}

int
isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* @(#)strtol.c	5.4 (Berkeley) 2/23/91 */

/*
 * Convert a string to a long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
long
strtol(
	const char *nptr,
	char **endptr,
	register int base)
{
	register const char *s = nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = any ? s - 1 : (char *)nptr;
	return (acc);
}

/* @(#)strtoul.c	5.3 (Berkeley) 2/23/91 */

/*
 * Convert a string to an unsigned long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
unsigned long
strtoul(
	const char *nptr,
	char **endptr,
	register int base)
{
	register const char *s = nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * See strtol for comments as to the logic used.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;
	cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
	cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = ULONG_MAX;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = any ? s - 1 : (char *)nptr;
	return (acc);
}

/* @(#)strpbrk.c,v Revision: 2.5.2.2 (OSF) Date: 1992/02/20 23:04:50 */

char *
strpbrk(
	const char *s1,
	const char *s2)
{
	char *p;

	do {
		for(p=(char *)s2; *p != '\0' && *p != *s1; ++p)
			;
		if(*p != '\0')
			return((char *)s1);
	} while(*s1++);
	return(NULL);
}

/* @(#)strspn.c,v Revision: 2.4.2.2 (OSF) Date: 1992/02/20 23:05:13 */

size_t	
strspn(
	const char *s1,
	register const char *s2)
{
	char *p, *q;

	for(q=(char *)s1; *q != '\0'; ++q) {
		for(p=(char *)s2; *p != '\0' && *p != *q; ++p)
			;
		if(*p == '\0')
			break;
	}
	return(q-s1);
}

/* @(#) strtok.c,v Revision: 2.7.2.4 (OSF) Date: 1992/03/23 21:31:54 */

char *
strtok_r(
	char *s1,
	const char *s2,
	char **savept)
{
	char	*p, *q, *r;

	if (savept == NULL)
		return NULL;

	/* first or subsequent call */
	p = (s1 == NULL)? *savept: s1;

	if (p == NULL)		/* return if no tokens remaining */
		return NULL;

	q = p + strspn(p, s2);	/* skip leading separators */

	if (*q == '\0')		/* return if no tokens remaining */
		return (0);

	if ((r = strpbrk(q, s2)) == 0)	/* move past token */
		*savept = 0;
	else {
		*r = '\0';
		*savept = ++r;
	}
	return (q);
}
