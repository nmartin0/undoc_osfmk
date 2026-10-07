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
	.align	4
	.globl	_vcf_send
	.globl	_vcf_recv
	.globl	_vcf_asend
	.globl	_vcf_arecv
_vcf_send:
	FUNCTION_ENTRY
	LOAD_TEMP_REGS
	mov	r17,r21
	mov	r19,r17
	mov	r18,r19
	mov	r20,r18
	mov	r21,r20
	call	send_2
	nop
	STORE_TEMP_REGS
	FUNCTION_EXIT
	bri	r1
	nop

_vcf_asend:
#if  VCF_DEBUG
	STVAL(r28,r31,vcf_temp)
	FOO(5,0)
	LDVAL(vcf_temp,r28)
#endif

#if  KEEP_UPP_IN_MEM
 LDVAL(_mcmsg_user_post_page_out,REG_UPP_NUM)
 addu 1,REG_UPP_NUM,REG_UPP_NUM
 and 63,REG_UPP_NUM,REG_UPP_NUM
 STVAL(REG_UPP_NUM,r30,_mcmsg_user_post_page_out)
#else
	addu	POST_PAGE_SIZE,REG_UPP_NUM,REG_UPP_NUM
	and	(POST_PAGE_SIZE*POST_PAGE_SLOTS)-1,REG_UPP_NUM,REG_UPP_NUM
#endif
	ld.l	 4(r28),r20	// r20 = chan
	ld.l	 8(r28),r19	// r19 = buf
	ld.l	12(r28),r17	// r17 = len
	ld.l	16(r28),r18	// r18 = sbufp
	st.l	r0,0(r28)	// usrcomm->status = UNUSED

send_2:
// Chalculate the PHYSICAL address of sbufp.
	shr	20,r18,r22
	and	0xffc,r22,r22	// r22 = Index into directory
	ld.l	r22(REG_DIRBASE),r21	// r21 = Page table address
	andnot	7,r17,r17	// len &= ~7
	and	0x0005,r21,r23
	btne	5,r23,bogus_sbufp
	andnot	0xfff,r21,r21
	shr	10,r18,r22
	and	0xffc,r22,r22	// r22 = Index into page table
	ld.l	r22(r21),r21	// r21 = Page address
	orh	ha%_vcf_chan_tbl,r0,r24
	and	0x0005,r21,r23
	btne	5,r23,bogus_sbufp
	and	0x0ffc,r18,r23
	andnot	0x0fff,r21,r22
	addu	r22,r23,r18	// r18 = physaddr of sbufp

	ld.l	l%_vcf_chan_tbl(r24),r24	// r24 = _chan_tbl
	orh	ha%_vcf_num_chans,r0,r26
	subu	r20,r24,r25
	ld.l	l%_vcf_num_chans(r26),r26	// r26 = num_chans
	bnc	invalid_argument	// bri r24:chan_tbl > r20:ch
	shl	7,r26,r26		// r26 = sizeof(chan_tbl)
	subu	r25,r26,r0
	bc	invalid_argument	// bri r26:sizeof(tbl) >= r25:offset

	andnot	0x7f,r20,r20		// ch &= ~127 (sizeof(chan) - 1)
	ld.l	0(r20),r24		// r24 = ch->status
	ld.l	20(r20),r21		// r21:msg = ch->send_pool
	and	VCF_CS_GOT_USR_CLOSE+VCF_CS_GOT_RMT_CLOSE,r24,r0
	bnc	dead_channel

	bte	r0,r21,no_buf
	ld.l	0(r21),r22		// r22:newempty = r21:msg->next
	ld.l	28(r20),r16		// r16:head = r20:ch->sendh
	st.l	r22,20(r20)		// r20:ch->send_pool = r22:newempty
	st.l	r17,4(r21)		// r21:msg->len = r17:len
	st.l	r18,8(r21)		// r21:msg->sbufp = r18:sbufp
	st.l	r19,12(r21)		// r21:msg->buf = r19:buf

// Time to nq the msg into the "to-send" list.
	st.l	r0,0(r21)		// r21:msg->next = 0
	btne	r0,r16,not_first_send
// This is the first request queued up.  You've got to check if an RD has
//   already arrived; if so, queue this channel up in the xmit_rda queue.
	ld.l	12(r20),r18		// r18:rd_len = r20:ch->rd_len
	st.l	r21,28(r20)		// r20:ch->sendh = r21:msg
	bte	1,r18,set_tail		// 1 means no rd has arrived.
	st.l	r21,32(r20)		// r20:ch->sendt = r21:msg
	st.l	r0,56(r20)		// ch->send_rda.next = 0
	btne	r0,REG_XMIT_HEAD,not_first_rda
	addu	56,r20,REG_XMIT_HEAD	// xmit_head = &(ch->send_rda)
	bri	r1
	mov	REG_XMIT_HEAD,REG_XMIT_TAIL
not_first_rda:
	mov	REG_XMIT_TAIL,r22	// r22 = xmit_rda_tail
	addu	56,r20,REG_XMIT_TAIL
	bri	r1
	st.l	REG_XMIT_TAIL,0(r22)	// old_tail->next = new_tail

not_first_send:
	ld.l	32(r20),r16		// r16:tail = r20:ch->sendt
	st.l	r21,32(r20)		// r20:ch->sendt = r21:msg
	bri	r1
	st.l	r21,0(r16)		// r16:tail->next = r21:msg
set_tail:
	bri	r1
	st.l	r21,32(r20)		// r20:ch->sendt = r21:msg

//----------------------------------------------------------------------
_vcf_recv:
	FUNCTION_ENTRY
	LOAD_TEMP_REGS
	mov	r17,r21
	mov	r19,r17
	mov	r18,r19
	mov	r20,r18
	mov	r21,r20
	call	recv_2
	nop
	STORE_TEMP_REGS
	FUNCTION_EXIT
	bri	r1
	nop

