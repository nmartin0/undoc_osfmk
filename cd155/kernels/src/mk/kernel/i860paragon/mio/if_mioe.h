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
 * Copyright 1992 by Intel Corporation,
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
 * $Log: if_mioe.h,v $
 * Revision 0.4  1994/11/18  20:45:11  mtm
 * Copyright additions/changes
 *
 * Revision 0.3  1993/06/30  22:39:42  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.2  1992/11/24  17:57:14  andyp
 * Cosmetics: DEBUG to MIOE_DEBUG, STATS to MIOE_STATS.
 * Disabled MIOE_DEBUG.  Tied MIOE_STATS to MACH_KDB.
 * Bug fix from rkl for PTS bug #3568 (setting promisc. mode).
 *
 * Revision 0.1  1992/07/09  16:59:48  andyp
 * Initial checkin of Paragon files.
 *
 *	Revision 1.11  92/06/17  11:24:29  rkl
 *	Added #include <i860paragon/mio/miodev.h> line
 *	
 *	Revision 1.10  92/05/19  19:06:36  rkl
 *	Added finishing edges for integration into the kernel.
 *	
 *	Revision 1.9  92/05/18  09:41:45  rkl
 *	Added Intel copyright and fixed #includes.  Also set SPLNET to splnet
 *	
 *	Revision 1.8  92/05/11  14:57:04  rkl
 *	Made changes to conform to kernel autoconfig interface.
 *	
 *	Revision 1.7  92/05/11  09:11:13  rkl
 *	Added support for 82586 chip reset.
 *	
 *	Revision 1.6  92/03/31  08:28:33  rkl
 *	Changed volatile types.
 *	
 *	Revision 1.5  92/03/09  15:03:06  rkl
 *	Changed ELOG macro and used MIO_ETHER_RAM_SIZE instead of hard coded value.
 *	
 *	Revision 1.4  92/03/05  11:39:07  rkl
 *	Fixes after receive simulation.
 *	
 *	Revision 1.3  92/03/02  17:28:32  rkl
 *	Version after tx simulation
 *	
 *	Revision 1.2  92/02/27  17:05:54  rkl
 *	Changes after personal code review.
 *	
 *	Revision 1.1  92/02/21  12:33:41  rkl
 *	Initial revision
 *	
 */
#include <i860paragon/mio/mio.h>
#include <i860paragon/mio/miodev.h>
#include <i860paragon/mio/i82586.h>
#include <mach_kdb.h>

#define	MIOE_DEBUG	0
#define MIOE_STATS	MACH_KDB

/* Externals */

extern	int	hz;
extern	int	mode_self();
extern	u_long	ntohl();

/*
 *  Defines
 */

/* MAX and SIZE defines */

#define	MIO_ETHER_RAM_SIZE	0x10000
#define	ETHER_ADD_SIZE		6
#define	MIOE_BUF_SIZE		1500
#define	TX_QUEUE_SIZE		10
#define	RX_QUEUE_SIZE		((MIO_ETHER_RAM_SIZE -			\
		 		 (TX_QUEUE_SIZE * sizeof(tx_q_t)) +	\
					sizeof(scb_t) +			\
					sizeof(ac_t)  +			\
					sizeof(iscp_t))	 /		\
					(sizeof(fd_t) +			\
					sizeof(rx_buf_t)))

/* MIO Ethernet Driver States */

#define	MIOE_UNINITIALIZED		0
#define	MIOE_PROBED			1
#define	MIOE_ATTACHED			2
#define	MIOE_OPEN			3
#define	MIOE_HARDWARE_INIT_ERROR	-1
#define	MIOE_SOFTWARE_INIT_ERROR	-2

/* MIO modes */

#define	MIOE_PROMISC 1

/* Watchdog poll rates */

#define	MIOE_SLOW_POLL		(5 * hz)
#define	MIOE_FAST_POLL		(1 * hz)
#define	MIOE_TIMER_RETRIES	3

/* Loopback values */

#define	LOOPBACK_ON	0	/* active low signal */
#define	LOOPBACK_OFF	1	/* active low signal */

/* MIO address defines */

#define	MIO_LOOPBACK_ADDR	0xa0b80000
#define	MIO_CA_ADDR		0xa0c00000

/*
 *  Macros
 */
#define	MAX(x, y)		(x > y ? x : y)
#define	MIN(x, y)		(x < y ? x : y)
#define	CHIP_RESET(sp)		mio_ether_reset(sp->ds_if.if_unit)
#define	SET_LOOPBACK(sp, flag)	outb(MIO_LOOPBACK_ADDR, flag)
#define	CHANN_ATTN(sp)		outb(MIO_CA_ADDR, 0)
#define	NODE_ID  		node_self()

/*
 *  N.B.  These address conversion routines assume that the 82586 memory
 *        is no greater than 64K and do not cross a 64K boundary.
 */
