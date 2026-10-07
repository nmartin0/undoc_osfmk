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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * Copyright (c) 1991 Center for High Performance Computing of
 *	Worcester Polytechnic Institute
 *	Rights granted to OSF/RI under contract.
 */
/*
 * HISTORY
 * $Log: kern_xxx.c,v $
 * Revision 1.11  1994/11/18  20:27:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/04/04  17:36:58  paul
 *  Reviewer:John Litvin
 *  Risk: M
 *  Benefit or PTS #:7046
 *  Testing: test program run on Plymouth several thousand times...
 *  Module(s):
 *         server/tnc/dvp_vpsops.c
 *         server/tnc/dpvproc_struct.h
 *         server/tnc/tnc_types.h
 *         server/tnc/tnc_types.defs
 *         server/tnc/rvp_pvpsops.c
 *         server/tnc/dvp_pvpsops.c
 *         server/bsd/kern_xxx.c
 *         server/tnc/rvp_pvpsops_server.c
 * Use spanning trees for setting global hostname, hostid, domain name,
 *     timezone, rpm offset
 *
 * Revision 1.9  1993/12/03  20:31:27  paul
 * Various fixes to RPM support, plus support for global setting of Timezone
 *
 * Added two routines. settimezone() is intended to be called by any system
 * call which wants to change the timezone of the system, while
 * pps_settimezone is the code which actually does the change on a specific
 * node.
 *
 *  Reviewer: John Litvin (jlitvin@ssd.intel.com) Brent Olsen (bolsen@locus.com)
 *  Risk: Moderate
 *  Benefit or PTS #: :Fixes bug #s 3503 5303 6029 7299
 *  Testing: functionality checked on olympus.sd.locus.com w/RPM support
 *  Module(s): server/bsd/kern_xxx.c
 *
 * Revision 1.8  1993/07/14  17:48:18  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/01  18:47:59  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/06  19:03:50  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.6  1993/04/03  03:04:03  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.1.4  1993/05/03  17:24:32  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.3.2.1  1992/12/16  05:58:42  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.5  1992/12/11  15:41:29  cfj
 * Remove the return(EINVAL) in reboot().
 *
 * Revision 1.4  1992/12/11  02:54:37  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:16:08  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.3  1992/11/13  03:42:47  cfj
 * reboot() always return EINVAL until we support it.
 *
 * Revision 1.1.2.2  1992/11/06  18:21:45  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/06  00:06:27  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.7  92/12/01  11:10:53  chrisp
 * In reboot(), replace the call to boot() by virtual process system op
 * 	VPSOP_REBOOT(). For TNC, this redirects the call to the root fs node.
 * 
 * Revision 2.6  92/10/28  14:55:08  roman
 * Change sethostname, setdomainname, and sethostid system calls to
 * 	restructure them into virtual process sytem and physical
 * 	process system operations. This allows the TNC code to
 * 	perform the correct operations.
 * 
 * Revision 2.7  1992/12/01  11:10:53  chrisp
 * In reboot(), replace the call to boot() by virtual process system op
 * 	VPSOP_REBOOT(). For TNC, this redirects the call to the root fs node.
 *
 * Revision 2.6  92/10/28  14:55:08  roman
 * Change sethostname, setdomainname, and sethostid system calls to
 * 	restructure them into virtual process sytem and physical
 * 	process system operations. This allows the TNC code to
 * 	perform the correct operations.
 * 
 * Revision 2.5  92/01/02  18:51:42  roy
 * 	1991/10/14  20:04:20  noemi
 * 	Added HASPATHBUF option in call to namei and set ni_segflg to 
 * 	UIO_SYSSPACE.
 * 
 * Revision 2.3  91/09/16  15:36:42  rabii
 * 	Merge of V2.0 and Locus (locus check-in by hao)
 * 	System calls with file descriptors as the first uarg were changed to 
 * 	have a file pointer as the first argument, and no file descriptor
 * 	in the uarg. Use new interface to getf().
 * 
 * Revision 2.2  91/08/31  13:22:21  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/28  10:53:59  barbou
 * Upgrade to OSF/1.0.2.
 * 
 * Revision 1.12  90/10/07  13:18:13  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:57:38  gm]
 * 
 * Revision 1.11  90/08/24  11:17:32  devrcs
 * 	Changes for new system call interface.
 * 	Changes for u_file_state and getf
 * 	[90/08/17  17:38:19  nags]
 * 
 * 	Intercept and return an error on old quota call.
 * 	[90/08/18  23:40:31  nags]
 * 
 * 	fix an MP race that could cause improper auditing
 * 	of changes to hostname or domainname
 * 	[90/08/14  14:34:26  hosking]
 * 
 * 	Removed 4.1bsd compatibility code.
 * 	[90/08/12  12:21:57  ers]
 * 
 * 	Removed (o)ssig (moved it to kern_sig.c).
 * 	[90/08/07  14:46:47  coren]
 * 
 * Revision 1.10  90/07/17  11:19:25  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:52:18  seiden]
 * 
 * 	Add support for per thread synchronous signals
 * 	[90/06/29  09:09:57  sp]
 * 
 * Revision 1.8  90/06/22  20:06:30  devrcs
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:54:00  seiden]
 * 
 * 	nags merge
 * 	[90/06/12  19:06:55  gmf]
 * 
 * 	Added ostat, olstat, ofstat for 4.3bsd binary compatibility.
 * 	[90/06/10  17:23:38  ers]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:40:22  seiden]
 * 
 * 	Compressed history (reverse chronology):
 * 	Parallelized for OSF/1.			nags@encore.com
 * 	Maintain lock across copyout in uname	bet@osf.org
 * 	Added uname() system call.		bet@osf.org
 * 	Removed obsolete "o-" functions.	coren@osf.org
 * 	Removed include of <sys/dir.h>.		gmf@osf.org
 * 	Fixes for first snapshot.		gm@osf.org
 * 	Fixed suser() call in setdomainname.	gmf@osf.org
 * 	Mach 2.5 and Encore 0.6 merge.		gm@osf.org
 * 	Integrated early Mach 2.5, Encore MP.	alan@encore.com
 * 	MMAX_MP:  lock around system time.	boykin@encore.com
 * 	MMAX_MP:  lock access to the hostname.	alan@encore.com
 * 	Mips actually never needed owait().	af@cmu.edu
 * 	Use new inode macros.			gm0w@cmu.edu
 * 	More cleanup.				rpd@cmu.edu
 * 	Changes for cleanup.			gm0w@cmu.edu
 * 	Code cleanup cataclysm.			mwyoung@cmu.edu
 * 	Use new_printf_cpu_number.		dlb@cmu.edu
 * 	Changes for I386: cut in 386 registers.	rvb@cmu.edu
 * 	Added Mips changes, e.g. to owait().	af@cmu.edu
 * 	Turn off cpu number printout in reboot.	rvb@cmu.edu
 * 	MACH:  declare vm_object_shutdown.	mwyoung@cmu.edu
 * 	Made QUOTA a #if-type option.		bolosky@cmu.edu
 * 
 * 	Older contributors:  mja@cmu.edu, beck@sequent.com, avie@cmu.edu.
 * 	[90/06/10  01:55:53  seiden]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)kern_xxx.c	7.1 (Berkeley) 6/5/86
 */

