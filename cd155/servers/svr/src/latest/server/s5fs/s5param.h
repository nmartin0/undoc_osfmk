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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: s5param.h,v $
 * Revision 1.5  1994/11/18  20:38:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:18:55  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:12:55  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:18  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:38:11  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:36:09  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:34:01  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:32:29  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:57:46  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/06/27  15:50:59  sp
 * Incorporated from 1.0.2
 * 
 * Revision 1.3  90/10/07  14:41:29  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:26:08  gm]
 * 
 * Revision 1.2  90/04/27  19:21:30  devrcs
 * 	Merged for snapshot.
 * 	[90/04/20  17:36:36  morris]
 * 
 * 	Initial version from system V.2.
 * 	[90/04/20  15:15:46  morris]
 * 
 * $EndLog$
 */
/* @(#)s5param.h  */
#ifndef _S5_PARAM_H_
#define _S5_PARAM_H_
/*
 * Fundamental variables and macros for the System V file system
 * don't change too often
 * 
 */
			/* Some day we should move it to where it belong */

typedef ushort			s5ino_t;

#define	s5ROOTINO	((s5ino_t)2)	/* i number of all roots */

#define	SUPERB	((daddr_t)1)	/* physical block number of the super block */
#define	SUPERBOFF	512	/* byte offset of the super block */
#define	s5DIRSIZ	14	/* max characters per directory */
#define	NICINOD	100		/* number of superblock inodes */
#define	NICFREE	50		/* number of superblock free blocks */


#define MAX_S5BSIZE	2048	/* max bsize supported */

#define	FsBSIZE(fp) /* bsize of S5FS of fp */		\
	(((fp)->s_type == Fs3b) ? 2048 :		\
	 ((fp)->s_type == Fs2b) ? 1024 : 512 )


#define	FsBSHIFT(bsize) /* LOG2(bsize) */		\
	(((bsize) == 2048) ? 11 : 			\
	 ((bsize) == 1024) ? 10 : 9)

#define	FsNINDIR(bsize) /* bsize/sizeof(daddr_t)) */	\
	(((bsize) == 2048) ? 512 : 			\
	 ((bsize) == 1024) ? 256 : 128)

#define	FsBMASK(bsize)	/* bsize-1 */			\
	(((bsize) == 2048) ? 03777 : 			\
	 ((bsize) == 1024) ? 01777 : 0777)

#define	FsBOFF(bsize, x) /* offset in block */		\
	(((bsize) == 2048) ? ((x)&03777) :		\
	 ((bsize) == 1024) ? ((x)&01777) : ((x)&0777))

#define	FsBNO(bsize, x)	/* logical block number	*/	\
        (((bsize) == 2048) ? ((x)>>11) : 		\
	 ((bsize) == 1024) ? ((x)>>10) : ((x)>>9))

#define	FsINOPB(bsize)	/* inodes per block */		\
        (((bsize) == 2048) ? 32 : 			\
	 ((bsize) == 1024) ? 16 : 8)
	
#define	FsLTOP(bsize, b) /* logical to phys block */    \
        (((bsize) == 2048) ? (b)<<2 : 			\
	 ((bsize) == 1024) ? (b)<<1 : (b))

#define	FsPTOL(bsize, b)  /* phys to logical block */	\
        (((bsize) == 2048) ? (b)>>2 : 			\
	 ((bsize) == 1024) ? (b)>>1 : (b))

#define	FsNMASK(bsize) /*  NMASK <==> NINDIR - 1 */	\
        (((bsize) == 2048) ? 0777 : 			\
	 ((bsize) == 1024) ? 0377 : 0177)

#define	FsNSHIFT(bsize)	/* LOG2(NINDIR) */		\
        (((bsize) == 2048) ? 9 : 			\
	 ((bsize) == 1024) ? 8 : 7)

#define	FsINOS(bsize, x)					\
	(((bsize) == 2048) ? (((x)&~037)+1) : 			\
	 ((bsize) == 1024) ? (((x)&~017)+1) : (((x)&~07)+1) )

#define	FsITOD(bsize, x) /* Inode number to disk block */      \
        (daddr_t) (((bsize) == 2048) ? ((unsigned)(x)+63)>>5 : \
        (((bsize) == 1024) ? \
	((unsigned)(x)+31)>>4 : ((unsigned)(x)+15)>>3))

#define	FsITOO(bsize, x) /* Inode offset within disk block */   \
        (daddr_t) (((bsize) == 2048) ? ((unsigned)(x)+63)&037 : \
        ((bsize) == 1024) ? \
	((unsigned)(x)+31)&017 : ((unsigned)(x)+15)&07 )
#endif _S5_PARAM_H_
