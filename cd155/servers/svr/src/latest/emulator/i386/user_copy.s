/*
 *
 *$Copyright
 *Copyright 1993, 1994 , 1995 Intel Corporation
 *INTEL CONFIDENTIAL
 *The technical data and computer software contained herein are subject
 *to the copyright notices; trademarks; and use and disclosure
 *restrictions identified in the file located in /etc/copyright on
 *this system.
 *Copyright$
 *
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: user_copy.s,v $
 *Revision 1.5  1994/11/18  20:25:19  mtm
 *Copyright additions/changes
 *
 *Revision 1.4  1993/07/14  17:32:27  cfj
 *OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 *Revision 1.1.1.3  1993/07/01  18:25:24  cfj
 *Adding new code from vendor
 *
 *Revision 1.3  1993/05/06  18:59:05  cfj
 *ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:18:53  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:09:40  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:30:07  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.6  1992/10/06  12:07:02  roman
 * Fix RCS comments.
 *
 * Revision 2.5  92/10/05  13:36:51  klh
 * 	Revision 2.8  92/09/17  13:44:02  rabii
 * 		Fixed more bugs in user_rwcheck2 and user_rcheck2 (sjs)
 * 
 * 	Revision 2.7  92/09/11  10:27:54  rabii
 * 		Fixed bug in user_rwcheck2 where an incorrect jump was 
 *		happening (rabii)
 * 
 * 	Revision 2.6  92/09/11  09:25:50  rabii
 * 		Added new routines user_rcheck2 and user_rwcheck2 to behave
 * 		similar to user_bcopy2 (return correct counts, use uacc_err
 * 		interface).  Fixed uacc_err to return correct counts.  Cleaned
 * 		up user_bcopy2.
 * 		[92/09/04            sjs]
 * 
 * Revision 2.4  92/08/06  13:29:53  klh
 * 	Revision 2.5  92/07/29  08:49:16  rabii
 * 		Fixed comments for user_bcopy2
 * 
 * 	Revision 2.4  92/07/29  08:31:45  rabii
 * 		Added user_bcopy2 as explained in the comments below
 * 
 * Revision 2.3  92/07/10  16:19:20  mbarnett
 * 	Detect user_r[w]check(addr, len) wrapping location 0.
 * 
 * Revision 2.2  92/05/31  18:57:45  loverso
 * 	From Gr3.5
 * 
 * Revision 1.2  1992/05/12  13:04:12  devrcs
 * 	Created for OSF/1 MK
 * 	[1992/05/05  00:57:08  condict]
 *
 * $EndLog$
*/
/*
Revision 3.0  92/02/28  00:10:53  condict
Contains versions of bcopy and strlen that can recover from faults while
accessing user memory.  Also, general purpose read and write access-checks.
Provides support for EFAULT recovery in the emulator.

 */

/*
 * Functions for copying data between the emulator and the user, and for
 * checking access to user data, with fault recovery.  This is the analog
 * of copyin/copyout in an integrated kernel
 */

#include <i386/asm.h>

/*
 * Marks the starting address of functions that can fault on access to user
 * addresses.  Any exception between here and uacc_end will result in the
 * server branching us to the common error recovery point, uacc_err
 */

/*
 * Note: user_bcopy2 has very different semantics than the rest of
 * routines in this file. 
 * 1) Unlike the other routines, user_bcopy2 will return 0 for success, 
 * and the actual error (put in the proper register by the server 
 * exception handler) in case of an error.
 * 2) For it's arguments user_bcopy2 takes the usual "from", and "to"
 * pointers, but as the third argument, takes the address of bcount,
 * the number of bytes to be copied, which is updated to the actual
 * number of bytes copied upon exit.
 * 
 * Since the routines share a common error exit, the exit code needs to
 * know which entry point it came in from. On i386, ebx is used to 
 * indicate this. 
 * 
 * ebx = 0 means entry point not through user_bcopy2. In these cases
 * return 0 for failure and 1 for success.
 * ebx != 0 means entry point through user_bcopy2. In these cases
 * return the contents of eax (set by the server's exception handling
 * routines) for failure and 0 for success.
 *
 * When ebx != 0, the top value of the stack is used as the address of
 * where to save the number of bytes actually copied.
 */
