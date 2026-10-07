/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#ifndef	_I860_ASM_H_
#define	_I860_ASM_H_ 1


#ifdef	ASSEMBLER
//
//	This file contains #defines used mainly in the libc stubs to make the
//	interface as portable as possible
//
#endif	/* ASSEMBLER */

#define ALIGN 4
#define TEXT_ALIGN 16

#ifdef	__STDC__

#define EXT(x) _##x
#define LCL(x) .##x
#define MK_LABEL(a,b) a##b

#else	/*__STDC__*/

#define EXT(x) _/**/x
#define LCL(x) ./**/x
#define MK_LABEL(a,b) a/**/b

#endif	/*__STDC__*/


#define AS_SET(x)	.data; .align ALIGN; EXT(x)::
#define ASSIGN(x)	.data; .align ALIGN; EXT(x)::

#define	ENTRY(x)	.text; .align TEXT_ALIGN; EXT(x)::
#define	ASENTRY(x)	.text; .align TEXT_ALIGN; EXT(x)::

/*
 * Quick subroutine return - NO Frame pointer support.
 */
#define	RET		bri	r1;	\
			nop

/*
 * For OSF1 library generation ONLY use the following definitions:
 *	SYSCALL()	Entry definition for syscall stub; fp support
 *	RSYSCALL()	Entire syscall stub; fp support
 *	RPSEUDO(x,y)	generate a syscall label "_x" but call syscall "y"
 *			  Entire syscall stub; fp support
 *	PSEUDO()	Entry definition for pseudo syscall stub; fp support
 *	ENTRY()		routine entry code with NO fp support
 *	FRET		routine exit code with fp support
 *	SRET		routine exit code with fp support
 *	RET		routine exit code with NO fp support
 */
#ifndef	OSF1_SERVER

/*
 * subroutine entry with frame pointer support.
 */
#define	FENTRY(x)	\
	.text; .align TEXT_ALIGN; EXT(x):: \
	addu	-32, sp, sp;            \
	st.l	fp,16(sp);              \
	addu	16, sp, fp;             \
	st.l	r1, 4(fp)

/*
 * subroutine return macro with frame-pointer support.
 */

#define FRET					\
	adds	16, fp, r31;			\
	ld.l	0(fp), fp;			\
	bri	r1;				\
	 mov	r31, sp

#define	SRET FRET

#ifdef	__STDC__

/*
 * define a SYSCALL stub with frame pointer support
 */
#define	RSYSCALL(x)	.globl	_cerror;		\
			FENTRY(x);			\
			or	SYS_##x,r0,r31;		\
			trap	r31,r31,r0;		\
			bc	_cerror;		\
			FRET

#define	SYSCALL(x)	.globl	_cerror;		\
			FENTRY(x);			\
			or	SYS_##x,r0,r31;		\
			trap	r31,r31,r0;		\
			bc	_cerror

#define	CALL(x)		.globl	_cerror;		\
			or	SYS_##x,r0,r31;		\
			trap	r31,r31,r0;		\
			bc	_cerror


#define	PSEUDO(x,y)	FENTRY(x);			\
			or	SYS_##y,r0,r31;		\
			trap	r31,r31,r0;		\
			bc	_cerror

#define	RPSEUDO(x,y)	FENTRY(x);			\
			or	SYS_##y,r0,r31;		\
			trap	r31,r31,r0;		\
			bc	_cerror;		\
			FRET

#else	/*__STDC__*/

#define	RSYSCALL(x)	.globl	_cerror;		\
			FENTRY(x);			\
			or	SYS_/**/x,r0,r31;	\
			trap	r31,r31,r0;		\
			bc	_cerror;		\
	 		FRET

#define	SYSCALL(x)	.globl	_cerror;		\
			FENTRY(x);			\
			or	SYS_/**/x,r0,r31;	\
			trap	r31,r31,r0;		\
			bc	_cerror

#define	CALL(x)		.globl	_cerror;		\
			or	SYS_/**/x,r0,r31;	\
			trap	r31,r31,r0;		\
			bc	_cerror

#define	PSEUDO(x,y)	FENTRY(x);			\
			or	SYS_/**/y,r0,r31;	\
			trap	r31,r31,r0;		\
			bc	_cerror

#define	RPSEUDO(x,y)	FENTRY(x);			\
			or	SYS_/**/y,r0,r31;	\
			trap	r31,r31,r0;		\
			bc	_cerror;		\
	 		FRET

#endif	/*__STDC__*/


#endif	/* OSF1_SERVER */
#endif	/* _I860_ASM_H_ */
