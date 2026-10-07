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
GP Node FLASH boot block code
Copywrite Intel Corporation, 1991
Revision 0.  3/28/91
////////////////////////////////////////////////////////////////////////
//                                                                     //
//             INTEL CORPORATION PROPRIETARY  INFORMATION              //
//                                                                     //
//     This software is supplied under  the  terms  of  a  licence     //
//     agreement    or   non-disclosure   agreement   with   Intel     //
//     Corporation and  may not be copied  nor disclosed except in     //
//     accordance with the terms of that agreement.                    //
//                                                                     //
////////////////////////////////////////////////////////////////////////

// These routines implement the N11 i/o instructions
*/

outl(addr, l_data)
unsigned long addr, l_data;
	{
	asm(".long 0x4e008c09"); /* stio.l r17,(0)r16  */
	}

outw(addr, s_data)
unsigned long addr;
unsigned short s_data;
	{
	asm(".long 0x4e008a09"); /* stio.s r17,(0)r16  */
	}

outb(addr, b_data)
unsigned long addr;
unsigned char b_data;
	{
	asm(".long 0x4e008809"); /* stio.b r17,(0)r16  */
	}

unsigned long inl(addr)
unsigned addr;
	{
	asm(".long 0x4e100408"); /* ldio.l (0)r16,r16  */
	}

unsigned short inw(addr)
unsigned addr;
	{
	asm(".long 0x4e100208"); /* ldio.s (0)r16,r16  */
	asm("shl 16, r16, r16");
	asm("shr 16, r16, r16");
	}

unsigned char inb(addr)
unsigned addr;
	{
	asm(".long 0x4e100008"); /* ldio.b (0)r16,r16  */
	asm("shl 24, r16, r16");
	asm("shr 24, r16, r16");
	}
