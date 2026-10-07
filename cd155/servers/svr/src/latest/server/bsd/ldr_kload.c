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
 * $Log: ldr_kload.c,v $
 * Revision 1.5  1994/11/18  20:27:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:48:31  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:48:16  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:04:09  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:24:38  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.4  1992/03/09  14:21:50  durriya
 * 	Revision 3.2  91/12/20  17:56:20  barbou
 * 	Bug fix (from Paul Roy): missing "address" parameter to vm_protect().
 *
 * Revision 2.3  91/12/20  10:03:40  rabii
 * 	Added missing argument to vm_protect call
 * 
 * Revision 2.2  91/12/16  13:05:52  roy
 * 	Initial check-in.
 * 
 * Revision 3.1  91/10/30  17:41:56  bernadat
 * Bug fix for checking unallocated memory in klc_vm_read(). [barbou]
 * 
 * Revision 3.0  91/10/29  16:10:36  barbou
 * From OSF/1.0.2, adapted to Mach3.0 VM interface.
 * 
 * Revision 1.5  90/10/07  13:18:24  devrcs
 * 	Modified KLC_VM_ALLOCATE and KLC_VM_ALLOCATE_WIRED to set
 * 	the default protection to RW so that a later call to vm_protect()
 * 	that adds X will cause caches to be synchronized.  RW will
 * 	be the default for vm_allocate() in a future release and at
 * 	that time this code should be removed.
 * 	[90/10/03  09:46:16  kwallace]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  08:58:01  gm]
 * 
 * Revision 1.4  90/08/24  11:17:54  devrcs
 * 	Changes for new system call interface.
 * 	Removed include of syscontext.h
 * 	[90/08/17  17:38:38  nags]
 * 
 * Revision 1.3  90/08/09  13:14:28  devrcs
 * 	Forgot to make sure size is page-aligned in KLC_VM_ALLOCATE_WIRED.
 * 	[90/07/27  07:57:04  kwallace]
 * 
 * Revision 1.2  90/06/22  20:06:38  devrcs
 * 	Added a small change around call to suser() for security.
 * 	[90/06/15  11:53:14  kwallace]
 * 
 * 	Initial Revision.
 * 	[90/06/06  21:28:52  kwallace]
 * 
 * $EndLog$
 */

/*
 * This file implements the kloadcall(2) system call.  kloadcall(2)
 * provides functionality to manipulate the kernel address space for
 * kernel dynamic loading.  We would have preferred to use the normal
 * user-mode Mach VM primitives in conjunction with task_by_unix_pid(-1), 
 * however, there were and are many complications to that approach.
 * Due to various constraints we chose to place the necessary
 * functionality into kloadcall(2), at least until some future time.
 * The interfaces to kloadcall(2) have been structured to closely
 * resemble the user-mode Mach VM primitives, in order to make it
 * easier for applications, such as the kernel load server, to switch
 * to the user-mode Mach VM primitives when they are available.
 *
 * N.B.:  Beware that the kloadcall() system call is a hybrid between
 *        a UN*X system call and a Mach VM call.  It can return errors
 *        in two ways.  We are fortunate that ESUCCESS and
 *        KERN_SUCCESS are the same value.  A UN*X error is returned,
 *        in the traditional way, by having kloadcall(), return a UN*X
 *        errno value.  A Mach VM error (i.e. a kern_return_t) is
 *        returned as the return value (i.e. *retval) of the
 *        kloadcall() system call.
 *
 *        Another way of looking at this is from the user-mode
 *        application's perspective.  When kloadcall() return 0 (i.e.
 *        ESUCCESS or KERN_SUCCESS), the kloadcall() operation was
 *        successful.  When kloadcall() returns -1, kloadcall() failed
 *        with a UN*X system call error and the application should
 *        check errno for the reason.  When kloadcall() does not
 *        return zero nor -1, kloadcall() failed with a Mach VM call
 *        error and the return value is a kern_return_t that indicates
 *        the reason for the failure. 
 */

