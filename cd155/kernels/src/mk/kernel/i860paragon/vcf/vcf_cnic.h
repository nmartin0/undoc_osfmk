/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#ifndef _VCF_CNIC_H_

#define _VCF_CNIC_H_

#include <i860paragon/mcmsg/mcmsg_hw.h>

#define VCF_INLINE
#ifdef VCF_INLINE

#define NIC_RECV_END_MSG() {int x, y; recv2(x, y); }
#define NIC_RECV_WORDS(addr1, addr2) recv2(*(addr1), *(addr2));
#define NIC_SEND_WORDS(val1, val2) send2((val1), (val2));
#define NIC_SEND_START_MSG(dest) send2((dest), 0);
#define NIC_SEND_END_MSG() { nic_reg burst; send2eod(0xdeadbeef, 0xbeefdead); \
			     burst.halfs.hi = 0; \
			     burst.halfs.lo = NIC_TEST_RCV_AFULL_SEL; \
			     NIC.reset_test.full = burst.full; }

#else

extern void NIC_RECV_END_MSG(void);
extern void NIC_RECV_WORDS(int *val1, int *val2);
extern void NIC_SEND_WORDS(int val1, int val2);
extern void NIC_SEND_START_MSG(int dest);
extern void NIC_SEND_END_MSG(void);

#endif

extern int NIC_OUTBUF_FULL(void);
extern int IN_LTU_DONE(void);
extern int OUT_LTU_DONE(void);
extern void LTU_SEND_BLOCK(int *buf, int packetlen);
extern void LTU_RECV_BLOCK(int *buf, int packetlen);

#endif
