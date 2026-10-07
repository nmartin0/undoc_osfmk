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
 
#ifndef  _MSGP_ASM_DIS_H_
#define  _MSGP_ASM_DIS_H_  1

#include <i860paragon/msgp/msgp_asm.h>

#define  VCF_DEBUG  0

// General constants used _ONLY_ by the msgp assembly dispatch loop.

#define  REG_NIC	r3
#define  REG_KPP	r4
#define  REG_UPP        r5
#define  REG_UPP_NUM    r6
#define  REG_USER_TASK  r7
#define  REG_SEND_INT_ENABLED  r8
#define  REG_UPP_SWITCH    r9
#define  REG_IN_LTU_ACTIVE   r10
#define  REG_OUT_LTU_ACTIVE  r11
#define  REG_DIRBASE         r12  /* If this is NULL, then VCF isn't here. */
#define  REG_XMIT_HEAD       r13
#define  REG_XMIT_TAIL       r14
#define  REG_RJHEAD	     r15

#define  POST_PAGE_METHOD       0
#define  POST_PAGE_ARG0         4
#define  POST_PAGE_ARG1         8
#define  POST_PAGE_ARG2        12
#define  POST_PAGE_ARG3        16
#define  POST_PAGE_ARG4        20
#define  POST_PAGE_ARG5        24
#define  POST_PAGE_ARG6        28
#define  POST_PAGE_ARG7        32
#define  POST_PAGE_ARG8        36
#define  POST_PAGE_ARG9        40
#define  POST_PAGE_DUMMY1      44
#define  POST_PAGE_DUMMY2      48
#define  POST_PAGE_DUMMY3      52
#define  POST_PAGE_MCMSG_TASK  56
#define  POST_PAGE_STATUS      60
#define  POST_EMPTY        0

// If this is changed, change the SHLs too!
#define  POST_PAGE_SIZE    64
#define  POST_PAGE_SLOTS   64
#define  MSGP_MAX_METHOD  128
#define  MSGP_MAX_CNTRL   128

#define  MCMSG_HW_RECV_STATUS      0
#define  MCMSG_HW_SEND_INT_ENABLE  4

#define  FUNCTION_ENTRY  \
        addu    -64,sp,sp;  \
        st.l     r1, 0(sp); \
        st.l     r3, 4(sp); \
        st.l     r4, 8(sp); \
        st.l     r5,12(sp); \
        st.l     r6,16(sp); \
        st.l     r7,20(sp); \
        st.l     r8,24(sp); \
        st.l     r9,28(sp); \
        st.l    r10,32(sp); \
        st.l    r11,36(sp); \
        st.l    r12,40(sp); \
        st.l    r13,44(sp); \
        st.l    r14,48(sp); \
        st.l    r15,52(sp); \
	mov	NIC_BASE,REG_NIC; \
	LDVAL(_mcmsg_user_mcmsg_task,REG_USER_TASK); \
	LDVAL(_mcmsg_user_post_page,REG_UPP)

#define  FUNCTION_EXIT  \
        ld.l     0(sp), r1; \
        ld.l     4(sp), r3; \
        ld.l     8(sp), r4; \
        ld.l    12(sp), r5; \
        ld.l    16(sp), r6; \
        ld.l    20(sp), r7; \
        ld.l    24(sp), r8; \
        ld.l    28(sp), r9; \
        ld.l    32(sp),r10; \
        ld.l    36(sp),r11; \
        ld.l    40(sp),r12; \
        ld.l    44(sp),r13; \
        ld.l    48(sp),r14; \
        ld.l    52(sp),r15; \
        addu    64,sp,sp
 
#define  LDVAL(addr, reg) \
	orh	ha%addr,r0,reg; \
	ld.l	l%addr(reg),reg

#define  STVAL(reg, tmpreg, addr) \
	orh	ha%addr,r0,tmpreg; \
	st.l	reg,l%addr(tmpreg)

#if  VCF
#define  LOAD_TEMP_REGS  						\
	LDVAL(_mcmsg_hw+MCMSG_HW_SEND_INT_ENABLE,REG_SEND_INT_ENABLED);	\
	LDVAL(_in_ltu_active, REG_IN_LTU_ACTIVE); 			\
	LDVAL(_out_ltu_active, REG_OUT_LTU_ACTIVE); 			\
	LDVAL(_vcf_dirbase, REG_DIRBASE); 				\
	LDVAL(_vcf_xmit_head, REG_XMIT_HEAD); 				\
	LDVAL(_vcf_xmit_tail, REG_XMIT_TAIL);				\
	LDVAL(_vcf_rejection_head, REG_RJHEAD);				\
	LDVAL(_mcmsg_user_post_page_out, r31);				\
	shl	6,r31,REG_UPP_NUM

#define  STORE_TEMP_REGS  \
	STVAL(REG_SEND_INT_ENABLED,r30,_mcmsg_hw+MCMSG_HW_SEND_INT_ENABLE); \
	STVAL(REG_XMIT_HEAD,r30,_vcf_xmit_head);			\
	STVAL(REG_XMIT_TAIL,r30,_vcf_xmit_tail);			\
	STVAL(REG_IN_LTU_ACTIVE,r30,_in_ltu_active);			\
	STVAL(REG_OUT_LTU_ACTIVE,r30,_out_ltu_active);			\
	STVAL(REG_RJHEAD, r30,_vcf_rejection_head);			\
	shr	6,REG_UPP_NUM,r25;					\
	STVAL(r25, r30, _mcmsg_user_post_page_out)
#else  /* NOT VCF */
#define  LOAD_TEMP_REGS  						\
	LDVAL(_mcmsg_hw+MCMSG_HW_SEND_INT_ENABLE,REG_SEND_INT_ENABLED)

#define  STORE_TEMP_REGS  \
	STVAL(REG_SEND_INT_ENABLED,r30,_mcmsg_hw+MCMSG_HW_SEND_INT_ENABLE);
#endif  /* VCF */

// These are stolen from nic.h
#define  NIC_BASE                0xFFFF0000
#define  NIC_SET       0
#define  NIC_CLEAR     8
#define  NIC_CONTROL  16
#define  NIC_STATUS   24
#define  NIC_IO       32

#if  VCF
	.globl	_vcf_foo
	.globl	_vcf_temp

#define  FOO(x,y)  \
	STVAL(r1,r16,_vcf_temp); \
	mov x,r16; \
	mov y,r17; \
	call _vcf_foo; \
	nop; \
	LDVAL(_vcf_temp,r1)
#endif  /* VCF */

#endif  /* _MSGP_ASM_DIS_H_ */
