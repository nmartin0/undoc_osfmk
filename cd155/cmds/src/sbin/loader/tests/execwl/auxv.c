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

#include <stdio.h>
#include <sys/auxv.h>

extern auxv_t *_auxv;
int _auxv_size;

static int exec_loader_flags, exec_loader_flags_found;
static char *exec_loader_filename = (char *)-1;
static char *exec_filename = (char *)-1;

void auxv_print_entry();
char *type2name();

int
auxv_init()
{
	auxv_t *avp;

	/* 
	 * Pass over auxv, caching useful entries and
	 * computing the size of auxv, excluding the
	 * AT_NULL entry.
	 */
	for (avp = _auxv; avp->a_type != AT_NULL; avp++) {
		switch (avp->a_type) {

		case AT_EXEC_FILENAME:
			exec_filename = (char *)avp->a_un.a_ptr;
			break;

		case AT_EXEC_LOADER_FILENAME:
			exec_loader_filename = (char *)avp->a_un.a_ptr;
			break;

		case AT_EXEC_LOADER_FLAGS:
			exec_loader_flags_found = 1;
			exec_loader_flags = avp->a_un.a_val;
			break;
		}			
		_auxv_size++;
	}
	return(0);
}

char *
auxv_get_exec_filename()
{
	return(exec_filename);
}

char *
auxv_get_exec_loader_filename()
{
	return(exec_loader_filename);
}

int
auxv_get_exec_loader_flags(flagsp)
	int *flagsp;
{
	if (exec_loader_flags_found) {
		*flagsp = exec_loader_flags;
		return(0);
	} else
		return(-1);
}

int
auxv_get_type(i)
{
	if (i > _auxv_size)
		return(-1);
	return(_auxv[i].a_type);
}


char *
auxv_get_typename(i)
{
	if (i > _auxv_size)
		return((char *)-1);
	return(type2name(_auxv[i].a_type));
}

void
auxv_print(prefix)
	char *prefix;
{
	int i;

	for (i = 0; i < _auxv_size; i++)
		auxv_print_entry(i, prefix);
}

void
auxv_print_entry(i, prefix)
	char *prefix;
{
	void *ptr, (*fcn)();
	int type, val;
	auxv_t *avp;

	avp = &_auxv[i];
	type = avp->a_type;
	val = avp->a_un.a_val;
	ptr = avp->a_un.a_ptr;
	fcn = avp->a_un.a_fcn;

	(void)printf("%sauxv[%d] = {%d, 0x%x}", prefix, i, type, val);
	switch (type) {

	default:
		(void)printf("\n");
		break;

	case AT_NULL:
		(void)printf(", {AT_NULL}\n");
		break;

	case AT_IGNORE:
		(void)printf(", {AT_IGNORE}\n");
		break;

	case AT_EXECFD:
		(void)printf(", {AT_EXECFD, %d}\n", val);
		break;

	case AT_PHDR:
		(void)printf(", {AT_PHDR}\n");
		break;

	case AT_PHENT:
		(void)printf(", {AT_PHENT, %d}\n", val);
		break;

	case AT_PHNUM:
		(void)printf(", {AT_PHNUM, %d}\n", val);
		break;

	case AT_PAGESZ:
		(void)printf(", {AT_PAGESZ, %d}\n", val);
		break;

	case AT_BASE:
		(void)printf(", {AT_BASE}\n");
		break;

	case AT_FLAGS:
		(void)printf(", {AT_FLAGS}\n");
		break;

	case AT_ENTRY:
		(void)printf(", {AT_ENTRY}\n");
		break;

	case AT_EXEC_FILENAME:
		(void)printf(", {AT_EXEC_FILENAME, \"%s\"}\n", (char *)ptr);
		break;

	case AT_EXEC_LOADER_FILENAME:
		(void)printf(", {AT_EXEC_LOADER_FILENAME, \"%s\"}\n",
			(char *)ptr);
		break;

	case AT_EXEC_LOADER_FLAGS:
		(void)printf(", {AT_EXEC_LOADER_FLAGS}\n");
		break;

	}
}

static char *
type2name(t)
{
	switch (t) {

	default:
		return((char *)-1);
	case AT_NULL:
		return("AT_NULL");
	case AT_IGNORE:
		return("AT_IGNORE");
	case AT_EXECFD:
		return("AT_EXECFD");
	case AT_PHDR:
		return("AT_PHDR");
	case AT_PHENT:
		return("AT_PHENT");
	case AT_PHNUM:
		return("AT_PHNUM");
	case AT_PAGESZ:
		return("AT_PAGESZ");
	case AT_BASE:
		return("AT_BASE");
	case AT_FLAGS:
		return("AT_FLAGS");
	case AT_ENTRY:
		return("AT_ENTRY");
	case AT_EXEC_FILENAME:
		return("AT_EXEC_FILENAME");
	case AT_EXEC_LOADER_FILENAME:
		return("AT_EXEC_LOADER_FILENAME");
	case AT_EXEC_LOADER_FLAGS:
		return("AT_EXEC_LOADER_FLAGS");
	}
	return((char *)-1);
}
