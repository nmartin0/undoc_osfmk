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
 
#ifndef  _VCF_ASM_H_
#define  _VCF_ASM_H_

#include <i860paragon/vcf/vcf_dispatch.h>

#ifndef  VCF_DEBUG_PRINTOUTS
#define  VCF_DEBUG_PRINTOUTS  0
#endif

#ifndef  VCF_SUPERLOCK
#define  VCF_SUPERLOCK  1
#endif

/* #define  BUMPERS */
#ifndef  BURST
#if  BUMPERS
#define  BURST  1
#else
#define  BURST  0
#endif
#endif  /* BURST */

#define  NIC_BASE                0xFFFF0000
#define  NIC_SET_OFFSET          0x0000
#define  NIC_CLEAR_OFFSET        0x0008
#define  NIC_STATUS_OFFSET       0x0018
#define  NIC_DATAIN_OFFSET       0x0020
#define  NIC_DATAOUT_OFFSET      0x0020
#define  NIC_RESET_TEST_OFFSET   0x0038
#define  NIC_EOD_DATAOUT_OFFSET  0x1020
#define  BURST_ME                0x0010  /* Special code to do a burst send */

#define  NIC_STAT_RX_FIFO_NOT_EMPTY  0x00000200
#define  NIC_STAT_RX_LOW             0x00000200
#define  NIC_STAT_EOD_IN_NIC         0x00000400
#define  NIC_STAT_EOD_IN_WORD        0x00002000

#define  NIC_CNTRL_EN_EOD_IN_NIC  0x0400

#define  DP_STATUS_LO            0x70000210
#define  DP_STATUS_HI            (DP_STATUS_LO + 4)
#define  DP_ISTAT_LTU0_CNT       0x4
#define  DP_ISTAT_LTU1_CNT       0x8
#define  DP_LTU0_CLEAR_CNT       0x60400000
#define  DP_LTU1_CLEAR_CNT       0x60500000

#define  PAGESIZE   (4*1024)
#define  PAGELEN    PAGESIZE
#define  PACKETLEN  1024  /* Also found in vcf_msgout.h */
#define  LTULEN     32

#define  VCF_PACKET_BASE  110  /* MAY CHANGE! */
#define  VCF_HWORD_RD   (VCF_PACKET_BASE+0)
#define  VCF_HWORD_RDA  (VCF_PACKET_BASE+1)
#define  VCF_HWORD_RDAC (VCF_PACKET_BASE+2)

#define  VCF_CS_GOT_RMT_CLOSE  0x001
#define  VCF_CS_SENT_CLOSE     0x002
#define  VCF_CS_GOT_USR_CLOSE  0x004

#define  VCF_CS_GOT_RMT_OPEN   0x008
#define  VCF_CS_GOT_USR_OPEN   0x040

#define  VCF_CS_REJECTED       0x080
#define  VCF_CS_READY          0x100
#define  VCF_CS_CHANDEAD       0x200
#define  VCF_CS_MAYBE          0x400

#define  VCF_EINVAL     -1  /* Invalid argument. */
#define  VCF_ENOBUF     -2  /* No buffers available. */
#define  VCF_ECHANDEAD  -3  /* Channel dead. */
#define  VCF_ENOCTE     -4  /* No CTEs left. */
#define  VCF_ENOMEM     -5  /* No memory available. */
#define  VCF_EMEMFAULT  -6  /* Catastrophic memory fault. */

#define  ENTER_ASM  \
	addu	-64,sp,sp;  \
	st.l	 r1, 0(sp); \
	st.l	 r3, 4(sp); \
	st.l	 r4, 8(sp); \
	st.l	 r5,12(sp); \
	st.l	 r6,16(sp); \
	st.l	 r7,20(sp); \
	st.l	 r8,24(sp); \
	st.l	 r9,28(sp); \
	st.l	r10,32(sp); \
	st.l	r11,36(sp); \
	st.l	r12,40(sp); \
	st.l	r13,44(sp); \
	st.l	r14,48(sp); \
	st.l	r15,52(sp)
#define  EXIT_ASM  \
	ld.l	 0(sp), r1; \
	ld.l	 4(sp), r3; \
	ld.l	 8(sp), r4; \
	ld.l	12(sp), r5; \
	ld.l	16(sp), r6; \
	ld.l	20(sp), r7; \
	ld.l	24(sp), r8; \
	ld.l	28(sp), r9; \
	ld.l	32(sp),r10; \
	ld.l	36(sp),r11; \
	ld.l	40(sp),r12; \
	ld.l	44(sp),r13; \
	ld.l	48(sp),r14; \
	ld.l	52(sp),r15; \
	addu	64,sp,sp

