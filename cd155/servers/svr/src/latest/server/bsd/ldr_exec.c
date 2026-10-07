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
 * HISTORY
 * $Log: ldr_exec.c,v $
 * Revision 1.6  1994/11/18  20:27:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/13  17:52:59  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.4  1993/07/14  17:48:27  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:48:06  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:04:00  nandy
 * ad103+tnc merged with Intel code.
 *
 * Initial 1.0.3 code drop
 *
 * Revision 2.16  1992/09/24  16:49:53  rabii
 * 	emove 2.15 in lieu of the Grenoble fix. (dwm; #376)
 *
 * Revision 2.15  92/09/17  13:42:05  rabii
 * 	[1992/09/11  17:37:34  cfj]
 * 	Now exec_args_set() checks if the process is traced and sends a
 * 	SIGTRAP to if so.
 * 
 * Revision 2.14  92/05/31  18:58:14  loverso
 * 	Call remote_vfree instead of remote_vrele to deallocate vnode proxy
 * 	(pjg).
 * 
 * Revision 2.13  92/05/24  14:16:41  pjg
 * 	Revision 3.8  92/03/23  18:02:17  condict
 * 	Fix previous bug fix: sizeof(exdata) is big enough in the vn_rdwr calls.
 * 
 * 	Revision 3.7  92/03/13  15:18:26  condict
 * 	Read the maximum of exec header size and MAXINTERP, in order to ensure
 * 	that we have enough bytes for either shell script or binary program.
 * 
 * 	Revision 3.6  92/01/30  16:04:56  sp
 * 	Clean up auxiliary vector code
 * 	[92/05/19            srl]
 * 
 * Revision 2.12  92/04/07  13:41:32  pjg
 * 	Add ndp argument to calls to remote_exec_lookup.
 * 
 * Revision 2.11  92/03/09  14:21:23  durriya
 * 	Revision 3.6  92/01/30  16:04:56  sp
 * 	Clean up auxiliary vector code
 * 
 * Revision 2.10  92/03/03  13:56:32  pjg
 * 	Add support for remote FS operations in exec_load_loader.
 * 
 * Revision 2.9  92/01/14  11:16:48  roy
 * 	Changes to do_exec_setup_suid (pjg).
 * 
 * Revision 2.8  92/01/02  18:52:35  roy
 * 	91/12/16  19:44:34  pjg
 * 	Limited test of OSF1_ADFS support for remote exec(). 
 * 
 * Revision 2.7  91/12/17  16:32:18  roy
 * 	91/11/26  15:32:45  sp
 * 	Upgrade to 1.0.3
 * 
 * Revision 2.6  91/11/13  12:51:33  rabii
 * 	i860 support
 * 
 * Revision 2.5  91/10/14  12:08:04  sjs
 * 	Revision 3.4  91/10/04  16:37:11  sp
 * 	Add support for auxiliary vector
 * 	Fix exec_load_loader
 * 
 * Revision 2.4  91/10/04  14:48:00  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:37:22  rabii
 * 	Merge of V2.0 and Locus (locus check-in by hao)
 * 	File descriptors now handled in emulator. Remove handling of
 * 	close-on-exec files from here.
 * 
 * Revision 2.2  91/08/31  13:22:26  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/07/26  15:50:40  jose
 * Adapted from 1.0.2 to OSF/1s environment
 * 
 * Revision 1.12.10.2  91/09/20  16:04:05  melanie
 *	Fixed bug 2055, removing o_mach_o setregs code from sysv_coff
 *	ifdef, allowing exec to be built for only the ROSE format.
 *	[91/09/20  16:00:20  melanie]
 *
 * Revision 1.12.7.2  91/06/19  08:43:09  lwa
 * 	In secure systems, get privileges from the ORIGINAL file passed
 * 	in to exec() in exec_get_effective_ids(), and establish the
 * 	privileges in exec_setuid().  Move the exec_setuid() calls OUT
 * 	of the getxfile() routines into common code.
 * 	[91/06/17  08:16:51  lwa]
 * 
 * Revision 1.12  90/10/07  13:18:18  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:57:48  gm]
 * 
 * Revision 1.11  90/09/23  15:43:23  devrcs
 * 	Increased size of NCARGS to accommodate the one or possibly two
 * 	strings, used by the auxiliary vector, added to the "exec arglist"
 * 	area.  Both of them are pathnames, one for the file argument to
 * 	exec() and the other for the pathname of the loader, if a loader
 * 	is used to load the program.  In exec_args_collect(), limited the
 * 	maximum size of the argument strings and the environment variable
 * 	strings to ARG_MAX bytes.
 * 	[90/09/12  07:46:34  kwallace]
 * 
 * Revision 1.10  90/09/13  11:42:15  devrcs
 * 	Fixed missed changes to calling sequence for
 * 	getxfile routines
 * 	[90/08/30  09:19:07  lwa]
 * 
 * Revision 1.9  90/08/24  11:17:41  devrcs
 * 	Changes for new system call interface.
 * 	Changes for u_file_state
 * 	[90/08/17  17:38:26  gmf]
 * 
 * 	Use consistent credentials macros.
 * 	[90/08/18  23:41:11  nags]
 * 
 * 	Removed dead i386 code.
 * 	[90/08/17  19:47:25  mcg]
 * 
 * 	HP/Apollo M68K
 * 	[90/08/13  17:35:47  mcg]
 * 
 * 	Fixed bug number 0210.  The pmap_pte_fault panic was caused
 * 	by a spurious reference made by copyinstr().  The fundamental
 * 	problem was that although any error returned by exec_copyinstr()
 * 	was collected, in the loops to copy in the arguments and the
 * 	environment variables, the error indication was ignored.
 * 	[90/08/10  19:37:51  kwallace]
 * 
 * 	Fixed bug number 0210.  The pmap_pte_fault panic was caused
 * 	by a spurious reference made by copyinstr().  The fundamental
 * 	problem was that although any error returned by exec_copyinstr()
 * 	was collected, in the loops to copy in the arguments and the
 * 	environment variables, the error indication was ignored.
 * 	[90/08/10  19:37:51  kwallace]
 * 
 * Revision 1.8  90/08/09  13:14:22  devrcs
 * 	Apply credentials ref count fix to exec_load_loader().
 * 	[90/08/07  12:41:26  ers]
 * 
 * Revision 1.7  90/07/27  08:44:17  devrcs
 * 	Dead mount structures.
 * 	[90/07/20  13:50:47  nags]
 * 
 * 	Changed Mach-O recognizer to handle files with canonical headers.
 * 	[90/07/17  16:26:07  melanie]
 * 
 * Revision 1.6  90/07/17  11:19:30  devrcs
 * 	Changed mod register start for Multimax to be 0x2000.
 * 	[90/07/09  11:39:37  melanie]
 * 
 * Revision 1.5  90/07/05  23:07:53  devrcs
 * 	Save pointer to arguments and environment in u area.
 * 	[90/06/29  15:33:06  brezak]
 * 
 * Revision 1.4  90/06/22  20:06:34  devrcs
 * 	Added ability to load Mach-O object format.
 * 	[90/06/18  10:47:29  melanie]
 * 
 * 	nags merge
 * 
 * 	     Parallelized for OSF/1                          nags@encore.com
 * 	Changes for LDR_EXEC_{DEFAULT_LOADER,SETUID_F}	kwallace@osf.org
 * 	exec_get_effective_uids returns privilege flag 	lwa@osf.org
 * 	Removed quad-word alignment for mips stack 	brezak@osf.org
 * 	New argument/environment variable logic		kwallace@osf.org
 * 	Rewrote exec_args_collect()			kwallace@osf.org
 * 	Modified exec_copy* counts			kwallace@osf.org
 * 	Fix for ps argument string hack			kwallace@osf.org
 * 	Modified to use exec_copy*			kwallace@osf.org
 * 	Created						kwallace@osf.org
 * 	[90/06/12  19:07:01  gmf]
 * 
 * $EndLog$
 */
