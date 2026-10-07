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
 * $Log: hippi_memget.c,v $
 * Revision 1.7  1995/03/06  23:28:42  arlin
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
 * Revision 1.6  1995/02/25  02:53:22  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Medium .. because of the volume of files changed.
 *  Benefit or PTS #: 12263 and 11772
 *  Testing: test which showed problem and the tests the EATS were developed from.
 *  Module(s): hippi_memget.c hippi_open.c hippi_read.c hippi_read_complete.c
 * 	    hippi_read_request.c hippi_var.h hippi_write.c
 *
 * Revision 1.5  1995/01/17  22:39:16  hobbes
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
 * Revision 1.4  1994/11/19  02:24:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/05/06  22:07:18  hobbes
 *  Merged in the final R1_2 changes which were applied
 *  to fix PTS 6677. Done after WW19.
 *
 * Revision 1.2.4.1  1994/04/19  23:04:30  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 6677
 *  Testing: tests provided in the PTS and HIPPI EATS
 *  Module(s): all .c modules in libhippi
 *
 * Revision 1.2  1993/07/16  22:19:18  hobbes
 * Changes to make HIPPI_DATA mode work.
 *
 * Revision 1.1  1993/05/28  02:26:59  hobbes
 * Initial checkin.
 *
 */

/*
 *
 * hippi_memget(ihandle, size)
 *
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <mach.h>
#include <raw_hippi.h>
#include "hippi_var.h"

extern struct   hippi_table lt[];

char *
hippi_memget(int ihandle, u_long size)
{
	char *ptr, *nptr;
	u_long extra, rsize;

	kern_return_t rc;

	/* attempt to validate the ihandle */
	if ((ihandle < 0) || (ihandle >= MAXCONNS)) {
		errno = EBADF;
		return (NULL);
	}

        if ((lt[ihandle].hippi_mode & (HIPPI_RAW | HIPPI_DATA)) == 0) {
		errno = EBADF;
		return(NULL);
	}

	/*
	 * compute the "right" size of memory to allocate
	 * based on the mode of the open connection.
	 */
	if( lt[ihandle].hippi_mode & HIPPI_DATA ) {
		extra = sizeof(struct hippi_header);
		if( lt[ihandle].d1_data != NULL )
			extra += ( lt[ihandle].d1_size * sizeof(double));
		if( lt[ihandle].b == 1 )
			extra += ((MAX_D1_SIZE - lt[ihandle].d1_size) * sizeof(double));
		rsize = size + extra;
#if HLIB_DBG
		printf("hippi_memget: size(%d) + extra(%d) = total size(%d)\n",
						       size,extra, rsize);
#endif
	}
	else {
		rsize = size;
	}

	/* check the HIPPI nodes memory available */
	if (rsize > lt[ihandle].node_mem) {
		errno = ENOMEM;
		return (NULL);
	}

#if HLIB_DBG
	printf("hippi_memget: before vm_allocate call, size = %d\n", rsize);
#endif

	rc = vm_allocate(mach_task_self(), (vm_offset_t *)&ptr, rsize, TRUE);

	if( rc != KERN_SUCCESS ) {
#if HLIB_DBG
		printf("hippi_memget: vm_allocate failed rc = %d\n", rc);
#endif
		return(NULL);
	}

#if HLIB_DBG
	printf("hippi_memget: ptr is 0x%x, *ptr is 0x%x, &ptr is 0x%x\n",
			ptr, *ptr, &ptr);
#endif

	if( lt[ihandle].hippi_mode & HIPPI_DATA )
		nptr = ptr + extra;
	else
		nptr = ptr;

	return(nptr);
}
