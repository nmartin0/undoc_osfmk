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
 
#include <i860paragon/msgp/msgp_asm.h>
#include <i860paragon/vcf/vcf_dispatch.h>
#if  VCF
#include <i860paragon/vcf/vcf_asm.h>
#endif
		
#define  UPP_FIX  and 63,REG_UPP_NUM,REG_UPP_NUM

	.globl	_mcmsg_kernel_post_page_out
	.globl	_mcmsg_kernel_post_page
	.globl	_mcmsg_user_post_page_out
	.globl	_mcmsg_user_post_page

// #define mcmsg_dispatch_call(mt, pp, sw) \
// 	if (pp->method < POST_MAX) { \
// 		pp->status = sw[pp->method]( mt, \
// 			 pp->arg0, pp->arg1, pp->arg2, pp->arg3, pp->arg4, \
// 			 pp->arg5, pp->arg6, pp->arg7, pp->arg8, pp->arg9); \
// 	} else { \
// 		pp->status = -1; \
// 	} \
// 	pp->method = POST_EMPTY

	.text
	.align 4
	.globl	_mcmsg_adispatch_user_vcf
_mcmsg_adispatch_user_vcf:
	FUNCTION_ENTRY		// Save permanent registers.
	LOAD_TEMP_REGS

// Fetch REG_KPP - a pointer to the kernel post page.
	LDVAL(_mcmsg_kernel_post_page_out,r16)
	LDVAL(_mcmsg_kernel_post_page,REG_KPP)
	shl	6,r16,r16	// r16 = 64*mcmsg_kernel_post_page_out
	addu	r16,REG_KPP,REG_KPP

// Fetch REG_UPP - a pointer to the user post page.
	LDVAL(_mcmsg_user_post_page,REG_UPP)
	LDVAL(_mcmsg_user_post_page_out,REG_UPP_NUM)
	shl	6,REG_UPP_NUM,REG_UPP_NUM

	mov	_msgp_asm_upp_switch,REG_UPP_SWITCH

dis_loop:
no_user_request:
	ld.c	epsr,r16		// r16 = epsr
no_user_request_2:
#if  VCF
	btne	r0,REG_IN_LTU_ACTIVE,in_ltu_active
	btne	r0,REG_RJHEAD,rj_waiting
no_rj_waiting:
#endif  /* VCF */
	andh	h%0x00020000,r16,r0
	bnc	inter_raised
no_inter_raised:
#if  VCF
	btne	r0,REG_OUT_LTU_ACTIVE,out_ltu_active
no_out_ltu_active:
	btne	r0,REG_XMIT_HEAD,xmit_ready
no_xmit_ready:
#endif  /* VCF */
	ld.l	POST_PAGE_METHOD(REG_KPP),r19	// r19 = kpp->method
#if  KEEP_UPP_IN_MEM
 LDVAL(_mcmsg_user_post_page_out,REG_UPP_NUM)
 UPP_FIX
 shl 6,REG_UPP_NUM,REG_UPP_NUM
#endif
	subs	r0,r19,r0
	ld.l	REG_UPP_NUM(REG_UPP),r18	// r18 = upp->method
	bc	kernel_request
no_kernel_request:
 	subs	r0,r18,r0			
	ld.c	epsr,r16		// r16 = epsr
	bnc	no_user_request_2

user_request:
	and	MSGP_MAX_METHOD-1,r18,r18 // r18 is now GUARANTEED to be valid.
	shl	2,r18,r16		// r16 = Method * 4
	ld.l	r16(REG_UPP_SWITCH),r17	// r17 = Branch address.
#if  KEEP_UPP_IN_MEM
 LDVAL(_mcmsg_user_post_page_out,REG_UPP_NUM)
 UPP_FIX
 shl 6,REG_UPP_NUM,REG_UPP_NUM
#endif
	addu	REG_UPP_NUM,REG_UPP,r28	// r28 = post page address.
	orh	h%no_user_request,r0,r1	// This does a call that returns to
	bri	r17			//   no_user_request automatically.
	or	l%no_user_request,r1,r1

// Kernel request.  NOT optimized for speed.
kernel_request:
#if  VCF
	btne	r0,REG_OUT_LTU_ACTIVE,no_kernel_request
	btne	r0,REG_IN_LTU_ACTIVE,no_kernel_request
