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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: vfs_flock.c,v $
 * Revision 1.7  1994/11/18  20:50:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/01/11  18:25:55  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: less lint complaints
 *  Testing: compiled
 *  Module(s):
 * 	nfs/nfs_vnops.c
 * 	vfs/fifo_vnops.c
 * 	vfs/vfs_cache.c
 * 	vfs/vfs_flock.c
 * 	vfs/vfs_vnops.c
 * 	vfs/vfs_bio.c
 * 	vfs/vfs_subr.c
 * 	vfs/vfs_vio.c
 * 	vfs/spec_vnops.c
 * 	vfs/vfs_syscalls.c
 * 	vfs/vfs_lookup.c
 *
 * Revision 1.5  1993/07/14  18:46:16  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:09:05  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/06/01  22:19:20  nandy
 * Changed MAXEND to 0x80000000 from 0x7fffffff
 *
 * Revision 1.3  1993/05/06  20:32:00  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:37  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.7  93/06/10  11:29:15  klh
 * Change MAXEND from 0x7fffffff to 0x80000000 for locking (from Nandini)
 * 
 * Revision 2.6  93/03/23  12:47:42  condict
 * 	Fixed illegal references to various fields of the lock structure in the
 * 	defs of the SLEEPLCK macros.  Use the macros from kern/lock.h instead.
 *
 * Revision 1.2  1992/11/30  22:57:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:46:22  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:57:26  cfj
 * Bump major revision number.
 *
 * Revision 2.5  1992/04/07  13:46:47  pjg
 * 	Fixed casting in SLEEPLCK_LOCK_THREAD() to uthread_t to fix i860
 * 	builds (rabii)
 * 
 * Revision 2.4  91/12/16  21:10:21  roy
 * 	91/11/26  15:34:34  sp
 * 	Move xenix_sem_cleanup to correct place
 * 
 * Revision 2.3  91/10/14  13:26:43  sjs
 * 	91/09/13  12:52:00  sp
 * 	include uxkern/vm_param.h to find PAGE_SIZE
 * 
 * Revision 2.2  91/08/31  14:31:21  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/08/01  17:03:23  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10.3.3  91/02/26  12:06:04  gmf
 * 	Move reset of f_offset to end of locked() function.
 * 	This is a better place than inside the loop.
 * 	[91/02/25  10:43:18  gmf]
 * 
 * 	1) Don't reset f_offset to current value after sleeping;
 * 	   reset it to where it was before sleeping.  This makes
 * 	   SVVS test happy.
 * 	2) Don't touch vnode when inserting and deleting from
 * 	   sleeplcks chain.  This causes enforcement mode
 * 	   locks to be left in a bad state.
 * 	[91/02/20  16:32:37  gmf]
 * 
 * Revision 1.10.3.2  91/02/01  10:46:23  gmf
 * 	Modify set_vlocks to set VENF_LOCK flag.  Clear it
 * 	in clear_vlocks.
 * 	[90/11/15  15:00:18  gmf]
 * 
 * 	Change locked() function to unlock, and re-lock the
 * 	FP_IO_LOCK if it will sleep on the lock.  It also
 * 	needs to reset the offset, since fp->f_offset may
 * 	have changed during the sleep.  This changed the
 * 	interface to locked() slightly.
 * 	[90/11/06  12:44:16  gmf]
 * 
 * Revision 1.10  90/10/31  14:08:34  devrcs
 * 	POSIX says that fcntl with the argument GETLK returns with SEEK_SET.
 * 	We were converting the whence back to the passed in value.
 * 	[90/10/25  19:03:44  collins]
 * 
 * 	Added a forward declaration for xenix_blocked() to get rid of a
 * 	compiler warning.
 * 	[90/10/25  15:14:45  swallace]
 * 
 * 	added call to xenix_sem_cleanup() in cleanlocks() for xenix
 * 	compatibility
 * 	in insflck(), set the VXENIX bit in the vnode flag if a Xenix file lock
 * 	is created on a file.  added set_vxenix() and clear_vxenix()
 * 	for Xenix file locking.
 * 	added routine xenix_blocked() to detect when a Xenix lock will cause a
 * 	process to block.  It has different criteria from SVR5 file
 * 	locking.  See Locus Final Design Doc page 56 for details.
 * 	Also, added an argument to locked() for xenix compatibility.
 * 	[90/10/08  13:59:06  swallace]
 * 
 * Revision 1.9  90/10/07  15:00:34  devrcs
 * 	Make file lock zones exhaustible and return ENOLCK when a lock
 * 	can not be created.  All this to pass SVVS and fix bug #1105.
 * 	[90/10/02  11:42:01  jvs]
 * 
 * 	Fix problem in freefid(): the flip was not completely pulled
 * 	out of the doubly-linked list. Also, changed SLEEP() macro
 * 	to not reference the ptr argument after the wakeup - it may
 * 	have been ZFREE'd, and either reassigned or garbage collected.
 * 	[90/10/01  18:30:45  jeffc]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:55:10  gm]
 * 
 * Revision 1.8  90/09/23  16:01:31  devrcs
 * 	Added clear_vlocks and set_vlocks to clear or set VLOCKS to indicate
 * 	if any file locks are on the file.  Insflck() uses set_vlocks when a
 * 	lock is added.  Delflck() uses clear_vlocks  when the last file lock
 * 	is deleted.  Flckadj(), insflck() and delflck() get one more
 * 	argument: vp.  Added function locked().
 * 	Incorporated changes to SLEEP and WAKEUP.
 * 	[90/09/06  14:28:44  swallace]
 * 
 * Revision 1.7  90/09/13  11:51:57  devrcs
 * 	Fixed locking problem when using SLEEP()
 * 	Changed SLEEP to use tsleep()
 * 	Fixed the WAKEUP macro.  There were missing curly braces.
 * 	Fixes the assert_wait call in the SLEEP macro to inable interrupts.
 * 	Changed the local variable blkpid to be a pid_t instead of a int.
 * 	[90/08/29  16:30:07  collins]
 * 
 * Revision 1.6  90/07/27  09:09:51  devrcs
 * 	Bogus memory fixes.
 * 	[90/07/20  17:09:28  nags]
 * 
 * 	System V locking parallelization fixes.
 * 	[90/07/17  08:53:08  nags]
 * 
 * Revision 1.5  90/07/17  11:43:33  devrcs
 * 	Remove unexecuted code from allocfid.
 * 	[90/07/10  22:04:39  seiden]
 * 
 * Revision 1.4  90/06/22  20:56:34  devrcs
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Remove pageable flag from zinit argument list.		jvs@osf.org
 * 	Parallelized for OSF/1.					nags@encore.com
 * 	File layer parallelization changes.			noemi@osf.org
 * 	v_mount->m_fsid becomes m_stat.f_fsid (no hung fs).	gmf@osf.org
 * 	POSIX 1003.1 record and file locking support.		ers@osf.org
 * 	[90/06/12  21:43:29  nags]
 * 
 * $EndLog$
 */

