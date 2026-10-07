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
	.globl	vcf_xferinfo
	.align 4
vcf_xferinfo:
	.long	0
	.long	0
g_pbuf:	.long	0	//  8 offset
g_len:	.long	0	// 12 offset
g_msg:	.long	0	// 16 offset
g_ch:	.long	0	// 20 offset
g_flen:	.long	0	// 24 offset

	.text
	.globl	_vcf_handle_rda
	.globl	_vcf_ahandle_rda
	.align	8
// In: r16 = RDA header word, r17 = Channel
_vcf_handle_rda:
	FUNCTION_ENTRY
	LOAD_TEMP_REGS
#if  VCF_DEBUG || 1
	STVAL(r17,r31,vcf_temp)
	FOO(0x161616,0)
	mov	0x1001001,r17
	STVAL(r17,r31,_vcf_infoblock)
	LDVAL(vcf_temp,r17)
#endif
	call	_vcf_ahandle_rda2
	nop
wait_for_in_ltu:
	bte	r0,REG_IN_LTU_ACTIVE,done_w_rda
#if  VCF_DEBUG
	FOO(0x161010,0)
#endif
inx:	mov	DP_STATUS_HI,r18
	ldio.l	r18,r19			// r19 = DP_STATUS_HI
	and	DP_ISTAT_LTU0_CNT,r19,r0
	bc	inx
// The LTU is done.  Call out_ltu_active().
#if  VCF_DEBUG
	FOO(0x161010,1)
#endif
	mov	DP_LTU0_CLEAR_CNT,r17
	calli	REG_IN_LTU_ACTIVE
	ldio.l	r17,r0			// Clear the count on the outbound LTU.
#if  VCF_DEBUG
	FOO(0x161010,2)
#endif
	btne	r0,REG_IN_LTU_ACTIVE,inx

done_w_rda:
	STORE_TEMP_REGS
	FUNCTION_EXIT
	bri	r1
	nop
	
_vcf_ahandle_rda:
#if  VCF_DEBUG
	STVAL(r17,r31,vcf_temp)
	FOO(0x16,r17)
	LDVAL(vcf_temp,r17)
#endif
_vcf_ahandle_rda2:
	ld.l	36(r17),r18	// r18 = msg
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f8	// f8 = packetlen, f9 = msglen
	ld.l	44(r17),r21	// r21 = pbuf
	fxfr	f8,r23		// r23 = packetlen ("len" in C code).
	ld.l	12(r18),r22	// r22 = vbuf
	bte	r0,r23,close_off_no_end
	st.l	r0,44(r17)	// pbuf = NULL
	btne	r0,r21,pbuf_valid
	shr	20,r22,r29
	and	0x0ffc,r29,r29	// r29 = Index into directory
	ld.l	r29(REG_DIRBASE),r21	// r21 = Page table address
	and	0x0fff,r22,r25	// r25 = Index into page
	and	0x0005,r21,r29	// User and present bits
	btne	5,r29,bogus_buffer
	andnot	0x0fff,r21,r21
	shr	10,r22,r29
	and	0x0ffc,r29,r29	// r29 = Index into page table
	ld.l	r29(r21),r29	// r29 = Page address
	and	0x0005,r29,r24	// r24 = User and present bits
	btne	5,r24,bogus_buffer
	andnot	0x0fff,r29,r29
	or	r29,r25,r21	// r21 = pbuf

pbuf_valid:
	and	LTULEN-1,r21,r0
	bc	ready_for_ltu_good_pbuf
stream_to_ltu:
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f14
	addu	-8,r23,r23	// len -= 8
	fst.d	f14,0(r21)	// Store dword to user's buffer
	addu	8,r21,r21	// pbuf += 8
	bte	0,r23,close_off
	and	LTULEN-1,r21,r0
	bnc	stream_to_ltu

ready_for_ltu:
	and	PAGELEN-1,r21,r0
	bnc	ready_for_ltu_good_pbuf
// Ouch!  After moving just a few words, we're on a new page!  Well,
//   you gotta do what you gotta do...time to recalc pbuf (r21).
ready_for_ltu_bad_pbuf:
	addu	PAGELEN-1,r22,r22	// vbuf += PAGELEN-1
	shr	20,r22,r24
	and	0x0ffc,r24,r24	// r24 = Index into directory
	ld.l	r24(REG_DIRBASE),r21	// r21 = Page table address
	and	0x0005,r21,r24	// User and present bits
	btne	5,r24,bogus_buffer
	andnot	0x0fff,r21,r21
	shr	10,r22,r24
	and	0x0ffc,r24,r24	// r24 = Index into page table
	ld.l	r24(r21),r24	// r24 = Page address
	and	0x0005,r24,r25	// r25 = User and present bits
	btne	5,r25,bogus_buffer
	andnot	0x0fff,r24,r21	// We KNOW that we're on the start of the page.