#include <sys/secdefines.h>
#include <sys/security.h>

#include <sys/types.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>

#include <mach/std_types.h>
#include <mach/mach_types.h>
#ifdef	OSF1_SERVER
#include <uxkern/vm_param.h>
#else	/* OSF1_SERVER */
#include <mach/vm_param.h>
#endif	/* OSF1_SERVER */

#include <sys/kloadcall.h>

#define	KLOADCALL_TRACE	1

#ifdef	KLOADCALL_TRACE
#define	dprintf(x)	if (kloadcall_trace)	uprintf x
int kloadcall_trace = 0;
#else
#define	dprintf(x)
#endif

kloadcall(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		int	op;
	} *uap = (struct args *) args;
	int error;

	/*
	 * Must have privilege
	 */
#if SEC_BASE
	if (!privileged(SEC_DEBUG, EPERM))	/* XXX which priv needed? */
		return(EPERM);
#else
	if (error = suser(u.u_cred, &u.u_acflag))
		return(error);
#endif

	switch (uap->op) {

	default:
		dprintf(("kloadcall: kloadcall(%d)\n", uap->op));
		error = EINVAL;
		break;

	case KLC_VM_ALLOCATE:
		error = klc_vm_allocate(p, args, retval);
		break;

	case KLC_VM_DEALLOCATE:
		error = klc_vm_deallocate(p, args, retval);
		break;

	case KLC_VM_READ:
		error = klc_vm_read(p, args, retval);
		break;

	case KLC_VM_WRITE:
		error = klc_vm_write(p, args, retval);
		break;

	case KLC_VM_PROTECT:
		error = klc_vm_protect(p, args, retval);
		break;

	case KLC_VM_ALLOCATE_WIRED:
		error = klc_vm_allocate_wired(p, args, retval);
		break;

	case KLC_CALL_FUNCTION:
		error = klc_call_function(p, args, retval);
		break;
	}

	if (error) {
		dprintf(("kloadcall: value=-1, errno=%d\n", error));
	} else {
		dprintf(("kloadcall: value=%d(0x%x)\n", *retval,
			*retval));
	}
	return(error);
}

klc_vm_allocate(p, args, retval)
	struct proc *p;
	void *args;
	register int *retval;
{
	register struct args {
		int		op;
		vm_offset_t	*address;
		vm_size_t	size;
		boolean_t	anywhere;
	} *uap = (struct args *) args;
	vm_offset_t	address;
	
	dprintf(("kloadcall: kloadcall(KLC_VM_ALLOCATE, address=0x%x, size=%d(0x%x), anywhere=%d)\n",
		uap->address, uap->size, uap->size, uap->anywhere));

	if (!uap->anywhere) {
		if (copyin((caddr_t)uap->address, (caddr_t)&address,
		    sizeof(address))) {
			*retval = KERN_INVALID_ADDRESS;
			return(0);
		}

		dprintf(("kloadcall: *address=0x%x IN\n", address));

		if (trunc_page(address) != address) {
			*retval = KERN_INVALID_ARGUMENT;
			return(0);
		}
	}

	if (trunc_page(uap->size) != uap->size) {
		*retval = KERN_INVALID_ARGUMENT;
		return(0);
	}

#ifdef	OSF1_SERVER
	if ((*retval = vm_allocate(mach_task_self(), &address,
				   uap->size, uap->anywhere)) != KERN_SUCCESS)
		return 0;
#else	/* OSF1_SERVER */
	if ((*retval = kernel_memory_allocate_paged(kernel_map, &address,
	    uap->size, uap->anywhere)) != KERN_SUCCESS)
		return(0);
#endif	/* OSF1_SERVER */

	/*
	 * Set the default protection to RW so that a later
	 * call to vm_protect() that adds X will cause caches
	 * to be synchronized.  RW will be the default for
	 * vm_allocate() in a future release and at that time
	 * this code should be removed.
	 */
#ifdef	OSF1_SERVER
	if ((*retval = vm_protect(mach_task_self(), address, uap->size, FALSE,
				  (VM_PROT_READ|VM_PROT_WRITE)))
	     != KERN_SUCCESS) {
		vm_deallocate(mach_task_self(), address, uap->size);
		*retval = KERN_INVALID_ADDRESS;
		return 0;
	}
#else	/* OSF1_SERVER */
	if ((*retval = vm_protect(kernel_map, address, uap->size,
	    FALSE, (VM_PROT_READ|VM_PROT_WRITE))) != KERN_SUCCESS) {
		kmem_free(kernel_map, address, uap->size);
		*retval = KERN_INVALID_ADDRESS;
		return(0);
	}
#endif	/* OSF1_SERVER */

	if (copyout((caddr_t)&address, (caddr_t)uap->address,
	    sizeof(address))) {
#ifdef	OSF1_SERVER
		vm_deallocate(mach_task_self(), address, uap->size);
#else	/* OSF1_SERVER */
		kmem_free(kernel_map, address, uap->size);
#endif	/* OSF1_SERVER */
		*retval = KERN_INVALID_ADDRESS;
		return(0);
	}

	dprintf(("kloadcall: *address=0x%x OUT\n", address));

	*retval = KERN_SUCCESS;
	return(0);
}