ENTRY(uacc_start)

/*
 * int user_bcopy(from, to, bcount)
 *
 * NOTE: This version must be used whenever accessing user addresses.  It
 *	returns 1 normally, and returns 0 if there was an exception (because
 *	the server resets our PC to uacc_err in this case).
 */

ENTRY(user_bcopy)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	movl	$0,%ebx		/* initialize to zero */
	movl	B_ARG0,%esi
	movl	B_ARG1,%edi
	movl	B_ARG2,%edx
	cld
/*  move longs */
	movl	%edx,%ecx
	sarl	$2,%ecx
	rep
	movsl
/* move bytes */
	movl	%edx,%ecx
	andl	$3,%ecx
	rep
	movsb
/* return success: */
	movl	$1,%eax
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret

/*
 * int user_bcopy(from, to, &count)
 *
 * NOTE: Please see comment at the begining of the file
 */
ENTRY(user_bcopy2)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	movl	B_ARG0,%esi
	movl	B_ARG1,%edi
	movl	B_ARG2,%ebx	/ get the count address
	movl	(%ebx),%edx	/ put count in edx
	pushl	%ebx		/ put address of count on stack
	cld
/*  move longs */
	movl	%edx,%ecx
	sarl	$2,%ecx
	rep
	movsl
/* move bytes */
	movl	%edx,%ecx
	andl	$3,%ecx
	rep
	movsb
/* return success: */
	popl	%ebx
	movl	$0,%eax
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret
/*
 * int user_strlen(s, len_p)
 *
 * NOTE: This version of strlen must be used whenever accessing user addresses.
 *	It returns 1 normally, and returns 0 if there was an exception (because
 *	the server resets our PC to uacc_err in this case).  It returns the
 *	length of the string in the int pointed to by its second arg.
 */
ENTRY(user_strlen)
	pushl	%ebp
	movl	%esp,%ebp
/*
 * push these two just so common error recovery code can be used (see uacc_err):
 */
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	movl	$0,%ebx		/* initialize to zero */
	movl	B_ARG0,%eax
	xorl	%ecx,%ecx
	jmp L2
	.align 2
L1:
	incl	%ecx
L2:
	movb	(%eax),%dl
	incl	%eax
	testb	%dl,%dl
	jne L1
	movl	B_ARG1,%eax
	movl	%ecx,(%eax)
	movl	$1,%eax
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret

/*
 * int user_rcheck(cp, len)
 *
 *	Checks for read access of the memory starting at cp and extending
 *	len bytes.  It returns 1 normally, and returns 0 if there was an
 *	exception (the server resets our PC to uacc_err in this case).
 */
ENTRY(user_rcheck)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	movl	$0,%ebx			/ initialize to zero
	movl	B_ARG0,%eax		/ reg(cp) = cp
	movl	%eax,%edi		/ end = reg(cp)
	addl	B_ARG1,%edi		/	+ len;
L3:
	cmpl	%edi,%eax		/ while (cp > end)
	jbe	L4			/ {
	movb	(%eax),%esi		/	reg(temp) = *cp;
	addl	_vm_page_size,%eax	/	reg(cp) += vm_page_size;
	jmp	L3			/ }
	.align	2
L4:
	cmpl	%eax,%edi		/ while (cp < end)
	jbe	L5			/ {
	movb	(%eax),%esi		/	reg(temp) = *cp;
	addl	_vm_page_size,%eax	/	reg(cp) += vm_page_size;
	jmp	L4			/ }
	.align	2
L5:
	movl	$1,%eax			/ return success;
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret
/*
 * int user_rcheck(cp, &len)
 *
 * NOTE that ebx is used as a switch in the error path (if we get
 * vectored to uacc_err); everything is fine so long as ebx has a 
 * non-zero value.
 */

