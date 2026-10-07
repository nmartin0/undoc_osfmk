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
 * $Log: hippi_open.c,v $
 * Revision 1.11  1995/04/21  20:35:54  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 13006 hippi_bind() always fails in HIPPI_CNT mode
 *  Testing: test from the PTS and Eval tested CalTech functionality.
 *  Module(s): hippi_open.c
 *
 * 	hippi_open needed to keep some library state around for hippi_bind
 * 	to check later.
 *
 * Revision 1.10  1995/03/31  22:27:09  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 12720
 *  Testing: code used to demonstrate the problem.
 *  Module(s): cmds_libs/src/usr/ccs/lib/libhippi/hippi_open.c
 * 	    server/uxkern/raw_hippi.c
 *
 * Revision 1.9  1995/03/21  23:39:34  hobbes
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 12718 hippi_open fails to guard against using CNT & MPC
 * 			 modes with HIPPI_DATA mode
 *  Testing: HIPPI EATS
 *  Module(s): hippi_open.c
 *
 * Revision 1.8  1995/03/06  23:28:48  arlin
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
 * Revision 1.7  1995/02/25  02:53:32  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Medium .. because of the volume of files changed.
 *  Benefit or PTS #: 12263 and 11772
 *  Testing: test which showed problem and the tests the EATS were developed from.
 *  Module(s): hippi_memget.c hippi_open.c hippi_read.c hippi_read_complete.c
 *          hippi_read_request.c hippi_var.h hippi_write.c
 *
 * Revision 1.6  1995/01/17  22:39:21  hobbes
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
 * Revision 1.5  1994/11/19  02:24:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/05/06  22:07:29  hobbes
 *  Merged in the final R1_2 changes which were applied
 *  to fix PTS 6677. Done after WW19.
 *
 * Revision 1.3.4.1  1994/04/19  23:04:34  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 6677
 *  Testing: tests provided in the PTS and HIPPI EATS
 *  Module(s): all .c modules in libhippi
 *
 * Revision 1.3  1993/07/27  21:25:48  hobbes
 * Fixed the error check in the ioctl return
 * and improved the debugging printf(s).
 *
 * Revision 1.2  1993/07/20  01:32:20  hobbes
 * Fixed the saving away of the mode of the open interface
 * into the array of open connections.
 *
 * Revision 1.1  1993/05/28  02:27:03  hobbes
 * Initial checkin.
 *
 */
/*
 *
 * hippi_open(dev_name, hippi_mode, mode)
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <mach.h>
#include <raw_hippi.h>
#include "hippi_var.h"

struct hippi_table	lt[MAXCONNS];
int			next;

int
hippi_open(char *dev_name, u_long hippi_mode, u_long mode)
{
	int	uhandle;
        int     status = TRUE;
        int     node;
        mach_port_t     remote_task;
        struct vm_statistics	vm_stat;
        kern_return_t kret;


	if (next == MAXCONNS) {
		errno = EMFILE;
		return (-1);
	}

        /* must be either RAW or DATA, but not both
         * can be either MPC or CNT, but not both
         * must be RAW when either MPC or CNT,
         * if MPC or CNT, then it must be 0_EXCL
	 * PTS #12718, DATA and MPC or CNT not allowed.
         */
        if (((hippi_mode & (HIPPI_RAW | HIPPI_DATA)) == 0) ||
            ((hippi_mode & HIPPI_RAW) && (hippi_mode & HIPPI_DATA)) ||
            ((hippi_mode & HIPPI_MPC) && (hippi_mode & HIPPI_CNT)) ||
            ((hippi_mode & (HIPPI_MPC | HIPPI_CNT)) && (hippi_mode & HIPPI_DATA)) || 
            ((hippi_mode & (HIPPI_MPC | HIPPI_CNT)) && (!(mode & O_EXCL)))) {
                errno = EINVAL;
                return (-1);
        }

#if HLIB_DBG
	printf("hippi_open: calling open, next = %d\n", next);
#endif

	if ((lt[next].fd = open(dev_name, mode)) < 0)
		return (-1);

#if HLIB_DBG
	printf("hippi_open: open succeeded, fd = %d\n", lt[next].fd);
#endif

	lt[next].master_port = ioctl(lt[next].fd, FIODEVPORT, 0);
        if (lt[next].master_port == -1) {
                errno = EIO;
                return(-1);
        }

#if HLIB_DBG
	printf("hippi_open: ioctl succeeded port = %d\n",lt[next].master_port);
#endif

	lt[next].hippi_mode = hippi_mode;
	lt[next].reply_port = 0;
	lt[next].rmsg = 0;
	lt[next].first_write = TRUE;

	/* Limit the IO size of requests, vm_stat on hippi node  */
        kret = norma_port_location_hint(mach_task_self(), lt[next].master_port, &node);
        if(kret != KERN_SUCCESS) {
                errno = EIO;
                return(-1);
#if HLIB_DBG
                printf("hippi_open: norma_port_location_hint failed\n");
#endif
        }

