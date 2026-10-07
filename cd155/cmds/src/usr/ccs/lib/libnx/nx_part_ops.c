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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_part_ops.c,v 1.21 1994/11/19 02:31:53 mtm Exp $
 *
 * HISTORY
 * $Log: nx_part_ops.c,v $
 * Revision 1.21  1994/11/19  02:31:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.20  1994/11/18  18:29:15  sdh
 * A check for an invalid scheduling type was dded in _nx_mkpart_attr()
 * before it calls NX_mkpart_attr().
 *
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: 7844
 *  Testing:
 * 	Developer testing
 * 	Test listed in PTS reports
 * 	Eval tests:  nx_mkpart_err, nx_mkpart_map_err
 * 	rmcall, rmcmd EATs (14 nodes, WW46)
 *  Module(s):
 * 	nx_part_ops.c
 *
 * Revision 1.19  1994/11/16  19:16:20  sdh
 * Check for negative rqs before the rq is assigned and manipulated
 * as an unsigned long in nx_chpart_rq.
 *
 *  Reviewer: tracy
 *  Risk: low
 *  Benefit or PTS #: 6888
 *  Testing:
 * 	Developer testing
 * 	Test case listed in PTS report
 * 	Fortran nx_chpart Eval test
 * 	rmcall, rmcmd EATs (8 nodes, WW46)
 *  Module(s):
 * 	cmds_libs/src/usr/ccs/lib/libnx/nx_part_ops.c
 *
 * Revision 1.18  1994/07/19  20:40:02  mag
 * Correct handling of extraneous/duplicated NX_ATTR_xxx arguments (10278, 10301, 10303)
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 10278, 10301, 10303
 *  Testing: developer, EATS: rmcmd, rmcall
 *  Module(s): utils.c, nx_initve.c, nx_part_ops.c
 *
 * Revision 1.17  1994/07/06  17:29:12  mag
 * nx_mkpart better arg checking (9958, 9959, 9960, 9961)
 * Make -nt ,,, an EINVAL (10073)
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 9958, 9959, 9960, 9961, 10073
 *  Testing: developer
 *  Module(s): (9958, 9959, 9960, 9961, 10073): nx_part_ops.c
 * 	    (10073): attributes.c, utils.c, nx_initve.c,
 * 		     allocator/mkpart_rpc.c, mkpart/mkpart.c
 *
 * Revision 1.16  1994/06/15  17:17:53  mag
 * Revert to original behavior of permitting only size=-1 for mkpart
 * and size=0 for initve.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 9840
 *  Testing: developer
 *  Module(s): nx_initve.c, nx_part_ops.c
 *
 * Revision 1.15  1994/06/01  20:14:46  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): Makefile, bitmap.c, bitmap2.c, nx_initve.c, nx_initve_.c,
 * 	    nx_part_ops.c, partprint.c, partutils.c, utils.c,
 * 	    _copyargs.c (new), attrprint.c (news), attributes.c (new)
 *  Related: server, emulator, allocator, bootmesh, mkpart, showpart, lspart
 *
 * Revision 1.14  1993/12/10  19:32:29  carbajal
 * Take out scheduling types checks in this module, defer checking
 * until we get to NX_mkpart()
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 7372
 *  Testing: ran test case
 *  Module(s):
 *
 * Revision 1.13  1993/12/01  01:21:02  carbajal
 * added nx_chpart_sched()
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.12  1993/11/18  19:25:10  dleslie
 *  Reviewer: shala
 *  Risk: low
 *  Benefit or PTS #: get nx and mcmsg headers out of export tree, not obj
 * 	tree.  This allows users to build without having an obj tree fully
 * 	populated with headers
 *  Testing: built libnx
 *  Module(s):
 *     Makefile _gcol.c _gcolx.c _gops.c _gsync.c _load.c allocUser.c
 *     allocsys.c allocsys_.c autoinit.c bitmap.c bitmap2.c create.c
 *     nodeparser.c nx_initve.c nx_load.c nx_load_.c nx_loadve.c
 *     nx_loadve_.c nx_lock.c nx_part_ops.c nx_part_ops_.c nx_port.c
 *     nx_pri.c nxlib.c parsepart.c parsesched.c partlock.c
 *     partprint.c partutils.c rkassert.c rklib.c rkmem.c utils.c
 *     writepart.c
 *
 *
 * VS:    writepart.c
 *
 * Revision 1.11  1993/11/17  02:43:06  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.10  1993/10/27  01:47:21  carbajal
 * ANSI'd function definitions
 *
 * Revision 1.9  1993/10/26  16:47:08  shala
 * Used type definition for the following functions in order to build
 * with R4.5 compiler:
 * nx_mkpart(), nx_mkpart_rect(), nx_mkpart_map(), nx_rmpart(), nx_chpart_mod(),
 * nx_chpart_epl(), nx_chpart_rq(), nx_chpart_owner(), nx_chpart_name().
 *
 * Revision 1.8  1993/09/01  01:08:52  carbajal
 * Fixed PTS #6471, 3622, 6435
 *
 * Revision 1.7  1993/07/20  17:53:41  carbajal
 * Use new allocator system calls
 *
 * Revision 1.6  1993/07/18  19:52:03  carbajal
 * Removed definitions of NX_STD and NX_GANG
 *
 * Revision 1.5  1993/03/01  20:03:31  carbajal
 * In changing owner or group check to see if arg == -1 and
 * adjust format string accordingly.
 * In rmpart and mkpart calls set errno if system() == -1
 *
 * Revision 1.4  1992/12/18  02:32:40  carbajal
 *  nx_rmpart()
 *                 add recurse flag to nx_rmpart.
 *                 don't set errno here let it come from
 *                 rmpart call.
 *                 let rmpart do all the work
 *         nx_chpart_owner:
 *                 if owner == -1 then call chpart -g
 *
 * Revision 1.3  1992/10/21  20:08:36  rkl
 * Changed format string for nx_chpart_epl() call.
 *
 * Revision 1.2  1992/10/21  17:30:25  rkl
 * Fixed format string in nx_chpart_mod().
 *
 * Revision 1.1  1992/10/01  17:53:41  rkl
 * Initial revision
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <allocsys.h>
#include <nx.h>
#include <nx/alloc_types.h>
#include <stdarg.h>

