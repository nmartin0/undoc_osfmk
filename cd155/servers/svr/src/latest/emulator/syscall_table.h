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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: syscall_table.h,v $
 * Revision 1.5  1994/11/18  20:24:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:31:47  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.3  1993/07/01  18:24:41  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:15:34  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:18:41  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:09:20  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:16:14  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 4.1  1992/11/03  23:58:11  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/12/16  09:55:32  roy
 * 	Remove CMUCS conditionals (sp).
 *
 * Revision 2.2  91/08/30  16:40:53  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.1  91/02/27  16:24:29  condict
 * Eliminate CMU syscalls.
 * 
 * Revision 3.0  91/01/17  12:05:17  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.3  90/06/19  23:07:11  rpd
 * 	Added pid_by_task.
 * 	[90/06/14            rpd]
 * 
 * Revision 2.2  89/10/17  11:24:24  rwd
 * 	Added sysent_init_process.
 * 	[89/09/29            rwd]
 * 
 */
/*
 * Definition of system call table.
 */
struct sysent {
	int	nargs;		/* number of arguments, or special code */
	int	(*routine)();
};

/*
 * Special arguments:
 */
#define	E_GENERIC	(-1)
				/* no specialized routine */
#define	E_CHANGE_REGS	(-2)
				/* may change registers */

/*
 * Exported system call table
 */
extern struct sysent	sysent[];	/* normal system calls */
extern int		nsysent;

extern struct sysent	sysent_task_by_pid;
extern struct sysent	sysent_pid_by_task;
extern struct sysent	sysent_htg_ux_syscall;
extern struct sysent	sysent_init_process;