/*
 * This file implements exec_with_loader().  exec_with_loader()
 * implements, for the most part, the exact same functionality as
 * execve().  All the execve() checks of the file, including #!
 * interpretation, are performed by exec_with_loader(), however,
 * exec_with_loader() loads a user space loader, instead of the file,
 * and passes the name of the file to the loader, with the hope that the
 * loader will load and execute the file.
 */

#include <sys/secdefines.h>
#if	SEC_BASE
#include <sys/security.h>
#endif
#if	SEC_ARCH
#include <sys/secpolicy.h>
#endif
#include <sys/exec_incl.h>
#include <sys/ldr_exec.h>
#include <sys/auxv.h>

#ifdef	OSF1_SERVER
#include <bsd/exec.h>
#endif

#ifndef	OSF1_SERVER
void exec_args_copyback();
#else
void exec_auxv_copyback();
#endif

#if	OSF_MACH_O
extern int decode_mach_o_hdr(void *, size_t, unsigned long, mo_header_t *);
#endif

struct exec_with_loader_args {
	int	flags;
	char	*loader;
	char	*file;
	char	**argv;
	char	**envp;
};

/*
 * exec_with_loader()
 *
 * exec_with_loader() implements, for the most part, the exact same
 * functionality as execve().  All the execve() checks of the file,
 * including #!  interpretation, are performed by exec_with_loader(),
 * however, exec_with_loader() loads a user space loader, instead of the
 * file, and passes the name of the file to the loader, with the hope
 * that the loader will load and execute the file.
 */
exec_with_loader(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct exec_with_loader_args *uap;
	int flags;

	uap = (struct exec_with_loader_args *) args;
	flags = uap->flags & LDR_EXEC_USER_MASK;
	return (exec_load_loader(p, flags, uap->loader, uap->file,
#ifndef	OSF1_SERVER
		uap->argv, uap->envp));
#else
		uap->argv, uap->envp, retval));
#endif
}

/*
 * exec_load_loader()
 *
 * exec_load_loader() is the common code, that really implements
 * exec_with_loader(), that can be called by both exec_with_loader() and
 * execve(). 
 */
#ifndef	OSF1_SERVER
exec_load_loader(p, flags, loader, ufile, uargv, uenvp)
#else
exec_load_loader(p, flags, loader, ufile, uargv, uenvp, retval)
#endif
	struct proc *p;
	int flags;
	char *loader, *ufile, **uargv, **uenvp;
#ifdef	OSF1_SERVER
	int	*retval;
