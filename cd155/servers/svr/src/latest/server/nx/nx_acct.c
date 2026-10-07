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
 * HISTORY
 * $Log: nx_acct.c,v $
 * Revision 1.3  1994/11/18  20:37:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  22:33:21  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:31:29  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:25:43  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/08/25  09:37:38  rkl
 * Initial revision
 *
 *
 */

#include <sys/secdefines.h>
#include <sys/proc.h>

/* ARGSUSED */
nx_getacctid(p, uap, retval)
	struct proc *p;
	void *uap;
	int *retval;
{
	BM(PROC_LOCK(p));
	*retval = u.u_cred->cr_acctid;	/* XXX is this the right thing to do */
	BM(PROC_UNLOCK(p));
	retval[1] = u.u_cred->cr_acctid;
	return (0);
}

nx_set_acctid(p, acctid)
	struct proc *p;
	gid_t acctid;
{
	struct ucred	*newcr, *cr;
	int	error;

	fetch_real_creds(p, 0, 0, 0, 0, &cr);

#if     SEC_BASE
	    if (!privileged(SEC_SETPROCIDENT, EPERM))) {
		crfree(cr);
		return (EPERM);
	}
#else
	    if (error = suser(cr, &u.u_acflag)) {
		crfree(cr);
		return (error);
	}
#endif
	newcr = crcopy(cr);
	newcr->cr_acctid = acctid;
	substitute_real_creds(p, NOUID, NOUID, NOUID, NOUID, newcr);
#if     SEC_ARCH
	SP_CHANGE_SUBJECT();
#endif
	return (0);
}

/* ARGSUSED */
nx_setacctid(p, args, retval)
	register struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		gid_t	acctid;
	} *uap = (struct args *)args;
	register gid_t acctid;

	if ((int)uap->acctid < 0)
		return(EINVAL);

#if     SEC_BASE
	if (!issetluid())
		return (EPERM);
#endif
	acctid = uap->acctid;
	return(nx_set_acctid(p, acctid));
}
