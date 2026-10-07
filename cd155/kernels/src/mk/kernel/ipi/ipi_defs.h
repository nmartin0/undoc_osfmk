/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1993 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: ipi_defs.h,v $
 * Revision 1.5  1995/03/02  23:36:11  arlin
 *   Added Multiple Packet per Connection support
 *   and CONTinuation support for multiple I/O
 *   per packet and connection. F/W is R1.4
 *
 *   Reviewer: Jerrie Coffman, Bernie Keany
 *   Risk: medium
 *   Benefit or PTS #: 12411
 *   Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *       Also developed special applications to test
 *       new MPC and CONT modes.
 *   Module(s):
 *     /i860paragon/hippi/
 *       hctlr.c, hctlr.h, rhippi.h, rhippi.c,
 *       hippi_status.h, hdc.c
 *     /ipi/ipi_misc.c ipi_defs.h, ipi.c,
 *     /device/ds_routines.c
 *
 * Revision 1.4  1995/02/02  19:11:42  jerrie
 * Configuration checks during the first open of an IPI device failed due
 * to an I-field conflict with another IPI device, but returned leaving the
 * driver initialized to bypass the configuration checks on subsequent opens.
 *
 * The driver was changed to correct this problem and also to change the
 * policy used to perform I-field checking.  Under the new policy, the driver
 * returns a device busy error if the conflicting slave is open, otherwise
 * it succeeds.  The master device open will always succeed, regardless of
 * any I-field conflicts, to allow the user can correct an I-field conflict.
 * An ioctl call to set the I-field to a new value will fail if the new
 * I-field conflicts with another open device, otherwise it will succeed
 * with a warning message displayed on the I/O node console.
 *
 *  Reviewer:         Arlin Davis
 *  Risk:             Low.  Only affects the IPI device driver.
 *  Benefit or PTS #: 11413
 *  Testing:          Performed the test sequence outlined in the PTS bug
 *                    report.  Ran several test cases opening and closing
 *                    IPI devices with conflicting I-fields, using the master
 *                    device, and using ioctls to set new I-field values.
 *
 *  Module(s):        The following kernel files are being modified:
 *
 *                    kernel/i860paragon/hippi:       hdc.c
 *                    kernel/ipi:                     ipi.c, ipi_defs.h,
 *                                                    ipi_master.c, ipi_misc.c
 *
 * Revision 1.3  1994/11/18  20:50:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/06/15  19:46:40  lenb
 *         dclock non longer exists outside the ipi driver.
 *
 * Revision 1.1  1994/06/08  16:54:25  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *	File:	ipi_defs.h
 * 	Author: Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	10/93
 *
 *	Controller-independent definitions for the IPI-3 driver
 */

#ifndef	_IPI_DEFS_H_
#define	_IPI_DEFS_H_

#include <kern/queue.h>
#include <kern/lock.h>
#include <i860paragon/hippi/hctlr.h>


typedef struct {
	unsigned char	ulp_id;			/* destination ULP	  */
	BITFIELD_3 (unsigned char,
			reserved1	: 6,	/* reserved, must be zero */
			b		: 1,	/* D2 burst boundary	  */
			p		: 1);	/* D1 present		  */
	BITFIELD_2(unsigned char,
			d1_size_msb	: 3,	/* D1 size, in bytes	  */
			reserved2	: 5);	/* reserved, must be zero */
#define		D1_SIZE_MSB_SHIFT		5
	BITFIELD_2(unsigned char,
			d2_offset	: 3,	/* D2 offset		  */
			d1_size_lsb	: 5);	/* D1 size, in bytes	  */
	unsigned char	d2_size_msb;		/* D2 size, in bytes	  */
	unsigned char	d2_size_b2;		/* D2 size, in bytes	  */
	unsigned char	d2_size_b1;		/* D2 size, in bytes	  */
	unsigned char	d2_size_lsb;		/* D2 size, in bytes	  */
} hippi_fp_t;

