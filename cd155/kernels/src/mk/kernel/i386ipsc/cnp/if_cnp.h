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
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: if_cnp.h,v $
 * Revision 1.5  1994/11/18  20:38:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:29:32  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:27:54  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:28:29  dleslie
 * First R1_0 release
 *
 * Revision 2.2  91/06/18  20:50:07  jsb
 * 	Moved here from i386ipsc for licensing reasons.
 * 	[91/06/18  19:45:04  jsb]
 * 
 * Revision 2.2  90/12/04  14:47:18  jsb
 * 	First checkin.
 * 	[90/12/04  10:56:44  jsb]
 * 
 */
/*
 *	File:	if_cnp.c
 *	Author:	Joseph S. Barrera III
 *
 *	Copyright (c) 1990 Joseph S. Barrera III
 *	Ethernet driver definitions for CMC ENP-100.
 */
/*
 *	Copyright (c) 1984 by Communication Machinery Corporation
 *
 *	This file contains material which is proprietary to
 *	Communication Machinery Corporation (CMC) and which
 *	may not be divulged without the written permission
 *	of CMC.
 *
 *	ENP-10 Ram Definition
 *
 *	3/15/85 Jon Phares
 *	Update 7/10/85 S. Holmgren
 *	ENP-10 update 7/21/85 J. Mullen
 *	ENP-20 update 8/11/85 J. Mullen
 *	ENP-100 (CNP) update 1/5/88 J. Mullen
 * 
 */

#define K		*1024

#define CNPSIZE		(512 K)		/* VME bus space allocated to cnp */
#define MINPKTSIZE	60		/* minimum ethernet packet size */

#define RAM_WINDOW	(508 K)		/* `real' RAM */
#define IOACCESS_WINDOW (4 K)		/* control window */

#define	BYTE_MSF	0
#if	BYTE_MSF
#define __ntohl(x)	(x)
#define __ntohs(x)	(x)
#define __htonl(x)	(x)
#define __htons(x)	(x)
#else	BYTE_MSF
#define __ntohl(x)	((0xff & (x)>>24) | (0xff00 & (x)>>8) | \
			(0xff0000 & (x)<<8) | (0xff000000 & (x)<<24))
#define __ntohs(x)	((0xff & (x)>>8) | (0xff00 & (x)<<8))
#define __htonl(x)	ntohl(x)
#define __htons(x)	ntohs(x)
#endif	BYTE_MSF

/*
 * Automatic casting to unsigned scalars allows us to not worry
 * about explicitly casting pointers, but raises the dangers
 * associated with mixing signed and unsigned (eg comparisons against 0).
 */
#define ntohl(x)	__ntohl((u_long)(x))
#define ntohs(x)	__ntohs((u_short)(x))
#define htonl(x)	__htonl((u_long)(x))
#define htons(x)	__htons((u_short)(x))

/* Top 4K space for CNP is for control functions */

struct iow100 {
	char	hst2cnp_interrupt;	/* R or W interrupts CNP */
	char	i_pad1[2 K -1];
	char	hst2cnp_reset;		/* R or W resets CNP */
	char	i_pad2[2 K -1];
};

struct ether_addr {
	u_char	ea_addr[6];
};
#define ETHADDR		struct ether_addr

struct ethlist
{
	u_long	e_listsize;		/* active addr entries */
	ETHADDR	e_baseaddr;		/* addr lance is working with */
	ETHADDR e_addrs[16];		/* possible addresses */
};
#define	ETHLIST	struct ethlist

struct cnpstat
{
	u_long e_xmit_successful;		/* Successful transmissions */
	u_long e_mult_retry;		/* multiple retries on xmit */
	u_long e_one_retry;		/* single retries */
	u_long e_fail_retry;		/* too many retries */
	u_long e_deferrals;		/* xmit delayed due to active medium */
	u_long e_xmit_buff_err;		/* xmit chaining failed, can't happen */
	u_long e_silo_underrun;		/* transmit data fetch failed */
	u_long e_late_coll;		/* collision after xmit */
	u_long e_lost_carrier;
	u_long e_babble;			/* xmit length > 1518 */
	u_long e_no_heartbeat;		/* transceiver mismatch, not an error */
	u_long e_xmit_mem_err;
	u_long e_rcv_successful;		/* good receptions */
	u_long e_rcv_missed;		/* no recv buff available */
	u_long e_crc_err;			/* checksum failed */
	u_long e_frame_err;		/* crc error && data len != 0 mod 8 */
	u_long e_rcv_buff_err;		/* rcv chaining failed, can't happen */
	u_long e_silo_overrun;		/* receive data store failed */
	u_long e_rcv_mem_err;
};
#define	CNPSTAT	struct cnpstat

struct ring
{
	u_short	r_rdidx;
	u_short	r_wrtidx;
	u_short	r_size;
	u_short	r_pad;
	u_long	r_slot[1];
};
#define	RING	struct ring

#define r_threshold r_pad

struct ring32
{
	u_short	r_rdidx;
	u_short	r_wrtidx;
	u_short	r_size;
	u_short	r_pad;			/* to make VAXen happy */
	u_long	r_slot[32];
};
#define	RING32	struct ring32

struct ring256
{
	u_short	r_rdidx;
	u_short	r_wrtidx;
	u_short	r_size;
	u_short	r_pad;			/* to make VAXen happy */
	u_long	r_slot[256];
};
#define	RING256	struct ring256

/*
 * 	CNP Ram data layout
 *
 *	If you don't put it here - it isn't there
 *
 */