ready_for_ltu_good_pbuf:
	addu	r21,r23,r25		// r25 = pbuf + packetlen
	addu	-1,r25,r25		// r25 = addr of last byte of packet
	andnot	PAGELEN-1,r21,r26	// r26 = Page address of pbuf
	andnot	PAGELEN-1,r25,r27
	btne	r26,r27,splitpage	// This packet will straddle a page!

	subu	(LTULEN*2)-1,r23,r0
	bc	loop_tsfl
// No page straddling here...all set to use the LTU.
					// Just trust me, this chunk of code
	andnoth	0xc000,r21,r24		//   sets up an outbound LTU starting
	shr	5,r23,r25		//   at address r21 with length
	addu	-1,r25,r25		//   r23.
// The "4" here says to snoop both caches...not necessary to snoop the MP
//   cache.  Something to fix later.
	orh	0x4000,r25,r25
	stio.l	r25,r24

	andnot	LTULEN-1,r23,r25
	addu	r25,r21,r21		// r21(pbuf) += r25(len & ~LTU_MASK)
	and	LTULEN-1,r23,r23	// r23(len) &= LTU_MASK
	mov	finish_rda_noltu,REG_IN_LTU_ACTIVE
	mov	vcf_xferinfo,r31
	st.l	r21, 8(r31)	// Save all the registers that will be needed
	st.l	r23,12(r31)	//   when the LTU finishes.
	st.l	r18,16(r31)
	fst.l	f9,24(r31)
	bri	r1
	st.l	r17,20(r31)	// r17 -> xferinfo

splitpage:
	subu	r27,r21,r24	// r24 = Length of the part on THIS page
	subu	32,r24,r0
	bnc	long_enuf
// The part on this page isn't long enough for a LTU transaction.
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f12
	addu	32,r21,r21
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f14
	addu	-32,r23,r23
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f16
	fst.q	f12,-32(r21)
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f18
	br	ready_for_ltu_bad_pbuf	// Be sure to recalc the page offset!
	fst.q	f16,-16(r21)
long_enuf:
					// Just trust me, this chunk of code
	andnoth	0xc000,r21,r26		//   sets up an inbound LTU starting
	shr	5,r24,r25		//   at address r21 with length
	addu	-1,r25,r25		//   r24.
// The "4" here says to snoop both caches...not necessary to snoop the MP
//   cache.  Something to fix later.
	orh	0x4000,r25,r25
	stio.l	r25,r26

// Now we KNOW that we crossed a page so we have to recalculate pbuf.
// r27 = Address of start of next page.
	addu	PAGELEN-1,r22,r27
	shr	20,r27,r25
	and	0x0ffc,r25,r25	// r25 = Index into directory
	ld.l	r25(REG_DIRBASE),r21	// r21 = Page table address
	subu	r23,r24,r23	// r23 = Length left
	and	0x0005,r21,r25	// User and present bits
	btne	5,r25,bogus_buffer
	andnot	0x0fff,r21,r21
	shr	10,r27,r25
	and	0x0ffc,r25,r25	// r25 = Index into page table
	ld.l	r25(r21),r25	// r25 = Page address
	and	0x0005,r25,r26	// r26 = User and present bits
	btne	5,r26,bogus_buffer
	andnot	0x0fff,r25,r21	// We know that it is the START of the page.

	subu	63,r23,r0
	bc	no_ltu
	mov	finish_rda_ltu,REG_IN_LTU_ACTIVE
	orh	h%vcf_xferinfo,r0,r30
save_regs:
	or	l%vcf_xferinfo,r30,r31
	st.l	r21, 8(r31)
	st.l	r23,12(r31)
	st.l	r18,16(r31)
	st.l	r17,20(r31)
	bri	r1
	fst.l	f9,24(r31)
no_ltu:
	mov	finish_rda_noltu,REG_IN_LTU_ACTIVE
	br	save_regs
	orh	h%vcf_xferinfo,r0,r30

finish_rda_noltu:
	mov	vcf_xferinfo,r31
	ld.l	 8(r31),r21
	ld.l	12(r31),r23
	mov	r0,REG_IN_LTU_ACTIVE
	ld.l	16(r31),r18
	ld.l	20(r31),r17
	fld.l	24(r31),f9

too_short_for_ltu:
	bte	r0,r23,close_off	// r23 is never zero here.
loop_tsfl:
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f10
	addu	-8,r23,r23		// len -= 8
	fst.d	f10,0(r21)		// user buffer = f10:data in
	addu	8,r21,r21		// pbuf += 8
	btne	r0,r23,loop_tsfl