klc_vm_deallocate(p, args, retval)
	struct proc *p;
	void *args;
	register int *retval;
{
	register struct args {
		int		op;
		vm_offset_t	address;
		vm_size_t	size;
	} *uap = (struct args *) args;

	dprintf(("kloadcall: kloadcall(KLC_VM_DEALLOCATE, address=0x%x, size=%d(0x%x))\n",
		uap->address, uap->size, uap->size));

	if ((trunc_page(uap->address) != uap->address)
	    || (trunc_page(uap->size) != uap->size)) {
		*retval = KERN_INVALID_ARGUMENT;
		return(0);
	}

#ifdef	OSF1_SERVER
	vm_deallocate(mach_task_self(), uap->address, uap->size);
#else	/* OSF1_SERVER */
	kmem_free(kernel_map, uap->address, uap->size);
#endif	/* OSF1_SERVER */
	*retval = KERN_SUCCESS;
	return(0);
}

klc_vm_read(p, args, retval)
	struct proc *p;
	void *args;
	register int *retval;
{
	register struct args {
		int		op;
		vm_offset_t	address;
		vm_size_t	size;
		vm_offset_t	*data;
		int		*data_count;
	} *uap = (struct args *) args;
	vm_offset_t	data;
	int		error;

	dprintf(("kloadcall: kloadcall(KLC_VM_READ, address=0x%x, size=%d(0x%x), data=0x%x, data_count=0x%x)\n",
		uap->address, uap->size, uap->size, uap->data,
		uap->data_count));

	if ((trunc_page(uap->address) != uap->address)
	    || (trunc_page(uap->size) != uap->size)) {
		*retval = KERN_INVALID_ARGUMENT;
		return(0);
	}

#ifdef	OSF1_SERVER
	if ((*retval = vm_allocate(p->p_task, &data, uap->size, TRUE))
	    != KERN_SUCCESS)
		return 0;
#else	/* OSF1_SERVER */
	if ((*retval = vm_allocate(current_task()->map, &data, uap->size,
	    TRUE)) != KERN_SUCCESS)
		return(0);
#endif	/* OSF1_SERVER */

	if (error = copyout((caddr_t)&data, (caddr_t)uap->data, sizeof(data)))
		goto out;

	if (error = copyout((caddr_t)&uap->size, (caddr_t)uap->data_count,
	    sizeof(uap->size)))
		goto out;

	/* 
	 * Should check to make sure that source kernel address
	 * is acessible.  For now, we expect copyout() to catch
	 * not only an inacessible user target address but also
	 * an inacessible source kernel address.
	 */

#ifdef	OSF1_SERVER
	{
		/* Check the source address: copyout won't do it for us */
		vm_address_t address;
		vm_size_t size;
		vm_prot_t protection, max_protection;
		vm_inherit_t inheritance;
		boolean_t shared;
		memory_object_name_t object_name;
		vm_offset_t offset;
		
		address = uap->address;
		if ((vm_region(mach_task_self(), &address, &size,
			       &protection, &max_protection, 
			       &inheritance, &shared, &object_name,
			       &offset) != KERN_SUCCESS) ||
		    (address != uap->address) ||
		    (size < uap->size) ||
		    ((protection&VM_PROT_READ) != VM_PROT_READ)) {
			error = EFAULT;
			goto out;
		}
	}

	if (error = copyout((caddr_t)uap->address, (caddr_t)data, uap->size))
		goto out;
#else	/* OSF1_SERVER */
	if (error = copyout((caddr_t)uap->address, (caddr_t)*(uap->data), 
			    uap->size))
		goto out;
#endif	/* OSF1_SERVER */

	dprintf(("kloadcall: *data=0x%x, *data_count=%d(0x%x))\n",
		data, uap->size, uap->size));

	*retval = KERN_SUCCESS;
	error = 0;
out:
	if (error) {
		*retval = KERN_INVALID_ADDRESS;
#ifdef	OSF1_SERVER
		(void)vm_deallocate(p->p_task, data, uap->size);
#else	/* OSF1_SERVER */
		(void)vm_deallocate(current_task()->map, data, uap->size);
#endif	/* OSF1_SERVER */
	}
	return(0);
}

