/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
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
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * 28-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Increases hba_dep size, needed for himem on i386at H/W.
 *
 * $Log: scsi_defs.h,v $
 * Revision 1.12  1995/03/14  23:48:59  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 1.11  1994/11/18  21:00:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/08/31  21:25:58  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.8.8.1  1994/08/04  16:08:45  richardg
 *  Reviewer: Jerry Coffman
 *  Risk: Medium
 *  Benefit or PTS #: 10171
 *  Testing: ran Tensor's tapewrt program simultaneously against both LUNs.
 *  Module(s): target_info_t
 *
 * Revision 1.8  1994/02/11  23:58:53  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: add support for enabling/disabling buffering on
 * 		tape drives.
 *  Testing: Tape EAT's
 *  Module(s):  scsi_tape_info_t structure.
 *
 * Revision 1.7  1993/09/24  17:33:19  jerrie
 * Added TGT_COMPRESSION flag to support tape data compression.
 *
 * Revision 1.6  1993/09/16  18:12:20  richardg
 * modified the memory allocation to evenly divide available MIO memory between existing scsi devices
 *
 * Revision 1.5  1993/06/30  22:54:04  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/04/27  20:48:09  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:53:39  dleslie
 * First R1_0 release
 *
 * Revision 2.9.2.1  92/03/28  10:16:02  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:33:03  jeffreyh]
 * 
 * Revision 2.10  92/02/23  22:44:55  elf
 * 	Changed unused field into masterno in target descriptor.
 * 	[92/02/22  19:31:54  af]
 * 
 * Revision 2.9  91/08/24  12:28:38  af
 * 	Added processor_type infos, definition of an opaque type,
 * 	multiP locking.
 * 	[91/08/02  03:55:05  af]
 * 
 * Revision 2.8  91/07/09  23:22:53  danner
 * 	Added include of <scsi/rz_labels.h>
 * 	[91/07/09  11:16:30  danner]
 * 
 * Revision 2.7  91/06/19  11:57:43  rvb
 * 	File moved here from mips/PMAX since it is now "MI" code, also
 * 	used by Vax3100 and soon -- the omron luna88k.
 * 	[91/06/04            rvb]
 * 
 * Revision 2.6  91/05/14  17:30:18  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/05/13  06:05:34  af
 * 	Made counters unsigned, added copy_count for use by HBAs
 * 	that do unlimited DMA via double buffering.  Made explicit
 * 	two padding bytes, and let them be HBA-specific (e.g. used
 * 	for odd-byte-boundary conditions on some).
 * 	Made max_dma_data unsigned, a value of -1 means unlimited.
 * 	Removed unsed residue field.
 * 
 * 	Defined tape-specific information fields to target structure.
 * 	Added tape-specific flags and flag for targets that require
 * 	the long form of various scsi commands.
 * 	Added disconnected-state information to target structure.
 * 	Added watchdog field to adapter structure.
 * 	[91/05/12  16:24:10  af]
 * 
 * Revision 2.4.1.2  91/04/05  13:13:29  af
 * 	Made counters unsigned, added copy_count for use by HBAs
 * 	that do unlimited DMA via double buffering.  Made explicit
 * 	two padding bytes, and let them be HBA-specific (e.g. used
 * 	for odd-byte-boundary conditions on some).
 * 	Made max_dma_data unsigned, a value of -1 means unlimited.
 * 	Removed unsed residue field.
 * 
 * Revision 2.4.1.1  91/03/29  17:06:09  af
 * 	Defined tape-specific information fields to target structure.
 * 	Added tape-specific flags and flag for targets that require
 * 	the long form of various scsi commands.
 * 	Added disconnected-state information to target structure.
 * 	Added watchdog field to adapter structure.
 * 
 * Revision 2.4  91/02/05  17:45:43  mrt
 * 	Added author notices
 * 	[91/02/04  11:19:29  mrt]
 * 
 * 	Changed to use new Mach copyright
 * 	[91/02/02  12:18:11  mrt]
 * 
 * Revision 2.3  90/12/05  23:35:12  af
 * 	Cleanups, use BSD labels internally.
 * 	[90/12/03  23:47:29  af]
 * 
 * Revision 2.1.1.1  90/11/01  03:39:55  af
 * 	Created.
 * 	[90/09/03            af]
 */