#include <sys/param.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/mode.h>
#include <sys/time.h>
#include <sys/vnode.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/mount.h>
#include <sys/flock.h>
#include <sys/lock_types.h>
#include <uxkern/vm_param.h>
#include <kern/zalloc.h>
#include <kern/lock.h>

/* region types */
#define	S_BEFORE	010
#define	S_START		020
#define	S_MIDDLE	030
#define	S_END		040
#define	S_AFTER		050
#define	E_BEFORE	001
#define	E_START		002
#define	E_MIDDLE	003
#define	E_END		004
#define	E_AFTER		005

#define	SLEEP(ptr,retval)						\
MACRO_BEGIN								\
		struct flino *flip = ptr->flip;				\
		LASSERT(FICHAIN_LOCK_HOLDER(flip));			\
		ptr->stat.wakeflg++;					\
		assert_wait((int)&ptr->stat.wakeflg, TRUE);		\
		SLEEPLCK_UNLOCK();					\
		FICHAIN_UNLOCK(flip);					\
		retval = tsleep((caddr_t)0, (PZERO+1)|PCATCH, "flock", 0); \
		FICHAIN_LOCK(flip);					\
		SLEEPLCK_LOCK();					\
MACRO_END

#define	WAKEUP(ptr)							\
MACRO_BEGIN								\
		if (ptr->stat.wakeflg) {				\
			ptr->stat.wakeflg = 0;				\
			thread_wakeup((int)&ptr->stat.wakeflg);		\
		}							\
MACRO_END

#define l_end 		l_len
#define MAXEND  	0x80000000

struct	flckinfo flckinfo;		/* configuration and acct info */
struct	flino	*fids;			/* file id head list */
struct	flino	sleeplcks;		/* head of chain of sleeping locks */

#ifdef i386
struct filock *xenix_blocked();	/* forward declaration */
#endif

/*
 * Locking precedence:
 *
 *	FICHAIN_LOCK	(blocking lock)
 *	SLEEPLCK_LOCK	(blocking lock)
 *	FIDS_LOCK	(spin lock)
 *	FLCKINFO_LOCK	(spin lock)
 *	FLINO_LOCK	(spin lock)
 *
 * These are the locks associated with the different structures related to
 * the flock package.  The lock with the highest precedence, fichain_lock is
 * a mutex lock contained in the flino structure.  Each flino structure has
 * a chain of file locks associated with it (filock structures).  The
 * fichain_lock protects this chain.  The lock is a blocking lock because it
 * is held across blocking operations.  We want to hold it across these
 * operations to protect against races that can occur in flckadj() and other
 * functions that modify the chain, like delflck().  Using a blocking lock
 * allows us to hold that lock while flckadj() travels down the chain.
 * We chose to use a mutex lock rather than a read/write lock because most
 * of the time we traverse the chain in order to modify it, so a read/write
 * lock would not gain us much additional parallelism.  The parallelism through
 * this code is very coarse.  The fichain_lock also protects the entire filock
 * structure for each filock on its chain.  This is very coarse grain, but
 * only will have contention on multiple fcntl's going on with a single file.
 *
 * The sleeplck_lock is really an fichain_lock that protects the global
 * sleeplck structure.  The same locking holds true here, as it does
 * above.  The sleeplck_lock can be acquired by a thread holding a fichain
 * lock.
 *
 * The fids_lock has the next highest precedence.  This protects the global
 * fids list.  Fids is the head of a list of file ids (flino structs).  All
 * modifications to this list must be done under the fids_lock.  This is a
 * spin lock and is usually held for a relatively short duration, only to
 * traverse the list and do an insertion or deletion.
 *
 * The flckinfo_lock protects flckinfo structures.  The global 'flckinfo'
 * variable is protected by this lock.  This lock is used for very short
 * periods of time, only to increment or decrement counts, and is therefore
 * a spin lock.
 *
 * The flino_lock protects the rest of the flino structure.  That is, it
 * protects the refcnt field.  It is a spin lock.  It is infrequently used
 * and held for very short periods of time.
 *
 * It was necessary to add a new field to the filock structure.
 * The 'flip' field is a backpointer to the flino structure whose file
 * lock chain we are on.  This is necessary because we have to take
 * the fichain_lock when we remove ourselves from that list.
 * When we are in setflck(), we check to see if our new lock overlaps
 * (or is 'blocked') by another lock in the chain.  If it is, blocked()
 * returns that filock structure with the filock_lock LOCKED.  We then
 * record the timestamp and increment the sleepcnt in setflck() before
 * releasing the filock_lock.  There is the potential in setflck() that
 * we will block, waiting for the filock that is 'blocking' us to get
 * deleted or modified in some way.  When that happens, a 'wakeup' is posted
 * to wake us up.  However, in the MP case, it is possible that we are
 * racing with another thread who is doing the wakeup.  Therefore, we may
 * have found the blocking filock, but the other thread may post the wakeup
 * before we actually go to sleep.  The 'timestamp' field protects against
 * this.  The timestamp field is incremented everytime a wakeup is posted
 * on this filock.  Therefore, we record the current timestamp under lock,
 * and if it has changed before we want to go to sleep, we know the wakeup
 * has been posted, and we just continue.  The 'sleepcnt' field protects us
 * from a nasty race between setflck() and delflck().  One of the places
 * where a wakeup is posted is in delflck(), when an filock structure gets
 * deleted from the list and its memory is freed.  If blocked() returns us
 * this structure, and we are using it to check various conditions, and this
 * structure suddenly gets freed in the middle of this, bad things can happen.
 * So, again, when blocked returns us the locked filock structure in setflck(),
 * we increment the sleepcnt field.  In delflck, the other thread checks this
 * field and if it is set, that thread will decrement the count and post the
 * wakeup.  In setflck, when we either awaken or don't sleep due to the
 * timestamp changing, we also call delflck(), to actually complete the deletion
 * of this structure (or to post a wakeup, should another thread have found it
 * in the meantime).
 */
