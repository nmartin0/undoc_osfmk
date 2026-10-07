/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
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
 * $Log: if_sdbe.h,v $
 * Revision 1.2.8.1  1995/06/11  18:32:40  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1995/03/14  23:43:31  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	if_sdbe.h
 *	Author:	Jim Chorn (modified by Jerrie Coffman)
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	11/94
 *
 *	Defines for the Intel 82596-CA Ethernet Controller Device Driver
 */


#ifndef	_IF_SDBE_H_
#define	_IF_SDBE_H_

/* Externals */

extern	int	hz;
extern	int	node_self();
extern	u_long	ntohl();

/*
 *  Defines and configurations
 */

#define TX_DUAL_RX_NODE

/*
 * Use all of local ram for control/tx/rx
 */
#ifdef ALL_IN_LRAM	
#define SDBE_TX_BUF_SIZE	( 2048 - sizeof(ac_t) - sizeof(tbd_t) )
#define RBDS_PER_BUFFER		1
#define	SDBE_BUF_SIZE		( 1536 / RBDS_PER_BUFFER ) /* Must be even */
#define RFD_QUEUE_SIZE		100
#define RX_DATA_AREA_SIZE	( 1536 * RFD_QUEUE_SIZE )
#define RBD_QUEUE_SIZE		( RBDS_PER_BUFFER * RFD_QUEUE_SIZE )
#define	TX_QUEUE_SIZE		10
#endif

/*
 * Use local ram for control/tx, node mem for rx
 */
#ifdef TX_LRAM_RX_NODE	
#define SDBE_TX_BUF_SIZE	( 2048 - sizeof(ac_t) - sizeof(tbd_t) )
#define RBDS_PER_BUFFER		1
#define	SDBE_BUF_SIZE		( 1536 / RBDS_PER_BUFFER ) /* Must be even */
#define RFD_QUEUE_SIZE		40
#define RX_DATA_AREA_SIZE	( 1 )
#define RBD_QUEUE_SIZE		( RBDS_PER_BUFFER * RFD_QUEUE_SIZE )
#define	TX_QUEUE_SIZE		10
#endif

/*
 * dualport control/tx, node mem for rx
 *
 * Note: Although there is 32K of dual port ram, the top 6K
 * is reserved by h/w and the bottom 16K for SCSI controllers.
 */
#ifdef TX_DUAL_RX_NODE	
#define SDBE_TX_BUF_SIZE	( 2048 - sizeof(ac_t) - sizeof(tbd_t) )
#define RBDS_PER_BUFFER		1
#define	SDBE_BUF_SIZE		( 1536 / RBDS_PER_BUFFER ) /* Must be even */
#define RFD_QUEUE_SIZE		40
#define RX_DATA_AREA_SIZE	( 1 )
#define RBD_QUEUE_SIZE		( RBDS_PER_BUFFER * RFD_QUEUE_SIZE )
#define	TX_QUEUE_SIZE		4
#endif

/* Address Conversion Macros */

#define	VIRT_TO_SDBE(virt)			\
	( kvtophys((vm_offset_t)(virt)) )

#define	SDBE_TO_VIRT(phys)			\
	( (u_char *)(sp->map) +			\
	 ((u_char *)(phys) - (u_char *)(sp->map_phys)) )

#define	SDBE_RX_DATA_TO_VIRT(phys)		\
	( (u_char *)(sp->rx_data) +		\
	 ((u_char *)(phys) - (u_char *)(sp->rx_data_phys)) )

/* Ethernet Driver States */

#define	SDBE_UNINITIALIZED		0
#define	SDBE_PROBED			1
#define	SDBE_ATTACHED			2
#define	SDBE_OPEN			3
#define	SDBE_HARDWARE_INIT_ERROR	-1
#define	SDBE_SOFTWARE_INIT_ERROR	-2

/* Ethernet Command Unit States */
#define	SDBE_READY		0
#define	SDBE_TXBUSY		1
#define	SDBE_ERROR		2

/* modes */

#define	SDBE_PROMISC 1

/* Watchdog poll rates */

