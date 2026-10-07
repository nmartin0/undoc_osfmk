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
 * $Log: un_debug.c,v $
 * Revision 1.9  1995/02/01  22:00:53  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:44:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/18  00:49:29  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.6  1994/05/04  21:45:28  mjl
 * TNC select rewrite.  In print_port_info(), print the "type" bits as
 * well.  Move the routine outside of UN_DEBUG so non-assert servers can
 * use it for pre-panic output.
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
 * Revision 1.5  1993/07/14  18:35:29  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:49:18  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/17  19:07:03  cfj
 * 05-06-93 MI driver drop from Locus.
 *
 * Revision 1.3  1993/05/06  19:27:01  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:49  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1993/03/07  01:27:45  cfj
 * Merge from T9.
 *
 * Revision 1.1.8.1  1993/03/07  01:15:34  cfj
 * Fix to make TNC work with OSF sync. close.
 *
 * Revision 1.1  1993/01/15  02:26:23  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 3.4  93/06/24  12:45:13  mjl
 * Change print_port_info() printf strings to disambiguate # of sorights held
 * versus # of sorights in existence.
 * 
 * Revision 3.3  93/06/14  13:58:23  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Enable pipe relocation
 * 
 * Revision 3.2  93/05/07  19:06:34  mjl
 * Fix typo.
 * 
 * Revision 3.1  1993/03/10  14:17:57  yazz
 * Disable pipe relocation temporarily until synchronous close code
 * can be written for vsockets/tnc.
 *
 * Revision 3.0  92/12/10  16:58:11  mjl
 * Debugging routines for TNC Unix domain sockets.
 * 
 */

#include "tnc/un_debug.h"

#ifdef	UN_DEBUG

int	un_si_alloced		= 0;
int	un_si_freed		= 0;

#ifdef	NO_PIPE_RELOC
int	un_enable_relocation	= 0;
#else
int	un_enable_relocation	= 1;
#endif
int	undebug			= 0x0;	/* */
int	sosleep_debug		= 0x0;	/* */
/* #define	X_OFFSET(a,b)	((int)(b) - (int)(a)) */

#endif	/* UN_DEBUG */


void
print_port_info(
	mach_port_t	name,
	char		*str)
{
	kern_return_t		kr;
	mach_port_urefs_t	srefs, sorefs, rrefs;
	mach_port_status_t	mps;
	mach_port_type_t	type;

	printf("%s>> Port 0x%x: ", str, name);

	kr = mach_port_type(mach_task_self(), name, &type);
	if ( kr == KERN_INVALID_NAME ) {
		printf("invalid name\n");
		return;
	} else if ( kr != KERN_SUCCESS ) {
		printf("unexpected: mach_port_type 0x%x\n", kr);
	}
	kr = mach_port_get_refs(mach_task_self(), name,
				MACH_PORT_RIGHT_SEND, &srefs);
	if ( kr != KERN_SUCCESS )
		printf("unexpected: mach_port_get_refs 0x%x\n", kr);
	kr = mach_port_get_refs(mach_task_self(), name,
				MACH_PORT_RIGHT_SEND_ONCE, &sorefs);
	if ( kr != KERN_SUCCESS )
		printf("unexpected2: mach_port_get_refs 0x%x\n", kr);
	kr = mach_port_get_refs(mach_task_self(), name,
				MACH_PORT_RIGHT_RECEIVE, &rrefs);
	if ( kr != KERN_SUCCESS )
		printf("unexpected3: mach_port_get_refs 0x%x\n", kr);

	printf("type=0x%x srights=%d sorights(held)=%d", type, srefs, sorefs);

	if (rrefs == 0) {
		printf("\n");
		return;
	}

	kr = mach_port_get_receive_status(mach_task_self(), name, &mps);
	if ( kr != KERN_SUCCESS )
		printf("\nunexpected4: mach_port_get_receive_status: 0x%x\n",
		       kr);
	printf(" rright pset=0x%x seqno=%d mscount=%d\n",
	       mps.mps_pset,
	       mps.mps_seqno,
	       mps.mps_mscount);
	printf("%s (cont'd)>> qlim=%d qmsgs=%d sorights(made)=%d%s%s%s\n",
	       str,
	       mps.mps_qlimit,
	       mps.mps_msgcount,
	       mps.mps_sorights,
	       ( mps.mps_srights ? " srights" : "" ),
	       ( mps.mps_pdrequest ? " pdreq" : "" ),
	       ( mps.mps_nsrequest ? " nmsreq" : "" ));
}