udecl_simple_lock_data(, fids_lock)
#define	FIDS_LOCK()		usimple_lock(&fids_lock)
#define	FIDS_UNLOCK()		usimple_unlock(&fids_lock)
#define	FIDS_LOCKINIT()		usimple_lock_init(&fids_lock)

#if	UNIX_LOCKS
#define SLEEPLCK_LOCKINIT()        lock_init2(&sleeplcks.fichain_lock, TRUE, LTYPE_FICHAIN)
#define SLEEPLCK_LOCK()            lock_write(&sleeplcks.fichain_lock)
#define SLEEPLCK_UNLOCK()          lock_write_done(&sleeplcks.fichain_lock)
#define SLEEPLCK_LOCKED()          LOCK_LOCKED(&sleeplcks.fichain_lock)

#define SLEEPLCK_LOCK_THREAD()     LOCK_THREAD(&sleeplcks.fichain_lock)
#define SLEEPLCK_LOCK_OWNER()      LOCK_OWNER(&sleeplcks.fichain_lock)
#define SLEEPLCK_LOCK_HOLDER()     LOCK_HOLDER(&sleeplcks.fichain_lock)
#else
#define SLEEPLCK_LOCKINIT()      
#define SLEEPLCK_LOCK()           
#define SLEEPLCK_UNLOCK()          
#define SLEEPLCK_LOCK_HOLDER()      
#endif

#if	MACH
/*
 * Zones used by lock package for dynamic data structures
 */
zone_t lockfile_zone;
zone_t lockrec_zone;
#endif


/* find file id */

struct flino *
findfid(fp)
struct file *fp;
{
	register struct flino *flip;
	register struct vnode *vp;

	ASSERT(fp != 0);
	vp = (struct vnode *) fp->f_data;
	FIDS_LOCK();
	flip = fids;
	while (flip != NULL) {
		if (flip->vp == vp) {
			FLINO_LOCK(flip);
			flip->fl_refcnt++;
			FLINO_UNLOCK(flip);
			break;
		}
		flip = flip->next;
	}
	FIDS_UNLOCK();
	return (flip);
}

print_fid(str, fp)
char *str;
struct fid *fp;
{
	register int i;

	printf("%s len= %d res= %x ", str, fp->fid_len, fp->fid_reserved);
	for (i = 0; i < fp->fid_len - 4; i++)
		printf("%x ", fp->fid_data[i]);
	printf("\n");
}

struct flino *
allocfid(fp)
struct file *fp;
{
	struct flino *flip, *flinop;

	ZALLOC(lockfile_zone, flip, struct flino *);
	if (flip != NULL) {
		FLCKINFO_LOCK(&flckinfo);
		++flckinfo.filcnt;
		++flckinfo.filtot;
		FLCKINFO_UNLOCK(&flckinfo);

		/* set up file identifier info */
		flip->fl_refcnt = 1;
		flip->vp = (struct vnode *)fp->f_data;
		flip->fl_flck = NULL;
		FLINO_LOCKINIT(flip);
		FICHAIN_LOCKINIT(flip);

		/* insert into allocated file identifier list */
		FIDS_LOCK();
#if	UNIX_LOCKS
		/*
		 * Someone else may have already inserted a flino entry onto
		 * the list with the same handle information.  Recheck the
		 * list to make sure it wasn't inserted while we were setting
		 * ours up.  If it was, deallocate ours and return the other
		 * one.
		 */
		
		flinop = fids;
		while (flinop != NULL) {
			if (flinop->vp == (struct vnode *)fp->f_data) {
				FLINO_LOCK(flinop);
				flinop->fl_refcnt++;
				FLINO_UNLOCK(flinop);
				FLCKINFO_LOCK(&flckinfo);
				--flckinfo.filcnt;
				--flckinfo.filtot;
				FLCKINFO_UNLOCK(&flckinfo);
				ZFREE(lockfile_zone, flip);
				FIDS_UNLOCK();
				return(flinop);

			}
			flinop = flinop->next;
		}
#endif
		if (fids != NULL)
			fids->prev = flip;
		flip->next = fids;
		flip->prev = NULL;
		fids = flip;
		FIDS_UNLOCK();

	}
	return (flip);
}

