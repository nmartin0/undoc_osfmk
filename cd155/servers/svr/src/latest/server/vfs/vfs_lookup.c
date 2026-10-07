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
 * Copyright (c) 1991 Center for High Performance Computing of
 *	Worcester Polytechnic Institute
 *	Rights granted to OSF/RI under contract.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: vfs_lookup.c,v $
 * Revision 1.11  1995/02/01  22:32:56  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/11/18  20:50:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/01/11  18:26:36  jlitvin
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
 * Revision 1.8  1994/01/04  01:47:32  jlitvin
 * Don't redirect a FIFO that we're in the process of unlinking.
 *
 *  Reviewer: Locus (mjl)
 *  Risk: Low
 *  Benefit or PTS #: 7193
 *  Testing: bug report test
 *  Module(s): server/vfs/vfs_lookup.c
 *
 * Revision 1.7  1993/12/10  22:18:18  nina
 * Fixed bugs that prevented Paragons from being used
 * as NFS clients if the boot node is not a network
 * server node.  See #6831, #6719, #7421, #7422, #7423
 * #7424 and #7426.  TNC FIFO hook in namei() erroneously
 * assumed that ni_cdir field in struct nameidata is never
 * NULL. It can be if current directory is remote.
 *
 *
 *  Reviewer:bolsen@locus.com, dbm@intel.com
 *  Risk:Medium
 *  Benefit or PTS #:#7422
 *  Testing:Lachman NFS main suite, various configs, Locus network testing
 *  Module(s):./server/vfs/vfs_lookup.c
 *
 * Revision 1.6  1993/07/14  18:46:29  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:09:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  20:32:05  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:47  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.10  1993/04/30  14:10:00  klh
 * Changes from durriya:
 *   Revision 2.10  93/04/29  10:18:45  durriya
 *   	In the ADFS case, for we need to do some additional processing
 *   	before starting the dirloop to handle mount on mount correctly
 *   	also get rid of all BM() macros - they areof no use.
 *   	[93/04/26            durriya]
 *
 * Revision 2.9  93/04/24  18:45:51  klh
 * 	Revision 2.9  93/04/06  11:57:43  rabii
 * 		Eliminate special cases from check to turn name caching off
 * 		in namei.  Now only turn it off if DELETE or NOCACHE flag set.
 * 
 * 	Revision 2.7  92/12/08  10:47:59  durriya
 * 		Implemented new mount/unmount/pathname translation synchronization.
 * 		Involves elimination of bypassvp and vmount* code and replacing
 * 		it with unmount synchronization to prevent unmounting of
 * 		filesystems while translating.  Most of code is here and in
 * 		vfs_syscalls.c (mount and unmount).                 (durriya)
 * 
 * Revision 1.4  1993/04/03  03:12:56  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:08:46  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.3  1993/01/15  02:40:56  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1  1992/11/05  23:46:32  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.8  92/12/10  17:38:13  mjl
 * Use new debug macros.
 * 
 * Revision 2.7  92/09/28  13:37:26  klh
 * Use conditional printfs for TNC namei() hooks.  Have the "throw" hook wait
 * if the namei() redirection port is not yet set.  Allow redirection to be
 * cancelled (and waiting "throw" threads proceed normally). (klh for mjl).
 * 
 * Revision 2.6  92/08/18  07:49:29  mjl
 * Added TNC hooks for redirecting namei() lookups on FIFOs to the remote
 * FIFO storage site, if any.
 * 
 * Revision 2.5  92/03/01  18:45:00  pjg
 * 	Moved the test for "stripslash" out of the "if (getbuf)"
 * 	conditional.
 * 
 * Revision 2.4  92/01/14  10:57:41  roy
 * 	OSF1_ADFS mods to namei to support non-local directories (pjg).
 * 
 * Revision 2.3  92/01/05  20:00:38  roy
 * 	1991/11/12  19:27:19  noemi
 * 	Changed m_rootport to m_remoteport.  Fixed typos (naresh).
 * 
 * 	1991/10/14  20:15:27  noemi
 * 	Changes to handle HASCOMPBUF and HASPATHBUF namei options, as well as
 * 	the MOUNT namei option.  Changed ni_nextport to ni_forwport.
 * 
 * 	1991/09/22  18:34:32  noemi
 * 	Added MOUNT option handling to namei.  Ifdefed out bypassvp fix since 
 * 	it doesn't distribute and only partially solves deadlocks.  Fixed OSF/1
 * 	and OSF1/AD bugs.
 * 
 * 	1991/09/16  19:28:32  noemi
 * 	Changes to support remote mount.  Also moved nullcompat support to the
 * 	emulation library for OSF1_ADFS.
 * 
 * Revision 2.2  91/08/31  14:31:39  rabii
 *      Initial V2.0 Checkin
 *
 * Revision 3.2  91/08/01  17:03:43  sp
 * Upgrade to 1.0.2
 *
 * Revision 1.13.4.4  91/03/11  12:18:35  gmf
 *      Add wantparent check to reference of rootdir
 *      on lookup of "/".
 *      [91/03/08  10:17:00  gmf]
 *
 * Revision 1.13.4.3  91/03/01  11:33:02  gmf
 *      1.  Change to cache file names with CREATE set
 *          under some conditions (already exist).
 *      2.  Deal with case of wantparent and path is "/".
 *          Callers assume that if wantparent is set and
 *          there is no error, ni_dvp is valid.
 *          A previous "fix" broke this.
 *      [91/03/01  11:23:11  gmf]
 *
 * Revision 1.13.3.2  91/01/31  15:06:55  gmf
 *      1.  Removed printnullcompat code, but left in nullcompat
 *          itself, for now.
 *      2.  Fixed bug 1645, related to trailing slashes.  The
 *          current semantic will correctly allow them for
 *          directories, but not for regular files.
 *      [91/01/31  14:46:41  gmf]
 *
 * Revision 1.13  90/11/03  10:43:07  devrcs
 *      disable nullcompat and printnullcompat
 *      [90/10/31  13:29:14  gmf]
 *
 *      1.  Namei was incorrectly synchronizing with mount and unmount.
 *      2.  Namei now recognizes a "bypassvp", the target vnode
 *          mount is attempting to mount onto, to prevent deadlock
 *          on malicious mount pathname combinations.
 *      3.  Minor bogus memory and assertion clean ups.
 *      [90/10/24  14:38:06  nags]
 *
 *      Change namei to take a new operation, STRIPSLASH, which
 *      tells it to strip all trailing slashes.  It was doing
 *      this by default, which is not in keeping with the
 *      current interpretation of POSIX 1003.1 (page 32) for
 *      regular files.
 *      [90/10/13  12:45:22  gmf]
 *
 *      fixed chroot problem
 *      [90/10/12  18:37:27  gmf]
 *
 * Revision 1.12  90/10/07  15:00:46  devrcs
 *      Enable "printnullcompat" by default.
 *      [90/10/04  09:22:23  tmt]
 *
 *      Added EndLog Marker.
 *      [90/09/28  11:55:28  gm]
 *
 *      Do not try and reference the first character of a pathname when
 *      stripping trailing /'s if the null pathname "" is passed.
 *      [90/09/27  15:04:34  sp]
 *
 *      fix up assertions
 *      [90/09/24  13:01:19  gmf]
 *      Must re-take spin lock on dp if vmountread blocks
 *      during crossing of mount point in namei.
 *      [90/09/24  11:15:50  gmf]
 *
 * Revision 1.11  90/09/23  16:01:39  devrcs
 *      Remove any trailing '/'s from the pathname before lookup for POSIX
 *      which allows directories to terminate with any number of them.
 *      [90/09/19  13:44:40  sp]
 *
 *      Delete checks for 8th bit being set on file names. Now we are
 *      internationalized these may be OK so we must allow them.
 *      [90/09/17  17:06:22  sp]
 *
 *      If vmountread() blocked when we are about to cross a mount point,
 *      we need to verify that we are still at the root of a file system.
 *      [90/09/12  15:27:47  noemi]
 *
 * Revision 1.10  90/09/13  11:52:05  devrcs
 *      Map ENOENT to ENAMETOOLONG if returned from copy*str in namei().
 *      [90/08/31  10:41:44  gmf]
 *
 * Revision 1.9  90/08/24  12:30:29  devrcs
 *      Add curlies to assertion.
 *      [90/08/20  15:50:42  tmt]
 *
 *      Fix syntax error.
 *      [90/08/20  08:40:19  gmf]
 *
 *      Print out system call for printnullcompat (PMAX only).
 *      [90/08/20  06:57:57  gmf]
 *
 *      include file changes for MP support
 *      [90/08/15  18:36:33  hosking]
 *
 * Revision 1.8  90/08/09  13:29:57  devrcs
 *      Added code to return ENOENT for null string lookup; however, for
 *      compatibility, this feature is only enabled with a global variable, whic
