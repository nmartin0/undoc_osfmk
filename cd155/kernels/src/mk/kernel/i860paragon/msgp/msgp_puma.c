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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_puma.c,v 1.17 1995/04/01 05:04:58 robboy Exp $
 */

/*
 * mcmsg_puma.c
 *
 * PUMA compatibility module
 *
 */

 /*
  * This version is for NIC-A or NIC-B nodes with or without BIGPKTS
  * defined.  The interface is the same with or without BIGPKTS.
  * Assumes 40-byte headers and message bodies = 0 mod 32 in all cases.
  * In case BUMPERS is undefined, the same code also works, with no
  * changes to this module.  The BUMPERS code is handled elsewhere in Mach.
  */

#define	MCMSG_MODULE	MCMSG_MODULE_PUMA

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>

#define	PUMA_VERSION	(2 << 16)

#define PUMA_MAX_PKT	1784
#define PUMA_BUFP_RND	32
#define PUMA_PKT_EVEN	16
#define PUMA_HDR_ALIGN	8

/* Bytes in the Sunmos message header, excluding hdr1/hdr2 */
#if BIGPKTS
#define PUMA_HDR_SIZE	32
#else
/* Used to be 24 */
#define PUMA_HDR_SIZE	32
#endif

/*
 * PUMA error codes
 */

#define	PUMA_ERR_CONFIG		-1
#define	PUMA_ERR_HDRMEM		-2
#define	PUMA_ERR_BUFMEM		-3
#define	PUMA_ERR_NHDR		-4
#define	PUMA_ERR_ALIGN		-5
#define	PUMA_ERR_BUFSIZE	-6
#define	PUMA_ERR_NOSLOTS	-7
#define	PUMA_ERR_VERSION	-8
#define PUMA_ERR_SLOT		-9
#define PUMA_ERR_STRIP		-10
#define PUMA_ERR_NODE		-11
#define PUMA_ERR_COUNT		-12
#define PUMA_ERR_BODY		-13
#define PUMA_ERR_HEADER		-14

/*
 * PUMA slots
 */

#define	PUMA_SLOTS	100

typedef
struct puma_slot {
	mcmsg_task_t	*mt;
	unsigned long	hdrp;
	unsigned long	nhdr;
	unsigned long	hdrsize;
	unsigned long	strip;
	unsigned long	hdri;
	unsigned long	bufp;
	unsigned long	offset;
	unsigned long	bufsize;
	long		fill[7];
} puma_slot_t;

puma_slot_t	mcmsg_puma_slot[PUMA_SLOTS];

/*
 * PUMA Send side
 */

/*
 * Send request structures.
 * One of these is used for each send request to transfer the send info
 * across mcmsg_send(). Becomes obsolete when mcmsg_send() can take more
 * than two arguments.
 */

#define PUMA_NSEND	16

typedef
struct puma_send_request {
	unsigned long	hdr_buf;  /* pointer to user's Sunmos header */
	unsigned long	data_buf;  /* pointer to user's data */
	unsigned short	count;
	unsigned short	slot;
	unsigned long	route;
	unsigned long	statusp;
} puma_send_request_t;

puma_send_request_t mcmsg_puma_send_request[PUMA_NSEND];

/* Subscript of next available send_request header */
unsigned long	mcmsg_puma_send_request_in;

/* Subscript of send_request header for current pending message */
unsigned long	mcmsg_puma_send_request_out;

/*
 * Maximum node number and precalculated second header word (source part)
 * to speed send processing.
 */

unsigned long	mcmsg_puma_maxnode;
unsigned long	mcmsg_puma_source;

/*
 * Initialize global variables
 */

mcmsg_puma_init()
{
	int	i;

	mcmsg_puma_send_request_in = 0;
	mcmsg_puma_send_request_out = 0;
	mcmsg_puma_maxnode = paragon_mesh_x * paragon_mesh_y;
	mcmsg_puma_source = ipsc_physnode << 16;
	for (i = 0; i < PUMA_SLOTS; i++) {
		mcmsg_puma_slot[i].mt = 0;
	}
}

/*
 * PUMA send interface
 * Invoked by user via PUMA library to send a message
 */

