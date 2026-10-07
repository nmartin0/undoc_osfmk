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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_part_ops_.c,v 1.9 1994/11/19 02:31:54 mtm Exp $
 *
 */

static char oldpart[256];
static char newpart[256];

#include <stdarg.h>
#include <nx.h>
#include <nx/defines.h>
#include <errno.h>

#define	MAX_ARGS	40

long
nx_mkpart_(part, size, attrib, partlen)
char	*part;			/* partition name */
long    *size;
long    *attrib;
long	partlen;
{
	long status;
	_copyname(part, newpart, partlen, sizeof(newpart));
 	status = nx_mkpart(newpart, *size, *attrib);
        return status;
}

long
nx_mkpart_rect_(part, rows, cols, attrib, partlen)
char	*part;			/* partition name */
long    *rows;
long    *cols;
long    *attrib;
long	partlen;
{
	long status;
	_copyname(part, newpart, partlen, sizeof(newpart));
 	status = nx_mkpart_rect(newpart, *rows, *cols, *attrib);
        return status;
}

long
nx_mkpart_map_(part, size, map, attrib, partlen)
char	*part;			/* partition name */
long    *size;
long    *map;
long    *attrib;
long	partlen;
{
	long status;
	_copyname(part, newpart, partlen, sizeof(newpart));
 	status = nx_mkpart_map(newpart, *size, map, *attrib);
        return status;
}

long
nx_rmpart_(part, force, recursive, partlen)
char	*part;			/* partition name */
long    *force;
long    *recursive;
long	partlen;
{
	long status;
	_copyname(part, newpart, partlen, sizeof(newpart));
 	status = nx_rmpart(newpart,*force,*recursive);
        return status;
}


long
nx_chpart_mod_(part, mod, partlen)
char	*part;			/* partition name */
long    *mod;
long	partlen;
{
	long status;
	_copyname(part, newpart, partlen, sizeof(newpart));
 	status = nx_chpart_mod(newpart, *mod);
        return status;
}

long
nx_chpart_epl_(part, priority, partlen)
char	*part;			/* partition name */
long    *priority;
long	partlen;
{
	long status;
	_copyname(part, newpart, partlen, sizeof(newpart));
 	status = nx_chpart_epl(newpart, *priority);
        return status;
}

long
nx_chpart_rq_(part, quantum, partlen)
char	*part;			/* partition name */
long    *quantum;
long	partlen;
{
	long status;
	_copyname(part, newpart, partlen, sizeof(newpart));
 	status = nx_chpart_rq(newpart, *quantum);
        return status;
}

long
nx_chpart_owner_(part, owner, group, partlen)
char	*part;			/* partition name */
long    *owner;
long    *group;
long	partlen;
{
	long status;
	_copyname(part, newpart, partlen, sizeof(newpart));
 	status = nx_chpart_owner(newpart, *owner, *group);
        return status;
}

long
nx_chpart_name_(old_part, new_part, oldlen, newlen)
char	*old_part;		/* old partition name */
char    *new_part;		/* new partition name */
long    oldlen;
long	newlen;
{
	long status;
	_copyname(old_part, oldpart, oldlen, sizeof(oldpart));
	_copyname(new_part, newpart, newlen, sizeof(newpart));
 	status = nx_chpart_name(oldpart, newpart);
        return status;
}

long
nx_chpart_sched_(name, sched_type,namelen)
char	*name;		/* partition name */
long	*sched_type;
long	namelen;
{
	long	status;
	_copyname(name, newpart, namelen, sizeof(newpart));
 	status = nx_chpart_sched(newpart, *sched_type);
        return status;
}


long
nx_mkpart_attr_(char* name, ...)
{
	va_list ap;
	long namelen;
	long *arg;
	int status;
	long nArgs[MAX_ARGS];

	va_start(ap, name);

	for (;;) {
	    arg = va_arg(ap, long*);
	    if (*arg == NX_ATTR_END)
		break;
	    arg = va_arg(ap, long*);
	}
	namelen = va_arg(ap, long);
	_copyname(name, newpart, namelen, sizeof(newpart));

	va_start(ap, name);

	if (_copyargs(ap, 1, &nArgs, MAX_ARGS) < 0) {
		errno = EINVAL;
		return -1;
	}

#if MAX_ARGS == 40
	status = nx_mkpart_attr(newpart, 
		nArgs[0], nArgs[1], nArgs[2], nArgs[3], nArgs[4],
		nArgs[5], nArgs[6], nArgs[7], nArgs[8], nArgs[9],
		nArgs[10], nArgs[11], nArgs[12], nArgs[13], nArgs[14],
		nArgs[15], nArgs[16], nArgs[17], nArgs[18], nArgs[19],
		nArgs[20], nArgs[21], nArgs[22], nArgs[23], nArgs[24],
		nArgs[25], nArgs[26], nArgs[27], nArgs[28], nArgs[29],
		nArgs[30], nArgs[31], nArgs[32], nArgs[33], nArgs[34],
		nArgs[35], nArgs[36], nArgs[37], nArgs[38], nArgs[39]);
#else
	fprintf(stderr, "In file %s, MAX_ARGS != 40\n", __FILE__);
	exit(1);
#endif


	return status;
}
