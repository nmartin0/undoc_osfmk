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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_recv.c,v 1.26 1994/11/18 20:41:53 mtm Exp $
 */

/*
 * mcmsg_recv.c
 *
 * Receive interrupt handler for multicomputer message passing
 */

#if	iPSC860

#include <kern/assert.h>
#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>

extern mcmsg_console_need_int;

dcm_recv_intr()
{
	register unsigned long	hdr1;		/* First word of header */
	register unsigned long	hdr2;		/* Second word of header */
	register unsigned long	junk1;		/* For junk words in header */
	register unsigned long	junk2;		/* For junk words in header */
	register int		x;
	register long		t;

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	assert((t = MAXLOOP) != 0);
	while (mcmsg_recv_ready()) {
		if (ipcreq_receiving != 0) {
			ipcreq_recv_intr(0, ipcreq_receiving);
		} else {
			recv2(junk1, junk2);
			hdr1 = junk1 + junk2;
			recv2(hdr1, hdr2);
			if ((hdr1 & 0xFFFF) == MCTRL_IPC) {
				ipcreq_recv_intr(hdr1, hdr2);
			} else {
				switch (hdr1 & 0xFFFF) {

				case MCTRL_IPC:
					ipcreq_recv_intr(hdr1, hdr2);
					break;

				case MCTRL_RKA:
					mcmsg_recv_rka(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_RKS:
					mcmsg_recv_rks(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_RKR:
					mcmsg_recv_rkr(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_RK1:
					mcmsg_recv_rk1(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_RKN:
					mcmsg_recv_rkn(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_NXS:
					mcmsg_recv_nxs(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_NXM:
					mcmsg_recv_nxm(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_NXQ:
					mcmsg_recv_nxq(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_NXC:
					mcmsg_recv_nxc(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_NX1:
					mcmsg_recv_nx1(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_NXF:
					mcmsg_recv_nxf(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_NXN:
					mcmsg_recv_nxn(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_CONW:
					mcmsg_recv_conw(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_CONT:
					mcmsg_recv_cont(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_CONR:
					mcmsg_recv_conr(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_PNQ:
				case MCTRL_PNQL:
					mcmsg_recv_pnq(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_PNA:
					mcmsg_recv_pna(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_PAK:
				case MCTRL_PAKL:
					mcmsg_recv_pak(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_PRM:
					mcmsg_recv_prm(hdr1, hdr2);
					assert_mcmsg_eod_last();
					break;

				case MCTRL_BT0:
				case MCTRL_BT1:
				case MCTRL_BT2:
				case MCTRL_BT3:
					mcmsg_recv_boot(hdr1, hdr2);
					/* assert_mcmsg_eod_last(); */
					break;


				default:
					mcmsg_trace_drop("invalid message",
							 hdr1 & 0xFFFF);
					mcmsg_trace_debug("invalid msg data", 2,
							hdr1, hdr2, 0, 0);
#if DEBUG
printf("invalid message %08X %08X, sr %08X\n", hdr1, hdr2, STATUS_REG);
#endif DEBUG
					while (mcmsg_recv_ready()) {
						recv2(hdr1, hdr2);
					}
				}
			}
		}
		assert(t-- != 0);
	}
	if (mcmsg_flush) {
		flush();
		mcmsg_flush = 0;
	}

	if (mcmsg_console_need_int) {
		mcmsg_console_ints();
	}

	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
}

#endif	iPSC860