/*
 * The ULP-id of the HiPPI-FP header designates the destination ULP to
 * which the data is to be delivered.  ULP-ids are defined as follows:
 */

#define	ULP_ID_MEMORY_INTERFACE		0x02
#define	ULP_ID_MEMORY_INTERFACE_INIT	0x03
#define	ULP_ID_LINK_ENCAPSULATION	0x04
#define	ULP_ID_IPI_3_SLAVE		0x06
#define	ULP_ID_IPI_3_MASTER		0x07
#define	ULP_ID_IPI_3_PEER		0x08
				     /* 0x80 - 0x8f = locally assigned */

/*
 * IPI defaults
 */
#define	IPI_DEFAULT_I_FIELD	0
#define	IPI_DEFAULT_FACILITY	0
#define	IPI_DEFAULT_PARTITION	0
#define	IPI_DEFAULT_CMD_REF_MIN	0
#define IPI_DEFAULT_CMD_REF_MAX	15
#define IPI_DEFAULT_BLOCK_SIZE	1

/* NSL Unitree uses 0x8000 - 0xffff */
#define IPI_MAX_CMD_REF		((unsigned short)(0x7fff))

#define splipi			splnet

#define	await(event)		sleep(event, 0)
extern	wakeup();

#ifdef	btodb
#undef	btodb
#endif
#define btodb(byte_count)	((unsigned)(byte_count) / tgt->block_size)

#ifdef	dbtob
#undef	dbtob
#endif
#define dbtob(block_number)	((unsigned)(block_number) * tgt->block_size)

#define IPI_GET_CMD(t, w)	ipi_get_cmd((t), (w))
#define IPI_REL_CMD(c)		((c)->state = CMD_FREE)

#define	IPI_GO(t, c)		(*(ipi_softc[(t)->masterno])->go)((t), (c))
#define	IPI_GO_AND_WAIT(t, c)	ipi_go_and_wait((t), (c))

#define	IPI_STATISTICS
#ifdef	IPI_STATISTICS
#ifdef	PARAGON860
union part_time {
	struct {
		unsigned long lo;
		unsigned long hi;
	} longs;
	double	dbl;
};
#endif	PARAGON860
#define	IPI_DRIVER_CHECKIN(ior)		ipi_driver_checkin((ior))
#define	IPI_DRIVER_CHECKOUT(tgt, ior)	ipi_driver_checkout((tgt), (ior))
#define	IPI_DEVICE_CHECKIN(tgt, ior)	ipi_device_checkin((tgt), (ior))
#define	IPI_DEVICE_CHECKOUT(tgt, ior)	ipi_device_checkout((tgt), (ior))
#else
#define	IPI_DRIVER_CHECKIN(ior)
#define	IPI_DRIVER_CHECKOUT(tgt, ior)
#define	IPI_DEVICE_CHECKIN(tgt, ior)
#define	IPI_DEVICE_CHECKOUT(tgt, ior)
#endif

typedef unsigned int		ipi_ret_t;

/*
 * Error recovery actions
 */
#define	ACTION_SUCCESS		0x01	/* lowest priority */
#define	ACTION_ERROR		0x02
#define	ACTION_RETRY		0x04
#define	ACTION_QUEUE		0x08
#define	ACTION_BBR		0x10
#define	ACTION_ABORT		0x20
#define	ACTION_RESET		0x40	/* highest priority */

/*
 * Media access rights
 */
#define	IPI_NO_ACCESS		0x00
#define	IPI_READ_ONLY		0x01
#define	IPI_WRITE_ONLY		0x02
#define	IPI_READ_WRITE		0x03


/*
 * Device-specific information kept by driver
 */
typedef struct {
	struct disklabel	l;
} ipi_disk_info_t;

typedef struct {
	struct disklabel	l;
} ipi_optical_info_t;

typedef struct {
	boolean_t	read_only;
	unsigned int	speed;
	unsigned int	density;
	unsigned int	maxreclen;
	boolean_t	fixed_size;
} ipi_tape_info_t;

