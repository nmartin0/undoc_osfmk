/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: sbrk.c,v $
 * Revision 1.6  1994/11/18  21:05:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  23:04:36  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/10  20:04:42  stans
 *    Removed extra #include's which were escaping off into '/usr/include'.
 *
 * Revision 1.3  1993/04/27  20:58:55  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:05:00  dleslie
 * First R1_0 release
 *
 * Revision 2.3.2.1  92/03/28  10:34:23  jeffreyh
 * 	Changes for i860
 * 	[92/03/26  11:28:29  jeffreyh]
 * 
 * 	Changes from MK71
 * 	[92/03/11  18:35:08  jeffreyh]
 * 
 * Revision 2.3  92/02/23  19:47:44  elf
 * 	Added luna88k goop.
 * 	[92/02/20            danner]
 * 
 * Revision 2.2  92/01/16  00:01:23  rpd
 * 	Moved from user collection to mk collection.
 * 
 * Revision 2.2  91/03/26  17:46:40  mrt
 * 	First checkin
 * 
 */
/*
 *	File:	sbrk.c
 *	Author: Avadis Tevanian, Carnegie Mellon University
 *	Date:	June 1986
 *
 *	Unix compatibility for sbrk system call.
 */

#define  EXPORT_BOOLEAN

typedef char	*caddr_t;	/* address of a (signed) char */

#include <mach_init.h>		/* for vm_page_size */

#if defined(luna88k)
asm("		global _curbrk");
asm("		global _minbrk");
asm("		global _end");
asm("		data");
asm("_minbrk:	word _end");
asm("_curbrk:	word _end");
#endif

#if	(defined(vax) || defined(ibmrt) || defined(ns32000) || defined(sun) || defined(i386)) || defined(i860)
#if	defined(multimax) || defined(i860)
DEF_FUNC()
{
#endif	multimax
asm(".data");
asm(".globl	curbrk");
asm(".globl	minbrk");
asm(".globl	_curbrk");
asm(".globl	_minbrk");
asm(".globl	_end");
#if	defined(multimax)
asm("_minbrk:");
asm("minbrk:	.double	_end");
asm("_curbrk:");
asm("curbrk:	.double	_end");
asm(".text");
}
#else	multimax
asm("_minbrk:");
asm("minbrk:	.long	_end");
asm("_curbrk:");
asm("curbrk:	.long	_end");
asm(".text");
#ifdef i860
}
#endif i860
#endif	multimax
#else	(defined(vax) || defined(ibmrt) || defined(ns32000) || defined(sun) || defined (i386) || defined (i860)

/* Will not find get "assembler" forms of cubrk, minbrk. */

#ifdef	mips
extern char end;
#define curbrk _curbrk
#define minbrk _minbrk
caddr_t curbrk = &end;
caddr_t minbrk = &end;
#endif	mips

#endif	(defined(vax) || defined(ibmrt) || defined(ns32000) || defined(sun) || defined(i386) 

#ifdef lint
   /* lint doesn't see asm stuff */
caddr_t	curbrk;
caddr_t	minbrk;
#else lint
extern caddr_t curbrk;
extern caddr_t minbrk;
#endif lint

#define	roundup(a,b)	((((a) + (b) - 1) / (b)) * (b))

static int sbrk_needs_init = FALSE;

caddr_t sbrk(size)
	int	size;
{
	vm_offset_t	addr;
	kern_return_t	ret;
	caddr_t		ocurbrk;

	if (sbrk_needs_init) {
		sbrk_needs_init = FALSE;
		/*
		 *	Get "curbrk"
		 */

	}
	
	if (size <= 0)
		return(curbrk);
	addr = (vm_offset_t) roundup((int)curbrk,vm_page_size);
	ocurbrk = curbrk;
	if (((int)curbrk+size) > addr)
	{	ret = vm_allocate(mach_task_self(), &addr, 
			    (vm_size_t) size -((int)addr-(int)curbrk), FALSE);
		if (ret == KERN_NO_SPACE) {
			ret = vm_allocate(mach_task_self(), &addr, (vm_size_t) size, TRUE);
			ocurbrk = (caddr_t)addr;
		}
		if (ret != KERN_SUCCESS) 
			return((caddr_t) -1);
	}

	curbrk = (caddr_t)ocurbrk + size;
	return(ocurbrk);

}

void brk(x)
	caddr_t x;
{
	char	*msg="brk() not implemented\n";

	/*
         * output to stderr, 2 is hardcoded as we do not want to drag in
	 * stdio.h
	 */
	write(2, msg, strlen(msg));
}