klc_vm_write(p, args, retval)
	struct proc *p;
	void *args;
	register int *retval;
{
	register struct args {
		int		op;
		vm_offset_t	address;
		vm_offset_t	data;
		int		data_count;
	} *uap = (struct args *) args;

	/* 
	 * Should check to make sure that target kernel address
	 * is acessible.  For now, we expect copyin() to catch
	 * not only an inacessible user source address but also
	 * an inacessible target kernel address.
	 */

	dprintf(("kloadcall: kloadcall(KLC_VM_WRITE, address=0x%x, data=0x%x, data_count=%d(0x%x))\n",
		uap->address, uap->data, uap->data_count,
		uap->data_count));

	if ((trunc_page(uap->address) != uap->address)
	    || (trunc_page(uap->data) != uap->data)
	    || (trunc_page(uap->data_count) != uap->data_count)) {
		*retval = KERN_INVALID_ARGUMENT;
		return(0);
	}

	if (copyin((caddr_t)uap->data, (caddr_t)uap->address,
	    uap->data_count)) {
		*retval = KERN_INVALID_ADDRESS;
		return(0);
	}

	*retval = KERN_SUCCESS;
	return(0);
}

klc_vm_protect(p, args, retval)
	struct proc *p;
	void *args;
	register int *retval;
{
	register struct args {
		int		op;
		vm_offset_t	address;
		vm_size_t	size;
		boolean_t	set_maximum;
		vm_prot_t	new_protection;
	} *uap = (struct args *) args;

	dprintf(("kloadcall: kloadcall(KLC_VM_PROTECT, address=0x%x, size=%d(0x%x), set_maximum=%d, new_protection=0x%x)\n",
		uap->address, uap->size, uap->size, uap->set_maximum,
		uap->new_protection));

	if ((trunc_page(uap->address) != uap->address)
	    || (trunc_page(uap->size) != uap->size)) {
		*retval = KERN_INVALID_ARGUMENT;
		return(0);
	}

#ifdef	OSF1_SERVER
	*retval = vm_protect(mach_task_self(), uap->address, uap->size,
			     uap->set_maximum, uap->new_protection);
#else	/* OSF1_SERVER */
	*retval = vm_protect(kernel_map, uap->address, uap->size,
		uap->set_maximum, uap->new_protection);
#endif	/* OSF1_SERVER */
	return(0);
}

