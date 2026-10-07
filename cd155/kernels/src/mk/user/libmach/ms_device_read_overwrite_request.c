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
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: ms_device_read_overwrite_request.c,v $
 * Revision 1.3  1994/11/18  21:05:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/06/30  23:03:52  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1993/06/18  19:20:50  andyp
 * Initial revision
 *
 * Revision 1.1.2.3  1993/05/13  17:06:21  rod
 * 	Fix declaration of io_buf_ptr_t.
 * 	[1993/05/13  17:04:59  rod]
 *
 * Revision 1.1.2.2  1993/04/22  19:39:39  rod
 * 	device_read_overwrite support:  Created.
 * 	[1993/03/12  10:35:50  rod]
 * 
 * $EndLog$
 */

#include <mach/mach.h>
#include <mach/message.h>
#include <device/device.h>
#include <device/device_request.h>

kern_return_t device_read_overwrite_request(device, reply_port, mode, recnum, 
					    bytes_wanted, data)
	mach_port_t		device;
	mach_port_t		reply_port;
	dev_mode_t		mode;
	recnum_t		recnum;
	int			bytes_wanted;
	vm_address_t		data;
{
	kern_return_t result;

	result = syscall_device_read_overwrite_request(device, reply_port, 
						       mode, recnum, 
						       bytes_wanted, data);
	if (result == MACH_SEND_INTERRUPTED) {
		io_buf_ptr_t		buf;
		mach_msg_type_number_t	count;
		
		result = mig_device_read(device, mode, recnum, bytes_wanted, 
					 &buf, &count);
		if (result == KERN_SUCCESS) {
			bcopy((char *) buf, (char *) data, count);
			(void) vm_deallocate(mach_task_self(), 
					     (vm_offset_t) buf, count);
		}
		result = ds_mig_device_read_reply_overwrite(reply_port,
					MACH_MSG_TYPE_MAKE_SEND_ONCE, 
					result, (int) count);
	}
	return(result);
}
