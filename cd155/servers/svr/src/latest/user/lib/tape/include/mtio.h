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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)mtio.h	7.1 (Berkeley) 6/4/86
 */
/*
 * Structures and definitions for mag tape io control commands
 */

#ifndef	_SYS_MTIO_H_
#define _SYS_MTIO_H_

#include <sys/types.h>

/* structure for MTIOCTOP - mag tape op command */
struct	mtop	{
	short	mt_op;		/* operations defined below */
	daddr_t	mt_count;	/* how many of them */
};

/* operations */
#define MTWEOF	0	/* write an end-of-file record */
#define MTFSF	1	/* forward space file */
#define MTBSF	2	/* backward space file */
#define MTFSR	3	/* forward space record */
#define MTBSR	4	/* backward space record */
#define MTREW	5	/* rewind */
#define MTOFFL	6	/* rewind and put the drive offline */
#define MTNOP	7	/* no operation, sets status only */
#define MTCACHE	8	/* enable controller cache */
#define MTNOCACHE 9	/* disable controller cache */
#define MTLOCATE  10    /* position tape to requested block number */

#ifdef	ibmrt
#define MTERASE 18	/* for streamer: erase tape */
#define MTRETENSION 19	/* for streamer: retension tape */
#endif	ibmrt
#ifdef	sun
#define MTRETEN 8       /* retension the tape */
#define MTERASE 9       /* erase the entire tape */
#endif	sun
#ifdef	multimax
/*
 * Additional Encore-specific commands
 */
#define MTERASE	8	/* erase, valid only for streamers. */
#define MTRETEN	9	/* retension, valid only for streamers. */
#define MTEOT	10	/* space to end of data, valid only for streamers. */
#define MTFSSF	11	/* forward space sequential file */
#define MTBSSF	12	/* backward space sequential file */
#endif	multimax
#ifdef	balance
/*
 * Additional Sequent specific commands for SCSI streamer tape.
 */
#define MTERASE	8	/* Erase from current position to eot */
#define MTRET	9	/* Retention the tape (streamers) */
#define MTSEOD	10	/* Space to end of data */
#define MTNORET	11	/* don't retention the tape this time */
#endif	balance

#ifdef	mips
/* Mips specific commands */
#define MTRET	10	/* retention operation */
#define MTRST	11	/* reset operation */
#define MTERASE	12	/* secuity erase (9-track only) */
#define MTRETEN	13	/* retension the tape */
#endif	mips

/* structure for MTIOCGET - mag tape get status command */

struct	mtget	{
	short	mt_type;	/* type of magtape device */
/* the following two registers are grossly device dependent */
	short	mt_dsreg;	/* ``drive status'' register */
	short	mt_erreg;	/* ``error'' register */
/* end device-dependent registers */
	short	mt_resid;	/* residual count */
/* the following two are not yet implemented */
	daddr_t	mt_fileno;	/* file number of current position */
	daddr_t	mt_blkno;	/* block number of current position */
/* end not yet implemented */
};


typedef struct position position_t;

struct position{
        char            flags;
        char            partition;
        char            reserved_1;
        char            reserved_2;
        unsigned long   first_block;
};


/*
 * Constants for mt_type byte.  These are the same
 * for controllers compatible with the types listed.
 */
#define MT_ISTS		0x01		/* TS-11 */
#define MT_ISHT		0x02		/* TM03 Massbus: TE16, TU45, TU77 */
#define MT_ISTM		0x03		/* TM11/TE10 Unibus */
#define MT_ISMT		0x04		/* TM78/TU78 Massbus */
#define MT_ISUT		0x05		/* SI TU-45 emulation on Unibus */
#define MT_ISCPC	0x06		/* SUN */
#define MT_ISAR		0x07		/* SUN */
#define MT_ISTMSCP	0x08		/* DEC TMSCP protocol (TU81, TK50) */
#ifdef	multimax
#define MT_ISCSI        0x08            /* Multimax SCSI Tape controller */
#endif	multimax
#ifdef	ibmrt
#define MT_ISST		0x08
#endif	ibmrt
#ifdef	sun
#define MT_ISSC         0x08            /* sun: SCSI archive */
#define MT_ISXY         0x09            /* sun: Xylogics 472 */
#endif	sun
#define MT_ISSCSI       0x0a            /* SCSI tape (all brands) */
#ifdef	mips
#define MT_ISQIC	0x09		/* ISI ts11 qic-2 tape controller */
#define MT_ISSC		0x0a		/* SUN */
#define MT_ISXY		0x0b		/* SUN */
#endif	mips

typedef struct load_display load_display_t;

struct load_display{
        char            message[17];
};

/* mag tape io control commands */
#define MTIOCTOP	_IOW('m', 1, struct mtop)		/* do a mag tape op */
#define MTIOCGET	_IOR('m', 2, struct mtget)	/* get tape status */
#define MTIOCIEOT	_IO('m', 3)			/* ignore EOT error */
#define MTIOCEEOT	_IO('m', 4)			/* enable EOT error */
#define MTIOPOS         _IOR('m', 5, struct position)   /* get tape position */
#define MTIODISPLY      _IOW('m', 6, struct load_display)/* 3480 load display */


#ifndef	KERNEL
#define DEFTAPE	"/dev/io0/rmt6"
#endif	KERNEL
#endif	_SYS_MTIO_H_
