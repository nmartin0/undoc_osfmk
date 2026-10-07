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
 * $Log: ltu.h,v $
 * Revision 0.3  1994/11/18  20:42:39  mtm
 * Copyright additions/changes
 *
 * Revision 0.2  1993/06/30  22:37:25  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.1  1992/07/09  16:43:39  andyp
 * Initial checkin of Paragon files.
 *
 *
 */

/*
 *	LTU Address Space
 *
 *	LTU address space initiates line transfers between memory and
 *	the NIC.
 *
 *	This space is accessed when the most significant four
 *	bits of the address are between 0 and 3 inclusive.
 *
 *	LTU space does not support burst transfers, therefore all accesses
 *	in LTU space must be non-cached or I/O.
 */

/*
 *	The read LTU is enabled by writing the lower 32 bits of data
 *	and the write LTU is enabled by writing the upper 32 bits of data.
 *	The read LTU transfers from the NIC to memory; the write LTU
 *	transfers data to the NIC from memory.
 *
 *	Data bit <61> specifies a terminating transfer to the NIC and
 *	sets the EOD bit.  The line count is specified by data bits
 *	<47:32> and <15:0>.  The starting line address is specified by
 *	address bits <29:4>.  Address bits <30:29> specify which of the
 *	two processors should be snooped during the LTU transfer.
 *	Address bit <30> when set specifies the system processor
 *	and address bit <29> specifies the user processor.
 *
 *	The address and line count can be read by DP ASIC accesses.
 */
