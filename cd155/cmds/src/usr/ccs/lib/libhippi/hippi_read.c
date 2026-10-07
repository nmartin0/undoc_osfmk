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
 * $Log: hippi_read.c,v $
 * Revision 1.10  1995/03/21  23:30:54  hobbes
 *  Reviewer: Arlin Davis, Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 12723 hippi_read is typed u_long should be long .. and ..
 * 		   12721 hippi_read (CNT mode returns wrong error if len < 2048
 *  Testing: HIPPI EATS
 *  Module(s): hippi_read.c
 *
 * Revision 1.9  1995/03/06  23:28:55  arlin
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
 * Revision 1.8  1995/02/25  02:53:36  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Medium .. because of the volume of files changed.
 *  Benefit or PTS #: 12263 and 11772
 *  Testing: test which showed problem and the tests the EATS were developed from.
 *  Module(s): hippi_memget.c hippi_open.c hippi_read.c hippi_read_complete.c
 *          hippi_read_request.c hippi_var.h hippi_write.c
 *
 * Revision 1.7  1995/01/17  22:39:28  hobbes
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
 * Revision 1.6  1994/12/06  23:24:48  hobbes
 *  Reviewer: Arlin
 *  Risk: Low
 *  Benefit or PTS #: 11731
 *  Testing: code supplied with the PTS and regression testing based on the EATS
 *  Module(s): hippi_read_request.c hippi_read.c (later found via inspection)
 *
 * Revision 1.5  1994/11/19  02:24:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/06/08  18:16:25  hobbes
 * Initial changes to support R1.3
 *
 * Revision 1.3  1994/05/06  22:07:35  hobbes
 *  Merged in the final R1_2 changes which were applied
 *  to fix PTS 6677. Done after WW19.
 *
 * Revision 1.1.6.2  1994/04/19  23:04:38  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 6677
 *  Testing: tests provided in the PTS and HIPPI EATS
 *  Module(s): all .c modules in libhippi
 *
 * Revision 1.2  1994/01/12  19:45:49  hobbes
 * Merging in changes from R1.2 branch.
 *
 * Revision 1.1.6.1  1994/01/12  19:41:12  hobbes
 * While adding the function prototypes to raw_hippi.h I discovered that
 * the declaration for len in this module was not correct per the Man Page.
 * I fixed it to match the Man Page and the function prototypes.
 *
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 5273
 *  Testing: developer testing on a R1.2 WW50 installed system.
 *  Module(s): hippi_read.c
 *
 * Revision 1.1  1993/05/28  02:27:06  hobbes
 * Initial checkin.
 *
 */

/*
 *
 * hippi_read(ihandle, ptr, len)
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
hippi_read(int ihandle, char **ptr, u_long len)
{
	kern_return_t			rc;
	Request                         *rptr;

	/* attempt to validate the ihandle */
	if ((ihandle < 0) || (ihandle >= MAXCONNS)) {
		errno = EBADF;
		return (-1);
	}

	/* make sure the ihandle was obtained via hippi_open */
        if ((lt[ihandle].hippi_mode & (HIPPI_RAW | HIPPI_DATA)) == 0) {
		errno = EBADF;
		return (-1);
	}

	/* make sure the user called hippi_bind */
        if (lt[ihandle].reply_port == 0) { 
		errno = EBADF;
		return (-1);
	}

        /* check the HIPPI nodes memory available */
        if (len > lt[ihandle].node_mem) {
                errno = ENOMEM;
                return (-1);
        }

	/* the hippi interface cannot support any 
	 * read data smaller then 2KBytes in CNT mode
	 */
	if ((lt[ihandle].hippi_mode & HIPPI_CNT) && (len < 2048)) {
		errno = EMSGSIZE;
		return (-1);
	}

	/*
	 * Receive the message: 
	 * must use an async call since there is no time out logic
	 * for sync reads for a HiPPI channel. There is no way to
	 * control-c or kill out of a device_read that is blocked
	 * at the driver with an iowait().
	 */
        rc = device_read_request(       lt[ihandle].master_port,
                                        lt[ihandle].reply_port,
                                        D_READ,
                                        lt[ihandle].rdf,
                                        len );

        if ( rc != KERN_SUCCESS ) {
#if HLIB_DBG
	printf("hippi_read: device_read_request failed rc = %u\n", rc);
#endif
		errno = EIO;
		return (-1);
	}

#if HLIB_DBG
	printf("hippi_read: read_request done, calling mach_msg receive...\n", rc);
#endif
        /*
	 * Receive the the message, no timeout
	 */
	 rc = mach_msg(   lt[ihandle].rmsg,
			  MACH_RCV_MSG,
			  0,
			  PAGE_SIZE,
			  lt[ihandle].reply_port,
			  MACH_MSG_TIMEOUT_NONE,
			  MACH_PORT_NULL);

        if ( rc != KERN_SUCCESS ) {
#if HLIB_DBG
	printf("hippi_read: mach_msg receive failed rc = %u\n", rc);
#endif
		errno = EIO;
		return (-1);
	}

        /*
	 * Pull the user message out of the Mach message
	 */
	rptr = (Request *)lt[ihandle].rmsg;
	if (rptr->return_code) {
	 	errno = EIO;
	 	return (-1);
	}

#if HLIB_DBG
	printf("hippi_read: device_read succeeded %u bytes read\n",
				rptr->dataType.msgtl_number);
#endif

	*ptr = (char *)rptr->data;
	return ((long)rptr->dataType.msgtl_number);
}
