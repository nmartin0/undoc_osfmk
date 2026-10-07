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
 * $Log: hippi_write.c,v $
 * Revision 1.13  1995/03/21  23:41:45  hobbes
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 12713 hippi_write (CNT Mode) fails to return error on 1st
 * 			 write if not 1 kbyte aligned.
 *  Testing: HIPPI Raw EATS
 *  Module(s): hippi_write.c
 *
 * Revision 1.12  1995/03/06  23:30:45  arlin
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
 * Revision 1.11  1995/02/25  02:53:53  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Medium .. because of the volume of files changed.
 *  Benefit or PTS #: 12263 and 11772
 *  Testing: test which showed problem and the tests the EATS were developed from.
 *  Module(s): hippi_memget.c hippi_open.c hippi_read.c hippi_read_complete.c
 *          hippi_read_request.c hippi_var.h hippi_write.c
 *
 * Revision 1.10  1995/01/19  01:47:46  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 11963
 *  Testing: developed a test to verify the fix and reran some existing apps.
 *  Module(s): hippi_write()
 *
 * Revision 1.9  1995/01/17  22:39:42  hobbes
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
 * Revision 1.8  1994/11/19  02:24:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/05/06  22:07:44  hobbes
 *  Merged in the final R1_2 changes which were applied
 *  to fix PTS 6677. Done after WW19.
 *
 * Revision 1.5.4.2  1994/04/19  23:04:47  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 6677
 *  Testing: tests provided in the PTS and HIPPI EATS
 *  Module(s): all .c modules in libhippi
 *
 * Revision 1.6  1993/12/30  01:04:06  hobbes
 * merging the changes in the R1_2 branch into the trunk.
 *
 * Revision 1.5.4.1  1993/12/15  01:58:59  hobbes
 * fix for PTS #7224 .. check for NULL before using pointer supplied to
 * hippi_write.c
 *
 *  Reviewer: hobbes
 *  Risk:	low
 *  Benefit or PTS #: 7224
 *  Testing: code in PTS and developer tests
 *  Module(s): hippi_write.c
 *
 * Revision 1.5  1993/08/26  18:55:36  hobbes
 * Fix for PTS 6038 .. the cause of the hippi_read was a bug here.
 *
 * Revision 1.4  1993/07/19  15:00:15  shala
 * Remove mach3 dependencies.
 *
 * Revision 1.3  1993/07/16  22:24:00  hobbes
 * Changes to make HIPPI_DATA mode work.
 *
 * Revision 1.1  1993/05/28  02:27:10  hobbes
 * Initial checkin.
 *
 */

/*
 *
 * hippi_write(ihandle, ptr, len)
 *
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <mach/mach.h>
#include <mach/port.h>
#include <device/device_types.h>
#include <raw_hippi.h>
#include "hippi_var.h"

extern struct	hippi_table	lt[];

hippi_write(int ihandle, char *ptr, u_long len)
{
	struct hippi_header *hh;
	kern_return_t rc;
	int i;
	int rlen;
	int bytes_written;


	/* attempt to catch bogus ihandle */
	if ((ihandle < 0) || (ihandle >= MAXCONNS)) {
		errno = EBADF;
		return (-1);
	}

        if ((lt[ihandle].hippi_mode & (HIPPI_RAW | HIPPI_DATA)) == 0) {
		errno = EBADF;
		return(-1);
	}

        /* check the HIPPI nodes memory available */
        if (len > lt[ihandle].node_mem) {
                errno = ENOMEM;
                return (-1);
        }

	if (ptr == NULL) {
		errno = EINVAL;
		return (-1);
	}

#define BURST_NOT_ALIGNED (1024-1)

	/* if CNT mode,  length must be multiple of 1024 
	 * don't include ifield on first write 
	 * Add check for first write. PTS #12713
	 */
	if (lt[ihandle].hippi_mode & HIPPI_CNT) {
		if (((!lt[ihandle].first_write) && (len & BURST_NOT_ALIGNED)) ||
		     ((lt[ihandle].first_write) && 
			((len - sizeof(struct hippi_ifield)) & BURST_NOT_ALIGNED))) {
			errno = EMSGSIZE;
			return (-1);
		}
	}

	/*
	 * we might need to manipulate the len value,
	 * so we'll squirrel it away
	 */
	rlen = len;

	if (lt[ihandle].hippi_mode & HIPPI_DATA) {
		/*
		 * compute the real address to
		 * start to build the packet at
		 */
		if (lt[ihandle].b == 1) {
			ptr -= (sizeof(struct hippi_header) +
					(MAX_D1_SIZE * sizeof(double)));
			rlen += (sizeof(struct hippi_header) +
					(MAX_D1_SIZE * sizeof(double)));
		}
		else if (lt[ihandle].d1_data != NULL) {
			ptr -= (sizeof(struct hippi_header) +
					lt[ihandle].d1_size * sizeof(double));
			rlen += (sizeof(struct hippi_header) +
					lt[ihandle].d1_size * sizeof(double));
		}
		else {
			ptr -= ( sizeof(struct hippi_header));
			rlen += ( sizeof(struct hippi_header));
		}
		/*
		 * Build the packet .. filling in the headers
		 * and copying (if applicable) the d1_data.
		 */
		hh = (struct hippi_header *)ptr;
		bzero((char *)hh, sizeof(struct hippi_header));
		hh->cci.Ifield = htonl(lt[ihandle].ifield);
		hh->fp.fields.ulp = lt[ihandle].ulp;
		hh->fp.fields.b = lt[ihandle].b;
		if( lt[ihandle].d1_data != NULL ){
			hh->fp.fields.p = 1;
			hh->fp.fields.d1_size = lt[ihandle].d1_size;
			bcopy((caddr_t)lt[ihandle].d1_data,
				(caddr_t)(ptr + sizeof(struct hippi_header)),
				(lt[ihandle].d1_size * sizeof(double)));
		} else {
			hh->fp.fields.p = 0;
			hh->fp.fields.d1_size = 0;
		}
		/* 
		 * byte swap header words just use the union
		 * for the first word.
		 */
		hh->fp.words.w1 = htonl(hh->fp.words.w1);
		hh->fp.fields.d2_size = htonl(len);
	}

#if HLIB_DBG
	printf("hippi_write: before device_write\n");
#endif

/*
	for ( i=0; i < 100; ++i, ++ptr) {
		printf("%x ", *ptr);
		if ((i % 8) == 0)
			printf("\n");
	}
*/

	rc = device_write(lt[ihandle].master_port,
					0,	/* mode */
					0,	/* recnum */
					ptr,
					rlen,
					&bytes_written);

	if (rc != D_SUCCESS) {
		if (rc == D_INVALID_SIZE)
			errno = EMSGSIZE;
		else
			errno = EIO;
#if HLIB_DBG
		printf("hippi_write: device_write failed %x\n", rc);
#endif
		return (-1);
	}

#if HLIB_DBG
	printf("hippi_write: device_write returned %d bytes_written\n",
				bytes_written);
#endif

        /* if continuation mode, keep track of first write
         * since the subsequent writes will have to be check
         * for hippi_burst (1024 bytes) alignment
         */
        if ((lt[ihandle].hippi_mode & HIPPI_CNT) &&
	    (lt[ihandle].first_write)) {
		lt[ihandle].first_write = FALSE;
	}

	return(0);
}
