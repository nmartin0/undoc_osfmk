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
 * $Log: ipi_map.h,v $
 * Revision 1.2  1994/11/18  20:51:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/08  16:54:33  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_map.h
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Mapping between Unix-like indexing and slave+facility+partition
 *	Each slave handles at most 16 facilities.
 */

#ifndef	_IPI_MAP_H_
#define	_IPI_MAP_H_


/*
 *            The shift and mask values are currently defined
 *            such that the dev number is viewed as follows:
 *
 *             11 10         8 7          4 3          0
 *            +--+------------+------------+------------+
 *            |  |   slave    |  facility  |  partition |
 *            +--+------------+------------+------------+
 *            |..|<- 3 bits ->|<- 4 bits ->|<- 4 bits ->|
 *             ||      8            16           16
 *             ||    slaves     facilities   partitions
 *	       ||
 *    The controller field (bit 11) is defined but currently not used
 *    since we only really support one HiPPI controller per node.
 */

#define IPI_CONTROLLER_SHIFT	11
#define IPI_SLAVE_SHIFT		8
#define	IPI_FACILITY_SHIFT	4
#define IPI_PARTITION_SHIFT	0

#define IPI_CONTROLLER_MASK	0x1
#define	IPI_SLAVE_MASK		0x7
#define	IPI_FACILITY_MASK	0xf
#define IPI_PARTITION_MASK	0xf

#define DEV_MASK		(		        \
	(IPI_CONTROLLER_MASK << IPI_CONTROLLER_SHIFT) | \
	(IPI_SLAVE_MASK	     << IPI_SLAVE_SHIFT)      | \
	(IPI_FACILITY_MASK   << IPI_FACILITY_SHIFT)   | \
	(IPI_PARTITION_MASK  << IPI_PARTITION_SHIFT)    \
				)

#define	IPI_CONTROLLER(dev)	\
	(((dev) >> IPI_CONTROLLER_SHIFT) & IPI_CONTROLLER_MASK)

#define	IPI_SLAVE(dev)		\
	(((dev) >> IPI_SLAVE_SHIFT) & IPI_SLAVE_MASK)

#define	IPI_FACILITY(dev)	\
	((((dev) >> IPI_FACILITY_SHIFT) & IPI_FACILITY_MASK) + tgt->facility)

#define	IPI_PARTITION(dev)	\
	(((dev) >> IPI_PARTITION_SHIFT) & IPI_PARTITION_MASK)

#endif	_IPI_MAP_H_