#include	<nx/allocator.h>
#include	<nx/defines.h>

long _nx_mkpart_attr(char *part, va_list ap);

/*
 *  nx_mkpart:
 *
 *	Make a partition based on a size.
 */
long
nx_mkpart( char *part, long size, long attrib)
{
	if (size == 0) {
		errno = EPBADNODE;	/* compat w/ pre-*_attr universe */
		return -1;
	}

	return nx_mkpart_attr(part,
		NX_ATTR_SZ, size,
		NX_ATTR_SCHED, attrib,
		NX_ATTR_END);

}

/*
 *  nx_mkpart_rect:
 *
 *	Make a partition based on rectangle dimensions.
 */
long nx_mkpart_rect(char *part, long rows, long cols, long attrib)
{
	nx_node_list_element_t nodes[2];

	nodes[0] = rows;
	nodes[1] = cols;
	return nx_mkpart_attr(part,
		NX_ATTR_RECT, nodes,
		NX_ATTR_SCHED, attrib,
		NX_ATTR_END);
}

/*
 *  nx_mkpart_map:
 *
 *	Make a partition based on a node map.
 */
long nx_mkpart_map(char *part, long size, long *map, long attrib)
{
	return nx_mkpart_attr(part,
		NX_ATTR_SZ, size,
		NX_ATTR_MAP, map,
		NX_ATTR_SCHED, attrib,
		NX_ATTR_END);
}

/*
 *  nx_mkpart_attr:
 *
 *	Make a partition.
 */
long
nx_mkpart_attr(char *part, ...)
{
	va_list ap;
	va_start(ap, part);

	return _nx_mkpart_attr(part, ap);
}


