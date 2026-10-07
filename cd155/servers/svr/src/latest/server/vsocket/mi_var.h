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
 * $Log: mi_var.h,v $
 * Revision 1.12  1995/02/01  23:18:09  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.11  1994/11/18  20:51:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1993/09/20  23:58:26  cfj
 * Merge R1.1 bug fixes into main stem.
 *
 * Revision 1.9.2.1  1993/09/20  23:49:26  cfj
 * Bug fix for PTS #6663.  The correct netserver is now chosen.
 *
 * Revision 1.9  1993/09/01  01:40:04  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.8  1993/07/14  18:48:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  21:13:12  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/20  23:32:31  cfj
 * Correctly check for MACH_ASSERT in order to turn on MI_DEBUG.
 *
 * Revision 1.6  1993/05/20  16:04:15  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 3.10  93/08/26  10:52:06  mjl
 * Add MI statistics macros.
 * 
 * Revision 3.9  93/07/27  11:42:39  mjl
 * Use MACRO_{BEGIN,END} macros.
 * 
 * Revision 3.8  93/05/25  10:42:40  mjl
 * Change "#ifdef MACH_ASSERT" to "#if MACH_ASSERT".
 * 
 * Revision 3.7  93/05/13  11:39:06  mjl
 * New MIDOT() macro for quiet debugging of packet sends/receives.
 * 
 * Revision 3.6  93/05/07  19:11:48  mjl
 * Make MI_DEBUG conditional on MACH_ASSERT.  Add MIDEBUGX() macro, useful
 * for calling pretty-print routines.  Make MIDEBUG() always print node number.
 * 
 * Revision 3.5  93/05/07  15:12:32  nina
 * Added MD_INVR and MD_INVF to support debugging
 * of new MI driver
 * 
 * Revision 3.4  93/05/06  17:36:27  mjl
 * Default compilation now has debugging turned off.  Get rid of stale debug
 * bits.  New MIDEBUGX() macro for calling pretty-print subroutines.
 * 
 * Revision 3.3  93/05/05  22:30:52  mjl
 * New data types and macros for MIv3.
 * 
 * Revision 3.2  93/02/22  17:39:00  mjl
 * Changes for broadcast-style MI network interface.
 * 
 * Revision 3.1  92/07/08  09:15:55  roman
 * Change node numbers to type node_t.
 * 
 * Revision 3.0  92/04/20  17:21:24  bhk
 * Genesis
 * 
 *
 * Genisis 1/20/91 bhk
 *
 */

#ifndef _MI_VAR_H_
#define	_MI_VAR_H_

#include "kern/queue.h"
#include "kern/macro_help.h"
#include "kern/zalloc.h"
#include "kern/lock.h"

/*
 *  MI network interface structure
 */
typedef struct mi_softc {
	struct ifnet 	sc_if;		/* iface seen by protocol stack */
	unsigned int	sc_flag;	/* MI-specific flags */
	int		sc_iface_id;	/* id (address) of remote real iface */
	char		sc_ifname[IFNAMSIZ]; /* construct MI iface name here */
	struct rtentry	*sc_host_rt;	/* zero hop route to real iface */
	struct ifaddr	*sc_h_rtifa;	/* saved host route rt_ifa */
	struct ifaddr	*sc_n_rtifa;	/* " for ntwrk rt, see mi_rtrequest */
	struct mi_assoc	*sc_assoc;	/* transport info */
	struct mi_softc	*sc_next;	/* list of all MI interfaces */
	struct mi_softc	*sc_comrads;	/* list of ifaces sharing this assoc */
} mi_softc_t;

/*
 *  MI association.  Sometimes referred to as the "MI link".
 *  Contains all transport-related info for MI communication
 *  between nodes.
 */
struct mi_assoc {
	node_t		ma_remote_node;	/* node number at remote end */
	mach_port_t	ma_ctrl_port;	/* for sending control messages */
	mach_port_t	ma_data_port;	/* for sending packet data */
	struct mi_assoc	*ma_next;	/* list of all MI associations */
	struct mi_softc	*ma_comrads;	/* list of ifaces using this assoc */
};

#define	MI_REMOTE_NODE(ifp)					\
	(((mi_softc_t *)ifp)->sc_assoc				\
	 ? ((mi_softc_t *)ifp)->sc_assoc->ma_remote_node	\
	 : INVALID_NODE)

#define MI_REMOTE_ID(ifp)	(((mi_softc_t *)ifp)->sc_iface_id)

/*
 *  Mach_port-to-data_structure translation
 */
