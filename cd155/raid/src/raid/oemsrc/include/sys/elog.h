/*
 *
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */ 
/*	Copyright 1988 NCR Corporation - Dayton, Ohio, USA	*/

#ident  "@(#)kern-port:sys/elog.h	1.3"

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * "True" major device numbers. These correspond
 * to standard positions in the configuration
 * table, but are used for error logging
 * purposes only.
 */

#ifdef tower32_200
#define CNTL	1
#define SYS	2
#define CAC	3
#define PF	4
#endif

#ifdef tower32_600
#define NEC	0	/* PMC Serial IO ports		*/
#define MER	1	/* Memory related errors	*/
#define DK0	2	/* DTC86 w/ST412 disks		*/
#define IOS	3	/* Intellegent Octal Serial Card*/
#define MPC	5	/* Multiprotocol adapter	*/
#define XL0	6	/* Xylogics controller		*/
#define WD0	7	/* Western Digital Controller	*/
#define TP0	8	/* Screaming tape		*/
#define CP0	9	/* 9 Track tape			*/
#define HPM	10	/* HPSIO controller		*/
#define HP0	11	/* HPSIO port			*/
#define SD0     22      /* SCSI low-id, diff disk       */
#define SD1     34      /* SCSI high-id, diff disk      */
#define SD2     64      /* SCSI low-id, OC disk         */
#define SD3     67      /* SCSI high-id, diff disk      */
#define SS0     23      /* SCSI diff mag tape           */
#define SS1     65      /* SCSI OC mag tape             */
#define SQ0     35      /* SCSI diff cartridge tape     */
#define SQ1     68      /* SCSI OC cartridge tape       */
#define ERR     25      /* Firmware log record          */
#define SP0     42      /* SCSI printer                 */
#endif

/*
 * IO statistics are kept for each physical unit of each
 * block device (within the driver). Primary purpose is
 * to establish a guesstimate of error rates during
 * error logging.
 */

struct iostat {
	long	io_ops;		/* number of read/writes */
	long	io_misc;	/* number of "other" operations */
	long	io_qcnt;	/* number of jobs assigned to drive */
	ushort io_unlog;	/* number of unlogged errors */
};

/*
 * structure for system accounting
 */
struct iotime {
	struct iostat ios;
	long	io_bcnt;	/* total blocks transferred */
	time_t	io_resp;	/* total block response time */
	time_t	io_act;		/* total drive active time (cumulative utilization) */
	int	io_pad;		/* round size to 2^n */
};
#define	io_cnt	ios.io_ops
#define io_qc ios.io_qcnt
/* drive utilization times can be calculated by system software as follows */

/* Average drive utilization = (io_cact/io_elapt) */
/* Average drive utilization for last interval = (io_liact/io_intv) */