mcmsg_puma_send(mt, hdr_buf,  count, node, slot, statusp, data_buf)
	mcmsg_task_t	*mt;
	unsigned long	hdr_buf;	/* Sunmos header */
	unsigned long	count;
	unsigned long	node;
	unsigned long	slot;
	unsigned long	statusp;
	unsigned long	data_buf;	/* body of data */
{
	int		st;
	int		*stp;
	register	rqi;
	register	rqn;
	register puma_send_request_t *rqp;

	if (node >= mcmsg_puma_maxnode) {
		st = PUMA_ERR_NODE;
		goto err;
	}
	if (slot >= PUMA_SLOTS) {
		st = PUMA_ERR_SLOT;
		goto err;
	}
	if (count > PUMA_MAX_PKT) {
		st = PUMA_ERR_COUNT;
		goto err;
	}

	/* Allocate a message header */
	rqi = mcmsg_puma_send_request_in;
	rqn = (rqi+1) & (PUMA_NSEND-1);
	/* If we're out of free headers... */
	if (rqn == mcmsg_puma_send_request_out) {
		st = -3;
		goto err;
	}
	mcmsg_puma_send_request_in = rqn;

	/* Fill out the message header */
	rqp = &mcmsg_puma_send_request[rqi];
	rqp->hdr_buf = hdr_buf;
	rqp->data_buf = data_buf;
	rqp->count = count;
	rqp->slot = slot;
	rqp->route = calculate_route(node);
	rqp->statusp = statusp;

mcmsg_trace_debug("puma_send buf/count/node ", 3,  hdr_buf, count, node, 0);

	/* Send it out.  This call results in mcmsg_send_puma1 getting
	 * called */
	mcmsg_send(mt, MCTRL_PUMA1, 0, 0);
	return 0;

err:
	stp = (int *)mcmsg_validate_long(mt, statusp);
	if (stp != 0) {
		*stp = st;
	}
	return st;
}

/*
 * Packet send procedure for PUMA send.
 * Lower level routine, called indirectly via a table from mcmsg_send().
 * This code assumes both the Sunmos header and the body of the data
 * are properly aligned.  The library code must do that.
 */

