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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: un_pp_vsops.c,v $
 * Revision 1.10  1995/02/01  22:08:52  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.9  1994/11/18  20:45:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1993/09/01  01:37:30  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.7  1993/07/14  18:36:24  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.5  1993/07/01  20:50:20  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:28:47  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/05/03  17:48:16  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:10:02  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.1  1993/02/16  20:06:53  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/15  02:03:25  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.2  1992/11/06  20:32:32  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 3.5  93/08/23  00:07:38  mjl
 * Get rid of i860 compiler warnings.
 * 
 * Revision 3.4  92/12/11  14:57:50  mjl
 * No more VSOP_VSOCK{CREATE,DESTROY}() virtual socket operations.
 *
 * Revision 3.3  92/10/27  17:40:49  bhk
 * Aligned the virtual socket operations callout table with the new
 * virutal socket operations (userreq -> getaddr, added soreserve)
 * 
 * Revision 3.2  92/08/12  15:14:40  jdh
 * most of the pipe vsops are the same as stream socket vsops and
 * are contained in un_vsops.c.  A few vsops are handled differently
 * for pipes, and those are contained in here, along with the
 * un_pp_vsops vsocket_ops array.
 * -- jdh
 * 
 * Revision 3.1  92/06/30  17:21:42  mjl
 * Fix cleanup of file and socket structures left behind after successful
 * pipe relocation.  The sockinfo structure is now reference counted.
 * 
 * Revision 3.0  92/06/26  17:58:29  mjl
 * UNix PiPe virtual socket operations.
 * 
 * Revision 3.7  92/06/16  18:41:52  bhk
 * moved SOCKET_LOCK before INCREMENT_VS_REFCNT macro
 * 
 * Revision 3.6  92/06/15  17:39:10  mjl
 * In virtual operations that create a socket, get so->vs_fport value from
 * uthread area.
 * 
 * Revision 3.5  92/06/11  16:28:50  mjl
 * Remove bogus assertion on vs_seqno in ust_soconnect2().
 * Call select_wakeup() before relocating().  This effectively places select
 * queue entries in the file port's receive queue; they are then reprocessed
 * once the receive-right arrives on the new node.  Rewrote ust_relocate()
 * for clarity; also, deallocate local resources if relocation succeeds.
 * 
 * Revision 3.4  92/05/06  13:10:48  bhk
 * added sosleep as AF_UNIX virtual socket sleep operation
 * 
 * Revision 3.3  92/04/22  17:00:16  bhk
 * Added support for virtual getsockname call
 * 
 * Revision 3.2  92/04/06  11:01:09  mjl
 * Moved common ust definitions to ust.h.  New {ENTER,LEAVE}_UST_VSOP() macroes.
 * New VSOP_RELOCATE() virtual socket operation.  We can now successfully wait
 * for all outstanding threads operating on a socket pair to complete prior to
 * beginning an actual relocation.
 * 
 * Revision 3.1  92/03/20  17:06:47  bhk
 * moved vsocket.h to vsocket directory
 *  
 * Revision 3.0  92/03/18  17:18:49  mjl
 * Virtual socket layer for AF_UNIX SOCK_STREAM sockets.
 * 
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mbuf.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <vsocket/vsocket.h>
#include <sys/socketvar.h>
#include <sys/protosw.h>
#include <sys/user.h>
#include <sys/errno.h>
#include <sys/file.h>

#include <tnc/un.h>

extern int un_socreate();
extern int un_soclose();
extern int un_sosend();
extern int un_sorecv();
extern int un_pp_sodequeue();
extern int un_solisten();
extern int un_soconnect();
extern int un_soconnect2();
extern int un_sobind();
extern int un_soshutdown();
extern int un_sosetopt();
extern int un_sogetopt();
extern int un_notify();
extern int un_relocate();
extern int un_sogetaddr();
extern int un_sosleep();
extern int un_pp_vsockconnect(struct socket *, mach_port_t);
extern int vs_noop();
extern int un_pp_soreserve(struct socket *, u_long, u_long);


struct vsocket_ops un_pp_vsops = {
	un_socreate,
	un_soclose,
	un_sosend,
	un_sorecv,
	un_pp_sodequeue,
	un_solisten,
	un_soconnect,
	un_soconnect2,
	un_sobind,
	un_soshutdown,
	un_sosetopt,
	un_sogetopt,
	un_notify,
	un_relocate,
	un_sogetaddr,
	un_sosleep,
	vs_noop,
	un_pp_soreserve
};

		
int
un_pp_soreserve(
	struct socket	*so,
	u_long		sendcc,
	u_long		recvcc)
{
	/* no op */
	panic("un_pp_soreserve");
}

int
un_pp_sodequeue(head, so, nam, compat_43)
struct socket *head; 
struct socket **so;
struct mbuf **nam;
int compat_43;
{
	/* no op */
	panic("un_pp_sodequeue");
}


int
un_pp_vsockconnect(
	struct socket *so,		/* connecting socket */
	mach_port_t sockid)		/* socket port for listening socket */
{
	/* should never be called by pipe sockets */
	panic("un_pp_vsockconnect");
}
