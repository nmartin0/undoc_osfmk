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
 * $Log: vp_subr.c,v $
 * Revision 1.10  1994/11/18  20:51:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1993/07/14  18:47:48  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.5  1993/07/01  21:11:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.8  1993/05/06  19:11:41  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.7  1993/04/09  15:29:42  cfj
 * Merge with T9.5
 *
 * Revision 1.5.4.1  1993/04/08  23:03:29  cfj
 * VPROC LOCK fix from Locus.
 *
 * Revision 1.6  1993/04/03  03:13:28  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.1  1993/02/16  20:09:27  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 * 
 * Revision 1.5  1993/02/03  01:00:14  nandy
 * Removed #ifdef VPROC_DEBUG in vproc_new to all the vp_next to be set.
 *
 * Revision 1.4  1993/01/22  18:19:02  cfj
 * 01-20-93 Locus code drop.
 *
 * Revision 2.13  92/12/29  13:46:20  chrisp
 * The vproc table is now dynamically allocated. When allocating a vproc,
 * 	if the free list is empty, a new vproc is zalloc()'ed from the
 * 	vproc zone with a new pvproc being zalloc()ed from the pvproc
 * 	zone and attached to the new vproc.
 * 
 * Revision 1.1.2.2  1992/11/06  20:35:03  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.1.3  1993/05/03  17:55:38  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.14  1993/04/22  16:43:54  roman
 * [Bug 224] Timing window with vproc_release() where the vproc lists
 * 	can end up being corrupted.
 *
 * Revision 2.13  92/12/29  13:46:20  chrisp
 * The vproc table is now dynamically allocated. When allocating a vproc,
 * 	if the free list is empty, a new vproc is zalloc()'ed from the
 * 	vproc zone with a new pvproc being zalloc()ed from the pvproc
 * 	zone and attached to the new vproc.
 * 
 * Revision 2.12  92/10/09  10:04:17  roman
 * Add procedure prototype for clean i860 compilation.
 * 
 * Revision 2.11  92/09/29  08:36:18  roman
 * Initialize and invoke vproc locks using new locking macros.
 * 
 * Revision 2.10  92/09/21  14:16:06  chrisp
 * Alteration of release/free logic to avoid holding the vproc hash list
 * locked over VPOP_FREE() which potentially does a remote free to the vproc's
 * origin node.
 * 
 * Revision 2.9  92/03/20  16:08:53  chrisp
 * Set magic number for allocated vprocs.
 * 
 * Revision 2.8  92/03/12  15:53:38  roman
 * Vproc reference and free/hash list process changed to use a per-vproc
 * 	reference counting mutex and a single list lock.
 * 
 * Revision 2.7  92/01/07  11:22:01  chrisp
 * Correct call to vproc_db4 adding 4th parm.
 * 
 * Revision 2.6  91/12/24  10:39:28  roman
 * Pay attention to return code of VPOP_FREE(), which can now return
 * an error indicating the vproc cannot be freed at this time.
 * 
 * Revision 2.5  91/11/01  17:29:05  roman
 * Change the call to vproc_db4 to have the correct number of
 * parameters. Call VPOP_FREE when freeing a vproc to give the pvproc
 * layer a chance to do something.
 * 
 * Revision 2.4  91/10/30  12:57:45  roman
 * Change the name of the routine vprocloc() to locate_vproc_pid().
 * 
 * Revision 2.3  91/10/25  10:51:29  roman
 * Added new routine vproc_dealloc(). Change routines to consistant
 * declaration style.
 * 
 * Revision 2.2  91/09/16  16:25:53  rabii
 * 	Initial Checkin (locus check-in by roman)
 * 
 *
 */
#include <uxkern/import_mach.h>
#include <sys/types.h>
#include <sys/unix_defs.h>
#include <sys/vproc.h>
#include <sys/errno.h>
#include <kern/zalloc.h>

int vproc_hold(struct vproc *);		/* Forward reference */
struct vproc *vproc_new();		/* Forward reference */

vp_lock_t	vproc_list_lock = VP_SLOCK_T_INITIALIZER;
	
struct vproc *
vprocptr(
	register pid_t		pid)
{
	register struct vproc	*v;

	VPROC_LIST_LOCK();
	for (v = vproc_hash[VPROCPIDHASH(pid)]; v != 0; v = v->vp_hashfwd)
		if (v->vp_pid == pid)
			break;
	VPROC_LIST_UNLOCK();
	return(v);
}