mcmsg_send_puma1(mt)
	mcmsg_task_t	*mt;
{
	register	rqi, i;
	register puma_send_request_t *rqp;
	register unsigned long count;
	register unsigned long hdr_buf;
	register unsigned long data_buf;
	register unsigned long bp1;
	register unsigned long bp2;
	register unsigned long hdr1;
	register unsigned long hdr2;
	int		*stp;
	unsigned long	h[8];
	unsigned long *bufp, *kernelbufp;

	/* Get a pointer to the current message header */
	rqi = mcmsg_puma_send_request_out;
	rqp = &mcmsg_puma_send_request[rqi];
	/* Increment subscript to point to next header */
	mcmsg_puma_send_request_out = (rqi + 1) & (PUMA_NSEND-1);
	hdr_buf = rqp->hdr_buf;
	data_buf = rqp->data_buf;
	/* The count is what the user passed us. In the case of host_send(),
	   used by Sunmos utilities, this is (message data length padded
  	   to a multiple of 16) + sizeof(SUNMOS_MSG_HEAD)
	 */
	count = rqp->count;

	/* Pull out the message header. 
	   Get pointers to each word of the 8-word header */
	bufp = (unsigned long *)hdr_buf;
        kernelbufp=(unsigned long *)mcmsg_validate_read1(bufp, 4, mt->dirbase);
	if (kernelbufp == 0) {
		mcmsg_trace_drop("send_puma1 invalid header", hdr_buf);
		stp = (int *)mcmsg_validate_long(mt, rqp->statusp);
		if (stp != 0) {
			*stp = PUMA_ERR_HEADER;
		}
		return;
	}
	bufp++;

        h[0] = *kernelbufp++;
        for (i=1; i<PUMA_HDR_SIZE/sizeof(unsigned long); i++)
        {
                /* If crossing a page boundary */
                if (((long)bufp & (MSG_PAGE_SIZE -1)) == 0){
                        kernelbufp = (unsigned long *)mcmsg_validate_read1(bufp,
                          sizeof(unsigned long), mt->dirbase);
			if (kernelbufp == 0) {
				mcmsg_trace_drop("send_puma1 invalid header",
						hdr_buf);
				stp = (int *)mcmsg_validate_long(mt,									 rqp->statusp);
				if (stp != 0) {
					*stp = PUMA_ERR_HEADER;
				}
				return;
			}
                } 
                bufp++;
                h[i] = *kernelbufp++;
        }

	count -= PUMA_HDR_SIZE;
#if	!BIGPKTS
	/* If BIGPKTS, Mach automatically rounds up the message body to
 	   0 mod 32.  If !BIGPKTS, we do the same, so the interface to
	   Sunmos is uniform .
           In case the end of the message body is near a page boundary,
           this could cause us  to try to access nonexistent memory,
           but the host_send library aligns its buffer on
           32-byte boundaries, so this does not happen.
         */
        count = (count + (LTU_ALIGN-1)) & ~(LTU_ALIGN-1);
#endif	!BIGPKTS

	if (count > 0) {
		/* Point to start and end of current message in buffer.
		   It's up to the library code to make sure it's aligned */
		bp1 = mcmsg_validate_read1(data_buf, count, mt->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0) {
			mcmsg_trace_drop("puma1 send invalid buffer", data_buf);
			stp = (int *)mcmsg_validate_long(mt, rqp->statusp);
			if (stp != 0) {
				*stp = PUMA_ERR_BODY;
			}
			return;
		}
	}

        /*
         * Format and send the message header:
         *
         *      +---------------+---------------+
         *      |        route                  | (stripped off by hardware)
         *      +---------------+---------------+
         *      |       (reserved)              | (stripped off by hardware)
         *      +---------------+---------------+
         * hdr1 | slot          | MCTRL_PUMA1   | (packet type)
         *      +---------------+---------------+
         * hdr2 | source node   | count         |
         *      +---------------+---------------+
         */

	hdr1 = MCTRL_PUMA1 | (rqp->slot << 16);
	/* Give Sunmos a count of header + body.  We use rqp->count here,
	   so in case we've modified the count, the receiver still gets
	   the sender's idea of count in header
	 */
	hdr2 = rqp->count | mcmsg_puma_source;



	mcmsg_trace_send(hdr1, hdr2, 0, 0, 0, 0);

	if (count > 0) {
		/* Send the header */
		mcmsg_send_hdr10(rqp->route, hdr1, hdr2, h[0], h[1], h[2],
			h[3], h[4],h[5], h[6],h[7]);
		/* Send the body and do any necessary bumper and EOD
		 * stuff.  Mach pads the body to a multiple of 32 bytes. 
		 */
		mcmsg_send_buf(bp1, bp2, count);
	} else {
		mcmsg_send_hdr10_eod(rqp->route, hdr1, hdr2, h[0], h[1], h[2],
			h[3], h[4],h[5], h[6],h[7]);
	}


	mcmsg_trace_debug("send_puma1 buf/count ", 2,  bp1, count, 0, 0);
	stp = (int *)mcmsg_validate_long(mt, rqp->statusp);
	if (stp != 0) {
		*stp = 1;
	}
	return;
}

/*
 * PUMA Receive side
 */

/*
 * Receive info structure
 */

typedef
struct puma_hdr {
	unsigned long	count;
	long		source;
	unsigned long	buf;
	long		fill;
	unsigned long	strip[4];
} puma_hdr_t;

/*
 * PUMA receive setup interface
 * Called by the PUMA library once for initialization.
 */