freefid(flip)
struct flino *flip;
{
	FLINO_LOCK(flip);
	if (--flip->fl_refcnt <= 0 && flip->fl_flck == NULL) {
		FLINO_UNLOCK(flip);
		FLCKINFO_LOCK(&flckinfo);
		--flckinfo.filcnt;
		FLCKINFO_UNLOCK(&flckinfo);
		FIDS_LOCK();
		if (flip->prev == NULL)
			fids = flip->next;
		if (flip->next != NULL)
			flip->next->prev = flip->prev;
		if (flip->prev != NULL)
			flip->prev->next = flip->next;
		FIDS_UNLOCK();
		ZFREE(lockfile_zone, flip);
	} else
		FLINO_UNLOCK(flip);
}
	

/* build file lock free list */

flckinit()
{
	flckinfo.fils = nfile / 4;
	flckinfo.recs = nfile;
	lockfile_zone = zinit(sizeof(struct flino),
			flckinfo.recs*sizeof(struct flino),
			PAGE_SIZE, "lockfile");
	if (lockfile_zone == (zone_t) NULL)
		panic("flckinit: no file zone");
	/* set lockfile_zone !pageable, !sleepable, exhaustible, collectable */
	zchange(lockfile_zone, TRUE, FALSE, TRUE, TRUE);

	flckinfo.filcnt = 0;

	lockrec_zone = zinit(sizeof(struct filock),
			flckinfo.recs*sizeof(struct filock),
			PAGE_SIZE, "lockrec");
	if (lockrec_zone == (zone_t) NULL)
		panic("flckinit: no rec zone");
	/* set lockrec_zone !pageable, !sleepable, exhaustible, collectable */
	zchange(lockrec_zone, TRUE, FALSE, TRUE, TRUE);

	flckinfo.reccnt = 0;
	FLCKINFO_LOCKINIT(&flckinfo);
	FIDS_LOCKINIT();
	SLEEPLCK_LOCKINIT();
}

/* insert lock after given lock using locking data */

#if	UNIX_LOCKS
/*
 * lckdat is a new lock.  No one else can know about it, therefore, it doesn't
 * require any locking in the MP case.
 */
#endif
struct filock *
insflck(flip, lckdat, fl, vp)
struct	flino	*flip;
struct	filock	*fl;
struct	flock	*lckdat;
struct  vnode   *vp;
{
	register struct filock *new;

	LASSERT(FICHAIN_LOCK_HOLDER(flip));
	ZALLOC(lockrec_zone, new, struct filock *);
	if (new != NULL) {
		FLCKINFO_LOCK(&flckinfo);
		++flckinfo.reccnt;
		++flckinfo.rectot;
		FLCKINFO_UNLOCK(&flckinfo);
		new->set = *lckdat;
		new->set.l_pid = u.u_procp->p_pid;
		new->stat.wakeflg = 0;
		new->flip = flip;
		if (fl == NULL) {
			new->next = flip->fl_flck;
			if (flip->fl_flck != NULL)
				flip->fl_flck->prev = new;
			flip->fl_flck = new;
		} else {
			new->next = fl->next;
			if (fl->next != NULL)
				fl->next->prev = new;
			fl->next = new;
		}
		new->prev = fl;
		/*
		 * Don't touch vnode if only dealing with sleeplcks
		 */
		if (flip != &sleeplcks) {
			set_vlocks(vp);
#ifdef i386
			if (lckdat->l_type & F_ENFRCD)
				set_vxenix(vp);
#endif
		}
	}
	return (new);
}

/* delete lock */

delflck(flip, fl, vp)
struct flino *flip;
struct filock *fl;
struct vnode  *vp;
{
	LASSERT(FICHAIN_LOCK_HOLDER(flip));

	if (fl->prev != NULL)
		fl->prev->next = fl->next;
	else
		flip->fl_flck = fl->next;
	if (fl->next != NULL)
		fl->next->prev = fl->prev;
	/*
	 * Don't touch vnode if only dealing with sleeplcks
	 */
	if ((flip != &sleeplcks) && (flip->fl_flck == NULL)) {
		/* no locks on file */
	        clear_vlocks(vp);
#ifdef i386
		clear_vxenix(vp);
#endif
	}
	WAKEUP(fl);

	FLCKINFO_LOCK(&flckinfo);
	--flckinfo.reccnt;
	FLCKINFO_UNLOCK(&flckinfo);

	ZFREE(lockrec_zone, fl);
}

/*
 * regflck sets the type of span of this (un)lock relative to the specified
 * already existing locked section.
 * There are five regions:
 *
 *  S_BEFORE        S_START         S_MIDDLE         S_END          S_AFTER
 *     010            020             030             040             050
 *  E_BEFORE        E_START         E_MIDDLE         E_END          E_AFTER
 *      01             02              03              04              05
 * 			|-------------------------------|
 *
 * relative to the already locked section.  The type is two octal digits,
 * the 8's digit is the start type and the 1's digit is the end type.
 */
/*
 * ld is a new lock.  No one else can know about it, therefore, it doesn't
 * require any locking in the MP case.
 */