#include <cputypes.h>
#include <quota.h>
#include <sys/secdefines.h>

#include <sys/unix_defs.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/reboot.h>
#include <sys/utsname.h>
#include <sys/file.h>
#include <sys/stat.h>
#if	SEC_BASE
#include <sys/security.h>
#endif
#include <uxkern/bsd_types.h>

static convert_stat();

gethostid(p, args, retval)
	struct proc *p;
	void *args;
	long *retval;
{

	HOSTNAME_READ_LOCK();
	*retval = hostid;
	HOSTNAME_READ_UNLOCK();
	return (0);
}

sethostid(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	struct args {
		long	hostid;
	} *uap = (struct args *) args;
	int error;

#if	SEC_BASE
	if (!privileged(SEC_SYSATTR, EPERM))
		return (EPERM);
#else 
	if ((error = suser(u.u_cred, &u.u_acflag)))
		return (error);
#endif
	return (VPSOP_SETHOSTID(uap->hostid));
}

pps_sethostid(new_hostid)
	long new_hostid;
{
	HOSTNAME_WRITE_LOCK();
	hostid = new_hostid;
	HOSTNAME_WRITE_UNLOCK();
	return(ESUCCESS);
}

gethostname(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	struct args {
		char	*hostname;
		u_int	len;
	} *uap = (struct args *) args;
	int error;

	HOSTNAME_READ_LOCK();
	if (uap->len > hostnamelen + 1)
		uap->len = hostnamelen + 1;
	error = copyout((caddr_t)hostname, (caddr_t)uap->hostname, uap->len);
	HOSTNAME_READ_UNLOCK();
	return (error);
}

sethostname(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	struct args {
		char	*hostname;
		u_int	len;
	} *uap = (struct args *) args;
	int		error;
	hostname_t	new_hostname;
#if	SEC_BASE
	if (!privileged(SEC_SYSATTR, EPERM))
		return (EPERM);
#else   
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
	if (uap->len > sizeof (new_hostname))
		return (EINVAL);

	error = copyin((caddr_t)uap->hostname, new_hostname, uap->len);
	if (!error)
		error = VPSOP_SETHOSTNAME(new_hostname, uap->len);

	return(error);
}