mcmsg_puma_setup(mt, hdrp, nhdr, bufp, bufsize, hdrsize, strip)
	mcmsg_task_t	*mt;	/* comes from the mach kernel */
	/*
	 * Remaining arguments are pointers to objects allocated by
	 * the user and wired by the library
	 */
	unsigned long	hdrp;	/* PUMA message headers */
	unsigned long	nhdr;
	unsigned long	bufp;	/* data buffers */
	unsigned long	bufsize;
	unsigned long	hdrsize;
	unsigned long	strip;
{
	int		slot;
	puma_slot_t	*ps;

	/* Check for valid args */
	if (mt == 0) {
		return PUMA_ERR_CONFIG;
	}
	if (nhdr == 0) {
		return PUMA_ERR_NHDR;
	}
	if (hdrp & PUMA_HDR_ALIGN-1) {
		return PUMA_ERR_ALIGN;
	}
	if (hdrsize & PUMA_HDR_ALIGN-1) {
		return PUMA_ERR_ALIGN;
	}
	if (strip & PUMA_HDR_ALIGN-1) {
		return PUMA_ERR_ALIGN;
	}
	if (strip > hdrsize - 16) {
		return PUMA_ERR_STRIP;
	}
	if (bufsize < PUMA_MAX_PKT) {
		return PUMA_ERR_BUFSIZE;
	}

	for (slot = 0; slot < PUMA_SLOTS; slot++) {
		if (mcmsg_puma_slot[slot].mt == 0) {
			break;
		}
	}
	if (slot == PUMA_SLOTS) {
		return PUMA_ERR_NOSLOTS;
	}
	ps = &mcmsg_puma_slot[slot];
	ps->mt = mt;
	ps->hdrp = hdrp;
	ps->nhdr = nhdr;
	ps->hdri = 0;
	ps->bufp = bufp;
	ps->offset = 0;
	ps->bufsize = bufsize;
	ps->hdrsize = hdrsize;
        /* We always strip off a header of PUMA_HDR_SIZE. The user's strip
	   parameter, if it exists, is now ignored */
	ps->strip = PUMA_HDR_SIZE ;

	return PUMA_VERSION | slot;
}

/*
 * PUMA receive reset interface
 */

mcmsg_puma_reset(mt, slot)
	mcmsg_task_t	*mt;
	unsigned long	slot;
{
	puma_slot_t	*ps;

	if (slot >= PUMA_SLOTS) {
		return PUMA_ERR_SLOT;
	}
	if (mt == 0) {
		return PUMA_ERR_CONFIG;
	}
	ps = &mcmsg_puma_slot[slot];
	if (ps->mt != mt) {
		return PUMA_ERR_SLOT;
	}
	ps->hdri = 0;
	ps->offset = 0;
	return 0;
}

/*
 * PUMA receive release interface
 */

mcmsg_puma_release(mt, slot)
	mcmsg_task_t	*mt;
	unsigned long	slot;
{
	puma_slot_t	*ps;

	if (slot >= PUMA_SLOTS) {
		return PUMA_ERR_SLOT;
	}
	if (mt == 0) {
		return PUMA_ERR_CONFIG;
	}
	ps = &mcmsg_puma_slot[slot];
	if (ps->mt != mt) {
		return PUMA_ERR_SLOT;
	}
	ps->mt = 0;
	return 0;
}

/*
 * PUMA trace stub
 */

mcmsg_puma_trace(mt, data)
	mcmsg_task_t	*mt;
	int		data;
{

	return	data;
}

/*
 * PUMA destroy task
 * called from msgp_task.c
 */

mcmsg_puma_destroy_task(mt)
	mcmsg_task_t	*mt;
{
	unsigned long	slot;

	for (slot = 0; slot < PUMA_SLOTS; slot++) {
		if (mcmsg_puma_slot[slot].mt == mt) {
			mcmsg_puma_slot[slot].mt = 0;
		}
	}
}

/*
 * Receive PUMA message.
 *
 * Receive packet procedure called from interrupt or MCP dispatch loop.
 * Message header:
 *
 *      +---------------+---------------+
 * hdr1 | slot          | MCTRL_PUMA1   | (packet type)
 *      +---------------+---------------+
 * hdr2 | source node   | count         |
 *      +---------------+---------------+
 *
 * Gets message from NIC fifo into buffer.
 * The user program will pluck it out of the buffer.
 */

