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
	.globl	_vcf_handle_rd
	.globl	_vcf_ahandle_rd
	.align	8
// In: R16 = ID # for an RD
//     R17 = First word of RD (Channel).
#define  reg_lchan    r17
#define  reg_status   r18
#define  reg_rdlen    r19
#define  reg_sendlen  r20
#define  reg_rdgroup  r21
#define  reg_sendh    r22
#define  reg_temp     r23

_vcf_handle_rd:
	FUNCTION_ENTRY
	LOAD_TEMP_REGS
	call	_vcf_ahandle_rd
	nop
	STORE_TEMP_REGS
	FUNCTION_EXIT
	bri	r1
	nop
	
_vcf_ahandle_rd:
#if  VCF_DEBUG
	STVAL(r17,r30,vcf_temp)
	FOO(0x22222222,0)
	LDVAL(vcf_temp,r17)
#endif

	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f8	// f8 = gang, f9 = path
	ld.l	VCF_CHAN_STATUS(reg_lchan),reg_status
	fld.d	NIC_DATAIN_OFFSET(REG_NIC),f10	// f10 = seqno, f11 = length
//	fxfr	f8,reg_rdgroup		GROUPS: Not supported

	and	VCF_CS_GOT_USR_CLOSE,reg_status,r0
//	btne	REG_GROUP,reg_rdgroup,bogus_group	GROUPS: Not supported
	bte	r0,REG_DIRBASE,bogus_group	// GROUPS: Not supported

	ld.l	VCF_CHAN_SENDH(reg_lchan),reg_sendh // r21 = ch->sendh = msg
	bnc	ch_maybe_closed

ch_not_closed:
	fst.l	f10,VCF_CHAN_SEQ_RECVD(reg_lchan) // ch->seq_recvd = f10
	btne	0,reg_sendh,send_pending
// No send is pending.  Record the rd and continue.
	bri	r1
	fst.l	f11,VCF_CHAN_RD_LEN(reg_lchan)	// ch->rd_len = f11

send_pending:	// Queue this channel for RDA transmission.
	ld.l	VCF_REQ_LEN(reg_sendh),reg_sendlen
	fxfr	f11,reg_rdlen		// r22 = f11 = RD length
	st.l	r0,VCF_CHAN_SRDA_NEXT(reg_lchan) // ch->send_rda.next = NULL
	subs	reg_rdlen,reg_sendlen,r0
	bc	msglen_nogreater
	st.l	reg_rdlen,4(reg_sendh)
	mov	reg_rdlen,reg_sendlen
msglen_nogreater:
	st.l	reg_sendlen,VCF_CHAN_RD_LEN(reg_lchan) // r20 = msg->len
	btne	r0,REG_XMIT_HEAD,head_nonzero
// The queue is empty, so stick this channel on the head.
	addu	VCF_CHAN_SRDA_NEXT,reg_lchan,REG_XMIT_HEAD
	bri	r1
	mov	REG_XMIT_HEAD,REG_XMIT_TAIL

head_nonzero:
	mov	REG_XMIT_TAIL,reg_temp
	addu	VCF_CHAN_SRDA_NEXT,reg_lchan,REG_XMIT_TAIL
	bri	r1
	st.l	REG_XMIT_TAIL,0(reg_temp)	// old_tail->next = tail

ch_maybe_closed:
	btne	0,reg_sendh,ch_not_closed
// Channel is dead.  This change MAY not be necessary.
	or	VCF_CS_CHANDEAD,reg_status,reg_status
	bri	r1
	st.l	reg_status,0(reg_lchan)

bogus_group:
	STORE_TEMP_REGS
	mov	r1,REG_NIC
	fxfr	f10,REG_DIRBASE
	call	_vcf_invert_path
	fxfr	f9,r16
	call	_vcf_got_rq_badgang
	mov	REG_DIRBASE,r17
	LOAD_TEMP_REGS
	bri	REG_NIC
	nop

	.data
	.align 4
vcf_temp::	.long	0
       .long   0
       .long   0
       .long   0
       .long   0
       .long   0
       .long   0
       .long   0
       .long   0
       .long   0
       .long   0
       .long   0
       .long   0
       .long   0
       .long   0
