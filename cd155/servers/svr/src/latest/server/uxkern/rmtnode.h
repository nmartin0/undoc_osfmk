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
 * This source file was created by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: rmtnode.h,v $
 * Revision 1.5  1994/11/18  20:49:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:43:52  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:05:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:32:20  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:55:56  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:44:32  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:55:05  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/08/13  19:21:38  rabii
 * 	Removed time fields from rmtnode since they are not used. (rabii)
 * 
 * Revision 2.3  92/01/14  11:23:45  roy
 * 	92/01/10  22:09:07  noemi
 * 	Added RMTNODE and RMT_TIMEOUT defines and extern of spec_rmtnodeops.
 * 
 * Revision 2.2  92/01/05  20:15:10  roy
 * 	1991/11/12  22:02:22  noemi
 * 	Initial revision
 * 
 */

#ifdef	OSF1_ADFS
#ifndef _UXKERN_RMTNODE_H_
#define _UXKERN_RMTNODE_H_

/*
 * The file defines the rmtnode data structure only used by remote special
 * files.
 */
struct	rmtnode {
	u_long		r_flag;
	struct vnode 	*r_vnode;
#ifdef  _KERNEL
	udecl_simple_lock_data(,r_lock)
#endif
};


/*
 * rmtnode flags.
 */
#define RMTUPD         	0x00000001 /* remote device was modified */
#define RMTACC         	0x00000001 /* access time to be updated */
#define RMTCHG         	0x00000004 /* rmtnode has been changed */
#define	RMTMOD		0x00000008 /* rmtnode has been modified */

/*
 * The rmtnode lock protects the fileds of the rmtnode.
 */
#define RMT_LOCK(rp)		usimple_lock(&(rp)->r_lock)
#define RMT_UNLOCK(rp)		usimple_unlock(&(rp)->r_lock)
#define RMT_LOCK_TRY(rp)	usimple_lock_try(&(rp)->r_lock)
#define RMT_LOCK_INIT(rp)	usimple_lock_init(&(rp)->r_lock)
#define RMT_LOCK_HOLDER(rp)	SLOCK_HOLDER(&(rp)->r_lock)

/*
 * Test to see if an anonymous vnode is a remote vnode agent.
 */
#define	RMTNODE(vp)		((vp)->v_op == &spec_rmtnodeops)

/*
 * Timeout for remote vnode agent syncs.
 */
#define RMT_TIMEOUT	60

#ifdef	_KERNEL
extern struct vnodeops spec_rmtnodeops;
#endif

#endif 	/* _UXKERN_RMTNODE_H_ */
#endif	/* OSF1_ADFS */
