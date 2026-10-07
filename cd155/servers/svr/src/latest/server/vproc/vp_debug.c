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
 * HISTORY
 * $Log: vp_debug.c,v $
 * Revision 1.5  1994/11/18  20:51:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:47:36  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:11:09  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:11:32  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:58:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:47:27  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:58:34  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:55:34  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.10  1992/09/29  08:35:04  roman
 * Add debugging for virtual process system operations (vpsops).
 *
 * Revision 2.9  92/06/26  14:46:40  chrisp
 * [Bug #30] Vproc tracing can't assume u.u_procp is not NULL.
 * 
 * Revision 2.8  92/03/12  11:24:21  klh
 * dpvpoptrace now functions like syscalltrace (0=>none;-1=>all;n=>pid)
 * 
 * Revision 2.7  92/01/07  11:21:28  chrisp
 * Add panic if NULL vproc pointer specified.
 * 
 * Revision 2.6  91/11/14  15:01:31  chrisp
 * Vproc debugging message format now saved in per-thread uarea.
 * 
 * Revision 2.5  91/11/04  12:55:37  roman
 * Eliminated vproc_db5() and add extra parameter vproc_db4() with an
 * adjustment count.
 * 
 * Revision 2.4  91/11/01  17:28:24  roman
 * Change db4 and db5 to be able to be called before the vp_ref_cnt field
 * is manipulated rather than before.
 * 
 * Revision 2.3  91/10/18  18:39:36  chrisp
 * 	More informative messages implemented.
 * 
 * Revision 2.2  91/09/16  16:25:38  rabii
 * 	Initial Checkin (locus -in by roman)
 * 
 *
 */
#include <sys/vproc.h>
#include <sys/user.h>

/*
 * Routines used for debug printing of vproc operations. See the file
 * <sys/vproc.h> for details.
 */

int vpoptrace = 0;
int vprefcnt = 0;

int
vproc_db1(vp, op, fmt)
	struct vproc *vp;
	char *op, *fmt;	
{
	if (vpoptrace && (vpoptrace == vp->vp_pid || vpoptrace < 0)) {
		struct proc *procp = u.u_procp;
		u.uu_vpargfmt = fmt;
		printf("[%d]%s<%d>", procp ? procp->p_pid : -1, op,
				     vp ? vp->vp_pid : -1);
	}
	return(0);
}

int
vproc_db2(vp, arg1, arg2, arg3, arg4, arg5, arg6)
	struct vproc *vp;
	int arg1, arg2, arg3, arg4, arg5, arg6;
{
	if (vpoptrace && (vpoptrace == vp->vp_pid || vpoptrace < 0))
		printf(u.uu_vpargfmt,vp,arg1,arg2,arg3,arg4,arg5,arg6);

	if (vp == NULL)
		panic("vproc_db2: NULL vproc");

	return(0);
}

int
vproc_db3(vp, retval)
	struct vproc *vp;
	int retval;
{
	if (vpoptrace && (vpoptrace == vp->vp_pid || vpoptrace < 0))
		printf("   returns(%d)\n",retval);
	return(retval);
}

int
vproc_db4(vp, str1, str2, adj)
	struct vproc *vp;
	char *str1, *str2;
	int adj;
{
	if (vprefcnt && (vprefcnt == vp->vp_pid || vprefcnt < 0))
		printf("%s %s: v=0x%x[%d], ref_cnt=%u\n", 
		       str1, str2, 
		       vp, vp->vp_pid, vp->vp_ref_cnt+adj);
	return(0);
}

int
vps_db1(op, fmt)
	char *op, *fmt;	
{
	if (vpoptrace < 0) {
		struct proc *procp = u.u_procp;
		u.uu_vpargfmt = fmt;
		printf("[%d]%s", procp ? procp->p_pid : -1, op);
	}
	return(0);
}

int
vps_db2(arg1, arg2, arg3, arg4, arg5, arg6)
	int arg1, arg2, arg3, arg4, arg5, arg6;
{
	if (vpoptrace < 0)
		printf(u.uu_vpargfmt,arg1,arg2,arg3,arg4,arg5,arg6);
	return(0);
}

int
vps_db3(retval)
	int retval;
{
	if (vpoptrace < 0)
		printf("   returns(%d)\n",retval);
	return(retval);
}
