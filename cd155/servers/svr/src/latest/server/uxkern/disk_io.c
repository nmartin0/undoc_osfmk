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
 * Mach Operating System
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * $Id: disk_io.c,v 1.14 1995/01/17 20:29:48 lenb Exp $
 */
/*
 * Routines for char IO to block devices.
 */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <sys/errno.h>

#include <uxkern/device_utils.h>

mach_port_t node_to_master_device_port();

int
disk_open(dev, mod, flag, flgp, node)
	dev_t			dev;
	int			flag;
	int			mod;
	int			*flgp;
	int			node;
{
	char			name[32];
	kern_return_t		rc;
	devinfo_t		*devinfo;
	mach_port_t		devport;
	int			devstat[DEV_GET_SIZE_COUNT];
	int			mode, i, devstat_count;

	rc = cdev_name_string(dev, name);
	if (rc != 0)
		return (rc);	/* bad name */

	/* fix modes */
	mode = 0;	/* XXX */
	rc = device_open(node_to_master_device_port(node),
			 mode,
			 name,
			 &devport);
	if (rc != D_SUCCESS)
		return (dev_error_to_errno(rc));

	/*
	 * See whether we had the device open already.
	 */
	if (dev_lookup(dev, node, CHAR_DEV)) {
		(void)device_close(devport);	/* match extra open w/ close */
		return (0);
	}

	devinfo = (devinfo_t *) malloc(sizeof(devinfo_t));
	devinfo->devport = devport;
        devinfo->return_short_reads = FALSE;

	devstat_count = DEV_GET_SIZE_COUNT;
	rc = device_get_status(devport,
			       DEV_GET_SIZE,
			       (int *)&devstat,
			       &devstat_count);
	if (rc != D_SUCCESS)
		panic("disk_open.device_get_status failure 0x%x\n", rc);

	devinfo->mrecsize = devstat[DEV_GET_SIZE_RECORD_SIZE];  
	devinfo->mrecmask = ~(devinfo->mrecsize - 1);
	for (devinfo->mrecshift = 0, i = devinfo->mrecsize; i > 1; i >>= 1)
		devinfo->mrecshift++;

	dev_enter(dev, node, CHAR_DEV, (char *) devinfo);

	return (0);
}

int
disk_close(dev, node, flag)
	dev_t	dev;
        node_t  node;
	int	flag;
{
	devinfo_t	*devinfo;
	int		error;

	devinfo = (devinfo_t *) dev_lookup(dev, node, CHAR_DEV);
	if (devinfo == NULL)
	    return (0);		/* should not happen */

	dev_remove(dev, node, CHAR_DEV);
	error = dev_error_to_errno(device_close(devinfo->devport));
	(void) mach_port_deallocate(mach_task_self(), devinfo->devport);
	free((char *) devinfo);
	return (error);
}

int
disk_read(dev, node, uio)
	dev_t		dev;
        node_t          node;
	struct uio	*uio;
{
	register struct iovec *iov;
	register int	c;
	register kern_return_t	rc;
	devinfo_t	*devinfo;
	io_buf_ptr_t	data;
	unsigned int	count;

	devinfo = (devinfo_t *) dev_lookup(dev, node, CHAR_DEV);
	if (devinfo == NULL)
	    return (EIO);		/* should not happen */

	while (uio->uio_iovcnt > 0) {

	    iov = uio->uio_iov;
	    if (useracc(iov->iov_base, (u_int)iov->iov_len, 0) == 0)
		return (EFAULT);

	    /*
	     * Can read entire block here - device handler
	     * breaks into smaller pieces.
	     */

	    do {
                    c = iov->iov_len;

		    /* some disk drivers can't read that much ... */
		    rc = device_read(devinfo->devport,
				     0,	/* mode */
				     uio->uio_offset >> devinfo->mrecshift,
				     iov->iov_len,
				     &data,
				     &count);
		    if (rc != 0)
			return (dev_error_to_errno(rc));

		    (void) moveout(data, iov->iov_base, count);
				/* deallocates data (eventually) */

		    iov->iov_base += count;
		    iov->iov_len -= count;
		    uio->uio_resid -= count;
		    uio->uio_offset += count;
                    /* 
                     * for tape drives - don't want to continue beyond
                     * tape marks 
                     */
                    if ((count < c) && (devinfo->return_short_reads))
                            return(0);

            } while (iov->iov_len > 0 && count != 0);

	    uio->uio_iov++;
	    uio->uio_iovcnt--;
	}
	return (0);
}