/*
 *	File: scsi_defs.h
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	Date:	9/90
 *
 *	Controller-independent definitions for the SCSI driver
 */

#ifndef	_SCSI_SCSI_DEFS_H_
#define	_SCSI_SCSI_DEFS_H_

#include <kern/queue.h>
#include <kern/lock.h>

#define	await(event)	sleep(event,0)
extern	wakeup();

#ifdef	btodb
#undef	btodb
#endif
#define btodb(byte_offset)	/* calculates (byte_offset / dev_bsize) */ \
	((unsigned)(byte_offset) / tgt->block_size)

#ifdef	dbtob
#undef	dbtob
#endif
#define dbtob(block_number)	/* calculates (block_number * dev_bsize) */ \
	((unsigned)(block_number) * tgt->block_size)

typedef	vm_offset_t	opaque_t;	/* should be elsewhere */

/*
 * Internal error codes, and return values
 * XXX use the mach/error.h scheme XXX
 */
typedef unsigned int		scsi_ret_t;

#define	SCSI_ERR_GRAVITY(x)	((unsigned)(x)&0xf0000000)
#define	SCSI_ERR_GRAVE		0x80000000
#define SCSI_ERR_BAD		0x40000000

#define	SCSI_ERR_CLASS(x)	((unsigned)(x)&0x0fffffff)
#define	SCSI_ERR_STATUS		0x00000001
#define	SCSI_ERR_SENSE		0x00000002
#define SCSI_ERR_MSEL		0x00000004
#define SCSI_ERR_MSEN		0x00000008
#define SCSI_ERR_NO_LUN		0x00000025

extern	void	scsi_error(/* target_info_t *, unsigned, unsigned */);

#define	SCSI_RET_IN_PROGRESS	0x00
#define	SCSI_RET_SUCCESS	0x01
#define	SCSI_RET_RETRY		0x02
#define SCSI_RET_NEED_SENSE	0x04
#define SCSI_RET_ABORTED	0x08
#define	SCSI_RET_DEVICE_DOWN	0x10

#define SCSI_STATISTICS
#ifdef	SCSI_STATISTICS
#ifdef	PARAGON860
union part_time {
	struct {
		unsigned long	lo;
		unsigned long	hi;
	} longs;
	double	dbl;
};
#endif	PARAGON860
#define SCSI_DRIVER_CHECKIN(ior)	scsi_driver_checkin((ior))
#define SCSI_DRIVER_CHECKOUT(tgt, ior)	scsi_driver_checkout((tgt), (ior))
#define SCSI_DEVICE_CHECKIN(tgt, ior)	scsi_device_checkin((tgt), (ior))
#define SCSI_DEVICE_CHECKOUT(tgt, ior)	scsi_device_checkout((tgt), (ior))
#else
#define SCSI_DRIVER_CHECKIN(ior)	
#define SCSI_DRIVER_CHECKOUT(tgt, ior)	
#define SCSI_DEVICE_CHECKIN(tgt, ior)	
#define SCSI_DEVICE_CHECKOUT(tgt, ior)	
#endif

/*
 * Device-specific information kept by driver
 */
typedef struct {
	struct disklabel	l;
	struct {
	    unsigned int	badblockno;
	    unsigned int	save_rec;
	    char		*save_addr;
	    int			save_count;
	    int			save_resid;
	    int			retry_count;
	} b;
} scsi_disk_info_t;

typedef struct {
	boolean_t	read_only;
	unsigned int	buffer_mode; /* added for 3480 support */
	unsigned int	speed;
	unsigned int	density;
	unsigned int	maxreclen;
	boolean_t	fixed_size;
} scsi_tape_info_t;

typedef struct {
	char		req_pending;
	char		req_id;
	char		req_lun;
	char		req_cmd;
	unsigned int	req_len;
	/* more later */
} scsi_processor_info_t;

/*
 * Device-specific operations
 */
typedef struct {
	char		*(*driver_name)();	/* my driver's name */
	scsi_ret_t	(*optimize)();		/* tune up internal params */
	scsi_ret_t	(*open)();		/* open time ops */
	scsi_ret_t	(*close)();		/* close time ops */
	int		(*strategy)();		/* sort/start routine */
	int		(*restart)();		/* completion routine */
	io_return_t	(*get_status)();	/* specialization */
	io_return_t	(*set_status)();	/* specialization */
} scsi_devsw_t;

