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
 * $Log: user_bcopy.s,v $
 *Revision 1.3  1994/11/18  20:31:27  mtm
 *Copyright additions/changes
 *
 *Revision 1.2  1993/07/14  17:57:29  cfj
 *OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:16:29  cfj
 * Adding new code from vendor
 *
 * Revision 1.1  1992/12/02  21:33:18  dleslie
 * New bug drop from Locus, December 2, 1992
 *
 * Revision 2.2  92/11/17  19:49:36  loverso
 * 	New for server
 * 
 * 	$EndLog$
 *
*/

#include <i386/asm.h>

/*
 * Marks the starting address of functions that can fault on access to user
 * addresses.  Any exception between here and uacc_end will result in the
 * server branching us to the common error recovery point, uacc_err
 */
ENTRY(uacc_start)

/*
 * Note: user_bcopy2 has very different semantics than other bcopy routines.
 * 1) Unlike the other routines, user_bcopy2 will return 0 for success, 
 * and the actual error (put in the proper register by the server 
 * exception handler) in case of an error.
 * 2) For its arguments user_bcopy2 takes the usual "from", and "to"
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

/*
 * int user_bcopy2(from, to, &count)
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

