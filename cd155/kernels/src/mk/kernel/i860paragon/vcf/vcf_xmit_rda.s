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

	.data
	.align 4
xferinfo:
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	
	.text
	.globl	_vcf_transmit_rda
	.align	8
// in: r16 = xi
_vcf_transmit_rda:
#if  VCF_DEBUG_PRINTOUTS
	STVAL(r16,r31,vcf_temp)
	FOO(4,0)
	FOO(0x1004,0)
	FOO(0x2004,0)
	FOO(0x3004,0)
	LDVAL(vcf_temp,r16)
#endif	
	andnot	0x7f,r16,r16	// r16 = ch
	fld.l	4(r16),f8	// f8 = destnode
	ld.l	28(r16),r19	// r19 = msg
	fld.l	16(r16),f11	// f11 = destch
	ld.l	4(r19),r21	// r21 = len
	ld.l	12(r19),r20	// r20 = vbuf

	bte	r0,r21,zero_len
	ld.l	48(r16),r22	// r22 = g_lastpbuf
	st.l	r0,48(r16)
	btne	0,r22,pbuf_gotten
// lastpbuf is null, so you must recalculate pbuf.
	shr	20,r20,r23
	and	0x0ffc,r23,r23	// r23 = Index into directory
	ld.l	r23(REG_DIRBASE),r18	// r18 = Page table address
	and	0x0005,r18,r23	// r23 = User and present bits
	btne	5,r23,bogus_buffer
	andnot	0x0fff,r18,r18
	shr	10,r20,r23
	and	0x0ffc,r23,r23	// r23 = Index into page table
	ld.l	r23(r18),r23	// r23 = Page address
	and	0x0fff,r20,r22	// r22 = Index into page
	and	0x0005,r23,r24	// r22 = User and present bits
	btne	5,r24,bogus_buffer
	andnot	0x0fff,r23,r23
	or	r23,r22,r22	// r22 = pbuf (the physical address)

// r22 = pbuf
pbuf_gotten:
	famov.ss f0,f9		// f9 = 0.  f8/f9 now ready for transmission.
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)

pbuf_valid_1:
#if  BLAT
	STVAL(r16,r31,vcf_temp)
	STVAL(r17,r31,vcf_temp+4)
	STVAL(r18,r31,vcf_temp+8)
	STVAL(r19,r31,vcf_temp+12)
	STVAL(r20,r31,vcf_temp+16)
	STVAL(r21,r31,vcf_temp+20)
	STVAL(r22,r31,vcf_temp+24)
	STVAL(r23,r31,vcf_temp+28)
	STVAL(r24,r31,vcf_temp+32)
	STVAL(r25,r31,vcf_temp+36)
	fxfr	f13,r18
	FOO(0x100500,r18)
	LDVAL(vcf_temp,r16)
	LDVAL(vcf_temp+4,r17)
	LDVAL(vcf_temp+8,r18)
	LDVAL(vcf_temp+12,r19)
	LDVAL(vcf_temp+16,r20)
	LDVAL(vcf_temp+20,r21)
	LDVAL(vcf_temp+24,r22)
	LDVAL(vcf_temp+28,r23)
	LDVAL(vcf_temp+32,r24)
	LDVAL(vcf_temp+36,r25)
#endif

	addu	r22,r21,r23	// r23 = pbuf + len
	addu	-1,r23,r23	// r23 = pbuf + len - 1
	andnot	(PACKETLEN-1),r22,r24	// r24 = Packet address of start
	andnot	(PACKETLEN-1),r23,r25	// r25 = Packet address of end
	btne	r24,r25,not_last_packet

last_packet:
	mov	VCF_HWORD_RDA,r23
	ixfr	r23,f10
	ixfr	r21,f12			// f12 = Length of this packet
	fst.d	f10,NIC_DATAOUT_OFFSET(REG_NIC)
	fld.l	12(r16),f13		// f13 = Length of all RDA[C]s combined
#if  BLAT
	STVAL(r16,r31,vcf_temp)
	STVAL(r17,r31,vcf_temp+4)
	STVAL(r18,r31,vcf_temp+8)
	STVAL(r19,r31,vcf_temp+12)
	STVAL(r20,r31,vcf_temp+16)
	STVAL(r21,r31,vcf_temp+20)
	STVAL(r22,r31,vcf_temp+24)
	STVAL(r23,r31,vcf_temp+28)
	STVAL(r24,r31,vcf_temp+32)
	STVAL(r25,r31,vcf_temp+36)
	fxfr	f13,r18
	FOO(0x100100,r18)
	LDVAL(vcf_temp,r16)
	LDVAL(vcf_temp+4,r17)
	LDVAL(vcf_temp+8,r18)
	LDVAL(vcf_temp+12,r19)
	LDVAL(vcf_temp+16,r20)
	LDVAL(vcf_temp+20,r21)
	LDVAL(vcf_temp+24,r22)
	LDVAL(vcf_temp+28,r23)
	LDVAL(vcf_temp+32,r24)
	LDVAL(vcf_temp+36,r25)