int
regflck(ld, flp)
struct flock *ld;
struct filock *flp;
{
	register int regntype;

	LASSERT(FICHAIN_LOCK_HOLDER(flp->flip));
	if (ld->l_start > flp->set.l_start) {
		if (ld->l_start > flp->set.l_end) {
			return(S_AFTER|E_AFTER);
		} else if (ld->l_start == flp->set.l_end) {
			return(S_END|E_AFTER);
		} else
			regntype = S_MIDDLE;
	} else if (ld->l_start == flp->set.l_start)
		regntype = S_START;
	else
		regntype = S_BEFORE;

	if (ld->l_end > flp->set.l_start) {
		if (ld->l_end > flp->set.l_end)
			regntype |= E_AFTER;
		else if (ld->l_end == flp->set.l_end)
			regntype |= E_END;
		else
			regntype |= E_MIDDLE;
	} else if (ld->l_end == flp->set.l_start)
		regntype |= E_START;
	else
		regntype |= E_BEFORE;

	return (regntype);
}

/* Adjust file lock from region specified by 'ld' starting at lock 'insrtp' */

/*
 * ld is a new lock.  No one else can know about it, therefore, it doesn't
 * require any locking in the MP case.
 */
flckadj(flip, insrtp, ld, vp)
struct flino	*flip;
struct filock	*insrtp;
struct flock	*ld;
struct vnode    *vp;
{
	struct	flock	td;			/* lock data for severed lock */
	struct	filock	*flp, *nflp, *tdi, *tdp;
	int	insrtflg, rv = 0;
	int	regtyp;

	insrtflg = (ld->l_type != F_UNLCK) ? 1 : 0;

	LASSERT(FICHAIN_LOCK_HOLDER(flip));
	nflp = (insrtp == NULL) ? flip->fl_flck : insrtp;
	while (flp = nflp) {
		nflp = flp->next;
		if (flp->set.l_pid == u.u_procp->p_pid) {

			regtyp = regflck(ld, flp);

			/* release already locked region if necessary */

			switch (regtyp) {
			case S_BEFORE|E_BEFORE:
				nflp = NULL;
				break;
			case S_BEFORE|E_START:
				if (ld->l_type == flp->set.l_type) {
					ld->l_end = flp->set.l_end;
					if (insrtp == flp)
						insrtp = flp->prev;
					delflck(flip, flp, vp);
				} else
				nflp = NULL;
				break;
			case S_START|E_END:
				/* don't bother if this is in the middle of
				 * an already similarly set section.
				 */
				if (ld->l_type == flp->set.l_type) 
					return(rv);
				/* fall thru */
			case S_START|E_AFTER:
				insrtp = flp->prev;
				delflck(flip, flp, vp);
				break;
			case S_BEFORE|E_END:
				if (ld->l_type == flp->set.l_type)
					nflp = NULL;
				/* fall thru */
			case S_BEFORE|E_AFTER:
				if (insrtp == flp) 
					insrtp = flp->prev;
				delflck(flip, flp, vp);
				break;
			case S_BEFORE|E_MIDDLE:
				if (ld->l_type == flp->set.l_type)
					ld->l_end = flp->set.l_end;
				else {
					/* setup piece after end of (un)lock */
					td = flp->set;
					td.l_start = ld->l_end;
					tdp = tdi = flp;
					do {
						if (tdp->set.l_start < ld->l_start)
							tdi = tdp;
						else
							break;
					} while (tdp = tdp->next);
					if (insflck(flip, &td, tdi, vp) == NULL) {
						return(ENOLCK);
					}
				}
				if (insrtp == flp)
					insrtp = flp->prev;
				delflck(flip, flp, vp);
				nflp = NULL;
				break;
			case S_START|E_MIDDLE:
			case S_MIDDLE|E_MIDDLE:
				/* don't bother if this is in the middle of
				 * an already similarly set section.
				 */
				if (ld->l_type == flp->set.l_type)
					return(rv);
				/* setup piece after end of (un)lock */
				td = flp->set;
				td.l_start = ld->l_end;
				tdp = tdi = flp;
				do {
					if (tdp->set.l_start < ld->l_start)
						tdi = tdp;
					else
						break;
				} while (tdp = tdp->next);
				if (insflck(flip, &td, tdi, vp) == NULL) {
					return(ENOLCK);
				}
				if (regtyp == (S_MIDDLE|E_MIDDLE)) {
					/* setup piece before (un)lock */
					flp->set.l_end = ld->l_start;
					WAKEUP(flp);
					insrtp = flp;
				} else {
					insrtp = flp->prev;
					delflck(flip, flp, vp);
				}
				nflp = NULL;
				break;
			case S_MIDDLE|E_END:
				/* don't bother if this is in the middle of
				 * an already similarly set section.
				 */
				if (ld->l_type == flp->set.l_type)
					return(rv);
				flp->set.l_end = ld->l_start;
				WAKEUP(flp);
				insrtp = flp;
				break;
			case S_MIDDLE|E_AFTER:
			case S_END|E_AFTER:
				if (ld->l_type == flp->set.l_type) {
					ld->l_start = flp->set.l_start;
					insrtp = flp->prev;
					delflck(flip, flp, vp);
				} else {
					flp->set.l_end = ld->l_start;
					WAKEUP(flp);
					insrtp = flp;
				}
				break;
			case S_AFTER|E_AFTER:
				insrtp = flp;
				break;
			}
		} else {
			if (flp->set.l_start > ld->l_end)
				nflp = NULL;
		}
	}