#endif
{
	enum uio_seg loader_segflg;
	int na, ne, nc;
	struct exec_privs privs;
	char *shell_name, *shell_arg;
	char shell_or_file_name_tail[MAXCOMLEN + 1];
#ifdef	OSF1_ADFS
	struct vnode_proxy *vp;
#else
	struct vnode *vp;
	struct vattr vattr;
	vm_offset_t exec_args = 0;
#endif
	register struct nameidata *ndp = &u.u_nd;
	char line[MAXINTERP];
	int pflag;
	union {
#if	SYSV_COFF
		/* 
		 * Coff fileheader structures.
		 */
		struct {
			struct filehdr fhdr;
			struct aouthdr ohdr;
#define ahdr exdata.coff.ohdr
		} coff;
		struct {
			short	magic;
		} coff_hdr;
#endif
#if	OSF_MACH_O
		long	omo_magic;
#endif
#if	BSD_A_OUT
		struct	exec ex_exec;
#endif
	} exdata;
	int resid, error;
#if	SYSV_COFF
	int 	docoff = 0;
	int	aouthdr_offset;
#endif
#if	OSF_MACH_O
	int 		do_o_mach_o = 0;
	long		entry_addr = 0;
	int		conversion_error;
        char		mo_header_buf[MO_SIZEOF_RAW_HDR];
	                              /* buffer for raw canonical version */
        mo_header_t	mo_header;    /* translated version of the header */
#endif
#if	BSD_A_OUT
        int do_bsd_a_out = 0;
#endif
#ifdef	OSF1_SERVER
	register struct execr *rtp;

	rtp = (struct execr *) retval;
#endif

	privs.creds = NULL;
	privs.is_priv = FALSE;

	/*
	 * Get vnode for file.
	 */
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_dirp = ufile;
#ifdef OSF1_ADFS
	ndp->ni_segflg = UIO_SYSSPACE;
	error = remote_exec_lookup(ndp, &vp);
	if (error)
		return(error);
	ASSERT(vp != 0);
	{
	uid_t new_uid;
	gid_t new_gid;
	boolean_t new_is_priv;

	error = remote_exec_setup_suid(vp, p->p_flag, TRUE,
				       &new_uid, &new_gid, &new_is_priv);
	if (error)
		goto bad;
	/*
	 * XXX Does not support security option SEC_BASE
	 */
	privs.creds = crdup(u.u_cred);
	privs.creds->cr_uid = new_uid;
	privs.creds->cr_gid = new_gid;
	privs.is_priv = new_is_priv;
	}	
#else	/* OSF1_ADFS */
	ndp->ni_segflg = UIO_USERSPACE;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	VOP_GETATTR(vp, &vattr, u.u_cred, error);
	if (error)
		goto bad;

	/*
	 * Get effective uid and gid, then check for proper access to
	 * the file.
	 */
	if (error = exec_get_effective_ids(vp, &vattr, &privs))
		goto bad;
	if (error = exec_check_access(p, vp, &vattr))
		goto bad;
#endif	/* OSF1_ADFS */
	/*
	 * Read first MAXINTERP bytes of file.
	 */
	line[0] = '\0';		/* for zero length files */
#ifdef OSF1_ADFS
	error = remote_exec_read(vp, (caddr_t)line, MAXINTERP, (off_t)0,
				 IO_UNIT, &resid);
#else
	error = vn_rdwr(UIO_READ, vp, (caddr_t)line, MAXINTERP,
		(off_t)0, UIO_SYSSPACE, IO_UNIT, u.u_cred, &resid);
#endif
	if (error)
		goto bad;

	/*
	 * First check to see if we have a shell script.  If #!
	 * interpretation is being requested, upon return from
	 * exec_get_shell(), shell_name contains the pathname of
	 * the shell to execute and shell_arg contains any additional
	 * argument to the shell.  Note that shell_arg may be NULL.
	 * Throughout the remainder of this function, shell_name is
	 * used to indicate where #! interpretation has been requested.
	 *
	 * Next, if the file is less than MAXINTERP bytes long, it
	 * must be a shell script before we will continue trying to
	 * load it  (i.e. before we will continue trying to load a
	 * loader that will load the shell specified in the script).
	 * Therefore if the file is less than MAXINTERP bytes long and
	 * it isn't a shell script, we return ENOEXEC.
	 */
	if (error = exec_get_shell(line, &shell_name, &shell_arg))
		goto bad;
	if (resid && (!shell_name)) {
		error = ENOEXEC;
		goto bad;
	}

	/*
	 * If #! found, finished with vnode for file.  Now get the
	 * vnode for the shell and make sure we have execute access to
	 * it.  We do not allow set*id shell scripts.
	 *
	 * Next, save the directory entry name of shell or file as
	 * returned by namei().  Now we are finished with either the
	 * shell vnode or the file vnode.
	 */
	if (shell_name) {
#ifdef	OSF1_ADFS
		remote_vfree(vp);
#else
		vrele(vp);
#endif
		ndp->ni_nameiop = LOOKUP | FOLLOW;
		ndp->ni_segflg = UIO_SYSSPACE;
		ndp->ni_dirp = shell_name;
#ifdef	OSF1_ADFS
		error = remote_exec_lookup(ndp, &vp);
		if (error)
			return (error);
		ASSERT(vp != 0);
		{
			int arg0, arg1, arg2;	/* temp var for mig */
			error = remote_exec_setup_suid(vp, p->p_flag, FALSE,
						&arg0, &arg1, &arg2);
		}
		if (error)
			goto bad;
#else	/* OSF1_ADFS */
		if (error = namei(ndp))
			return (error);
		vp = ndp->ni_vp;
		VOP_GETATTR(vp, &vattr, u.u_cred, error);
		if (error)
			goto bad;
		if (exec_check_access(p, vp, &vattr) == -1)
			goto bad;
#endif	/* OSF1_ADFS */
		reset_effective_ids(&privs);	/* shell scripts can't be setid */
	}
	bcopy((caddr_t)ndp->ni_dent.d_name, (caddr_t)shell_or_file_name_tail,
		MAXCOMLEN);
	shell_or_file_name_tail[MAXCOMLEN] = '\0';
#ifdef	OSF1_ADFS
	remote_vfree(vp);
#else
	vrele(vp);
#endif
	/*
	 * Get loader name.  Setuid and/or setgid files must use the
	 * LDR_EXEC_DEFAULT_LOADER.  Set appropriate flags.
	 */
	if (loader && !privs.is_priv)
		loader_segflg = UIO_USERSPACE;
	else {
		if (privs.creds->cr_uid != u.u_uid)
			flags |= LDR_EXEC_SETUID_F;
		if (privs.creds->cr_gid != u.u_gid)
			flags |= LDR_EXEC_SETGID_F;
		loader = LDR_EXEC_DEFAULT_LOADER;
		loader_segflg = UIO_SYSSPACE;
	}

	/*
	 * Get vnode for loader.
	 */
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = loader_segflg;
	ndp->ni_dirp = loader;
#ifdef	OSF1_ADFS
	error = remote_exec_lookup(ndp, &vp);
	if (error)
		return (error);
	ASSERT(vp != 0);
	{
		int arg0, arg1, arg2;	/* temp var for mig */
		error = remote_exec_setup_suid(vp, p->p_flag, FALSE,
					       &arg0, &arg1, &arg2);
	}
	if (error)
		goto bad;
#else
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	VOP_GETATTR(vp, &vattr, u.u_cred, error);
	if (error)
		goto bad;
	/*
	 * Check for execute access to loader.
	 */
	if (error = exec_check_access(p, vp, &vattr))
		goto bad;
#endif
	/*
	 *	Read in the header to get magic number.
	 *	This magic number is architecture-dependent.
	 */
#ifdef OSF1_ADFS
	error = remote_exec_read(vp, (caddr_t)&exdata, sizeof(exdata),
				 (off_t)0, IO_UNIT, &resid);
#else
	error = vn_rdwr(UIO_READ, vp, (caddr_t)&exdata, sizeof(exdata),
	    (off_t)0, UIO_SYSSPACE, IO_UNIT, u.u_cred, &resid);
#endif
	if (error)
		goto bad;
	if (resid) {
		error = ENOEXEC;
		goto bad;
	}

#if	BSD_A_OUT
	/*
	 * Read in first few bytes of file for segment sizes, magic number:
	 *	OMAGIC = plain executable
	 *	NMAGIC = RO text
	 *	ZMAGIC = demand paged RO text
	 * Also an ASCII line beginning with #! is
	 * the file name of a ``shell'' and arguments may be prepended
	 * to the argument list if given here.
	 *
	 * SHELL NAMES ARE LIMITED IN LENGTH.
	 *
	 * ONLY ONE ARGUMENT MAY BE PASSED TO THE SHELL FROM
	 * THE ASCII LINE.
	 */
	if (exdata.ex_exec.a_magic == ZMAGIC ||
	    exdata.ex_exec.a_magic == NMAGIC) {
		if (exdata.ex_exec.a_text == 0) {
			error = ENOEXEC;
			goto bad;
		}
		do_bsd_a_out = 1;
		goto gotobject;
	} else
	if (exdata.ex_exec.a_magic == OMAGIC) {
		exdata.ex_exec.a_data += exdata.ex_exec.a_text;
		exdata.ex_exec.a_text = 0;
		do_bsd_a_out = 1;
		goto gotobject;
	} else
#ifdef	balance
	if (exdata.ex_exec.a_magic == 0x10ea) {	/* ZMAGIC: 0@0 */
						/* no XMAGIC yet */
		exdata.ex_exec.a_magic = ZMAGIC;	/* make other code easier */
		if (exdata.ex_exec.a_text == 0) {
			error = ENOEXEC;
			goto bad;
		}
		do_bsd_a_out = 1;
		goto gotobject;
	} else
#endif
#endif	/* BSD_A_OUT */
#if	OSF_MACH_O
	if (exdata.omo_magic == OUR_MOH_MAGIC) {
		/*
		 * Now read in the complete file header, 
		 * starting from the beginning.
		 */
#ifdef	OSF1_ADFS
		error = remote_exec_read(vp, (caddr_t)&mo_header_buf,
					 sizeof(mo_header_buf), (off_t)0,
					 (IO_UNIT | IO_NODELOCKED), &resid);
#else
		error = vn_rdwr (UIO_READ, vp, (caddr_t)&mo_header_buf,
			    sizeof(mo_header_buf), (off_t)0, UIO_SYSSPACE,
			    (IO_UNIT | IO_NODELOCKED), u.u_cred, &resid);
#endif
		if (error)
			goto bad;
	
		/* Convert the canonical version of the header so we can
		 * read it. 
		 */

		conversion_error = decode_mach_o_hdr ((void *)&mo_header_buf,
				     (size_t)sizeof(mo_header_buf),
				     (unsigned long)MOH_HEADER_VERSION,
				     &mo_header);
		if (conversion_error != MO_HDR_CONV_SUCCESS) {
			error = ENOEXEC;
			goto bad;
		}


		/* Now we have valid header information; see if we 
		 * can load it here.
		 */
		if ((!(mo_header.moh_flags & MOH_EXECABLE_F))
		    || (mo_header.moh_flags & MOH_RELOCATABLE_F) /* needs relocation */
		    || (mo_header.moh_flags & MOH_UNRESOLVED_F)) {
			error = ENOEXEC;
			goto bad;
		}
		/*
		 * The following checks are for whether the file
		 * can be loaded/executed on this system.
		 * We only have to worry about the 
		 * version number (and vendor_type) when
		 * there is a compatibility problem, 
		 * 	e.g. check that version >= N, where
		 * versions < N are not supported or are
		 * supported differently.  Then this 
		 * program only has to be updated for
		 * those changes it would find incompatible.
		 */
		if ((mo_header.moh_byte_order == OUR_BYTE_ORDER)
		    && (mo_header.moh_data_rep_id == OUR_DATA_REP_ID)
		    && (mo_header.moh_cpu_type == OUR_CPU_TYPE)
		    && (mo_header.moh_max_page_size==PAGE_SIZE)
		    && (mo_header.moh_sizeofcmds > 0)
		    && (mo_header.moh_first_cmd_off > 0)
		    && (mo_header.moh_load_map_cmd_off >= 
			mo_header.moh_first_cmd_off)
		    && (mo_header.moh_load_map_cmd_off < 
			(mo_header.moh_first_cmd_off +
			 mo_header.moh_sizeofcmds))) {
			do_o_mach_o = 1;
			goto gotobject;
		} else {
			error = ENOEXEC;
			goto bad;
		}
	} else
#endif	/* OSF-MACH-O */

#if	SYSV_COFF
#ifdef	i860
	if (exdata.coff_hdr.magic == I860MAGIC) {
		aouthdr_offset = sizeof(struct filehdr);
		goto gotcoff;
	} else
	/* XXX: tools create i386 magic in i860 executables; soon to be fixed */
	if (exdata.coff_hdr.magic == I386MAGIC) {
		aouthdr_offset = sizeof(struct filehdr);
		goto gotcoff;
	} else
#endif	/* i860 */
#ifdef	i386
	if (exdata.coff_hdr.magic == I386MAGIC) {
		aouthdr_offset = sizeof(struct filehdr);
		goto gotcoff;
	} else
#endif
#ifdef	multimax
	if (exdata.coff_hdr.magic == N16WRMAGIC ||
	    exdata.coff_hdr.magic == N16ROMAGIC) {
		aouthdr_offset = N16FILHSZ;
		goto gotcoff;
	} else if (exdata.coff_hdr.magic == NS32GMAGIC ||
		   exdata.coff_hdr.magic == NS32SMAGIC) {
		aouthdr_offset = FILHSZ;
		goto gotcoff;
	} else
#endif
#ifdef	mips
	if (exdata.coff_hdr.magic == MIPSMAGIC) {
		aouthdr_offset = FILHSZ;
		goto gotcoff;
	} else
#endif
#ifdef	__mc68000
	if (exdata.coff_hdr.magic == MC68MAGIC) {
		aouthdr_offset = FILHSZ;
		goto gotcoff;
	} else
#endif
#endif	/* SYSV_COFF */
	/*
	 *	Magic number not recognized.
	 */
	{
		error = ENOEXEC;
		goto bad;
	}

#if	SYSV_COFF
gotcoff:
	/*
	 * Now read in the second (a.out) header for segment sizes
	 * and magic number:
	 *	OMAGIC = plain executable
	 *	NMAGIC = RO text
	 *	ZMAGIC = demand paged RO text
	 *
	 * XXX On some machines, OMAGIC here does not
	 * XXX mean what OMAGIC means under BSD_A_OUT.  This code
	 * XXX may need to be fixed for those machines.
	 */
	docoff = 1;
#ifdef	OSF1_ADFS
	error = remote_exec_read(vp, (caddr_t)&ahdr,
				 sizeof(struct aouthdr), aouthdr_offset,
				 IO_UNIT, &resid);
#else
	error = vn_rdwr(UIO_READ, vp, (caddr_t)&ahdr,
		sizeof(struct aouthdr), aouthdr_offset,
		UIO_SYSSPACE, IO_UNIT, u.u_cred, &resid);
#endif
	if (error)
		goto bad;

#if	defined(mips) || defined(__hp_osf)
	/*
	 * check for unaligned entry point
	 */
	if (ahdr.entry & (sizeof(int)-1)) {
		error = ENOEXEC;
		goto bad;
	}
#endif

	switch (ahdr.magic) {
	    case OMAGIC: /* XXX */
#if	defined(mips) || defined(__hp_osf)
		/* We do it right:
		 * Do not make text read-only, e.g. put it in the data section
		 * Note that by definition text and data are contiguous both
		 * in the file and in memory.
		 */
		ahdr.data_start = ahdr.text_start;
		ahdr.dsize += ahdr.tsize;
		ahdr.tsize = 0;
		break;
#endif
	    case NMAGIC:
	    case ZMAGIC:
		if (ahdr.tsize == 0) {
			error = ENOEXEC;
			goto bad;
		}
		break;

	    default:
		error = ENOEXEC;
		goto bad;
	}
#ifdef mips
	/*
	 * Enforce (artificial) addressability limit: this covers
	 * a chip bug.
	 */
	if ((ahdr.text_start < VM_MIN_ADDRESS) || 
	      (ahdr.data_start < VM_MIN_ADDRESS)) {
		error = ENOEXEC;
		goto bad;
	}
#endif
#ifdef	multimax
	/*
	 *	XXX Alignment flags don't get set in N16 fileheaders.
	 */
	if (aouthdr_offset == N16FILHSZ)
		ahdr.flags |= U_AL_1024;
#endif
#endif	/* SYSV_COFF */

gotobject:


#ifndef	OSF1_SERVER
	/*
	 * Collect arguments and lock vnode/inode.
	 */
	if (error = exec_args_collect(loader, loader_segflg, ufile, uargv, 
		    uenvp, shell_name, shell_or_file_name_tail, shell_arg, 
		    &exec_args, &na, &ne, &nc))
		goto bad;
#else
	na = ne = nc = 0;
#endif

#ifdef	sun
	/*
	 *	Save a.out header for Sun debuggers
	 */
	current_thread()->pcb->pcb_exec = exdata.ex_exec;
#endif

#if	SYSV_COFF
	if (docoff)
		error = coff_getxfile(p, vp, &exdata.coff.fhdr, 
				      &ahdr, nc + (na+4)*NBPW, privs.is_priv);
#endif

#if	OSF_MACH_O
	if (do_o_mach_o) 
  		error = o_mach_o_getxfile(p, vp, &mo_header, &entry_addr,
					  nc + (na+4)*NBPW, privs.is_priv);
#endif

#if	BSD_A_OUT
	if (do_bsd_a_out)
#ifdef	sparc
	/*
	 * Make sure user register windows are empty before attempting to
	 * make a new stack.
	 */
		flush_user_windows();
		error = getxfile(p, vp, &exdata.ex_exec, 
				 SA(nc + (na+4)*NBPW + sizeof (struct rwindow)),
				 privs.is_priv);
#else
		error = getxfile(p, vp, &exdata.ex_exec, nc + (na+4)*NBPW,
				 privs.is_priv);
#endif	/* sparc */
#endif	/* BSD_A_OUT*/

	if (error) {
	/*
	 *	NOTE: to prevent a race condition, getxfile had
	 *	to temporarily unlock the inode.  If new code needs to
	 *	be inserted here before the iput below, and it needs
	 *	to deal with the inode, keep this in mind.
	 */
		goto bad;
	}

	/*
	 * set SUID/SGID protections, if no tracing
	 */
	BM(PROC_LOCK(p));
	pflag = p->p_flag;
	BM(PROC_UNLOCK(p));

	substitute_real_creds(p, NOCRED, privs.creds->cr_uid, NOCRED,
			      privs.creds->cr_gid, privs.creds);

	/* substitute_real_creds consumes our
	 * reference on the credentials, so
	 * arrange to avoid freeing them later.
	 */
	privs.creds = NULL;

#if	SEC_ARCH
	/* Inform security policy layer of change of ID if any */
	
	SP_CHANGE_SUBJECT();
#endif	/* SEC_ARCH */

#if	SEC_BASE
	compute_subject_privileges(&privs.vsattr);
#endif	/*SEC_BASE*/

#ifdef	OSF1_ADFS
	remote_vfree(vp);
#else
	vrele(vp);
#endif
	vp = NULL;

	/*
	 * Check for tracing and copy back arguments and environment
	 * variables.
	 */
	if (p->p_flag & STRC)
		flags |= LDR_EXEC_PTRACE_F;
#ifndef	OSF1_SERVER
	exec_args_copyback(flags, loader, exec_args, na, ne, nc);
#else
	exec_auxv_copyback(flags, loader, ufile, shell_name);
#endif

	unix_master();
	execsigs(p);
	unix_release();
#if	SYSV_COFF
	if (docoff)
#ifdef	multimax
		setregs(ahdr.entry, ahdr.mod_start);
#endif
#if	defined(i386) || defined(mips) || defined(__hp_osf) || defined(i860)
#ifndef	OSF1_SERVER
		setregs(ahdr.entry);
#else
		set_entry_address(ahdr.entry, rtp->entry, rtp->entry_count);
#endif
#endif
#endif	/* SYSV_COFF */

#if	OSF_MACH_O
	if (do_o_mach_o)
#ifdef	multimax
#define	MMAX_MOD_START	0x2000
	setregs(entry_addr, MMAX_MOD_START);
#else
#ifndef	OSF1_SERVER
	setregs(entry_addr);
#else
	set_entry_address(entry_addr, rtp->entry, rtp->entry_count);
#endif
#endif
#endif	/* OSF_MACH_O */

#if	BSD_A_OUT
	if (do_bsd_a_out)
#ifndef	OSF1_SERVER
		setregs(exdata.ex_exec.a_entry);
#else
		set_entry_address(exdata.ex_exec.a_entry, rtp->entry, rtp->entry_count);
#endif
#endif
#ifdef	vax
	{
		/*
		 *	This belongs in vax.setregs()
		 */
		extern int nsigcode[5];

		bcopy((caddr_t)nsigcode,
		      (caddr_t)(VM_MAX_ADDRESS - sizeof(nsigcode)),
		      sizeof(nsigcode));
	}
#endif
#ifdef	ibmrt
	{
	    	/*
		 *	sigcode[] must agree with declaration in pcb.h
		 *
		 *	sigcode goes at the bottom of the user_stack,
		 *	where, of course, the user's stack can grow
		 *	down on top of it, but this seems unlikely.
		 *	Putting it at the top makes ps(1) unhappy.
		 */
		extern int sigcode[3];
		bcopy((caddr_t)sigcode,
		      (caddr_t)SIGCODE_ADDRESS,
		      sizeof(sigcode));
	}
#endif

	/*
	 * Remember file name for accounting.
	 */
	u.u_acflag.fi_flag &= ~AFORK;
	bcopy((caddr_t)shell_or_file_name_tail, (caddr_t)u.u_comm, MAXCOMLEN);

bad:
#ifndef	OSF1_SERVER
	if (exec_args)
		exec_args_free(exec_args);
#endif
#ifdef	OSF1_ADFS
	if (vp)
		remote_vfree(vp);
#else
	if (vp)
		vrele(vp);
#endif
	if (privs.creds) {
		crfree(privs.creds);
		privs.creds = NULL;
	}
	if (error == EGETXFILE) {

		/* 
		 *	getxfile failed, kill the current process.
		 *	Send SIGKILL, blow away other pending signals.
		 */
		unix_master();
		p->p_sig = sigmask(SIGKILL);
		p->p_cursig = SIGKILL;
		u.u_sig = 0;
		u.u_cursig = 0;
		psig();		/* Bye */
		unix_release();
	}
	return (error);
}

