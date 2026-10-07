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

/* Loader implementation of brk() and sbrk() calls.  Uses ldr_mmap() to
 * obtain storage, based on loader-maintained idea of where the end
 * of the main program is.
 */

#include <sys/types.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>

#include "ldr_types.h"
#include "ldr_errno.h"
#include "ldr_lock.h"
#include "ldr_sys_int.h"

/* minbrk is the loader's idea of the minimum allowable break address;
 * it's set to one greater than the highest address used in loading the
 * main program.  curbrk is the loader's idea of the current break;
 * it's initially set to minbrk, and it varies as brk() and sbrk()
 * are performed.
 */

static	univ_t	minbrk = NULL;
static	univ_t	curbrk = NULL;


#define	round(x, s)	(((unsigned)(x) + (unsigned)(s) - 1) & ~((unsigned)(s) - 1))
#define	round_page(addr)	((void *)round((addr), ldr_getpagesize()))


int
ldr_brk(char *addr)

/* Set the loader's idea of the current break to the specified address,
 * If we don't yet have any idea of the current break, this routine sets
 * the minimum and current break addresses to the specified value, and does
 * not allocate any memory.  Otherwise, it grows or shrinks the break area
 * (by using mmap() and munmap()) so that the break area ends at the
 * specified address.
 */
{
	univ_t		newbrk;
	int		rc;

	addr = round_page(addr);

	ldr_lock(&ldr_global_lock);

	if (minbrk == NULL) {
		minbrk = curbrk = addr;
		rc = LDR_SUCCESS;
		goto unlock_exit;
	}

	if ((char *)addr < (char *)curbrk) {

		/* Shrinking */

		if ((char *)addr < (char *)minbrk)
			addr = minbrk;
		rc = ldr_munmap(addr, ((char *)curbrk - (char *)addr));

	} else if ((char *)addr > (char *)curbrk) {

		/* Expanding */

		rc = ldr_mmap(curbrk, ((char *)addr - (char *)curbrk),
			      LDR_R|LDR_W|LDR_X,
			      LDR_MAP_ANON|LDR_MAP_PRIVATE|LDR_MAP_FIXED,
			      LDR_FILE_NONE, (off_t)0, &newbrk);

	} else

		/* No change */

		rc = LDR_SUCCESS;

	if (rc == LDR_SUCCESS)
		curbrk = addr;

unlock_exit:
	ldr_unlock(&ldr_global_lock);
	return(rc);
}


int
ldr_sbrk(int incr, char **pobrk)

/* Add the specified (signed) quantity to the loader's idea of the current
 * break.  If shrinking, unmap the space being removed.  If growing,
 * map more space.  Returns the old break.
 */
{
	univ_t		obrk;
	univ_t		newbrk;
	univ_t		tmp;
	int		rc;

	ldr_lock(&ldr_global_lock);

	obrk = (char *)curbrk;
	newbrk = (char *)curbrk + incr;
	newbrk = round_page(newbrk);
	rc = LDR_SUCCESS;

	if (incr < 0) {

		/* Shrinking */

		if ((char *)newbrk < (char *)minbrk)
			newbrk = minbrk;
		rc = ldr_munmap(newbrk, -incr);

	} else if (incr > 0) {

		/* Expanding */

		rc = ldr_mmap(curbrk, incr, LDR_R|LDR_W|LDR_X,
			      LDR_MAP_ANON|LDR_MAP_PRIVATE|LDR_MAP_FIXED,
			      LDR_FILE_NONE, (off_t)0, &tmp);
	}
	/* else staying the same; just return old value */

	if (rc == LDR_SUCCESS)
		curbrk = newbrk;

	ldr_unlock(&ldr_global_lock);

	*pobrk = obrk;
	return(rc);
}
