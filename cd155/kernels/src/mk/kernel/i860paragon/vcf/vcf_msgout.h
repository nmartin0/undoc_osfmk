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
 
#ifndef _MSGOUT_H_
#define _MSGOUT_H_

#include <i860paragon/vcf/vcf_pmachine.h>

/* Max bytes per RDA.  MUST BE KEPT SMALLER THAN MEMORY PAGESIZE. */
#define  PACKETSIZE     1024  /* This constant is also found in vcf_asm.h. */
#define  PACKETMASK(x)  ((void *)((int)x & ~(PACKETSIZE - 1)))
#define  PACKET_BOUNDARY_CROSSED(buf, len) (PACKETMASK(buf) != PACKETMASK(buf + len - 1))
#define  BYTES_TO_PACKET_BOUNDARY(buf) (PACKETSIZE - ((PACKETSIZE - 1) & (int) buf))

extern vcf_xmitinfo  vcf_rejection_out;

extern void (*out_ltu_active)(void);
extern void  vcf_transmit_rda(vcf_xmitinfo *xi);
extern void  vcf_transmit_rd(vcf_xmitinfo *xi);
extern void  vcf_transmit_rejection(vcf_xmitinfo *xi);
extern void  vcf_transmit_roc(vcf_xmitinfo *xi);
extern void  vcf_transmit_rcc(vcf_xmitinfo *xi);

#endif