ENTRY(user_rcheck2)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	movl	B_ARG0,%esi		/ reg(cp) = cp
	movl	%esi,%edi		/ end = reg(cp)
	mov	B_ARG1,%ebx		/ ebx is also flag to uacc_err
	pushl	%ebx			/ save count pointer
	addl	(%ebx),%edi		/	+ len;

	movb	(%esi),%eax		/ test current page
	/
	/ Point sourc at page_boundary - 1 to render accurate counts
	/ in the error case
	/
	addl	_vm_page_size,%esi	/	reg(cp) += vm_page_size;
	movl	_vm_page_size,%ebx	/ form a page mask
	decl	%ebx
	notl	%ebx
	and	%ebx,%esi		/ page align
	decl	%esi			/ point one before page end
	
L42:
	cmpl	%esi,%edi		/ while (cp < end)
	jbe	L52			/ {
	movb	1(%esi),%eax		/	reg(temp) = *cp;
	addl	_vm_page_size,%esi	/	reg(cp) += vm_page_size;
	jmp	L42			/ }
	.align	2
L52:
	movl	$0,%eax			/ return success;
	popl	%ebx			/ pop extra count address
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret

/*
 * int user_rwcheck(cp, len)
 *
 *	Just like user_rcheck, above, except checks for read/write access.
 */
ENTRY(user_rwcheck)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	movl	$0,%ebx			/ initialize to zero
	movl	B_ARG0,%eax		/ reg(cp) = cp
	movl	%eax,%edi		/ end = reg(cp)
	addl	B_ARG1,%edi		/	+ len;
L6:
	cmpl	%edi,%eax		/ while (cp > end)
	jbe	L7			/ {
	movb	(%eax),%esi		/	reg(temp) = *cp;
	movb	%esi,(%eax)		/	*cp = reg(temp);
	addl	_vm_page_size,%eax	/	reg(cp) += vm_page_size;
	jmp	L6			/ }
	.align	2
L7:
	cmpl	%eax,%edi		/ while (cp < end)
	jbe	L8			/ {
	movb	(%eax),%esi		/	reg(temp) = *cp;
	movb	%esi,(%eax)		/	*cp = reg(temp);
	addl	_vm_page_size,%eax	/	reg(cp) += vm_page_size;
	jmp	L7			/ }
	.align	2
L8:
	movl	$1,%eax			/ return success;
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret

/*
 * user_rwcheck2(src, &len)
 *
 * NOTE that ebx is used as a switch in the error path (if we get
 * vectored to uacc_err); everything is fine so long as ebx has a 
 * non-zero value.
 */
ENTRY(user_rwcheck2)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	movl	B_ARG0,%esi		/ reg(cp) = cp
	movl	%esi,%edi		/ end = reg(cp)
	mov	B_ARG1,%ebx		/ ebx is also flag to uacc_err
	pushl	%ebx			/ save count pointer
	addl	(%ebx),%edi		/	+ len;

	movb	(%esi),%eax		/ test current page
	/
	/ Point sourc at page_boundary - 1 to render accurate counts
	/ in the error case
	/
	addl	_vm_page_size,%esi	/	reg(cp) += vm_page_size;
	movl	_vm_page_size,%ebx	/ form a page mask
	decl	%ebx
	notl	%ebx
	and	%ebx,%esi		/ page align
	decl	%esi			/ point one before page end

L72:
	cmpl	%esi,%edi		/ while (cp < end)
	jbe	L82			/ {
	movb	1(%esi),%eax		/	reg(temp) = *cp;
	movb	%eax,1(%esi)		/	*cp = reg(temp);
	addl	_vm_page_size,%esi	/	reg(cp) += vm_page_size;
	jmp	L72			/ }
	.align	2
L82:
	movl	$0,%eax			/ return success;
	popl	%ebx			/ pop extra count address
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret

/*
 * Marks the end of functions that access user addresses.  See uacc_start,
 * above.
 */
ENTRY(uacc_end)

ENTRY(uacc_err)
	cmpl	$0,%ebx			/ ebx !=0 means user_bcopy2
	jz	L10
	movl	B_ARG0,%edi		/ get original source pointer 
	subl	%edi,%esi		/ subtract off current source
	popl	%ebx
	movl	%esi,(%ebx)		/ save the count back
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret
L10:
	movl	$0,%eax
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret

