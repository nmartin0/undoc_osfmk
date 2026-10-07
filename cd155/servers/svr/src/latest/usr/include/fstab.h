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
 * $Log: fstab.h,v $
 * Revision 1.3  1994/11/18  21:09:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:19  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:25  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:49  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:29  nandy
 * Initial revision
 *
 * Revision 2.7  92/04/13  14:35:32  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.7  90/10/07  20:27:31  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:45:31  gm]
 * 
 * Revision 2.6  90/08/09  14:26:13  devrcs
 * 	Changes for reentrant C library
 * 	[90/08/02  09:55:48  sp]
 * 
 * Revision 2.5  90/03/13  21:22:17  mbrown
 * 	changed __STCD__ to _NO_PROTO, endfsent is an INT, not a VOID
 * 	[90/02/24  15:04:19  mbrown]
 * 
 * Revision 2.4  90/02/23  00:35:56  devrcs
 * 	Changes for snapshot
 * 	[90/02/21  00:04:23  gm]
 * 
 * Revision 2.3  90/01/02  19:15:16  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 2.2  89/12/26  10:10:02  gm
 * 	Added function prototype declarations.
 * 	[89/06/30  11:56:28  bww]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	%W% (Berkeley) %G%
 */

#ifndef _FSTAB_H_
#define _FSTAB_H_
/*
 * File system table, see fstab(5).
 *
 * Used by dump, mount, umount, swapon, fsck, df, ...
 *
 * The fs_spec field is the block special name.  Programs that want to use
 * the character special name must create that name by prepending a 'r'
 * after the right most slash.  Quota files are always named "quotas", so
 * if type is "rq", then use concatenation of fs_file and "quotas" to locate
 * quota file.
 */
#define	_PATH_FSTAB	"/etc/fstab"
#define	FSTAB		"/etc/fstab"	/* deprecated */

#define	FSTAB_RW	"rw"		/* read/write device */
#define	FSTAB_RQ	"rq"		/* read/write with quotas */
#define	FSTAB_RO	"ro"		/* read-only device */
#define	FSTAB_SW	"sw"		/* swap device */
#define	FSTAB_XX	"xx"		/* ignore totally */

struct fstab {
	char	*fs_spec;		/* block special device name */
	char	*fs_file;		/* file system path prefix */
	char	*fs_type;		/* FSTAB_* */
	int	fs_freq;		/* dump frequency, in days */
	int	fs_passno;		/* pass number on parallel dump */
	char	*fs_vfstype;		/* File system type, ufs, nfs */
	char	*fs_mntops;		/* Mount options ala -o */
};

#ifdef _NO_PROTO
struct	fstab *getfsent();
struct	fstab *getfsspec();
struct	fstab *getfsfile();
int	setfsent();
int	endfsent();
#if defined(_REENTRANT) || (_THREAD_SAFE)
int     getfsent_r();
int     getfsspec_r();
int     getfsfile_r();
#endif
#else
extern struct fstab *getfsent(void);
extern struct fstab *getfsspec(const char*);
extern struct fstab *getfsfile(const char*);
extern int setfsent(void);
extern int endfsent(void);
#if defined(_REENTRANT) || (_THREAD_SAFE)
extern int getfsent_r(struct fstab *, char*);
extern int getfsspec_r(const char *, struct fstab *, char*);
extern int getfsfile_r(const char *, struct fstab *, char*);
#endif
#endif	/* _NO_PROTO */
#endif	/* _FSTAB_H_ */