#endif
	ld.l	POST_PAGE_MCMSG_TASK(REG_KPP),r16
	mov	_mcmsg_post_switch,r28

	shl	2,r19,r29		// r29 = 4*pp->method
	ld.l	r29(r28),r28		// r28 = sw[pp->method]
	ld.l	POST_PAGE_ARG0(REG_KPP),r17
	ld.l	POST_PAGE_ARG1(REG_KPP),r18
	ld.l	POST_PAGE_ARG2(REG_KPP),r19
	ld.l	POST_PAGE_ARG3(REG_KPP),r20
	ld.l	POST_PAGE_ARG4(REG_KPP),r21
	ld.l	POST_PAGE_ARG5(REG_KPP),r22
	ld.l	POST_PAGE_ARG6(REG_KPP),r23
	ld.l	POST_PAGE_ARG7(REG_KPP),r24
	ld.l	POST_PAGE_ARG8(REG_KPP),r25
	STORE_TEMP_REGS
	calli	r28
	ld.l	POST_PAGE_ARG9(REG_KPP),r26
	LOAD_TEMP_REGS
#if  SUPERLOCK
	lock
#endif
	st.l	r16,POST_PAGE_STATUS(REG_KPP)
	st.l	r0,POST_PAGE_METHOD(REG_KPP)
#if  SUPERLOCK
	unlock
#endif
// Update REG_KPP
	LDVAL(_mcmsg_kernel_post_page_out,r17)
	addu	1,r17,r17
	and	POST_PAGE_SLOTS-1,r17,r17
	STVAL(r17,r31,_mcmsg_kernel_post_page_out)
	LDVAL(_mcmsg_kernel_post_page,r18)
	shl	6,r17,r17	// r17 = 64*mcmsg_kernel_post_page_out
	addu	r17,r18,REG_KPP
// Check; if user_post_tasks != 1, time to return.
	mov	_mcmsg_user_post_tasks,r16
	ld.l	0(r16),r16
#if  KEEP_UPP_IN_MEM
 LDVAL(_mcmsg_user_post_page_out,REG_UPP_NUM)
 UPP_FIX
 shl 6,REG_UPP_NUM,REG_UPP_NUM
#endif
	ld.l	REG_UPP_NUM(REG_UPP),r18
	bte	1,r16,no_kernel_request
#if  VCF
	mov	r0,REG_DIRBASE
	STVAL(REG_DIRBASE,r30,_vcf_dirbase)
	mov	r0,REG_IN_LTU_ACTIVE
	mov	r0,REG_OUT_LTU_ACTIVE
	mov	r0,REG_XMIT_HEAD
#endif  /* VCF */
	STORE_TEMP_REGS
#if  VCF
	call	_vcf_exit_cleanup
	nop
#endif
	FUNCTION_EXIT
	bri	r1
	nop

inter_raised:
	btne	r0,REG_SEND_INT_ENABLED,maybe_send
inter_recv:
	fld.d	NIC_IO(REG_NIC),f8	// f8/f9 = First 2 words in fifo.

	orh	h%_msgp_asm_recv_switch,r0,r30
	fxfr	f8,r16			// r16 = Control word.
	or	l%_msgp_asm_recv_switch,r30,r30  // r30 = Addr of handler list
	and	MSGP_MAX_CNTRL-1,r16,r29
	shl	2,r29,r29		// r29 = 4*control word.
	ld.l	r29(r30),r28		// r28 = Handler routine address.
	fxfr	f9,r17
	orh	h%no_inter_raised,r0,r1
	bri	r28
	or	l%no_inter_raised,r1,r1

maybe_send:
	btne	r0,REG_OUT_LTU_ACTIVE,no_inter_raised
	STORE_TEMP_REGS

#if  VCF_DEBUG_PRINTOUTS
	FOO(REG_SEND_INT_ENABLED,0x200)
#endif  /* VCF_DEBUG_PRINTOUTS */

	call	_mcmsg_intr
	nop
	LOAD_TEMP_REGS
	br	no_inter_raised
	nop

#if  VCF
rj_waiting:
	LDVAL(_vcf_retry_counter,r17)
	addu	-1,r17,r17
	bnc	rj_to_go
	STVAL(r17,r30,_vcf_retry_counter)
	br	no_rj_waiting
	nop
rj_to_go:
	STORE_TEMP_REGS
	call	_vcf_retry_failures
	LOAD_TEMP_REGS
	br	no_rj_waiting
	ld.c	epsr,r16

