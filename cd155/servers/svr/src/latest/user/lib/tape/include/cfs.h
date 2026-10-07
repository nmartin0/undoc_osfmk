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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	cfs.h 11.3 94/03/25 14:11:58
 */
/** static char	cfsh_ver[] = "@(#) cfs.h 11.3 94/03/25 14:11:58";
/**/

/*
 *
 * . { TEXT
 * .H 4 cfs.h
 *
 * Basic declarations for CFS
 *
 * . } TEXT
 */

#include "estat.h"
typedef unsigned char unchar;
/* typedef unsigned long ulong; */

/* Logical disk block */
/* 16384 or 4096 */
typedef
struct block {
	unchar	byte[4096];
} BLOCK;
#define BLOCKSIZE (sizeof(BLOCK))

/* Logical block number */
typedef
struct bn {
	long	volume;
	ulong	block;
} BN;

#define BN_NE(x,y) ((x.volume != y.volume) || (x.block != y.block))
#define BN_EQ(x,y) ((x.volume == y.volume) && (x.block == y.block))
#define BNSIZE (sizeof(BN))
#define BNERROR(b) (b.volume < 0)
#define ERR_VOL		-1
#define DIR_VOL		-2
#define BUSY_VOL	-3

/* Volume label structure */
typedef
struct vol_label {
	long	reserved[4];	/*  reserved for bootstrap */
	ulong	version;	/* File system version */
	long	volume;		/* Volume number */
	long	numvol;		/* Number of volumes when label was written */
	time_t	ctime;		/* Creation time */
	ulong	size;		/* Size in blocks of entire volume */
	ulong 	ronly	: 1;	/* Read only flag */
	ulong		:31;	/*  reserved for more flags */
	char 	magic[30];	/* Volume label identifier */
	char	name[31];	/* File system name */
} VOL_LABEL;

/* Values of file system name, version and identifier used in volume label */
#define CFS_ID " iPSC Concurrent File System "
#define CFS_VERSION 1

/* Volume table */
typedef
struct voltab {
	short	node;
	short	drive;
} VOLTAB;
#define MAXVOL 128
#define MAXDRIVE 7

extern VOLTAB voltab[MAXVOL];

/* Value of fhdr.magic */
#define MAGIC 0x50534301

/* File header structure */
typedef
struct fhdr {
	ulong	magic;		/* File header identifier		*/
	ulong	version;	/* File system version			*/
	esize_t	data_off;	/* Offset of data			*/
	esize_t	data_end;	/* Size of allocated data area		*/
	esize_t	size;		/* File size				*/
	ushort 	mode;		/* Access permissions			*/
	short  	nlink;		/* Number of links			*/
	ushort 	uid;		/* User id				*/
	ushort 	gid;		/* Group id				*/
	time_t	atime;		/* Last access time			*/
	time_t	mtime;		/* Last modification time		*/
	time_t	ctime;		/* Creation time			*/
	ulong	reserved1;	/*  reserved for expansion		*/
	ulong	reserved2;	/*  reserved for expansion		*/
	ulong	reserved3;	/*  reserved for expansion		*/
	char	name[256];	/* Filename when created (not used)	*/
} FHDR;
#define FHDRSIZE (sizeof(FHDR))

/* Maximum number of times a file might be restructured */
#define NBRK 5

/* Subrequests for CHANGE_FILE and CHANGEDIR shared by diskproc and nameproc */
#define CHMOD			1
#define CHOWN			2
#define INCLINKS		3
#define DECLINKS		4
#define UTIME			5
#define OPEN			6
#define CLOSE			7
#define UPDATE			9

/**** 3480 changes by Ernie Granese 01/07/92 - JTI ****/

/*** see also cb.h ***/

#define RESERVE         28
#define RELEASE         29

/**** end 3480 changes ****/

/**********************************************************************/

/* Debugging */

/*
 * ASSERT can be used to check for internal errors, for debugging
 */

#define ASSERT(t, m) if (!(t)) cfs_internal_error(m)

extern int enable_debugf;

#ifdef DEBUG
#define DEBUGF(x1,x2,x3,x4,x5) idprintf(x1,x2,x3,x4,x5)
#define DBPRINT(x)	printf x
#else
#define DEBUGF(x1,x2,x3,x4,x5)
#define DBPRINT(x)
#endif

extern char *request_name[];
extern char *subrequest_name[];
extern char *state_name[];
extern char *activity_name[];
extern char *gb_name[];
extern esize_t cfs_fbrk[];