/*
 * Device-specific operations
 */
typedef struct {
	char		*(*driver_name)();	/* my driver's name	   */
	ipi_ret_t	(*optimize)();		/* tune up internal params */
	ipi_ret_t	(*open)();		/* open time ops	   */
	ipi_ret_t	(*close)();		/* close time ops	   */
	int		(*strategy)();		/* sort/start routine	   */
	int		(*restart)();		/* completion routine	   */
	io_return_t	(*get_status)();	/* specialization	   */
	io_return_t	(*set_status)();	/* specialization	   */
} ipi_devsw_t;

extern ipi_devsw_t	ipi_devsw[];

/*
 * Driver and device statistics
 */
typedef struct {
	unsigned int	requests;		/* number of I/O requests  */
	unsigned int	xfer_count;		/* number of bytes/blocks  */
	unsigned int	driver_time;		/* driver service time	   */
	unsigned int	device_time;		/* device service time	   */
} ipi_rw_stats_t;

typedef struct {
	ipi_rw_stats_t	read;			/* read statistics	   */
	ipi_rw_stats_t	write;			/* write statistics	   */
	unsigned int	queue_max;		/* maximum queue depth	   */
	unsigned int	chain_max;		/* maximum chain depth	   */
	unsigned int	queue_depth;		/* current queue depth	   */
	unsigned int	chain_depth;		/* current chain depth	   */
	unsigned int	driver_full;		/* driver queue full count */
	unsigned int	device_full;		/* device queue full count */
	unsigned int	retries;		/* driver command retries  */
} ipi_stats_t;


/*
 * Command queue descriptor
 */
typedef struct {
	hctlr_src_t		sreq;		/* source request     */
	hippi_fp_t		*hdr_ptr;	/* virtual address    */
	caddr_t			cmd_ptr;	/* virtual address    */
	volatile char		state;		/* command state      */
#define		CMD_FREE		0
#define		CMD_LOCKED		1
#define		CMD_READY		2
#define		CMD_QUEUED		3
#define		CMD_IN_PROGRESS		4
#define		CMD_DATA_IN		5
#define		CMD_DATA_OUT		6
#define		CMD_STATUS		7
#define		CMD_COMPLETE		8
	volatile char		result;		/* command outcome    */
#define		IPI_RET_SUCCESS		0x00
#define		IPI_RET_RETRY		0x01
#define		IPI_RET_ERROR		0x02
#define		IPI_RET_ABORTED		0x04
#define		IPI_RET_DEVICE_DOWN	0x08
	unsigned short		cmd_ref;	/* command reference  */
	unsigned char		dir;		/* data direction     */
#define		NO_DATA			0x00
#define		DATA_IN			0x01
#define		DATA_OUT		0x02
	unsigned int		flags;
#define		CMD_SRC_INTR_PENDING	0x00000001	/* src intr pending   */
#define		CMD_OPTIONAL		0x00000002	/* optional cmd	      */
#define		CMD_NEEDS_BBR		0x00000004	/* BBR is needed      */
#define		CMD_BBR_ACTIVE		0x00000008	/* BBR is in progress */
	io_req_t		ior;		/* what we are doing  */
/* CLASS-2 Support */
	vm_offset_t		iosge_phys;	/* saved physical address     */
	long			iosge_length;	/* saved length in bytes      */
/* CLASS-2 Support */
	struct target_info	*tgt;		/* back pointer	      */
	struct {					/* bad block recovery */
	    int			    retry_count;
	    unsigned int	    badblockno;
	    recnum_t		    save_recnum;
	    io_buf_ptr_t	    save_data;
	    io_sglist_t		    save_sgp;
	    long		    save_count;
	    long		    save_residual;
	} bbr;
} cmd_queue_t;


/*
 * Device descriptor
 */
#define MAX_IPI_TARGETS		8		/* maximum number of targets */

#define	IPI_TARGET_NAME_LEN	(16+1+8+1+4+1)	/* our way to keep it */

