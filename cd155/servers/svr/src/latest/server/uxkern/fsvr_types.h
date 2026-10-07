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
 * This file was created by the Center for High Performance Computing of
 * Worcester Polytechnic Institute (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: fsvr_types.h,v $
 * Revision 1.7  1994/11/18  20:48:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/12/23  01:47:43  brad
 * Fixed various compilers warnings, lint errors, and lint warnings.
 *
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #: None.
 *  Testing: Booted and ran minimal PFS tests.
 *  Module(s): emulator/emul_stack_alloc.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_user_side.c
 *             emulator/pfs2_user_side.c
 *             server/pfs/pfs_vfsops.c
 *             server/uxkern/fsvr_types.defs
 *             server/uxkern/fsvr_server_side.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr_types.h
 *             server/uxkern/pfs2.defs
 *
 * Revision 1.5  1993/07/14  18:42:26  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:03:25  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:30:16  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:52:41  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.1.2.1  1992/12/16  22:48:28  dbm
 * Added PFS token functionality.
 *
 * Revision 1.2  1992/11/30  22:54:55  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:43:39  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:53:27  cfj
 * Bump major revision number.
 *
 * Revision 2.5  1992/06/08  18:26:33  pjg
 * 	Added definition for vattr_t (rabii)
 *
 * Revision 2.4  92/01/17  19:44:39  roy
 * 	Include sys/protosw.h (pjg).
 * 
 * Revision 2.3  92/01/16  17:17:28  roy
 * 	92/01/16  13:26:17  pjg
 * 	Added type definitions for NFS.
 * 
 * Revision 2.2  92/01/14  11:23:12  roy
 * 	92/01/05  23:43:59  noemi
 * 	Header file to correspond with fsvr_types.defs.
 * 
 */
#ifndef	_UXKERN_FSVR_TYPES_H_
#define	_UXKERN_FSVR_TYPES_H_

/*
 * Types for BSD kernel interface.
 */
#include <sys/types.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/user.h>
#include <sys/mount.h>
#include <sys/protosw.h>
#include <netinet/in.h>
#ifdef PFS
#include <sys/estat.h>
#endif

#include <uxkern/bsd_types_gen.h>

typedef struct statfs statfs_t;
typedef struct vattr vattr_t;

typedef struct nfs_args nfs_args_t;
typedef struct sockaddr_in sockaddr_in_t;

typedef	unsigned long	iomode_t;

#endif	_UXKERN_FSVR_TYPES_H_