_vcf_arecv:
#if  VCF_DEBUG
	STVAL(r28,r31,vcf_temp)
	FOO(7,0)
	LDVAL(vcf_temp,r28)
#endif

#if  KEEP_UPP_IN_MEM
 LDVAL(_mcmsg_user_post_page_out,REG_UPP_NUM)
 addu 1,REG_UPP_NUM,REG_UPP_NUM
 and 63,REG_UPP_NUM,REG_UPP_NUM
 STVAL(REG_UPP_NUM,r30,_mcmsg_user_post_page_out)
#else
	addu	POST_PAGE_SIZE,REG_UPP_NUM,REG_UPP_NUM
	and	(POST_PAGE_SIZE*POST_PAGE_SLOTS)-1,REG_UPP_NUM,REG_UPP_NUM
#endif
	ld.l	 4(r28),r20	// r20 = chan
	ld.l	 8(r28),r19	// r19 = buf
	ld.l	12(r28),r17	// r17 = len
	ld.l	16(r28),r18	// r18 = sbufp
	st.l	r0,0(r28)	// usrcomm->status = UNUSED

recv_2:
// Chalculate the PHYSICAL address of sbufp.
	shr	20,r18,r22
	and	0xffc,r22,r22	// r22 = Index into directory
	ld.l	r22(REG_DIRBASE),r21	// r21 = Page table address
	andnot	7,r17,r17	// len &= ~7
	and	0x0005,r21,r23
	btne	5,r23,bogus_sbufp
	andnot	0xfff,r21,r21
	shr	10,r18,r22
	and	0xffc,r22,r22	// r22 = Index into page table
	ld.l	r22(r21),r21	// r21 = Page address
	orh	ha%_vcf_chan_tbl,r0,r24
	and	0x0005,r21,r23
	btne	5,r23,bogus_sbufp
	and	0x0ffc,r18,r23
	andnot	0x0fff,r21,r22
	addu	r22,r23,r18	// r18 = physaddr of sbufp

	ld.l	l%_vcf_chan_tbl(r24),r24	// r24 = _chan_tbl
	andnot	0x7f,r20,r20		// ch &= ~127:(sizeof(chan) - 1)
	subu	r20,r24,r25
	bnc	invalid_argument	// bri r24:chan_tbl > r20:ch
	orh	ha%_vcf_num_chans,r0,r26
	ld.l	l%_vcf_num_chans(r26),r26	// r26 = num_chans
	shl	7,r26,r26		// r26 = sizeof(chan_tbl)
	subu	r25,r26,r0
	bc	invalid_argument	// bri r26:sizeof(tbl) >= r25:offset

	ld.l	0(r20),r24		// r24 = ch->status
	and	VCF_CS_GOT_USR_CLOSE+VCF_CS_GOT_RMT_CLOSE,r24,r0
	bnc	dead_channel

	ld.l	24(r20),r21		// r21:msg = ch->recv_pool
	bte	r0,r21,no_buf
	ld.l	0(r21),r22		// r22:newempty = r21:msg->next
	ld.l	36(r20),r16		// r16:head = r20:ch->recvh
	st.l	r22,24(r20)		// r20:ch->send_pool = r22:newempty
	st.l	r17,4(r21)		// r21:msg->len = r17:len
	st.l	r18,8(r21)		// r21:msg->sbufp = r18:sbufp
	st.l	r19,12(r21)		// r21:msg->buf = r19:buf

// Time to nq the msg into the "to-send" list.
	st.l	r0,0(r21)		// r21:msg->next = 0
	btne	r0,r16,not_first_recv
// This is the first request queued up.  You've got to put the channel
//   into the xmit queue for the RD.
	st.l	r21,36(r20)		// r20:ch->recvh = r21:msg
	and	VCF_CS_GOT_RMT_OPEN,r24,r0
	bc	cant_send_rd_yet	// You need rmt O to send RD
	st.l	r21,40(r20)		// r20:ch->recvt = r21:msg
	st.l	r0,64(r20)		// ch->send_rd.next = 0
	btne	r0,REG_XMIT_HEAD,not_first_xmit
	addu	64,r20,REG_XMIT_HEAD	// xmit_head = &(ch->send_rd)
	bri	r1
	mov	REG_XMIT_HEAD,REG_XMIT_TAIL
not_first_xmit:
	mov	REG_XMIT_TAIL,r22	// r22 = xmit_tail
	addu	64,r20,REG_XMIT_TAIL
	bri	r1
	st.l	REG_XMIT_TAIL,0(r22)	// old_tail->next = new_tail

cant_send_rd_yet:
	bri	r1
	st.l	r21,40(r20)		// r20:ch->recvt = r21:msg

not_first_recv:
	ld.l	40(r20),r16		// r16:tail = r20:ch->recvt
	st.l	r21,40(r20)		// r20:ch->recvt = r21:msg
	bri	r1
	st.l	r21,0(r16)		// r16:tail->next = r21:msg

no_buf:
	mov	VCF_ENOBUF,r16
	bri	r1
	st.l	r16,0(r18)	// *sbufp = VCF_ENOBUF
dead_channel:
	mov	VCF_ECHANDEAD,r16
	bri	r1
	st.l	r16,0(r18)	// *sbufp = VCF_ECHANDEAD
invalid_argument:
	mov	VCF_EINVAL,r16
	st.l	r16,0(r18)	// *sbufp = VCF_EINVAL
bogus_sbufp:
	mov	1,r16
	STVAL(r16,r31,_vcf_infoblock+8)
	bri	r1
	nop
