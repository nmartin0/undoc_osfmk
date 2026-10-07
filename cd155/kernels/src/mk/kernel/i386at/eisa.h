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
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 * HISTORY
 * $Log: eisa.h,v $
 * Revision 1.5  1994/11/18  20:35:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:26:25  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:25:11  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:24:57  dleslie
 * First R1_0 release
 *

/*
 * Copyright 1992 by Open Software Foundation,
 * Grenoble, FRANCE
 *
 * 		All Rights Reserved
 * 
 *   Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OSF or Open Software
 * Foundation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 *   OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Eisa defs
 */

#ifndef _I386AT_EISA_H_
#define _I386AT_EISA_H_

#include <eisa.h>
#include <mach/boolean.h>

#if EISA
extern boolean_t is_eisa_bus;

#define EISA_ID_REG(board, byte)	(0xc80 | (byte) | ((board) << 12))

#define EISA_ID_REG_0	0x0
#define EISA_ID_REG_1	0x1
#define EISA_ID_REG_2	0x2
#define EISA_ID_REG_3	0x3

#define EISA_SYSTEM_BOARD 0x0

struct std_board_id {
	unsigned revision:	8,	/* Revision number */
		 product:	8;	/* Product number */ 
};

struct sys_board_id {
	unsigned bus_vers:	3,	/* EISA bus version */
		 reserved:	13;	/* Manufacturer reserved */
};

struct board_id {
	union {
		struct sys_board_id sys_id;
		struct std_board_id std_id;
	} bd_id;
	unsigned name_char_2: 	5,	/* 3nd compressed char */
      		 name_char_1: 	5,	/* 2nd compressed char */
      		 name_char_0: 	5,	/* 1st compressed char */
		 not_eisa:    	1;	/* 0 if eisa board */
};

union eisa_board_id {
	unsigned char byte[4];
	struct board_id id;
};

typedef union eisa_board_id eisa_board_id_t;


/* Additional DMA registers */

#define	DMA0HIPAGE	0x481		/* DMA 0 address: bits 24-31 */
#define	DMA0HICNT	0x405		/* DMA 0 count: bits 16-23 */


#else EISA
#define is_eisa_bus FALSE
#define probe_eisa()
#endif EISA

#endif _I386AT_EISA_H_
