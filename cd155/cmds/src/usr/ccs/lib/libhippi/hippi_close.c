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
 * $Log: hippi_close.c,v $
 * Revision 1.7  1995/03/06  23:28:26  arlin
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
 * Revision 1.6  1995/01/17  22:38:59  hobbes
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
 * Revision 1.5  1994/11/19  02:24:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/05/06  22:06:48  hobbes
 *  Merged in the final R1_2 changes which were applied
 *  to fix PTS 6677. Done after WW19.
 *
 * Revision 1.1.6.3  1994/04/19  23:04:16  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 6677
 *  Testing: tests provided in the PTS and HIPPI EATS
 *  Module(s): all .c modules in libhippi
 *
 * Revision 1.3  1994/01/17  23:20:26  hobbes
 * Merged R1.2 changes into the mainline. See checkin below for details.
 *
 * Revision 1.1.6.2  1994/01/15  00:47:46  hobbes
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 7691
 *  Testing: ran EATS.
 *  Module(s): hippi_close.c
 *
 * Revision 1.2  1993/12/30  01:03:59  hobbes
 * merging the changes in the R1_2 branch into the trunk.
 *
 * Revision 1.1.6.1  1993/12/15  02:07:24  hobbes
 * fix for PTS #7277 .. and general clean up of error handling.
 *
 *  Reviewer: hobbes
 *  Risk: Low
 *  Benefit or PTS #: 7277
 *  Testing: test case provided in PTS and developer testing.
 *  Module(s): hippi_close.c
 *
 * Revision 1.1  1993/05/28  02:26:48  hobbes
 * Initial checkin.
 *
 */

/*
 *
 * hippi_close(ihandle)
 *
 */
#include <sys/types.h>
#include <mach.h>
#include <sys/errno.h>
#include <raw_hippi.h>
#include "hippi_var.h"


extern struct	hippi_table	lt[];

hippi_close(int ihandle)
{
	int	status = FALSE;
	hlib_filter_t   filter;
	kern_return_t ret;

	/* attempt to catch bogus ihandles */
	if ((ihandle < 0) || (ihandle >= MAXCONNS)) {
		errno = EBADF;
		return (-1);
	}

        if ((lt[ihandle].hippi_mode & (HIPPI_RAW | HIPPI_DATA)) == 0) {
#if HLIB_DBG
                printf("hippi_close: hippi_mode incorrect\n");
#endif
                errno = EBADF;
                return(-1);
        }

        /* unset the filter if one was set */ 
	if (lt[ihandle].reply_port) {
		/* get and check for -1 in port
		 * if it is true then set offset to 0
		 * and bsize to 0
		 */
		filter.flag = FALSE;   /* unsetting the filter */
		filter.ulp = lt[ihandle].rulp;
		if (lt[ihandle].rport == -1) {
			filter.offset = 0;
		    	filter.bsize = 0;
		    	filter.data = 0;   
		} else {
			filter.offset = 8;
		    	filter.bsize = 2;
		    	filter.data = htons(lt[ihandle].rport);
		}
	
               /*
	 	* unset the filter
	 	*/
        	ret = device_set_filter(lt[ihandle].master_port,
					lt[ihandle].reply_port,
					MACH_MSG_TYPE_MAKE_SEND,
					100,    /* priority */
					(filter_array_t) &filter,
					((mach_msg_type_number_t) sizeof(struct hlib_filter)));

        	if (ret != D_SUCCESS) {
			if (ret == D_INVALID_OPERATION)
				errno = EADDRINUSE;
			else
				errno = EIO;

#if HLIB_DBG
			printf("hippi_close: Couldn't unset filter (0x%x)\n", ret);
#endif
			return (-1);
		}
	}

	lt[ihandle].rport = 0;
	lt[ihandle].rulp = 0;

        /* multiple packets per connection mode */
        if (lt[ihandle].hippi_mode & HIPPI_MPC) {
                if (device_set_status( lt[ihandle].master_port,
                                        HIPPI_DEV_MPPC,
                                        &status,
                                        1                    ) != D_SUCCESS) {
#if HLIB_DBG
                        printf("hippi_close: ioctl HIPPI_DEV_MPPC failed\n");
#endif
                        /* attempt to reset driver out of EXCL mode before exit */
                        device_set_status(lt[ihandle].master_port,HIPPI_DEV_EXCL,&status,1);
                        errno = ENXIO;
                        return(-1);
                }

        /* continuation mode */
        } else if (lt[ihandle].hippi_mode & HIPPI_CNT) {
                if (device_set_status( lt[ihandle].master_port,
                                        HIPPI_DEV_CONT,
                                        &status,
                                        1                    ) != D_SUCCESS) {
#if HLIB_DBG
                        printf("hippi_close: ioctl HIPPI_DEV_CONT failed\n");
#endif
                        /* attempt to reset driver out of EXCL mode before exit */
                        device_set_status(lt[ihandle].master_port,HIPPI_DEV_EXCL,&status,1);
                        errno = ENXIO;
                        return(-1);
                }
        }

        /* reset driver out of exclusive mode via set_status */
        if (lt[ihandle].hippi_mode & HIPPI_EXC) {
                if (device_set_status( lt[ihandle].master_port,
                                        HIPPI_DEV_EXCL,
                                        &status,
                                        1                    ) != D_SUCCESS) {
#if HLIB_DBG
                        printf("hippi_close: ioctl HIPPI_DEV_EXCL failed\n");
#endif
                        errno = ENXIO;
                        return(-1);
                }
        }

        if ( close(lt[ihandle].fd)) {
#if HLIB_DBG
                printf("hippi_close: close failed\n");
                perror("hippi_close");
#endif
                return(-1);
        }
#if HLIB_DBG
        printf("hippi_close: close complete\n");
#endif

	if (lt[ihandle].reply_port != 0) {
		if ((ret = (mach_port_destroy(mach_task_self(),
						lt[ihandle].reply_port))) != KERN_SUCCESS) {
#if HLIB_DBG
			printf("hippi_close: failed deallocating the reply_port\n");
			printf("hippi_close: ret = 0x%x\n", ret);
#endif
			errno = ENXIO;
			return(-1);
		}
	}

	if ((ret = (mach_port_deallocate(mach_task_self(),
					lt[ihandle].master_port))) != KERN_SUCCESS) {
#if HLIB_DBG
		printf("hippi_close: failed deallocating the master_port\n");
		printf("hippi_close: ret = 0x%x\n", ret);
#endif
		errno = ENXIO;
		return(-1);
	}

        /* finally deallocate the mach reply message buffer */
	if (lt[ihandle].rmsg) {
        	ret = vm_deallocate(	mach_task_self(), 
					(vm_offset_t)lt[ihandle].rmsg,
					PAGE_SIZE);

		if (ret != KERN_SUCCESS) {
#if HLIB_DBG
			printf("hippi_close: rmsg vm_deallocate failed\n");
#endif
			return(-1);
		}
		lt[ihandle].rmsg = 0;
	}

	lt[ihandle].hippi_mode = 0;
	lt[ihandle].reply_port = 0;
	lt[ihandle].fd = 0;

#if HLIB_DBG
	printf("hippi_close: succeeded\n");
#endif
	return(0);
}
