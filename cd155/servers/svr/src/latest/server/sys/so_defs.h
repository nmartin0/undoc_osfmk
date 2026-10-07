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
 * $Log: so_defs.h,v $
 * Revision 1.10  1995/02/01  21:37:38  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.9  1994/11/18  20:41:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1993/07/14  18:27:38  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.5  1993/07/01  20:36:56  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/06  19:19:58  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.6  1993/04/03  03:07:55  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.2  1993/02/16  20:05:00  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.5  1993/01/15  01:56:58  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  06:01:13  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 3.11  92/12/11  14:56:12  mjl
 * No more VSOP_VSOCK{CREATE,DESTROY}() ops.  Make VSOP_VSOCKCONNECT() call
 * unp_local_connect() in non-TNC builds.
 * 
 * Revision 3.10  92/10/28  08:19:08  klh
 * fix definition of VSOP_RESERVE, delete extraneous VSOP_GETADDR definition
 * 
 * Revision 3.9  92/10/27  17:39:55  bhk
 * Fixed sosleep to follow the other so.... defines
 * removed VSOP_USRREQ and replaced it woth VSOP_GETADDR
 * added VSOP_RESERVE for soreserve calls
 * 
 * Revision 3.8  92/08/17  12:55:40  mjl
 * Revision 3.11  1992/12/11  14:56:12  mjl
 * No more VSOP_VSOCK{CREATE,DESTROY}() ops.  Make VSOP_VSOCKCONNECT() call
 * unp_local_connect() in non-TNC builds.
 *
 * Revision 3.10  92/10/28  08:19:08  klh
 * fix definition of VSOP_RESERVE, delete extraneous VSOP_GETADDR definition
 * 
 * Revision 3.9  92/10/27  17:39:55  bhk
 * Fixed sosleep to follow the other so.... defines
 * removed VSOP_USRREQ and replaced it woth VSOP_GETADDR
 * added VSOP_RESERVE for soreserve calls
 * 
 * Revision 3.8  92/08/17  12:55:40  mjl
 * Got rid of extraneous arg to VSOP_SLEEP().
 * 
 * Revision 3.7  92/08/13  13:21:24  klh
 * Add Virtual operation selectors for non TNC code
 * 
 * Revision 3.6  92/08/12  16:52:00  klh
 * Remove extraneous "--" in non TNC ifdef
 * 
 * Revision 3.5  92/08/08  01:40:59  jdh
 * 3 new default vsops definitions;  also changed VSOP_SLEEP
 * XXX get rid of extra field -- don't need anymore -- jdh
 * 
 * Revision 3.4  92/05/06  14:22:56  bhk
 * added VSOP_SLEEP define to allow non-tnc compilation
 * 
 * Revision 3.3  92/04/22  16:58:39  bhk
 * Added  virutal protocol userreq operation.
 * 
 * Revision 3.2  92/04/20  17:27:59  bhk
 * Added VSOP_SHUTDOWN definition
 * 
 * Revision 3.1  92/04/06  14:51:21  klh
 * Add VSOP_CONNECT2 for non TNC code
 * 
 * Revision 3.0  92/03/28  10:37:05  chrisp
 * Initial checkin on behalf of the Boston Brigade.
 * 
 */
#ifndef _SO_DEFS_H_
#define _SO_DEFS_H_

#ifndef TNC

/*
 * The following defines are used to when vsockets are not present
 */
#define VSOP_CREATE		socreate
#define VSOP_CLOSE		soclose
#define VSOP_SEND		sosend
#define VSOP_RECEIVE		soreceive
#define VSOP_DEQUEUE		sodequeue
#define VSOP_LISTEN		solisten
#define VSOP_CONNECT		soconnect
#define VSOP_CONNECT2		soconnect2
#define VSOP_BIND		sobind
#define VSOP_SHUTDOWN		soshutdown
#define	VSOP_SETOPT		sosetopt
#define	VSOP_GETOPT		sogetopt
#define VSOP_SHUTDOWN		soshutdown
#define	VSOP_GETADDR		sogetaddr
#define	VSOP_SLEEP		sosleep
#define VSOP_VSOCKCONNECT	unp_local_connect
#define	VSOP_RESERVE		soreserve

/*
 *  Virtual operation selectors for AF_UNIX SOCK_STREAM
 */
#define VOS_NORMAL	0	/* No TNC storage site migration */
#define VOS_PIPE	0	/* Use pipe virtual ops */
#define VOS_SOCK	0	/* Use bind/connect virtual ops */
#define VOS_FIFO	0	/* Use FIFO virtual ops */

#endif /* ! TNC */

#endif  /* _SO_DEFS_H_ */
