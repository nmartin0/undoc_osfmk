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
 * $Log: hippi_read_request.c,v $
 * Revision 1.7.6.1  1995/06/11  18:42:41  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.7  1995/03/21  23:47:07  hobbes
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 12721 hippi_read_request (CNT mode) return wrong error if
 * 			 len < 2048
 *  Testing: HIPPI Raw EATS
 *  Module(s): hippi_read_request.c
 *
 * Revision 1.6  1995/03/06  23:29:00  arlin
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
 * Revision 1.5  1995/02/25  02:53:45  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Medium .. because of the volume of files changed.
 *  Benefit or PTS #: 12263 and 11772
 *  Testing: test which showed problem and the tests the EATS were developed from.
 *  Module(s): hippi_memget.c hippi_open.c hippi_read.c hippi_read_complete.c
 *          hippi_read_request.c hippi_var.h hippi_write.c
 *
 * Revision 1.4  1995/01/19  01:45:21  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 11732
 *  Testing: wrote a test case for the PTS and reran some existing applications.
 *  Module(s): hippi_read_request
 *
 * Revision 1.3  1995/01/17  22:39:37  hobbes
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
 * Revision 1.2  1994/12/06  23:25:41  hobbes
 *  Reviewer: Arlin
 *  Risk: Low
 *  Benefit or PTS #: 11731
 *  Testing: code supplied with the PTS and regression testing based on the EATS
 *  Module(s): hippi_read_request.c hippi_read.c (later found via inspection)
 *
 * Revision 1.1  1994/10/26  00:12:03  hobbes
 *  Reviewer: Arlin
 *  Risk: Low
 *  Benefit or PTS #: 10942
 *  Testing: Developer tests
 *  Module(s): hippi_read_complete.c, hippi_read_request.c, Makefile, hippi_var.h
 *
 */


/*
 *
 * hippi_read_request(ihandle, len)
 *
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <mach.h>
#include <device/net_status.h>
#include <raw_hippi.h>
#include "hippi_var.h"

extern struct	hippi_table	lt[];

int
hippi_read_request(int ihandle, long len)
{
	kern_return_t			rc;
	io_buf_ptr_t			data;
	mach_msg_type_number_t	count;

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

	/* make sure the user asked for > 0 bytes */
	if (len <= 0) {
		errno = EINVAL;
		return (-1);
	}

        /* check the HIPPI nodes memory available */
        if (len > lt[ihandle].node_mem) {
                errno = ENOMEM;
                return (-1);
        }

        /* the hippi interface cannot support any 
	 * read data smaller then 2KBytes in CNT mode
	 * Change errno to EMSGSIZE.
	 */
	if ((lt[ihandle].hippi_mode & HIPPI_CNT) && (len < 2048)) {
	      errno = EMSGSIZE;
	      return (-1);
	}

	/*
	 * Request to receive a packet
	 */
	 
	rc = device_read_request(	lt[ihandle].master_port,
					lt[ihandle].reply_port,
					D_READ,
					lt[ihandle].rdf,
					len );

	if ( rc != KERN_SUCCESS ) {
#if HLIB_DBG
		printf("hippi_read_request: device_read_request failed rc = %u\n", rc);
#endif
		errno = EIO;
		return (-1);
	}

#if HLIB_DBG
	printf("hippi_read_request: device_read_request succeeded\n");
#endif

	lt[ihandle].pending++;
	return (0);
}