#define	MI_DATAPORT(ma)		((mach_port_t)(ma))
#define MI_CTRLPORT(ma)		((mach_port_t)((int)(ma) + 1))

#define	MI_DATAPORT_TO_ASSOC(port, ma)			\
MACRO_BEGIN						\
	(ma) = (struct mi_assoc *)(port);		\
MACRO_END

#define	MI_CTRLPORT_TO_ASSOC(port, sc)			\
MACRO_BEGIN						\
	(ma) = (struct mi_assoc *)((int)port - 1);	\
MACRO_END


/*
 *  Useful MI extern declarations
 */

extern struct mutex	mi_list_mtx;   /* protects mi_list and mi_assoc_list */
extern struct mi_assoc	*mi_assoc_list;
extern struct mi_softc	*mi_list;
extern mach_port_t	mi_data_port_set;
extern zone_t		mi_msg_zone;

extern struct ifnet	*ifminode(node_t);


#define	MI_NEED_PORT	0x01
#define	MI_NEED_VERIFY	0x02

/*
 *  Packet-related definitions
 */
#define MI_ALLOC	8192
#define	MI_MACH_HDR_SIZE	(sizeof(mach_msg_header_t) + \
				 sizeof(mach_msg_type_long_t))
#define	MI_PKT_SIZE	(MI_ALLOC-MI_MACH_HDR_SIZE)
#define MIMTU 		(MI_PKT_SIZE-sizeof(mi_hdr_t))
#define MIMAXQLEN	50			 

typedef struct mi_hdr {
	int		mih_family;	/* packet address family */
	int		mih_ifid;	/* i/f pkt would use in uniprocessor */
	mach_port_t	mih_dest;	/* MI data port to send packet on */
} mi_hdr_t;

typedef struct mi_mach_pkt {
	mach_msg_header_t Head;
	mach_msg_type_long_t	Type;
	char		mi_pkt_data[MI_PKT_SIZE];
} mi_mach_pkt_t;

typedef struct mi_data_pkt {
	mi_hdr_t	mi_hdr;
	char		mi_data[MIMTU];
} mi_data_pkt_t;

#define	MI_ERROR_NOT_FOUND	0x101010

/*
 *  Statistics
 */
struct mi_stat {
	int	mis_zalloc;
	int	mis_zfree;
	decl_simple_lock_data(,mis_lock)
};
extern struct mi_stat mistat;

#define MISTAT_DECL	struct mi_stat mistat

#define MISTAT_INIT				\
MACRO_BEGIN					\
	bzero(&mistat, sizeof(struct mi_stat));	\
	simple_lock_init(&mistat.mis_lock);	\
MACRO_END

#define MISTAT(x)				\
MACRO_BEGIN					\
	simple_lock(&mistat.mis_lock);		\
	mistat.x ++ ;				\
	simple_unlock(&mistat.mis_lock);	\
MACRO_END

/*
 *  Debugging printfs and flags...
 */
#if	MACH_ASSERT
#define	MI_DEBUG	1
#endif

#ifdef	MI_DEBUG

extern int	mi_debug;
extern node_t	this_node;

#define	MIDEBUG(mask, args)				\
MACRO_BEGIN						\
	if (mi_debug & (mask)) {			\
		printf("[node %d] ", this_node);	\
		printf args ;				\
	}						\
MACRO_END

#define MIDEBUGX(mask, action)				\
MACRO_BEGIN						\
	if (mi_debug & (mask)) {			\
		action ;				\
	}						\
MACRO_END

#define MIDOT(ch)					\
MACRO_BEGIN						\
	if (mi_debug & MD_DOTS) {			\
		printf("%c", ch);			\
	}						\
MACRO_END

#define	MD_CONFIG	0x001	/* autoconfiguration */
#define MD_TRACE	0x002	/* routine entry points */
#define MD_ERR		0x004	/* error returns */
#define MD_INFO		0x008	/* miscellaneous information */
#define	MD_PKT		0x010	/* show packet contents */
#define	MD_SND		0x020	/* packet transmission */
#define	MD_RCV		0x040	/* packet reception */
#define MD_INVR		0x080	/* show invisible MI routes (netstat, etc.) */
#define MD_INVF		0x100	/* show invisible MI interfaces (ditto) */
#define MD_DOTS		0x200	/* print char on packet receive/send */

#else	/* ! MI_DEBUG */

#define	MIDEBUG(mask, args)
#define MIDEBUGX(mask, action)
#define MIDOT(ch)

#endif	/* ! MI_DEBUG */

#endif	/* ! _MI_VAR_H_ */
