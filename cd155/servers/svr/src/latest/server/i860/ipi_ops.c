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
 * $Log: ipi_ops.c,v $
 * Revision 1.4  1995/02/02  21:50:55  jerrie
 * Macros used in the IPI device driver require the number of sub-devices
 * to be set to 16.  The kernel dev_ops structure master device entry had
 * the number of sub-devices incorrectly set to 0.  In addition, the server
 * character device name string function was not checking for the number of
 * sub-devices in the cdevsw table and was incorrectly using the number of
 * sub-devices from the bdevsw table when building the character device name.
 *
 *  Reviewer:         Arlin Davis
 *  Risk:             Low.  Only affects the IPI device driver.
 *  Benefit or PTS #: 12064
 *  Testing:          Ran the EAT test mention in the PTS bug report.
 *                    Developed a test program to manually exercise the
 *                    interface.
 *  Module(s):        The following files are being modified:
 *
 *                    kernel/i860paragon:     conf.c
 *                    server/i860:            ipi_ops.c
 *
 * Revision 1.3  1994/12/02  21:17:43  yazz
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
 * Revision 1.2  1994/11/18  20:31:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/28  23:03:06  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 */
/*
 * Routines to open block and char IPI devices.
 */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/uio.h>
#include <sys/conf.h>
#include <uxkern/bsd_msg.h>
#include <sys/ioctl.h>
#include <sys/errno.h>

#include <uxkern/device_utils.h>

mach_port_t node_to_master_device_port();

/*
 * Open a block IPI device.
 */
int
bipi_open(dev, mode, flag, node)
	dev_t			dev;
	int			mode;
	int			flag;
	int			node;
{
	char			name[32];
	kern_return_t		rc;
	devinfo_t		*devinfo;
	mach_port_t		devport;
	int			devstat[DEV_GET_SIZE_COUNT];
	int			i, devstat_count;

	rc = bipi_name_string(dev, name);
	if (rc != 0)
		return (rc);

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
	if (dev_lookup(dev, node, BLOCK_DEV)) {
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
		panic("bipi_open.device_get_status failure 0x%x\n", rc);

	devinfo->mrecsize = devstat[DEV_GET_SIZE_RECORD_SIZE];  
	devinfo->mrecmask = ~(devinfo->mrecsize - 1);
	for (devinfo->mrecshift = 0, i = devinfo->mrecsize; i > 1; i >>= 1)
		devinfo->mrecshift++;

	dev_enter(dev, node, BLOCK_DEV, (char *) devinfo);

	return (0);
}

int
bipi_name_string(dev, str)
	dev_t	dev;
	char	str[];	/* REF OUT */
{
	extern	bipi_base;
	int	major_num = major(dev);
	int	minor_num = minor(dev);
	char	num[6];
	int	part_count;
	int	ipi_dev;
	int	facility;
	int	slave;

	if (major_num < 0 || major_num >= nblkdev)
	    return (ENXIO);

	strcpy(str, bdevsw[major_num].d_name);
	part_count = C_BLOCK_GET(bdevsw[major_num].d_flags);

	if (part_count <= 0) {
	    return (ENXIO);
	}

	facility = (minor_num >> 4) & 0xf;
	slave = (major_num - bipi_base) & 7;
	ipi_dev = (slave << 4) | facility;

	(void)itoa(ipi_dev, num);
	strcat(str, num);

	num[0] = 'a' + (minor_num % part_count);
	num[1] = '\0';
	strcat(str, num);
	return (0);
}

/*
 * Open a character IPI device.
 */
int
cipi_open(dev, mod, flag, flgp, node)
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

	rc = cipi_name_string(dev, name);
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
		panic("cipi_open.device_get_status failure 0x%x\n", rc);

	devinfo->mrecsize = devstat[DEV_GET_SIZE_RECORD_SIZE];  
	devinfo->mrecmask = ~(devinfo->mrecsize - 1);
	for (devinfo->mrecshift = 0, i = devinfo->mrecsize; i > 1; i >>= 1)
		devinfo->mrecshift++;

	dev_enter(dev, node, CHAR_DEV, (char *) devinfo);

	return (0);
}

int
cipi_name_string(dev, str)
	dev_t	dev;
	char	str[];	/* REF OUT */
{
	extern	cipi_base;
	int	major_num = major(dev);
	int	minor_num = minor(dev);
	char	num[6];
	int	ipi_dev;
	int	facility;
	int	slave;

	if (major_num < 0 || major_num >= nchrdev)
	    return (ENXIO);

	strcpy(str, cdevsw[major_num].d_name);

	facility = (minor_num >> 4) & 0xf;
	slave = (major_num - cipi_base) & 7;
	ipi_dev = (slave << 4) | facility;
	(void)itoa(ipi_dev, num);
	strcat(str, num);

	if (cdevsw[major_num].d_flags & C_BLOCK(0)) {
		/*
		 * Append the partition
		 */
		int	part_count;

		part_count = C_BLOCK_GET(cdevsw[major_num].d_flags);
		num[0] = 'a' + (minor_num % part_count);
		num[1] = '\0';
		strcat(str, num);
	}

	return (0);
}