long
_nx_mkpart_attr(char *part, va_list ap)
{
	nx_nodes_t node_parse_list;
	nx_part_info_t partinfo;
	int relaxed;
	int selector_cnt;
	char* selector_buf;
	int selector_buflen;
	int size;
	int type;

	if (begin_parse_validation(ap) == -1)
		return -1;

	size = parse_for_map(&node_parse_list, &relaxed, ap);

	if (size == -1)
		return -1;

	parse_for_part_info(&partinfo, ap);

	selector_cnt = parse_for_selectors(&selector_buf,
	  &selector_buflen, ap);
	if (selector_cnt == -1)
		return -1;

	if (size == 0) {
		partinfo.nodes = -1;			/* nothing spec'ed */
		type = MKPART_ND;
	} else if (size == 2 && node_parse_list && node_parse_list[0] < 0) {
		partinfo.nodes = size;
		type = MKPART_SZ;			/* Rectangle */
	} else if (node_parse_list) {
		partinfo.nodes = size;
		type = MKPART_ND;			/* Map or anchored */
	} else {
		partinfo.nodes = size;
		type = MKPART_SZ;			/* Just a size */
	}

	if (check_args_used() == -1)
		return -1;

	/*
	 *  Check for invalid scheduling type here because -1
	 *  (NO_SCHED_SPECIFIED) is considered valid in NX_mkpart_attr().
	 */
	if (partinfo.sched != GANG && partinfo.sched != UNIX && 
	    partinfo.sched != SPACE_SHARE) {
		errno = EPINVALSCHED;
		return(-1);
	}

	return NX_mkpart_attr(type, part, partinfo, node_parse_list,
	  relaxed, selector_cnt, selector_buf, selector_buflen);
}

/*
 *  nx_rmpart:
 *
 *	Remove a partition.
 */
long nx_rmpart(char *part, long force, long recursion)
{
	return(NX_rmpart(part,recursion,force));
}

/*
 *  nx_chpart_mod:
 *
 *	Change the access (pertection) mode of a partition.
 */
long nx_chpart_mod(char *part, long mode)
{
	nx_part_info_t	partinfo;

	nx_init_partinfo(&partinfo);
	partinfo.flags_or_size = MODFLG;
	partinfo.access = mode;
	return(NX_chpart(part,partinfo,(char *)0));
}

/*
 *  nx_chpart_epl:
 *
 *	Change the effective priority level.
 */
long nx_chpart_epl(char *part, long priority)
{
	nx_part_info_t	partinfo;

	nx_init_partinfo(&partinfo);
	partinfo.flags_or_size = EPLFLG;
	partinfo.epl = priority;
	return(NX_chpart(part,partinfo,(char *)0));
}

/*
 *  nx_chpart_rq:
 *
 *	Change the rollin quantum.
 */
long nx_chpart_rq(char *part, long quantum)
{
	nx_part_info_t	partinfo;
	if (quantum < MINRQ) {
		errno = EPINVALSCHED;
		return -1;
	}
	nx_init_partinfo(&partinfo);
	partinfo.flags_or_size = RQFLG;
	partinfo.rq = quantum;
	return(NX_chpart(part,partinfo,(char *)0));
}

/*
 *  nx_chpart_owner:
 *
 *	Change the owner and group of a partition.
 */
long nx_chpart_owner(char *part, long owner, long group)
{
	nx_part_info_t	partinfo;

	nx_init_partinfo(&partinfo);
	partinfo.flags_or_size = OWNERFLG;
	partinfo.uid = (uid_t) owner;
	partinfo.gid = (gid_t) group;
	return(NX_chpart(part,partinfo,(char *)0));
}

/*
 *  nx_chpart_name:
 *
 *	Change the name of a partition
 */
long nx_chpart_name(char *old_part, char *new_part)
{
	nx_part_info_t	partinfo;

	nx_init_partinfo(&partinfo);
	partinfo.flags_or_size =  NMFLG;

	return(NX_chpart(old_part,partinfo,new_part));
}

/*
 *  nx_chpart_sched:
 *
 *	Change the scheduling characteristics for the partition
 */
long nx_chpart_sched(char *part, long sched_type)
{
	nx_part_info_t	partinfo;

	nx_init_partinfo(&partinfo);
	if (sched_type == SPACE_SHARE)
		partinfo.flags_or_size = SPSFLG;
	else 
	if (sched_type == GANG){
		partinfo.flags_or_size = RQFLG;
		partinfo.rq = 0;
	}
	else{
		errno = EPINVALSCHED;
		return(-1);
	}
	
	partinfo.sched = sched_type;
	return(NX_chpart(part,partinfo,(char *)0));
}