mcmsg_recv_puma1(hdr1, hdr2)
	register unsigned long	hdr1;	/* Packet type */
	register unsigned long	hdr2;	/* Source and count */
{
	register mcmsg_task_t	*mt;
	register puma_hdr_t	*hdrp;
	register puma_slot_t	*ps;
	register unsigned long	slot;
	register unsigned long	count;
#if	BIGPKTS
#define	ecount  count
#else	
	register unsigned long	ecount;
#endif	BIGPKTS
	register unsigned long	strip;
	register unsigned long	*s;
	register unsigned long	buf;
	register unsigned long	offset;
	register unsigned long	bp1;
	register unsigned long	bp2;
	register unsigned long	src;

	mcmsg_trace_recv(hdr1, hdr2, 0, 0, 0, 0);

       /* count is how many bytes the sender says he is sending.  For
	  a message coming from Sunmos, this is the length of message body
	  padded to a multiple of 32 + sizeof(SUNMOS_MSG_HEAD) */
	count = hdr2 & 0xFFFF;

/***********************************************************
This check commented out.  We need to check for packet size when sending,
but if a guest O. S. can get away with sending us a bigger packet,
we shouldn't care.
	if (count > PUMA_MAX_PKT) {
		mcmsg_trace_drop("bad puma count", count);
		mcmsg_flush_invalid();
		return;
	}
*************************************************************/
#if !BIGPKTS
        /* We will read a multiple of 32 from the NIC. The message body
           is rounded up to 32, but since the Sunmos header is 32 bytes,
           it's OK to do the rounding here */
        ecount = (count + (LTU_ALIGN-1)) & ~(LTU_ALIGN-1);
#endif	!BIGPKTS

	slot = hdr1 >> 16;
	src = hdr2 >> 16;

	mcmsg_trace_debug("slot/type/count/src", 4,slot, hdr1 & 0xffff,
			count, src  ); 
	
	if (slot >= PUMA_SLOTS) {
		mcmsg_trace_drop("bad puma slot", slot);
		mcmsg_fifo_flush(ecount);
		return;
	}
	ps = &mcmsg_puma_slot[slot];
	if (ps->mt == 0) {
		mcmsg_trace_drop("free puma slot", slot);
		mcmsg_fifo_flush(ecount);
		return;
	}
	if (count < ps->strip) {
		mcmsg_trace_drop("puma count less than strip", ps->strip);
		mcmsg_fifo_flush(ecount);
		return;
	}
	mt = ps->mt;

	/* Point to the right message header for this message */
	hdrp = (puma_hdr_t *)mcmsg_validate_long(mt, ps->hdrp +
						 ps->hdri*ps->hdrsize);
	if (hdrp == 0) {
		mcmsg_trace_drop("bad puma hdr", ps->hdrp);
		return;
	}

	s = hdrp->strip;

	recv_hdr10(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7]);
	count -= ps->strip;
#if	!BIGPKTS
	ecount -= ps->strip;
#endif	!BIGPKTS

	/* Allocate space in the buffer for this message */
	offset = ps->offset;
	if (count > 0) {
		/* Get ecount bytes from the NIC. Set ps->offset to point
                   to the end of the actual data, not including pad.
                   ps->offset is where the next message after this one
                   will go.
		  This means the user will have to allocate a buffer at least
		  8 bytes bigger than the actual data to be received.
	        */
		if (offset + ecount > ps->bufsize) {
			buf = ps->bufp;
			ps->offset = (count + PUMA_BUFP_RND-1) &
					  ~(PUMA_BUFP_RND-1);
		} else {
			buf = ps->bufp + offset;
			ps->offset = offset + ((count + PUMA_BUFP_RND-1) &
						    ~(PUMA_BUFP_RND-1));
		}
		/* Point to start and end of message in buffer */
		bp1 = mcmsg_validate_write1(buf, ecount, mt->dirbase);
		bp2 = mcmsg_validate2();
		if (bp1 == 0) {
			mcmsg_trace_drop("puma1 recv invalid buffer", buf);
			mcmsg_fifo_flush(ecount);
			return;
		}
		/* Get the message from the NIC fifo (ecount bytes) */
		mcmsg_recv_buf(bp1, bp2, ecount);
	}

	/* Fill in message header for user's use */
