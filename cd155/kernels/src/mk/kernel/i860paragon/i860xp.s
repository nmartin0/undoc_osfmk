/*
 *    
 *    $Copyright
 *    Copyright 1991 , 1994, 1995 Intel Corporation
 *    INTEL CONFIDENTIAL
 *    The technical data and computer software contained herein are subject
 *    to the copyright notices; trademarks; and use and disclosure
 *    restrictions identified in the file located in /etc/copyright on
 *    this system.
 *    Copyright$
 *    
 */
 
/*
 * Copyright 1992 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * $Id: i860xp.s,v 0.6 1994/11/18 20:42:25 mtm Exp $
 *
 * HISTORY
 * $Log: i860xp.s,v $
 *    Revision 0.6  1994/11/18  20:42:25  mtm
 *    Copyright additions/changes
 *
 *    Revision 0.5  1993/06/30  22:37:17  dleslie
 *    Adding copyright notices required by legal folks
 *
 *    Revision 0.4  1992/10/14  22:25:40  andyp
 *    Added a routine to return the bear register.
 *
 *    Revision 0.3  1992/10/02  00:56:24  stans
 *    added "get_pregs" which returns processor protected registers images.
 *    used by kdb "show pregs" command.
 *
 * Revision 0.2  1992/09/14  16:26:12  andyp
 * Corrected the log message and corrected the comment leader.
 *
 * Revision 0.1  1992/07/09  17:00:22  andyp
 * Initial checkin of Paragon files.
 *
 */
	.file	"i860xp.s"
	.text
	.align	16

/*	andyp@ssd.intel.com
 *
 *	The routines in this file will only work on i860XP chips
 *	in supervisor mode.
 *	On an i860XR, they will (or should) cause an instruction fault.
 */
	//
	//	void scyc_b(type)
	//	int	type;
	//
	//	void scyc_s(type)
	//	int	type;
	//
	//	void scyc_l(type)
	//	int	type;
	//
_scyc_b::
	bri	r1
	 scyc.b	r16
_scyc_s::
	bri	r1
	 scyc.s	r16
_scyc_l::
	bri	r1
	 scyc.l	r16

	//
	//	void outb(addr, byte)
	//	unsigned char *addr, byte;
	//
	//	void outw(addr, word)
	//	unsigned short *addr, word;
	//
	//	void outl(addr, word)
	//	unsigned int *addr, word;
	//
_outb::
	bri	r1
	 stio.b	r17,r16
_outw::
	bri	r1
	 stio.s	r17,r16
_outl::
	bri	r1
	 stio.l	r17,r16

	//
	//	unsigned char inb(addr)
	//	unsigned char *addr;
	//
	//	unsigned short inw(addr)
	//	unsigned short *addr;
	//
	//	unsigned int inl(addr)
	//	unsigned int *addr;
	//
_inb::
	ldio.b	r16,r17
	bri	r1
	 and	0xff,r17,r16		// get rid of sign-extension
_inw::
	ldio.s	r16,r17
	bri	r1
	 and	0xffff,r17,r16		// get rid of sign-extension
_inl::
	ldio.l	r16,r17
	bri	r1
	 mov	r17,r16

/*
 * return processor protected registers p0..p3
 *
 * get_pregs( &foo )
 *	unsigned 	foo[4];
 *
 */
_get_pregs::
	ld.c	p0,r17
	st.l	r17,0(r16)
	ld.c	p1,r17
	st.l	r17,4(r16)
	ld.c	p2,r17
	st.l	r17,8(r16)
	ld.c	p3,r17
	bri	r1
	  st.l	r17,12(r16)

_get_bear::
	bri	r1
	 ld.c	bear,r16