struct vproc *
locate_vproc_pid(
	register pid_t		pid)
{
	register struct vproc *v;

	/*
	 * Look through the hash list for a particular pid. Note that we
	 * should never search for a pid of -1, since this is a special
	 * case of an invisible vproc.
	 */
	ASSERT(pid != (pid_t)-1);
	VPROC_LIST_LOCK();
	for (v = vproc_hash[VPROCPIDHASH(pid)]; v != 0; v = v->vp_hashfwd)
		if (v->vp_pid == pid) {

			/*
			 * We probably found the vproc. In rare cases though,
			 * the vproc may be in the process of being deleted. 
			 * In this case we may not see that the pid has been
			 * changed to -1 until the refcnt lock is locked. 
			 * If the vproc is in the process of being deleted, 
			 * then simply pretend we never found the vproc.
			 */
			VPROC_REFCNT_LOCK(v);
			if (v->vp_pid == (pid_t)-1) {
				VPROC_REFCNT_UNLOCK(v);
				VPROC_LIST_UNLOCK();
				return(NULL);
			}
			v->vp_ref_cnt++;
			VPROC_REFCNT_UNLOCK(v);
			break;
		}
	VPROC_LIST_UNLOCK();
	return(v);
}

struct vproc *
vproc_alloc()
{
	register struct vproc *v;
	int error;

	VPROC_LIST_LOCK();
	v = vprocfree;
	if (v == NULL)
		v = vproc_new();
	vprocfree = v->vp_hashfwd;
	v->vp_ref_cnt = 0;
	v->vp_magic = VP_MAGIC;
	VPROC_LIST_UNLOCK();
	
	return(v);
}

void
vproc_dealloc(
	struct vproc	*vp)
{
	VPROC_LIST_LOCK();
	if (vp) {
		vp->vp_hashfwd = vprocfree;
		vprocfree = vp;
		vp->vp_magic = 0;
	}
	VPROC_LIST_UNLOCK();
}

int
vproc_release(
	struct vproc *v)
{
	int			error;
	pid_t			save_pid;

	/*
	 * Take the vproc lock while decrementing.
	 */
	VPROC_REFCNT_LOCK(v);
	v->vp_ref_cnt--;
	if (v->vp_ref_cnt > 0) {
		VPROC_REFCNT_UNLOCK(v);
		return(ESUCCESS);
	}

	/*
	 * Allow the underlying code its chance to do freeing. Note that
	 * the underlying code may decide that a VPOP_FREE() operation
	 * is inappropriate at this time, in which case we leave the vproc
	 * lying around with a zero reference count. The idea is that
	 * at some future time the reference count will be incremented
	 * and decremented again, allowing a "true" free to be done.
	 */
	error = VPOP_FREE(v);
	if (error != ESUCCESS) {
		VPROC_REFCNT_UNLOCK(v);
		return(error);
	}

	/*
	 * All looks fair to remove the vproc from the hash list.
	 * However, this requires us get the list lock and so the 
	 * reference count lock must be unlocked and re-acquired after
	 * getting the list lock. We wish to make this process invisible
	 * while we remove it, so we set the pid to -1 (the code in
	 * LOCATE_VPROC_PID() must take this into account).
	 */
	save_pid = v->vp_pid;
	v->vp_pid = (pid_t)-1;
	VPROC_REFCNT_UNLOCK(v);
	VPROC_LIST_LOCK();
	VPROC_REFCNT_LOCK(v);

	/*
	 * Check to make sure things haven't changed while we were
	 * unlocked.
	 */
	ASSERT(v->vp_ref_cnt == 0);
	ASSERT(v->vp_pid == (pid_t)-1);

	/* 
	 * Take the dead vproc out of the hash list
	 */
	if (v->vp_hashfwd != NULL)
		v->vp_hashfwd->vp_hashbwd = v->vp_hashbwd;
	if (v->vp_hashbwd != NULL)
		v->vp_hashbwd->vp_hashfwd = v->vp_hashfwd;
	else
		vproc_hash[VPROCPIDHASH(save_pid)] = v->vp_hashfwd;

	/* 
	 * Put the vproc on the free chain 
	 */
	v->vp_hashfwd = vprocfree;
	vprocfree = v;

	VPROC_REFCNT_UNLOCK(v);
	VPROC_LIST_UNLOCK();
	return(ESUCCESS);
}

int
vproc_hold(
	struct vproc *v)
{
	VPROC_REFCNT_LOCK(v);
	v->vp_ref_cnt++;
	VPROC_REFCNT_UNLOCK(v);
	return(ESUCCESS);
}

/*
 * Put a brand new vproc into the dynamic vproc table
 *	BUT NOT onto the free list.
 */
struct vproc *
vproc_new()
{
	extern struct vproc *vproc, *vprocNVPROC;
	extern zone_t vproc_zone, pvproc_zone;
	register struct vproc *v = (struct vproc *) zalloc(vproc_zone);

	v->vp_data = (caddr_t) zalloc(pvproc_zone);
	VPROC_REFCNT_LOCK_INIT(v);

	if (vproc == NULL)
		vproc = v;

	/* 
	 * Removed #ifdef VPROC debug to allow nx_tam_wait() to follow
	 * the next field.
	 */
/* #ifdef	VPROC_DEBUG */
	else
		vprocNVPROC->vp_next = v;
	v->vp_next = NULL;
	v->vp_index = nvproc;
/*#endif */	/* VPROC_DEBUG */

	nvproc++;

	vprocNVPROC = v;

	return(v);
}
