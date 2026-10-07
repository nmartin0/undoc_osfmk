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
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * $Log: pp_procset.c,v $
 * Revision 1.5  1994/11/18  20:51:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:47:33  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:11:05  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:11:27  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:58:43  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:47:23  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:58:31  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:55:32  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.0  1992/09/28  16:12:35  roman
 * Initial submission of file that has subroutines for determining
 * process set membership.
 *
 */

#include <sys/types.h>
#include <sys/proc.h>
#include <sys/procset.h>


/*
 * See if the process matches the id and idtype specified.
 */
int
proc_in_proc_subset(
	struct proc	*p,
	int		idtype,
	long		id)
{
	switch (idtype) {
		case P_PID:
			return((pid_t) id == p->p_pid);
		case P_PGID:
			return((pid_t) id == p->p_pgid);
		case P_SID:
			return((pid_t) id == p->p_sid);
		case P_UID:
			return((uid_t) id == p->p_uid);
		case P_GID:
			return((gid_t) id == p->p_gid);
		case P_ALL:
			return(TRUE);
		case P_NONE:
			return(FALSE);
		default:
			panic("is_proc_in_proc_subset: invalid idtype %d",
			      idtype);
			/* NOTREACHED */
	}
}


/*
 * See if the process falls within the specified procset.
 */
int
proc_in_procset(
	struct proc	*p,
	struct procset	*ps)
{
	boolean_t	proc_on_left, proc_on_right;
	
	proc_on_left = proc_in_proc_subset(p, ps->p_lidtype, ps->p_lid);
	proc_on_right = proc_in_proc_subset(p, ps->p_ridtype, ps->p_rid);

	switch (ps->p_op) {
		case POP_DIFF:
			return(proc_on_left & !(proc_on_right));
		case POP_AND:
			return(proc_on_left & proc_on_right);
		case POP_OR:
			return(proc_on_left | proc_on_right);
		case POP_XOR:
			return(proc_on_left ^ proc_on_right);
		default:
			panic("proc_in_procset: invalid p_op=%d", ps->p_op);
			/* NOTREACHED */
	}
}
