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
 * $Log: raw_hippi.c,v $
 * Revision 1.6  1995/03/31  23:49:16  hobbes
 *  Reviewer: Arlin Davis, Bob Yasi
 *  Risk: Low
 *  Benefit or PTS #: 12720
 *  Testing: code which demonstrates the problem.
 *  Module(s): cmds_libs/src/usr/ccs/lib/libhippi/hippi_open.c
 * 	    server/uxkern/hippi_open.c
 *
 * Revision 1.5  1994/12/02  21:21:02  yazz
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 11518 C-0
 *  Testing: Specific Testcase in PTS report.  Controlc EAT.
 *  Module(s):
 * 	server/i860/ipi_ops.c
 * 	server/uxkern/block_io.c
 * 	server/uxkern/device_misc.c
 * 	server/uxkern/disk_io.c
 * 	server/uxkern/raw_hippi.c
 * Make each user process open() of a device result in a microkernel
 * device_open() call, so MK device drivers can enforce 1-at-a-time usage.
 * Ensure that the number of MK device_open() & device_close() calls match.
 *
 * Revision 1.4  1994/11/18  20:49:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/01/12  17:46:07  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.2  1993/07/27  01:23:22  hobbes
 * Added the node parameter to all dev_* routines.
 *
 * Revision 1.1  1993/05/27  21:50:19  hobbes
 * Initial support for character device for hippi ... used by libhippi.
 *
 */
/*
 * Routines for char IO to HiPPI device.
 */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/uio.h>
#include <sys/fcntl.h>
#include <uxkern/bsd_msg.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <i860paragon/hippi/hippi_status.h>

#include <uxkern/device_utils.h>

mach_port_t node_to_master_device_port();

/*
 *  Open the raw HiPPI device
 */
int
hippi_open(dev, flag, mod, flgp, node)
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
	int			mode;
	int			status = TRUE;

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
	 * Set status if appropriate
	 */ 
	if ( flag & FEXCL ) {
		if ((rc = device_set_status( devport,
					     HIPPI_DEV_EXCL,
					     &status,
					     1 )) != D_SUCCESS) {
			(void)device_close(devport);
			/* we want the user to get a meaningful error */
			rc = D_ALREADY_OPEN;
			return (dev_error_to_errno(rc));
		}
	}

	/*
	 * See whether we had the device open already.
	 */
	if (dev_lookup(dev, node, CHAR_DEV)) {
		(void)device_close(devport);	/* match extra open w/ close */
		return (0);
	}

	devinfo = (devinfo_t *) malloc(sizeof(devinfo_t));
	devinfo->devport = devport;
	dev_enter(dev, node, CHAR_DEV, (char *) devinfo);

	return (0);
}

/*
 *  Close the raw HiPPI device
 */
int
hippi_close(dev, node, flag)
	dev_t	dev;
	node_t	node;
	int		flag;
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

/*
 *  Read the raw HiPPI device
 */
int
hippi_read(dev, uio)
	dev_t		dev;
	struct uio	*uio;
{
	return (EIO);		/* We don't do reads */
}

/*
 *  Write the raw HiPPI device
 */
hippi_write(dev, uio)
	dev_t		dev;
	struct uio	*uio;
{
	return (EIO);		/* We don't do writes */
}

/*
 *  Do raw HiPPI ioctl stuff
 */
mach_msg_type_t dev_port_type = {
	MACH_MSG_TYPE_COPY_SEND,	/* msgt_name */
	sizeof(mach_port_t) * 8,	/* msgt_size */
	1,				/* msgt_number */
	TRUE,				/* msgt_inline */
	FALSE,				/* msgt_longform */
	FALSE,				/* msgt_deallocate */
	0				/* msgt_unused */
};

int
hippi_c_ioctl(dev, node, cmd, data, flag)
	dev_t	dev;
	node_t	node;
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

	switch(cmd) {
		case FIODEVPORT: {
			struct dev_port {
				mach_msg_type_t	type;
				mach_port_t	port;
			} *rep;
				
			mach_msg_header_t	*hdr;
			struct uthread		*uth = &u;

			hdr = uth->uu_reply_msg;
			rep = (struct dev_port*) ((char*)hdr + hdr->msgh_size);

			/*
			 *  Set the data
			 */
			rep->type = dev_port_type;	/* structure copy */ 
			rep->port = devinfo->devport;	/* set the port  */

			/*
			 *  Fix the reply header.
			 */
			hdr->msgh_bits |= MACH_MSGH_BITS_COMPLEX;
			hdr->msgh_size += (sizeof(struct dev_port));

			error = KERN_SUCCESS;
			break;
		}
		
		/*
		 *  Not one that we handle, try sending to the
		 *  device and see what happens.
		 */
		default:
			count = (cmd & ~(IOC_INOUT|IOC_VOID)) >> 16; /* bytes */
			count = (count + 3) >> 2;                    /* ints */
			if (count == 0)
				count = 1;

			if (cmd & (IOC_VOID|IOC_IN)) {
				if (error = device_set_status(devinfo->devport,
							  cmd,
							  (int *)data,
							  count))
					break;
			}
			if (cmd & IOC_OUT) {
				if (error = device_get_status(devinfo->devport,
							  cmd,
							  (int *)data,
							  &count));
					break;
			}
	}
	if (error)
		error = EINVAL;

	return (error);
}
