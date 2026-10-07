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
 * $Log: tnc_mig.c,v $
 * Revision 1.8  1995/02/01  21:55:30  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.7  1994/11/18  20:44:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/04/28  19:15:12  chrisp
 * tncgen: autugenerated code now derived from *.ops definition files
 * and processed by tncgen and associated gen*.sh scripts. #ifdefs
 * introduced where previously they were ignored.
 *
 *  Reviewer: dleslie, cfj
 *  Risk: M
 *  Benefit or PTS #: 9188
 *  Testing: Builds, builds, builds.
 *  Module(s):
 *      Modified Files:
 *  	dpvproc.h dvp_pvpsops.c dvp_vpops.c rvp_pvpops.c
 *  	rvp_pvpops_server.c rvp_pvpsops.c rvp_pvpsops_server.c
 *  	tnc.defs tnc_mig.c
 *      Added Files:
 *  	gen_client_stubs.sh gen_mig.sh gen_migrate_wrappers.sh
 *  	gen_server_stubs.sh pvp.ops pvps.ops tncgen.mk
 *      Removed Files:
 *  	dpvproc_struct.h makeTNCtables.sh
 *
 * Revision 1.5  1993/07/20  21:39:57  nandy
 * Added definition of rpc_waittime.
 *
 * Revision 1.4  1993/07/14  18:34:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:48:31  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:26:08  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:27  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:48:53  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:46:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.5  1993/04/22  16:41:23  roman
 * Remove unnecessary copyrights and RCS messages (they came from
 * 	another file.
 *
 * Revision 3.4  92/04/01  16:22:03  roman
 * Include the MiG generated TNC asynchronous files.
 * 
 * Revision 3.3  92/01/28  16:27:11  roman
 * Get rid of inclusion of mach_norma_user.c, which is now in the
 * microkernel library area.
 * 
 * Revision 3.2  91/10/30  14:29:09  klh
 * delete extra empty log message from an accidental checkin
 * 
 * Revision 3.0  91/10/21  13:21:06  roman
 * Kludge file that includes all MiG generated files. Initial submission.
 */
/*
 * Wrapper to compile bsd servers without KERNEL defined.
 */
#include <sys/time.h>
#ifdef NX
extern mach_msg_timeout_t rpc_waittime;
#endif /* NX */

#undef	KERNEL
#include <tnc/tnc_server.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <tnc/tnc_user.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <tnc/tnc_async_server.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <tnc/tnc_async_user.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <tnc/tnc_async_user_send.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <tnc/tnc_async_user_receive.c>
