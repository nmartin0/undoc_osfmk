/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
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
 * $Log: dump.h,v $
 * Revision 1.6  1994/11/19  03:05:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/04/22  00:37:13  brad
 * Merged in revision 1.3.6.2 from the R1.2 branch.
 *
 * Revision 1.3.6.2  1994/04/22  00:12:16  brad
 * Fixed support for Mapped File reserved blocks, and added support for
 * preallocated disk blocks.  Now if a block is marked reserved, write
 * a block of zeroes to tape (rather than skipping the block and causing
 * "resync restore, skipped <n> blocks" errors on restore).
 *
 *  Reviewer: Dave Minturn
 *  Risk: Low
 *  Benefit or PTS #: 9093
 *  Testing: Developer testing, MCAT
 *  Module(s): usr/sbin/dump/{dump.h,dumptraverse.c}
 *
 * Revision 1.4  1994/01/26  22:25:15  dleslie
 *  Reviewer: Shala Arshi
 *  Risk: low
 *  Benefit or PTS #: change definitions of read and write to match the new
 * 	POSIX compliant ones in unistd.h
 *  Testing: builds
 *  Module(s): dump.h
 *
 * Revision 1.3  1993/04/06  20:37:37  shala
 * Fixed the defaults for device and partition.
 *
 * Revision 1.2  1992/10/12  21:46:29  shala
 * New version to understand maj, min and node numbers.
 *
 * Revision 2.9  90/10/07  22:07:35  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:08:38  gm]
 * 
 * Revision 2.8  90/08/25  12:28:23  devrcs
 * 	insert rcsid string;
 * 	remove #ifdef NLS and #else branch;
 * 	[90/08/09  03:50:31  nm]
 * 
 * Revision 2.7  90/07/27  11:18:22  devrcs
 * 	replace MAXNAMLEN by NAME_MAX;
 * 	[90/07/12  10:05:20  nm]
 * 
 * Revision 2.6  90/07/17  12:38:29  devrcs
 * 	More changes for gcc.
 * 	[90/07/08  17:39:43  gm]
 * 
 * 	cast off_t in lseek()
 * 	[90/07/05  08:31:30  nm]
 * 
 * 	change default names for tape and disk, insert SERVER_PATH define
 * 	[90/06/22  06:31:21  nm]
 * 
 * 	integration to osc.12
 * 	[90/05/31  04:36:55  nm]
 * 
 * 	delete NLctime and NLstrtime
 * 	[90/05/08  05:37:26  nm]
 * 
 * Revision 2.4  90/04/14  00:12:32  devrcs
 * 	integration of Jim's code
 * 	[90/04/10  08:08:06  nm]
 * 
 * 	Merged AIX3.1 and BSD4.3 to one source.
 * 	Made major changes fixing that code up. (Inserted comments too!)
 * 	Merged (BSD4.3 --> CMU) changes with what resulted.
 * 	Disabled edump (cannot quite figure out what it wants to do,
 * 		also not supported by rmt).
 * 	[90/01/11            walkerj]
 * 
 * Revision 2.3  90/01/02  18:41:30  gm
 * 	Fixes for first snapshot.
 * 	[90/01/02            gm]
 * 
 * Revision 2.2  89/12/26  08:56:38  gm
 * 	Current version from CMU.
 * 	[89/12/21            gm]
 * 
 * 	Make include file changes for SUNOS vs MACH.
 * 	[89/08/31  21:36:30  ern]
 * 
 * 	Add a better version of howmany macro. This version is slower but is
 * 	not subject to wrapping with large integers that occur for large files.
 * 	[89/07/31  00:25:43  ern]
 * 
 * $EndLog$
 */
/*
 * @(#) $RCSfile: dump.h,v $ $Revision: 1.6 $ (OSF) $Date: 1994/11/19 03:05:32 $
 */
/*
 * This module contains IBM CONFIDENTIAL code. -- (IBM Confidential Restricted
 * when combined with the aggregated modules for this product) OBJECT CODE ONLY
 * SOURCE MATERIALS (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/*	system include files	*/