/*
 * exec_get_effective_ids()
 */
int
exec_get_effective_ids(vp, vap, privs)
	struct vnode *vp;
	struct vattr *vap;
	struct	exec_privs	*privs;
{
	struct mount *mp;
	int	m_flag;
#if	SEC_BASE
	boolean_t isroot = (u.u_cred->cr_uid == 0);
#endif	/* SEC_BASE */

	BM(VN_LOCK(vp));
	mp = vp->v_mount;
	BM(VN_UNLOCK(vp));
	if (mp == DEADMOUNT)
		return(ENODEV);
	BM(MOUNT_LOCK(mp));
	m_flag = mp->m_flag;
	BM(MOUNT_UNLOCK(mp));
	if (m_flag & M_NOEXEC)
		return (EACCES);
        if (((m_flag & M_NOSUID) == 0) &&
	    (vap->va_mode & (VSUID | VSGID))) {
		privs->creds = crdup(u.u_cred);
		privs->is_priv = TRUE;
		if (vap->va_mode & VSUID) {
			privs->creds->cr_uid = vap->va_uid;
#if	SEC_BASE
			if (privs->creds->cr_uid == 0)
				isroot = TRUE;
#endif	/* SEC_BASE */
		}
		if (vap->va_mode & VSGID)
			privs->creds->cr_gid = vap->va_gid;
	} else
		reset_effective_ids(privs);

#if	SEC_BASE
        if ((m_flag & M_NOSUID) == 0 &&
	    get_subject_privileges(vp, &privs->vsattr, isroot))
		privs->is_priv = TRUE;
#endif	/* SEC_BASE */