#endif
	and	LTULEN-1,r22,r0		// If (pbuf & LTU_MASK) == 0,
	fst.d	f12,NIC_DATAOUT_OFFSET(REG_NIC)
	bc	ltu_aligned_rda		//   Ready to use the ltu!
stream_to_ltu_rda:
	fld.d	0(r22),f14		// Fetch a doubleword...
	addu	8,r22,r22
	addu	-8,r21,r21		// len -= 8...
	fst.d	f14,NIC_DATAOUT_OFFSET(REG_NIC)	// And Store the doubleword!
	bte	r0,r21,send_end_rda
	and	LTULEN-1,r22,r0
	bnc	stream_to_ltu_rda
	
ltu_aligned_rda:
	subu	(LTULEN*2)-1,r21,r0	// LTU transfers are 2 line MINIMUM.
	bc.t	ltu_done_rda
	fld.d	0(r22),f14

// It's OK to use the LTU here.
	andnot	LTULEN-1,r21,r23
	addu	4,r22,r24		// Just trust me, this chunk of code
	andnoth	0xc000,r24,r24		//   sets up an outbound LTU starting
	shr	5,r21,r25		//   at address r22 with length
	addu	-1,r25,r25		//   r23.
// The "c" here says to snoop both caches...not necessary to snoop the MP
//   cache.  Something to fix later.
	orh	0xc000,r25,r25
	stio.l	r25,r24

	addu	r23,r22,r22		// r22(pbuf) += r23(len & ~LTU_MASK)
	and	LTULEN-1,r21,r21	// r21(len) &= LTU_MASK
	mov	finish_rda,REG_OUT_LTU_ACTIVE
	mov	xferinfo,r31
	st.l	r16, 0(r31)	// Save all the registers that will be needed
	st.l	r19, 4(r31)	//   when the LTU finishes.
	st.l	r21, 8(r31)
	bri	r1
	st.l	r22,12(r31)

finish_rda:
	mov	r0,REG_OUT_LTU_ACTIVE
	mov	xferinfo,r31
	ld.l	 0(r31),r16
	ld.l	 4(r31),r19
	ld.l	 8(r31),r21
	ld.l	12(r31),r22
	bte	r0,r21,send_end_rda
stream_after_ltu_rda:
	fld.d	0(r22),f14
ltu_done_rda:
	addu	8,r22,r22
	addu	-8,r21,r21
	fst.d	f14,NIC_DATAOUT_OFFSET(REG_NIC)
	btne	r0,r21,stream_after_ltu_rda

send_end_rda:
#if  !BUMPERS
	fst.d	f0,NIC_EOD_DATAOUT_OFFSET(REG_NIC)
#else
	fst.d	f0,NIC_DATAOUT_OFFSET(REG_NIC)
	mov	bump,r31
	fld.d	 0(r31),f8
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_EOD_DATAOUT_OFFSET(REG_NIC)
#endif
#if  BURST
	mov	BURST_ME,r31
	famov.ss f0,f9
	ixfr	r31,f8
	fst.d	f8,NIC_RESET_TEST_OFFSET(REG_NIC)
#endif
done_with_rda:
	ld.l	12(r16),r18	// r18 = ch->rd_len
done_with_rda_2:
	ld.l	8(r19),r22	// r22 = msg->sbufp
	addu	1,r0,r29	// r29 = 1 = No RD arrived
	ld.l	0(r19),r25	// r25 = msg->next
	ld.l	20(r16),r23	// r23 = ch->send_pool
	ld.l	0(r16),r24	// r24 = ch->status
#if  SUPERLOCK
	lock
#endif
	st.l	r18,0(r22)	// *(msg->sbufp) = ch->rd_len
#if  SUPERLOCK
	unlock
#endif
	st.l	r29,12(r16)	// ch->rd_len = 1
// Here we dq the message buffer.
	st.l	r25,28(r16)	// ch->sendh = msg->next
	st.l	r23,0(r19)	// msg->next = ch->send_pool

	and	VCF_CS_GOT_USR_CLOSE,r24,r0
	bnc	ch_may_close
ch_not_closed:
	bri	r1
	st.l	r19,20(r16)	// ch->send_pool = msg