#include	<stdio.h>
#include	<ctype.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<ufs/fs.h>
#include	<ufs/inode.h>
#include	<protocols/dumprestore.h>
#include	<sys/dir.h>
#include	<utmp.h>
#include	<sys/time.h>
#include	<sys/file.h>
#include	<signal.h>
#include	<fstab.h>
#include	<limits.h>
#include	<varargs.h>

#ifdef  OSF1_ADFS
#define BLK_RESERVE_SUPPORT	/* reserved blocks are understood */
#endif

/* global definition of important spcl record */

#define	spcl	u_spcl.s_spcl

extern union u_spcl	u_spcl;

/*	local defines		*/

#define	min(a, b)	(((a) < (b))? (a): (b))
#define	max(a, b)	(((a) > (b))? (a): (b))

/* define our own howmany which will work with values like the largest int */

#undef	howmany
#define	howmany(x, y)	(((x) / (y)) + (((x) % (y) != 0)? 1 : 0))

#define	TRUE	1
#define	FALSE	0

#define	YES	1
#define	NO	0

#define	MINUTE	60
#define	HOUR	(60 * MINUTE)
#define	DAY	(24 * HOUR)

/*
 * Exit	status codes
 */

#define	X_FINOK		0		/* normal exit */
#define	X_FINBAD	1		/* bad exit */
#define	X_REWRITE	2		/* rewrite from the checkpoint */
#define	X_ABORT		3		/* abort all of dump; don't attempt
					 * checkpointing */

/*
 * Devices, Files, Operators, Etc.
 */

#define	DEFAULT_TAPE	"/dev/io0/rmt6"		/* default tape device */
#define	DEFAULT_DISK	"/dev/io0/rrz0g"	/* default disk */
#define	DUMP_HISTORY	"/etc/dumpdates"	/* dump_history info file */
#define	DIALUP_PREFIX	"ttyd"			/* prefix for dialups */
#define	OPERATOR_GROUP	"operator"		/* group entry to notify */
#define SERVER_PATH	"/usr/sbin/rmt"		/* path of the remote daemon */

/*	system function declarations	*/

extern ssize_t		read();
extern ssize_t		write();
extern long		atol();
extern long		ftell();
extern off_t		lseek();
extern long		time();
extern unsigned		sleep();
extern char	       *ctime();
extern char	       *getenv();
extern char	       *index();
extern char	       *mktemp();
extern char	       *rindex();
extern char	       *strchr();
extern char	       *strcpy();
extern char	       *strncpy();
extern void		abort();
extern void		bzero();
extern void		exit();
extern void		perror();
extern void		sync();
extern void	       *calloc();
extern void	       *malloc();

/*	system external variables	*/

extern int		errno;
extern int		optind;
extern char	       *optarg;

/*	local function declarations	*/

extern int		check_tape();
extern int		query();
extern int		set_operators();
extern struct fstab    *fstabsearch();	/* search in fs_file and fs_spec */
extern time_t		unctime();
extern char	       *prdate();
extern char	       *rawname();
extern void		Exit();
extern void		abort_dump();
extern void		bitmap();
extern void		broadcast();
extern void		bread();
extern void		close_at_end();
extern void		dmpblk();
extern void		dump_perror();
extern void		open_at_start();
extern void		getfstab();
extern void		job_trailer();
extern void		lastdump();
extern void		msg();
extern void		msgtail();
extern void		rewrite_tape();
extern void		taprec();
extern void		volume_label();

/*	remote function declarations	*/

extern void		rmtclose();
extern void		rmthost();
extern int		rmtioctl();
extern int		rmtopen();
extern int		rmtread();
extern int		rmtseek();
extern int		rmtwrite();

/*	routines which deal with dump history file */

extern void		getitime();
extern void		putitime();
extern void		inititimes();

/*	pass controller and four passes over the file system	*/

extern void		pass();
extern void		mark();
extern void		add();
extern void		dirdump();
extern void		dump();

/*	local structures definitions	*/