/* Controls whether we pass user memory directly down into the kernel,
 * or allocate new memory and copy it in.  The former is much more
 * efficient, but we need to be sure that the user memory is in the
 * server address space, which it may not be in the no-emulator archi-
 * tecture.
 */
int disk_alloc_copy = TRUE;

int
disk_write(dev, node, uio)
	dev_t		dev;
        node_t          node;
	struct uio	*uio;
{
	register struct iovec *iov;
	register int	c;
	register kern_return_t	rc;
	vm_offset_t	kern_addr;
	vm_size_t	kern_size;
	vm_size_t	count;
	devinfo_t	*devinfo;

	devinfo = (devinfo_t *) dev_lookup(dev, node, CHAR_DEV);
	if (devinfo == NULL)
	    return (EIO);		/* should not happen */

	while (uio->uio_iovcnt > 0) {
	    iov = uio->uio_iov;

	    kern_size = iov->iov_len;

	    if (disk_alloc_copy) {
		(void) vm_allocate(mach_task_self(),
				   &kern_addr, kern_size, TRUE);
		if (copyin(iov->iov_base, kern_addr, (u_int)iov->iov_len)) {
		    (void) vm_deallocate(mach_task_self(),
					 kern_addr, kern_size);
		    return (EFAULT);
		}
	    } else
		kern_addr = (vm_offset_t)iov->iov_base;

	    /*
	     * Can write entire block here - device handler
	     * breaks into smaller pieces.
	     */

	    c = iov->iov_len;

	    rc = device_write(devinfo->devport,
			      0,	/* mode */
			      uio->uio_offset >> devinfo->mrecshift,
			      kern_addr,
			      iov->iov_len,
			      &count);

	    if (disk_alloc_copy)
		(void) vm_deallocate(mach_task_self(), kern_addr, kern_size);

	    if (rc != 0)
		return (dev_error_to_errno(rc));

	    iov->iov_base += count;
	    iov->iov_len -= count;
	    uio->uio_resid -= count;
	    uio->uio_offset += count;

	    /* temp kludge for tape drives */
	    if (count < c)
		break;

	    uio->uio_iov++;
	    uio->uio_iovcnt--;
	}
	return (0);
}

/*
 * To guarantee that device_get_status() returns the stuff from the
 * correct device_set_status(), we must lock them together.
 * We come through this lock for ioctl's on all scsi bus targets,
 * including tape devices.
 *
 * This would not be necessary if the interface were specified such
 * that device_set_status() returned its status and did not require
 * a device_get_status().
 */
simple_lock_data_t disk_ioctl_lock = { FALSE };

disk_ioctl(dev, node, cmd, data, flag)
	dev_t	dev;
        node_t  node;
	int	cmd;
	caddr_t	data;
	int	flag;
{
	devinfo_t	*devinfo;
	unsigned int	count;
	register int	error;

	devinfo = (devinfo_t *) dev_lookup(dev, node, CHAR_DEV);
	if (devinfo == NULL)
		return(EIO);

	count = (cmd & ~(IOC_INOUT|IOC_VOID)) >> 16; /* bytes */
	count = (count + 3) >> 2;		     /* ints */
	if (count == 0)
	    count = 1;

	simple_lock(&disk_ioctl_lock);

	if (cmd & (IOC_VOID|IOC_IN)) {
	    error = device_set_status(devinfo->devport,
				      cmd,
				      (int *)data,
				      count);
	    if (error)
		return (dev_error_to_errno(error));
	}
	if (cmd & IOC_OUT) {
	    error = device_get_status(devinfo->devport,
				      cmd,
				      (int *)data,
				      &count);
	}

	simple_unlock(&disk_ioctl_lock);

	if (error)
	     return (dev_error_to_errno(error));
	else
	     return (0);
}

#if i386
mach_port_t
disk_port(dev, node)
	dev_t	dev;
        node_t  node;
{
	devinfo_t	*devinfo;

	devinfo = (devinfo_t *) dev_lookup(dev, node, CHAR_DEV);
	if (devinfo == NULL)
		return(MACH_PORT_NULL);
	else
		return (devinfo->devport);
}
#endif
