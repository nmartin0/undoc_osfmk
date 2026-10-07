/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * File:	mach_iface.c
 *
 * Abstract:
 *	Getting packets from/to a Mach Network Interface.
 *
 * HISTORY
 * $Log: ether_io.c,v $
 * Revision 1.27  1995/02/25  00:52:31  hobbes
 *  Reviewer: Jerrie Coffman
 *  Risk: Low
 *  Benefit or PTS #: 12476
 *  Testing:  verified you could bring up the new interface (es0) ..
 * 	   as well as all current interfaces (em0 and ifhip0).
 *  Module(s): uxkern/ether_io.c
 *
 * Revision 1.26  1995/02/01  22:18:51  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.25  1994/12/13  20:55:59  suri
 *  Reviewer: stans
 *  Risk: M
 *  Benefit or PTS #: 10550
 *  Testing: tcp-ip, nfs and message EATs
 *  Module(s): iface_find() in server/uxkern/ether_io.c
 *  Description: When allocating the receive port, the ethernet interface uses
 *  MACH_PORT_QLIMIT_MAX for setting a qlimit on the port. However, during the
 *  NORMA rewrite, MACH_PORT_QLIMIT_MAX was raised from 16 to 16K, causing the
 *  qlimit on the ethernet's receive port to go upto 16K. With such a huge
 *  qlimit, network nodes (usually the bootnode) were occasionally running out
 *  of memory with a lot of network type kmsgs queued up in the ethernet's
 *  receive port. A new macro called MACH_PORT_QLIMIT_FOR_ETHERNET has been
 *  defined with value 16 and used instead of MACH_PORT_QLIMIT_MAX.
 *
 * Revision 1.24  1994/11/18  20:47:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.23  1994/05/12  18:08:45  jlitvin
 * Remove some dead code found by lint.
 *
 * Revision 1.22  1994/01/12  17:47:05  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.21  1993/09/15  21:01:52  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.20.2.1  1993/09/15  21:00:07  cfj
 * Back out the changes made in 1.19 and force IFF_BROADCAST for HIPPI.
 *
 * Revision 1.19  1993/07/16  20:15:11  hobbes
 * Fixed for PTS #5684 .. correctly wrap and out-of-line message
 * in an mbuf .. and zfree the mach_msg.
 *
 * Revision 1.18  1993/07/16  19:30:26  rkl
 * Added support for HiPPI network buffer cache.
 *
 * Revision 1.17  1993/07/14  18:41:16  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.16  1993/07/08  20:59:34  rkl
 * Changed device_write_request() to device_write for the single mbuf case.
 * This should have been done when OSF made the fix to the multi-mbuf case,
 * it was just missed.  This could lead to possible buffer corruption.
 *
 * Revision 1.1.1.3  1993/07/01  21:00:55  cfj
 * Adding new code from vendor
 *
 * Revision 1.15  1993/06/23  21:57:19  hobbes
 * Added call to initialize HIPPI driver buffers.
 *
 * Revision 1.14  1993/06/09  21:16:51  cfj
 * Fixed a case where when building the RAMDISK, this file would not compile due to RFC_1323 not being defined.
 *
 * Revision 1.13  1993/06/08  21:55:37  hobbes
 * fixed the filter logic and added RFC_1323 ifdefs to work
 * with the ramdisk server.
 *
 * Revision 1.12  1993/05/27  22:12:50  hobbes
 * Checkin of HIPPI specific networking changes.
 *
 * Revision 1.11  1993/05/07  22:31:59  cfj
 * Fixed the merge.  Now matches ad1.0.3.
 *
 * Revision 1.10  1993/05/07  18:25:55  nandy
 * Fixed a merge conflict.
 *
 * Revision 1.9  1993/05/06  19:26:57  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.8  1993/04/15  00:06:56  cfj
 * Removed another define(__i860__)
 *
 * Revision 1.7  1993/04/14  15:15:36  cfj
 * Merge with T9.5
 *
 * Revision 1.4.6.2  1993/04/14  15:04:45  cfj
 * Remove defined(__i860__) where not needed.
 *
 * Revision 1.6  1993/04/03  03:11:31  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/03/25  23:30:58  cfj
 * T9 Merge.
 *
 * Revision 1.4.6.1  1993/03/24  23:44:02  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 *     Revision 2.14  93/03/22  19:45:38  nina
 *     Default device_node and network_node to netserver_node if
 *     a network interface prefix is not specified.
 * 
 *     Revision 2.13  93/03/03  17:15:17  mjl
 *     Use INVALID_NODE to properly initialzie ns_node in net_interface_name().
 * 
 *     Revision 2.12  93/02/25  17:50:28  nina
 *     Rewrote net_interface_name() to support new network interface
 *     naming scheme.
 * 
 * Revision 1.1.2.1.2.2  1993/01/09  00:05:46  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.4  1992/12/18  01:31:15  rkl
 * Added network buffer cache to be used by ether_start() to virtually
 * eliminate the vm_allocate/vm_deallocate when mbufs are chained.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:04:52  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 * 
 * Revision 1.3  1992/12/11  03:05:15  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:54:02  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:42:52  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.11  92/11/11  22:37:06  loverso
 * 	Remove ; after decl_simple_lock_data; 860 compiler complains.
 * 
 * Revision 2.10  92/11/11  18:34:52  loverso
 * 	Revision 1.3  1992/10/21  13:03:42  devrcs
 * 	Fix for bug #162: completed network interface initialization and
 * 	replaced ether_ioctl() locking mechanism.
 * 	Fix for bug #88: support statistics for netstat -i (paire)
 * 	[1992/09/23  12:39:57  barbou]
 * 
 * Revision 2.17  1993/04/29  14:04:10  klh
 * 	Revision 2.15  93/04/15  22:47:39  condict
 * 		Optimization: copy non-contiguous mbufs into a zalloc'd buffer,
 * 		instead of into vm_allocated memory, avoiding heavy-duty vm ops.
 *
 * 	Revision 2.14  93/04/08  11:39:32  loverso
 * 		ux server threads are wired by default. (loverso)
 *
 * 	Revision 2.13  93/04/05  18:05:26  durriya
 * 		change name of netisr_init() to netinput_init(). netisr_is_init is now
 * 		initialised in netinit().
 * 		[93/04/05            durriya]
 *
 * 	Revision 2.12  93/01/14  13:09:05  loverso
 * 		net_interface_name() should use node_t and strtoul(), not int.
 *
 * Revision 2.16  93/04/16  14:14:34  nina
 * netserver_node must be #if defined(TNC).  According to INTEL engineering
 * #if   defined(__i860__) || defined(TNC) should be #if defined(TNC).
 * 
 * Revision 2.15  93/04/12  15:49:43  nina
 * Rearranged #ifdefs to fix i860 compilation errors
 * 
 * Revision 2.14  93/03/22  19:45:38  nina
 * Default device_node and network_node to netserver_node if
 * a network interface prefix is not specified.
 * 
 * Revision 2.13  93/03/03  17:15:17  mjl
 * Use INVALID_NODE to properly initialzie ns_node in net_interface_name().
 * 
 * Revision 2.12  93/02/25  17:50:28  nina
 * Rewrote net_interface_name() to support new network interface
 * naming scheme.
 * 
 * Revision 2.11  92/11/11  22:37:06  loverso
 * 	Remove ; after decl_simple_lock_data; 860 compiler complains.
 * 
 * Revision 2.10  92/11/11  18:34:52  loverso
 * 	Revision 1.3  1992/10/21  13:03:42  devrcs
 * 	Fix for bug #162: completed network interface initialization and
 * 	replaced ether_ioctl() locking mechanism.
 * 	Fix for bug #88: support statistics for netstat -i (paire)
 * 	[1992/09/23  12:39:57  barbou]
 * 
 * Revision 2.9  92/09/11  09:29:19  rabii
 * 	Added Intel's changes for i860 to deal with <node>cnp0 (rabii)
 * 
 * Revision 2.8  92/06/09  16:42:18  pjg
 * 	Set flag netisr_is_init in netisr_init.
 * 
 * Revision 2.7  92/05/24  13:55:28  pjg
 * 	Revision 3.13  92/04/03  14:14:05  condict
 * 	Change the location of interrupt_enter/exit calls in net_input_thread 
 * 	so that the entire loop except for the message-receive is executed 
 * 	between them.  Necessary for the uniproc config, because zalloc can 
 * 	call thread_wakeup.
 * 
 * 	Revision 3.12  92/03/31  15:41:10  emcmanus
 * 	Shorten the network input thread's name for greater gdb 
 * 	comprehensibility.
 * 
 * Revision 2.6  92/05/01  10:00:13  rabii
 * 	Fill in interface details from kernel reply.
 * 	Don't take IFQ_LOCK in ether_ioctl(); causes deadlock as it is taken
 * 	on output when address set.  We probably need to add another lock
 * 	(let Grenoble do this).(loverso)
 * 
 * Revision 2.5  92/03/09  13:33:22  durriya
 * 	92/02/25  17:48:39  condict
 * 	Change all calls to cthread_wire to ux_thread_wire, so ux_server_loop
 * 	can correctly compute required number of Mach kernel threads.
 * 
 * 	91/12/20  17:57:33  barbou
 * 	Check the device_xxx() routines' return value.
 * 
 * Revision 2.4  91/12/17  09:21:12  roy
 * 	91/10/23  16:39:01  condict
 * 	Remove unnecessary get_time calls.  The global time var now works 
 * 	correctly.
 * 
 * Revision 2.3  91/10/14  13:23:58  sjs
 * 	91/10/01  14:11:55  condict
 * 	Protect ether_ioctl with IFQ_LOCKS instead of spl's, which are no-ops 
 * 	now.
 * 	The wheel spins once again...  Adapted for use in POE and
 * 	with the new 4.4 network code.
 * 	[91/01/19            af]
 * 
 * Revision 2.2  91/08/31  14:26:14  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.7  91/08/27  15:38:39  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.6  91/07/02  14:47:04  condict
 * ?
 * 
 * Revision 3.5  91/06/28  14:14:57  sp
 * Make net_msg_zone pageable
 * 
 * Revision 3.4  91/06/27  15:52:39  sp
 * Use OSF/1 zinit/zchange interfaces
 * 
 * Revision 3.3  91/06/25  17:12:34  condict
 * Adapt to OSF/1 networking code (call ether_input/ether_output funcs).
 *
 * Revision 3.2  91/05/29  16:18:36  condict
 * change ifa_addr. to ifa_addr->, to accomodate change in struct def.
 * 
 * Revision 2.7  91/01/08  15:00:56  rpd
 * 	Fixed iface_find to raise the qlimit of the network receive port.
 * 	[91/01/05            rpd]
 * 
 * Revision 2.6  90/09/09  22:32:55  rpd
 * 	Initial support for mapped ether interface.
 * 	[90/08/30  17:48:50  af]
 * 
 * Revision 2.5  90/09/09  14:13:48  rpd
 * 	Deallocate the device port after closing it.
 * 	[90/08/23            rpd]
 * 
 * Revision 2.4  90/06/02  15:27:41  rpd
 * 	Reorganize to have interrupt_enter around mbuf routines since
 * 	they call spl routines.
 * 	[90/05/10            rwd]
 * 	Updated set_thread_priority call for the new priority range.
 * 	[90/04/23            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  20:15:13  rpd]
 * 
 * Revision 2.3  90/05/29  20:25:09  rwd
 * 	Reorginize to have interrupt_enter around mbuf routines since
 * 	they call spl routines.
 * 	[90/05/10            rwd]
 * 
 * Revision 2.2  89/12/08  20:16:42  rwd
 * 	Call ux_thread_wire before setting thread priority
 * 	[89/11/01            rwd]
 * 
 * Revision 2.1  89/08/04  14:51:21  rwd
 * Created.
 * 
 * 23-Mar-89  David Golub (dbg) at Carnegie-Mellon University
 *	Changed network input to receive packets asynchronously.
 *
 * 22-Feb-89  David Golub (dbg) at Carnegie-Mellon University
 *	Modified for U*X server use.
 *
 * 29-Sep-88  Alessandro Forin (af) at Carnegie-Mellon University
 *	Created.
 *
 * $EndLog$
 */




