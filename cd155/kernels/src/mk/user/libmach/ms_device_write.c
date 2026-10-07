/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1992 by Open Software Foundation,
 * Cambridge, Massachusetts USA
 *
 * 		All Rights Reserved
 * 
 *   Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OSF or Open Software
 * Foundation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 *   OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * HISTORY
 * $Log: ms_device_write.c,v $
 * Revision 1.5  1994/11/18  21:05:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:03:56  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:58:13  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.8.2  1993/04/22  19:04:10  dleslie
 * First R1_0 release
 *
 * 
 */

#include <mach/mach.h>
#include <mach/message.h>
#include <device/device.h>

int	device_write_traps = 1;

kern_return_t device_write(device, mode, recnum, data, data_count, bytes_written)
	mach_port_t		device;
	dev_mode_t		mode;
	recnum_t		recnum;
	io_buf_ptr_t		data;
	mach_msg_type_number_t	data_count;
	int			*bytes_written;
{
	kern_return_t result;

	if (device_write_traps) {
		result = syscall_device_write(device, mode, recnum, data, data_count, bytes_written);
		if (result == MACH_SEND_INTERRUPTED)
			result = mig_device_write(device, mode, recnum, data, data_count, bytes_written);
	} else {
			result = mig_device_write(device, mode, recnum, data, data_count, bytes_written);
	}
	return(result);
}
