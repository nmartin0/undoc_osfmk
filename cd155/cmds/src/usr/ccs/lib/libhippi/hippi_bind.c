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
 * $Log: hippi_bind.c,v $
 * Revision 1.10  1995/03/21  23:33:46  hobbes
 *  Reviewer: Arlin Davis, Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 12749 hippi_bind shouldn't allow multiple binds .. and ..
 * 		   12736 hippi_bind fails to enforce ulp to be 0 in CNT mode
 *  Testing: HIPPI EATS
 *  Module(s): hippi_bind.c
 *
 * Revision 1.9  1995/03/06  23:28:14  arlin
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
 * Revision 1.8  1995/01/17  22:38:44  hobbes
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
 * Revision 1.7  1994/12/06  23:39:17  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: cleanup a lint error
 *  Testing: the test cases provided with PTS 11731 and regression tests.
 *  Module(s): hippi_bind.c
 *
 * Revision 1.6  1994/11/19  02:24:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/06/08  18:16:27  hobbes
 * Initial changes to support R1.3
 *
 * Revision 1.4  1994/05/06  22:06:00  hobbes
 *  Merged in the final R1_2 changes which were applied
 *  to fix PTS 6677. Done after WW19.
 *
 * Revision 1.2.4.2  1994/04/19  23:03:13  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 6677
 *  Testing: tests provided in the PTS and HIPPI EATS
 *  Module(s): all .c modules in libhippi
 *
 * Revision 1.3  1993/12/30  01:03:33  hobbes
 * merging the changes in the R1_2 branch into the trunk.
 *
 * Revision 1.2.4.1  1993/12/15  02:01:26  hobbes
 *  Reviewer: hobbes
 *  Risk: Low
 *  Benefit or PTS #: 6041
 *  Testing: no test supplied via PTS .. developer testing done.
 *  Module(s): hippi_bind.c
 *
 * Revision 1.2  1993/07/27  21:36:29  hobbes
 * Improved packet filter logic and debug printf(s).
 *
 * Revision 1.1  1993/05/28  02:26:19  hobbes
 * Initial checkin.
 *
 */

/*
 *
 * hippi_bind()
 *
 */
#include <sys/types.h>
#include <device/net_status.h>
#include <errno.h>
#include <raw_hippi.h>
#include "hippi_var.h"

extern struct	hippi_table	lt[];

hippi_bind(int ihandle, u_char ulp, short port)
{
	kern_return_t	retcode;
	hlib_filter_t	filter;
	u_char		timeout = 0xff;

        /* if user is requesting to recieve ALL DST traffic
         * with ulp = 0 then we must be opened exclusive
         */
        if (ulp == 0) { 
		if ((lt[ihandle].hippi_mode & HIPPI_EXC) == 0) {
			errno = EACCES;
			return(-1);
		}
        }
        else if (ulp < 0x80) {
                errno = EADDRNOTAVAIL;
                return(-1);
        }

	if ((ihandle < 0) || (ihandle >= MAXCONNS)) {
		errno = EBADF;
		return (-1);
	}

        if ((lt[ihandle].hippi_mode & (HIPPI_RAW | HIPPI_DATA)) == 0) {
		errno = EBADF;
		return(-1);
	}

        /* HIPPI_CNT mode must set promiscuous filter, PTS#12736 */
        if ((lt[ihandle].hippi_mode & HIPPI_CNT) && (ulp != 0)) {
		errno = EBADF;
		return(-1);
	}

	/* check to make sure the user isn't calling bind again PTS 12749 */
	if (lt[ihandle].reply_port != 0) {
		errno = EEXIST;
		return (-1);
	}

	/*
	 * check for -1 in port 
	 * if it is true then set offset and bsize to 0
	 * Save the ulp and port so the close can remove filter
	 */
	filter.ulp = ulp;
	filter.flag = TRUE;
        lt[ihandle].rulp = ulp;
	lt[ihandle].rport = port;
	if (port == -1) {
		filter.offset = 0;
		filter.bsize = 0;
		port = 0; 		/* set port to zero so the recnum is okay */
	} else {
		filter.offset = 8;
		filter.bsize = 2;
		filter.data = htons(port);
	}

#if HLIB_DBG
	printf("hippi_bind: ulp(%x),port(%x),off(%x),bsize(%x),flag(%x)\n",
		filter.ulp,htons(filter.data),filter.offset,filter.bsize,filter.flag);
#endif
	/*
	 *  Get a reply port.
	 */
	if ((lt[ihandle].reply_port = mach_reply_port()) == MACH_PORT_NULL) {
#if HLIB_DBG
		printf("hippi_bind: Couldn't get reply port\n");
#endif
		lt[ihandle].reply_port = 0;
		errno = EIO;
		return (-1);
	}

	/*
	 * bump the queue limit
	 */
	if (mach_port_set_qlimit(	mach_task_self(),
					lt[ihandle].reply_port,
					MACH_PORT_QLIMIT_MAX) != KERN_SUCCESS) {
#if HLIB_DBG
		printf("hippi_read: couldn't increase port queue limit\n");
#endif
		errno = EIO;
		return (-1);
	}

	/*
	 * set the filter
	 */
	retcode = device_set_filter(    lt[ihandle].master_port,
					lt[ihandle].reply_port,
					MACH_MSG_TYPE_MAKE_SEND,
					100,    /* priority */
					(filter_array_t) &filter,
					((mach_msg_type_number_t) sizeof(struct hlib_filter)) );

	if (retcode != D_SUCCESS) {
		if (retcode == D_IO_ERROR)
			errno = ENOBUFS;
		else if (retcode == D_INVALID_OPERATION)
			errno = EADDRINUSE;
		else
			errno = EIO;
#if HLIB_DBG
		printf("hippi_bind: Couldn't set filter (0x%x)\n", retcode);
		printf("hippi_bind: destroyed the reply port reference\n");
#endif
                /* destroy and clear the reply_port reference */
		mach_port_destroy(mach_task_self(),lt[ihandle].reply_port);
		lt[ihandle].reply_port = 0;

		return (-1);
	}

       /* allocate a page for the async mach reply message */
	retcode = vm_allocate(mach_task_self(), (vm_offset_t *)&lt[ihandle].rmsg,
				      PAGE_SIZE, TRUE);

	if (retcode != D_SUCCESS) {
#if HLIB_DBG
		printf("hippi_bind: Couldn't vm_allocate rmsg (0x%x)\n",retcode);
#endif
		return (-1);
	}

	lt[ihandle].rdf |= (htons(port) << 16);
	lt[ihandle].rdf |= (timeout << 8);
	lt[ihandle].rdf |= (ulp << 0);

	return (0);
}