h
 *      is defaulted on (nullcompat = 1, which means don't error on "").
 *      Also added global (printnullcompat) to print out information about
 *      processes attempting to lookup a null string.
 *      [90/08/02  13:11:24  gmf]
 *
 * Revision 1.7  90/07/27  09:09:57  devrcs
 *      Permit namei to re-use a pathname buffer.
 *      [90/07/20  17:09:40  nags]
 *
 * Revision 1.6  90/07/17  11:43:43  devrcs
 *      Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 *      [90/07/10  22:04:46  seiden]
 *
 *      More changes for gcc.
 *      [90/07/07  22:43:50  gm]
 *
 * Revision 1.5  90/06/22  20:56:42  devrcs
 *      Post-nags-merge bug fixes
 *      [90/06/18  09:58:20  seiden]
 *
 *      nags merge
 *
 *      Condensed history (reverse chronology):
 *      Secureware changes.                             seiden@osf.org
 *      Parallelized for OSF/1.                         nags@encore.com
 *      Integrated 4.4BSD file system changes [1/5/90]. noemi@osf.org
 *      Fixes for first snapshot.                       gm@osf.org
 *      New networking code from BSD.                   tmt@osf.org
 *      [90/06/12  21:43:46  gmf]
 *
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)vfs_lookup.c	7.18 (Berkeley) 8/10/89
 */

#include <sys/secdefines.h>
#if	SEC_MAC
#include <sys/security.h>
#endif
#if	SEC_AUDIT_SYMLINKS
#include <kern/lock.h>
#include <sys/audit.h>
#endif

#include <sys/param.h>
#include <sys/time.h>
#include <sys/user.h>
#include <sys/namei.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/errno.h>

#ifdef	TNC
#include <tnc/un_ff.h>
#endif

#include <sys/user.h>
#include <kern/zalloc.h>
#ifdef	OSF1_ADFS
#include <kern/kalloc.h>
#endif