#define	SDBE_SLOW_POLL		(5 * hz)
#define	SDBE_FAST_POLL		(1 * hz)
#define	SDBE_TIMER_RETRIES	3

/* Loopback values */

#define	LOOPBACK_ON		1
#define	LOOPBACK_OFF		0

/* Register Addresses */

#define S16_CONTROL_REG		SDB_CONTROL
#define S16_INTMASK_REG		SDB_IMASK
#define S16_INTSTAT_REG		SDB_ISTAT
#define S16_LAN_CA		SDB_ETHER_START
#define S16_LAN_PORT		(SDB_ETHER_START + 0x10)

/* Register Bit Definitions */

#define LANRST			SDB_CONTROL_ETHER_RESET
#define LANMSK			SDB_IMASK_ETHER
#define LANPAR			SDB_IMASK_ETHER_PARITY
#define LANINT			SDB_ISTAT_ETHER

/*
 *  Macros
 *	Note: SCP Address must be aligned on a 16 byte boundary
 */
#define	MAX(x, y)		(x > y ? x : y)
#define	MIN(x, y)		(x < y ? x : y)
#define	DELAY_10MS		10000		/* 10 MilliSecond delay count */
#define	DELAY_30MS		30000		/* 30 MilliSecond delay count */

#define SET_CONTROL_REG(val)	\
			{ outw(S16_CONTROL_REG, inw(S16_CONTROL_REG) |  val); }
#define CLEAR_CONTROL_REG(val)	\
			{ outw(S16_CONTROL_REG, inw(S16_CONTROL_REG) & ~val); }
#define	CHIP_RESET(sp)		\
			{ outl(S16_LAN_PORT, LAN_RESET); delay(DELAY_30MS); }
#define	LOAD_SCPADDR(scp)	\
			{ outl(S16_LAN_PORT, LAN_SCP((u_int)(scp) & ~0xF)); }
#define	CHANN_ATTN(sp)		\
			{ outl(S16_LAN_CA, 0); }

#define	SET_LOOPBACK(sp, enable) if (enable)	\
					(sp)->mode |=  SDBE_LOOPBACK; \
				else \
					(sp)->mode &= ~SDBE_LOOPBACK;
#define USEINTRS
#ifndef USEINTRS
#define LAN_INTR_ENABLE(enable)
#define PAR_INTR_ENABLE(enable)
#else
#define LAN_INTR_ENABLE(enable) 					    \
		if (enable)						    \
		    outw(S16_INTMASK_REG, inw(S16_INTMASK_REG) & ~LANMSK);  \
		else							    \
		    outw(S16_INTMASK_REG, inw(S16_INTMASK_REG) |  LANMSK);
#define PAR_INTR_ENABLE(enable) 					    \
		if (enable)						    \
		    outw(S16_INTMASK_REG, inw(S16_INTMASK_REG) & ~LANPAR);  \
		else							    \
		    outw(S16_INTMASK_REG, inw(S16_INTMASK_REG) |  LANPAR);
#endif

#define	NODE_ID  		node_self()
#define	SPLNET			splnet

#ifdef SDBE_STATS
#define	STATS(p, f)		p->stats.f
#else
#define	STATS(p, f)
#endif

#ifdef	DEBUG

#define	STATIC

#define	ELOG_ENTER	BIT(0)
#define	ELOG_HIGH	BIT(1)
#define	ELOG_MEDIUM	BIT(2)
#define	ELOG_LOW	BIT(3)

#define	ELOG(lev, args) \
	if ((sdbe_log_level & lev) && (sdbe_log_id & _this_id)) printf args

#define ELOG_ENTRY(id, args)     \
	int _this_id = BIT(id);  \
	ELOG(ELOG_ENTER, args)

#else

#define ELOG_ENTRY(i, a)
#define ELOG(l, a)
#define	STATIC		static

#endif /* DEBUG */

/*
 *  Typedefs
 */

typedef	unsigned char	ram_t;

/*
 *  Monolithic transmit
 */
typedef struct {
	ac_t		cmd;	/* Command structure */
	tbd_t		tbd;	/* First transmit buffer */
	ram_t		buf[ SDBE_TX_BUF_SIZE ];
} tx_q_t;