extern scsi_devsw_t	scsi_devsw[];

/*
 * Driver and device statistics
 */
typedef struct {
	unsigned int		requests;	/* number of I/O requests  */
	unsigned int		xfer_count;	/* number of bytes/blocks  */
	unsigned int		driver_time;	/* driver service time	   */
	unsigned int		device_time;	/* device service time	   */
} scsi_rw_stats_t;

typedef struct {
	scsi_rw_stats_t		read;		/* read statistics	   */
	scsi_rw_stats_t		write;		/* write statistics	   */
	unsigned int		queue_max;	/* maximum queue depth	   */
	unsigned int		chain_max;	/* maximum chain depth	   */
	unsigned int		queue_depth;	/* current queue depth	   */
	unsigned int		chain_depth;	/* current chain depth	   */
	unsigned int		driver_full;	/* driver queue full count */
	unsigned int		device_full;	/* device queue full count */
	unsigned int		retries;	/* driver command retries  */
} scsi_stats_t;

/*
 * State info kept while waiting to seize bus, either
 * for first selection or while in disconnected state
 */
typedef struct {
	struct script	*script;
	int		(*handler)();
	unsigned int	out_count;
	unsigned int	in_count;
	unsigned int	copy_count;	/* optional */
	unsigned int	dma_offset;
	unsigned char	identify;
	unsigned char	cmd_count;
	unsigned char	hba_dep[4];	/* [bernadat@gr.osf.org] */
} transient_t;

/*
 * Device tagged command queuing descriptor
 */
typedef struct tag_info {
	queue_chain_t		links;		/* to queue for tgt	      */
	unsigned char		tag;		/* queue tag id		      */
	int			state;		/* queue tag state	      */
#define	TAG_FREE		0x00000000	/* tag is available	      */
#define	TAG_BUSY		0x00000001	/* tag is in use	      */
	io_req_t		ior;		/* what we are doing	      */
	unsigned int		flags;		/* flags saved for this ior   */
        unsigned char           *cmd_ptr;       /* pointer to command buffer  */
	int			cmd_count;	/* num bytes in cmd_ptr[]     */
        boolean_t               cmd_only;       /* send no messages           */
        unsigned char		cur_cmd;        /* first byte of cmd          */
	char			lun;		/* logical unit number	      */
	struct target_info	*tgt;		/* back pointer to tgt struct */
} tag_info_t;

/*
 * Device descriptor
 */

#define	SCSI_TARGET_NAME_LEN	8+16+4+8	/* our way to keep it */

#define MAX_SCSI_TARGETS	16
#define MAX_LUNS		8
#define MAX_QUEUE_TAGS		256

