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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/debugxmsg.h,v 1.6 1994/11/19 02:44:39 mtm Exp $
 *
 */


#if ASSERT
#define TIMEOUT (1024 * 1024)
#define DEBUGXMSG 1
#endif ASSERT

#if DEBUGXMSG

extern int debugxmsg_active;
extern unsigned int debugxmsg_count;

#if DEBUGXMSG_INSTANTIATE

int debugxmsg_active = -1;
unsigned int debugxmsg_count = 0;

#define NSTATES (sizeof(states)/sizeof(states[0]))
static
char *states[] = {
	"XMSG_FREE ",
	"XMSG_EMPTY",
	"XMSG_FULL ",
	"XMSG_TRASH",
	"XMSG_STOP",
	"XMSG_CONT",
};

debugxmsg(xm, s)
	xmsg_t	*xm;
	char	*s;
{
	extern xmsg_t *mcmsg_buffer;
	extern xmsg_t *mcmsg_buffer_end;

	debugxmsg_count++;
	if (debugxmsg_active == -1 || debugxmsg_active > debugxmsg_count)
		return;
	if (xm < mcmsg_buffer || xm >= mcmsg_buffer_end) {
		fprintf(stderr, "%-20s %08X\n", s, xm);
	} else {
		fprintf(stderr, "%-20s %08X %s size %08X f %08X b %08X\n",
			s, xm,
			xm->state < NSTATES? states[xm->state] : "????????? ",
			xm->size, xm->link, xm->length);
	}
}
#endif DEBUGXMSG_INSTANTIATE

#else DEBUGXMSG
#define debugxmsg(x,y)
#endif DEBUGXMSG