	/* If a nonprivileged task holds port rights to this task, can't
	 * allow privileges, so back out of the current operation if so.
	 */

	if (privs->is_priv && !task_secure(current_task())) {
		uprintf("%s: privileges disabled because of outstanding IPC access to task\n",
			u.u_comm);
		reset_effective_ids(privs);
	}

	return(0);
}

/*
 * reset_effective_ids()
 */
int
reset_effective_ids(privs)
	struct exec_privs *privs;
{
	privs->is_priv = FALSE;
	if (privs->creds != NULL)
		crfree(privs->creds);
	privs->creds = u.u_cred;
	crhold(privs->creds);

#if	SEC_BASE
	reset_subject_privileges(&privs->vsattr);
#endif	/* SEC_BASE */
	return(0);
}


/*
 * exec_check_access()
 */
int
exec_check_access(p, vp, vap)
	struct proc  *p;
	struct vnode *vp;
	struct vattr *vap;
{
	int flag;
	enum vtype type;
	int error;

	VOP_ACCESS(vp, VEXEC, u.u_cred, error);
	if (error)
		return (error);
	PROC_LOCK(p);
	flag = p->p_flag;
	PROC_UNLOCK(p);
	if (flag & STRC) {
		VOP_ACCESS(vp, VREAD, u.u_cred, error);
		if (error)
			return (error);
	}
	VN_LOCK(vp);
	type = vp->v_type;
	VN_UNLOCK(vp);
	if (type != VREG ||
	    (vap->va_mode & (VEXEC|(VEXEC>>3)|(VEXEC>>6))) == 0)
		return (EACCES);
	return (0);
}

