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
/* ldr_auxv.c
 * loader auxiliary vector abstraction
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <sys/auxv.h>
#include "ldr_errno.h"
#include "ldr_auxv.h"

extern auxv_t *_auxv;
int _auxv_size;

static int exec_filename_found;
static int exec_loader_filename_found;
static int exec_loader_flags_found;
static int pagesz_found;

static char *exec_filename;
static char *exec_loader_filename;
static int exec_loader_flags;
static int pagesz;

int
ldr_auxv_init(void)
{
	auxv_t *avp;

	/* 
	 * Pass over auxv.  Cache useful entries.  Set those entries
	 * that shouldn't be passed to program to AT_IGNORE.  Compute
	 * the size of auxv, excluding the terminating AT_NULL entry.
	 * The size may eventually be of some use.
	 */
	for (avp = _auxv; avp->a_type != AT_NULL; avp++) {
		switch (avp->a_type) {

		case AT_EXEC_FILENAME:
			exec_filename_found = 1;
			exec_filename = (char *)avp->a_un.a_ptr;
			break;

		case AT_EXEC_LOADER_FILENAME:
			exec_loader_filename_found = 1;
			exec_loader_filename = (char *)avp->a_un.a_ptr;
			avp->a_type = AT_IGNORE;
			avp->a_un.a_val = 0;
			break;

		case AT_EXEC_LOADER_FLAGS:
			exec_loader_flags_found = 1;
			exec_loader_flags = avp->a_un.a_val;
			avp->a_type = AT_IGNORE;
			avp->a_un.a_val = 0;
			break;

		case AT_PAGESZ:
			pagesz_found = 1;
			pagesz = avp->a_un.a_val;
			break;
		}			
		_auxv_size++;
	}
}

int
ldr_auxv_get_exec_filename(filename)
	char **filename;
{
	if (exec_filename_found) {
		*filename = exec_filename;
		return(LDR_SUCCESS);
	}
	return(LDR_ESRCH);
}

int
ldr_auxv_get_exec_loader_filename(filename)
	char **filename;
{
	if (exec_loader_filename_found) {
		*filename = exec_loader_filename;
		return(LDR_SUCCESS);
	}
	return(LDR_ESRCH);
}

int
ldr_auxv_get_exec_loader_flags(flags)
	int *flags;
{
	if (exec_loader_flags_found) {
		*flags = exec_loader_flags;
		return(LDR_SUCCESS);
	}
	return(LDR_ESRCH);
}

int
ldr_auxv_get_pagesz(size)
	int *size;
{
	if (pagesz_found) {
		*size = pagesz;
		return(LDR_SUCCESS);
	}
	return(LDR_ESRCH);
}
