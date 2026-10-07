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
 * HISTORY
 * $Log: if_hippi.h,v $
 * Revision 1.4  1994/11/18  20:33:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/23  00:46:10  hobbes
 *  Reviewer: bernie keany, arlin davis
 *  Risk: low
 *  Benefit or PTS #: debugging info always checked, slower HIPPI TCP/IP processing
 *  Testing: built R1.2 server .. tested on a WW45 system.
 *  Module(s): net/if_hippi.h net/if_hippisubr.c
 *
 * made the debugging logic be a compile time feature
 *
 * Revision 1.2  1993/08/18  00:14:54  hobbes
 * Increase size of MAXARTSIZE to 64 this should be big enough for
 * PS-32 switches.
 *
 * Revision 1.1  1993/05/20  20:35:10  hobbes
 * Initial check-in for HIPPI interface support.
 *
 */
/*
 * if_hippi.h
 *
 * Largely based on PD HIPPI Device Driver code from:
 *	Thomas Skibo
 *	skibo@ncsa.uiuc.edu
 *
 *	Header file for HiPPI interface in an OSF1/AD server.  Describes:
 *	  HIPPI Address Resolution Tables (ART)
 *	  HIPPI header formats
 *
 */

#ifndef _if_hippi_h
#define _if_hippi_h


/* ART- Address Resolution Table.  Internal tables are used to resolve
 * protocol addresses.
 *
 * One table maps protocol addresses (usually IP addresses) to link
 * addresses, which include an IEEE ULA (universal LAN address) and
 * an I-field each.  Another table is used to map IEEE ULA's to I-field
 * values.  The I-field is used for switch control to specify a
 * destination switch port or to provide source routing for a transmission.
 */


/* The l-entry table maps ULA's (IEEE 48-bit addresses) to I-fields.
 */
struct art_lentry {
	u_char	al_ula[6];	/* IEEE 802.2 ula address */
	u_char	al_valid, al_flags;
	u_long	al_Ifield;		/* HiPPI I-field to reach host */
};

/* The p-entry (protocol address entry) table maps protocol addresses
 * to their corresponding l-entry (link address entry) in the l-entry table.
 * An array of struct art_pentry's constitutes a protocol address table.
 */
struct art_pentry {
	struct sockaddr ap_pa;		/* protocol address */
	struct art_lentry ap_ha;	/* link address entry, NULL=invalid */
};


#define MAXARTSIZE	64		/* MUST be a power of 2!!! */
#define ARTINDEXMASK	(MAXARTSIZE-1)	/* ...or this won't work */


/* "Hash" functions for ART tables.  The ART_HASH_SOCKADDR returns a hash
 * value from a sockaddr structure (right now, it assumes its a sockaddr_in
 * and hashes from the inaddr field).  ART_HASH_ULA returns a hash value
 * from a mac_addr_802 (a 6-byte ethernet-like address).
 */
#define ART_HASH_SOCKADDR(s) ((*((unsigned int*)((s).sa_data+2)))&ARTINDEXMASK)
#define ART_HASH_ULA(u) ((*((unsigned short *)&(u).l_fmt[4]))&ARTINDEXMASK)


/* These appear in the al_flags field: */
#define ART_HASHCOLL	0x02		/* Hash function collided. */





/* hippi interface values:
 */

#define HIPPI_MTU	65535		/* MTU size placed in if structure. */
#define HIPPI_IF_NAME	"ifhip"		/* Name of if interface. */

/*
 * HiPPI Ifield, FP and LE header formats.
 */

struct hippi_ifield {
	u_long Ifield;
	u_long filler[7];
};

union hippi_fp_header {
	struct bits {
		u_long	d2_off:3;
		u_long	d1_size:8;
		u_long	res:11;
		u_long	b:1;
		u_long	p:1;
		u_long	ulp:8;
#define ULP_MI			2			/* ULP codes for FP header */
#define ULP_MI_INIT		3
#define ULP_LE			4
#define ULP_IPI3_SLAVE	6
#define ULP_IPI3_MASTER	7

		u_long	d2_size;
	} fields;

	struct {
		u_long w1;
		u_long w2;
	} words;
};


struct hippi_le_header {
	u_int	fc:3;
	u_int	res1:9;
	u_int	dest_hub_address:12;
	u_int	dest_port:8;

	u_int	res2:12;
	u_int	src_hub_address:12;
	u_int	src_port:8;

	u_short	rsvd1;
	u_char	dest_ieee_addr[6];
	u_short	le_local_admin;
	u_char	src_ieee_addr[6];
};


struct hippi_header {
	struct hippi_ifield		cci;
	union  hippi_fp_header	fp;
	struct hippi_le_header	le;
	struct llc				llc;
};

struct inbound_hippi_header {
	union  hippi_fp_header	fp;
	struct hippi_le_header	le;
	struct llc				llc;
};

/*
 * Debuging HIPPI network traffic
 */

#define HIPPIPRINTFS 0

int hippiprintfs;

#define HIPPIOUT 0x0001
#define HIPPIIN  0x0002

#endif _if_hippi_h
