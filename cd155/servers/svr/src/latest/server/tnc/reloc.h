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
 * $Log: reloc.h,v $
 * Revision 1.6  1995/02/01  21:46:33  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:43:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:33:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/09  15:06:27  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.1.1.3  1993/07/01  20:45:32  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:22:41  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:45:36  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:47:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:45:45  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.7  93/07/07  16:40:19  bhk
 * added return codes to allow the stack to be unwound in case of a deadlock
 * 
 * Revision 3.6  93/06/14  13:53:57  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Upped the #define for the maximum number of ports sent in a relocation msg.
 * In this case, we're worried about a large number of files for a named pipe.
 * 
 * Revision 3.5  92/09/28  13:24:43  klh
 * Move all definitions not related to the out-of-line transport RPC to
 * <tnc/un.h> instead (klh for mjl).
 *
 * Revision 3.3  92/08/17  13:02:50  mjl
 * XXX This redefinition of INVALID_NODE is bogus and should be nuked.
 * 
 * Revision 3.2  92/08/08  01:43:22  jdh
 *  
 * policy stuff has been rewritten by mjl -- jdh
 * 
 * Revision 3.1  92/06/16  18:39:18  bhk
 * commented out RELOC_DEBUG definition
 * 
 * Revision 3.0  92/06/11  15:58:00  mjl
 * General definitions for UNIX domain socket/pipe/FIFO relocation.
 * 
 */

#ifndef	_H_TNC_RELOC
#define	_H_TNC_RELOC

/*
 * Note - RELOC_PORT_ARRAY_SIZE was increased to handle the unlikely event
 *        of a fifo being relocated with many file ports.
 */
#define MAGIC_MACH_MSG_SIZE	8192
#define RELOC_PORT_ARRAY_SIZE	64

/*
 * these constants are returned by tnc_fsvr_end_op
 */
#define	RELOC_SUCCEEDED		1
#define	RELOC_LOCKED		-1
#define	RELOC_NOT_NEEDED	0
/*
 *  Magic number "52" comes from examining the Request structure generated
 *  by MIG for the tnc_relocate_inline() RPC.
 */
#define MAX_BLOB_SIZE	\
	( MAGIC_MACH_MSG_SIZE - \
	  (52 + ( (RELOC_PORT_ARRAY_SIZE * sizeof(mach_port_t)) << 1 ) ) )

#endif	/* ! _H_TNC_RELOC */
