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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: tihdr.h,v $
 * Revision 1.5  1994/11/18  20:39:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:22:19  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:18:56  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:08:42  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:39:30  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:37:25  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:36:19  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:40:14  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1991/10/17  11:11:42  sjs
 * fixed RCS comments
 *
 * Revision 2.2  91/10/14  12:58:44  sjs
 * Check in
 * 
 * Revision 3.0  91/09/27  11:51:43  emcmanus
 * Installed for streams.
 * 
 * Revision 1.8  90/10/31  14:05:24  devrcs
 * 	Post-file-rearrangement pathnames.
 * 	[90/10/11  08:30:51  tmt]
 * 
 * 	Checkin prior to file rearrangement.
 * 	[90/10/08  13:41:18  tmt]
 * 
 * Revision 1.7  90/10/07  14:44:59  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:31:23  gm]
 * 
 * 	Make TE_NOEVENTS, TS_NOSTATES accurate. Add useful comments.
 * 	[90/09/21  09:04:10  tmt]
 * 
 * Revision 1.6  90/09/13  11:50:28  devrcs
 * 	Add proper TE_* defines, TS_NOSTATES, TE_NOERRORS.
 * 	Fix "union T_primitives" elements.
 * 	Fix TS_WACK_OPTREQ spelling, remove TS_BAD_STATE.
 * 	[90/08/27  13:58:40  tmt]
 * 
 * Revision 1.5  90/06/29  13:38:25  devrcs
 * 	No changes.
 * 	[90/06/26  21:29:57  gm]
 * 
 * Revision 1.4  90/06/22  20:49:21  devrcs
 * 	No changes.
 * 
 * Revision 1.3  90/05/24  23:05:12  devrcs
 * 	Changed _TIHDR_ to _TIHDR_H.
 * 	[90/05/18  13:03:55  nolting]
 * 
 * Revision 1.2  90/02/23  00:31:18  devrcs
 * 	Added more files from Mentat software.
 * 	[90/02/14  21:38:05  nolting]
 * 
 * $EndLog$
 */
/** Copyright (c) 1989  Mentat Inc.
 ** tihdr.h 1.2, last change 12/20/89
 **/

#ifndef _TIHDR_H
#define _TIHDR_H

/* User generated requests */
#define	T_BIND_REQ		1	/* Bind request			*/
#define	T_CONN_REQ		2	/* Connection request		*/
#define	T_CONN_RES		3	/* Connection response		*/
#define	T_DATA_REQ		4	/* Data request			*/
#define	T_DISCON_REQ		5	/* Disconnect request		*/
#define	T_EXDATA_REQ		6	/* Expedited data request	*/
#define	T_INFO_REQ		7	/* Information request		*/
#define	T_OPTMGMT_REQ		8	/* Option management request	*/
#define	T_ORDREL_REQ		9	/* Orderly release request	*/
#define	T_UNBIND_REQ		10	/* Unbind request		*/
#define	T_UNITDATA_REQ		11	/* Unitdata (datagram) request	*/
#define	T_FEEDBACK_REQ		19	/* Used internally		*/

/* Transport generated indications and acknowledgements */
#define	T_BIND_ACK		20	/* Bind acknowledgement		*/
#define	T_CONN_CON		21	/* Connection confirmation	*/
#define	T_CONN_IND		22	/* Connection indication	*/
#define	T_DATA_IND		23	/* Data indication		*/
#define	T_DISCON_IND		24	/* Disconnect indication	*/
#define	T_ERROR_ACK		25	/* Error acknowledgement	*/
#define	T_EXDATA_IND		26	/* Expedited data indication	*/
#define	T_INFO_ACK		27	/* Information acknowledgement	*/
#define	T_OK_ACK		28	/* OK acknowledgement		*/
#define	T_OPTMGMT_ACK		29	/* Option management ack.	*/
#define	T_ORDREL_IND		30	/* Orderly release indication	*/
#define	T_UNITDATA_IND		31	/* Unitdata (datagram) ind.	*/
#define	T_UDERROR_IND		32	/* Unitdata error indication	*/

