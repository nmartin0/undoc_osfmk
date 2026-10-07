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
 * $Log: procset.h,v $
 * Revision 1.4  1994/11/18  20:41:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:26:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:35:37  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:11  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:51  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:44  cfj
 * Bump major revision number.
 *
 * Revision 3.1  1992/10/01  10:25:24  roman
 * Fix file so it can be included multiple times without harm.
 *
 * Revision 3.0  92/09/28  16:05:14  roman
 * Initial submission of file with process set definitions.
 * 
 */

#ifndef	_SYS_PROCSET_H_
#define _SYS_PROCSET_H_

/*
 * Structure defining set of processes being operated upon.
 */
struct procset {
	int	p_op;
	int	p_lidtype;
	long	p_lid;
	int	p_ridtype;
	long	p_rid;
};

/*
 * Values for p_lidtype and p_ridtype, which indicate the type of
 * p_lid and p_rid respectively.
 */
#define	P_PID		1	/* id is a process id */
#define	P_PGID		2	/* id is a process group id */
#define	P_SID		3	/* id is a session id */
#define P_UID		4	/* id is an effective user id */
#define P_GID		5	/* id is an effective group id */
#define P_ALL		6	/* all processes; id is ignored */
#define P_NONE		7	/* no processes; id is ignored */

/*
 * Values for p_op, which indicate how processes matching (p_lidtype, p_lid) 
 * are to be combined with the processes matching (p_ridtype, p_rid) to give
 * the resulting processor set.
 */
#define POP_DIFF	1	/* processes in left grp and not in right grp */
#define POP_AND		2	/* processes in both left grp in right grp */
#define POP_OR		3	/* processes in either left grp or right grp */
#define POP_XOR		4	/* processes in either left grp or right grp 
				   but not in both */

#endif	/* _SYS_PROCSET_H_ */
