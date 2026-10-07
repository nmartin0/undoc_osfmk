// 
// $Copyright
// Copyright 1993, 1994, 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
#include <i860paragon/vcf/vcf_asm.h>

	.text
	
// RD format: RD-receiver channel-gang-path-seqno-length
	.globl	_vcf_transmit_rd
	.align	8
// In: R16 = xi
_vcf_transmit_rd:
#if  VCF_DEBUG
	STVAL(r16,r31,vcf_temp)
	FOO(1,0)
	LDVAL(vcf_temp,r16)
#endif
	andnot	0x7f,r16,r16		// r16 = ch
	fld.l	VCF_CHAN_PEER(r16),f8	// f8 = ch->peer
	addu	VCF_HWORD_RD,r0,r17	// r17 = VCF_HWORD_RD
	famov.ss f0,f9			// f9 = 0
	ixfr	r17,f10			// f10 = VCF_HWORD_RD
	fld.l	VCF_CHAN_PEER_CTE(r16),f11	// f11 = ch->peer_cte
//	ixfr	REG_GROUP,f12		// f12 = vcf_groupid
	famov.ss f8,f13			// f13 = ch->peer
	orh	ha%_vcf_sequence_number,r0,r17	// r17 = sequence address
	ld.l	l%_vcf_sequence_number(r17),r18	// r18 = sequence
	addu	1,r18,r18		// r18 = sequence + 1
	ixfr	r18,f14			// f14 = vcf_sequence_number + 1
	st.l	r18,VCF_CHAN_SEQ_NO(r16) // ch->seq_no = sequence_number
	st.l	r18,l%_vcf_sequence_number(r17)
	ld.l	VCF_CHAN_RECVH(r16),r20	// r20 = ch->recvh = msg
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fld.l	VCF_REQ_LEN(r20),f15	// f15 = ch->recvh->len
	fst.d	f10,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f12,NIC_DATAOUT_OFFSET(REG_NIC)
#if  !BUMPERS
#if  !BURST
	bri	r1
#endif  /* No burst. */
	fst.d	f14,NIC_EOD_DATAOUT_OFFSET(REG_NIC)
#else
// Send out the bumper.
	fst.d	f14,NIC_DATAOUT_OFFSET(REG_NIC)
	mov	bump,r16
	fld.d	 0(r16),f8
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
#if  !BURST
	bri	r1
#endif  /* No burst. */
	fst.d	f8,NIC_EOD_DATAOUT_OFFSET(REG_NIC)
#endif  /* No bumpers. */

#if  BURST
	mov	BURST_ME,r16
	famov.ss f0,f9
	ixfr	r16,f8
	bri	r1
	fst.d	f8,NIC_RESET_TEST_OFFSET(REG_NIC)
#endif

_vcf_atransmit_rejection::
	STORE_TEMP_REGS
	mov	r1,r15
	call	_vcf_transmit_rejection
	nop
	mov	r15,r1
	LOAD_TEMP_REGS
	bri	r1
	nop

_vcf_atransmit_roc::
	STORE_TEMP_REGS
	mov	r1,r15
	call	_vcf_transmit_roc
	nop
	mov	r15,r1
	LOAD_TEMP_REGS
	bri	r1
	nop

_vcf_atransmit_rcc::
	STORE_TEMP_REGS
	mov	r1,r15
	call	_vcf_transmit_rcc
	nop
	mov	r15,r1
	LOAD_TEMP_REGS
	bri	r1
	nop

	
#if  BUMPERS
	.data
	.align	8
bump:	.long	0x4,0xbabefe9d
	.long	0x4,0xbabefe9d
	.long	0x4,0xbabefe9d
	.long	0x4,0xbabefe9d
#endif
