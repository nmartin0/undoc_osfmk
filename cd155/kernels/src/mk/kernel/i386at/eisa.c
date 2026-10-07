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
 * $Log: eisa.c,v $
 * Revision 1.5  1994/11/18  20:35:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:26:23  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:25:09  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:24:55  dleslie
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
 * Eisa Bus handling
 */


#include <i386at/eisa.h>
extern unsigned char inb();

boolean_t is_eisa_bus = FALSE;

boolean_t
is_eisa_board(board, bid)
eisa_board_id_t *bid;
{		
	register i;

	outb(EISA_ID_REG(board, EISA_ID_REG_0), 0xff);
	if(inb(EISA_ID_REG(board, EISA_ID_REG_0)) & 0x80)
		return(FALSE);

	for (i=0; i<4; i++)
		bid->byte[3-i] = inb(EISA_ID_REG(board, i));
	return(TRUE);
}
	

probe_eisa()
{
	eisa_board_id_t bid;

	/* probe system board */

	if (is_eisa_bus = is_eisa_board(EISA_SYSTEM_BOARD, &bid))
	  printf("Eisa system board: %c%c%c (0x%x) eisa bus version 0x%x\n",
		 bid.id.name_char_0+0x40,
		 bid.id.name_char_1+0x40,
		 bid.id.name_char_2+0x40,
		 bid.id.bd_id.sys_id.reserved,
		 bid.id.bd_id.sys_id.bus_vers);
}