klc_vm_allocate_wired(p, args, retval)
	struct proc *p;
	void *args;
	register int *retval;
{
	register struct args {
		int		op;
		vm_offset_t	*address;
		vm_size_t	size;
		vm_prot_t	wire_prot;
		boolean_t	anywhere;
	} *uap = (struct args *) args;
	vm_offset_t	address;
	
	dprintf(("kloadcall: kloadcall(KLC_VM_ALLOCATE_WIRED, address=0x%x, size=%d(0x%x), wire_prot=0x%x, anywhere=%d)\n",
		uap->address, uap->size, uap->size, uap->wire_prot,
		uap->anywhere));

	if (!uap->anywhere) {
		if (copyin((caddr_t)uap->address, (caddr_t)&address,
		    sizeof(address))) {
			*retval = KERN_INVALID_ADDRESS;
			return(0);
		}

		dprintf(("kloadcall: *address=0x%x IN\n", address));

		if (trunc_page(address) != address) {
			*retval = KERN_INVALID_ARGUMENT;
			return(0);
		}
	}

	if (trunc_page(uap->size) != uap->size) {
		*retval = KERN_INVALID_ARGUMENT;
		return(0);
	}

	if (trunc_page(uap->size) != uap->size) {
		*retval = KERN_INVALID_ARGUMENT;
		return(0);
	}

#ifdef	OSF1_SERVER
	if ((*retval = vm_allocate(mach_task_self(), &address,
				   uap->size, uap->anywhere)) != KERN_SUCCESS)
		return 0;
#else	/* OSF1_SERVER */
	if ((*retval = kernel_memory_allocate_wired(kernel_map, &address,
	    uap->size, uap->wire_prot, uap->anywhere)) != KERN_SUCCESS)
		return(0);
#endif	/* OSF1_SERVER */

	/*
	 * Set the default protection to RW so that a later
	 * call to vm_protect() that adds X will cause caches
	 * to be synchronized.  RW will be the default for
	 * vm_allocate() in a future release and at that time
	 * this code should be removed.
	 */
#ifdef	OSF1_SERVER
	if ((*retval = vm_protect(mach_task_self(), address, uap->size, FALSE,
				  (VM_PROT_READ|VM_PROT_WRITE)))
	    != KERN_SUCCESS) {
		vm_deallocate(mach_task_self(), address, uap->size);
		*retval = KERN_INVALID_ADDRESS;
		return 0;
	}
#else	/* OSF1_SERVER */
	if ((*retval = vm_protect(kernel_map, address, uap->size,
	    FALSE, (VM_PROT_READ|VM_PROT_WRITE))) != KERN_SUCCESS) {
		kmem_free(kernel_map, address, uap->size);
		*retval = KERN_INVALID_ADDRESS;
		return(0);
	}
#endif	/* OSF1_SERVER */

	if (copyout((caddr_t)&address, (caddr_t)uap->address,
	    sizeof(address))) {
#ifdef	OSF1_SERVER
		vm_deallocate(mach_task_self(), address, uap->size);
#else	/* OSF1_SERVER */
		kmem_free(kernel_map, address, uap->size);
#endif	/* OSF1_SERVER */
		*retval = KERN_INVALID_ADDRESS;
		return(0);
	}

	dprintf(("kloadcall: *address=0x%x OUT\n", address));

	*retval = KERN_SUCCESS;
	return(0);
}

klc_call_function(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		int		op;
		vm_offset_t	address;
		int		arg1;
		int		arg2;
		int		arg3;
	} *uap = (struct args *) args;
	int (*funcp)();

	dprintf(("kloadcall: kloadcall(KLC_CALL_FUNCTION, address=0x%x, arg1=0x%x, arg2=0x%x, arg3=0x%x)\n",
		uap->address, uap->arg1, uap->arg2, uap->arg3));

	funcp = (int(*)())uap->address;
	*retval = (*funcp)(uap->arg1, uap->arg2, uap->arg3);
	return(0);
}