in_ltu_active:
	mov	DP_STATUS_HI,r18
	ldio.l	r18,r19			// r19 = DP_STATUS_HI
	and	DP_ISTAT_LTU0_CNT,r19,r0
	bc	no_inter_raised
// The LTU is done.  Call in_ltu_active().
	mov	DP_LTU0_CLEAR_CNT,r17
	calli	REG_IN_LTU_ACTIVE
	ldio.l	r17,r0			// Clear the count on the inbound LTU.
	br	no_inter_raised
	nop

out_ltu_active:
	mov	DP_STATUS_HI,r18
	ldio.l	r18,r19			// r19 = DP_STATUS_HI
	and	DP_ISTAT_LTU1_CNT,r19,r0
	bc	no_xmit_ready
// The LTU is done.  Call out_ltu_active().
	mov	DP_LTU1_CLEAR_CNT,r17
	calli	REG_OUT_LTU_ACTIVE
	ldio.l	r17,r0			// Clear the count on the outbound LTU.
	br	no_xmit_ready
	nop

xmit_ready:
	fld.d	NIC_STATUS(REG_NIC),f8
	mov	REG_XMIT_HEAD,r16
	fxfr	f8,r17
	ld.l	4(REG_XMIT_HEAD),r18	// r18 = xmit_head->func
#if  BURST
	and	0x8,r17,r0		// c=Outbound buffer nonempty?
#else  /* !BURST */
	and	0x2,r17,r0		// c=Outbound buffer full?
#endif  /* BURST */
	bc	no_xmit_ready

#if  BURST
	mov	0x11,r29
	ixfr	r29,f8
	ixfr	r0,f9
	fst.d	f8,NIC_RESET_TEST_OFFSET(REG_NIC)
#endif  /* BURST */

	calli	r18
	ld.l	0(REG_XMIT_HEAD),REG_XMIT_HEAD	// xmit_head = xmit_head->next
	br	no_xmit_ready
	nop
#endif  /* VCF */

// Upcall to C from assembly for a message received.
// In:
//    All "REG_*" registers must be accurate.
//    r29 = 4*1st word in
//    r16 = 1st word in
//    r17 = 2nd word in
	.globl	_msgp_dis_recv_upcall
_msgp_dis_recv_upcall:
	addu	-16,sp,sp
	st.l	r1,0(sp)

#if  VCF
dru1:	bte	r0,REG_OUT_LTU_ACTIVE,rupcalla
	st.l	r29,4(sp)
	st.l	r16,8(sp)
	call	wait_for_out_ltu
	st.l	r17,12(sp)
	ld.l	4(sp),r29
	ld.l	8(sp),r16
	br	dru1
	ld.l	12(sp),r17
rupcalla:
#endif
	STORE_TEMP_REGS

#if  VCF_DEBUG_PRINTOUTS
	STVAL(r16,r31,vcf_temp)
	STVAL(r17,r31,vcf_temp+4)
	STVAL(r29,r31,vcf_temp+8)
	FOO(REG_SEND_INT_ENABLED,0x3000)
	LDVAL(vcf_temp,r16)
	LDVAL(vcf_temp+4,r17)
	LDVAL(vcf_temp+8,r29)
#endif  /* VCF_DEBUG_PRINTOUTS */

	mov	_mcmsg_recv_switch,r18
	ld.l	r29(r18),r28

	calli	r28			// Call the C function.
	nop
	LOAD_TEMP_REGS
	ld.l	0(sp),r1
	bri	r1			// Return to main loop.
	addu	16,sp,sp

// Upcall to C from assembly for a user-only post-page call.
// In:
//    All "REG_*" registers must be accurate.
//    r16 = 4*method
//    r28 = Address of post page.
	.globl	_msgp_dis_pp_upcall
_msgp_dis_pp_upcall:
#if  VCF
	btne	r0,REG_OUT_LTU_ACTIVE,no_user_request
	btne	r0,REG_IN_LTU_ACTIVE,no_user_request
#endif
	addu	-16,sp,sp
	st.l	r1,0(sp)

// Calculate the address of the C function to call.
	mov	_mcmsg_user_switch,r17
	ld.l	r16(r17),r27		// r27 = Address of C function