#if HLIB_DBG
        printf("hippi_open: norma_port_location_hint node = %d\n",node);
#endif

        kret = norma_task_create(mach_task_self(), FALSE, node, &remote_task);
        if(kret != KERN_SUCCESS) {
                errno = EIO;
                return(-1);
#if HLIB_DBG
                printf("hippi_open: norma_task_create failed\n");
#endif
        }

        kret = vm_statistics(remote_task, (vm_statistics_data_t *) &vm_stat);
        if(kret != KERN_SUCCESS) {
                errno = EIO;
                return(-1);
#if HLIB_DBG
                printf("hippi_open: vm_statistics failed\n");
#endif
        }

#if HLIB_DBG
        printf("%d free, %d active, %d inactive, %d wired\n",
                        vm_stat.free_count, vm_stat.active_count,
                        vm_stat.inactive_count, vm_stat.wire_count);
#endif

        kret = task_terminate(remote_task);
        if(kret != KERN_SUCCESS) {
                errno = EIO;
                return(-1);
#if HLIB_DBG
                printf("hippi_open: task_terminate failed\n");
#endif
        }

        lt[next].node_mem = vm_stat.free_count + vm_stat.active_count
                                + vm_stat.inactive_count;
        if (vm_stat.wire_count > 1000)
                lt[next].node_mem += (vm_stat.wire_count - 1000);

        lt[next].node_mem = (lt[next].node_mem * vm_stat.pagesize);

#if HLIB_DBG
                printf("hippi_open: lt.node_mem = %d\n", lt[next].node_mem);
#endif

	/* set library state to account for O_EXCL .. checked in hippi_bind */
	if (mode & O_EXCL) 
		lt[next].hippi_mode |= HIPPI_EXC;

        /* multiple packets per connetion mode */
        if (hippi_mode & HIPPI_MPC) {
                if (device_set_status( lt[next].master_port,
                                        HIPPI_DEV_MPPC,
                                        &status,
                                        1                    ) != D_SUCCESS) {
#if HLIB_DBG
                        printf("hippi_open: ioctl HIPPI_DEV_MPPC failed\n");
#endif
                        /* reset driver out of EXCL mode */
                        status = FALSE;
                        device_set_status(lt[next].master_port,HIPPI_DEV_EXCL,&status,1);

                        /* free reference to master_port */
                        if ((kret = mach_port_deallocate(mach_task_self(),
                                        lt[next].master_port)) != KERN_SUCCESS) {
#if HLIB_DBG
                                printf("hippi_open: failed deallocating master_port\n");
                                printf("hippi_open: kret = 0x%x\n", kret);
#endif
                                errno = ENXIO;
                                return(-1);
                        }

                        /* close device */
                        if (close(lt[next].fd)) {
#if HLIB_DBG
                                printf("hippi_open: device close failed\n");
#endif
                                return(-1);
                        }
                        /* return failure status */
                        errno = EACCES;
                        return(-1);
                }
#if HLIB_DBG
                printf("hippi_open: ioctl HIPPI_DEV_MPPC completed\n");
#endif
        /* continuation mode */
        } else if (hippi_mode & HIPPI_CNT) {
                if (device_set_status( lt[next].master_port,
                                        HIPPI_DEV_CONT,
                                        &status,
                                        1                    ) != D_SUCCESS) {
#if HLIB_DBG
                        printf("hippi_open: ioctl HIPPI_DEV_CONT failed\n");
#endif
                        /* reset driver out of EXCL mode */
                        status = FALSE;
                        device_set_status(lt[next].master_port,HIPPI_DEV_EXCL,&status,1);

                        /* free reference to master_port */
                        if ((kret = mach_port_deallocate(mach_task_self(),
                                        lt[next].master_port)) != KERN_SUCCESS) {
#if HLIB_DBG
                                printf("hippi_open: failed deallocating master_port\n");
                                printf("hippi_open: kret = 0x%x\n", kret);
#endif
                                errno = ENXIO;
                                return(-1);
                        }

                        /* close device */
                        if (close(lt[next].fd)) {
#if HLIB_DBG
                                printf("hippi_open: device close failed\n");
#endif
                                return(-1);
                        }
                        /* return failure status */
                        errno = EACCES;
                        return(-1);
                }
#if HLIB_DBG
                printf("hippi_open: ioctl HIPPI_DEV_CONT completed\n");
#endif
        }

#if HLIB_DBG
                printf("hippi_open: exiting\n");
#endif

	uhandle = next;
	++next;
	return(uhandle);
}