struct cnpdevice {
	union {
		char	all_ram[RAM_WINDOW];
		struct {
			char	t_pad[0x1000];
			u_short	t_go;
			u_short	t_csr;
			u_long	t_pstart;
		} t;
		struct {
			char	pad7[ 0x1100 ];	/* starts 0x1100 - 0x2000 */
			u_short	e_cnpstate;		
			u_short	e_cnpmode;		
			u_long	e_cnpbase;		
			u_short	e_cnprun;
			u_short	e_intvector;

			RING256	h_tocnp;		
			RING256	h_hostfree;		
			RING256	e_tohost;		
			RING256	e_cnpfree;		
			RING256 e_rcvdma;
			RING256 h_rcv_d;

			CNPSTAT	e_stat;
			ETHLIST	e_netaddr;		
		} iface;
		struct {
			char	r_pad[0x4000];
			char	r_scratch[RAM_WINDOW - 0x4000];
		} r;
	} cnp_u;
	struct iow100	cnp_iow;
};

#define	CNPDEVICE	struct cnpdevice

/*
 * cnp csr bit defs
 */
#define	CNPCSR_ONLINE	0x2		/* entered application firmware */
#define	CNPCSR_RDY	0x4		/* reset complete */
#define	CNPCSR_OVPROM	0x8		/* don't transfer to prom on reset */
#define	CNPCSR_IE	0x40		/* interrupt enable for bus debug */
#define	CNPCSR_ERR	0x8000		/* cnp detected error */

#define	cnp_ram		cnp_u.all_ram
#define	cnp_go		cnp_u.t.t_go
#define	cnp_csr		cnp_u.t.t_csr
#define	cnp_prog_start	cnp_u.t.t_pstart
#define cnp_state	cnp_u.iface.e_cnpstate
#define cnp_mode	cnp_u.iface.e_cnpmode
#define cnp_base	cnp_u.iface.e_cnpbase
#define cnp_cnprun	cnp_u.iface.e_cnprun
#define cnp_intvector	cnp_u.iface.e_intvector
#define cnp_tocnp	cnp_u.iface.h_tocnp
#define cnp_hostfree	cnp_u.iface.h_hostfree
#define cnp_tohost	cnp_u.iface.e_tohost
#define cnp_cnpfree	cnp_u.iface.e_cnpfree
#define cnp_rcvdma	cnp_u.iface.e_rcvdma
#define cnp_rcv_d	cnp_u.iface.h_rcv_d
#define cnp_stat	cnp_u.iface.e_stat
#define cnp_addr	cnp_u.iface.e_netaddr
#define cnp_scratch	cnp_u.r.r_scratch

#define CNPVAL		0xff	/* poke in cnp_iow.hst2cnp_interrupt */
#define RESETVAL	0x00	/* poke in cnp_iow.cnp2hst_clear_intr */

#define INTR_CNP(addr)		addr->cnp_iow.hst2cnp_interrupt = CNPVAL
#define RESET_CNP(addr)		addr->cnp_iow.hst2cnp_reset = CNPVAL

/*
 * state bits
 */
#define S_CNPRESET	01		/* cnp is in reset state */
#define S_CNPRUN	02		/* cnp is in run state */

/*
 * mode bits
 */
#define E_SWAP16		0x1		/* swap two octets within 16 */
#define E_SWAP32		0x2		/* swap 16s within 32 */
#define E_SWAPRD		0x4		/* swap on read */
#define E_SWAPWRT		0x8		/* swap on write */
#define E_DMA			0x10		/* cnp does xmit data moving */
#define E_XMIT_DMA E_DMA
#define E_ARP			0x20		/* Reserved for Future Use */
#define E_DONTINT		0x40		/* do not interrupt host */
#define E_NOTERM		0x80		/* don't use serial port */
#define E_RCV_DMA		0x100		/* cnp moves rcv data */
#define E_DO_TRAILERS		0x200		/* Reserved for Future Use */
#define E_UNDO_TRAILERS		0x400		/* de-encapusulate trailers */

#define E_EXAM_LIST		0x8000	/* cnp should examine addr list */

/*
 * 	Download ioctl definitions
 */

#define mkioctl(type,value) (0x20000000|('type'<<8)|value)

#define CNPIOGO		mkioctl( S,1 )		/* start the cnp */
#define CNPIORESET	mkioctl( S,2 )		/* reset the cnp */

/*
 *	The bus description structure 
 */

struct bus_descr {
	u_long	bd_link;
	u_long	bd_addr;		/* Physical bus address */
	u_long	bd_len;			/* length in bytes */
	u_short bd_mode;		/* For VME, address modifier */
	u_short bd_pad;
};
#define BUS_D struct bus_descr

/*
 * 	The CNP Data Buffer Structure
 */
struct bcb {
	struct bcb *b_link;
	u_short	b_stat;
	u_short	b_len;
	u_long	b_addr;
	u_short	b_msglen;
	u_short	b_reserved;
	/* From here on is reserved for the host's use */
	struct mbuf *b_mbuf;
	struct bus_descr *b_bus_descr;
	u_long	b_pad[2];
};
#define	BCB	struct bcb

/*
 * BCB b_stat field defines
 */
#define	BCB_DONE	0x8000		/* buffer complete */
#define	BCB_ERR		0x4000		/* error summary */
#define	BCB_FRAME	0x2000		/* framing error */
#define	BCB_OFLO	0x1000		/* silo overflow */
#define	BCB_CRC		0x0800		/* crc error */
#define	BCB_RXBUF	0x0400		/* rx buffer err */
#define	BCB_STP		0x0200		/* start of packet */
#define	BCB_ENP		0x0100		/* end of packet */
#define	BCB_MEMERR	0x0002		/* memory error */
#define	BCB_MISSED	0x0001		/* missed packet */

#define	DSF_LOCK	1
#define DSF_RUNNING	2
#define	DSF_SETADDR	3

#define MOD_ENAL 1
#define MOD_PROM 2
