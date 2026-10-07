/*
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 */
/*
 * HISTORY
 * $Log: hippi_read_complete.c,v $
 * Revision 1.5.6.1  1995/06/11  18:42:39  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.5  1995/03/06  23:28:58  arlin
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
 * Revision 1.4  1995/02/25  02:53:41  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Medium .. because of the volume of files changed.
 *  Benefit or PTS #: 12263 and 11772
 *  Testing: test which showed problem and the tests the EATS were developed from.
 *  Module(s): hippi_memget.c hippi_open.c hippi_read.c hippi_read_complete.c
 *          hippi_read_request.c hippi_var.h hippi_write.c
 *
 * Revision 1.3  1995/01/17  22:39:33  hobbes
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
 * Revision 1.2  1994/10/28  20:32:57  hobbes
 *  Reviewer: arlin
 *  Risk: low
 *  Benefit or PTS #: 11424
 *  Testing: completed developer which demonstrated the failure previously.
 *  Module(s): hippi_read_complete.c
 *
 * Revision 1.1  1994/10/26  00:13:16  hobbes
 *  Reviewer: Arlin
 *  Risk: Low
 *  Benefit or PTS #: 10942
 *  Testing: Developer tests
 *  Module(s): hippi_read_complete.c, hippi_read_request.c, Makefile, hippi_var.h
 *
 */


/*
 *
 * hippi_read_complete(ihandle, ptr)
 *
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <mach.h>
#include <device/net_status.h>
#include <raw_hippi.h>
#include "hippi_var.h"

extern struct	hippi_table	lt[];

long
hippi_read_complete(int ihandle, char **ptr)
{
	kern_return_t			rc;
	io_buf_ptr_t			data;
	mach_msg_type_number_t		count;
	Request				*rptr;

	/* attempt to validate the ihandle */
	if ((ihandle < 0) || (ihandle >= MAXCONNS)) {
		errno = EBADF;
		return (-1);
	}

        if ((lt[ihandle].hippi_mode & (HIPPI_RAW | HIPPI_DATA)) == 0) {
                errno = EBADF;
                return (-1);
        }

	if (lt[ihandle].pending == 0) {
		errno = EINVAL;
		return (-1);
	}

	/*
	 * Receive the the message
	 */
	rc = mach_msg(	lt[ihandle].rmsg,
			MACH_RCV_MSG | MACH_RCV_TIMEOUT,
			0,
			PAGE_SIZE,
			lt[ihandle].reply_port,
			0,
			MACH_PORT_NULL);

	if ( rc != MACH_MSG_SUCCESS ) {
#if HLIB_DBG
		printf("hippi_read_complete: mach_msg failed rc = 0x%x\n", rc);
#endif
		if ( rc == MACH_RCV_TIMED_OUT )
			errno = EWOULDBLOCK;
		else
			errno = EIO;
		return (-1);
	}

#if HLIB_DBG
	printf("hippi_read_complete: mach_msg succeeded\n");
#endif

	/* adjust the pending count */
	if (lt[ihandle].pending > 0)
		lt[ihandle].pending--;

	/*
	 * Pull the user message out of the Mach message
	 */

	rptr = (Request *)lt[ihandle].rmsg;
	if (rptr->return_code) {
#if HLIB_DBG
                printf("hippi_read_complete: read failed, retcode = 0x%x\n",
                        rptr->return_code);
#endif
		errno = EIO;
		return (-1);
	}
	else {
		*ptr = (char *)rptr->data;
		return ((long)rptr->dataType.msgtl_number);
	}
}