	if (insrtflg) {
		if (flp = insrtp) {
			do {
				if (flp->set.l_start < ld->l_start)
					insrtp = flp;
				else
					break;
			} while (flp = flp->next);
		}
		if (insflck(flip, ld, insrtp, vp) == NULL)
			rv = ENOLCK;
	}

	return (rv);
}

/*
 * blocked checks whether a new lock (lckdat) would be
 * blocked by a previously set lock owned by another process.
 * When blocked is called, 'flp' should point
 * to the record from which the search should begin.
 * Insrt is set to point to the lock before which the new lock
 * is to be placed.
 */
#if	UNIX_LOCKS
/*
 * blocked returns with the lock LOCKED on the filock structure.
 * It is up to the calling routine to unlock the structure.
 *
 * blocked is called only from locked, getflck or setflck, with lckdat being a
 * new lock.  Since no one else can know of lckdat, it requires no locking.
 */
#endif
struct filock *
blocked(flp, lckdat, insrt)
struct filock *flp;
struct flock *lckdat;
struct filock **insrt;
{
	struct filock *f;

	*insrt = NULL;
	for (f = flp; f != NULL; ) {
		if (f->set.l_start < lckdat->l_start)
			*insrt = f;
		else {
			break;
		}
		if (f->set.l_pid == u.u_procp->p_pid) {
			if (lckdat->l_start <= f->set.l_end
			    && lckdat->l_end >= f->set.l_start) {
				*insrt = f;
				break;
			}
		} else	if (lckdat->l_start < f->set.l_end
			    && lckdat->l_end > f->set.l_start
			    && (f->set.l_type == F_WRLCK
				|| (f->set.l_type == F_RDLCK
				    && lckdat->l_type == F_WRLCK))) {
				return(f);
		}
		f = f->next;
	}

	for ( ; f != NULL; ) {
		if (lckdat->l_start < f->set.l_end
		    && lckdat->l_end > f->set.l_start
		    && f->set.l_pid != u.u_procp->p_pid
		    && (f->set.l_type == F_WRLCK
			|| (f->set.l_type == F_RDLCK && lckdat->l_type == F_WRLCK))) 
			return(f);
		if (f->set.l_start > lckdat->l_end) {
			break;
		}
		f = f->next;
	}

	return(NULL);
}

/* locate overlapping file locks */

#if	UNIX_LOCKS
/*
 * lckdat is a new lock, no one else can know about it, therefore, it doesn't
 * require any locking in the MP case.
 */
#endif
getflck(fp, lckdat)
struct file *fp;
struct flock *lckdat;
{
	register struct flino *flip;
	struct filock *found, *insrt = NULL;
	register int retval = 0;

	if (fp->f_type != DTYPE_VNODE)
		return(0);

	/* get file identifier and file lock list pointer if there is one */
	flip = findfid(fp);
	if (flip == NULL) {
		lckdat->l_type = F_UNLCK;
		return (0);
	}

	/* convert start to be relative to beginning of file */
	if (retval=convoff(fp, lckdat, L_SET))
		return (retval);
	if (lckdat->l_len == 0)
		lckdat->l_end = MAXEND;
	else
		lckdat->l_end += lckdat->l_start;

	/* find overlapping lock */

	FICHAIN_LOCK(flip);
	found = blocked(flip->fl_flck, lckdat, &insrt);
	LASSERT(FICHAIN_LOCK_HOLDER(flip));
	if (found != NULL) {
		*lckdat = found->set;
	} else
		lckdat->l_type = F_UNLCK;
	FICHAIN_UNLOCK(flip);
	freefid(flip);

	/* restore length */
	if (lckdat->l_end == MAXEND)
		lckdat->l_len = 0;
	else
		lckdat->l_len -= lckdat->l_start;

	retval = convoff(fp, lckdat, L_SET);
	return (retval);
}

/* clear and set file locks */

#if	UNIX_LOCKS
/*
 * lckdat is a new lock, no one else can know about it, therefore, it doesn't
 * require any locking in the MP case.
 */
#endif
setflck(fp, lckdat, slpflg)
struct file *fp;
struct flock *lckdat;
int slpflg;
{
	register struct flino *flip;
	register struct filock *found, *sf;
	struct filock *insrt = NULL;
	register int retval = 0;
	int contflg = 0;
	register struct vnode *vp;

	ASSERT(fp != 0);

	if (fp->f_type != DTYPE_VNODE)
		return(0);

	/* check access permissions */
	if ((lckdat->l_type == F_RDLCK && (fp->f_flag&FREAD) == 0)
	    || (lckdat->l_type == F_WRLCK && (fp->f_flag&FWRITE) == 0))
		return (EBADF);
	
	/* convert start to be relative to beginning of file */
	if (retval=convoff(fp, lckdat, 0))
		return (retval);
	if (lckdat->l_len == 0)
		lckdat->l_end = MAXEND;
	else
		lckdat->l_end += lckdat->l_start;

	/* get or create a file record lock header */
	flip = findfid(fp);
	if (flip == NULL) {
		if (lckdat->l_type == F_UNLCK)
			return (0);
		if ((flip=allocfid(fp)) == NULL)
			return (ENOLCK);	/* was EMFILE */
	}

	vp = (struct vnode *) fp->f_data;


	do {
		contflg = 0;
		switch (lckdat->l_type) {
		case F_RDLCK:
		case F_WRLCK:
			FICHAIN_LOCK(flip);
			if ((found=blocked(flip->fl_flck, lckdat, &insrt)) == NULL) {
				LASSERT(FICHAIN_LOCK_HOLDER(flip));
				retval = flckadj(flip, insrt, lckdat, vp);
			} else {
				LASSERT(FICHAIN_LOCK_HOLDER(flip));
				if (slpflg) {
				/* do deadlock detection here */
					SLEEPLCK_LOCK();
					if (deadflck(found))
						retval = EDEADLK;
					else if ((sf=insflck(&sleeplcks, lckdat, NULL, vp)) == NULL)
						retval = ENOLCK;
					else {
						sf->stat.blkpid = found->set.l_pid;
						SLEEP(found,retval);
						if (retval == 0) {
							contflg = 1;
						}
						sf->stat.blkpid = 0;
						delflck(&sleeplcks, sf, vp);
					}
					SLEEPLCK_UNLOCK();
				} else
					retval = EACCES;
			}
			FICHAIN_UNLOCK(flip);
			break;
		case F_UNLCK:
			/* removing a file record lock */
			FICHAIN_LOCK(flip);
			retval = flckadj(flip, flip->fl_flck, lckdat, vp);
			FICHAIN_UNLOCK(flip);
			break;
		default:
			retval = EINVAL;	/* invalid lock type */
			break;
		}
	} while (contflg);
	freefid(flip);
	return(retval);
}

