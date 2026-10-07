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
 * HISTORY
 * $Log: dram.h,v $
 * Revision 0.5  1994/11/18  20:42:18  mtm
 * Copyright additions/changes
 *
 * Revision 0.4  1994/01/28  17:30:17  steved
 * Removed the old DRAM_HIGH_ALIAS definition which was enclosed within an
 * #if 0/#endif block. The old value was "0x08000000"
 *
 * Revision 0.3  1993/12/14  23:00:02  steved
 * Fixed a multitude of problems using the real MP3 nodeboard. Added
 * new led routines to blink the additional LED's on the MP3 board.
 * Cleaned up some of the constants and removed those which are
 * not used on the GP nor MP3 boards.
 *
 * Revision 0.2  1993/06/30  22:37:07  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.1  1992/07/09  17:00:25  andyp
 * Initial checkin of Paragon files.
 *
 *
 */

/*
 *	DRAM Address Space
 *
 *	DRAM address space accesses on-board DRAM.
 *
 *	Local bus space is accessed when the most significant four bits of
 *	the address are equal to 0xC, 0xD, 0xE, or 0xF.
 *
 *	Physical memory is aliased (modulo memory size) from 0xC0000000
 *	to the top of physical address space.
 *
 *	Address bits <29:28> are used to select cache-snoop cycle behaviour:
 *
 *		0xc0000000 to 0xcfffffff: snoop neither
 *		0xd0000000 to 0xdfffffff: snoop cpu1
 *		0xe0000000 to 0xefffffff: snoop cpu0
 *		0xf0000000 to 0xffffffff: snoop both
 */

#define	DRAM_BASE		(0xc0000000)
#define DRAM_SNOOP_CPU0		(1 << 29)
#define DRAM_SNOOP_CPU1		(1 << 28)

/*
 * 	Define an alias for mapping the kernel into RAM. With the older trap
 *	hanlder, this ALIAS was defined as "0x08000000" to map the kernel
 *	into address 0xf8000000. This was done because the old trap handler
 *	was limited to 128Meg of where it could transfer control. With the
 *	new trap handler, the 128Meg limitation went away. In addition,
 *	given that the MP3 board can have 128Meg of RAM on the baseboard,
 *	the only way to fit the RAM, the kernel virtual address space and
 *	the trap page was to "move" the kernel down to 0xF0000000.
 */
#define DRAM_HIGH_ALIAS		(0x00000000)	/* Requires new trap hdlr */