/* State values */
#define	TS_UNBND		1	/* Unbound			*/
#define	TS_WACK_BREQ		2	/* Wait for ack of BIND_REQ	*/
#define	TS_WACK_UREQ		3	/* Wait for ack of UNBIND_REQ	*/
#define	TS_IDLE			4	/* Ho hum			*/
#define	TS_WACK_OPTREQ		5	/* Wait for ack of OPTMGMT_REQ	*/
#define	TS_WACK_CREQ		6	/* Wait for ack of CONN_REQ	*/
#define	TS_WCON_CREQ		7	/* Wait for confirm of CONN_REQ	*/
#define	TS_WRES_CIND		8	/* Wait for response to CONN_IND*/
#define	TS_WACK_CRES		9	/* Wait for ack of CONN_RES	*/
#define	TS_DATA_XFER		10	/* Data transfer		*/
#define	TS_WIND_ORDREL		11	/* Indication of RD/!WR release	*/
#define	TS_WREQ_ORDREL		12	/* Request for WR/!RD release	*/
#define	TS_WACK_DREQ6		13	/* Wait for DISCON_REQ ack	*/
#define	TS_WACK_DREQ7		14	/* ...				*/
#define	TS_WACK_DREQ9		15	/* ...				*/
#define	TS_WACK_DREQ10		16	/* ...				*/
#define	TS_WACK_DREQ11		17	/* ...				*/
#define	TS_WACK_ORDREL		18	/* Used internally (only?)	*/

#define TS_NOSTATES		19	/* Includes "missing" 0 */

/* Transport events */
#define	TE_BIND_REQ		1	/* Bind request			*/
#define	TE_UNBIND_REQ		2	/* Unbind request		*/
#define	TE_OPTMGMT_REQ		3	/* Option management request	*/
#define	TE_BIND_ACK		4	/* Bind acknowledgement		*/
#define	TE_OPTMGMT_ACK		5	/* Option management ack.	*/
#define	TE_CONN_REQ		6	/* Connect request		*/
#define	TE_CONN_RES		7	/* Connect response		*/
#define	TE_DISCON_REQ		8	/* Disconnect request		*/
#define	TE_DATA_REQ		9	/* Data request			*/
#define	TE_EXDATA_REQ		10	/* Expedited data request	*/
#define	TE_ORDREL_REQ		11	/* Orderly release request	*/
#define	TE_CONN_IND		12	/* Connect indication		*/
#define	TE_CONN_CON		13	/* Connect confirmation		*/
#define	TE_DATA_IND		14	/* Data indication		*/
#define	TE_EXDATA_IND		15	/* Expedited data indication	*/
#define	TE_ORDREL_IND		16	/* Orderly release indication	*/
#define	TE_DISCON_IND1		17	/* Disconnect indication seq 0	*/
#define	TE_DISCON_IND2		18	/* Disconnect indication seq 1	*/
#define	TE_DISCON_IND3		19	/* Disconnect indication seq >1	*/
#define	TE_ERROR_ACK		20	/* Error acknowledgement	*/
#define	TE_OK_ACK1		21	/* OK acknowledgement seq 0	*/
#define	TE_OK_ACK2		22	/* OK acknowledgement seq 1 q=r	*/
#define	TE_OK_ACK3		23	/* OK acknowledgement seq 1 q!=r*/
#define	TE_OK_ACK4		24	/* OK acknowledgement seq >1	*/
#define TE_PASS_CONN		25	/* Pass connection		*/
#define	TE_UNITDATA_REQ		26	/* Unitdata (datagram) request	*/
#define	TE_UNITDATA_IND		27	/* Unitdata indication		*/
#define	TE_UDERROR_IND		28	/* Unitdata error indication	*/

#define TE_NOEVENTS		29	/* Includes "missing" 0 */

/* TLI library constant */
#define TLI_STACK_BUF_SIZE	512

struct T_bind_ack {
	long	PRIM_type;	/* Always T_BIND_ACK */
	long	ADDR_length;
	long	ADDR_offset;
	unsigned long	CONIND_number;
};

struct T_bind_req {
	long	PRIM_type;	/* Always T_BIND_REQ */
	long	ADDR_length;
	long	ADDR_offset;
	unsigned long	CONIND_number;
};

struct T_conn_con {
	long	PRIM_type;	/* Always T_CONN_CON */
	long	RES_length;	/* Responding address length */
	long	RES_offset;
	long	OPT_length;
	long	OPT_offset;
};

struct T_conn_ind {
	long	PRIM_type;	/* Always T_CONN_IND */
	long	SRC_length;
	long	SRC_offset;
	long	OPT_length;
	long	OPT_offset;
	long	SEQ_number;
};

struct T_conn_req {
	long	PRIM_type;	/* Always T_CONN_REQ */
	long	DEST_length;
	long	DEST_offset;
	long	OPT_length;
	long	OPT_offset;
};

