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
 
/* %W% %U% %G% SecureWare */
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: kern_kmodcall.c,v $
 * Revision 1.4  1994/11/18  20:27:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:47:26  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:46:37  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:15:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:05:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:06:35  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1992/03/09  15:56:08  durriya
 * 	initial check-in
 * 	[Revision 3.0  91/12/24  12:37:34  sp]
 * 	brought in from 1.0.3 for the kmodcall syscall
 *
 */

#include <sys/secdefines.h>
#include <sys/security.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/sysconfig.h>

/*
 * NAME:	kmodcall()
 *
 * FUNCTION:	Calls a kernel loaded subsystem's {}configure() entry point
 *		Passing an input and output buffers, akin to ioctl().
 */
kmodcall(p, args, retval)
        register struct proc *p;
	void *args;
	int *retval;
{
        struct args {
	    sysconfig_entrypt_t	entrypt; /* Configuration entry pt address */
	    sysconfig_op_t	op;	/* Configuration command request */
	    caddr_t	in_buf;		/* Input data buffer */
	    size_t   	in_len;		/* Input data buffer length */
	    caddr_t	out_buf;	/* Output data buffer */
	    size_t 	out_len;	/* Output data buffer length */
        } *uap = (struct args *)args;
        caddr_t in_memp = NULL;		/* Auxillary input data kernel buffer */
        caddr_t out_memp = NULL;	/* Auxillary output data kernel buffer */
#define STK_PARMS       128		
        char in_stkbuf[STK_PARMS];	/* Static input data buffer */
        char out_stkbuf[STK_PARMS];	/* Static output data buffer */
        caddr_t in_data = in_stkbuf;	/* Input data buffer */
        caddr_t out_data = out_stkbuf;	/* Output data buffer */
	int	error;

#if SEC_BASE
						/* Must have privilege */
	if (!privileged(SEC_DEBUG, EPERM))	/* XXX need new priv */
		return (EPERM);
#else
						/* Must be super user */
        if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
						/* Sanity check entry pt */
	if (uap->entrypt == NULL) 
		return (EINVAL);

	/*
	 * Create and copyin input buffer
	 */
        if (uap->in_len > SYSCONFIG_PARAM_MAX)
		return (EINVAL);
        if (uap->in_len > sizeof(in_stkbuf)) {
		if ((in_memp = (caddr_t) kalloc(uap->in_len)) == NULL)
			return (ENOMEM);
		in_data = in_memp;
	}
	if (uap->in_len)
		if (error=copyin(uap->in_buf, (caddr_t)in_data, uap->in_len))
			goto out;

	/*
	 * Create and initialize output buffer
	 */
        if (uap->out_len > SYSCONFIG_PARAM_MAX) {
		error = EINVAL;
		goto out;
        }
        if (uap->out_len > sizeof(out_stkbuf)) {
		if ((out_memp = (caddr_t) kalloc(uap->out_len)) == NULL) {
			error = ENOMEM;
			goto out;
		}
		out_data = out_memp;
	}
	if (uap->out_len)
		bzero((caddr_t)out_data, uap->out_len);

	/*
	 * Call configure{} entry point, passing input and output buffers
	 */
	error = (*uap->entrypt)(uap->op, 
				in_data, uap->in_len, 
				out_data, uap->out_len);

	/*
	 * If {}configure has no error, copy data
	 */
	if (error == 0) 
		copyout((caddr_t)out_data, (caddr_t)uap->out_buf, uap->out_len);

out:
        if (in_memp)	
		kfree(in_memp, uap->in_len);
        if (out_memp)	
		kfree(out_memp, uap->out_len);
	return (error);
}