/* 
 *  Volatile typedefs
 */
typedef volatile scp_t		*v_scp_t;
typedef volatile iscp_t		*v_iscp_t;
typedef	volatile scb_t		*v_scb_t;

typedef volatile ac_t		*v_ac_t;
typedef volatile tbd_t		*v_tbd_t;
typedef volatile tx_q_t		*v_tx_q_t;

typedef volatile rfd_t		*v_fd_t;
typedef volatile rbd_t		*v_rbd_t;

/****************************
 * SDBE Ethernet memory map *
 ****************************/
typedef	volatile struct {
	scp_t		scp;
	iscp_t		iscp;
	scb_t		scb;
	ac_t		gen_cmd;		/* <- Don't use AC_TRANSMIT */
	tx_q_t		tx_q[ TX_QUEUE_SIZE ];		/* Circular list */
	rfd_t		fd_q[ RFD_QUEUE_SIZE ];		/* Circular list */
	rbd_t		rbd_q[ RBD_QUEUE_SIZE ];	/* Circular list */
	ram_t		rx_data[ RX_DATA_AREA_SIZE ];
} sdbe_map_t;

#define	SDBE_ETHER_RAM_SIZE	sizeof(sdbe_map_t)

/*
 *  Statistics and counters for driver.
 */
typedef struct {
	struct {
		u_int packets;		/*  0 */
		u_int bytes;		/*  1 */
		u_int interrupts;	/*  2 */
		u_int defer;		/*  3 */
		u_int fatal_collis;	/*  4 */
		u_int busy;		/*  5 */
		u_int q_full;		/*  6 */
		u_int q_null;		/*  7 */
		u_int q_hit;		/*  8 */
		u_int q_miss;		/*  9 */
		u_int errors;		/* 10 */
		u_int underruns;	/* 11 */
	} tx; 
	struct {
		u_int packets;		/* 12 */
		u_int bytes;		/* 13 */
		u_int interrupts;	/* 14 */
		u_int crcerrs;		/* 15 */
		u_int alnerrs;		/* 16 */
		u_int rscerrs;		/* 17 */
		u_int ovrnerrs;		/* 18 */
		u_int restarts;		/* 19 */
		u_int q_empty;		/* 20 */
		u_int no_buffs;		/* 21 */
		u_int longs;		/* 22 */
		u_int null_rbd;		/* 23 */
	} rx;
	u_int timeouts;			/* 24 */
	u_int spurious_int;		/* 25 */
	u_int resets;			/* 26 */
	u_int wait_calls;		/* 27 */
	u_int wait_spins;		/* 28 */
	u_int wait_timeouts;		/* 29 */
} sdbe_stats_t;

/*
 *  Software control structure for SDBE Ethernet
 */
typedef struct { 
	struct ifnet	ds_if;		/* generic interface header */
        int		node_id;	/* Unit id */
	short		mode;		/* Modes from above */
	int		state;		/* Driver state */
        int		custate;	/* Command Unit state */
	u_char		ds_addr[6];	/* Ethernet hardware address */
	sdbe_map_t	*map;		/* Virtual addr to start of sdbe ram */
	sdbe_map_t	*map_phys;	/* Physical addr to start of sdbe ram */
	v_scb_t		scb_p;		/* Quick reference to SCB */
        v_fd_t		fd_q;		/* Currently recieving frame */
	v_fd_t		fd_q_tail;	/* End of queue */
	v_fd_t		fd_unk;		/* Last unknown status'd rfd */
	v_rbd_t		rx_unk;		/* Last unknown status'd rbd */
	v_rbd_t		rx_rbds;	/* Start of allocated rbd space */
	v_rbd_t		rx_rbdtail;	/* End of queue */
	ram_t		*rx_data;	/* Virtual Start of Receive buffer */
	ram_t		*rx_data_phys;	/* Physical Start of Receive buffer */
	v_tx_q_t	tx_qhead;	/* Currently transmitting frame */
	v_tx_q_t	tx_qtail;	/* End of queue */
#ifdef SDBE_STATS
	sdbe_stats_t	stats;
#endif
} sdbe_softc_t;

#endif	_IF_SDBE_H_
