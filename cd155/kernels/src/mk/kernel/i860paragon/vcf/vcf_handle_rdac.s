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
	.globl	vcf_xferinfo

	.text
	.globl	_vcf_ahandle_rdac
	.globl	_vcf_handle_rdac
	.align	8
// In: r16 = RDAC header word, r17 = Channel
_vcf_handle_rdac:
	FUNCTION_ENTRY
	LOAD_TEMP_REGS
#if  VCF_DEBUG || 1
	STVAL(r17,r31,vcf_temp)
	FOO(0x151515,0)
	mov	0x1001001,r17
	STVAL(r17,r31,_vcf_infoblock+4)
	LDVAL(vcf_temp,r17)
#endif
	call	_vcf_ahandle_rdac2
	nop
wait_for_in_ltu:
	bte	r0,REG_IN_LTU_ACTIVE,done_w_rdac
#if  VCF_DEBUG
	FOO(0x151010,0)
#endif
inx:	mov	DP_STATUS_HI,r18
	ldio.l	r18,r19			// r19 = DP_STATUS_HI
	and	DP_ISTAT_LTU0_CNT,r19,r0
	bc	inx
// The LTU is done.  Call out_ltu_active().
#if  VCF_DEBUG
	FOO(0x151010,1)
#endif
	mov	DP_LTU0_CLEAR_CNT,r17
	calli	REG_IN_LTU_ACTIVE
	ldio.l	r17,r0			// Clear the count on the outbound LTU.
#if  VCF_DEBUG
	FOO(0x151010,2)
#endif
	btne	r0,REG_IN_LTU_ACTIVE,inx

done_w_rdac:	
	STORE_TEMP_REGS
	FUNCTION_EXIT
	bri	r1
	nop
	
_vcf_ahandle_rdac:
#if  VCF_DEBUG
	STVAL(r17,r31,vcf_temp)
	FOO(0x15,0)
	LDVAL(vcf_temp,r17)
#endif
_vcf_ahandle_rdac2:
	ld.l	36(r17),r18	// r18 = msg
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f8	// f8 = packetlen
	ld.l	44(r17),r21	// r21 = pbuf
	ld.l	12(r18),r22	// r22 = vbuf
	st.l	r0,44(r17)	// pbuf = NULL
	fxfr	f8,r23		// r23 = packetlen ("len" in C code).
	btne	r0,r21,pbuf_valid
	shr	20,r22,r29
	and	0x0ffc,r29,r29	// r29 = Index into directory
	ld.l	r29(REG_DIRBASE),r21	// r21 = Page table address
	and	0x0005,r21,r29	// User and present bits
	btne	5,r29,bogus_buffer
	andnot	0x0fff,r21,r21
	shr	10,r22,r29
	and	0x0ffc,r29,r29	// r29 = Index into page table
	ld.l	r29(r21),r29	// r29 = Page address
	and	0x0005,r29,r24	// r24 = User and present bits
	btne	5,r24,bogus_buffer
	and	0x0fff,r22,r24	// r24 = Index into page
	andnot	0x0fff,r29,r29
	or	r29,r24,r21	// r21 = pbuf

pbuf_valid:
	addu	r23,r22,r25	// r25 = Start address of next buffer
	and	LTULEN-1,r21,r0
	st.l	r25,12(r18)	// msg->buf += packetlen
	bc	ready_for_ltu_good_pbuf
stream_to_ltu:
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f14
	addu	-8,r23,r23	// len -= 8
	fst.d	f14,0(r21)	// *(r21:pbuf) = f14:meshdata
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
	bc	too_short_for_ltu

// No page straddling here...all set to use the LTU.
					// Just trust me, this chunk of code
	andnoth	0xc000,r21,r24		//   sets up an inbound LTU starting
	shr	5,r23,r25		//   at address r21 with length
	addu	-1,r25,r25		//   r23.
// The "4" here says to snoop both caches...not necessary to snoop the MP
//   cache.  Something to fix later.
	orh	0x4000,r25,r25
	stio.l	r25,r24

	andnot	LTULEN-1,r23,r25
	addu	r25,r21,r21		// r21(pbuf) += r25(len & ~LTU_MASK)
	and	LTULEN-1,r23,r23	// r23(len) &= LTU_MASK
	mov	finish_rdac_noltu,REG_IN_LTU_ACTIVE
	mov	vcf_xferinfo,r31
	st.l	r21, 8(r31)	// Save all the registers that will be needed
	st.l	r23,12(r31)	//   when the LTU finishes.
	st.l	r18,16(r31)
	fst.l	f9,24(r31)
	bri	r1
	st.l	r17,20(r31)

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
	andnoth	0xc000,r21,r26		//   sets up an outbound LTU starting
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
	mov	finish_rdac_ltu,REG_IN_LTU_ACTIVE
	orh	h%vcf_xferinfo,r0,r30
save_regs:
	or	l%vcf_xferinfo,r30,r31
	st.l	r21, 8(r31)
	st.l	r23,12(r31)
	st.l	r18,16(r31)
	st.l	r17,20(r31)
	bri	r1
	fst.l	 f9,24(r31)
no_ltu:
	mov	finish_rdac_noltu,REG_IN_LTU_ACTIVE
	br	save_regs
	orh	h%vcf_xferinfo,r0,r30

finish_rdac_noltu:
	mov	vcf_xferinfo,r31
	ld.l	 8(r31),r21
	ld.l	12(r31),r23
	mov	r0,REG_IN_LTU_ACTIVE
	ld.l	16(r31),r18
	ld.l	20(r31),r17
	fld.l	24(r31),f9

too_short_for_ltu:
	bte	r0,r23,close_off
loop_tsfl:
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f10
	addu	-8,r23,r23		// len -= 8
	fst.d	f10,0(r21)
	addu	8,r21,r21		// pbuf += 8
	btne	r0,r23,loop_tsfl

close_off:
	and	PAGELEN-1,r21,r0
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f0	// Get the end words.
	bc	no_valid_addr
	st.l	r21,44(r17)	// pbuf = r21	
	bri	r1
	nop
no_valid_addr:
	bri	r1
	nop

// An RDAC was read with thi LTU, but then there was a page break with at
//   least 64 bytes on the other side!  What to do?  Set _in_ltu_active
//   to this function!
finish_rdac_ltu:
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
	mov	finish_rdac_noltu,REG_IN_LTU_ACTIVE
	bri	r1
	st.l	r21, 8(r30)

bogus_buffer:
	mov	3,r30
	STVAL(r30,r31,_vcf_infoblock+8)
	bte	r0,r23,close_off		// If len=0, yer done!
wait_for_data:
	fld.d	NIC_STATUS_OFFSET(REG_NIC),f8
	fxfr	f8,r29
	and	NIC_STAT_RX_FIFO_NOT_EMPTY,r29,r0
	bc	wait_for_data
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f8
	br	bogus_buffer
	addu	-8,r23,r23
