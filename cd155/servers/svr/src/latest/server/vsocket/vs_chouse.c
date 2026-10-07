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
 * $Log: vs_chouse.c,v $
 * Revision 1.14  1995/02/01  23:21:08  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.13  1994/12/23  01:38:42  nina
 *  Reviewer:hobbes
 *  Risk:Medium
 *  Benefit or PTS #:10356
 *  Testing:Test case and TCP/IP EATS with various network configs
 *  Module(s):
 * 	net/rtsock.c
 * 	netinet/in_proto.c
 * 	vsocket/vs_chouse.h
 * 	vsocket/vs_chouse.c
 * 	vsocket/vs.defs
 * 	vsocket/vs_ipc.c
 * 	vsocket/vs_subr.c
 * 	vsocket/vs_netops.c
 *
 *   The problem was only seen in systems with multiple network configurations
 *   when a user attempted to bind to INADDR_ANY/port 0. In this
 *   situation, the system is supposed to pick the port number for
 *   the caller. The problem occurred because the VSOCKET code
 *   didn't ensure that the primary socket and each secondary socket used
 *   the same port number.
 *
 *   Now, a bind to port 0 results in an RPC to the clearinghouse to
 *   pick a port number, based on the domain and protocol of the socket.
 *
 * Revision 1.12  1994/11/18  20:52:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/05/04  22:15:17  mjl
 * Merge revision 1.9.2.2 from R1_2 branch into main trunk.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.10  1994/03/03  19:26:53  slk
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 7016 merge from R1.2
 *  Testing: build and boot
 *  Module(s):
 *
 * Revision 1.9.2.2  1994/04/27  23:04:22  yazz
 *  Reviewer: Charlie Johnson, Bob Yasi
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
 * TNC select rewrite.  Use new VS_MALLOC macro here.
 *
 * Revision 1.9.2.1  1994/03/01  02:41:51  yazz
 *  Reviewer: Bernie Keany
 *  Risk: lo
 *  Benefit or PTS #: #7016
 *  Testing: extensive
 *  Module(s): server/vsocket/vs_chouse.c
 *
 * Have the clearinghouse handle the SIOCSIFFLAGS ioctl command.
 *
 * Revision 1.9  1993/09/20  23:58:38  cfj
 * Merge R1.1 bug fixes into main stem.
 *
 * Revision 1.8  1993/09/14  15:37:09  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.7.2.2  1993/09/20  23:49:39  cfj
 * Bug fix for PTS #6663.  The correct netserver is now chosen.
 *
 * Revision 1.7.2.1  1993/09/14  15:35:32  cfj
 * Correctly delete the clearinghouse entry in chouse_addr_reset().
 * Configuring two interfaces on the same node and the same network no longer
 * causes the bootstrap to hang.  Note that the "File exists" error is
 * legitimate for this case.  Fix for PTS bug #6492.
 *
 * Revision 1.7  1993/09/01  01:40:40  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.6  1993/07/14  18:48:53  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  21:13:35  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/20  16:04:31  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 3.11  93/08/17  19:33:24  mjl
 * [Bug #0349] Make chouse_list(), if_list(), etc. indicate a local network
 * server by returning MACH_PORT_NULL in ports[0].  This keeps local server
 * info in the first element of the rvs chain, as expected by lots of code.
 * 
 * Revision 3.10  93/06/11  12:13:46  slively
 * Fixed bogus argument passing to sockaddr_to_string function.
 * No bug number assigned.
 * 
 * Revision 3.9  93/05/26  07:06:37  mjl
 * Set the errno in all return paths in chouse_find_key().  LCC bug #0277.
 * 
 * Revision 3.8  93/05/13  11:40:56  mjl
 * Fix NULL pointer deref in chouse_addr_update().  [LCC bug #0256]  Also,
 * deallocate network server port if initial check-in fails.
 * 
 * Revision 3.7  93/05/07  19:16:28  mjl
 * If no address family is specified, have chouse_find_key() search all address
 * families for a particular interface id.  In chouse_addr_update(), tighten
 * the criteria for considering a particular update to be a "create".
 * 
 * Revision 3.6  93/05/07  15:26:00  nina
 * Major rewrite of chouse_add_netserv(), chouse_list(), 
 * chouse_walk_servers(), to support the new MI driver,
 * and make use of common code.
 * 
 * Revision 3.5  93/05/05  22:40:31  mjl
 * Major rewrite for MIv3.  Move common lookup/insert code to
 * chouse_find_key().  Changes in preparation for tracking all
 * addressing-related ifioctl() calls, not just SIOCSIFADDR and
 * SIOCAIFADDR.  Store netmask, dst/broadcast addr, interface flags,
 * interface id #.  Support for MIv3 autoconfiguration.  New checking
 * paradigm: if_addr_update, then either confirm_update or addr_reset.
 * 
 * Revision 3.4  93/05/03  14:48:06  bhk
 * Make sure if_lookup() always returns local network server port *first*.
 * (mjl for bhk)
 * 
 * Revision 3.3  93/04/12  15:55:24  nina
 * Needed a list of all network server nodes to support single
 * system semantics for SIOCGIFCONF, SIOCADDRT and SIOCDELRT.
 * Extended clearinghouse database to retrieve data easily
 * and wrote function to fetch and return said data.
 * 
 * Revision 3.2  93/03/02  14:08:19  bhk
 * Set network node number correctly for network servers in the clearinghouse
 * 
 * Revision 3.1  93/02/22  17:41:52  mjl
 * Add name string to chouse_t initialization, and use it in error messages.
 * Don't ignore caller-specified max array size *ports_len .
 * 
 * Revision 3.0  93/02/09  18:06:55  mjl
 * Clearinghouse management for virtual sockets.
 * 
 */

#include "uxkern/import_mach.h"
#include "sys/types.h"
#include "sys/errno.h"
#include "sys/socket.h"
#include "sys/uio.h"
#include "sys/param.h"
#include "sys/mbuf.h"
#include "sys/ioctl.h"
#include "sys/domain.h"
#include "sys/protosw.h"
#include "kern/queue.h"
#include "net/if.h"
#include "netinet/in.h"

#include "vsocket/vsocket.h"
#include "vsocket/vs_subr.h"
#include "vsocket/vs_chouse.h"

extern match_func_t	vsfindmatch();

#define	Register	register /* */
#define	Forward		extern


Forward int chouse_add_netserv(chouse_t	*, netserv_data_t *);

/*
 *  Clearinghouse support routines.
 *
 *  A clearinghouse is a database that maps socket addresses to
 *  Mach IPC ports (XXX and node numbers).
 */


/*
 * For each protocol in a configured domain, allocate and initialize
 * a proto_port_ns_t.  Called from init_portmaps().
 */
protocol_portns_t * 
init_protocol_portns (
	struct domain 		*dp)
{

	extern struct protosw 	routesw[];
	extern int		size_inetsw, size_routesw;
	int			cnt, i, size;
	struct protosw		*pswp;
	protocol_portns_t	*pnsp, *prev_pnsp, *head;


	VSDEBUG(VSDEBCLRHOUSE, ("entered init_protocol_portns\n"));

	switch (dp->dom_family) {

	case AF_INET:
		/* 
		 * Calculate number of entries in inetsw[] table.
		 */
		cnt = size_inetsw;
		pswp = &inetsw[0];

		VSDEBUG(VSDEBCLRHOUSE,
			("init_protocol_portns: AF_INET. cnt %d pswp 0x%x\n",
			cnt, pswp));

		break;

		/* 
		 * Calculate the number of entries in routesw[] table
		 */
	case AF_ROUTE:
		cnt = size_routesw;
		pswp = &routesw[0];

		VSDEBUG(VSDEBCLRHOUSE,
			("init_protocol_portns: AF_ROUTE. cnt %d pswp 0x%x\n",
			cnt, pswp));

		break;

	default:

		VSDEBUG(VSDEBCLRHOUSE,
			("init_protocol_portns: default \n"));
		return NULL;
	}

	/*
	 * Go thru the protocol switch table, and set up a 
	 * struct protocol_portns for each entry.
	 */
	for (	i = 0, size = sizeof(protocol_portns_t), head = NULL; 
		i < cnt;
		pswp++, i++) {

		VSDEBUG(VSDEBCLRHOUSE,
			("init_protocol_portns: for loop. i %d cnt %d"
			"size %d head 0x%x pswp 0x%x\n", i, cnt, size,
			head, pswp));

		/* allocate and bzero some heap */
		VS_MALLOC(pnsp, protocol_portns_t *, size, VSM_NSRVDATA);
		if(pnsp == NULL) 
			panic("init_protocol_portns: can't allocate memory");
		bzero((char*)pnsp, size);

		/* add new element to list */
		if (head == NULL)
			head = pnsp;
		else
			prev_pnsp->pns_next = pnsp;
		prev_pnsp = pnsp;

		/* Initialize fields */
		pnsp->pns_protocol = pswp->pr_protocol;
		pnsp->pns_port = IPPORT_RESERVED;

		VSDEBUG(VSDEBCLRHOUSE,
			("init_protocol_portns: pr_protocol %d\n",
			pswp->pr_protocol));

		mutex_init(&pnsp->pns_lock);
	}

	VSDEBUG(VSDEBCLRHOUSE,
		("init_protocol_portns: head 0x%x\n", head));

	return head;
}

/*
 * For each configured domain, allocate and intialize a portmap_t.
 * Called from chouse_init().
 */
portmap_t *
init_portmaps(
	chouse_t	*chs)
{
	extern struct domain	*domains;
	int			size;
	portmap_t		*pmp, *prev_pmp, *head;
	protocol_portns_t	*ppnsp, *prev_ppnsp;
	struct domain		*dp;

	VSDEBUG(VSDEBCLRHOUSE,
		("init_portmaps: entered\n"));	

	for (dp = domains, head = NULL, prev_pmp = NULL; 
	     dp; dp = dp->dom_next) {

		/*
		 * Port numbers don't exist for AF_UNIX domain.
		 */
		VSDEBUG(VSDEBCLRHOUSE,
			("init_portmaps: dp 0x%x. dp->dom_family %d\n",
			dp, dp->dom_family));

		if (dp->dom_family == AF_UNIX || dp->dom_family == AF_UNSPEC)
			continue;
		
		/*
		 * Allocate a portmap_t for each domain. 
		 */
		size = sizeof(portmap_t);
		VS_MALLOC(pmp, portmap_t *, size, VSM_NSRVDATA);
		if (pmp == NULL)
			panic("init_portmaps: can't allocate memory");
		bzero((char *)pmp, size);

		if (head == NULL)
			head = pmp;
		else	
			prev_pmp->pm_next = pmp;		
		prev_pmp = pmp;

		pmp->pm_domain = dp->dom_family;

		VSDEBUG(VSDEBCLRHOUSE,
			("init_portmap: pm_domain %d\n", pmp->pm_domain));

		mutex_init(&pmp->pm_lock);

		if ( ( ppnsp = init_protocol_portns(dp) ) == NULL)
			panic("init_portmaps:can't init protocol name spaces");
		else
			pmp->pm_proto = ppnsp;

		VSDEBUG(VSDEBCLRHOUSE,
			("init_portmap: ppnsp 0x%x\n", ppnsp));
	}	

	VSDEBUG(VSDEBCLRHOUSE,
		("init_portmaps: head 0x%x\n", head));

	return head;
}

/*
 *  Initialize an empty clearinghouse map.
 */

chouse_init(
        chouse_t        *chs,
        char            *name)
{
	extern node_t	this_node;
	extern int	clearinghouse_node;

	portmap_t	*pmp;

	if (this_node != clearinghouse_node) 
		return;

        ASSERT(chs != NULL);
        chs->ch_map = NULL;
        mutex_init(&chs->ch_mtx);
        queue_init(&(chs->ch_netserv_queue));
        chs->ch_name = name;

	VSDEBUG(VSDEBCLRHOUSE,
		("chouse_init: calling init_portmaps\n"));

	if ( (pmp = init_portmaps(chs) ) == NULL)
		panic("chouse_init: can't initialize port maps");
	else
		chs->ch_portmap = pmp;

	VSDEBUG(VSDEBCLRHOUSE,
		("returning from chouse_init. ch_portmap 0x%x pmp 0x%x\n",
		chs->ch_portmap, pmp));
}

/*
 *  chouse_get_family returns the netserv_data chain for the associated
 *  family or NULL if none exists.  Assumes the chouse mutex is held.
 */
netserv_data_t *
chouse_get_family(
	chouse_t	*chs,
	int		family)
{
	Register netserv_data_t *rnsp;

	ASSERT(chs != NULL);
	for ( rnsp = chs->ch_map; rnsp; rnsp = rnsp->next_fam ) {
		if(family == rnsp->if_info->ii_addr.sa_family)
			break;
	}
	return(rnsp);
}


/*
 *  Find interface information in the clearinghouse based on a key.
 *  If `create' is non-zero, allocate appropriate clearinghouse
 *  data structures if the key is not found.
 *
 *  Returns NULL if no entry was found or (if create flag set) if
 *  no entry was created.  Otherwise the the address of the interface
 *   info entry is returned.
 *
 *  The clearinghouse's mutex lock is assumed to be held.
 *
 *  Possible error codes:
 *	ENOMEM	- Could not allocate memory to add a new entry.
 *	ENOENT	- Entry not found.
 */
iface_info_t *
chouse_find_key(
	chouse_t	*chs,
	chouse_key_t	*ckp,
	int		create,
	int		*rval)
{
	Register netserv_data_t *nsdp, *fam_leader;
	Register iface_info_t	*iip;
	int			search_all_families;

	ASSERT(chs != NULL);
	ASSERT(ckp && ckp->ck_id);

	VSDEBUG(VSDEBCLRHOUSE,
		("chouse_find_key(%s): af %d, node %d, id 0x%x, create=%d\n",
		 CH_NAME(chs), ckp->ck_af, ckp->ck_node, ckp->ck_id, create));

	/*
	 *  If no address family was specified, we can use the node
	 *  and interface id alone to locate the clearinghouse entry,
	 *  looping through all the address family lists.  (This could
	 *  happen if SIOCAIFADDR is used to set the netmask, but not
	 *  to modify the configured address.)
	 *
	 *  XXX We are assuming that the first ifioctl() done will set
	 *  the interface address.  I believe this is a reasonable
	 *  assumption, but note that if it is wrong then addressing
	 *  info set prior to setting the i/f address will not be
	 *  recorded in the clearinghouse.
	 */
	if (ckp->ck_af == AF_UNSPEC) {
		ASSERT(create == FALSE);
		fam_leader = chs->ch_map;
		search_all_families = TRUE;
	} else {
		/* Find head of address family list. */
		search_all_families = FALSE;
		fam_leader = chouse_get_family(chs, ckp->ck_af);
		if (fam_leader == NULL && ! create) {
			*rval = ENOENT;
			return (NULL);
		}
	}

#define SearchNextFamily \
	(fam_leader ? (fam_leader = fam_leader->next_fam) : NULL)

next_family:

	/* Find netserv_data for key node. */
	for ( nsdp = fam_leader; nsdp; nsdp = nsdp->next_addr )
		if (nsdp->netserv_node == ckp->ck_node)
			break;
	if (nsdp == NULL && ! create) {
		if (SearchNextFamily) {
			goto next_family;
		} else {
			*rval = ENOENT;
			return (NULL);
		}
	}

	/*
	 *  If `create' flag is set and either no family leader at all
	 *  or no netserv entry in the family list corresponding to the
	 *  given node, then we have to make an entry.
	 */
	if (create && (fam_leader == NULL || nsdp == NULL)) {
		VS_MALLOC(nsdp, netserv_data_t *, sizeof(netserv_data_t),
			  VSM_NSRVDATA);
		if (nsdp == NULL) {
			*rval = ENOMEM;
			return (NULL);
		}
		bzero(nsdp, sizeof(netserv_data_t));
		nsdp->netserv_node = ckp->ck_node;

		VS_MALLOC(iip, iface_info_t *, sizeof(iface_info_t),
			  VSM_IFINFO);
		if (iip == NULL) {
			VS_FREE(nsdp, VSM_NSRVDATA);
			*rval = ENOMEM;
			return (NULL);
		}
		nsdp->if_info = iip;
		bzero(iip, sizeof(iface_info_t));
		iip->ii_id = ckp->ck_id;
		iip->ii_server = nsdp;

		if (fam_leader == NULL) {
			/*
			 *  Add family list leader to clearinghouse.
			 *  (Note next_server field is only valid
			 *  for family leaders.)
			 */
			nsdp->next_fam = chs->ch_map;
			chs->ch_map = nsdp;
			nsdp->next_server = nsdp;
		} else {
			/* 2nd entry on an existing family list */
			nsdp->next_addr = fam_leader->next_addr;
			fam_leader->next_addr = nsdp;
		}
		*rval = ESUCCESS;
		return (iip);
	}

	/*
	 *  We now have the netserv entry for the given node; try
	 *  to find a matching interface description.
	 */
	for (iip = nsdp->if_info; iip; iip = iip->ii_next)
		if (iip->ii_id == ckp->ck_id)
			break;
	if (iip == NULL) {
		if (create) {
			/*
			 *  No matching interface, so create one.
			 */
			VS_MALLOC(iip, iface_info_t *, sizeof(iface_info_t),
				  VSM_IFINFO);
			if (iip == NULL) {
				*rval = ENOMEM;
				return (NULL);
			}
			bzero(iip, sizeof(iface_info_t));
			iip->ii_id = ckp->ck_id;
			iip->ii_server = nsdp;
			iip->ii_next = nsdp->if_info;
			nsdp->if_info = iip;
		} else if (SearchNextFamily)
			goto next_family;
	}

	*rval = (iip ? ESUCCESS : ENOENT);
	VSDEBUG(VSDEBCLRHOUSE, ("chouse_find_key(%s): got iip 0x%x, error %d\n",
				CH_NAME(chs), iip, *rval));
	return (iip);
}


/*
 *  The chouse_addr_update() routine is called to inform the
 *  clearinghouse of ioctl's that affect interface addressing
 *  information.  Clearinghouse entries are added, modified,
 *  or deleted as a result.
 *
 *  In particular:
 *
 *	SIOCSIFADDR - Register a new entry in the clearinghouse.
 *		A server port and node must be specified.
 *	SIOCAIFADDR - Register a new entry, or change the address
 *		of an existing entry.
 *	SIOCDIFADDR - Delete an entry (not yet implemented).
 *	SIOCSIFNETMASK - Associate a netmask with an existing entry.
 *	SIOCSIFDSTADDR,
 *	SIOCSIFBRDADDR - Associate a destination or broadcast address
 *		with an existing entry.
 *	SIOCSIFFLAGS - Modify flags of existing entry (i.e. IFF_UP).
 *
 *  The clearing house is a 3D array.  The first dimension is
 *  family type, the second dimension is nodes and the third dimension
 *  is address(s).
 */
int
chouse_addr_update(
	chouse_t	*chs,
	chouse_key_t	*ckp,
	int		iface_flags,
	int		cmd,
	struct ifreq	*ifr,
	mach_port_t	port,
	iface_info_t	*old_iip)
{
	int			rval = ESUCCESS;
	Register netserv_data_t *nsdp;
	Register iface_info_t	*iip;
	match_func_t		match;
	struct ifaliasreq	*ifra = (struct ifaliasreq *)ifr;
	int			af, create;
	kern_return_t		kr;

	ASSERT(chs != NULL);
#ifdef VSOCKET_DEBUG
	if(vsdebug & (VSDEBENTRY|VSDEBCLRHOUSE)) {
		printf("chouse_addr_update(%s): cmd=0x%x, addr %s",
		       CH_NAME(chs),
		       cmd,
		       sockaddr_to_string(&ifra->ifra_addr));
		if (ifra->ifra_broadaddr.sa_family != AF_UNSPEC)
			printf(", brdaddr %s",
			       sockaddr_to_string(&ifra->ifra_broadaddr));
		if (ifra->ifra_mask.sa_family != AF_UNSPEC)
			printf(", mask %s",
			       sockaddr_to_string(&ifra->ifra_mask));
		printf("\n");
	}
#endif

	mutex_lock(&chs->ch_mtx);
#define	returnerror(x)	{ rval = (x); goto out; }

	/*
	 *  Locate (or allocate if `create' is set) the interface
	 *  info based on the given key.
	 */
	create = (port != MACH_PORT_NULL &&
		  ((cmd == SIOCAIFADDR && ifra->ifra_addr.sa_len) ||
		   cmd == SIOCSIFADDR));
	iip = chouse_find_key(chs, ckp, create, &rval);
	if (iip == NULL)
		returnerror(rval);
	nsdp = iip->ii_server;

	/*
	 *  Save prior state of the interface info, since we may have
	 *  to reset it if the subsequent ifioctl() call in vslo_ioctl()
	 *  fails.  If a new iface_info_t was allocated, we clear
	 *  to indicate this old_iip->ii_id .
	 */
	if (create)
		old_iip->ii_id = 0;
	else
		*old_iip = *iip;

	/*
	 *  If we created a new entry above, there are some fields
	 *  in the netserv_data to be initialized.
	 */
	if (create) {
		ASSERT(nsdp->netserv_port == MACH_PORT_NULL ||
		       nsdp->netserv_port == port);
		nsdp->netserv_port = port;
		iip->ii_addr = *(struct sockaddr *)&ifr->ifr_addr;
		(void) chouse_add_netserv(chs, nsdp);
	}

	/*
	 *  We now have both a netserv entry and associated interface
	 *  info.  Update the interface info according to the
	 *  interface ioctl that got us here.
	 *
	 *  This code parallels in_control() in netinet/in.c .
	 */
	iip->ii_flags = iface_flags;	/* they may have changed... */
	switch (cmd) {
	case SIOCSIFADDR:
		iip->ii_addr = *(struct sockaddr *)&ifr->ifr_addr;
		break;
	case SIOCSIFNETMASK:
		iip->ii_sockmask = *(struct sockaddr *)&ifra->ifra_addr;
		break;
	case SIOCSIFDSTADDR:
		ASSERT(iip->ii_flags & IFF_POINTOPOINT);
		iip->ii_dstaddr = *(struct sockaddr *)&ifr->ifr_dstaddr;
		break;
	case SIOCSIFBRDADDR:
		iip->ii_dstaddr = *(struct sockaddr *)&ifr->ifr_broadaddr;
		break;
	case SIOCAIFADDR:
		if (ifra->ifra_mask.sa_len)
			iip->ii_sockmask =
				*(struct sockaddr *)&ifra->ifra_mask;
#define ifra_dstaddr ifra_broadaddr
		if ((iip->ii_flags & IFF_POINTOPOINT) &&
		    (ifra->ifra_dstaddr.sa_family != AF_UNSPEC))
			iip->ii_dstaddr =
				*(struct sockaddr *)&ifra->ifra_dstaddr;
#undef ifra_dstaddr
		if ((iip->ii_flags & IFF_BROADCAST) &&
		    (ifra->ifra_broadaddr.sa_family != AF_UNSPEC))
			iip->ii_broadaddr =
				*(struct sockaddr *)&ifra->ifra_broadaddr;
		/*
		 *  XXX Not clear if this next part is completely correct.
		 *  Wish I had some docs on SIOCAIFADDR....
		 */
		if (ifra->ifra_addr.sa_len)
			iip->ii_addr = *(struct sockaddr *)&ifra->ifra_addr;
		break;
	case SIOCSIFFLAGS:
		iip->ii_flags = (iip->ii_flags & IFF_CANTCHANGE) |
			(ifr->ifr_flags &~ IFF_CANTCHANGE);
		break;
	default:
		panic("chouse_addr_update: unsupported ioctl 0x%x\n", cmd);
		/*NOTREACHED*/
	}

out:
	VSDEBUG(VSDEBCLRHOUSE, ("chouse_addr_update(%s): rval %d (0x%x)\n",
			 CH_NAME(chs), rval, rval));
	mutex_unlock(&chs->ch_mtx);
	if (create && rval != ESUCCESS) {
		kr = mach_port_deallocate(mach_task_self(), port);
		if (kr != KERN_SUCCESS)
		    printf("chouse_addr_update: port 0x%x dealloc: kr 0x%x\n",
			   port, kr);
	}
	return (rval);
}


/*
 *  Back out a change to clearinghouse address information.  If the
 *  interface's previous ii_id is zero, then we are backing out the
 *  creation of the clearinghouse entry, so delete the entry entirely.
 *  Otherwise replace it with appropriate fields of *prev.
 */
int
chouse_addr_reset(
	chouse_t	*chs,
	chouse_key_t	*ckp,
	iface_info_t	*prev)
{
	iface_info_t	*iip;
	iface_info_t	**iipp;
	int		rc;

	mutex_lock(&chs->ch_mtx);

	iip = chouse_find_key(chs, ckp, 0/*create*/, &rc);
	if (rc != ESUCCESS || iip == NULL)
	    panic("chouse_addr_reset: %s: af=%d node=%d id=0x%x, rc %d\n",
		  "key not found", ckp->ck_af, ckp->ck_node, ckp->ck_id, rc);

	/*
	 *  If the previous interface info had a valid id, just
	 *  reset the interface address information.
	 */
	if (prev->ii_id != 0) {
		iip->ii_flags	= prev->ii_flags;
		iip->ii_addr	= prev->ii_addr;
		iip->ii_sockmask = prev->ii_sockmask;
		iip->ii_dstaddr	= prev->ii_dstaddr;
		goto out;
	}

	/*
	 *  Since prev->ii_id is zero, the clearinghouse entry did not
	 *  previously exist, so to reset we must delete it.
	 *
	 *  NB we don't delete the netserv_data_t even if we are
	 *  deleting the last iface_info_t attached to it.  We should,
	 *  though.  XXX
	 */
	iipp = &iip->ii_server->if_info;
	while ( *iipp && *iipp != iip )
		iipp = &((*iipp)->ii_next);
	ASSERT(*iipp == iip);
	*iipp = iip->ii_next;
	VS_FREE(iip, VSM_IFINFO);

out:
	mutex_unlock(&chs->ch_mtx);
	return (ESUCCESS);
}


/*
 *  Look up a <sockaddr, node> pair in a clearinghouse, and return
 *  all the associated ports.
 */
int
chouse_lookup(
	chouse_t	*chs,
	node_t		node,
	int		family,
	struct sockaddr	*sa,
	int		salen,
	mach_port_t	*ports,		/* out */
	int		*ports_len)	/* out */
{
	Register netserv_data_t *rnsp;
	Register iface_info_t	*iip;
	match_func_t	match;
	int		serv_count = 0;
	int		rval = NSRV_ADDR_NOT_FOUND;

#ifdef VSOCKET_DEBUG
	if(vsdebug & (VSDEBENTRY|VSDEBCLRHOUSE)) {
		Register int	i;
		printf("chouse_look_up(%s): family %d, addr %s",
		       CH_NAME(chs), family, sockaddr_to_string(sa));
		printf(", %d ports requested\n", *ports_len);
	}
#endif
	
	if (*ports_len > MAX_SERVERS)
		*ports_len = MAX_SERVERS;

	ASSERT(chs != NULL);
	mutex_lock(&chs->ch_mtx);

	/* loop through each of the servers checking for an address match */

	if ( (rnsp = chouse_get_family(chs, family)) == NULL ) {
		*ports_len = 0; /* serv_count */
		mutex_unlock(&chs->ch_mtx);
		return (ESUCCESS);
	}

	match = vsfindmatch(family);
	if(salen == 0) {
		/*
		 *  An salen of 0 means the caller is binding to a NULL
		 *  address, that is, asking the system to choose an
		 *  address for it.  In this case we should return
		 *  all available servers of the specified family.
		 */
		for ( ; rnsp ; rnsp = rnsp->next_addr ) {
			/*
			 * if there are multiple network servers
			 * and one is local, return it first in the list
			 */
			if(rnsp->netserv_node == node) {
				ports[serv_count] = ports[0];
				ports[0] = MACH_PORT_NULL;
			} else
				ports[serv_count] = rnsp->netserv_port;
			serv_count++;
			
			if(serv_count >= *ports_len)
				break;
		}
		if (serv_count)
			rval = KERN_SUCCESS;
	} else for(;rnsp;rnsp=rnsp->next_addr) {
		for(iip = rnsp->if_info; iip; iip = iip->ii_next) {
			/*
			 *  XXX The match routines can now take advantage
			 *  of the netmask stored in the iface_info_t.
			 */
			if((*match)(sa, &iip->ii_addr)) {
				/*
				 * if there are multiple network servers and
				 * one is local, return it first in the list
				 */
				if(rnsp->netserv_node == node) {
					ports[serv_count] = ports[0];
					ports[0] = MACH_PORT_NULL;
				} else
					ports[serv_count] = rnsp->netserv_port;
				serv_count++;

				/*
				 * acknowledge that we found a matching server
				 */
				rval=KERN_SUCCESS;
				break;
			}
		}
		if(serv_count >= *ports_len)
			break;
	}

	*ports_len = serv_count;
	mutex_unlock(&chs->ch_mtx);
	return (rval);
}


/*
 *  Find map entry for the given address family that is as "close"
 *  as possible to the given node.
 *
 *  Currently there is no concept of node-to-node distance, so this
 *  routine just selects the next server in the list.
 */
mach_port_t
chouse_find_nearest(
	chouse_t	*chs,
	int		family,
	node_t		node,
	node_t		*host_node)
{
	Register netserv_data_t	*fam_leader;
	Register netserv_data_t	*fam_member;
	netserv_data_t		*nsd;	/* Next Server, Daddy-O! */
	mach_port_t		retport;

	retport = MACH_PORT_DEAD;	/* assume failure */
	if (host_node != NULL)
		*host_node = INVALID_NODE;

	ASSERT(chs != NULL);
	mutex_lock(&chs->ch_mtx);

	if ( (fam_leader = chouse_get_family(chs, family)) == NULL )
		goto out;

	/*
	 * First search for a server on the caller's local node.
	 */
	for ( fam_member = fam_leader ;
	      fam_member ;
	      fam_member = fam_member->next_addr ) {
		if ( fam_member->netserv_node == node ) {
			/* Found one!  Null port indicates local server. */
			retport = MACH_PORT_NULL;
			if (host_node != NULL)
				*host_node = fam_member->netserv_node;
			goto out;
		}
	}

	/*
	 * If we don't have a local server 
	 * use this lame load-sharing algorithm
	 * of cycling through the available servers.
	 */
	nsd = fam_leader->next_server;
	ASSERT(nsd != NULL);
	retport = nsd->netserv_port;
	if (host_node != NULL)
		*host_node = nsd->netserv_node;
	fam_leader->next_server =
		( nsd->next_addr ? nsd->next_addr : fam_leader );

 out:
	VSDEBUG(VSDEBCLRHOUSE,
		("chouse_find_nearest_server(%s, fam=%d, node=%d) --> 0x%x\n",
		 CH_NAME(chs), family, node, retport));
	mutex_unlock(&chs->ch_mtx);
	return (retport);
}


/*
 *  Return the network server <node, port> pair most appropriate for
 *  sending packets to the specified target address, as dictated by
 *  the routing tables.  NB we assume that the clearinghouse node is
 *  also a network server (and thus has routing tables).
 */
#include <net/route.h>
#include <net/if_types.h>
#include <vsocket/mi_var.h>
kern_return_t
chouse_get_server_by_route(
	chouse_t	*chs,
	node_t		pvs_node,	/* primary virtual socket's node */
	struct sockaddr	*sa,		/* the target socket address */
	node_t		*ret_node,	/* OUT - node of netserver */
	mach_port_t	*ret_port)	/* OUT - port of netserver */
{
	struct route	target_rt;
	struct ifnet	*ifp;
	chouse_key_t	chskey;
	iface_info_t	*iip;
	int		rc;
	ROUTE_LOCK_DECL()

	ASSERT(chs && ret_port && ret_node);

	/* Assume no network server found. */
	*ret_node = INVALID_NODE;
	*ret_port = MACH_PORT_NULL;

	/* Find a good interface for talking to this address. */
	bcopy(sa, &target_rt.ro_dst, sa->sa_len);
	target_rt.ro_rt = NULL;
	ROUTE_WRITE_LOCK();
	rtalloc(&target_rt);
	ROUTE_WRITE_UNLOCK();
	if (target_rt.ro_rt == 0)
		return (KERN_SUCCESS);
	ifp = target_rt.ro_rt->rt_ifp;
	ASSERT(ifp != NULL);

	/*
	 *  If the found interface is an MI interface, we want to
	 *  use the network server of the real interface it is
	 *  standing in for.
	 */
	if (ifp->if_type == IFT_MI) {
		if ( (chskey.ck_node = MI_REMOTE_NODE(ifp)) == pvs_node ) {
			/*
			 *  The interface to use is on the primary
			 *  virtual socket's own node, so there is
			 *  no need to do a clearinghouse query---just
			 *  return MACH_PORT_NULL and all is well.
			 */
			*ret_port = MACH_PORT_NULL;
			*ret_node = chskey.ck_node;
			goto out;
		}
		chskey.ck_af = sa->sa_family;
		chskey.ck_id = MI_REMOTE_ID(ifp);
		mutex_lock(&chs->ch_mtx);
		iip = chouse_find_key(chs, &chskey, 0/*create*/, &rc);
		if (rc != ESUCCESS || iip == NULL)
			panic("chouse_get_server_by_route: rt_ifp 0x%x but "
			      "no entry for af=%d node=%d id=0x%x, rc %d\n",
			      ifp, chskey.ck_af, chskey.ck_node, chskey.ck_id,
			      rc);
		ASSERT(iip->ii_server);
		*ret_port = iip->ii_server->netserv_port;
		ASSERT(iip->ii_server->netserv_node == chskey.ck_node);
		mutex_unlock(&chs->ch_mtx);
		*ret_node = chskey.ck_node;
	} else {
		extern node_t		this_node;
		extern mach_port_t	inetserver_port;

		/*
		 *  The interface is right here on this node!  Take
		 *  a short cut and don't bother to look in the
		 *  clearinghouse itself.
		 */
		*ret_node = this_node;
		*ret_port = (pvs_node == this_node
			    ? MACH_PORT_NULL
			    : inetserver_port);
	}

 out:
	/* Free the allocated route and we're done! */
	ROUTE_WRITE_LOCK();
	RTFREE(target_rt.ro_rt);
	ROUTE_WRITE_UNLOCK();
	return (KERN_SUCCESS);
}


/*
 *  This routine walks the clearinghouse queue of unique network
 *  servers and calls the provided action routine for each network
 *  server.  If the action routine ever returns an error, a flag
 *  determines whether to abort or continue on.  If we do continue,
 *  the last error encountered is returned.
 */
int
chouse_walk_servers(
	chouse_t	*chs,
	ch_action_t	action,
	caddr_t		arg,
	int		abort_on_error)
{
	int		rc;
	int		last_error = ESUCCESS;
	netserv_data_t	*ndp;
	queue_head_t	*q = &chs->ch_netserv_queue;;

	for (ndp = (netserv_data_t *) queue_first(q) ;
	     ! queue_end(q, (queue_entry_t)ndp) ;
	     ndp = (netserv_data_t *) queue_next(&ndp->netserv_queue) ) {
		rc = (*action)(ndp->netserv_port,
			       ndp->netserv_node,
			       arg);
		if (rc != ESUCCESS) {
			if (abort_on_error)
				return (rc);
			last_error = rc;
		}
	}
	return (last_error);
}


/*
 * Name:		chouse_list()
 *
 * Function:
 *	This function walks thru a given clearing house 
 *	database and returns the total number of network 
 *	server nodes found and a list of the server node
 *	numbers and corresponding mach control ports.
 *
 * Inputs:
 *	nel	- 	# of elements in nodes &/or ports array
 *	client_node -	node requesting this information
 *
 * Outputs:
 *	numns	- 	total # of network server nodes configured.
 *	rel	-	number of nodes/ports returned to caller.
 *	nodes	-	array of network server node numbers.
 *	ports	-	array of network server mach ports.
 *
 *	nodes[i] is the node number of a given network server node
 *	and ports[i] is the corresponding mach port.
 *
 * Returns:
 *
 *	EINVAL	- invalid input parameter
 */
int
chouse_list(
	chouse_t	*chs,
	node_t		client_node,
	int		nel,
	int		*numns,
	int		*rel,
	node_t		*nodes,
	mach_port_t	*ports)
{
	int			i;
	int			used;
	netserv_data_t		*ns;
	queue_head_t		*head;
	node_t			*np;
	mach_port_t		*mp;


	mutex_lock(&chs->ch_mtx);

	head = &chs->ch_netserv_queue;

	/* If the queue is empty, nothing to do */	
	if (queue_empty(head)) {
		*rel  = 0;
		*numns = 0;
		mutex_unlock(&chs->ch_mtx);
		return(ESUCCESS);
	}

	/*
	 * By the time we finish, "i" = number of
	 * network server nodes configured and "used"
	 * equals to the number of elements in 
	 * "nodes" & "ports" that we have filled
	 * in with interesting information.
	 */
	for(i = 0, used = 0, np = nodes, mp = ports,
	    ns = (netserv_data_t*)chs->ch_netserv_queue.next;
	    ns != (netserv_data_t*)head;
	    ns = (netserv_data_t*)ns->netserv_queue.next, i++) {

		if (i < nel) {
			/*
			 *  By convention, if there is a network server
			 *  on the client node, it is returned first in
			 *  the list, with netserv_port == MACH_PORT_NULL.
			 */
			if (ns->netserv_node == client_node) {
				*np++ = nodes[0];
				*mp++ = ports[0];
				nodes[0] = client_node;
				ports[0] = MACH_PORT_NULL;
			} else {
				*np++ = ns->netserv_node;
				*mp++ = ns->netserv_port;
			}
			used++; 
		}
	}

	*rel = used;
	*numns = i;

	mutex_unlock(&chs->ch_mtx);
	return(ESUCCESS);
}


/*
 * Name:
 *	chouse_add_netserv()
 *
 * Function:
 *	This function contructs a list that consists of one
 *	netserv_data block per network server node.  This was
 *	added to the clearinghouse in order to make it easier
 *	to get a complete list(with no duplicates) of network
 *	server nodes and their associated mach ports without
 *	traversing the entire clearinghouse, identifying and
 *	discarding duplicates, etc.  Assumes that caller handles
 *	locking/unlocking.
 *
 *
 * Inputs:
 *	chs 	-	ptr to clearinghouse chouse_t descriptor
 *	new	-	ptr to new element to add to list
 *
 * Outputs:
 *	none
 *
 * Calls:
 *	Uses definitions and macros from queue.h
 *
 * Called by:
 *	chouse_update()		-	not operational at this time XXX
 *	chouse_checkin()	-	only valid call so far
 *
 * Returns:
 *	EINVAL	- invalid parameter
 */
int
chouse_add_netserv(
	chouse_t	*chs,		/* clearinghouse ptr */
	netserv_data_t	*new)		/* new element to add to queue */
{
	netserv_data_t	*cur;		/* current element */
	queue_head_t	*head;		/* head of queue */

	/*
	 * If we don't know which clearinghouse or user called
	 * with no element to queue, quit.
	 */
	if( (chs == NULL) || (new == NULL) )
		return(EINVAL);

	head = &(chs->ch_netserv_queue);

	if (queue_empty(head)) {
		queue_enter_first(head, new, netserv_data_t *, netserv_queue);
		return (ESUCCESS);
	}

	for(cur = (netserv_data_t *)(queue_last(head));
	    cur != (netserv_data_t *)head;
	    cur = (netserv_data_t *)queue_prev(&(cur->netserv_queue))) {

		if(cur->netserv_node < new->netserv_node) {
			queue_insert_after(head, cur, new,
					    netserv_data_t *, netserv_queue);
			return (ESUCCESS);
		} else if (cur->netserv_node == new->netserv_node)
			/* Don't insert duplicates. */
			return (ESUCCESS);
	}
	queue_enter_first(head, new, netserv_data_t *, netserv_queue);
	return(ESUCCESS);
}