#define	MIOE_TO_VIRT(sp, offset) \
	(offset == 0xffff ? NULL : ((caddr_t)(sp->map)) + offset)

#define	VIRT_TO_MIOE(sp, addr) \
	((u_short)(((caddr_t)(addr)) - ((caddr_t)sp->map)))

#define	SPLNET	splnet

#if	MIOE_STATS
#define	STATS(p, f)	p->stats.f
#else	MIOE_STATS
#define	STATS(p, f)
#endif	MIOE_STATS

#if	MIOE_DEBUG

#define	STATIC
#define	BIT(x)	(1 << x)

#define	ELOG_ENTER	BIT(0)
#define	ELOG_HIGH	BIT(1)
#define	ELOG_MEDIUM	BIT(2)
#define	ELOG_LOW	BIT(3)

#define	ELOG(lev, args) \
	if((elog_level & lev) && (elog_id & _this_id)) printf args

#define ELOG_ENTRY(id, args)     \
	int _this_id = BIT(id);  \
	ELOG(ELOG_ENTER, args)

#else	/* MIOE_DEBUG */

#define ELOG_ENTRY(i, a)
#define ELOG(l, a)
#define	STATIC	static

#endif	/* MIOE_DEBUG */

/*
 *  Typedefs
 */
#ifdef sun
#define	volatile
#endif

typedef	unsigned char	mio_ram_t;

/*
 *  i82586.h does not define a stand alone transmit command block
 *  so one is defined here.
 */
typedef	struct	{
	u_short	status;
	u_short	command;
	u_short	link_offset;
	u_short	tbd_offset;
	u_char	dest_addr[6];
	u_short	length;
} tx_t;

/*
 *  Monolithic transmit and receive typedefs
 */
typedef struct {
	rbd_t			rbd;
	mio_ram_t		buf[ MIOE_BUF_SIZE ];
} rx_buf_t;

typedef struct {
	tx_t			cmd;
	tbd_t			tbd;
	mio_ram_t		buf[ MIOE_BUF_SIZE ];
} tx_q_t;

/* 
 *  Volatile typedefs
 */
typedef	volatile scb_t		*v_scb_t;
typedef volatile rbd_t		*v_rbd_t;
typedef volatile tbd_t		*v_tbd_t;
typedef volatile fd_t		*v_fd_t;
typedef volatile transmit_t	*v_transmit_t;
typedef volatile tx_t		*v_tx_t;
typedef volatile ac_t		*v_ac_t;
typedef volatile iscp_t		*v_iscp_t;
typedef volatile scp_t		*v_scp_t;
typedef volatile rx_buf_t	*v_rx_buf_t;
typedef volatile tx_q_t		*v_tx_q_t;

/*
 *  GP memory based transmit queue structure
 */
typedef	struct xmit_q {
	struct xmit_q	*next;
	v_tx_t		 cmd_p;
	v_tbd_t		 tbd_p;
	mio_ram_t	*buf_p;
} xmit_q_t;

#define	MIOE_MAP_FILL	(MIO_ETHER_RAM_SIZE - \
			(sizeof(scb_t)			   + \
			 sizeof(ac_t)			   + \
			(sizeof(tx_q_t)   * TX_QUEUE_SIZE) + \
			(sizeof(fd_t)     * RX_QUEUE_SIZE) + \
			(sizeof(rx_buf_t) * RX_QUEUE_SIZE) + \
			 sizeof(iscp_t)			   + \
			 sizeof(scp_t)))

/**************************
 *  MIO Ethernet memory map
 **************************/
typedef	struct {
	scb_t		scb;
	ac_t		gen_cmd;
	tx_q_t		tx_cmd_q[ TX_QUEUE_SIZE ];
	fd_t		fd_q    [ RX_QUEUE_SIZE ];
	rx_buf_t	rx_buf_q[ RX_QUEUE_SIZE ];
	char		fill[ MIOE_MAP_FILL ];
	iscp_t		iscp;
	scp_t		scp;
} mioe_map_t;

/*
 *  Statistics and counter for driver.
 */
typedef struct{
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
} mioe_stats_t;

/*
 *  Software control structure for MIO Ethernet
 */
typedef struct { 
	struct		ifnet	ds_if;	/* generic interface header */
	u_char		ds_addr[6];	/* Ethernet hardware address */
	mio_ram_t	*base;		/* virtual base address of MIO */
	mioe_map_t	*map;		/* virtual addr to start of mioe ram */
	v_scb_t		scb_p;		/* virtual addr to mioe SCB */
        v_fd_t		begin_fd;
	v_fd_t		end_fd;
	v_rx_buf_t	end_rbd;
	xmit_q_t	*tx_qhead;
	xmit_q_t	*tx_qtail;
	u_short		mioe_base;	/* upper 16 bits of mioe base address */
	int		state;
        int		timer;
        int		node_id;
	short		mode;
#if	MIOE_STATS
	mioe_stats_t	stats;
#endif	MIOE_STATS
} mioe_softc_t;