typedef struct target_info {
	queue_chain_t	links;			/* to queue for bus */
	io_req_t	ior;			/* what we are doing */

	unsigned int	flags;
#define	TGT_DID_SYNCH		0x00000001	/* finished the synch neg */
#define	TGT_TRY_SYNCH		0x00000002	/* do the synch negotiation */
#define	TGT_FULLY_PROBED	0x00000004	/* can sleep to wait */
#define	TGT_ONLINE		0x00000008	/* did the once-only stuff */
#define	TGT_ALIVE		0x00000010
#define	TGT_BBR_ACTIVE		0x00000020	/* bad block replc in progress */
#define	TGT_DISCONNECTED	0x00000040	/* waiting for reconnect */
#define	TGT_WRITTEN_TO		0x00000080	/* tapes: needs a filemark on close */
#define	TGT_REWIND_ON_CLOSE	0x00000100	/* tapes: rewind */
#define	TGT_BIG			0x00000200	/* disks: > 1Gb, use long R/W */
#define	TGT_REMOVABLE_MEDIA	0x00000400	/* e.g. floppy, cd-rom,.. */
#define	TGT_READONLY		0x00000800	/* cd-rom, scanner, .. */
#define	TGT_OPTIONAL_CMD	0x00001000	/* optional cmd, ignore errors */
#define TGT_WRITE_LABEL		0x00002000	/* disks: enable overwriting of label */
#define	TGT_US			0x00004000	/* our desc, when target role */
#ifdef	PARAGON860	/* Tape Compression Support */
#define TGT_COMPRESSION		0x00008000	/* tapes: data compression */
#endif	PARAGON860	/* Tape Compression Support */
#define TGT_TRY_WIDE		0x00010000	/* do the wide negotiation */
#define TGT_DID_WIDE		0x00020000	/* finished the wide negotiation */
#define TGT_TAGGED_QUEUING	0x00040000	/* device does tagged queuing */
#define TGT_QUEUE_FULL		0x00080000	/* device cmd queue is full */

#define	TGT_HW_SPECIFIC_BITS	0xfff00000	/* see specific HBA */
	char		*hw_state;		/* opaque */
	char		*dma_ptr;
	unsigned char	*cmd_ptr;
	scsi_devsw_t	*dev_ops;
	char		target_id;
	char		unit_no;
	unsigned char	sync_period;
	unsigned char	sync_offset;
	decl_simple_lock_data(,target_lock)
#ifdef	MACH_KERNEL
#else	/*MACH_KERNEL*/
	struct fdma	fdma;
#endif	/*MACH_KERNEL*/
	transient_t	transient_state;	/* state information */
	unsigned int	block_size;
	volatile char	done;
	unsigned char	cur_cmd;
	char		true_lun; /* the real LUN */
	char		lun;  /* the temp LUN, could be changed by passthru */
	char		masterno;
	char		tgt_name[SCSI_TARGET_NAME_LEN];
	union {
	    scsi_disk_info_t	disk;
	    scsi_tape_info_t	tape;
	    scsi_processor_info_t	cpu;
	} dev_info;
#ifdef PARAGON860	/* performance mod */
        unsigned int    max_dma_data;
#endif
	scsi_stats_t	statistics;

	tag_info_t	*tag_q;			/* queue tag data structs    */
	vm_size_t	tag_q_size;		/* sizeof each queue tag     */
	int		tag_q_depth;		/* number of queue tags      */
	queue_head_t	free_tag_q;		/* available queue tags      */
	queue_head_t	active_tag_q;		/* tags queued up            */
} target_info_t;

/*
 * HBA descriptor
 */

typedef struct {
	/* initiator (us) state */
	unsigned char	initiator_id;
	unsigned char	masterno;
	unsigned int	max_dma_data;
#define SCSI_UNLIMITED_IO_COUNT	-1
	boolean_t	supports_sgio;
	char		*hw_state;		/* opaque */
	int		(*tag_q_init)();
	int		(*go)();
	int		(*watchdog)();
	boolean_t	(*probe)();
	/* per-target state */
	target_info_t	*target[MAX_SCSI_TARGETS][MAX_LUNS];
} scsi_softc_t;

extern scsi_softc_t	*scsi_softc[];
extern scsi_softc_t	*scsi_master_alloc(/* int unit */);
extern target_info_t	*scsi_slave_alloc(/* int unit, int slave, char *hw */);

#define	BGET(d,mid,id)	(d[mid] & (1 << id))		/* bitmap ops */
#define BSET(d,mid,id)	d[mid] |= (1 << id)
#define BCLR(d,mid,id)	d[mid] &= ~(1 << id)

extern unsigned short	scsi_no_synchronous_xfer[];    /* one bitmap per ctlr */
extern unsigned short	scsi_no_wide_xfer[];	       /* one bitmap per ctlr */
extern unsigned short	scsi_use_long_form[];	       /* one bitmap per ctlr */
extern unsigned short	scsi_might_disconnect[];       /* one bitmap per ctlr */
extern unsigned short	scsi_should_disconnect[];      /* one bitmap per ctlr */
extern unsigned short	scsi_should_queue[];	       /* one bitmap per ctlr */
extern unsigned char	scsi_initiator_id[];	       /* one id per ctlr     */

extern boolean_t	scsi_exabyte_filemarks;
extern boolean_t	scsi_no_automatic_bbr;
extern int		scsi_bbr_retries;
extern int		scsi_watchdog_period;
extern int		scsi_delay_after_reset;
extern unsigned int	scsi_per_target_virtual;	/* 2.5 only */

extern int		scsi_debug;

/*
 * HBA-independent Watchdog
 */
typedef struct {

	unsigned short	reset_count;
	char		nactive;

	char		watchdog_state;

#define SCSI_WD_INACTIVE	0
#define	SCSI_WD_ACTIVE		1
#define SCSI_WD_EXPIRED		2

	int		(*reset)();

} watchdog_t;

extern int	scsi_watchdog(/* watchdog_t* */);

#endif	_SCSI_SCSI_DEFS_H_
