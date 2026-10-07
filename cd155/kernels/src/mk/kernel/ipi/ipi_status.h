/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1993 by Intel Corporation,
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
 * $Log: ipi_status.h,v $
 * Revision 1.2  1994/11/18  20:51:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/08  16:54:35  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_status.h
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	IPI device driver specific ioctl commands
 */

#ifndef	_IPI_STATUS_H_
#define	_IPI_STATUS_H_


/*
 * Structure used to get physical information
 */
struct ipi_phys {
	unsigned long		node;		/* node number 	     */
	unsigned char		controller;	/* controller number */
	unsigned char		slave;		/* slave address     */
	unsigned char		facility;	/* facility address  */
};

/*
 * Structure used to get/set min and max command reference numbers
 */
struct ipi_cmd_ref {
	unsigned short		min;		/* minimum command reference */
	unsigned short		max;		/* maximum command reference */
};

/*
 * Structures used to get/set the IPI driver statistics
 */
struct ipi_rw_stats {
	unsigned int		requests;	/* number of I/O requests  */
	unsigned int		xfer_count;	/* number of bytes/blocks  */
	unsigned int		driver_time;	/* driver service time	   */
	unsigned int		device_time;	/* device service time	   */
};

struct ipi_stats {
	struct ipi_rw_stats	read;		/* read statistics	   */
	struct ipi_rw_stats	write;		/* write statistics	   */
	unsigned int		queue_max;	/* maximum queue depth	   */
	unsigned int		chain_max;	/* maximum chain depth	   */
	unsigned int		queue_depth;	/* current queue depth	   */
	unsigned int		chain_depth;	/* current chain depth	   */
	unsigned int		driver_full;	/* driver queue full count */
	unsigned int		device_full;	/* device queue full count */
	unsigned int		retries;	/* driver command retries  */
};

/*
 * IPI device driver ioctl commands
 */
#define IPIPHYS		_IOR('i', 100, struct ipi_phys)		/* get */
#define IPIGIFIELD	_IOR('i', 101, unsigned int)		/* get */
#define IPISIFIELD	_IOW('i', 102, unsigned int)		/* set */
#define IPIGFACILITY	_IOR('i', 103, unsigned int)		/* get */
#define IPISFACILITY	_IOW('i', 104, unsigned int)		/* set */
#define IPIGPART	_IOR('i', 105, unsigned int)		/* get */
#define IPISPART	_IOW('i', 106, unsigned int)		/* set */
#define IPIGCMDREF	_IOR('i', 107, struct ipi_cmd_ref)	/* get */
#define IPISCMDREF	_IOW('i', 108, struct ipi_cmd_ref)	/* set */
#define IPIGSTATS	_IOR('i', 109, struct ipi_stats)	/* get */
#define IPISSTATS	_IOW('i', 110, struct ipi_stats)	/* set */

/*
 * NSL ioctl commands
 */
struct nsl_xfer {
	int		dir;	/* direction of transfer (read/write)	 */
#define		NSL_READ		0x00
#define		NSL_WRITE		0x01
	int		len;	/* length of transfer in bytes		 */
	unsigned char	*buf;	/* address of user data buffer		 */
	unsigned int	tid;	/* transfer ID assigned to the operation */
};

#define NSLXFER		_IOWR('i', 111, struct nsl_xfer)	/* set/get */
#define NSLCOMPLETE	_IOR('i',  112, unsigned int)		/* get */
#define NSLCANCEL	_IOW('i',  113, unsigned int)		/* set */

#endif	_IPI_STATUS_H_