/*
 * convoff - converts the given data (start, whence) to the
 * given whence.
 */
int
convoff(fp, lckdat, whence)
struct file *fp;
struct flock *lckdat;
int whence;
{
	struct 	vattr 	vattr;
	register off_t	offset;
	int error;

	VOP_GETATTR((struct vnode *)fp->f_data, &vattr, u.u_cred, error);
	BM(FP_IO_LOCK(fp));
	offset = fp->f_offset;
	BM(FP_IO_UNLOCK(fp));
	if (lckdat->l_whence == L_INCR)
		lckdat->l_start += offset;
	else if (lckdat->l_whence == L_XTND)
		lckdat->l_start += vattr.va_size;
	else if (lckdat->l_whence != L_SET)
		return (EINVAL);
	if (lckdat->l_start < 0)
		return (EINVAL);
	if (whence == L_INCR)
		lckdat->l_start -= offset;
	else if (whence == L_XTND)
		lckdat->l_start -= vattr.va_size;
	else if (whence != L_SET)
		return (EINVAL);
	lckdat->l_whence = whence;
	return (0);
}

/* deadflck does the deadlock detection for the given record */

int
deadflck(flp)
struct filock *flp;
{
	register struct filock *blck, *sf;
	pid_t blckpid;

	LASSERT(SLEEPLCK_LOCK_HOLDER());
	blck = flp;	/* current blocking lock pointer */
	blckpid = blck->set.l_pid;
	do {
		if (blckpid == u.u_procp->p_pid)
			return(1);
		/* if the blocking process is sleeping on a locked region,
		 * change the blocked lock to this one.
		 */
		for (sf = sleeplcks.fl_flck; sf != NULL; sf = sf->next) {
			if (blckpid == sf->set.l_pid) {
				blckpid = sf->stat.blkpid;
				break;
			}
		}
		blck = sf;
	} while (blck != NULL);
	return(0);
}

/* Clean up record locks left around by process (called in closef) */

cleanlocks(fp)
struct file *fp;
{
	register struct filock *flp;
	register struct flino *flip;

	flip = findfid(fp);
	if (flip == NULL)
		return;

	FICHAIN_LOCK(flip);
#ifdef i386
	xenix_sem_cleanup(flip);       /* for xenix compatibility */
#endif
	for (flp=flip->fl_flck; flp!=NULL;) {
		if (flp->set.l_pid == u.u_procp->p_pid) {
			delflck(flip, flp, (struct vnode *)fp->f_data);
			flp = flip->fl_flck;
		} else
			flp = flp->next;
	}
	FICHAIN_UNLOCK(flip);
	freefid(flip);

	return;
}

/*
 * locked() checks for enforcement mode blocking locks.  If check_wlck is set,
 * only write locks will block the operation (i.e., we are reading the file).
 * For writes, read or write locks will be blocking locks.
 */
int
locked(fp, no_sleep, uiop, check_wlck, man_lock)
struct file *fp;
int no_sleep, check_wlck, man_lock;
struct uio *uiop;
{

        struct flock lckdat;
	register struct filock *found, *sf;
	struct filock *insrt = NULL;
	struct flino *flip;
	int retval = 0;
	int contflg = 0;
	struct vnode *vp = (struct vnode *)fp->f_data;
#ifdef i386
	u_long v_flag;
#endif
	off_t count;

	count = uiop->uio_resid;
        lckdat.l_whence = L_SET;
	if (check_wlck)               
	      lckdat.l_type = F_RDLCK;   /* we will not be blocked by other
					    read locks */
	else
	      lckdat.l_type = F_WRLCK;

	flip = findfid (fp);
	if (flip == NULL)
	      return (0);

