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
 * $Log: vs_debug.c,v $
 * Revision 1.13  1995/02/01  23:22:43  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.12  1994/11/18  20:52:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/05/04  22:17:10  mjl
 * Merge revision 1.10.2.1 from R1_2 branch into main trunk.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.10.2.1  1994/04/27  23:05:17  yazz
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
 * TNC select rewrite.  Just some file svrref debug code.
 *
 * Revision 1.10  1993/09/14  15:51:54  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.9.2.1  1993/09/14  15:50:04  cfj
 * Add stern warning about the perils of calling sockaddr_to_string() in
 * subroutine argument lists.
 *
 * Revision 1.9  1993/09/01  01:40:50  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.8  1993/07/14  18:49:00  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  21:13:53  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/18  03:21:10  cfj
 * Complete MI Driver merge.
 *
 * Revision 1.6  1993/05/06  19:33:25  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 3.10  93/08/22  09:34:41  bhk
 * Enhanced debugging to help with mbuf leakage [358]
 * 
 * Revision 3.9  93/06/24  13:31:39  mjl
 * Add vs_show_node_binding() routine, used for virtual socket debug printfs.
 * 
 * Revision 3.8  93/05/05  22:42:32  mjl
 * Add sockaddr_to_string() to pretty-print sockaddr structs.
 * 
 * Revision 3.7  93/02/10  17:09:15  klh
 * Fix RCS comments
 *
 * Revision 3.6  93/02/08  16:54:03  bhk
 * Compressed the debug output by 2 lines
 * 
 * Revision 3.5  92/10/27  17:52:04  bhk
 * Removed the debug thread
 * 
 * Revision 3.4  92/09/24  17:10:34  bhk
 * corrected spelling on debug messages
 * 
 * Revision 3.3  92/07/26  17:49:59  bhk
 * cleaned up debug, added port debug thread
 * 
 * Revision 3.2  92/06/16  18:45:15  bhk
 * added port debugger
 * 
 * Revision 3.1  92/03/20  17:08:14  bhk
 * moved vsocket.h to vsocket directory
 * 
 * Revision 3.0  92/03/04  14:56:33  bhk
 * Genesis	bhk
 * 
 *
 * Extracted from vp_debug Version 2.6 bhk
 * 
 *
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/mbuf.h"
#include "sys/uio.h"
#include "sys/socket.h"
#include "sys/un.h"
#include "netinet/in.h"
#include "vsocket/vsocket.h"
#include "sys/socketvar.h"
#include "sys/file.h"


/*
 *  BEWARE!!! Because this uses a static buffer, it cannot be called more
 *  than once in, say, a printf argument list!!!
 *
 *  XXX Not thread safe!!!
 */
char *
sockaddr_to_string(
	struct sockaddr	*sa)
{
	static char		buffer[32];
	struct sockaddr_in	*sin = (struct sockaddr_in *)sa;
	struct sockaddr_un	*sun = (struct sockaddr_un *)sa;
	uchar_t			*cp;
	char			*bp;
	int			i;

	if (sa == NULL)
		return "(nil sockaddr)";

	bp = buffer;
#define advance_bp	do { while (*bp) bp++; } while(0)

	switch (sa->sa_family) { 
	case AF_INET:
		cp = (uchar_t *)&sin->sin_addr.s_addr;
		sprintf(bp, "%u.", *cp++);	advance_bp;
		sprintf(bp, "%u.", *cp++);	advance_bp;
		sprintf(bp, "%u.", *cp++);	advance_bp;
		sprintf(bp, "%u/%u", *cp++, sin->sin_port);	advance_bp;
		break;
	case AF_UNIX:
		sprintf(bp, "%s", sun->sun_path);
		break;
	default:
		sprintf(bp, "af %u addr ", sa->sa_family);
		advance_bp;
		for (i = 0, cp = (uchar_t *)sa->sa_data; i < 14; i++, cp++) {
			sprintf(bp, "%u.", *cp);	advance_bp;
		}
		break;
	}
#undef	advance_bp
	return (&buffer[0]);
}

#ifdef VSOCKET_DEBUG

/*
 * Routines used for debug printing of vsocket operations. See the file
 * <sys/vsocket.h> for details.
 */

int vsdebug = VSDEBERROR;

vsocket_db1(vs, op, fmt)
	struct socket *vs;
	char *op, *fmt;	
{
	if (vsdebug & VSDEBOPS) {
		vs->vs_debugfmt = fmt;
		printf("[%x]%s refcnt %d", vs, op, vs->vs_refcnt);
	}
	return(0);
}

int vs_mbuf_debug = -1;
int
vsocket_db2(vs, arg1, arg2, arg3, arg4, arg5, arg6)
	struct socket *vs;
	int arg1, arg2, arg3, arg4, arg5, arg6;
{
	if (vsdebug & VSDEBOPS)
		printf(vs->vs_debugfmt,vs,arg1,arg2,arg3,arg4,arg5,arg6);
	if(vs_mbuf_debug > 0 )
		printf("in mbufstat %d\n",mbstat.m_mtypes[vs_mbuf_debug]);
	return(0);
}

