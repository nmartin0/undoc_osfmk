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
 * $Log: s5dir.h,v $
 * Revision 1.5  1994/11/18  20:38:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:17:55  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:11:55  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:28:25  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:37:48  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:35:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:33:26  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:31:24  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:55:16  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/06/27  15:50:41  sp
 * Incorporated from 1.0.2
 * 
 * Revision 1.3.4.2  91/06/21  12:51:33  garyf
 * 	merge fixes from 1.1
 * 	[91/06/18  10:28:08  garyf]
 * 
 * Revision 1.4.2.2  91/06/17  11:11:45  garyf
 * 	add lock for readdirbuf
 * 	[91/06/17  11:03:35  garyf]
 * 
 * Revision 1.4  91/03/04  17:19:30  devrcs
 * 	A small step toward ansiC: commented else/endif/elif trailers.
 * 	[91/01/12  17:00:13  dwm]
 * 
 * Revision 1.3  90/10/07  14:40:33  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:24:40  gm]
 * 
 * Revision 1.2  90/04/27  19:20:41  devrcs
 * 	Merge for snapshot.
 * 	[90/04/20  17:29:51  morris]
 * 
 * 	Initial version from System V.2
 * 	[90/04/17  14:44:53  morris]
 * 
 * 	[90/04/02  12:35:10  morris]
 * 
 * $EndLog$
 */
/* @(#)s5dir.h	 */
#ifndef _S5_DIR_H_
#define _S5_DIR_H_

#include <s5fs/s5param.h>
/*
 * NOTE: s5ino_t is defined as ushort while ino_t is defined as ulong in UFS
 *
 */

struct	s5direct
{
	s5ino_t	d_ino;
	char	d_name[s5DIRSIZ];
};

/*
 * Used by s5fs_readdir to create file system independent directory entries.
 * This format is required by directory routines in libc.
 */
#ifdef _KERNEL
lock_data_t readbuflock;
#endif /* _KERNEL */
struct  s5direct *readdirbuf;
#define s5DIRECTSIZE sizeof(struct s5direct)

struct  gpdirect
{
        u_long  d_ino;
	u_short d_reclen;
	u_short d_namlen;
	char    d_name[s5DIRSIZ];
        u_short d_padding;
} *gpdirect;
#define gpDIRECTSIZE (sizeof(struct gpdirect))


struct  s5dirtemp
{
	s5ino_t	dot_ino;
	char	dot_name[s5DIRSIZ];
	s5ino_t	dotdot_ino;
	char	dotdot_name[s5DIRSIZ];
};

#endif  /*_S5_DIR_H_*/
