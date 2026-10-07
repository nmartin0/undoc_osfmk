/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
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
 *	Copyright 1991  Intel Corporation.
 */

/*
 * rktest.c
 *
 * RK interface library standalone test
 */

#include <stdio.h>
#include <mcmsg/mcmsg_appl.h>
#include <mcmsg/mcmsg_xmsg.h>
#include <nx/assert.h>
#define DEBUGXMSG_INSTANTIATE 1
#include <nx/debugxmsg.h>

#if POSTMORTEM

#define pmassert(x) if (!(x)) { fprintf(stdout, "error (%s)\n", "x"); break; }

postmortem()
{
	xmsg_t	*xm;
	xmsg_t	*xp;
	xmsg_t	*xk;
	unsigned long sz;
	extern unsigned long mcmsg_free_bytes;
	extern xmsg_t *mcmsg_free;
	extern xmsg_t *mcmsg_outgoing_head;
	extern xmsg_t *mcmsg_outgoing_tail;

	fprintf(stdout, "debugxmsg count %u\n", debugxmsg_count);
	debugxmsg_active = 0;
	fprintf(stdout, "mcmsg buffer   %08X - %08X\n",
		mcmsg_buffer, mcmsg_buffer_end);
	fprintf(stdout, "mcmsg_free     %08X  bytes %12u\n",
		mcmsg_free, mcmsg_free_bytes);
	fprintf(stdout, "\n");
	if (mcmsg_free != 0) {
		xm = mcmsg_free;
		for (;;) {
			debugxmsg(xm, "free");

			pmassert(xm >= mcmsg_buffer);
			pmassert(xm < mcmsg_buffer_end);
			pmassert(xm->state == XMSG_FREE);

			pmassert(xm->msg_type != 2);
			pmassert(xm->msg_type == 0);
			xm->msg_type = 1;
			for (xk = xm+1, sz = xm->size;
			     sz > 0;
			     (xk++)->msg_type = 2, sz -= sizeof(xmsg_t));

			xm = xm->link;
			if (xm == mcmsg_free)
				break;
		}
	} else {
		debugxmsg(mcmsg_free, "free");
	}
	fprintf(stdout, "\n");
	if (mcmsg_outgoing_head != 0) {
		xm = mcmsg_outgoing_head;
		for (;;) {
			if (mcmsg_outgoing_head == xm) {
				debugxmsg(xm, "outgoing head");
			} else if (mcmsg_outgoing_tail == xm) {
				debugxmsg(xm, "outgoing tail");
			} else {
				debugxmsg(xm, "outgoing");
			}

			pmassert(xm >= mcmsg_buffer);
			pmassert(xm < mcmsg_buffer_end);
			pmassert(xm->state == XMSG_FULL ||
				 xm->state == XMSG_TRASH);

			pmassert(xm->msg_type != 2);
			pmassert(xm->msg_type == 0);
			xm->msg_type = 1;
			for (xk = xm+1, sz = xm->size;
			     sz > 0;
			     (xk++)->msg_type = 2, sz -= sizeof(xmsg_t));

			xm = xm->link;
			if (xm == 0)
				break;
		}
	} else {
		debugxmsg(mcmsg_outgoing_head, "outgoing head");
		debugxmsg(mcmsg_outgoing_tail, "outgoing tail");
	}
	fprintf(stdout, "\n");
	for (xm = mcmsg_buffer;
	     xm < mcmsg_buffer_end;
	     xm = (xmsg_t *)((unsigned long)(xm + 1) + xm->size)) {
		debugxmsg(xm, "scan");
	}
}

#endif POSTMORTEM