typedef struct target_info {
	queue_head_t	waiting_cmds;		/* to queue for slave  */
	io_req_t	ior;			/* chained io requests */

	unsigned int	flags;
#define	TGT_FULLY_PROBED	0x00000001 /* can sleep to wait		      */
#define	TGT_ONLINE		0x00000002 /* did the once-only stuff	      */
#define	TGT_ALIVE		0x00000004 /* it's alive!		      */
#define	TGT_QUEUE_FULL		0x00000008 /* slave cmd queue is full	      */
#define	TGT_BBR_ACTIVE		0x00000010 /* bad block rep in progress	      */
#define	TGT_DISCONNECTED	0x00000020 /* waiting for reconnect	      */
#define	TGT_WRITTEN_TO		0x00000040 /* tapes: need a filemark on close */
#define	TGT_REWIND_ON_CLOSE	0x00000080 /* tapes: rewind		      */
#define	TGT_REMOVABLE_MEDIA	0x00000100 /* e.g. floppy, cd-rom, ...	      */
#define	TGT_READONLY		0x00000200 /* cd-rom, scanner, ...	      */
#define TGT_WRITE_LABEL		0x00000400 /* disks: enable label overwrite   */
#define	TGT_HW_SPECIFIC_BITS	0xffff0000 /* see specific HBA		      */
	char		*hw_state;		/* opaque */
	cmd_queue_t	*cmd_q;
	ipi_devsw_t	*dev_ops;
	decl_simple_lock_data(,target_lock)
	unsigned char	masterno;
	unsigned int	max_io_count;
	char		target_id;
	char		unit_no;
	unsigned int	i_field;
	unsigned char	facility;
	unsigned char	partition;	/* IPI device partition, *not* Unix */
	unsigned short	cmd_ref_min;
	unsigned short	cmd_ref_len;
	unsigned int	block_size;
	int		open_count;
	int		vendor;
	char		tgt_name[IPI_TARGET_NAME_LEN];
	union {
	    ipi_disk_info_t	disk;
	    ipi_optical_info_t	optical;
	    ipi_tape_info_t	tape;
	} dev_info;
	ipi_stats_t	statistics;
} target_info_t;


/*
 * HBA descriptor
 */
typedef struct {
	/* master (us) state */
	unsigned char	masterno;
	unsigned int	max_io_count;
#define	IPI_UNLIMITED_IO_COUNT	0xffffffff
	int		hba_timeout;		/* timeout in seconds */
	boolean_t	supports_sgio;
	char		*hw_state;		/* opaque */
	int		(*go)();
	int		(*watchdog)();
	boolean_t	(*probe)();
	io_return_t	(*filter)();
	/* per-target state */
	target_info_t	*target[MAX_IPI_TARGETS];
} ipi_softc_t;

/*
 * HBA-independent Watchdog
 */
typedef struct {
	char		nactive;
	char		state;
#define		IPI_WD_INACTIVE		0
#define		IPI_WD_ACTIVE		1
#define		IPI_WD_EXPIRED		2
} wd_t;

typedef struct {
	unsigned int	reset_count;
	wd_t		dog[MAX_IPI_TARGETS];
	int		(*reset)();
} watchdog_t;

extern int		ipi_watchdog();
extern ipi_softc_t	*ipi_master_alloc();
extern target_info_t	*ipi_slave_alloc();
extern ipi_ret_t	ipi_config_error();
extern ipi_ret_t	ipi_config_check();
extern cmd_queue_t	*ipi_get_cmd();
extern ipi_ret_t	ipi_probe();

extern ipi_softc_t	*ipi_softc[];
extern boolean_t	ipi_automatic_bbr;
extern int		ipi_retries;
extern int		ipi_bbr_retries;
extern int		ipi_open_timeout;
extern int		ipi_watchdog_period;
extern int		ipi_delay_after_reset;
extern int		ipi_debug;

#endif	_IPI_DEFS_H_
