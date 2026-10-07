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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: break.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:42 $";
#endif

#include <sys/types.h>
#include <errno.h>

#ifndef	_SHARED_LIBRARIES

#error "Compatibility brk code not yet supported; use brk.o and sbrk.o instead"

#include <sys/mman.h>

/* minbrk is the  minimum allowable break address; it's initialized to "end".
 * curbrk is the current break; it's initially set to minbrk, and it varies
 * as brk() and sbrk() are performed.
 */

extern	char	end[];
static	char	*minbrk = end;
static	char	*curbrk = end;


#define	round(x, s)	(((unsigned)(x) + (unsigned)(s) - 1) & ~((unsigned)(s) - 1))
#define	round_page(addr) ((void *)round((addr), getpagesize()))


char *
#ifndef _NO_PROTO
brk(char *addr)
#else
brk(addr)
char *addr;
#endif

/* Set the current break to the specified address,
 * Grows or shrinks the break area (by using mmap() and munmap())
 * so that the break area ends at the specified address.
 */
{
	char 		*newbrk;

	addr = round_page(addr);

	if ((char *)addr < (char *)curbrk) {

		/* Shrinking */

		if ((char *)addr < (char *)minbrk)
			addr = minbrk;
		if (munmap(addr, ((char *)curbrk - (char *)addr)) < 0)
			return((char *)(-1));

	} else if ((char *)addr > (char *)curbrk) {

		/* Expanding */

		if ((newbrk = mmap(curbrk, ((char *)addr - (char *)curbrk),
				   PROT_READ|PROT_WRITE|PROT_EXEC,
				   MAP_ANON|MAP_PRIVATE|MAP_FIXED,
				   -1, (off_t)0)) == (caddr_t)(-1))
			return((char *)(-1));

	}

	curbrk = addr;
	return((char *)0);
}


char *
#ifndef _NO_PROTO
sbrk(int incr)
#else
sbrk(incr)
int incr;
#endif

/* Add the specified (signed) quantity to the current break.
 * If shrinking, unmap the space being removed.  If growing,
 * map more space.  Returns the old break.
 */
{
	char		*obrk;
	char		*newbrk;
	char		*tmp;

	obrk = (char *)curbrk;
	newbrk = (char *)curbrk + incr;
	newbrk = round_page(newbrk);

	if (incr < 0) {

		/* Shrinking */

		if ((char *)newbrk < (char *)minbrk)
			newbrk = minbrk;
		if (munmap(newbrk, -incr) < 0)
			return((char *)(-1));

	} else if (incr > 0) {

		/* Expanding */

		if ((tmp = mmap(curbrk, incr, 
				PROT_READ|PROT_WRITE|PROT_EXEC,
				MAP_ANON|MAP_PRIVATE|MAP_FIXED,
				-1, (off_t)0)) == (char *)(-1))
			return((char *)(-1));
	}

	curbrk = newbrk;
	return(obrk);
}

#else	/* _SHARED_LIBRARIES */

/* This version of brk.c simply calls the standalone loader for 
 * brk() and sbrk().
 */

#include <loader.h>
#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>

char *
#ifndef _NO_PROTO
brk(char *addr)
#else
brk(addr)
char *addr;
#endif

{
	int	rc;

	if ((rc = ldr_brk(addr)) < 0) {
		errno = ldr_status_to_errno(rc);
		rc = -1;
	}
	return(rc);
}


char *
#ifndef _NO_PROTO
sbrk(int incr)
#else
sbrk(incr)
int incr;
#endif

{
	char	*obrk;
	int	rc;

	if ((rc = ldr_sbrk(incr, &obrk)) < 0) {
		errno = ldr_status_to_errno(rc);
		return((char *)-1);
	}
	return((char *)obrk);
}

#endif	/* _SHARED_LIBRARIES */