/*
 * Convert a pathname into a pointer to a locked inode.
 * This is a very central and rather complicated routine.
 *
 * The flag argument is LOOKUP, CREATE, RENAME, or DELETE depending on
 * whether the name is to be looked up, created, renamed, or deleted.
 * When CREATE, RENAME, or DELETE is specified, information usable in
 * creating, renaming, or deleting a directory entry may be calculated.
 *
 * The FOLLOW flag is set when symbolic links are to be followed
 * when they occur at the end of the name translation process.
 * Symbolic links are always followed for all other pathname
 * components other than the last.
 *
 * The segflg defines whether the name is to be copied from user
 * space or kernel space.
 *
 * Overall outline of namei:
 *
 *	copy in name
 *	get starting directory
 * dirloop:
 *	copy next component of name to ndp->ni_dent
 *	handle degenerate case where name is null string
 *	if .. and on mounted filesys, find parent
 *	call lookup routine for next component name
 *	  component vnode returned in ni_vp (if it exists), locked.
 *	if symbolic link, massage name in buffer and continue at dirloop
 *	if result inode is mounted on, find mounted on vnode
 *	if more components of name, do next level at dirloop
 *	return the answer in ni_vp as locked vnode;
 *
 */

#ifdef 	OSF1_ADFS
/*
 * nullcompat is handled by the emulation library.
 */
#else
/*
 * XXX -- temporary hacking to get binary compatibility for lookup of
 *        null string ("");  Remove this when no longer required!
 */
int nullcompat = 0;
/* end hack */
#endif

#define LAST_ASSERT(mp)	ASSERT((mp) == lastlocked)

