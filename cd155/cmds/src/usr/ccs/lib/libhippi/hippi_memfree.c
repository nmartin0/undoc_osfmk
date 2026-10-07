/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1993, Intel Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: hippi_memfree.c,v $
 * Revision 1.7  1995/03/06  23:28:37  arlin
 *  Added Multiple Packet per Connection support
 *  and CONTinuation support for multiple I/O
 *  per packet and connection.
 *
 *  Reviewer: Jerrie Coffman, Bernie Keany
 *  Risk: medium
 *  Benefit or PTS #: 12411
 *  Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *     Also developed special applications to test
 *     new MPC and CONT modes.
 *  Module(s):
 *     hippi_memfree.c, hippi_status.h, hippi_memget.c,
 *     hippi_var.h, hippi_open.c, hippi_write.c,
 *     hippi_bind.c, hippi_read.c, raw_hippi.h,
 *     hippi_close.c, hippi_read_complete.c,
 *     hippi_config.c, hippi_read_request.c
 *
 * Revision 1.6  1995/01/17  22:39:11  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 11084
 *  Testing: Developer testing.
 *  Module(s): hippi_bind.c hippi_close.c hippi_config.c hippi_memfree.c
 * 	    hippi_memget.c hippi_open.c hippi_read.c hippi_read_complete.c
 * 	    hippi_read_request.c hippi_write.c
 *
 *  Originally this problem should have just caused a change to hippi_bind.c
 *  but the compiler pointed out that all the function declarations were
 *  old style .. so we changed them all in conjunction with this PTS.
 *
 * Revision 1.5  1994/11/19  02:24:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/05/06  22:07:07  hobbes
 *  Merged in the final R1_2 changes which were applied
 *  to fix PTS 6677. Done after WW19.
 *
 * Revision 1.3.4.1  1994/04/19  23:04:25  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 6677
 *  Testing: tests provided in the PTS and HIPPI EATS
 *  Module(s): all .c modules in libhippi
 *
 * Revision 1.3  1993/08/26  18:57:55  hobbes
 * Fix for PTS 6180 adjust the ptr to be deallocated based on hippi_mode.
 *
 * Revision 1.2  1993/07/16  22:18:24  hobbes
 * Changes to get HIPPI_DATA mode to work.
 *
 * Revision 1.1  1993/05/28  02:26:56  hobbes
 * Initial checkin.
 *
 */

/*
 *
 * hippi_memfree(ptr, size)
 *
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <mach.h>
#include <raw_hippi.h>
#include "hippi_var.h"

extern struct	hippi_table lt[];

hippi_memfree(int ihandle, char *ptr, u_long size, int how)
{
	kern_return_t	retcode;

	/* attempt to validate the ihandle */
	if ((ihandle < 0) || (ihandle >= MAXCONNS)) {
		errno = EBADF;
		return (-1);
	}

        if ((lt[ihandle].hippi_mode & (HIPPI_RAW | HIPPI_DATA)) == 0) {
		errno = EBADF;
		return(-1);
	}

	/* check how for at least a valid input */
	if ((how != 0) && (how != 1)) {
		errno = EINVAL;
		return(-1);
	}

	/*
	 * If how is TRUE than the memory was accquired by a hippi_read.
	 *	So just deallocate the memory from the ptr passed.
	 * Else if how is FALSE than check an see if the mode is HIPPI_DATA,
	 *	and adjust the ptr. Otherwise its HIPPI_RAW so just delete
	 *	beginning at the ptr.
	 */

	if (how) {

#if HLIB_DBG
		printf("hippi_memfree: calling vm_deallocate how = %d\n", how);
		printf("\t ptr is 0x%x and size is %d\n", ptr, size);
#endif
		retcode = vm_deallocate(mach_task_self(), (vm_offset_t)ptr, size);

		if (retcode != KERN_SUCCESS) {
			printf("hippi_memfree: vm_deallocate failed\n");
			return(-1);
		}
	}
	else {
		if (lt[ihandle].hippi_mode == HIPPI_DATA) {
			/*
			 * compute the real address to
			 * start to build the packet at
			 */
			if (lt[ihandle].b == 1) {
				ptr -= (sizeof(struct hippi_header) +
						(MAX_D1_SIZE * sizeof(double)));
				size += (sizeof(struct hippi_header) +
						(MAX_D1_SIZE * sizeof(double)));
			}
			else if (lt[ihandle].d1_data != NULL) {
				ptr -= (sizeof(struct hippi_header) +
					lt[ihandle].d1_size * sizeof(double));
				size += (sizeof(struct hippi_header) +
					lt[ihandle].d1_size * sizeof(double));
			}
			else {
				ptr -= (sizeof(struct hippi_header));
				size += (sizeof(struct hippi_header));
			}
		}
#if HLIB_DBG
		printf("hippi_memfree: calling vm_deallocate how = %d\n", how);
		printf("\t ptr is 0x%x and size is %d\n", ptr, size);
#endif
		retcode = vm_deallocate(mach_task_self(), (vm_offset_t)ptr, size);

		if (retcode != KERN_SUCCESS) {
			printf("hippi_memfree: vm_deallocate failed\n");
			return(-1);
		}
	}

	return(0);
}
