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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: dvp_debug.c,v $
 * Revision 1.6  1995/02/01  21:40:27  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:43:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:32:20  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:43:52  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:21:31  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:44:33  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:46:53  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:45:16  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.6  1992/09/29  07:36:11  roman
 * Add debugging for private virtual process system operations (pvpsops).
 *
 * Revision 3.5  92/06/26  14:46:27  chrisp
 * [Bug #30] Vproc tracing can't assume u.u_procp is not NULL.
 * 
 * Revision 3.4  92/03/12  08:58:02  klh
 * dpvpoptrace now functions like syscalltrace (0=>none;-1=>all;n=>pid) (chrisp)
 * 
 * Revision 3.3  91/11/22  10:15:45  chrisp
 * pvpop tracing message format brought in line with vpop tracing.
 * 
 * Revision 3.2  91/11/19  10:08:39  chrisp
 * Increase the number of possible PVPOP arguments to 8.
 * 
 * Revision 3.1  91/11/18  11:30:18  chrisp
 * VPROC debugging format string in per-thread uarea.
 * 
 * Revision 3.0  91/10/25  10:24:49  roman
 * First appearance of this file in the tnc directory. Used to be in
 * pvproc directory which is being deleted.
 * 
 * Revision 2.3  91/10/18  18:31:33  chrisp
 * 	More explanatory debugging messages implemented.
 * 
 * Revision 2.2  91/09/16  16:07:15  rabii
 * 	Initial Checkin (locus check-in b chrisp)
 * 
 *
 */
#include <sys/vproc.h>
#include <sys/user.h>
#include <sys/errno.h>
#include <tnc/dpvproc.h>

int dpvpoptrace = 0;

/*
 * Routines used for debug printing of pvproc operations. See the file
 * <sys/pvproc.h> for details.
 */

int
dpvproc_db1(vp, op, fmt)
	struct vproc *vp;
	char *op, *fmt;
{
	if (dpvpoptrace && (dpvpoptrace == vp->vp_pid || dpvpoptrace < 0)) {
		struct proc *procp = u.u_procp;
		u.uu_pvpargfmt = fmt;
		printf("[%d]%s<%d>", procp ? procp->p_pid: - 1, op, vp->vp_pid);
	}
	return(ESUCCESS);
}

int
dpvproc_db2(vp, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
	struct vproc *vp;
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8;
{
	if (dpvpoptrace && (dpvpoptrace == vp->vp_pid || dpvpoptrace < 0))
		printf(u.uu_pvpargfmt,vp,
			arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
	return(ESUCCESS);
}

int
dpvproc_db3(vp, retval)
	struct vproc *vp;
	int retval;
{
	if (dpvpoptrace && (dpvpoptrace == vp->vp_pid || dpvpoptrace < 0))
		printf("   returns(%d)\n",retval);
	return(retval);
}

int
dpvps_db1(op, fmt)
	char *op, *fmt;	
{
	if (dpvpoptrace < 0) {
		struct proc *procp = u.u_procp;
		u.uu_vpargfmt = fmt;
		printf("[%d]%s", procp ? procp->p_pid : -1, op);
	}
	return(0);
}

int
dpvps_db2(arg1, arg2, arg3, arg4, arg5, arg6)
	int arg1, arg2, arg3, arg4, arg5, arg6;
{
	if (dpvpoptrace < 0)
		printf(u.uu_vpargfmt,arg1,arg2,arg3,arg4,arg5,arg6);
	return(0);
}

int
dpvps_db3(retval)
	int retval;
{
	if (dpvpoptrace < 0)
		printf("   returns(%d)\n",retval);
	return(retval);
}
