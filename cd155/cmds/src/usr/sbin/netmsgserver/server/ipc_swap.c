/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * ipc_swap.c 
 *
 *
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ipc_swap.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:11:14 $";
#endif
/*
 * Byte-swapping procedures for the IPC module. 
 */


#include	<stdio.h>

#include	"ipc_swap.h"
#include	"netmsg.h"
#include	"nm_extra.h"
#include	"sbuf.h"


/*
 * swap_long_sbuf -- 
 *
 * Copy and byte-swap several long words from an sbuf into a continuous buffer. 
 *
 * Parameters: 
 *
 * sb_ptr: pointer to the source sbuf. 
 *
 * pos_ptr: pointer to the sbuf_pos structure specifying the starting point. 
 *
 * to_ptr: pointer to the pointer specifying the destination. 
 *
 * count: number of words to copy. 
 *
 * Results: 
 *
 * pos_ptr ans to_ptr updated. 
 *
 * Side effects: 
 *
 * Performs the specified copy and byte-swap. May cause a panic if the sbuf does
 * not contain enough data. 
 *
 * Note: 
 *
 */
PUBLIC void
swap_long_sbuf(IN sb_ptr, INOUT pos_ptr, INOUT to_ptr, IN count)
	sbuf_ptr_t	sb_ptr;
	sbuf_pos_t	*pos_ptr;
	unsigned long	**to_ptr;
	int		count;
BEGIN("swap_long_sbuf")
	register int		cur_left;
	int			end_bytes;
	register unsigned long	*f;
	register unsigned long	*t;
SWAP_DECLS;

t = *to_ptr;

/*
 * Iterate over several sbuf segments. 
 */
cur_left = pos_ptr->data_left >> 2;
f = (unsigned long *) pos_ptr->data_ptr;
while (count > cur_left) {
	/*
	 * Copy as many full words as possible from the current segment. 
	 */
	count -= cur_left;
	end_bytes = pos_ptr->data_left - (cur_left << 2);
	for (; cur_left; cur_left--, f++, t++) {
		(void) SWAP_LONG(*f, *t);
	}

	/*
	 * Cross over to the next segment. 
	 */
	pos_ptr->seg_ptr++;
	if (count && (pos_ptr->seg_ptr == sb_ptr->end))
		panic("swap_long_sbuf");
	if (end_bytes) {
		register char  *fc = (char *) f;
		register char  *tc = (char *) t;
		register int    cnt = end_bytes;

		for (; cnt; cnt--, fc++, tc++)
			*tc = *fc;
		fc = (char *) pos_ptr->seg_ptr->p;
		cnt = 4 - end_bytes;
		pos_ptr->data_left = pos_ptr->seg_ptr->s - cnt;
		for (; cnt; cnt--, fc++, tc++)
			*tc = *fc;
		(void) SWAP_LONG(*t, *t);
		t++;
		count--;
		f = (unsigned long *) fc;
	} else {
		pos_ptr->data_left = pos_ptr->seg_ptr->s;
		f = (unsigned long *) pos_ptr->seg_ptr->p;
	}

	cur_left = pos_ptr->data_left >> 2;
}

/*
 * Get the remaining words from the beginning of the current segment. 
 */
if (count) {
	pos_ptr->data_left -= count << 2;
	for (; count; count--, f++, t++) {
		(void) SWAP_LONG(*f, *t);
	}
}
pos_ptr->data_ptr = (pointer_t) f;
*to_ptr = t;

RET;
END



/*
 * swap_short_sbuf -- 
 *
 * Copy and byte-swap several short words from an sbuf into a continuous buffer. 
 *
 * Parameters: 
 *
 * sb_ptr: pointer to the source sbuf. 
 *
 * pos_ptr: pointer to the sbuf_pos structure specifying the starting point. 
 *
 * to_ptr: pointer to the pointer specifying the destination. 
 *
 * count: number of words to copy. 
 *
 * Results: 
 *
 * pos_ptr ans to_ptr updated. 
 *
 * Side effects: 
 *
 * Performs the specified copy and byte-swap. May cause a panic if the sbuf does
 * not contain enough data. 
 *
 * Note: 
 *
 */
PUBLIC void
swap_short_sbuf(IN sb_ptr, INOUT pos_ptr, INOUT to_ptr, IN count)
	sbuf_ptr_t	sb_ptr;
	sbuf_pos_t	*pos_ptr;
	unsigned short	**to_ptr;
	int		count;
BEGIN("swap_short_sbuf")
	register int		cur_left;
	int			end_bytes;
	register unsigned short	*f;
	register unsigned short	*t;
SWAP_DECLS;

t = *to_ptr;

/*
 * Iterate over several sbuf segments. 
 */
cur_left = pos_ptr->data_left >> 1;
f = (unsigned short *) pos_ptr->data_ptr;
while (count > cur_left) {
	/*
	 * Copy as many short words as possible from the current segment. 
	 */
	count -= cur_left;
	end_bytes = pos_ptr->data_left - (cur_left << 1);
	for (; cur_left; cur_left--, f++, t++) {
		(void) SWAP_SHORT(*f, *t);
	}

	/*
	 * Cross over to the next segment. 
	 */
	pos_ptr->seg_ptr++;
	if (count && (pos_ptr->seg_ptr == sb_ptr->end))
		panic("swap_short_sbuf");
	if (end_bytes) {
		register char  *fc = (char *) f;
		register char  *tc = (char *) t;
		*tc = *fc;
		tc++;
		fc = (char *) pos_ptr->seg_ptr->p;
		*tc = *fc;
		fc++;
		(void) SWAP_SHORT(*t, *t);
		t++;
		count--;
		pos_ptr->data_left = pos_ptr->seg_ptr->s - 1;
		f = (unsigned short *) fc;
	} else {
		pos_ptr->data_left = pos_ptr->seg_ptr->s;
		f = (unsigned short *) pos_ptr->seg_ptr->p;
	}

	cur_left = pos_ptr->data_left >> 1;
}

/*
 * Get the remaining words from the beginning of the current segment. 
 */
if (count) {
	pos_ptr->data_left -= count << 1;
	for (; count; count--, f++, t++) {
		(void) SWAP_SHORT(*f, *t);
	}
}
pos_ptr->data_ptr = (pointer_t) f;
*to_ptr = t;

RET;
END
