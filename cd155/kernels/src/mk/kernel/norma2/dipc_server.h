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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1994  Intel Corporation.
 *
 *	$Id: dipc_server.h,v 1.6 1994/11/18 20:58:12 mtm Exp $
 */

/*
 *	Requests for the NORMA enqueue server.
 */
#define	DIPC_REQUEST_NOP		0	/* just reply now */
#define	DIPC_REQUEST_ENQUEUE		1	/* enqueue a message */
#define	DIPC_REQUEST_BUY_TRANSITS	2	/* get transits from a port */
#define	DIPC_REQUEST_SELL_TRANSITS	3	/* return transits to a port */
#define	DIPC_REQUEST_MIGRATION		4	/* begin migration */
#define DIPC_REQUEST_DEAD_NAME		5	/* request dn notification */
#define	DIPC_NOTIFY_QUEUE_AVAIL		6	/* queue is not full */
#define	DIPC_NOTIFY_POLYMORPH		7	/* principal->fwd proxy */
#define DIPC_NOTIFY_DEAD_NAME		8	/* dead name notification */


/*
 *	Possible reply status.
 */
#define	DIPC_OK			0	/* everything went well */
#define	DIPC_DEST_INVALID	1	/* unknown destination port */
#define	DIPC_DEST_DEAD		2	/* destination port was dead */
#define	DIPC_DEST_FULL		3	/* queue limit exceeded */
#define	DIPC_DEST_MOVED		4	/* destination port has moved */
#define	DIPC_BOGUS_REQUEST	5	/* unknown request */


/*
 *	Every request to the server requires the following
 *	structure at the head of the request.
 */
typedef struct dipc_request_header {
	unsigned short		type;		/* request/reply type */
	unsigned short		status;		/* current status code */
	rpc_node_t		node;		/* destination node */
	dipc_uid_t		uid;		/* destination port */
} dipc_header_t;


/*
 *	An enqueue request contains enough information to
 *	construct a meta_kmsg and enqueue on a port.
 */
typedef struct dipc_enqueue_request {
	dipc_header_t		request;	/* request/reply header */
	mach_msg_option_t	options;	/* options from sender */
	rdma_token_t		token;		/* sender's endpoint */
	struct {
		unsigned int	_size:30,	/* sender's kmsg size */
				_type:2;	/* meta or local kmsg */
	} kmsg_size_type;
#define	kmsg_size	kmsg_size_type._size
#define	kmsg_type	kmsg_size_type._type
} *dipc_enqueue_op_t;


/*
 *	How To Buy Send Rights With Little or No Money Down
 */
typedef struct dipc_transit_request {
	dipc_header_t		request;	/* request/reply header */
	long			transits;	/* transit count */
} *dipc_transit_op_t;


/*
 *	Extract migration state from a principal.
 */
typedef struct dipc_migration_request {
	dipc_header_t		request;	/* header */
	rpc_node_t		from;		/* originating node */
	dipc_port_migration_state	state;		/* principal state */
} *dipc_migration_op_t;


/*
 *	Message queue no longer full notification.
 */
typedef struct dipc_queue_avail_request {
	dipc_header_t		request;	/* request/reply header */
} *dipc_queue_avail_op_t;


/*
 *	Polymorph a principal into a forwarding proxy notification.
 */
typedef struct dipc_polymorph_request {
	dipc_header_t		request;	/* request/reply header */
	rpc_node_t		from;		/* originating node */
} *dipc_polymorph_op_t;


/*
 *	Request dead names notification
 */
typedef struct dipc_request_dead_name {
	dipc_header_t		request;
	rpc_node_t		from;
} *dipc_request_dead_op_t;


/*
 *	Notification of a dead name
 */
typedef struct dipc_notify_dead_name_request {
	dipc_header_t		request;
} *dipc_notify_dead_op_t;


/*
 *	Just for testing...
 */
typedef struct dipc_nop {
	dipc_header_t		request;	/* request/reply header */
} *dipc_nop_t;

/*
 *  Used to make sure payload bay is big enough to hold the largest
 *  request type.
 */
typedef union {
	struct dipc_enqueue_request		en_queue; /* macro conflict */
	struct dipc_transit_request		transit;
	struct dipc_migration_request		migration;
	struct dipc_queue_avail_request		queue_avail;
	struct dipc_polymorph_request		polymorph;
	struct dipc_request_dead_name		dead_name;
	struct dipc_notify_dead_name_request	notify_deadname;
	struct dipc_nop				nop;
} dipc_request_union_t;

/*
 *  Debug logging support.
 */

#define server_log1	NORMA_LOG1
#define server_log2	NORMA_LOG2
#define server_log3	NORMA_LOG3
#define server_log4	NORMA_LOG4
#define server_log5	NORMA_LOG5
#define server_log6	NORMA_LOG6
#define server_log7	NORMA_LOG7
#define server_log8	NORMA_LOG8

#define server_entry0(func) \
	NORMA_ENTRY0(SERVER_LOG_ID, func)
#define server_entry1(func, a)      \
	NORMA_ENTRY1(SERVER_LOG_ID, func, a)
#define server_entry2(func, a, b) \
	NORMA_ENTRY2(SERVER_LOG_ID, func, a, b)
#define server_entry3(func, a, b, c) \
	NORMA_ENTRY3(SERVER_LOG_ID, func, a, b, c)
#define server_entry4(func, a, b, c, d) \
	NORMA_ENTRY4(SERVER_LOG_ID, func, a, b, c, d)
#define server_entry5(func, a, b, c, d, e) \
	NORMA_ENTRY5(SERVER_LOG_ID, func, a, b, c, d, e)
#define server_entry6(func, a, b, c, d, e, f) \
	NORMA_ENTRY6(SERVER_LOG_ID, func, a, b, c, d, e, f)
