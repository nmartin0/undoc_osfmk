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
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: mi_config.c,v $
 * Revision 1.10  1995/02/01  23:15:03  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.9  1994/11/18  20:51:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/05/04  22:08:23  mjl
 * TNC select rewrite.  Use new VS_MALLOC macro here.
 *
 *  Reviewer: Charlie Johnson (Intel), Bob Yasi (Locus)
 *  Risk: Medium
 *  Benefit or PTS #: #7537 + select rewrite
 *  Testing: VSX, EATS, bobtest, Eval
 *  Module(s):
 * 	server/bsd/subr_select.c
 * 	server/sys/select.h
 * 	server/sys/socketvar.h
 * 	server/sys/user.h
 * 	server/tnc/un_debug.c
 * 	server/tnc/un_debug.h
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/bsd_server_side.c
 * 	server/uxkern/fsvr.defs
 * 	server/uxkern/fsvr2_server_side.c
 * 	server/uxkern/fsvr_port.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/port_hash.c
 * 	server/uxkern/port_hash.h
 * 	server/vsocket/mi_config.c
 * 	server/vsocket/sys_vsocket.c
 * 	server/vsocket/two_way_hash.h
 * 	server/vsocket/vs.defs
 * 	server/vsocket/vs_chouse.c
 * 	server/vsocket/vs_debug.c
 * 	server/vsocket/vs_init.c
 * 	server/vsocket/vs_ipc.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/vs_subr.c
 * 	server/vsocket/vs_subr.h
 * 	server/vsocket/vs_types.h
 * 	server/vsocket/vsocket.h
 *
 * Revision 1.7  1994/03/03  19:26:20  slk
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 7016 merge from R1.2
 *  Testing: build and boot
 *  Module(s):
 *
 * Revision 1.6.2.1  1994/03/01  02:35:31  yazz
 *  Reviewer: Bernie Keany
 *  Risk: lo
 *  Benefit or PTS #: #7016
 *  Testing: extensive
 *  Module(s): server/vsocket/mi_config.c
 *
 * Properly handle SIOCSIFFLAGS ioctl command when configuring mi interfaces.
 *
 * Revision 1.6  1993/10/28  03:17:18  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.5  1993/09/14  15:57:17  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.4.2.1  1993/09/14  15:55:51  cfj
 * Get rid of i860 compiler warnings.
 *
 * Revision 1.4  1993/09/01  01:39:48  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.3  1993/07/14  18:48:20  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.2  1993/07/01  21:12:59  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1993/05/20  16:04:09  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 3.7  93/08/26  10:51:19  mjl
 * Add decl and initialization of MI statistics data.
 * 
 * Revision 3.6  93/08/20  19:01:58  mjl
 * Update slightly stale comment.
 * 
 * Revision 3.5  93/07/27  11:42:11  mjl
 * [Bug #0273] Ignore EEXIST errors when adding network MI routes, but panic
 * if EEXIST occurs when adding host MI routes.
 * 
 * Revision 3.4  93/05/25  10:42:10  mjl
 * Fix up comments.  Sockmask length is not a reliable indicator of whether
 * a netmask was used to configure the device, since in_ifinit() sets up a
 * sockmask no matter what.  Be lenient on EEXIST errors when adding MI routes.
 * 
 * Revision 3.3  93/05/13  11:38:34  mjl
 * Make sure mi_answer_advertisement() doesn't advertise ifa's that have no
 * address family (these can be created when ifconfig is used to set flags
 * without also setting the interface address).  [LCC bug #0256]
 * 
 * Revision 3.2  93/05/07  19:10:16  mjl
 * Fix debugging printfs.  Remove redundant setting of RTF_INVISIBLE flag
 * (it should be done only in mi_rtrequest() ).
 * 
 * Revision 3.1  93/05/07  15:11:32  nina
 * Modified mi_autoconfig() to use new chouse_walk_servers()
 * interface
 * 
 * Revision 3.0  93/05/05  22:16:43  mjl
 * Autoconfiguration code for MIv3 network interfaces.
 * 
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/errno.h"

#include "uxkern/import_mach.h"
#include "kern/zalloc.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#include "sys/socketvar.h"
#include "sys/protosw.h"
#include "sys/uio.h"
#include "sys/ioctl.h"

#include "net/net_globals.h"
#include "net/route.h"
#include "net/if.h"
#include "net/if_types.h"
#include "netinet/in.h"

#include "vsocket/vs_subr.h"
#include "vsocket/vs_chouse.h"
#include "vsocket/mi_var.h"

#define Register	/* register /**/
#define Static		/* static /**/

extern node_t		this_node;
extern mach_port_t	inetserver_port;

/*
 *  Globals
 */
struct mutex		mi_list_mtx	= MUTEX_INITIALIZER;
struct mi_assoc		*mi_assoc_list	= NULL;
struct mi_softc		*mi_list	= NULL;
MISTAT_DECL;

/*
 *  Forward declarations
 */
int	do_advertise_iface(mach_port_t, node_t, caddr_t);
int	mi_configure(struct mi_softc *, int, struct ifreq *, iface_info_t *);
int	mi_answer_advertisement(struct mi_assoc *);
struct mi_assoc *mi_create_assoc(node_t, mach_port_t, int *);

/* Compare two sockaddrs */
#define	sa_equal(a1, a2) \
  (bcmp((caddr_t)(a1), (caddr_t)(a2), ((struct sockaddr *)(a1))->sa_len) == 0)

/*
 *  We are about to autoconfigure an MI interface on this node, so do
 *  all the per-node initialization.
 */
void
mi_init_node()
{
	static int initialized = 0;
	extern int ipforwarding, ipgateway;

	if (initialized)
		return;
	initialized = 1;

	MIDEBUG(MD_CONFIG|MD_TRACE, ("mi_init_node\n"));

	MISTAT_INIT;
	mi_msg_zone = zinit(MI_ALLOC, 100*MI_ALLOC, MI_ALLOC,
			    "if_mi network msgs");

	if (ipforwarding || ipgateway)
	    printf("WARNING: No MI support for IP forwarding (yet)!\n");
}


struct argblock {
	int		ab_cmd;
	struct ifreq	*ab_ifr;
	int		ab_ifid;
	iface_info_t	*ab_ii;
	node_t		ab_node;
	mach_port_t	ab_port;
};

/*
 *  MI driver autoconfiguration.  Called as a side effect of
 *  if_addr_update(), the network server clearinghouse check-in
 *  routine.
 */
/*ARGSUSED*/
int
mi_autoconfig(
	chouse_t	*chs,
	chouse_key_t	*ckp,
	int		cmd,
	struct ifreq	*ifr)
{
	struct argblock	args;
	iface_info_t	*iip;
	int		rc;

	/* XXX Temporary!!!  We should support other ioctls soon. */
	if ( !(cmd == SIOCSIFADDR || cmd == SIOCAIFADDR || cmd == SIOCSIFFLAGS))
		return (ESUCCESS);

	iip = chouse_find_key(chs, ckp, /*create*/0, &rc);
	if (iip == NULL)
		panic("mi_autoconfig: key af=%d/id=0x%x/node=%d: no i/f: %d\n",
		      ckp->ck_af, ckp->ck_id, ckp->ck_node, rc);

	ASSERT(iip->ii_server && iip->ii_server->netserv_port);

	args.ab_cmd = cmd;
	args.ab_ifr = ifr;
	args.ab_ifid = ckp->ck_id;
	args.ab_ii = iip;
	args.ab_node = ckp->ck_node;
	args.ab_port = iip->ii_server->netserv_port;
	return chouse_walk_servers(chs,
				   do_advertise_iface,
				   (caddr_t)&args,
				   TRUE /* abort on error */);
}


/*
 *  Inform a network server node "nsnode" that a network interface has
 *  been configured (or its address info modified) on ab->ab_node.
 */
Static int
do_advertise_iface(
	mach_port_t	nsport,
	node_t		nsnode,
	caddr_t		args)
{
	Register struct argblock	*ab = (struct argblock *)args;
	kern_return_t	kr;
	int		rc;

	/*
	 *  If the interface was configured on the node we would tell,
	 *  then there is no need to tell it.
	 */
	if (ab->ab_node == nsnode)
		return (ESUCCESS);

	ux_server_thread_blocking();
	kr = r_mi_advertise_iface(nsport,
				  ab->ab_ifid,
				  ab->ab_node,
				  ab->ab_port,
				  ab->ab_cmd,
				  ab->ab_ifr,
				  ab->ab_ii,
				  &rc);
	ux_server_thread_unblocking();
	if (kr != KERN_SUCCESS)
		panic("do_advertise_iface: mi_advertise_iface: kr 0x%x\n", kr);
	return (rc);
}


/*
 *  The server side of the mi_advertise_iface RPC.  This runs on a
 *  network server node to inform it about a change in a non-local
 *  interface's addressing information.  Based on the new info, we
 *  update the corresponding local MI interface.  If a local MI
 *  interface does not exist, then one is created.
 *
 *  This RPC is employed in two scenarios: (a) initiated by a confirmed
 *  update to the clearinghouse, to tell existing network server nodes
 *  of a newly configured ns node, and (b) initiated by an existing ns
 *  node itself to tell the newly configured ns node about interfaces
 *  on the existing ns node.
 *
 *  NB Right now mi_autoconfig() only passes SIOCAIFADDR or SIOCSIFADDR
 *  ioctls, so for now we always create a new MI interface here.  Eventually
 *  we want to track *all* addressing-related interface ioctls.  XXX
 *  (Therefore don't be alarmed that the data in the ifr and the iip is
 *  largely redundant for SIOCAIFADDR --- for other ioctls it won't be.)
 *
 *  Input Parameters:
 *	server_port	-- The usual RPC binding handle.
 *	iface_id	-- The address of the remote interface in the
 *				remote server address space, used to
 *				locate the corresponding local MI interface.
 *	netserv_node	-- Node where remote interface resides.
 *	netserv_port	-- Send-right leading to netserv_node.
 *	cmd		-- Ioctl command that caused this advertisement.
 *	ifr		-- Interface ioctl request data.
 *	iip		-- Interface info as stored in the clearinghouse,
 *				after ioctl was processed.
 */
kern_return_t
S_mi_advertise_iface(
	mach_port_t	server_port,
	int		iface_id,
	node_t		netserv_node,
	mach_port_t	netserv_port,
	int		cmd,
	struct ifreq	*ifr,
	iface_info_t	*iip,
	int		*rval)
{
	struct mi_softc	*sc;
	struct mi_assoc	*ma;
	int		rc;
	extern char	*sockaddr_to_string();
	int		new_assoc = FALSE;

	MIDEBUG(MD_CONFIG|MD_TRACE,
		("ifioctl 0x%x (i/f addr %s) happened on node %d\n",
		 cmd, sockaddr_to_string(&ifr->ifr_addr), netserv_node));
	
	mi_init_node();
	*rval = ESUCCESS;

	/*
	 *  Look for an existing association (MI link) to the new
	 *  interface's node.  If none exists, one must be created.
	 */
	for (ma = mi_assoc_list; ma; ma = ma->ma_next)
		if (ma->ma_remote_node == netserv_node)
			break;
	if (ma == NULL) {
		ma = mi_create_assoc(netserv_node, netserv_port, rval);
		if (ma == NULL)
			/* *rval has been set */
			return(KERN_SUCCESS);
		new_assoc = TRUE;
	}

	/*
	 *  We have the association to use, now create the MI interface
	 *  that will act as the surrogate for the remote real interface.
	 */
	for (sc = ma->ma_comrads; sc; sc = sc->sc_comrads)
		if (sc->sc_iface_id == iface_id)
			break;
	if (sc == NULL) {
		VS_MALLOC(sc, struct mi_softc *, sizeof(struct mi_softc),
			  VSM_MISOFTC);
		if (sc == NULL) {
			*rval = ENOMEM;
			return (KERN_SUCCESS);
		}
		bzero(sc, sizeof(struct mi_softc));
		sc->sc_iface_id = iface_id;
		sc->sc_assoc = ma;
		(void) mi_iface_init((struct ifnet *)sc, ifr->ifr_name, iip);

		/*
		 *  Insert new MI interface into the various lists
		 */
		mutex_lock(&mi_list_mtx);
		sc->sc_comrads = ma->ma_comrads;
		ma->ma_comrads = sc;
		sc->sc_next = mi_list;
		mi_list = sc;
		mutex_unlock(&mi_list_mtx);
	}

	/*
	 *  Apply the same ioctl done on the real remote interface
	 *  to the local MI surrogate interface.
	 */
	*rval = mi_configure(sc, cmd, ifr, iip);
	if (*rval != ESUCCESS)
		return (KERN_SUCCESS);

	/*
	 *  If a new association was created, we need to tell the
	 *  new network server node about local network interfaces
	 *  so that appropriate routes can be added on that node.
	 */
	if (new_assoc)
		*rval = mi_answer_advertisement(ma);

	return (KERN_SUCCESS);
}


struct mi_assoc *
mi_alloc_assoc(
	node_t	node,
	int	*rval)
{
	struct mi_assoc	*ma;
	kern_return_t	kr;

	*rval = ESUCCESS;

	/*
	 *  Create local MI association
	 */
	VS_MALLOC(ma, struct mi_assoc *, sizeof(struct mi_assoc), VSM_MIASSOC);
	if (ma == NULL) {
		*rval = ENOMEM;
		return (NULL);
	}
	ma->ma_remote_node = node;
	ma->ma_next = NULL;
	ma->ma_comrads = NULL;
	ma->ma_ctrl_port = ma->ma_data_port = MACH_PORT_NULL;

	/*
	 *  Set up associated ports, one for control messages and
	 *  "for all network servers" style operations, and another
	 *  for actual data transmission.  Create one send-right
	 *  for each, so we can exchange send-rights with the other
	 *  side of the association.
	 */
	kr = mach_port_allocate_name(mach_task_self(),
				     MACH_PORT_RIGHT_RECEIVE,
				     MI_DATAPORT(ma));
	if (kr != KERN_SUCCESS) {
	    panic("mi_alloc_assoc: dataport ...allocate_name(0x%x) kr 0x%x\n",
		  MI_DATAPORT(ma), kr);
	}
	kr = mach_port_insert_right(mach_task_self(),
				    MI_DATAPORT(ma), MI_DATAPORT(ma),
				    MACH_MSG_TYPE_MAKE_SEND);
	if (kr != KERN_SUCCESS)
		panic("mi_alloc_assoc: data send-right, ma 0x%x, kr 0x%x\n",
		      ma, kr);

	/*
	 *  ...and now the control port...
	 */
	kr = mach_port_allocate_name(mach_task_self(),
				     MACH_PORT_RIGHT_RECEIVE,
				     MI_CTRLPORT(ma));
	if (kr != KERN_SUCCESS) {
	    panic("mi_alloc_assoc: ctrlport ...allocate_name(0x%x) kr 0x%x\n",
		  MI_CTRLPORT(ma), kr);
	}
	kr = mach_port_insert_right(mach_task_self(),
				    MI_CTRLPORT(ma), MI_CTRLPORT(ma),
				    MACH_MSG_TYPE_MAKE_SEND);
	if (kr != KERN_SUCCESS)
		panic("mi_alloc_assoc: ctrl send-right, ma 0x%x, kr 0x%x\n",
		      ma, kr);

	return (ma);
}


mach_port_t	mi_data_port_set = MACH_PORT_NULL;

/*
 *  Currently we have one MI port set and one thread per node, and we
 *  add a new MI data port to the set for each new association.
 */
mi_assoc_enable(
	struct mi_assoc	*ma)
{
	kern_return_t	kr;
	extern mi_isr_thread();

	MIDEBUG(MD_TRACE|MD_CONFIG, ("mi_assoc_enable: assoc 0x%x\n", ma));

	mutex_lock(&mi_list_mtx);
	ma->ma_next = mi_assoc_list;
	mi_assoc_list = ma;
	mutex_unlock(&mi_list_mtx);

	ux_server_add_port(MI_CTRLPORT(ma));
	if (mi_data_port_set == MACH_PORT_NULL) {
		kr = mach_port_allocate(mach_task_self(),
					MACH_PORT_RIGHT_PORT_SET,
					&mi_data_port_set);
		if (kr != KERN_SUCCESS)
			panic("mi_assoc_enable: port set create: kr 0x%x\n",
			      kr);
		ux_create_thread(mi_isr_thread);
	}
	kr = mach_port_move_member(mach_task_self(),
				   MI_DATAPORT(ma),
				   mi_data_port_set);
	if (kr != KERN_SUCCESS)
		panic("mi_assoc_enable: move member: kr 0x%x\n", kr);
}


/*
 *  Pass local MI port rights to remote side of MI link, get back
 *  rights for remote side's ports, and begin listening on them.
 */
struct mi_assoc *
mi_create_assoc(
	node_t		node,
	mach_port_t	port,
	int		*rval)
{
	struct mi_assoc	*ma;
	int		rc;
	kern_return_t	kr;

	MIDEBUG(MD_CONFIG|MD_TRACE,
		("mi_create_assoc: %d --> %d\n", this_node, node));

	ma = mi_alloc_assoc(node, rval);
	if (ma == NULL)
		/* *rval has already been set */
		return (NULL);

	/*
	 *  Now ship the association send-rights to the remote node.
	 *  An association structure will be created there, and its
	 *  control and data send-rights will be shipped back.
	 */
	ux_server_thread_blocking();
	kr = r_mi_create_link(port,
			      this_node,
			      MI_CTRLPORT(ma),
			      MI_DATAPORT(ma),
			      &ma->ma_ctrl_port,
			      &ma->ma_data_port,
			      rval);
	ux_server_thread_unblocking();
	if (kr != KERN_SUCCESS)
		panic("mi_create_assoc: r_mi_create_link: kr 0x%x\n", kr);
	if (*rval != ESUCCESS) {
		MIDEBUG(MD_CONFIG,("mi_create_assoc (%d --> %d) failed: %d\n",
				   this_node, node, *rval));
		return (NULL);
	}

	mi_assoc_enable(ma);
	return (ma);
}


kern_return_t
S_mi_create_link(
	mach_port_t	server_port,
	node_t		client_node,
	mach_port_t	control_port,
	mach_port_t	data_port,
	mach_port_t	*remote_ctrl_port,
	mach_port_t	*remote_data_port,
	int		*rval)
{
	struct mi_assoc	*ma;

	MIDEBUG(MD_CONFIG|MD_TRACE,
		("S_mi_create_link: %d --> %d\n", this_node, client_node));

	mi_init_node();

	if ( (ma = mi_alloc_assoc(client_node, rval)) != NULL ) {
		ma->ma_ctrl_port = control_port;
		ma->ma_data_port = data_port;
		*remote_ctrl_port = MI_CTRLPORT(ma);
		*remote_data_port = MI_DATAPORT(ma);
		mi_assoc_enable(ma);
	}

	MIDEBUG(MD_CONFIG|MD_TRACE,
		("S_mi_create_link: node %d assoc 0x%x, rval %d\n",
		 this_node, ma, *rval));
	return (KERN_SUCCESS);
}


int
mi_configure(
	struct mi_softc	*sc,
	int		cmd,
	struct ifreq	*ifr,
	iface_info_t	*iip)
{
	struct socket	sock;
	struct socket	*so = &sock;
	int		af;
	int		rc;
	int		create;
	struct ifaddr	*ifa;
	struct rtentry	*save_rt;

	MIDEBUG(MD_TRACE|MD_CONFIG,
		("mi_configure(sc=0x%x, cmd=0x%x, ifr=0x%x, ifp=0x%x)\n",
		 sc, cmd, ifr, iip));

	create = (cmd == SIOCAIFADDR || cmd == SIOCSIFADDR);
	af = iip->ii_addr.sa_family;
	ASSERT(af != AF_UNSPEC);
	if (af != AF_INET)
		return (EPFNOSUPPORT);

	/*
	 *  XXX We know empirically that interfaces being given
	 *  AF_INET addresses use SOCK_DGRAM/IPPROTO_UDP sockets to do
	 *  the interface ioctls.  However, it appears that to be
	 *  correct we must store the socket type and protocol number
	 *  of this socket in the clearinghouse, and pass it here via
	 *  iip.  When that is done, I think we no longer need to
	 *  include "netinet/in.h" above.
	 */

	/*
	 *  Fake up a socket so we can call the proper PRU_CONTROL routine.
	 *  Note we can't just call socreate(), because that would require a
	 *  u-area and dummy proc structure, which we don't have.  We only
	 *  need enough of a socket to get us the correct PRU_CONTROL code.
	 */
	bzero(so, sizeof(struct socket));
	so->so_proto =
		(struct protosw *)pffindproto(af, IPPROTO_UDP, SOCK_DGRAM);
	if (so->so_proto == NULL) {
		MIDEBUG(MD_ERR|MD_CONFIG,
			("mi_configure: no UDP/DGRAM proto for af %d\n", af));
		return (EPROTONOSUPPORT);
	}
	so->so_state = SS_PRIV;

	/*
	 *  Do the interface ioctl.  For ioctls that configure a new
	 *  interface, this adds a network route for the interface's
	 *  network.  This will fail with EEXISTS if there is a local
	 *  interface attached to the same network.  That's OK, we can
	 *  still add a host route to the remote interface's address.
	 */
	switch(cmd) {
		case SIOCSIFFLAGS:
			rc = ifioctl(so, cmd, (caddr_t)ifr);
			break;
		default:
			rc = (*sock.so_proto->pr_usrreq)(so, PRU_CONTROL, cmd,
					ifr, sc);
			break;
	}

	if (rc != ESUCCESS) {
		if (rc == EEXIST && create) {		    
			ASSERT(ifa_ifwithnet(&iip->ii_addr) != NULL);
			MIDEBUG(/*MD_INFO, XXX -mjl*/ MD_ERR|MD_CONFIG,
				("mi_configure: no network route added: %s\n",
				 sockaddr_to_string(&iip->ii_addr)));
			rc = ESUCCESS;
		} else {
			MIDEBUG(MD_ERR|MD_CONFIG,
				("mi_configure: PRU_CONTROL af=%d: err %d\n",
				 af, rc));
		}
	}

	/*
	 *  If no new MI interface was configured as a result of this
	 *  ioctl, we are finished.
	 */
	if ( ! create || rc != ESUCCESS )
		return (rc);

	/*
	 *  Find out if the route added by the PRU_CONTROL was a host
	 *  route.  If so, we do not need to create an additional one
	 *  here.
	 */
	if ( (ifa = ifa_ifwithaddr(&iip->ii_addr)) == NULL )
		panic("mi_configure: no ifa for %s\n",
		      sockaddr_to_string(&iip->ii_addr));
	if (ifa->ifa_rt && (ifa->ifa_rt->rt_flags & RTF_HOST)) {
		MIDEBUG(MD_CONFIG,
			("mi_configure: i/f %s is host route only\n",
			 sockaddr_to_string(&iip->ii_addr)));
		return (ESUCCESS);
	}

	/*
	 *  A new MI interface was configured.  It has a direct network
	 *  route, but we need to give it a direct, zero-hop host route
	 *  as well (cf. MIv3 design document).
	 *
	 *  Note we use the remote real interface's address as the gateway
	 *  for the route, as well as for the destination.  This causes
	 *  rtrequest() to attach the MI interface to the route, as desired.
	 *  However, the IP layer will use this address to fill in the
	 *  source address for packets without them, and that's bad, since
	 *  if src == dst no reply can be returned.  This problem is dealt
	 *  with in the mi_rtrequest() hook.
	 */
	rc = rtrequest(RTM_ADD,
		       &iip->ii_addr,	/*dest*/
		       &iip->ii_addr,	/*gateway*/
		       NULL,		/*netmask*/
		       RTF_HOST|RTF_UP,
		       &sc->sc_host_rt);
	if (rc != ESUCCESS) {
		if (rc == EEXIST) {
			MIDEBUG(MD_ERR, /*MD_INFO XXX*/
				("mi_configure: no host route added for %s\n",
				 sockaddr_to_string(&iip->ii_addr)));
			panic("Got EEXIST while adding host route!\n");
			rc = ESUCCESS; /*XXX*/
		} else {
			MIDEBUG(MD_ERR|MD_CONFIG,
				("%s: rtrequest(ADD,dst=%s,HOST): err %d\n",
				 "mi_configure",
				 sockaddr_to_string(&iip->ii_addr), rc));
		}
	}
	return (rc);
}


/*
 *  Advertise all local hardware interfaces to the network server
 *  node on the other side of an MI association.
 */
int
mi_answer_advertisement(
	struct mi_assoc		*ma)
{
	struct ifnet		*ifp;
	struct ifaddr		*ifa;
	kern_return_t		kr;
	int			rc;
	iface_info_t		ifinfo;
	struct ifaliasreq	ifalias;

	MIDEBUG(MD_TRACE|MD_CONFIG,
	       ("mi_answer_ad(ma=0x%x) to node %d\n", ma, ma->ma_remote_node));

	/*
	 *  Send address information for each real local interface.
	 *  On the other side of the link, this establishes the MI
	 *  surrogate interfaces and corresponding IP routes for the
	 *  local interfaces.
	 */
	for (ifp = ifnet; ifp; ifp = ifp->if_next) {
		/* Ignore MI interfaces. */
		if (ifp->if_type == IFT_MI)
			continue;
		for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next) {
			/*
			 *  Ignore link level addresses.  (It's also
			 *  possible to get an ifaddr with unspecified
			 *  address family, as when ifconfig is used
			 *  to set i/f flags without configuring an
			 *  address, e.g. "ifconfig '<0>lo0' up".)
			 *  Only addresses registered with the
			 *  clearinghouse should be advertised here.
			 */
			if (ifa->ifa_addr->sa_family == AF_LINK ||
			    ifa->ifa_addr->sa_family == AF_UNSPEC)
				continue;

			/*
			 *  Using these ifnet and ifaddr structures,
			 *  create arguments needed to advertise them.
			 */
			ifinfo.ii_id		= (int) ifp;
			ifinfo.ii_flags		= ifp->if_flags;
			ifinfo.ii_addr		= *ifa->ifa_addr;
			ifinfo.ii_sockmask	= *ifa->ifa_netmask;
			ifinfo.ii_dstaddr	= *ifa->ifa_dstaddr;
			ifalias.ifra_addr	= *ifa->ifa_addr;
			ifalias.ifra_mask	= *ifa->ifa_netmask;
			ifalias.ifra_broadaddr	= *ifa->ifa_dstaddr;
			if (ifp->if_dvnode != this_node) {
				sprintf(ifalias.ifra_name, "<%d,%d>%s%d",
					ifp->if_dvnode,
					this_node,
					ifp->if_name,
					ifp->if_unit);
			} else {
				sprintf(ifalias.ifra_name, "<%d>%s%d",
					this_node,
					ifp->if_name,
					ifp->if_unit);
			}

			MIDEBUG(MD_CONFIG,
				("  mi_answer_ad: ifp 0x%x (%s) %s\n",
				 ifp, ifp->if_name,
				 sockaddr_to_string(ifa->ifa_addr)));

			/*
			 *  Advertise this interface/address combo to
			 *  the other side of the link.
			 */
			ux_server_thread_blocking();
			kr = r_mi_advertise_iface(ma->ma_ctrl_port,
						  (int) ifp,
						  this_node,
						  inetserver_port,
						  SIOCAIFADDR,
						  &ifalias,
						  &ifinfo,
						  &rc);
			ux_server_thread_unblocking();
			if (kr != KERN_SUCCESS)
			    panic("%sma 0x%x, ifp 0x%x, kr 0x%x\n",
				  "mi_answer_ad: advertise_iface: ",
				  ma, ifp, kr);
			if (rc != ESUCCESS) {
			    MIDEBUG(MD_ERR,
				    ("%sma 0x%x, ifp 0x%x, errno %d\n",
				     "mi_answer_ad: advertise_iface: ",
				     ma, ifp, rc));
			    return (rc);
			}
		}
	}
	return (ESUCCESS);
}
