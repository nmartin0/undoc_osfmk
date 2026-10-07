/*
 * 
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
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
 * HISTORY:
 * $Log: kmsg_parser.c,v $
 * Revision 1.3  1994/11/18  20:58:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  21:30:13  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.12  1994/07/11  16:50:28  rkl
 *  New and improved logging.
 *
 * Revision 1.1.2.11  1994/05/05  16:35:46  stans
 *   Added include of ipc/ipc_port.h for ipc_port_t typedef.
 *
 * Revision 1.1.2.10  1994/03/30  18:58:28  rkl
 *  Made logging all functions default.  Was set so that no function would log.
 *
 * Revision 1.1.2.9  1994/03/15  02:31:01  rkl
 *  Turned off logging function entries.
 *
 * Revision 1.1.2.8  1994/03/15  00:33:34  rkl
 *  Made changes to support new norma logging capability.
 *
 * Revision 1.1.2.7  1994/02/24  00:25:38  rkl
 *  Resolved problems (and bugs) reported by lint(1).
 *
 * Revision 1.1.2.6  1994/02/03  17:26:52  rkl
 *  Check point after resolving all test environment issues
 *
 * Revision 1.1.2.5  1994/02/02  16:50:19  rkl
 *  Added some debug logging.
 *
 * Revision 1.1.2.4  1994/02/01  00:59:34  rkl
 *  Added common logging macros
 *
 * Revision 1.1.2.3  1994/01/27  21:09:27  stans
 *   Data address passed to type descriptor functions is now a pointer back to
 *   the message body. Help for OOL address adjustments.
 *
 * Revision 1.1.2.2  1994/01/26  21:58:01  rkl
 *  Fixed problem were `data' was set and added an assert.
 *
 * Revision 1.1.2.1  1994/01/26  01:44:05  stans
 *   Added comments on how the parser calls the data type functions.
 *   Convert some hard-constants to be expressed in terms of sizeof() for
 *   future portability (64-bit issues).
 *
 * Revision 1.1  1994/01/25  19:41:48  rkl
 *  Initial checkin.
 *
 *
 */

#include <mach_assert.h>

#include <mach/message.h>
#include <mach/machine/vm_param.h>
#include <vm/vm_kern.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <norma2/kmsg_parser.h>
#include <norma2/norma_log.h>

/*
 *  Name:	norma_parse_kmsg
 *
 *  Input:	In/Out pointer to maximum number of types to process, pointer
 *		to first mach_msg_type_t to process, in/out pointer to bytes
 *		remaining in message body, point to parsing function table,
 *		out pointer to last parsing function return value, and opaque
 *		argument to pass to each parsing funciton.
 *
 *  Output:	Values pointed to by count and size reflect the number
 *		remaining from initial call and the value pointed to by
 *		return_val reflects the return code of the last parsing
 *		function called.
 *
 *  Returns:		Pointer to next mach_msg_t to process.
 *
 *  MP and locking
 *  consideration:	None
 *
 *  Description:
 *	This is a generic Mach message body parsing function.
 *	Given a pointer to a starting mach_msg_type_t, the
 *	number of bytes in the message body remaining to parse,
 *	a maximum number of types to process, and a pointer
 *	to a table of function pointers, this routine will
 *	handle the details of short and long message types,
 *	type sizes and alignment, and call processing functions
 *	for in-line and out-of-line ports and data.
 *
 *	It will parse types until either
 *		1) size bytes have been processed
 *		2) count types have been processed by functions specified in
 *		   the parsing function table
 *		3) a parsing function returns something other than KERN_SUCCESS.
 *
 *	Each type specific function is called with the following arguments
 *
 * 	*(func)( type, length, ptr, opaque )
 *
 *	 inputs:
 *		type	Mach type descriptor pointer
 *		length	actual data size in bytes.
 *		ptr	pointer to
 *			  inline: inline-data
 *			  ool: address in message which contains the actual
 *			  data address (char **).
 *		opaque
 *
 *	  returns:
 *		kern_return_t's
 */
#define	SHORT_FORM_LEN(type) \
	(((type->msgtl_header.msgt_number * type->msgtl_header.msgt_size ) \
		+ ((mach_msg_type_size_t)(BYTE_SIZE-1))) >> 3)

#define	LONG_FORM_LEN(type) \
	(((type->msgtl_number * type->msgtl_size ) + (BYTE_SIZE-1)) >> 3)

mach_msg_type_long_t*
norma_parse_kmsg(count, type, size, table, return_val, arg)
	int			*count;
	mach_msg_type_long_t	*type;
	vm_size_t		*size;
	kmsg_parse_tbl_t	*table;
	kern_return_t		*return_val;
	void			*arg;
{
	vm_offset_t	addr;
	vm_offset_t	data;
	int		local_count;
	vm_size_t	local_size;

	NORMA_ENTRY6(KMSG_PARSE_LOG_ID, norma_parse_kmsg, 
				*count, type, *size, table, *return_val, arg);

	/*
	 *  Initialize
	 */
	local_count = *count;
	local_size  = *size;
	addr        = (vm_offset_t)type;

	/*
	 *  Process as much as they want
	 */
	while (local_count && local_size) {
		mach_msg_type_name_t	name;
		mach_msg_type_size_t	length;
		int			align;
		int			(*fp)();
	
		assert(((int)local_size) > 0);

		/*
		 *  Assume no function call.
		 */
		fp = 0;

		/*
		 *  Get the length and adjust the remaining size.
		 */
		if (type->msgtl_header.msgt_longform) {
			name	    = type->msgtl_name;
			length      = LONG_FORM_LEN(type);
			addr       += sizeof(mach_msg_type_long_t);
			local_size -= sizeof(mach_msg_type_long_t);
		} else {
			name	    = type->msgtl_header.msgt_name;
			length      = SHORT_FORM_LEN(type);
			addr       += sizeof(mach_msg_type_t);
			local_size -= sizeof(mach_msg_type_t);
		}

		data	    = addr;	/* point at data or OOL address */

		if (type->msgtl_header.msgt_inline) {
			/*
			 *  Deal with inline type alignment
			 */
			align       = (length + (sizeof(int)-1))
							& ~(sizeof(int)-1);
			addr       += align;
			local_size -= align;

			if (MACH_MSG_TYPE_PORT_ANY(name))
				fp = table->inline_port;
			else
				fp = table->inline_data;
		} else {
			/*
			 * skip over OOL data address & adjust bytes in body.
			 */
			addr       += sizeof(vm_offset_t *);
			local_size -= sizeof(vm_offset_t *);

			if (MACH_MSG_TYPE_PORT_ANY(name))
				fp = table->ool_port;
			else
				fp = table->ool_data;
		}

		/*
		 *  Call the function to deal with the type.
		 */
		if (fp) {
			*return_val = (*fp)(type, length, data, arg);
			local_count--;
			if (*return_val != KERN_SUCCESS) {
				NORMA_LOG2(0,
					"kmsg parse function returned 0x%x\n",
								*return_val);
				break;
			}
		}

		/*
		 * set `type' for next go around.
		 */
		type = (mach_msg_type_long_t*) addr;
	}

	/* 
	 *  Let them know were we left off.
	 */
	*size  = local_size;
	*count = local_count;
	return (type);
}