namei(ndp)
	register struct nameidata *ndp;
{
	register char *cp;		/* pointer into pathname argument */
	register struct vnode *dp = 0;	/* the directory we are searching */
	register int i;		   	/* Temp counter */
	struct vnode *tdp;		/* saved dp */
	struct mount *mp;		/* mount table entry */
	int docache;			/* == 0 do not cache last component */
	int flag;			/* LOOKUP, CREATE, RENAME or DELETE */
	int wantparent;			/* 1 => wantparent flag */
#ifdef	OSF1_ADFS
	int haspathbuf = 0;		/* caller has buf for path */
	int hascompbuf = 0;		/* caller has buf for component */
	int getbuf = 0;			/* 1 => allocate a pathname buffer */
#else
	int getbuf;			/* 1 => allocate a pathname buffer */
#endif
	int rdonly;			/* mounted read-only flag bit(s) */
	int error = 0;
#ifndef	OSF1_ADFS
	int hadslash = 0;
#endif
	struct mount *lastlocked = NULLMOUNT;	/* last mp locked */
	int stripslash;			/* strip trailing slashes */

	ndp->ni_dvp = NULL;
	flag = ndp->ni_nameiop & OPFLAG;
	wantparent = ndp->ni_nameiop & WANTPARENT;
	stripslash = ndp->ni_nameiop & STRIPSLASH;
	docache = (ndp->ni_nameiop & NOCACHE) ^ NOCACHE;
#ifdef	OSF1_ADFS
	if ((ndp->ni_nameiop & (HASCOMPBUF|HASPATHBUF)) == 0)
		getbuf++;
	hascompbuf = ndp->ni_nameiop & HASCOMPBUF;
	haspathbuf = ndp->ni_nameiop & HASPATHBUF;
#else
	getbuf = (ndp->ni_nameiop & HASBUF) ^ HASBUF;
#endif

#ifdef	TNC
	/*
	 *  Catch FIFO remote storage node redirects.  See TNC code
	 *  below, and S_fsvr_open() et. al.
	 */
	dp = (struct vnode *) ndp->ni_cdir;
	if (dp && dp->v_tncdata && V_STRG_ONLY(dp) && (flag != DELETE)) {
		UNFFDEBUG(U_NAMEI, ("VREDIRECT: dp 0x%x: catch\n", dp));
		ASSERT(dp != MACH_PORT_NULL &&
		       dp->v_type == VFIFO &&
		       dp->v_magic == V_MAGIC);
		VREF(dp);
		ndp->ni_vp = dp;
		return (0);
	}
	dp = 0;
#endif	/* TNC */

	/*
	 * OK to cache hits for CREATE as long as they are positive.
	 * This must be handled by the fs-dependent lookups.
	 */
	if (flag == DELETE)
		docache = 0;
	rdonly = M_RDONLY;
	if (ndp->ni_nameiop & REMOTE)
		rdonly |= M_EXRDONLY;
	/*
	 * Get a buffer for the name to be translated, and copy the
	 * name into the buffer.
	 */
	if (getbuf) {
		ZALLOC(pathname_zone,ndp->ni_pnbuf, caddr_t);
		if (ndp->ni_segflg == UIO_SYSSPACE)
			error = copystr(ndp->ni_dirp, ndp->ni_pnbuf, 
					MAXPATHLEN, &ndp->ni_pathlen);
		else
			error = copyinstr(ndp->ni_dirp, ndp->ni_pnbuf, 
					  MAXPATHLEN, &ndp->ni_pathlen);
		if (error) {
			ZFREE(pathname_zone, ndp->ni_pnbuf);
			ndp->ni_vp = NULL;
			if (error == ENOENT)
				error = ENAMETOOLONG;
			return (error);
		}
		ndp->ni_ptr = ndp->ni_pnbuf;
	}
	/*
	 * Strip any trailing slashes if requested.  POSIX
	 * only requires this for directories.  The only 
	 * operations that should be specifying this are those
	 * that manipulate directories (mkdir, rmdir, rename).
	 * Trailing slashes for regular files generate an error.
	 * Note that stripping the slashes makes "foo/" look
	 * like "foo", rather than "foo/.".  This is not
	 * the same as the old behavior.
	 * Note that the pathlen includes the null character.
	 */
	if (stripslash) {
		cp = ndp->ni_pnbuf + ndp->ni_pathlen - 2;
		/*
		 * catch the degenerate name "/"
		 */
		while ((ndp->ni_pathlen > 2) && (*cp == '/')) {
			*cp-- = 0;
			ndp->ni_pathlen--;
		}
	}
#if	SEC_BASE
        /*
         * MP note:  This is pendable
         */
	audstub_pathname(ndp->ni_pnbuf, ndp->ni_pathlen);
#endif
	ndp->ni_loopcnt = 0;
	dp = ndp->ni_cdir;
#ifdef OSF1_ADFS
	/*
	 * The current directory may be remote but the pathname
	 * absolute, in which case we will vrele the vnode right away.
	 */
	if (dp == NULLVP  &&  *ndp->ni_ptr != '/') {
		error = EREMOTE;
		goto bad3;
	}
	if (dp)
#endif
        {
		VREF(dp);
        }
	/*
	 * Synchronization with unmount:  we use the mount structure's
	 * UNMOUNT_LOCK() to synchronize with unmount() operations.
	 * However, the only time we need to take this lock is when
	 * crossing mount points.  The fact that we have a referenced
	 * vnode (e.g. cdir or /) keeps an unmount from completing 
	 * successfully (at least until we exit this function).
	 * When mount points are crossed, we attempt to take the lock on
	 * the new mount structure.  If it's busy, we wait until it's
	 * clear and try again (see below, where we do this).  Once we
	 * have the lock, unmount() operations will block until it is
	 * released.  If, when we release the lock, we have a newly 
	 * referenced vnode on the filesystem, the unmount() will fail,
	 * but that's the way it is supposed to work.
	 * We could just take the unmount lock all the time, but this
	 * way the case of a lookup which crosses no mount points is
	 * optimized.
	 */
start:
	/*
	 * Get starting directory.
	 * Done at start of translation and after symbolic link.
	 */
	if (*ndp->ni_ptr == '/') {
#ifdef OSF1_ADFS
		if (dp)
#endif
		{
                        if (lastlocked != NULLMOUNT) {
                                UNMOUNT_READ_UNLOCK(lastlocked);
                                LAST_ASSERT(dp->v_mount);
                        }
                        lastlocked = NULLMOUNT;	/* we don't lock the root */
			vrele(dp);
		}
		while (*ndp->ni_ptr == '/') {
			ndp->ni_ptr++;
			ndp->ni_pathlen--;
		}
		if ((dp = ndp->ni_rdir) == NULL) {
			dp = rootdir;
#ifdef	OSF1_ADFS
			if (dp == NULL) {
			/*
			 * If rootdir is remote, don't panic and
			 * return EREMOTE instead. This may be caused
			 * by a bogus client sending a message on the cwd
			 * port with an absolute pathname (the emulation lib
			 * tests this case, but is not protected...).
			 */
			/* ASSERT(dp != NULLVP); */
				printf ("Null rootdir, returnig EREMOTE...\n");
				ndp->ni_forwport = ndp->ni_rdirport;
				ASSERT(ndp->ni_forwport != MACH_PORT_NULL);
				error = EREMOTE;
				goto bad3;
			}
#endif
		}
		VREF(dp);
#ifndef	OSF1_ADFS
		hadslash = 1;
#endif

	}
	ndp->ni_endoff = 0;
#ifdef OSF1_ADFS
        /* 
         * In the case of ADFS the fsvr servicing the directory only
         * knows about the first remote mounted partition.
         * Subsequent mounts on the same directory are recorded in the
         * fsvr servicing the previous mounted partition.
         * So if /mnt is serviced by A and the disk partiion /dev/f1 by
         * B, subsequent mount of /dev/f2 is recorded on node B.
         * When a lookup of /mnt/... is forwarded from A, the vnode
         * corresponds to /dev/f1. We need to get to the vnode 
         * corresponding to /dev/f2.
         * So we have to do the following which is very similar to
         * what is done later in the loop whenever a mount point is
         * encountered. Check the comments out there for more details.
         */

        if ((ndp->ni_ptr[0] == '.')  && (ndp->ni_ptr[1] == '.')) {
                /* 
                 * If we are trying to get to the parent don't resolve to the
                 * next mount point. We are tracing our steps back from the
                 * last mounted filesystems to reach our parent.
                 */
        } else {
                VN_LOCK(dp);
                if ((dp->v_type == VDIR) &&
                    (ndp->ni_nameiop & NOCROSSMOUNT) == 0) {
                        while (mp = dp->v_mountedhere) {
                                LASSERT(VN_LOCK_HOLDER(dp));
                                ASSERT(dp->v_flag & VMOUNTPOINT);
                                
                                if (!UNMOUNT_TRY_READ(mp)) {
                                        /*
                                         * mount point is being unmounted, wait
                                         * for it, then start the loop again.
                                         */
                                        VN_UNLOCK(dp);
                                        thread_block();
                                        VN_LOCK(dp);
                                        continue;
                                }
                                VN_UNLOCK(dp);
                                /*
                                 * If the filesystem is remotely mounted, leave 
                                 * the rest of the pathname in ndp->ni_ptr and 
                                 * the remote root vnode port in 
                                 * ndp->ni_forwport. Then return EREMOTE.
                                 */
                                if (mp->m_flag & M_REMOTE_FS) {
                                        /*
                                         * since we have not done any further 
                                         * lookups we don't need to set
                                         * ni_ptr to ni_next
                                         */
                                        ndp->ni_forwport = mp->m_remoteport;
                                        ASSERT(ndp->ni_forwport != MACH_PORT_NULL);
                                        UNMOUNT_READ_UNLOCK(mp);
                                        error = EREMOTE;
                                        goto bad;
                                }
                                
                                VFS_ROOT(mp, &tdp, error);
                                if (error) {
                                        UNMOUNT_READ_UNLOCK(mp);
                                        goto bad;
                                }
                                ASSERT(tdp != NULLVP && 
                                       tdp->v_mount != NULLMOUNT);
                                ASSERT(tdp->v_flag & VROOT);
                                
                                if (lastlocked != NULLMOUNT) {
                                        UNMOUNT_READ_UNLOCK(lastlocked);
                                        LAST_ASSERT(dp->v_mount);
                                }
                                lastlocked = mp;
                                vrele(dp);
                                ndp->ni_vp = dp = tdp;
                                VN_LOCK(dp);
                        } /* end of while (mp = dp->v_mountedhere) */
                } /* end of if NOCROSSMOUNT */
                VN_UNLOCK(dp);
        }
#endif
        
        /*
         * We come to dirloop to search a new directory.
         */
dirloop:
	/*
	 * Copy next component of name to ndp->ni_dent.
	 * XXX kern_exec looks at d_name
	 * ??? The ni_hash value may be useful for vfs_cache
	 * XXX There must be the last component of the filename left
	 * somewhere accessible via. ndp for NFS (and any other stateless file
	 * systems) in case they are doing a CREATE. The "Towards a..." noted
	 * that ni_ptr would be left pointing to the last component, but since
	 * the ni_pnbuf gets free'd, that is not a good idea.
	 */
#ifdef	OSF1_ADFS
	if (!hascompbuf)
#else
	if (getbuf)
#endif
	{
		ndp->ni_hash = 0;
		for (cp = ndp->ni_ptr, i = 0; *cp != 0 && *cp != '/'; cp++) {
			if (i >= NAME_MAX) {
				error = ENAMETOOLONG;
				goto bad;
			}
			ndp->ni_dent.d_name[i++] = *cp;
			ndp->ni_hash += (unsigned char)*cp * i;
		}
		ndp->ni_namelen = i;
		ndp->ni_dent.d_namlen = i;
		ndp->ni_dent.d_name[i] = '\0';
		ndp->ni_pathlen -= i;
		ndp->ni_next = cp;
#ifdef NAMEI_DIAGNOSTIC
		printf("{%s}: ", ndp->ni_dent.d_name);
#endif
	}
	cp = ndp->ni_next;
	ndp->ni_makeentry = 1;
	if (*cp == '\0' && docache == 0)
		ndp->ni_makeentry = 0;
	ndp->ni_isdotdot = (ndp->ni_namelen == 2 &&
		ndp->ni_dent.d_name[1] == '.' && ndp->ni_dent.d_name[0] == '.');

	/*
	 * Check for degenerate name (e.g. / or "").
	 */
	if (ndp->ni_ptr[0] == '\0') {
#ifdef	OSF1_ADFS
		/* NULL pathnames may be sent by fileservers forwarding
		 * operations on remote mount points or special files.
		 * If the entire pathname is translated before the operation
		 * is forwarded, a NULL pathname will be forwarded.
		 * NULL pathnames passed to system calls by users are
		 * detected by the emulation library.
                 */
#else
		/*
		 * POSIX demands that a path of "" generate an error of
		 * ENOENT.  Any leading "/" characters have been removed,
		 * and remembered by the hadslash variable.
		 */
		if (hadslash == 0 && !nullcompat) {
			error = ENOENT;
			goto bad;
		}
		/*
		 * Must have been "/" or trailing "/", which
		 * is only valid for directories.
		 * Note: ni_dvp may be referenced (if wantparent).
		 */
		if (dp->v_type != VDIR) {
			error = ENOTDIR;
			goto bad;
		}
		/*
		 * Can't delete/rename mount points or root vnodes.
		 */
		if ((flag == DELETE || flag == RENAME) &&
		    ((dp->v_flag & VROOT) || (dp->v_flag & VMOUNTPOINT))) {
			error = EBUSY;
			goto bad;
		}
#endif /* OSF1_ADFS */

		if (getbuf)
			ZFREE(pathname_zone, ndp->ni_pnbuf);

		if ((ndp->ni_dvp == NULLVP) && wantparent) {
#ifndef OSF1_ADFS
			/* Had to be "/" -- let caller clean up */
			ASSERT((dp == rootdir) || (dp == ndp->ni_rdir));
#endif
			ndp->ni_dvp = dp;
			VREF(dp);
		}
		ndp->ni_vp = dp;
		if (lastlocked != NULLMOUNT) {
			UNMOUNT_READ_UNLOCK(lastlocked);
			LAST_ASSERT(dp->v_mount);
		}
		return (0);
	}

#if	SEC_MAC
checkdotdot:
#endif
	/*
	 * Handle "..": three special cases.
	 * 1. If at root directory (e.g. after chroot)
	 *    then ignore it so can't get out.
	 * 2. If this vnode is the root of a locally
	 *    mounted file system (OSF1_ADFS distinction only), then
	 *    replace it with the vnode which was mounted on
	 *    so we take the .. in the other file system.
	 * 3. In OSF1_ADFS, if the vnode is the root of a remote
	 *    filesystem, we return the vnode port for the root
	 *    on the remote filesystem and an EREMOTE error.
	 */
	if (ndp->ni_isdotdot) {
		for (;;) {
			if (dp == ndp->ni_rdir || dp == rootdir) {
				ndp->ni_dvp = dp;
				ndp->ni_vp = dp;
				VREF(dp);
				goto nextname;
			}
			if (((dp->v_flag & VROOT) == 0) ||
			    (ndp->ni_nameiop & NOCROSSMOUNT))
				break;
#ifdef	OSF1_ADFS
			if (dp->v_mount->m_flag & M_REMOTE_DIR) {
				ndp->ni_forwport = dp->v_mount->m_remoteport;
				ASSERT(ndp->ni_forwport != MACH_PORT_NULL);
				error = EREMOTE;
				goto bad;
			}
#endif
			tdp = dp;
			dp = dp->v_mount->m_vnodecovered;
#ifdef	OSF1_ADFS
			ASSERT(dp);
#endif

			VREF(dp);
			ASSERT(dp->v_flag & VMOUNTPOINT);
			/*
			 * We are crossing a mount point, going up.
			 * Release unmount lock on current mountpoint
			 * if we have it.  We don't need to acquire the
			 * new one, since the fact that we're below the
			 * mount point implies that there is a referenced
			 * vnode preventing the unmount.
			 *
			 * After acquiring the new lookup lock, we can release
			 * our reference on the root vnode of the previous
			 * filesystem, but we hold on to it until we loop
			 * again and perform an access check, if necessary.
			 */
			if (lastlocked != NULLMOUNT) {
				UNMOUNT_READ_UNLOCK(lastlocked);
				LAST_ASSERT(tdp->v_mount);
			}
			lastlocked = NULLMOUNT;
			/*
			 * This may need to change if we support forcible
			 * unmount - XXX
			 */
			vrele(tdp);
		}
	}

	/*
	 * We now have a segment name to search for, and a directory to search.
	 */
	VOP_LOOKUP(dp, ndp, error);
	if (error) {
		if (ndp->ni_vp != NULL)
			panic("leaf should be empty");
#ifdef NAMEI_DIAGNOSTIC
		printf("not found\n");
#endif
		if (flag == LOOKUP || flag == DELETE ||
		    error != ENOENT || *cp != 0)
			goto bad;
		/*
		 * If creating and at end of pathname, then can consider
		 * allowing file to be created.
		 */
		if (ndp->ni_dvp->v_mount->m_flag & rdonly) {
			error = EROFS;
			goto bad;
		}
		/*
		 * We return with ni_vp NULL to indicate that the entry
		 * doesn't currently exist, leaving a pointer to the
		 * (possibly locked) directory inode in ndp->ni_dvp.
		 */
		if (getbuf)
			ZFREE(pathname_zone, ndp->ni_pnbuf);
		if (lastlocked != NULLMOUNT) {
			UNMOUNT_READ_UNLOCK(lastlocked);
			LAST_ASSERT(dp->v_mount);
		}
		return (0);	/* should this be ENOENT? */
	}
#ifdef NAMEI_DIAGNOSTIC
	printf("found\n");
#endif

	/*
	 * Check for symbolic link
	 */
	dp = ndp->ni_vp;
	ASSERT(dp != NULLVP && dp->v_mount != NULLMOUNT);
	if ((dp->v_type == VLNK) &&
	    ((ndp->ni_nameiop & FOLLOW) || *ndp->ni_next == '/')) {
		struct iovec aiov;
		struct uio auio;
		int linklen;

#ifndef	OSF1_ADFS
		if (!getbuf)
			panic("namei: unexpected symlink");
#endif
		if (++ndp->ni_loopcnt > MAXSYMLINKS) {
			error = ELOOP;
			goto bad2;
		}
#if	SEC_AUDIT_SYMLINKS || defined(OSF1_ADFS)
		ZALLOC(pathname_zone, cp, caddr_t);
#else
		if (ndp->ni_pathlen > 1)
			ZALLOC(pathname_zone, cp, caddr_t);
		else
			cp = ndp->ni_pnbuf;
#endif
		aiov.iov_base = cp;
		aiov.iov_len = MAXPATHLEN;
		auio.uio_iov = &aiov;
		auio.uio_iovcnt = 1;
		auio.uio_offset = 0;
		auio.uio_rw = UIO_READ;
		auio.uio_segflg = UIO_SYSSPACE;
		auio.uio_resid = MAXPATHLEN;
		VOP_READLINK(dp, &auio, ndp->ni_cred, error);
		if (error) {
#if	!SEC_AUDIT_SYMLINKS && !defined(OSF1_ADFS)
			if (ndp->ni_pathlen > 1)
#endif
				ZFREE(pathname_zone, cp);
			goto bad2;
		}
		linklen = MAXPATHLEN - auio.uio_resid;
		if (linklen + ndp->ni_pathlen >= MAXPATHLEN) {
#if	!SEC_AUDIT_SYMLINKS && !defined(OSF1_ADFS)
			if (ndp->ni_pathlen > 1)
#endif
				ZFREE(pathname_zone, cp);
			error = ENAMETOOLONG;
			goto bad2;
		}
#if	SEC_AUDIT_SYMLINKS
		AUDIT_GENERATE_SYMLINK(ndp->ni_pnbuf, ndp->ni_next,
					ndp->ni_pathlen - 1, cp, linklen);
#endif
#ifdef	OSF1_ADFS
		/*
		 * If the original pathname was relative and the symlink is
		 * an absolute path, check if the user's root directory is
		 * local.  If not, set error to EREMOTE.
		 */
		if (*ndp->ni_pnbuf != '/' && *cp == '/') {
			if (!ndp->ni_rdir) {
				ndp->ni_forwport = ndp->ni_rdirport;
				ASSERT(ndp->ni_forwport != MACH_PORT_NULL);
				error = EREMOTE;
			}
		}
#endif

#if	!SEC_AUDIT_SYMLINKS && !defined(OSF1_ADFS)
		if (ndp->ni_pathlen > 1) {
#endif
			bcopy(ndp->ni_next, cp + linklen, ndp->ni_pathlen);

#ifdef	OSF1_ADFS
			if (haspathbuf) {
				haspathbuf = 0;
				ndp->ni_allocbuf++;
			} else
#endif
				ZFREE(pathname_zone, ndp->ni_pnbuf);
			ndp->ni_pnbuf = cp;

#if	!SEC_AUDIT_SYMLINKS && !defined(OSF1_ADFS)
		} else
			ndp->ni_pnbuf[linklen] = '\0';
#endif
		ndp->ni_ptr = cp;
		ndp->ni_pathlen += linklen;
#ifdef	OSF1_ADFS
		/*
		 * If we need to forward the pathname translation of the
		 * symlink, return the EREMOTE error set above.
		 */
		if (error)
			goto bad2;
#endif
		vrele(dp);
		dp = ndp->ni_dvp;
		goto start;
	}
	ASSERT(dp != NULLVP && dp->v_mount != NULLMOUNT);

	/*
	 * If it is legal to cross mountpoints, repeatedly find the 
	 * root of each filesystem mounted here.
	 *
	 * Wait for unmount attempts to finish on the
	 * new filesystem.  If a mount is in progress, we will
	 * not see the v_mountedhere field until the operation
	 * is complete, so there's no synchronization required.
	 * If we acquire a vnode reference before the mount is
	 * complete, we will access the original vnode, rather
	 * than the new filesystem root (which is legal).
	 *
	 * We must, however, wait for unmounts to complete,
	 * otherwise we could end up referencing a bad pointer.
	 * The synchronization is achieved using the UNMOUNT_LOCK
	 * interfaces.
	 * Once we have this lock, unmounts will be blocked, and we
	 * will not acquire the lock until any in-progress unmount
	 * operations complete.  If we catch one in progress, we will
	 * have to start the loop again, since the mount pointer could
	 * be bad.
	 *
	 * Once the VFS_ROOT operation successfully completes,
	 * an unmount will fail, due to the extra reference on
	 * the filesystem.
         *
         * NOTE : For ADFS we used to pass the MOUNT option to indicate
         * not to attempt to cross into fs with mounts in progress. We
         * do not need to do this anymore, since the v_mountedhere field
         * is set in mount() only after the mount completes successfully
	 */
	/*
	 * We keep the vnode locked to be sure that the v_mountedhere
	 * field doesn't change while we lock the mount point (if we're
	 * racing an unmount()).
	 */
	VN_LOCK(dp);
	if ((dp->v_type == VDIR) &&
            (ndp->ni_nameiop & NOCROSSMOUNT) == 0) {
		while (mp = dp->v_mountedhere) {
                        LASSERT(VN_LOCK_HOLDER(dp));
			ASSERT(dp->v_flag & VMOUNTPOINT);

			if (!UNMOUNT_TRY_READ(mp)) {
				/*
				 * mount point is being unmounted, wait
				 * for it, then start the loop again.
				 */
				VN_UNLOCK(dp);
				thread_block();
				VN_LOCK(dp);
				continue;
			}
			VN_UNLOCK(dp);
#ifdef	OSF1_ADFS
			/*
			 * If the filesystem is remotely mounted, leave the
			 * rest of the pathname in ndp->ni_ptr and the remote
			 * root vnode port in ndp->ni_forwport. Then return
			 * EREMOTE.
			 */
			if (mp->m_flag & M_REMOTE_FS) {
				ndp->ni_ptr = ndp->ni_next;
				/*
				 * Remove trailing slashes.
				 */
				while (*ndp->ni_ptr == '/') {
					ndp->ni_ptr++;
					ndp->ni_pathlen--;
				}
				ndp->ni_forwport = mp->m_remoteport;
				ASSERT(ndp->ni_forwport != MACH_PORT_NULL);
                                UNMOUNT_READ_UNLOCK(mp);
				error = EREMOTE;
				goto bad2;
			}
#endif	/* OSF1_ADFS */

			VFS_ROOT(mp, &tdp, error);
			if (error) {
				UNMOUNT_READ_UNLOCK(mp);
				goto bad2;
			}
			ASSERT(tdp != NULLVP && tdp->v_mount != NULLMOUNT);
			ASSERT(tdp->v_flag & VROOT);

			if (lastlocked != NULLMOUNT) {
				UNMOUNT_READ_UNLOCK(lastlocked);
				LAST_ASSERT(dp->v_mount);
			}
			lastlocked = mp;
			vrele(dp);
			ndp->ni_vp = dp = tdp;
			VN_LOCK(dp);
		}
        } /* end of if NOCROSSMOUNT */
	VN_UNLOCK(dp);

#if	SEC_MAC
	switch (mld_traverse(ndp, &error)) {
	    case SEC_MLD_DIVERT:
		/*
		 * The vnode found by the previous VOP_LOOKUP is a multilevel
		 * directory. ndp->ni_dvp now points at the mld, possibly
		 * locked, and ndp->ni_vp points at the locked subdirectory
		 * corresponding to our sensitivity level.
		 */
		dp = ndp->ni_vp;
		break;
	    case SEC_MLD_PASS:
		/*
		 * Either the vnode just found is not a multilevel directory
		 * or the MULTILEVELDIR privilege is in effect.
		 */
		break;
	    case SEC_MLD_DOTDOT:
		/*
		 * The vnode just found is a multilevel directory and the
		 * component name is "..", meaning that we are backing up
		 * the tree from a mld subdirectory.  Repeat the lookup,
		 * this time in the mld itself.
		 */
		vrele(ndp->ni_dvp);
		goto checkdotdot;
	    case SEC_MLD_ERROR:
		/*
		 * We should have diverted to a subdirectory but were
		 * unable to create it. ni_dvp has been vrele'd.
		 */
		goto bad;
	    default:
		panic("namei: unexpected return from mld_traverse");
		goto bad2;
	}
#endif	/* SEC_MAC */

nextname:
	/*
	 * Not a symbolic link.  If more pathname,
	 * continue at next component, else return.
	 */
	ndp->ni_ptr = ndp->ni_next;
	if (*ndp->ni_ptr == '/') {
		while (*ndp->ni_ptr == '/') {
			ndp->ni_ptr++;
			ndp->ni_pathlen--;
		}
		/*
		 * Trailing / is allowed for directories, so, if we've got
		 * a directory, wantparent is true, and there's no pathname
		 * left, we don't want to release the
		 * parent directory.  We know which path will be taken
		 * from dirloop based on this information.
		 */
		if ((*ndp->ni_ptr != '\0') || (!wantparent) ||
		    (dp->v_type != VDIR)) {
			vrele(ndp->ni_dvp);
		}
#ifndef	OSF1_ADFS
		hadslash = 1;
#endif
		goto dirloop;
	}
	ASSERT(dp != NULLVP && dp->v_mount != NULLMOUNT);
	/*
	 * Check for read-only file systems and attempts to 
	 * remove or rename mount points and filesystem roots.
	 * NOTE: the latter checks moved from the system calls
	 * themselves (e.g. unlink).
	 */
	if (flag == DELETE || flag == RENAME) {
		/*
		 * Disallow directory write attempts on read-only
		 * file systems.
		 */
		if ((dp->v_mount->m_flag & rdonly) ||
		    (wantparent && (ndp->ni_dvp->v_mount->m_flag & rdonly))){
			error = EROFS;
			goto bad2;
		}
		/*
		 * Don't allow destructive operations on mount points
		 * and filesystem root vnodes.
		 */
		if ((dp->v_flag & VROOT) || (dp->v_flag & VMOUNTPOINT)) {
			error = EBUSY;
			goto bad2;
		}
	}
	ASSERT(dp != NULLVP && dp->v_mount != NULLMOUNT);
	if (!wantparent)
		vrele(ndp->ni_dvp);

#if	SEC_ARCH
	/*
	 * Save the labels of the found vnode for auditing.
	 */
	audstub_nami(dp);
#endif	/* SEC_ARCH */
	if (getbuf)
		ZFREE(pathname_zone, ndp->ni_pnbuf);
	if (lastlocked != NULLMOUNT) {
		UNMOUNT_READ_UNLOCK(lastlocked);
		LAST_ASSERT(dp->v_mount);
	}
#ifdef	TNC
	/*
	 *  Follow namei() redirection via the Namei Redirection port,
	 *  V_NRPORT(vp).  (This is the vnode port of a FIFO's storage
	 *  site vnode.)
	 */
	if ((dp->v_flag & VREDIRECT) && (flag != DELETE)) {
		ASSERT(dp->v_type == VFIFO);
		/*
		 *  There is a small window where VREDIRECT may be set
		 *  but no redirection port is yet allocated---wait for it.
		 */
		while ((ndp->ni_forwport = V_NRPORT(dp)) == MACH_PORT_NULL
		       && dp->v_flag & VREDIRECT) {
			V_XFLAG(dp) |= VX_NRWAIT;
			assert_wait(&V_NRPORT(dp), FALSE);
			thread_block();
		}
		V_XFLAG(dp) &= ~VX_NRWAIT;
		if (dp->v_flag & VREDIRECT) {
			error = EREMOTE;
			UNFFDEBUG(U_NAMEI,("VREDIRECT: dp 0x%x: throw\n", dp));
			goto bad;
		}
		/* ...else the redirection was cancelled. */
	}
#endif	/* TNC */
	return (0);

bad2:
	vrele(ndp->ni_dvp);
bad:
	if (lastlocked != NULLMOUNT) {
		UNMOUNT_READ_UNLOCK(lastlocked);
		LAST_ASSERT(dp->v_mount);
	}
	vrele(dp);
#ifdef OSF1_ADFS
bad3:
#endif
	ndp->ni_vp = NULL;
	if (getbuf)
		ZFREE(pathname_zone, ndp->ni_pnbuf);
	return (error);
}