#include <map_ether.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <kern/zalloc.h>
#include <sys/synch.h>

#include <sys/time.h>

#include <net/if.h>
#include <net/if_types.h>
#include <net/netisr.h>
#include <net/if_types.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>



#ifdef NS
#include <netns/ns.h>
#include <netns/ns_if.h>
#endif


#include <uxkern/device_reply_hdlr.h>
#include <uxkern/device_utils.h>

#include <uxkern/import_mach.h>
#include <device/device_types.h>
#include <device/net_status.h>
#include <kern/lock.h>
#include <sys/user.h>
#include <sys/kernel.h>

#define MACH_PORT_QLIMIT_FOR_ETHERNET 16

int debug_netcode = 0;
zone_t	mbuf_allocation_zone;
/*
 * Interface descriptor
 *	NOTE: most of the code outside will believe this to be simply
 *	a "struct ifnet".  That is ok, since it is exactly what ethernets
 *	do in Unix anyway.  The other information is, on the other hand
 *	our own business.
 */
struct ether_softc {
	struct	arpcom es_ac;	/* same as ethernets */
#define es_if	es_ac.ac_if	/* a struct ifnet used outside */
#define es_addr	es_ac.ac_enaddr	/* hardware address, 6 bytes ok for ethers */
#define es_ip	es_ac.ac_ipaddr	/* copy of IP address */
	mach_port_t es_port;	/* where to send requests */
	mach_port_t es_reply_port;	/* where to receive data */
	char	es_name[IFNAMSIZ];
				/* name lives here */
	struct ether_softc *es_link;
				/* link in list of reply ports */
	decl_simple_lock_data(,es_lock)	/* to parallelize ether_ioctl() */
};