#ifdef OSF1_ADFS

int
do_exec_setup_suid(vp, pflag, suid, new_uid, new_gid, new_is_priv)
	struct vnode	*vp;
        int             pflag;
        boolean_t       suid;
        int             *new_uid;       /* out */
        int             *new_gid;       /* out */
        boolean_t       *new_is_priv;   /* out */
{
        struct vattr            vattr;
        struct exec_privs       privs;
        int                     error;
	struct proc		*p;

	if (!suid) {	/* just check access */
		VOP_GETATTR(vp, &vattr, u.u_cred, error);
		if (error)
			return (error);
		p = u.u_procp;
		p->p_flag = pflag;
		error = exec_check_access(p, vp, &vattr);

		return(error);
	}
        *new_uid = *new_gid = -1;
        *new_is_priv = FALSE;
        privs.creds = (struct ucred *)0;

        VOP_GETATTR(vp, &vattr, u.u_cred, error);
        if (error)
                goto bad;
        if (error = exec_get_effective_ids(vp, &vattr, &privs))
                goto bad;
        p = u.u_procp;
        p->p_flag = pflag;
        error = exec_check_access(p, vp, &vattr);

#if     SEC_BASE
        /*
         * If not on NOSUID filesystem, check for EXECSUID privilege
         * for SUID programs.  Audit change of IDs.
         */
{
                int mflag;
                MOUNT_LOCK(vp->v_mount);
                mflag = vp->v_mount->m_flag;
                MOUNT_UNLOCK(vp->v_mount);
                if (!(mflag & M_NOSUID) && !exec_allowed(&vattr)) {
                        error = u.u_error;    /* XXX - change exec_allowed! */
                        goto bad;
                }
        }
#endif

bad:
        if (error == ESUCCESS) {
                *new_uid = privs.creds->cr_uid;
                *new_gid = privs.creds->cr_gid;
                *new_is_priv = privs.is_priv;
        }
        if (privs.creds != (struct ucred *)0)
                crfree(privs.creds);

	return(error);
}

#endif /* OSF1_ADFS */

/*
 * exec_get_shell()
 *
 * exec_get_shell() parses a character string checking if #!
 * interpretation is requested.  It only examines the first MAXINTERP
 * characters of the string, line.  If line, begins with #!,
 * exec_get_shell() performs the rest of the normal #! processing.
 * exec_get_shell() returns 0 if successful.  If successful,
 * *shell_namep indicates whether #! interpretation was requested.  If
 * *shell_namep is NULL, no #! interpretation was requested.  If
 * *shell_namep is non-NULL, then #! interpretation was requested and
 * *shell_namep is a pointer to the pathname string of the shell that
 * should be loaded and *shell_argp contains a pointer to the optional
 * shell argument string that should be passed to that shell.  Not
 * that *shell_argp may be NULL exec_get_shell() returns -1 if an
 * error occurred and sets u.u_error  to  indicate the error. 
 */
int
exec_get_shell(line, shell_namep, shell_argp)
	char *line, **shell_namep, **shell_argp;
{    
	char *cp;

	*shell_namep = *shell_argp = (char *)0;
	if ((line[0] == '#') && (line[1] == '!')) {
		for (cp = &line[2];; ++cp) {
			if (cp >= &line[MAXINTERP])
				return (ENOEXEC);
			if (*cp == '\n') {
				*cp = '\0';
				break;
			}
			if (*cp == '\t')
				*cp = ' ';
		}
		cp = &line[2];
		while (*cp == ' ')
			cp++;
		*shell_namep = cp;
		while (*cp && *cp != ' ')
			cp++;
		if (*cp) {
			*cp++ = '\0';
			while (*cp == ' ')
				cp++;
			if (*cp)
				*shell_argp = cp;
		}
	}
	return (0);
}

/*
 * exec_args_set().
 * 
 * exec_args_set() fills the uarea with some data from the emulator.
 */
int
exec_args_set(p, argp, arg_size, envp, env_size)
	struct proc	*p;
	char		*argp;
	int		arg_size;
	char		*envp;
	int		env_size;
{
	p->p_utask.uu_argp = argp;
	p->p_utask.uu_arg_size = arg_size;
	p->p_utask.uu_envp = envp;
	p->p_utask.uu_env_size = env_size;

	return 0;
}

/*
 * exec_auxv_copyback()
 *
 * exec_auxv_copyback() sends the data destined to become the auxiliary
 * vector back to the emulator to fill in for us. This replaces
 * exec_args_copyback() which fills in the arguments, environment and
 * auxiliary vector in the monolithic kernel but as the emulator already
 * has this information in the server, we just need to send nack the 
 * auxiliary vector.
 */
