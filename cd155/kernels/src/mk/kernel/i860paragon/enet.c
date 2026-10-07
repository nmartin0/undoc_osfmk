/*
 * 
 * $Copyright
 * Copyright 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1995 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: enet.c,v $
 * Revision 1.3  1995/03/28  01:33:52  jerrie
 * Added performance counters to SCSI-16 ethernet dev_ops to be fully
 * backward compatible with the MIO ethernet driver.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low.
 *  Benefit or PTS #: 12827
 *  Testing:	   Developer
 *  Module(s):	   See PTS report.
 *
 * Revision 1.2  1995/03/14  23:42:55  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	enet.c
 *	Author:	Jerrie Coffman
 *		Intel Corporation Scalable Systems Division
 *	Date:	3/95
 *
 *	Daughter board ethernet driver interface
 *
 *	Provides compatibility between ethernet driver interfaces.
 *	The driver probe, slave, and attach functions happen normally
 *	during auto-config.  The kernel dev_name_list dev_ops structure
 *	contains the entry points to this module, which forwards the
 *	operation to the proper ethernet device driver.  The ethernet
 *	device driver is determined at open time.  This allows a common
 *	ethernet device driver name to be used for all daughter board
 *	interfaces in the system.  For backward compatibility, the MIO
 *	ethernet driver name of "em" will be used.
 */

#include <sdb.h>
#include <mioe.h>
#include <sdbe.h>
#if	NMIOE > 0 || (NSDB > 0 && NSDBE > 0)

#include <device/errno.h>
#include <device/io_req.h>
#include <device/net_status.h>
#include <i860paragon/expansion.h>

#if	NMIOE > 0
/*
 * MIO daughter board ethernet device
 */
#define	mioename	"em"

extern int	mioe_open(),
		mioe_output(),
		mioe_getstat(),
		mioe_setstat(),
		mioe_setinput(),
		mioe_mmap();

struct dev_ops	mioe_dev_ops =
	{ mioename,	mioe_open,	nulldev,	nulldev,
	  mioe_output,	mioe_getstat,	mioe_setstat,	mioe_mmap,
	  mioe_setinput,nulldev,	nulldev,	0,
	  nodev };
#endif	NMIOE > 0

#if	NSDB > 0 && NSDBE > 0
/*
 * SCSI-16 daughter board ethernet device
 * and
 * SCSI-16 daughter board performance counters
 */
#define sdbename	"es"

extern int	sdbe_open(),
		sdbe_close(),
		sdbe_output(),
		sdbe_getstat(),
		sdbe_setstat(),
		sdbe_setinput();
extern int	sdb_mmap();

struct dev_ops	sdbe_dev_ops =
	{ sdbename,	sdbe_open,	sdbe_close,	nulldev,
	  sdbe_output,	sdbe_getstat,	sdbe_setstat,	sdb_mmap,
	  sdbe_setinput,nulldev,	nulldev,	0,
	  nodev };
#endif	NSDB > 0 && NSDBE > 0

/*
 * Ethernet dev_ops pointer
 */
struct dev_ops	*enet_dev_ops;

enet_open(dev, mode, ior)
	int		dev;
	int		mode;
	io_req_t	ior;
{
	extern int	expansion_id();

	/*
	 * Determine the daughter board type and set the
	 * ethernet dev_ops pointer to the board enet driver
	 */
	switch (expansion_id()) {

		case EXP_ID_MIO:
			enet_dev_ops = &mioe_dev_ops;
			break;

		case EXP_ID_SCSI:
			enet_dev_ops = &sdbe_dev_ops;
			break;

		default:
			return ENXIO;
	}

	return (*enet_dev_ops->d_open)(dev, mode, ior);
}

enet_close(dev)
	int		dev;
{
	assert(enet_dev_ops);
	return (*enet_dev_ops->d_close)(dev);
}

enet_output(dev, ior)
	int		dev;
	io_req_t	ior;
{
	assert(enet_dev_ops);
	return (*enet_dev_ops->d_write)(dev, ior);
}

enet_getstat(dev, flavor, status, status_count)
	int		dev;
	int		flavor;
	dev_status_t	status;
	unsigned int	*status_count;
{
	assert(enet_dev_ops);
	return (*enet_dev_ops->d_getstat)(dev, flavor, status, status_count);
}

enet_setstat(dev, flavor, status, status_count)
	int		dev;
	int		flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
	assert(enet_dev_ops);
	return (*enet_dev_ops->d_setstat)(dev, flavor, status, status_count);
}

enet_mmap(dev, offset, prot)
	int		dev;
	vm_offset_t	offset;
	int		prot;
{
	assert(enet_dev_ops);
	return (*enet_dev_ops->d_mmap)(dev, offset, prot);
}

enet_setinput(dev, receive_port, priority, filter, filter_count)
	int		dev;
	ipc_port_t	receive_port;
	int		priority;
	filter_t	filter[];
	unsigned int	filter_count;
{
	assert(enet_dev_ops);
	return (*enet_dev_ops->d_async_in)(dev, receive_port,
					priority, filter, filter_count);
}

#endif	NMIOE > 0 || (NSDB > 0 && NSDBE > 0)