struct ether_softc *es_list = 0;
				/* list of all ethernet interfaces,
				   to find by reply port. */

int hippi_output(), hippi_ioctl(), hippi_start();

/*
 * Ethernet trigger routine.
 */
ether_start(ifp)
	struct ifnet *ifp;
{
	register struct ether_softc *es = (struct ether_softc *)ifp;
	register struct mbuf *m;
	kern_return_t	rc;
	int bytes_written;

	IF_DEQUEUE(&ifp->if_snd, m);
	if (m == 0)
		return (0);

	/*
	 * Send message to interface
	 */
	{
	    unsigned int  totlen;
	    char *data_addr;
	    register struct mbuf *m1;

	    totlen = 0;
	    for (m1 = m;
		 m1;
		 m1 = m1->m_next)
		totlen += m1->m_len;

	    if (m->m_next == 0) {
		/*
		 * All data in one chunk
		 */
		data_addr = mtod(m, char *);
xxx("ether_start:", data_addr, totlen);
		rc = device_write(es->es_port,
					  0,	/* mode */
					  0,	/* recnum */
					  data_addr,
					  totlen,
					  &bytes_written);
		if (rc != KERN_SUCCESS) {
			printf("ether_start: device_write() returned 0x%x\n", rc);
			ifp->if_oerrors++;
		} else
			ifp->if_opackets++;
	    }
	    else {
		/*
		 * Must allocate contiguous memory and
		 * copy mbuf string into it.
		 */
		ZALLOC(mbuf_allocation_zone, data_addr, char *);

		if (debug_netcode) printf("ether_start: copy\n");

		(void) m_copydata(m, 0, totlen, data_addr);
		rc = device_write(es->es_port, 
					  0,	/* mode */
					  0,	/* recnum */
					  data_addr,
					  totlen,
					  &bytes_written);
		if (rc != KERN_SUCCESS) {
			printf("ether_start: device_write() returned 0x%x\n", rc);
			ifp->if_oerrors++;
		} else
			ifp->if_opackets++;
		ZFREE(mbuf_allocation_zone, data_addr);
	    }
	}
	m_freem(m);	/* sent */
	return (0);
}