int
vsocket_db3(retval)
	int retval;
{
	if (vsdebug & VSDEBOPS)
		printf("   returns(%d)\n",retval);
	if(vs_mbuf_debug > 0 )
		printf("out mbufstat %d\n",mbstat.m_mtypes[vs_mbuf_debug]);
	return(retval);
}

int
vsocket_db4(vs, str1, str2, adj)
	struct socket *vs;
	char *str1, *str2;
	int adj;
{
	if (vsdebug & VSDEBREFCNT)
		printf(" %s %s: 0x%x ref_cnt=%d\n", 
		       str1, str2, 
		       vs, vs->vs_refcnt+adj);
	return(0);
}


void
vs_debug_receive_port(port)
{
	kern_return_t	error;
	mach_port_status_t	status;
	char *TrUe = "TRUE";
	char *FaLsE = "FALSE";

	error = mach_port_get_receive_status(mach_task_self(),port,&status);
	printf("Port name 0x%x ",port);
	switch(error) {
	case KERN_INVALID_TASK:
		printf("BAD TASK\n");
		break;
	case KERN_INVALID_NAME:
		printf("BAD PORT\n");
		break;
	case KERN_INVALID_RIGHT:
		printf("NOT A RECEIVE RIGHT\n");
		break;
	case KERN_SUCCESS:
		printf("\n");
		break;
	default:
		printf(" UNKNOWN ERROR %x\n",error);
		break;
	}

	if(error != KERN_SUCCESS)  
		return;
	
	printf("containing port set 0x%x     seq. no %d     ",
		status.mps_pset,status.mps_seqno);
	printf("make-send count %d     queue limit %d\n",
		status.mps_mscount,status.mps_qlimit);
	printf("send-once rights %d     # in queue %d\n",
		status.mps_sorights,status.mps_msgcount);
	printf("Send rights exist %s, port-deleted requested %s no-senders requetsed %s\n",
	status.mps_srights?TrUe:FaLsE,status.mps_pdrequest?TrUe:FaLsE,
	status.mps_nsrequest?TrUe:FaLsE);
}	
	
	
void
vs_debug_port_type(port)
mach_port_t		port;
{
	mach_port_t	this_task = mach_task_self();
	kern_return_t	error;
	mach_port_type_t	type;
	mach_port_urefs_t	urefs;

	error = mach_port_type(this_task,port,&type);

	if(error == KERN_SUCCESS) {
		if( (type & MACH_PORT_TYPE_SEND) && 
			!mach_port_get_refs(this_task,port,MACH_PORT_RIGHT_SEND,
			&urefs))
			printf("Is a SEND_PORT with %d refs\n",urefs);
		if( (type & MACH_PORT_TYPE_RECEIVE) && 
			!mach_port_get_refs(this_task,port,MACH_PORT_RIGHT_RECEIVE,
			&urefs))
			printf("Is a RECEIVE_PORT with %d refs\n",urefs);
		if( (type & MACH_PORT_TYPE_SEND_ONCE) && 
			!mach_port_get_refs(this_task,port,MACH_PORT_RIGHT_SEND_ONCE,
			&urefs))
			printf("Is a SEND_ONCE_PORT with %d refs\n",urefs);
		if( (type & MACH_PORT_TYPE_PORT_SET) && 
			!mach_port_get_refs(this_task,port,MACH_PORT_RIGHT_PORT_SET,
			&urefs))
			printf("Is a PORT_SET_PORT with %d refs\n",urefs);
		if( (type & MACH_PORT_TYPE_DEAD_NAME) && 
			!mach_port_get_refs(this_task,port,MACH_PORT_RIGHT_DEAD_NAME,
			&urefs))
			printf("Is a DEAD_NAME_PORT with %d refs\n",urefs);
		if( type & MACH_PORT_TYPE_DNREQUEST) 
			printf("Has a PORT_DNREQUEST registered\n");
		if( type & MACH_PORT_TYPE_MAREQUEST) 
			printf("Has a PORT_MAREQUEST registered\n");
		if( type & MACH_PORT_TYPE_COMPAT) 
			printf("Is a Compatibility mode port\n");

	}
	else
		printf("mach_port_type error = %x\n",error);
}


vs_debug_port(port,str)
mach_port_t	port;
char *str;
{
	printf("%s %x\n",str,port);
	vs_debug_port_type(port);
	vs_debug_receive_port(port);
}

mach_port_t	vs_port_to_debug = MACH_PORT_NULL;


vs_show_node_binding(
	struct socket	*vs,
	mach_port_t	socket_port)
{
	kern_return_t	kr;
	node_t		node;

	kr = norma_port_location_hint(mach_task_self(), socket_port, &node);
	if (kr == KERN_SUCCESS) {
	    printf("primary vs 0x%x; secondary on node %d (port 0x%x)\n",
		   vs, node, socket_port);
	} else {
	    printf("primary vs 0x%x; secondary on unknown node (kr 0x%x)\n",
		   vs, kr);
	}
}


void
vs_frefs(
	struct file	*fp,
	char		*str)
{
	printf("%s: fp=0x%x count=%d-1 make=%d rel=%d svr=%d seq=%d\n",
	       str, fp, fp->f_count, fp->f_makesend, fp->f_relsend,
	       fp->f_svrsend, fp->f_seqno);
}

#endif /* VSOCKET_DEBUG */