close_off:
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f0	// Get the end words.
close_off_no_end2:
// dq the message from the recv queue
	ld.l	8(r18),r25		// r25 = msg->sbuf
	ld.l	0(r18),r26		// r26 = msg->next
	ld.l	24(r17),r28		// r28 = ch->recv_pool
	ld.l	0(r17),r27		// r27 = ch->status
#if  SUPERLOCK
	lock
#endif
	fst.l	f9,0(r25)		// *(msg->sbuf) = msglen
#if  SUPERLOCK
	unlock
#endif
	st.l	r26,36(r17)		// ch->recvh = msg->next
	st.l	r28,0(r18)		// msg->next = ch->recv_pool
	st.l	r18,24(r17)		// ch->recv_pool = msg

	andnot	VCF_CS_REJECTED,r27,r27	// r27 = status & ~REJECTED
	bte	r0,r26,no_rd_pending
	st.l	r27,0(r17)		// ch->status = ch->status & ~REJECTED

// An RD is pending, so we have to move this channel to the xmit_rd queue.
	st.l	r0,64(r17)		// ch->send_rd.next = NULL
	btne	r0,REG_XMIT_HEAD,nonempty_rd_queue
	addu	64,r17,REG_XMIT_HEAD	// xmit_head = &(ch->send_rd)
	bri	r1
	mov	REG_XMIT_HEAD,REG_XMIT_TAIL
nonempty_rd_queue:
	mov	REG_XMIT_TAIL,r28
	addu	64,r17,REG_XMIT_TAIL
	bri	r1
	st.l	REG_XMIT_TAIL,0(r28)	// old_tail->next = tail

no_rd_pending:
	and	VCF_CS_GOT_USR_CLOSE,r27,r0
	bnc	close_pending
no_close_pending:
	st.l	r27,0(r17)		// ch->status = ch->status & ~REJECTED
	fxfr	f9,r30

	bri	r1
	nop
#if  0
	bri	r1
	st.l	r27,0(r17)		// ch->status = ch->status & ~REJECTED
#endif
close_pending:
	ld.l	28(r17),r28		// r28 = ch->sendh
	btne	r0,r28,no_close_pending

	st.l	r27,0(r17)		// ch->status = ch->status & ~REJECTED
	st.l	r0,80(r17)
	btne	r0,REG_XMIT_HEAD,not_first_ctrl
	addu	80,r17,REG_XMIT_HEAD	// xmit_head = &(ch->send_close)
	bri	r1
	mov	REG_XMIT_HEAD,REG_XMIT_TAIL
not_first_ctrl:
	mov	REG_XMIT_TAIL,r27
	addu	80,r17,REG_XMIT_TAIL
	bri	r1
	st.l	REG_XMIT_TAIL,0(r27)	// olt_tail->next = tail

// An RDA was read with the LTU, but then there was a page break with at
//   least 64 bytes on the other side!  What to do?  Set _in_ltu_active
//   to this function!
finish_rda_ltu:
	mov	vcf_xferinfo,r30
	ld.l	 8(r30),r21	// r21 = g_pbuf
	ld.l	12(r30),r23	// r23 = g_len

					// Just trust me, this chunk of code
	andnoth	0xc000,r21,r24		//   sets up an outbound LTU starting
	shr	5,r23,r25		//   at address r21 with length
	addu	-1,r25,r25		//   r23.
// The "4" here says to snoop both caches...not necessary to snoop the MP
//   cache.  Something to fix later.
	orh	0x4000,r25,r25
	stio.l	r25,r24

	addu	r23,r21,r21
	and	LTULEN-1,r23,r23
	st.l	r23,12(r30)
	andnot	LTULEN-1,r21,r21
	mov	finish_rda_noltu,REG_IN_LTU_ACTIVE
	bri	r1
	st.l	r21, 8(r30)

close_off_no_end:
	mov	1,r25
	br	close_off_no_end2
	ixfr	r25,f9			// Set length to 1.

bogus_buffer:
	mov	2,r30
	STVAL(r30,r31,_vcf_infoblock+8)
	STVAL(r23,r31,vcf_temp)
	FOO(0x100,0x100)
	LDVAL(vcf_temp,r23)
bogus_buffer2:
	bte	r0,r23,close_off		// If len=0, yer done!
wait_for_data:
	fld.d	NIC_STATUS_OFFSET(REG_NIC),f8
	fxfr	f8,r29
	and	NIC_STAT_RX_FIFO_NOT_EMPTY,r29,r0
	bc	wait_for_data
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f8
	br	bogus_buffer2
	addu	-8,r23,r23