pps_sethostname(new_hostname, new_hostnamelen)
	hostname_t	new_hostname;
	unsigned int	new_hostnamelen;
{
	if (new_hostnamelen > sizeof (hostname) - 1)
		return (EINVAL);

	HOSTNAME_WRITE_LOCK();
	bcopy(new_hostname, hostname, new_hostnamelen);
	hostnamelen = new_hostnamelen;
	hostname[hostnamelen] = 0;
	bcopy(new_hostname, utsname.nodename, sizeof(utsname.nodename));
	utsname.nodename[sizeof(utsname.nodename)-1] = 0; 
#if	SEC_BASE
	/*
	 * MP note: audstub_pathname() is pendable.  Hopefully we won't be
	 * calling this often enough for it to matter that we're holding a
	 * lock.  (We don't want to risk races with other hostname changes,
	 * and it's probably not worth the effort to do copying to a 
	 * temp buffer.)
	 */
	audstub_pathname(hostname, hostnamelen + 1);
#endif 
	HOSTNAME_WRITE_UNLOCK();
	return (ESUCCESS);
}

/*
 * Re-use the hostname lock for the domainname, too.  As the host and
 * domain names are typically read and almost never set, this lock
 * shouldn't be a bottleneck unless use becomes so frequent that the
 * spinlock itself becomes a contention point.
 */
getdomainname(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	struct args {
		char	*domainname;
		int	len;
	} *uap = (struct args *) args;
	int error;

	HOSTNAME_READ_LOCK();
	if (uap->len > domainnamelen + 1)
		uap->len = domainnamelen + 1;
	error = copyout((caddr_t)domainname,(caddr_t)uap->domainname,uap->len);
	HOSTNAME_READ_UNLOCK();
	return (error);
}

setdomainname(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	struct args {
		char	*domainname;
		int	len;
	} *uap = (struct args *) args;
	int		error;
	hostname_t	new_domainname;

#if	SEC_BASE
	if (!privileged(SEC_SYSATTR, EPERM))
		return (EPERM);
#else  
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
	if (uap->len > sizeof (new_domainname) - 1)
		return (EINVAL);
	error = copyin((caddr_t) uap->domainname, new_domainname, uap->len);
	if (!error)
		error =  VPSOP_SETDOMAINNAME(new_domainname, uap->len);

	return (error);
}

pps_setdomainname(new_domainname, new_domainnamelen)
	domainname_t	new_domainname;
	unsigned int	new_domainnamelen;
{
	if (new_domainnamelen > sizeof (domainname) - 1)
		return (EINVAL);

	HOSTNAME_WRITE_LOCK();
	bcopy(new_domainname, domainname, new_domainnamelen);
	domainnamelen = new_domainnamelen;
	domainname[domainnamelen] = 0;
#if	SEC_BASE
	/*
	 * MP note: audstub_pathname() is pendable.  Hopefully we won't be
	 * calling this often enough for it to matter that we're holding a
	 * lock.  (We don't want to risk races with other hostname changes,
	 * and it's probably not worth the effort to do copying to a 
	 * temp buffer.)
	 */
	audstub_pathname(domainname, domainnamelen + 1);
#endif 
	HOSTNAME_WRITE_UNLOCK();
	return (ESUCCESS);
}

/*
 * Set default timezone for the system. This is not a system call itself,
 * but is called by the system calls to get the new timezone distributed
 * to all nodes in the system.
 */
settimezone(zone_p)
	struct timezone *zone_p;
{
	return (VPSOP_SETTIMEZONE(*zone_p));
}

pps_settimezone(new_timezone)
	struct timezone new_timezone;
{
	register int s;

	s = splhigh();
	TIME_WRITE_LOCK();
	tz = new_timezone;
	TIME_WRITE_UNLOCK();
	splx(s);
	return(ESUCCESS);
}

/* 
 *  get name of current operating system 
 */
uname(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	struct args {
		struct utsname *name;
	} *uap = (struct args *) args;
	int error;
	/*
	 * The utsname.nodename field is now set by sethostname.
	 */
	HOSTNAME_READ_LOCK();
	bcopy(hostname, utsname.nodename, sizeof(utsname.nodename));
	utsname.nodename[sizeof(utsname.nodename)-1] = 0;
	error = copyout(&utsname, uap->name, sizeof(struct utsname));
	HOSTNAME_READ_UNLOCK();
	return (error);
}