/*
 * The contents of the file DUMP_HISTORY is maintained both on a linked list,
 * and then (eventually) arrayified.
 */

struct idates
{
	char		id_name[NAME_MAX + 3];
	char		id_incno;
	time_t		id_ddate;
};

struct itime
{
	struct idates	it_value;
	struct itime   *it_next;
};

/*	local external variables	*/

extern int		imap_size;	/* inode map size in bytes	*/
extern char	       *not_clear_map;	/* bit map of inodes which are	*/
					/* (not) clear at time of dump	*/
					/* (0 == clear, 1 == not clear)	*/
extern char	       *directory_map;	/* bit map of inodes which are	*/
					/* directories and eligible for	*/
					/* dumping			*/
					/* (0 == not dir, 1 == is dir)	*/
extern char	       *to_dump_map;	/* bit map of inodes which are	*/
					/* still to be dumped		*/
					/* (0 == do not dump, 1 == do)	*/

extern char	       *disk_file_name;		/* name of the disk file */
extern char	       *tape_file_name;		/* name of the tape file */
extern char	       *dump_hist_file_name;	/* name of the file containing
						 * dump history information */

extern char		incr_num;		/* increment number */
extern char		last_incr_num;		/* increment number of previous
						 * dump */

extern ino_t		curr_inum;	/* current inumber; used globally */

extern int		new_tape_flag;	/* new tape flag */
extern int		curr_tape_num;	/* current tape number */
extern int		curr_volume_num; /* current volume number */

extern int		dir_added_flag;	/* true if added directory to to-be-
					 * dumped list on last pass-two */
extern int		dir_skipped_flag; /* set TRUE in mark() if any
					 * directories are skipped; this
					 * lets us avoid map pass 2 in
					 * some cases */

extern int		by_blocks_flag; /* TRUE when measuring output by blocks,
					 * FALSE when by inches */

extern long		full_tape_blocks; /* full tape size in blocks */
extern long		curr_tape_blocks; /* current tape size in blocks */
extern double		full_tape_inches; /* full tape size in inches */
extern double		curr_tape_inches; /* current tape size in inches */
extern long		blocks_on_curr_tape; /* number of blocks written on
					 * current tape */
extern double		inches_on_curr_tape; /* number of inches written on
					 * current tape */
extern int		tape_density;	/* density in bytes/inch */
extern long		est_tot_blocks;	/* estimated total number of blocks */
extern double		est_tot_inches;	/* estimated total number of inches */
extern int		est_tot_tapes;	/* estimated number of tapes */
extern int		blocks_per_write; /* blocking factor on tape */
extern double		inches_per_write; /* inches for each tape write */

extern long		dev_bsize;	/* machine device block size in bytes */

extern int		in_disk_fd;	/* disk file descriptor */
extern int		out_tape_fd;	/* tape file descriptor */

extern int		pipe_out_flag;	/* true => output to standard output */
extern int		medium_flag;	/* type of medium (see flags below) */
extern int		no_rewind_flag;	/* do not rewind tape */

extern int		notify_flag;	/* notify operators */

extern int		num_idate_records; /* number of idate records (might
					 * be zero) */
extern struct idates  **idate_array;	/* pointer to array of pointers to */
					/* dump_history idates structures */

extern struct fs       *super_block;	/* the file system super block */

/* values for medium_flag */
#define	NO_MEDIUM	0		/* medium is not known */
#define	REGULAR_FILE	1		/* medium is a regular file */
#define TAPE		2		/* medium is a tape */
#define CARTRIDGE	3		/* medium is a cartridge */
#define DISKETTE	4		/* medium is a diskette */

/*	NLS stuff	*/
#include	<locale.h>
#include	"dump_msg.h"

extern nl_catd		catopen();
extern char	       *catgets();

extern nl_catd		catd;
extern char	       *yes_str;
extern char	       *no_str;
extern int		yes_flag;
extern int		no_flag;

#define	MSGSTR(Num, Str)	catgets(catd, MS_DUMP, Num, Str)