int net_input_dispose();	/* forward */

#if RFC_1323
int net_hippi_dispose();
#endif

extern struct mbuf *mclgetx();
zone_t	net_msg_zone;

extern mach_port_t device_server_port;

mach_port_t	net_reply_port_set;

void
net_input_thread()
{
	register struct ether_softc *es;
	register struct ifnet *ifp;
	struct mbuf *m;
	int	len;
	char *	addr;
	struct ether_header *eh;
	register net_rcv_msg_t msg = (net_rcv_msg_t)0;
#if RFC_1323
	register net_rcv_msg_sg_t sg_msg;
	int hippi;
#endif

	cthread_set_name(cthread_self(), "net input thread");

	/*
	 * Make this thread high priority.
	 */
	set_thread_priority(mach_thread_self(), 3);

	/*
	 * Arrange that all OSF/1 code, including the zalloc/zfree is
	 * executed between interrupt_enter/interrupt_exit.  This is
	 * because zalloc will call thread_sleep/thread_wakeup and the
	 * uniproc configuration requires the master lock to be held
	 * for such operations:
	 */
	interrupt_enter(SPLIMP);

	while (TRUE) {
	    kern_return_t kr;

	    /*
	     * Allocate a message structure, and receive the next
	     * network message.
	     */
	    msg = (net_rcv_msg_t)zalloc(net_msg_zone);

	    interrupt_exit(SPLIMP);

	    kr = mach_msg(&msg->msg_hdr, MACH_RCV_MSG,
			  0, 8192, net_reply_port_set,
			  MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

	    interrupt_enter(SPLIMP);

	    if (kr != MACH_MSG_SUCCESS) {
		printf("net_input_thread: mach_msg failed: err = %d\n", kr);
		goto error;
	    }

#if RFC_1323
	    hippi = FALSE; /* XXX - but we run in the net thread */
#endif

	    if (debug_netcode) printf("netisr got pkt\n");

	    /*
	     * Find the interface that the message was received on.
	     */
	    for (es = es_list; es; es = es->es_link)
		if (msg->msg_hdr.msgh_local_port == es->es_reply_port)
		    break;
	    if (es == 0) {
		printf("net_input_thread: unrecognized interface port: %x\n",
					 msg->msg_hdr.msgh_local_port);
		goto error;
	    }

	    ifp = &es->es_if;
/*
	    ifp->if_lastchange.tv_sec = time.tv_sec;	
	    ifp->if_lastchange.tv_usec = time.tv_usec;	
*/
	    if ((ifp->if_flags & IFF_UP) == 0)
		goto error;

#if RFC_1323
	    /*
	     * Determine if this is an out-of-line packet, if so, just copy it
	     * into the inline area, free the allocated space and take
	     * the inline path.
	     */
	    if ((msg->header_type.msgt_longform == TRUE) &&
		(msg->header_type.msgt_inline == FALSE)) {
		/*
		 * Point at the scatter/gather message header/packet and
		 * create an inline message
		 */
		hippi = TRUE;
		sg_msg = (net_rcv_msg_sg_t)msg;
	    }
	    if (!hippi) {
#endif
		/*
		 * Save the ethernet header.  Do not include it
		 * in the data passed to upper levels.
		 */
		eh = (struct ether_header *)&msg->header[0];
		if (debug_netcode) xxx("header:", eh, sizeof(*eh));

		/*
		 * Get the packet address and length.
		 */
		addr = &msg->packet[0] + sizeof(struct packet_header);
		len  = msg->packet_type.msgt_number
			 - sizeof(struct packet_header);
		if (debug_netcode) {
		    printf("len %d,", len);
		    xxx("data:", addr, len);
		}

		if (len == 0)
		    goto error;
#if RFC_1323
	    } else {
		addr = (char *)sg_msg->data;
		len = sg_msg->data_type.msgtl_number;
	    }
	    if (!hippi) {
#endif
		/*
		 * Wrap an mbuf around the data, excluding the
		 * local header.
		 */
		m = mclgetx(net_input_dispose,
			(caddr_t)msg,
			addr,
			len,
			M_WAIT);
		/*
		 * Prepend the interface pointer.  We know that
		 * we will get enough room by clobbering the
		 * ethernet header.
		 */
		if(m == (struct mbuf *)NULL)
			panic("net_input_thread - no mbufs");
		m->m_pkthdr.len = len;
		m->m_flags |= M_PKTHDR;
		m->m_pkthdr.rcvif = ifp;


		/*
		 * Dispatch to protocol.
		 */
		len = ntohs(eh->ether_type);
		eh->ether_type = len;
#if RFC_1323
	    } else {
		/*
		 * Wrap an mbuf around the data.
		 */
		m = mclgetx(net_hippi_dispose,
			addr,
			addr,
			len,
			M_WAIT);
		/*
		 * Prepend the interface pointer.  We know that
		 * we will get enough room by clobbering the
		 * ethernet header.
		 */
		if(m == (struct mbuf *)NULL)
		    panic("net_input_thread - no mbufs");
		m->m_pkthdr.len = len;
		m->m_flags |= M_PKTHDR;
		m->m_pkthdr.rcvif = ifp;

		/*
		 * don't need this anymore just free it now
		 * the dispose routine will vm_deallocate the
		 * real data portion
		 */

		zfree(net_msg_zone, (vm_offset_t)msg);
	    }
#endif
/* XXX trailers XXX */
	    ifp->if_ipackets++;
#if RFC_1323
	    if (!hippi)
#endif
		ether_input(ifp, eh, m);
#if RFC_1323
	    else
		 hippi_input(ifp, m);
#endif
	    continue;
    error:
	    ifp->if_ierrors++;
#if RFC_1323
	    if (hippi)
		vm_deallocate(mach_task_self(), sg_msg->data,
				sg_msg->data_type.msgtl_number);
#endif
	    zfree(net_msg_zone, (vm_offset_t)msg);
	    continue;

	}
	/* NOTREACHED */
}

xxx(s,p,i)
	unsigned char *p;
{
	if (!debug_netcode) return;
	printf("%s", s);
	while (i--)
		printf(" %x", *p++);
	printf("\r\n");
}


net_input_dispose(msg)
	char *	msg;
{
    zfree(net_msg_zone, (vm_offset_t)msg);
}

#if RFC_1323
net_hippi_dispose(msg, len)
	char *  msg;
	u_long  len;
{

	(void) vm_deallocate(mach_task_self(), (vm_offset_t)msg, len);
}
#endif

/*ARGSUSED*/
kern_return_t
ether_write_reply(ifp_p, return_code, count)
	char *		ifp_p;
	kern_return_t	return_code;
	unsigned int	count;
{
}

/*
 * All interface setup is thru IOCTL.
 */
ether_ioctl(ifp, cmd, data)
	register struct ifnet *ifp;
	int	cmd;
	caddr_t	data;
{
	register struct ifaddr *ifa = (struct ifaddr *)data;
	register struct ether_softc *es = (struct ether_softc *)ifp;
	int error = 0;

	simple_lock(&es->es_lock);
	switch (cmd) {
	    /*
	     * Set interface address (and mark interface up).
	     */
	    case SIOCSIFADDR:
		switch (ifa->ifa_addr->sa_family) {
#ifdef INET
		    case AF_INET:
			es->es_ip = IA_SIN(ifa)->sin_addr;
			arpwhohas(&es->es_ac, &IA_SIN(ifa)->sin_addr);
			break;
#endif
#ifdef NS
		    case AF_NS:
		    {
			register struct ns_addr *ina = &(IA_SNS(ifa)->sns_addr);
			if (ns_nullhost(*ina))
			    ina->x_host = *(union ns_host *)es->es_addr;
			else {
			    bcopy(ina->x_host.c_host,
				  es->es_addr,
				  sizeof(es->es_addr));
			    /*
			     * Tell hardware to change address XXX
			     */
			}
			break;
		    }
#endif
		}
		break;

	    default:
		error = EINVAL;
		break;
	}
	simple_unlock(&es->es_lock);
	return (error);
}

ether_error()
{
}

/*
 * Name:
 *	net_interface_name()
 *
 * Function:
 *	Take a net interface device name and 
 *	return the appropriate values for device node
 *	and network server node.  A device node is a
 *	node that has the physical device, e.g. an ethernet
 *	card.  The network server node is the node that is
 *	running the protocol stack software.
 *
 *	Complete network interface names are in the form:
 *
 *	<DV,NS>interfaceunit
 *		DV = device node
 *		NS = network server node
 *		interface = system specific acronym for device type
 *		unit = unit number
 *
 * Inputs:
 *	iname	string address of net interface name
 *	ns_node	int pointer to integer network server node, can be NULL
 *	dv_node int pointer to integer device node, can be NULL
 *
 * Outputs:
 *	Returns string address of interface nameunit.
 *
 *	If *ns_node is a valid pointer and the network server node is 
 *	specified in the complete network name, extract and return it
 *	as an integer value.  If the network server node is not specified
 *	in the complete network name, default it to the device node.
 *
 *	If dv_node is a valid pointer and the device node is specified
 *	in the complete network name, extract and return it as an
 *	integer value.	
 */

char *
net_interface_name(iname, dv_node, ns_node)
	char	*iname;
	int	*dv_node, *ns_node;
{
#if defined(TNC)
	extern node_t	netserver_node;
#endif
	extern long	strtol();	/* POSIX routines now */
	register char	c;
	node_t		dv;
	char		*name;

	/* initialize non-NULL output parameters */
	if ( dv_node != (int *)0 )
		*dv_node = INVALID_NODE;
	if ( ns_node != (int *)0 )
		*ns_node = INVALID_NODE;

	name = iname;

	if ( *name == '<' ) {
		/* first part of prefix is device node */
		if (dv_node != NULL || ns_node != NULL) {
			name++;		/* get past '<' */
			dv = (node_t)strtol(name, &name, 0);	/* asc to bin */
			if(dv_node != NULL)
				*dv_node = dv;
		}
		else {
		  /* comma separates device node and network node */
			while ( (c = *name) != '\0' && (c != ',') &&
				(c != '>') ) name++;
		}

		if (ns_node != NULL) {
		/* extract network server node if it's there */
			if ( (c = *name) == ',') {
			  	name++;	/* get past ',' */
				*ns_node = (int)strtol(name, &name, 0);
			}
			else /* no ns_node defined */
				*ns_node = dv; /* default = dv_node */
		}

		/* find end of prefix */
		while ( (c = *name) != '\0' && (c != '>') ) name++;
		if ( c == '\0' )
			return (0);		/* invalid name */
		name++;	/* skip over '>' */
	}
#if defined(TNC)
	else
	{
		if(dv_node != NULL)
			*dv_node = netserver_node;
		if(ns_node != NULL)
			*ns_node = netserver_node;
	}
#endif
	return name;
}

/*
 * This function strategically plugs into ifunit(), and it is called
 * on a non-existant interface.  We try to look it up, and if successful
 * initialize a descriptor and call if_attach() with it.
 *
 * Name points to the full device name (including unit number).
 */
struct ifnet *
iface_find(name)
	char		*name;
{
	register struct ifnet	*ifp;
	register struct ether_softc *es;
	mach_port_t		if_port;
	mach_port_t		reply_port;
	struct net_status	if_stat;
	unsigned int		if_stat_count;
	char			if_addr[16];	/* XXX */
	unsigned int		if_addr_count;
	kern_return_t		rc;
	int			unit;
#if RFC_1323
	int			hippi_iface;
#endif
#if defined(TNC)
        mach_port_t             ds_port;
        node_t                  dv_node=0;
	node_t			ns_node=0;

        /* get optional MK node address */
        name = net_interface_name( name, &dv_node, &ns_node );
	if (name == NULL)
		return(0);
        if ( dv_node != -1 )
                ds_port = node_to_master_device_port(dv_node);
        else
                ds_port = device_server_port;

        rc = device_open(ds_port,
                         0,             /* mode */
                         name,
                         &if_port);
#else
	rc = device_open(device_server_port,
			 0,		/* mode */
			 name,
			 &if_port);
#endif
	if (rc != D_SUCCESS)
	    return (0);
	/*
	 * If we're opening a HIPPI device then set the buffer
	 * pool. This also effectively sets the MTU.
	 */
#if RFC_1323
#define HIPPI_SET_POOL		(('h'<<16) + 4)
	if (bcmp(name, "ifhip", 5) == 0)	/* a true hack */
		hippi_iface = TRUE;
	else
		hippi_iface = FALSE;

	if (hippi_iface)  {
		int status_word[2];
		status_word[0] = 20;			/* # of buffers */
		status_word[1] = ( 64*1024 );	/* size of buffers */
		rc = device_set_status( if_port,
					HIPPI_SET_POOL,
					&status_word[0],
					2);
		if (rc != D_SUCCESS) {
	    		(void) device_close(if_port);
	    		(void) mach_port_deallocate(mach_task_self(), if_port);
	    		return (0);
		}
	}
#endif

	/*
	 * Get status and address from interface.
	 */
	if_stat_count = NET_STATUS_COUNT;
	rc = device_get_status(if_port,
			NET_STATUS,
			(dev_status_t)&if_stat,
			&if_stat_count);
	if (rc != D_SUCCESS) {
	    (void) device_close(if_port);
	    (void) mach_port_deallocate(mach_task_self(), if_port);
	    return (0);
	}

#if	MAP_ETHER
	if (if_stat.mapped_size) {
		extern	struct ifnet *ether_map();	/* machdep */
		return ether_map(name, if_port, &if_stat);
	}
#endif	MAP_ETHER

	if_addr_count = sizeof(if_addr)/sizeof(int);
	rc = device_get_status(if_port,
			NET_ADDRESS,
			(dev_status_t)if_addr,
			&if_addr_count);
	if (rc != D_SUCCESS) {
	    (void) device_close(if_port);
	    (void) mach_port_deallocate(mach_task_self(), if_port);
	    return (0);
	}

	/*
	 * Byte-swap address into host format.
	 */
	{
	    register int 	i;
	    register int	*ip;

	    for (i = 0, ip = (int *)if_addr;
		 i < if_addr_count;
		 i++,ip++) {
		*ip = ntohl(*ip);
	    }
	}
	/*
	 * Allocate a receive port for the interface.
	 */
	reply_port = mach_reply_port();
	(void) mach_port_move_member(mach_task_self(), reply_port,
				     net_reply_port_set);
	(void) mach_port_set_qlimit(mach_task_self(), reply_port,
				    MACH_PORT_QLIMIT_FOR_ETHERNET);

	/*
	 * Allocate an Ethernet interface structure.
	 */
	es = (struct ether_softc *)malloc(sizeof(struct ether_softc));
	bzero((caddr_t) es, sizeof(struct ether_softc));

	ifp = &es->es_if;

	/*
	 * Save interface name in a safe place.  Caller
	 * has ensured that it is shorter than IFNAMSIZ
	 * and ends in a digit.
	 */
	{
	    register char *dst, *src;
	    register char c;

	    dst = es->es_name;
	    for (src = name; ; src++) {
		c = *src;
		if (c >= '0' && c <= '9')
		    break;
		*dst++ = c;
	    }
	    unit = c - '0';
	}
	ifp->if_name =		es->es_name;
	ifp->if_unit =		unit;
#if defined(TNC) 
	ifp->if_dvnode =	dv_node;
#endif
	ifp->if_mtu =		if_stat.max_packet_size
					- if_stat.header_size;
	ifp->if_flags =		if_stat.flags;

#if RFC_1323
#define HIPPI_RFC_HACK	65312	/* RFC 1374 specifies the HIPPI MTU size */
	if (hippi_iface) {
	    ifp->if_output =    hippi_output;
	    ifp->if_start =     hippi_start;
	    ifp->if_ioctl =     hippi_ioctl;
	    ifp->if_mtu =	HIPPI_RFC_HACK - if_stat.header_size;
	    ifp->if_flags |= IFF_BROADCAST;
	} else {
#endif
	    ifp->if_output =	ether_output;
	    ifp->if_start =	ether_start;
	    ifp->if_ioctl =	ether_ioctl;
#if RFC_1323
	}
#endif

	bcopy(if_addr, es->es_addr, sizeof(es->es_addr));

	es->es_ac.ac_bcastaddr = (u_char *)etherbroadcastaddr;
	ifp->if_type = IFT_ETHER;
	es->es_ac.ac_arphrd = if_stat.header_format;
	ifp->if_hdrlen = if_stat.header_size; 
	ifp->if_addrlen = if_stat.address_size; 

	simple_lock_init(&es->es_lock);
	es->es_port = if_port;
	es->es_reply_port = reply_port;

#if RFC_1323
	if (hippi_iface) {
		extern	void	init_hippi_buf_cache();
		init_hippi_buffer_cache(if_stat.max_packet_size);
	} else
#endif
	{
		mbuf_allocation_zone = zinit(if_stat.max_packet_size,
						2*vm_page_size, 0, 
						"mbuf_allocation_entry");
	}

	/*
	 * Set up a filter to receive packets.  Take all,
	 * for the moment. If the device is ethernet (em).
	 * Otherwise if its hippi (ifhip) just take HIPPI-LE.
	 */
#if RFC_1323
	if (hippi_iface) {
	    filter_t	ifhip_filter[6];

	    ifhip_filter[0] = NETF_PUSHLIT;
	    ifhip_filter[1] = (filter_t)0x00ff;
	    ifhip_filter[2] = (NETF_PUSHHDR + 0) | NETF_AND;
	    ifhip_filter[3] = NETF_PUSHLIT;
	    ifhip_filter[4] = (filter_t)0x0004;
	    ifhip_filter[5] = NETF_NOPUSH | NETF_EQ;

	    rc = device_set_filter(if_port,
				   reply_port,
				   MACH_MSG_TYPE_MAKE_SEND,
				   1,		/* priority */
				   ifhip_filter,
				   sizeof(ifhip_filter)/sizeof(ifhip_filter[0]));
	    if (rc != KERN_SUCCESS)
		printf("device_set_filter: %d\n", rc);
	} else 
#endif
	{
	    filter_t	filter[2];

	    filter[0] = NETF_PUSHLIT;
	    filter[1] = (filter_t)TRUE;

	    rc = device_set_filter(if_port,
				   reply_port,
				   MACH_MSG_TYPE_MAKE_SEND,
				   1,		/* priority */
				   filter,
				   sizeof(filter)/sizeof(filter[0]));
	    if (rc != KERN_SUCCESS)
		printf("device_set_filter: %d\n", rc);
	}

	/*
	 * Attach ether_softc structure to reply_port list.
	 */
	es->es_link = es_list;
	es_list = es;

	if_attach(ifp);
	ifinit();    

	return (struct ifnet *)ifp;
}

/* 
 * This function is called from netinit() to initialise stuff used by the
 * netinput thread - that's why the name was changed from netisr_init to 
 * netinput_init. This function is called only on PM nodes in AD w/o TNC and
 * on all nodes for TNC
 */
void
netinput_init()
{
    kern_return_t	rc;
    net_msg_zone = zinit(8192,
			 1000*8192,
			 10*8192,
			 "incoming network messages");
    
    rc = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_PORT_SET,
			    &net_reply_port_set);
    if (rc != KERN_SUCCESS)
	panic("Allocating net reply port set returns %d", rc);
    
    ux_create_thread(net_input_thread);
}