struct T_conn_res {
	long	PRIM_type;	/* Always T_CONN_RES */
	queue_t	* QUEUE_ptr;
	long	OPT_length;
	long	OPT_offset;
	long	SEQ_number;
};

struct T_data_ind {
	long	PRIM_type;	/* Always T_DATA_IND */
	long	MORE_flag;
};

struct T_data_req {
	long	PRIM_type;	/* Always T_DATA_REQ */
	long	MORE_flag;
};

struct T_discon_ind {
	long	PRIM_type;	/* Always T_DISCON_IND */
	long	DISCON_reason;
	long	SEQ_number;
};

struct T_discon_req {
	long	PRIM_type;	/* Always T_DISCON_REQ */
	long	SEQ_number;
};

struct T_exdata_ind {
	long	PRIM_type;	/* Always T_EXDATA_IND */
	long	MORE_flag;
};

struct T_exdata_req {
	long	PRIM_type;	/* Always T_EXDATA_REQ */
	long	MORE_flag;
};

struct T_error_ack {
	long	PRIM_type;	/* Always T_ERROR_ACK */
	long	ERROR_prim;	/* Primitive in error */
	long	TLI_error;
	long	UNIX_error;
};

struct T_info_ack {
	long	PRIM_type;	/* Always T_INFO_ACK */
	long	TSDU_size;	/* Max TSDU size */
	long	ETSDU_size;	/* Max ETSDU size */
	long	CDATA_size;	/* Connect data size */
	long	DDATA_size;	/* Disconnect data size */
	long	ADDR_size;	/* TSAP size */
	long	OPT_size;	/* Options size */
	long	TIDU_size;	/* TIDU size */
	long	SERV_type;	/* Service type */
	long	CURRENT_state;	/* Current state */
};

struct T_info_req {
	long	PRIM_type;	/* Always T_INFO_REQ */
};

struct T_ok_ack {
	long	PRIM_type;	/* Always T_OK_ACK */
	long	CORRECT_prim;
};

struct T_optmgmt_ack {
	long	PRIM_type;	/* Always T_OPTMGMT_ACK */
	long	OPT_length;
	long	OPT_offset;
	long	MGMT_flags;
};

struct T_optmgmt_req {
	long	PRIM_type;	/* Always T_OPTMGMT_REQ */
	long	OPT_length;
	long	OPT_offset;
	long	MGMT_flags;
};

struct T_ordrel_ind {
	long	PRIM_type;	/* Always T_ORDREL_IND */
};

struct T_ordrel_req {
	long	PRIM_type;	/* Always T_ORDREL_REQ */
};

struct T_unbind_req {
	long	PRIM_type;	/* Always T_UNBIND_REQ */
};

struct T_uderror_ind {
	long	PRIM_type;	/* Always T_UDERROR_IND */
	long	DEST_length;
	long	DEST_offset;
	long	OPT_length;
	long	OPT_offset;
	long	ERROR_type;
};

struct T_unitdata_ind {
	long	PRIM_type;	/* Always T_UNITDATA_IND */
	long	SRC_length;
	long	SRC_offset;
	long	OPT_length;
	long	OPT_offset;
};

struct T_unitdata_req {
	long	PRIM_type;	/* Always T_UNITDATA_REQ */
	long	DEST_length;
	long	DEST_offset;
	long	OPT_length;
	long	OPT_offset;
};

union T_primitives {
	long			type;
	struct T_bind_ack	bind_ack;
	struct T_bind_req	bind_req;
	struct T_conn_con	conn_con;
	struct T_conn_ind	conn_ind;
	struct T_conn_req	conn_req;
	struct T_conn_res	conn_res;
	struct T_data_ind	data_ind;
	struct T_data_req	data_req;
	struct T_discon_ind	discon_ind;
	struct T_discon_req	discon_req;
	struct T_exdata_ind	exdata_ind;
	struct T_exdata_req	exdata_req;
	struct T_error_ack	error_ack;
	struct T_info_ack	info_ack;
	struct T_info_req	info_req;
	struct T_ok_ack		ok_ack;
	struct T_optmgmt_ack	optmgmt_ack;
	struct T_optmgmt_req	optmgmt_req;
	struct T_ordrel_ind	ordrel_ind;
	struct T_ordrel_req	ordrel_req;
	struct T_unbind_req	inbind_req;
	struct T_uderror_ind	uderror_ind;
	struct T_unitdata_ind	unitdata_ind;
	struct T_unitdata_req	unitdata_req;
};

#endif	/* _TIHDR_H */