ch_may_close:
	btne	r0,r25,ch_not_closed	// if ch->sendh != 0, don't close yet.
	ld.l	36(r16),r23		// r23 = ch->recvh
	btne	r0,r23,ch_not_closed	// if ch->recvh != 0, don't close yet.
	st.l	r19,20(r16)	// ch->send_pool = msg
	st.l	r0,80(r16)
	btne	r0,REG_XMIT_HEAD,not_first_ctrl
	addu	80,r16,REG_XMIT_HEAD	// xmit_head = &(ch->send_close)
	bri	r1
	mov	REG_XMIT_HEAD,REG_XMIT_TAIL
not_first_ctrl:
	mov	REG_XMIT_TAIL,r27
	addu	80,r16,REG_XMIT_TAIL
	st.l	REG_XMIT_TAIL,0(r27)

bogus_buffer:
	mov	4,r30
	STVAL(r30,r31,_vcf_infoblock+8)
	famov.ss f0,f9
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	mov	VCF_HWORD_RDA,r23
	ixfr	r23,f10
	fst.d	f10,NIC_DATAOUT_OFFSET(REG_NIC)
	mov	8,r21
	ixfr	r21,f12			// f12 = Length of this packet
	ixfr	r21,f13			// f13 = Length of all RDA[C]s combined
	fst.d	f12,NIC_DATAOUT_OFFSET(REG_NIC)
	br	send_end_rda
	fst.d	f0,NIC_DATAOUT_OFFSET(REG_NIC)

not_last_packet:
// You must cross packet boundaries, so send an RDAC.
	addu	PACKETLEN,r22,r23	// r23 = pbuf + PACKETLEN
	andnot	PACKETLEN-1,r23,r23	// r23 = End addr of this RDAC
	subu	r23,r22,r23		// r23 = packetlen
	subu	r21,r23,r21		// r21 = (len -= packetlen)
	mov	VCF_HWORD_RDAC,r24
	ixfr	r24,f10
	fst.d	f10,NIC_DATAOUT_OFFSET(REG_NIC)	// RDAC, destch
	ixfr	r23,f12
	fst.d	f12,NIC_DATAOUT_OFFSET(REG_NIC)	// packetlen, unused
	addu	r20,r23,r20		// r20 = (vbuf += packetlen)

//	bte	0,r23,send_end_rda	// If len = 0, send the end message.
	and	LTULEN-1,r22,r0		// If (pbuf & LTU_MASK) == 0,
	bc	ltu_aligned_rdac	//   Ready to use the ltu!
stream_to_ltu_rdac:
	fld.d	0(r22),f14		// Fetch a doubleword...
	addu	8,r22,r22
	addu	-8,r23,r23		// packetlen -= 8...
	fst.d	f14,NIC_DATAOUT_OFFSET(REG_NIC)	// And Store the doubleword!
	bte	r0,r23,send_end_rdac_req
	and	LTULEN-1,r23,r0
	bnc	stream_to_ltu_rdac

ltu_aligned_rdac:
	subu	LTULEN,r23,r0		// If you only have one cache line to
	bc	stream_to_ltu_rdac	//   move, then stream it.
// Time to use the LTU!
	addu	4,r22,r24		// Just trust me, this chunk of code
	andnoth	0xc000,r24,r24		//   sets up an outbound LTU starting
	shr	5,r23,r25		//   at address r22 with length
	addu	-1,r25,r25		//   r23.
// The "c" here says to snoop both caches...not necessary to snoop the MP
//   cache.  Something to fix later.
	orh	0xc000,r25,r25
	stio.l	r25,r24

	and	PAGESIZE-1,r20,r0	// Clear CC if vbuf & (PAGESIZE-1)
	bc	recalc_pbuf
// The next RDAC will be on the same memory page.
	addu	r22,r23,r22		// r22 = (pbuf += packetlen)
	st.l	r22,48(r16)		// g_lastpbuf = pbuf
	br	setup_cont_rdac
	nop
// Might as well recalculate the new pbuf now, while the LTU is busy anyway.
recalc_pbuf:
	shr	20,r20,r23
	and	0x0ffc,r23,r23	// r23 = Index into directory
	ld.l	r23(REG_DIRBASE),r18	// r18 = Page table address
	and	0x0005,r18,r23	// r23 = User and present bits
	btne	5,r23,bogus_buffer
	andnot	0x0fff,r18,r18
	shr	10,r20,r23
	and	0x0ffc,r23,r23	// r23 = Index inte page table
	ld.l	r23(r18),r23	// r23 = Page address
	and	0x0005,r23,r22	// r22 = User and present bits
	btne	5,r22,bogus_buffer
	andnot	0x0fff,r23,r22	// r22 = pbuf (the physical address)
	st.l	r22,48(r16)