mcmsg_trace_debug("puma fill header", 4, ps->hdri, hdrp, src, buf);
	hdrp->source = src;
	hdrp->buf = buf;
	/* The user may be spinning on hdrp->count, so when it changes,
	 * that tells him the message is complete.
	 */
	hdrp->count = count;	
	/* Go to the next header for the next message */
	ps->hdri++;
	if (ps->hdri == ps->nhdr) {
		ps->hdri = 0;
	}
}


#ifdef	DB_MACHINE_COMMANDS

/*
 * PUMA module debugging command
 *
 *    machine puma [ slot ] [, count ]
 */

void
mcmsg_db_puma(addr, have_addr, count, modif)
	/* optional arguments to "machine puma" */
	unsigned long	addr;      /* slot (not an address) */
	int		have_addr; /* whether or not there is an arg */
	unsigned	count;     /* number of slots to look at */
	char		*modif;    /* not used */
{
	int		first, last;
	int		slot;
	int		active;

	first = 0;
	last = PUMA_SLOTS-1;
	/* If user entered arguments, look only at those slots */
	if (have_addr) {
		if (count == 0xFFFFFFFF) {
			count = 1;
		}
		if (addr >= PUMA_SLOTS || addr + count > PUMA_SLOTS) {
			db_printf("Invalid slot number\n");
			return;
		}
		first = addr;
		last = addr + count - 1;
	}

	active = 0;
	for (slot = first; slot <= last; slot++) {
		if (mcmsg_puma_slot[slot].mt != 0) {
			active++;
		}
	}
	if (active == 0) {
		if (last == first) {
			db_printf("Slot not active\n");
		} else {
			db_printf("No active slots\n");
		}
		return;
	}

	/* Show information on all active slots */
	db_printf("Active slots\n");
	db_printf("Slot   Task      Mt      Hdrp   Nhdr Size Strip Index Bufp    Offset  Bufsize\n");
	for (slot = first; slot <= last; slot++) {
		puma_slot_t	*ps;

		ps = &mcmsg_puma_slot[slot];
		if (ps->mt != 0) {
			db_printf("%3d. %08X %08X %08X %4d %4d %4d %4d %08X %08X %08X\n",
				slot,
				ps->mt->task,
				ps->mt,
				ps->hdrp,
				ps->nhdr,
				ps->hdrsize,
				ps->strip,
				ps->hdri,
				ps->bufp,
				ps->offset,
				ps->bufsize);
			/* If a slot was specified, show buffered messages */
			if (have_addr) {
				int		i, j;
				puma_hdr_t	*ph;

				for (i = ps->hdri; ; ) {
					ph = (puma_hdr_t *)
						mcmsg_validate_long(
						 ps->mt,
						 ps->hdrp + i*ps->hdrsize);
					if (ph != 0 &&
					    ph->count != 0xFFFFFFFF) {
						db_printf("     msg %4d. count %4d  buf %08X from %4d\n",
							i,
							ph->count,
							ph->buf,
							ph->source);
						if (ps->strip != 0) {
							db_printf("     strip");
							for (j = 0;
							     j < ps->strip/4;
							     j++) {
								db_printf(" %08X",
								  ph->strip[j]);
							}
							db_printf("\n");
						}
					
					}
					i++;
					if (i == ps->nhdr) {
						i = 0;
					}
					if (i == ps->hdri) {
						break;
					}
				}
			}
		}
	}

	/* If no slot was specified, show buffered sends */
	if (!have_addr &&
	    mcmsg_puma_send_request_in != mcmsg_puma_send_request_out) {
		db_printf("\nActive sends\n");
		db_printf("   Buf    Count Slot Route   Statusp    Databuf\n");
		for (slot = mcmsg_puma_send_request_out;
		     slot != mcmsg_puma_send_request_in; ) {
			db_printf(" %08X %4d %4d %08X %08X %08X\n",
				mcmsg_puma_send_request[slot].hdr_buf,
				mcmsg_puma_send_request[slot].count,
				mcmsg_puma_send_request[slot].slot,
				mcmsg_puma_send_request[slot].route,
				mcmsg_puma_send_request[slot].statusp,
				mcmsg_puma_send_request[slot].data_buf);
			slot++;
			if (slot == PUMA_NSEND) {
				slot = 0;
			}
		}
	}
}
#endif	DB_MACHINE_COMMANDS