reboot(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	struct args {
		int	opt;
	} *uap = (struct args *) args;
	int error;

#if	SEC_BASE
	if (!privileged(SEC_SHUTDOWN, EPERM))
		return (EPERM);
#else
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
	else {
#ifndef	OSF1_SERVER
		extern void vm_object_shutdown();
#endif	OSF1_SERVER
#ifndef	multimax
#if	NCPUS > 1
#ifndef	OSF1_SERVER
		extern int new_printf_cpu_number;

		printf("Reboot()ing.\n");
		new_printf_cpu_number = 0;
#endif	!OSF1_SERVER
#endif	NCPUS > 1
#endif

#if	SEC_BASE
		auddisable();
#endif
#ifndef	OSF1_SERVER
		vm_object_shutdown();
#endif	OSF1_SERVER
		(void) VPSOP_REBOOT(RB_BOOT, uap->opt);
	}
}

#ifdef COMPAT_43

struct  ostat
{
	short   ost_dev;                /* ID of device containing a directory*/
					/*   entry for this file.  File serial*/
					/*   no + device ID uniquely identify */
					/*   the file within the system */
	ino_t   ost_ino;                /* File serial number */
	u_short ost_mode;               /* File mode; see #define's below */
	short   ost_nlink;              /* Number of links */
	u_short ost_uid;                /* User ID of the file's owner */
	u_short ost_gid;                /* Group ID of the file's group */
	short   ost_rdev;               /* ID of device */
					/*   This entry is defined only for */
					/*   character or block special files */
	off_t   ost_size;               /* File size in bytes */
	time_t  ost_atime;              /* Time of last access */
	int     ost_spare1;
	time_t  ost_mtime;              /* Time of last data modification */
	int     ost_spare2;
	time_t  ost_ctime;              /* Time of last file status change */
	int     ost_spare3;
					/* Time measured in seconds since */
					/*   00:00:00 GMT, Jan. 1, 1970 */
	ulong_t ost_blksize;            /* Size of block in file */
	long    ost_blocks;             /* blocks allocated for file */
	u_long  ost_flags;              /* user defined flags for file */
	u_long  ost_gen;                /* file generation number */

};

ostat(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	return (ostat1(p, args, retval, FOLLOW));
}

olstat(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	return (ostat1(p, args, retval, NOFOLLOW));
}

ofstat(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	struct args {
		struct  ostat *ub;
	} *uap = (struct args *) args;
	struct stat sb;
	struct ostat osb;
	int error;

	if (error = getf(fp))
		return (error);
	switch (fp->f_type) {
	case DTYPE_VNODE:
		FILE_FUNNEL(fp->f_funnel);
		error = vn_stat((struct vnode *)fp->f_data, &sb);
		FILE_UNFUNNEL(fp->f_funnel);
		break;

	case DTYPE_SOCKET:
		error = soo_stat((struct socket *)fp->f_data, &sb);
		break;
	
	default:
		panic("ofstat");
		/*NOTREACHED*/
	}
	if (error == 0) {
		convert_stat(&osb, &sb);
		error = copyout((caddr_t)&osb, (caddr_t)uap->ub,
					sizeof (osb));
	}
	FP_UNREF(fp);
	return (error);
}

ostat1(p, args, retval, follow)
	struct proc *p;
	void *args;	
	int *retval;
	int follow;
{
	struct args {
		char    *fname;
		struct ostat *ub;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	struct stat sb;
	struct ostat osb;
	int error;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | follow | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | follow;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	if ((error = namei(ndp)) == 0) {
		error = vn_stat(ndp->ni_vp, &sb);
		vrele(ndp->ni_vp);
		if (error == 0) {
			convert_stat(&osb, &sb);
			error = copyout((caddr_t)&osb, (caddr_t)uap->ub, 
					 sizeof (osb));
		}
	}
	return (error);
}

static
convert_stat(osb, sb)
register struct ostat *osb;
register struct stat *sb;
{
	osb->ost_dev = sb->st_dev;
	osb->ost_ino = sb->st_ino;
	osb->ost_mode = sb->st_mode;
	osb->ost_nlink = sb->st_nlink;
	osb->ost_uid = sb->st_uid;
	osb->ost_gid = sb->st_gid;
	osb->ost_rdev = sb->st_rdev;
	osb->ost_size = sb->st_size;
	osb->ost_atime = sb->st_atime;
	osb->ost_spare1 = sb->st_spare1;
	osb->ost_mtime = sb->st_mtime;
	osb->ost_spare2 = sb->st_spare2;
	osb->ost_ctime = sb->st_ctime;
	osb->ost_spare3 = sb->st_spare3;
	osb->ost_blksize = sb->st_blksize;
	osb->ost_blocks = sb->st_blocks;
	osb->ost_flags = sb->st_flags;
	osb->ost_gen = sb->st_gen;
}
#endif /* COMPAT_43 */


/*
 * XXX should these be COMPAT_43 ???
 */
ovhangup(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{

	return (EINVAL);
}

oldquota(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{

	return (EINVAL);
}