setup_cont_rdac:
	mov	finish_rdac,REG_OUT_LTU_ACTIVE
	st.l	r20,12(r19)	// (r19=msg)->buf = pbuf
	st.l	r21,4(r19)	// (r19=msg)->len = len
// Re-queue the channel on the xmit work queue
	st.l	r0,56(r16)	// ch->send_rda.next = NULL
	btne	r0,REG_XMIT_HEAD,nonempty_q
	addu	56,r16,REG_XMIT_HEAD	// xmit_head = &(ch->send_rda)
	bri	r1
	mov	REG_XMIT_HEAD,REG_XMIT_TAIL
nonempty_q:
	mov	REG_XMIT_TAIL,r25
	addu	56,r16,REG_XMIT_TAIL
	bri	r1
	st.l	REG_XMIT_TAIL,0(r25)

send_end_rdac_req:
// You have to requeue the channel on the xmit queue.  Then send the end of
//   the rdac.
	st.l	r0,56(r16)	// ch->send_rda.next = NULL
	btne	r0,REG_XMIT_HEAD,nonempty_q2
	addu	56,r16,REG_XMIT_HEAD
	br	send_end_rdac
	mov	REG_XMIT_HEAD,REG_XMIT_TAIL
nonempty_q2:
	mov	REG_XMIT_TAIL,r25
	addu	56,r16,REG_XMIT_TAIL
	st.l	REG_XMIT_TAIL,0(r25)

send_end_rdac:
#if  !BUMPERS
	fst.d	f0,NIC_EOD_DATAOUT_OFFSET(REG_NIC)
#else
	fst.d	f0,NIC_DATAOUT_OFFSET(REG_NIC)
	mov	bump,r31
	fld.d	 0(r31),f8
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_EOD_DATAOUT_OFFSET(REG_NIC)
#endif
#if  BURST
	mov	BURST_ME,r31
	famov.ss f0,f9
	ixfr	r31,f8
	fst.d	f8,NIC_RESET_TEST_OFFSET(REG_NIC)
#endif
	and	PAGESIZE-1,r20,r0
	bc	need_recalc_rdac
	st.l	r22,48(r16)
need_recalc_rdac:
	st.l	r20,12(r19)	// (r19=msg)->buf = pbuf
	bri	r1
	st.l	r21,4(r19)	// (r19=msg)->len = len

// This is called when the LTU is done for the RDAC.
finish_rdac:
	mov	r0,REG_OUT_LTU_ACTIVE
#if  !BUMPERS
#if  !BURST
	bri	r1
#endif
	fst.d	f0,NIC_EOD_DATAOUT_OFFSET(REG_NIC)
#else
	fst.d	f0,NIC_DATAOUT_OFFSET(REG_NIC)
	mov	bump,r31
	fld.d	 0(r31),f8
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
#if  !BURST
	bri	r1
#endif
	fst.d	f8,NIC_EOD_DATAOUT_OFFSET(REG_NIC)
#endif
#if  BURST
	mov	BURST_ME,r31
	famov.ss f0,f9
	ixfr	r31,f8
	bri	r1
	fst.d	f8,NIC_RESET_TEST_OFFSET(REG_NIC)
#endif

// The message is zero length.  Special, very fast case.
zero_len:
	mov	VCF_HWORD_RDA,r23
	famov.ss f0,f9
	ixfr	r23,f10
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	mov	1,r18
	fst.d	f10,NIC_DATAOUT_OFFSET(REG_NIC)
#if !BUMPERS
#if !BURST
	br	done_with_rda_2
#endif
	fst.d	f0,NIC_EOD_DATAOUT_OFFSET(REG_NIC)	// Length = 0
#else
	fst.d	f0,NIC_DATAOUT_OFFSET(REG_NIC)
	mov	bump,r31
	fld.d	0(r31),f8
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
	fst.d	f8,NIC_DATAOUT_OFFSET(REG_NIC)
#if !BURST
	br	done_with_rda_2
#endif
	fst.d	f8,NIC_EOD_DATAOUT_OFFSET(REG_NIC)
#endif
#if BURST	
	mov	BURST_ME,r31
	famov.ss f0,f9
	ixfr	r31,f8
	br	done_with_rda_2
	fst.d	f8,NIC_RESET_TEST_OFFSET(REG_NIC)
#endif

#if  BUMPERS
	.data
	.align	8
bump:	.long	0x4,0xbabefe9d
	.long	0x4,0xbabefe9d
	.long	0x4,0xbabefe9d
	.long	0x4,0xbabefe9d
#endif