#define  SAVESTATE			\
	addu	-96,sp,sp;		\
	st.l	r1,0(sp);		\
	st.l	r16,4(sp);		\
	st.l	r17,8(sp);		\
	st.l	r18,12(sp);		\
	st.l	r19,16(sp);		\
	st.l	r20,20(sp);		\
	st.l	r21,24(sp);		\
	st.l	r22,28(sp);		\
	st.l	r23,32(sp);		\
	st.l	r24,36(sp);		\
	st.l	r25,40(sp);		\
	st.l	r26,44(sp);		\
	st.l	r27,48(sp);		\
	st.l	r28,52(sp);		\
	st.l	r29,56(sp);		\
	st.l	r30,60(sp);		\
	st.l	r31,64(sp);		\
	fst.d	f8,72(sp);		\
	fst.d	f10,80(sp);   \
	fst.d	f12,88(sp)
#define  RESTORESTATE			\
	ld.l	0(sp),r1;		\
	ld.l	4(sp),r16;		\
	ld.l	8(sp),r17;		\
	ld.l	12(sp),r18;		\
	ld.l	16(sp),r19;		\
	ld.l	20(sp),r20;		\
	ld.l	24(sp),r21;		\
	ld.l	28(sp),r22;		\
	ld.l	32(sp),r23;		\
	ld.l	36(sp),r24;		\
	ld.l	40(sp),r25;		\
	ld.l	44(sp),r26;		\
	ld.l	48(sp),r27;		\
	ld.l	52(sp),r28;		\
	ld.l	56(sp),r29;		\
	ld.l	60(sp),r30;		\
	ld.l	64(sp),r31;		\
	fld.d	72(sp),f8;		\
	fld.d	80(sp),f10;		\
	fld.d	88(sp),f12;		\
	addu	96,sp,sp

#define  C_CALL1(func, arg1)		\
	SAVESTATE;			\
	call	func;			\
	addu	arg1,r0,r16;		\
	RESTORESTATE
#define  PFLOAT(arg1, arg2)		\
	SAVESTATE;			\
	fxfr	arg1,r16;		\
	call	_vcf_bite_me;		\
	fxfr	arg2,r17;		\
	RESTORESTATE
#define  PINT(arg1, arg2)  \
  SAVESTATE;  \
  mov  arg1,r16;  \
  mov  arg2,r17;  \
  call  _vcf_bite_me;  \
  nop; \
  RESTORESTATE
#define  PRINTF(str)			\
	SAVESTATE;			\
	orh	h%str,r0,r31;		\
	call	_printf;		\
	or	l%str,r31,r16;		\
	RESTORESTATE

#define  VCF_CHAN_STATUS     0
#define  VCF_CHAN_PEER       4
#define  VCF_CHAN_SEQ_NO     8
#define  VCF_CHAN_RD_LEN    12
#define  VCF_CHAN_PEER_CTE  16
#define  VCF_CHAN_SEND_POOL 20
#define  VCF_CHAN_RECV_POOL 24
#define  VCF_CHAN_SENDH     28
#define  VCF_CHAN_SENDT     32
#define  VCF_CHAN_RECVH     36
#define  VCF_CHAN_RECVT     40
#define  VCF_CHAN_RDA_IN_PBUF  44
#define  VCF_CHAN_RDA_OUT_PBUF 48
#define  VCF_CHAN_SEQ_RECVD    52
#define  VCF_CHAN_SRDA_NEXT    56
#define  VCF_CHAN_SRD_NEXT     64
#define  VCF_CHAN_SOPEN_NEXT   72
#define  VCF_CHAN_SCLOSE_NEXT  80
#define  VCF_CHAN_OPEN_SBUFP   88
#define  VCF_CHAN_CLOSE_SBUFP  92
#define  VCF_CHAN_ID           96
#define  VCF_CHAN_HASHNEXT    100
#define  VCF_CHAN_HASHPREV    104
#define  VCF_CHAN_HASHBUCKET  108
#define  VCF_CHAN_DUMMY       112


#define  VCF_REQ_NEXT    0
#define  VCF_REQ_LEN     4
#define  VCF_REQ_SBUFP   8
#define  VCF_REQ_BUF    12

//	.globl	_vcf_groupid
	.globl	_vcf_invert_path
	.globl	_vcf_got_rq_badgang

#endif
