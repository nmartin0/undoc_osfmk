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
 * $Log: ipi_optical.c,v $
 * Revision 1.2  1994/11/18  20:51:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/06/08  16:54:01  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_optical.c
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Middle layer of the IPI-3 driver
 *	Driver operations specific to IPI optical disk devices
 */

#include <ipi.h>
#if	NIPI > 0

#include <ipi/ipi_compat.h>
#include <ipi/ipi_endian.h>
#include <ipi/ipi_defs.h>


char *
ipi_optical_name(internal)
	boolean_t	internal;
{
	return internal ? "iod" : "optical";
}

/*
 * Specialized side of the open routine for optical disks
 */
ipi_ret_t
ipi_optical_open(tgt, req)
	target_info_t		*tgt;
	io_req_t		req;
{
	return D_NO_SUCH_DEVICE;
}

/*
 * Specialized side of the close routine for optical disks
 */
ipi_ret_t
ipi_optical_close(dev, tgt)
	int			dev;
	target_info_t		*tgt;
{
	return D_NO_SUCH_DEVICE;
}

/*
 * Optical disk device optimization routine
 */
ipi_ret_t
ipi_optical_optimize(tgt)
	target_info_t		*tgt;
{
	return IPI_RET_ERROR;
}

/*
 * Optical disk strategy routine
 */
ipi_optical_strategy(ior)
	register io_req_t	ior;
{
	ior->io_error = D_NO_SUCH_DEVICE;
	ior->io_op |= IO_ERROR;
	ior->io_residual = ior->io_count;
	iodone(ior);
}

/*
 * Optical disk start/completion routine
 */
ipi_optical_start(tgt, cmd_q, done)
	target_info_t	*tgt;
	cmd_queue_t	*cmd_q;
	boolean_t	done;
{
}


#include <sys/ioctl.h>

io_return_t
ipi_optical_get_status(dev, tgt, flavor, status, status_count)
	int		dev;
	target_info_t	*tgt;
	unsigned int	flavor;
	dev_status_t	status;
	unsigned int	*status_count;
{
	return D_NO_SUCH_DEVICE;
}

io_return_t
ipi_optical_set_status(dev, tgt, flavor, status, status_count)
	int		dev;
	target_info_t	*tgt;
	unsigned int	flavor;
	dev_status_t	status;
	unsigned int	status_count;
{
	return D_NO_SUCH_DEVICE;
}

#endif	NIPI > 0