// Save REG_*'s that may have changed back to memory.  This means REG_UPP_NUM
//   and REG_SEND_INT_ENABLED.
	STORE_TEMP_REGS	

	mov	REG_USER_TASK,r16
	STVAL(r28,r31,vcf_store_pp)
	ld.l	POST_PAGE_ARG0(r28),r17
	ld.l	POST_PAGE_ARG1(r28),r18
	ld.l	POST_PAGE_ARG2(r28),r19
	ld.l	POST_PAGE_ARG3(r28),r20
	ld.l	POST_PAGE_ARG4(r28),r21
	ld.l	POST_PAGE_ARG5(r28),r22
	ld.l	POST_PAGE_ARG6(r28),r23
	ld.l	POST_PAGE_ARG7(r28),r24
	ld.l	POST_PAGE_ARG8(r28),r25
	calli	r27
	ld.l	POST_PAGE_ARG9(r28),r26
	LDVAL(vcf_store_pp,r28)
#if  SUPERLOCK
	lock
#endif
	st.l	r16,POST_PAGE_STATUS(r28)
	st.l	r0,POST_PAGE_METHOD(r28)
#if  SUPERLOCK
	unlock
#endif
	LOAD_TEMP_REGS
// Now update REG_UPP_NUM.
#if  KEEP_UPP_IN_MEM
	LDVAL(_mcmsg_user_post_page_out,REG_UPP_NUM)
	addu 1,REG_UPP_NUM,REG_UPP_NUM
	and 63,REG_UPP_NUM,REG_UPP_NUM
	STVAL(REG_UPP_NUM,r30,_mcmsg_user_post_page_out)
#else
	addu	POST_PAGE_SIZE,REG_UPP_NUM,REG_UPP_NUM
	and	(POST_PAGE_SIZE*POST_PAGE_SLOTS)-1,REG_UPP_NUM,REG_UPP_NUM
#endif
	ld.l	0(sp),r1
	bri	r1			// Return to dispatch loop.
	addu	16,sp,sp

#if  VCF
// If you may enter code that will do an XMIT w/o waiting for the LTU to
//   finish, you must first stop in here!
wait_for_out_ltu:
	addu	-16,sp,sp
	st.l	r1,0(sp)
outx:	btne	r0,REG_IN_LTU_ACTIVE,wait_for_both_ltus
	mov	DP_STATUS_HI,r18
	ldio.l	r18,r19			// r19 = DP_STATUS_HI
	and	DP_ISTAT_LTU1_CNT,r19,r0  // Is the out LTU done?
	bc	outx
// The LTU is done.  Call out_ltu_active().
	mov	DP_LTU1_CLEAR_CNT,r17
	calli	REG_OUT_LTU_ACTIVE
	ldio.l	r17,r0			// Clear the count on the outbound LTU.
	ld.l	0(sp),r1
	bri	r1
	addu	16,sp,sp

wait_for_in_ltu:
	addu	-16,sp,sp
	st.l	r1,0(sp)
inx:	btne	r0,REG_OUT_LTU_ACTIVE,wait_for_both_ltus
	mov	DP_STATUS_HI,r18
	ldio.l	r18,r19			// r19 = DP_STATUS_HI
	and	DP_ISTAT_LTU0_CNT,r19,r0
	bc	inx
// The LTU is done.  Call out_ltu_active().
	mov	DP_LTU0_CLEAR_CNT,r17
	calli	REG_IN_LTU_ACTIVE
	ldio.l	r17,r0			// Clear the count on the outbound LTU.
	ld.l	0(sp),r1
	bri	r1
	addu	16,sp,sp

wait_for_both_ltus:
bothx:	mov	DP_STATUS_HI,r18
	ldio.l	r18,r19			// r19 = DP_STATUS_HI
	and	DP_ISTAT_LTU1_CNT,r19,r0
	bnc	out_is_ready
	and	DP_ISTAT_LTU0_CNT,r19,r0
	bc	bothx
in_is_ready:
	call	wait_for_in_ltu
	nop
	br	outx
	nop
out_is_ready:
	call	wait_for_out_ltu
	nop
	br	inx
	nop
#endif

_vcf_ahandle_bump::
        fld.d   NIC_DATAIN_OFFSET(REG_NIC),f8   // Doubleword 2 of bumper.
        fld.d   NIC_DATAIN_OFFSET(REG_NIC),f8   // Doubleword 3 of bumper.
        bri     r1
        fld.d   NIC_DATAIN_OFFSET(REG_NIC),f8   // Doubleword 4 of bumper.

	.data
	.align	4
vcf_store_pp:	.long	0