void
exec_auxv_copyback(flags, loader, filename, shell_name)
int	flags;
char	*loader;
char	*filename;
char	*shell_name;
{
	struct proc	*p;
	char		*ptr, *optr;
	int		i;
	auxv_type_t	*auxv_ptr;

	p = u.u_procp;
	optr = ptr = p->p_auxv_strings;
	auxv_ptr = p->p_auxv_structs;
	i = 0;

	auxv_ptr->a_type = AT_EXEC_FILENAME;
	auxv_ptr->a_un.a_ptr = 0;
	auxv_ptr++; i++;
	if (shell_name) {
		bcopy(shell_name, ptr, strlen(shell_name) + 1);
		ptr += strlen(shell_name) + 1;
	} else {
		bcopy(filename, ptr, strlen(filename) + 1);
		ptr += strlen(filename) + 1;
	}

	if (loader) {
		auxv_ptr->a_type = AT_EXEC_LOADER_FILENAME;
		auxv_ptr->a_un.a_ptr = (long)(ptr - optr);
		auxv_ptr++; i++;
		bcopy(loader, ptr, strlen(loader) + 1);
		ptr += strlen(loader) + 1;

		auxv_ptr->a_type = AT_EXEC_LOADER_FLAGS;
		auxv_ptr->a_un.a_val = (long)flags;
		auxv_ptr++; i++;
	}
	auxv_ptr->a_type = AT_NULL;
	auxv_ptr->a_un.a_val = 0;
	i++;
	p->p_auxv_structs_count = i;
	p->p_auxv_strings_count = (unsigned int)(ptr - optr);
}

#ifndef	OSF1_SERVER
/*
 * exec_args_collect()
 *
 * exec_args_collect() collects the argument variables, the environment
 * variables, the file name string and the loader name string if present,
 * placing them in a chunk of pageable kernel memory.
 * exec_args_collect() returns 0 if successful.  It returns -1 and sets
 * u.u_error to indicate the error if it fails.
 * 
 * shell_name is a pointer to the file name of the shell if #!
 * interpretation was  requested.  shell_name_tail contains the tail
 * of the canonicallized form of that file name, as returned by
 * namei().  Note well, shell_name indicates whether #! interpretation
 * was requested.  Therefore shell_name is equivalent to the old indir
 * variable.  If shell_name is non-NULL, the arguments are massaged
 * such that argv[0] is the tail of the canonicallized form of the
 * shell file name, as returned by namei(), argv[1] is the argument
 * string to the shell (e.g. shell_arg) if present, argv[2] is the
 * original file name, and the  remaining arguments come from the
 * original argv[], skipping the original argv[0]. 
 */
int
exec_args_collect(loader, loader_segflg, ufile, uargp, uenvp, shell_name,
    shell_name_tail, shell_arg, exec_argsp, nap, nep, ncp)
	char *loader, *ufile, **uargp, **uenvp;
	char *shell_name, *shell_name_tail, *shell_arg;
	enum uio_seg loader_segflg;
	int *nap, *nep, *ncp;
	vm_offset_t *exec_argsp;
{
	int na, ne, nc, ap, cc, error;
	vm_offset_t exec_args, exec_args_allocate();
	char *cp;
	unsigned len;
	char **from;

	na = 0;
	ne = 0;
	nc = 0;
	exec_args = exec_args_allocate();
	cp = (char *) exec_args;	/* running pointer for copy */
	cc = ARG_MAX;			/* size of exec_args */
					/*   (actually only ARG_MAX to limit */
					/*   arguments and environment */
					/*   variables) */

	/*
	 * Copy arguments and environment variables.  Note if no
	 * arguments, then no environment variables are copied.
	 */
	if (uargp) {

		from = uargp;

		if (shell_name) {

			/* argv[0] */
			if ((ap = fuword((caddr_t)from)) == -1) {
				error = EFAULT;
				goto bad;
			}
			if (ap != NULL)
				from++;		/* skip argv[0] */
			error = exec_copystr(shell_name_tail, &cp, &cc, &len, &nc);
			if (error)
				goto bad;
			na++;

			/* optional argv[1] */
			if (shell_arg) {
				error = exec_copystr(shell_arg, &cp, &cc, &len, &nc);
				if (error)
					goto bad;
				na++;
			}

			/* argv[shell_arg ? 2 : 1] */
			error = exec_copyinstr(ufile, &cp, &cc, &len, &nc);
			if (error)
				goto bad;
			na++;
		}

		for ( ; ((ap = fuword((caddr_t)from)) != NULL); from++) {
			if (ap == -1) {
				error = EFAULT;
				goto bad;
			}
			error = exec_copyinstr((caddr_t)ap, &cp, &cc, &len, &nc);
			if (error)
				goto bad;
			na++;
		}

		if (uenvp) {
			for (from = uenvp; ((ap = fuword((caddr_t)from)) != NULL); from++) {
				if (ap == -1) {
					error = EFAULT;
					goto bad;
				}
				error = exec_copyinstr((caddr_t)ap, &cp, &cc, &len, &nc);
				if (error)
					goto bad;
				na++;
				ne++;
			}
		}
	}

	/* additional space for file name and loader name */
	cc += (NCARGS - ARG_MAX);

	/* copy file name */
	if (shell_name)
		error = exec_copystr(shell_name, &cp, &cc, &len, &nc);
	else
		error = exec_copyinstr((caddr_t)ufile, &cp, &cc, &len, &nc);
	if (error)
		goto bad;

	/* copy loader name if present */
	if (loader) {
		if (loader_segflg == UIO_USERSPACE)
			error = exec_copyinstr((caddr_t)loader, &cp, &cc, &len, &nc);
		else
			error = exec_copystr(loader, &cp, &cc, &len, &nc);
		if (error)
			goto bad;
	}

	/* align character count */
	nc = (nc + NBPW-1) & ~(NBPW-1);

	/*
	 * Success
	 */
	*exec_argsp = exec_args;
	*nap = na;
	*nep = ne;
	*ncp = nc;
	return (0);

	/*
	 * Failure
	 */
bad:
	exec_args_free(exec_args);
	return (error);
}

/*
 * exec_args_copyback()
 *
 * exec_args_copyback() copies the argument count, the argument vector,
 * the environment variables and the new auxiliary vector back to the
 * user stack.  The loader name string, if present, and the file name
 * string are the first strings in the block holding the arguments and
 * the environment variables.
 *
 * There are two forms of auxv[], depending on whether
 * exec_args_copyback() was called from exec_with_loader() or execve().
 * A non-zero value in loader indicates the former.  auxv[] will always
 * the filename.  If a loader is present, then auxv[] will also
 * contain the loader filename and the loader flags.  Therefore,
 * including the AT_NULL entry, if loader is set, auxv[] will contain
 * 4 entries, otherwise it will only contain 2 entries.  Note that
 * each element occupies 8 bytes.
 */
