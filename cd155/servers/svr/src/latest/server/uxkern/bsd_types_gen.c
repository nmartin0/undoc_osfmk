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
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: bsd_types_gen.c,v $
 * Revision 1.7  1995/02/01  22:18:16  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:47:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/14  18:39:58  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  20:59:06  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:25:32  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.3  1992/11/30  22:53:11  dleslie
 * Copy of NX branch back into main trunk
 * Revision 1.1.1.2  1993/05/03  17:50:58  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.2  1992/11/06  20:34:15  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:42:11  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.10  92/11/02  12:29:56  roman
 * Change the MAX_MULTI_LIST)_SIZE to a more reasonable number, now that
 * an out-of-line memory version of rforkmulti() exists.
 * 
 * Revision 2.9  92/10/28  16:00:11  roman
 * Add types for TNC-specific system calls.
 * 
 * Revision 2.8  92/06/10  10:06:49  klh
 * Revision 2.10  1992/11/02  12:29:56  roman
 * Change the MAX_MULTI_LIST)_SIZE to a more reasonable number, now that
 * an out-of-line memory version of rforkmulti() exists.
 *
 * Revision 2.9  92/10/28  16:00:11  roman
 * Add types for TNC-specific system calls.
 * 
 * Revision 2.8  92/06/10  10:06:49  klh
 * 	Revision 2.8  92/06/08  18:29:09  pjg
 * 		Add UIO_MAXIOV (pjg).
 * 		Add FHANDLE_SIZE, VATTR_SIZE and STATFS_SIZE (durriya).
 * 
 * Revision 2.7  92/04/01  16:24:41  roman
 * Add #define used for array sizes in (new) TNC rforkmulti() system call.
 * 
 * Revision 2.6  92/01/16  17:25:43  roy
 * 	92/01/16  13:29:32  pjg
 * 	Added definitions for types used by NFS.
 * 
 * Revision 2.5  91/11/22  15:20:28  rabii
 * 	locus merge
 * 	Added declaration of the size of the rusage_dev structure (needed
 * 	by bsd_types.defs). (roman)
 * 
 * Revision 2.4  91/10/04  15:18:35  chrisp
 * Add in Locus copyright.
 * 
 * Revision 2.3  91/09/17  11:28:14  sjs
 * integrate locus changes	roman
 * Add declaration of FILE_LIMIT, equivalent to NOFILE.  Also,
 * declarations added for credentials structures.
 * 
 * Revision 2.2  91/08/31  14:24:53  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.0  91/01/17  12:05:51  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/05/21  14:01:44  dbg
 * 	Return 0 to shell.
 * 	[90/03/14            dbg]
 * 
 */
/*
 * Generate definitions for Mig interfaces.  MiG can't handle random
 * C definitions or expressions any better than the assembler.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/user.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <sys/vnode.h>

main()
{
	printf("#define\tPATH_LENGTH %d\n",
			roundup(MAXPATHLEN,sizeof(int)));
	printf("#define\tSMALL_ARRAY_LIMIT %d\n",
			4096);
	printf("#define\tFD_SET_LIMIT %d\n",
			howmany(FD_SETSIZE, NFDBITS));
	printf("#define\tFILE_LIMIT %d\n",
			NOFILE);
	printf("#define\tGROUPS_LIMIT %d\n",
			NGROUPS);
	printf("#define\tHOST_NAME_LIMIT %d\n",
			MAXHOSTNAMELEN);
	printf("#define\tDOMAIN_NAME_LIMIT %d\n",
			MAXDOMNAMELEN);
	printf("#define\tCRED_SIZE %d\n",
			sizeof(struct ucred) / sizeof(int));
	printf("#define\tRUSAGE_SIZE %d\n",
			sizeof(struct rusage) / sizeof(int));
	printf("#define\tRUSAGE_DEV_SIZE %d\n",
			sizeof(struct rusage_dev) / sizeof(int));
	printf("#define\tNFS_ARGS_SIZE %d\n",
			sizeof(struct nfs_args) / sizeof(char));
	printf("#define\tSOCKADDR_IN_SIZE %d\n",
			sizeof(struct sockaddr_in) / sizeof(char));
	printf("#define\tNFSV2FH_SIZE %d\n",
			sizeof(nfsv2fh_t) / sizeof(char));
	printf("#define\tRLIMIT_TOTAL_ARRAY_SIZE %d\n",
			(RLIM_NLIMITS * sizeof(struct rlimit)) / sizeof(int));
	printf("#define\tSTAT_SIZE %d\n",
			sizeof(struct stat) / sizeof(int));
	printf("#define\tUIO_SIZE %d\n",
			sizeof(struct uio) / sizeof(int));
	printf("#define\tUIO_MAXIOV %d\n",
			UIO_MAXIOV);
	printf("#define\tFHANDLE_SIZE %d\n",
			(sizeof(struct fhandle)) / sizeof(char));
	printf("#define\tVATTR_SIZE %d\n",
			(sizeof(struct vattr)) / sizeof(char));
	printf("#define\tSTATFS_SIZE %d\n",
			(sizeof(struct statfs)) / sizeof(char));
#ifdef	TNC
	printf("#define\tMAX_MULTI_LIST_SIZE %d\n",
			42);		/* The answer to everything */
#endif	/* TNC */

	return (0);
};