	/*
	 * We start from the same file offset, regardless of whether
	 * we block and someone else changes the f_offset on us.
	 * This effectively makes this an atomic (seek,read|write).
	 */
        lckdat.l_start = uiop->uio_offset = fp->f_offset;
	/* If 0, then entire file */
        if (count)
	    	lckdat.l_len = count+lckdat.l_start;
        else
	 	lckdat.l_len = MAXEND;
	do {
		contflg = 0;
		/* find overlapping lock */
		FICHAIN_LOCK(flip);
		if (man_lock)
			found = blocked(flip->fl_flck, &lckdat, &insrt);
#ifdef i386
		else {
			found = xenix_blocked(flip->fl_flck, &lckdat, &insrt);
			if (found) {
				BM(VN_LOCK(vp));
				v_flag = vp->v_flag;
				BM(VN_UNLOCK(vp));
				/* if a SVR3 process is blocked by a Xenix lock
				   on a file that does not have enforcement
				   mode file locking turned on, sleep even 
				   though FNDELAY is set (pg 55 of
				   Locus Final Design Doc) */
				if ((!(v_flag & VENF_LOCK)) && 
				    (u.u_procp->cxenix == NULL))
					no_sleep = 0;
			}
		}
#endif
		LASSERT(FICHAIN_LOCK_HOLDER(flip));
		if (found == NULL)
		      goto out;
		if (no_sleep) {
		      retval = EAGAIN;
		      goto out;
		}

		/* do deadlock detection here */
		SLEEPLCK_LOCK();
		if (deadflck(found))
			retval = EDEADLK;
		else if ((sf=insflck(&sleeplcks, &lckdat, NULL,
			     (struct vnode *) fp->f_data)) == NULL)
			retval = ENOLCK;
		else {
			sf->stat.blkpid = found->set.l_pid;	
			FP_IO_UNLOCK(fp);
			SLEEP(found,retval);
			FP_IO_LOCK(fp);
			if (retval == 0)
				contflg = 1;
			sf->stat.blkpid = 0;
			delflck(&sleeplcks, sf, (struct vnode *)fp->f_data);
		}
		SLEEPLCK_UNLOCK();

out:
		FICHAIN_UNLOCK(flip);
	} while (contflg);
	if (retval == 0) {
		/*
		 * Since the f_offset could have changed on us when
		 * we released the FP_IO_LOCK, we need to reset it, so
		 * the calling code does not need to change.  This
		 * amounts to an implicit lseek, which only changes
		 * things if we did, in fact, sleep on a lock.
		 * Only reset if we woke up gracefully.
		 */
		FP_LOCK(fp);
		fp->f_offset = uiop->uio_offset;
		FP_UNLOCK(fp);
	}
	freefid(flip);
        return(retval);
}


clear_vlocks(vp)
struct vnode *vp;
{
	VN_LOCK(vp);
	vp->v_flag &= ~(VLOCKS|VENF_LOCK);
	VN_UNLOCK(vp);
} 


set_vlocks(vp)
struct vnode *vp;
{
	struct vattr attr;
	int error;

	/*
	 * Set VENF_LOCK flag if enforcement mode locking is to take
	 * place.  It gets cleared in clear_vlocks and update_venf_lock.
	 *
	 * NB. This function is called with the FICHAIN lock held, which
	 * is OK, since it's a blocking lock.
	 */
	VOP_GETATTR(vp, &attr, u.u_cred, error);
	VN_LOCK(vp);
	if (error == 0) {
		if ((attr.va_mode & S_ISGID) && !(attr.va_mode & S_IXGRP))
			vp->v_flag |= VENF_LOCK;
	}
	vp->v_flag |= VLOCKS;
	VN_UNLOCK(vp);
}
				 
#ifdef i386
set_vxenix(vp)
struct vnode *vp;
{
	VN_LOCK(vp);
	vp->v_flag |= VXENIX;
	VN_UNLOCK(vp);
}

clear_vxenix(vp)
struct vnode *vp;
{
	VN_LOCK(vp);
	vp->v_flag &= ~(VXENIX);
	VN_UNLOCK(vp);
} 

/*
 * xenix_blocked checks whether a new lock (lckdat) would be
 * blocked by a previously set lock owned by another process.
 * When xenix_blocked is called, 'flp' should point
 * to the record from which the search should begin.
 * Insrt is set to point to the lock before which the new lock
 * is to be placed.
 */
#if	UNIX_LOCKS
/*
 * xenix_blocked returns with the lock LOCKED on the filock structure.
 * It is up to the calling routine to unlock the structure.
 *
 * xenix_blocked is called only from locked, with lckdat being a
 * new lock.  Since no one else can know of lckdat, it requires no locking.
 */
#endif
struct filock *
xenix_blocked(flp, lckdat, insrt)
struct filock *flp;
struct flock *lckdat;
struct filock **insrt;
{
	struct filock *f;

	*insrt = NULL;
	for (f = flp; f != NULL; ) {
		if (f->set.l_start < lckdat->l_start)
			*insrt = f;
		else {
			break;
		}
		if (f->set.l_pid == u.u_procp->p_pid) {
			if (lckdat->l_start <= f->set.l_end
			    && lckdat->l_end >= f->set.l_start) {
				*insrt = f;
				break;
			}
		} else	if (lckdat->l_start < f->set.l_end
			    && lckdat->l_end > f->set.l_start
			    && (f->set.l_type == (F_WRLCK | F_ENFRCD)
				|| (f->set.l_type == (F_RDLCK | F_ENFRCD)
				    && lckdat->l_type == F_WRLCK))) {
				return(f);
		}
		f = f->next;
	}

	for ( ; f != NULL; ) {
		if (lckdat->l_start < f->set.l_end
		    && lckdat->l_end > f->set.l_start
		    && f->set.l_pid != u.u_procp->p_pid
		    && (f->set.l_type == (F_WRLCK | F_ENFRCD)
			|| (f->set.l_type == (F_RDLCK | F_ENFRCD)
			&& lckdat->l_type == F_WRLCK))) 
			return(f);
		if (f->set.l_start > lckdat->l_end) {
			break;
		}
		f = f->next;
	}

	return(NULL);
}

#endif