void
exec_args_copyback(flags, loader, exec_args, na, ne, nc)
	vm_offset_t exec_args;
	int flags, na, ne, nc;
	char *loader;
{
	int i, nav, ap, ucp, cc, error, uloader, ufile;
	unsigned len;
	char *cp;

	/* 
	 * Compute number of elements in auxv[]
	 */
	nav = loader ? 4 : 2;

	/*
	 * Compute locations for strings, vectors and data in user
	 * address space.
	 */
#ifdef mips
	ucp = USRSTACK - nc - NBPW - EA_SIZE;
	ap = ucp - na*NBPW - 3*NBPW - (nav*2)*NBPW;
	if (ap & 0xf) {
		int fudge;
		fudge = ap - (ap & ~0xf);
		ap -= fudge;
		ucp -= fudge;
	}
	u.u_ar0[EF_SP] = ap;
#else
	ucp = USRSTACK - nc - NBPW;
#ifdef sparc
        /*
         * Keep stack aligned and leave room for initial reg window on sparc.
         */
        ap = USRSTACK - SA(nc + (na+4+(nav*2))*NBPW);
        u.u_ar0[SP] = ap - sizeof (struct rwindow);
#else	/* sparc */
	ap = ucp - na*NBPW - 3*NBPW - (nav*2)*NBPW;
#ifdef	i386
	u.u_ar0[UESP] = ap;
#else
	u.u_ar0[SP] = ap;
#endif
#endif	/* sparc */
#endif	/* mips */

	/*
	 * Initialize pointers and counters.
	 */
	nc = 0;
	cc = 0;
	cp = (char *) exec_args;
	cc = NCARGS;

	/*
	 * Process argc, argv[] and envp[]
	 */
	(void) suword((caddr_t)ap, na-ne);
	ap += NBPW;
        u.u_argp = (char *)ucp;
	for (i = 0; i < (na - ne); i++) {
		(void) suword((caddr_t)ap, ucp);
		error = exec_copyoutstr(&cp, (caddr_t)ucp, &cc, &len, &nc);
		ucp += len;
		if (error == EFAULT)
			panic("exec: EFAULT");
		ap += NBPW;
	}
        u.u_arg_size = ucp - (unsigned long)u.u_argp;
	(void) suword((caddr_t)ap, 0);
	ap += NBPW;
        u.u_envp = (char *)ucp;
	for (i = 0; i < ne; i++) {
		(void) suword((caddr_t)ap, ucp);
		error = exec_copyoutstr(&cp, (caddr_t)ucp, &cc, &len, &nc);
		ucp += len;
		if (error == EFAULT)
			panic("exec: EFAULT");
		ap += NBPW;
	}
        u.u_env_size = ucp - (unsigned long)u.u_envp;
	(void) suword((caddr_t)ap, 0);
	ap += NBPW;

	/*
	 * Copyout file name string if present and remember address
	 */
	ufile = ucp;
	error = exec_copyoutstr(&cp, (caddr_t)ucp, &cc, &len, &nc);
	ucp += len;
	if (error == EFAULT)
		panic("exec: EFAULT");

	/*
	 * Copyout loader name string if present and remember address
	 */
	if (loader) {
		uloader = ucp;
		error = exec_copyoutstr(&cp, (caddr_t)ucp, &cc, &len, &nc);
		ucp += len;
		if (error == EFAULT)
			panic("exec: EFAULT");
	}

	/*
	 * Process auxv[].  See above comments.
	 */
	(void) suword((caddr_t)ap, AT_EXEC_FILENAME); ap += NBPW;
	(void) suword((caddr_t)ap, ufile); ap += NBPW;
	if (loader) {
		(void) suword((caddr_t)ap, AT_EXEC_LOADER_FILENAME); ap += NBPW;
		(void) suword((caddr_t)ap, uloader); ap += NBPW;
		(void) suword((caddr_t)ap, AT_EXEC_LOADER_FLAGS); ap += NBPW;
		(void) suword((caddr_t)ap, flags); ap += NBPW;
	}
	(void) suword((caddr_t)ap, AT_NULL); ap += NBPW;
	(void) suword((caddr_t)ap, 0); ap += NBPW;
}

/*
 * exec_args_allocate()
 *
 * exec_args_allocate() allocates memory to hold argument
 * variables and environment variables.  The size is always
 * a constant, NCARGS.
 */
vm_offset_t
exec_args_allocate()
{
	vm_offset_t exec_args;

	exec_args = kmem_alloc_pageable(kernel_pageable_map, NCARGS);
	if (exec_args == 0)
		panic("exec_args_allocate: cannot allocate arguments");
	return(exec_args);
}

/*
 * exec_args_free()
 *
 * exec_args_free() frees the memory allocated to hold argument
 * variables and environment variables.  The size is always a
 * constant, NCARGS.
 */
exec_args_free(exec_args)
	vm_offset_t exec_args;
{
	if (exec_args)
		kmem_free(kernel_pageable_map, exec_args, NCARGS);
}

/*
 * exec_copystr()
 * exec_copyinstr()
 * exec_copyoutstr()
 *
 * These are exec() versions of copystr(), copyinstr() and copyoutstr().
 * They not only do the obvious copy operation, but they also bump the
 * character pointer, decrement the maximum count and increment the
 * current count.  They also map ENOENT, which means string exceeded
 * maximum length, to E2BIG, which is what exec() would like to return.
 */
int
exec_copystr(src, destp, maxlengthp, lencopiedp, ncp)
	char *src, **destp;
	int *maxlengthp, *ncp;
	unsigned *lencopiedp;
{
	unsigned len;
	int error;

	error = copystr(src, *destp, (unsigned)*maxlengthp, &len);

	if (error == ENOENT)
		error = E2BIG;

	*lencopiedp = len;
	*destp += len;
	*ncp += len;
	*maxlengthp -= len;

	return(error);
}

int
exec_copyinstr(user_src, kernel_destp, maxlengthp, lencopiedp, ncp)
	caddr_t user_src;
	char **kernel_destp;
	int *maxlengthp, *ncp;
	unsigned *lencopiedp;
{
	unsigned len;
	int error;

	error = copyinstr(user_src, *kernel_destp, (unsigned)*maxlengthp, &len);

	if (error == ENOENT)
		error = E2BIG;

	*lencopiedp = len;
	*kernel_destp += len;
	*ncp += len;
	*maxlengthp -= len;

	return(error);
}

int
exec_copyoutstr(kernel_srcp, user_dest, maxlengthp, lencopiedp, ncp)
	char **kernel_srcp;
	caddr_t user_dest;
	int *maxlengthp, *ncp;
	unsigned *lencopiedp;
{
	unsigned len;
	int error;

	error = copyoutstr(*kernel_srcp, user_dest, (unsigned)*maxlengthp, &len);

	if (error == ENOENT)
		error = E2BIG;

	*lencopiedp = len;
	*kernel_srcp += len;
	*ncp += len;
	*maxlengthp -= len;

	return(error);
}
#endif	/* OSF1_SERVER */
