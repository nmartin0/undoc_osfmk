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
 * $Log: spl.h,v $
 * Revision 0.15  1994/11/18  20:43:15  mtm
 * Copyright additions/changes
 *
 * Revision 0.14  1994/03/19  01:12:30  lenb
 * Benefit: move SPLCPU to SPL6; lint.
 * Testing: SAT
 * Reviewer: sean
 *
 * Revision 0.13  1993/12/21  17:36:43  steved
 * Duplication of the dp_ipl data structure definition caused the kernel
 * to freeze when the message coprocessor was turned on. This structure is
 * now defined in spl.h
 *
 * Revision 0.12  1993/12/16  01:54:39  steved
 * Fixed some problems due to merging with the main trunk. The main problem was
 * a coding error in spl.c with respect to more than 1 CPU.
 *
 * Revision 0.11  1993/12/14  23:00:33  steved
 * Fixed a multitude of problems using the real MP3 nodeboard. Added
 * new led routines to blink the additional LED's on the MP3 board.
 * Cleaned up some of the constants and removed those which are
 * not used on the GP nor MP3 boards.
 *
 * Revision 0.10  1993/10/01  16:54:43  steved
 * Support for MP3
 *
 * Revision 0.9  1993/07/16  18:54:04  steved
 * Dropped message passing SPL to 4 to help debugging. Implemented a 'spinout'
 * in the delay() function to prevent delays from lasting to long. Fixed some
 * last minute bugs in the fast scan routines.
 *
 * Revision 0.8  1993/06/30  22:37:54  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.7  1993/06/18  17:38:59  stans
 *    Added ifdef around entire file to protect against duplicate includes.
 *
 * Revision 0.6  1992/12/17  00:23:21  regnier
 * Jerrie's LTU support.
 *
 * Revision 0.5  1992/12/02  17:39:07  terry
 * changed SPLNET to 5
 *
 * Revision 0.4  1992/12/01  19:14:27  stans
 * added SPLTIMER1 SPL6 as CPU1 clock.
 *
 * Revision 0.3  1992/10/29  02:27:45  stans
 * SPLCPU is now alive and well at SPL4
 *
 * Revision 0.2  1992/10/23  15:10:04  prp
 * Patch nested interrupt problem
 * Fix LED display
 * Automate SPL configuration
 * Disable scan except to break in for debugging
 *
 * Revision 0.1  1992/07/09  17:00:36  andyp
 * Initial checkin of Paragon files.
 *
 *
 */

#ifndef	_I860PARAGON_SPL_H_
#define	_I860PARAGON_SPL_H_ 1

#define	SPL0	0
#define SPL1	1
#define SPL2	2
#define SPL3	3
#define SPL4	4
#define SPL5	5
#define SPL6	6
#define SPL7	7

#define NSPL	8

#define SPLNEVER	SPL0
#define SPLALWAYS	NSPL

#define SPLHWERR	SPLALWAYS
#define SPLHI		SPL6
#define SPLIMP		SPL6
#define SPLSCHED	SPL6
#define SPLVM		SPL6
#define SPLCPU		SPL6	/* cross-processor doorbell */
#define SPLSCAN		SPL5	/* Scan Interface */
#define SPLTTY		SPL5	/* Console TTY */
#define SPLMSG		SPL4	/* Message Passing */
#define SPLCLOCK	SPL4	/* CPU0 hardware clock */
#define	SPLBIO		SPL3
#define SPLSOFTCLOCK	SPL1
#define SPLLTU		SPLNEVER


/*
 *	Generally, we need to enable/disable interrupts from a
 *	specific interrupt source (at a specific interrupt level)
 *	to a specific cpu.  We usually know which cpu and which
 *	interrupt source, but the layout of the bits within the
 *	dp asic interrupt mask register do not lead to a straightforward
 *	algorithm of shifts and shuffles to easily compute an
 *	address and a bit position.  Instead, a matrix, indexed
 *	by interrupt source and by target cpu is used.
 *
 *	Each element of the matrix is a tuple.  The first member is
 *	the word offset from the base I/O address of the control register.
 *	The second member is the bit position within the word.
 *
 *	Maintaining a "software pic" that, for a given ipl, yields
 *	a bit vector of interrupt sources that must be masked and/or
 *	enabled suddenly becomes easier.
 */
/*
 *	Interrupt sources
 *
 *	These defininitions are *not* bits within the dp asic, but
 *	they are "y" index into the matrix.
 */
#define	SPL_IMASK_NIC		0	/* the NIC */
#define SPL_IMASK_EXP		1	/* an expansion card */
#define SPL_IMASK_NET		2	/* the net (MRC maybe?) */
#define SPL_IMASK_CPU0		3	/* cpu0 (aka sys) */
#define SPL_IMASK_TIMER0	4	/* lo dp os timer */
#define SPL_IMASK_LTU0		5	/* lo dp ltu */
#define SPL_IMASK_HWERR0	6	/* lo dp hardware error */
#define SPL_IMASK_CPU1		7	/* cpu1 (aka usr) */
#define SPL_IMASK_TIMER1	8	/* hi dp os timer */
#define SPL_IMASK_LTU1		9	/* hi dp ltu */
#define SPL_IMASK_HWERR1	10	/* hi dp hardware error */
#define SPL_IMASK_CPU2		11	/* cpu2 (aka usr2) */
#define SPL_IMASK_TIMER2	12	/* vdp os timer */
#define N_INTRS			13

/*
 *	Matrix elements (a tuple)
 */
struct dp_imask_tab {
	unsigned long	dp_mask_off;	/* offset from base address */
	unsigned long	dp_mask_bit;	/* bit position in the register */
};
struct dp_ipl {
        unsigned long   dp0_set;
        unsigned long   dp0_clr;
        unsigned long   dp1_set;
        unsigned long   dp1_clr;
        unsigned long   vdp_set;
        unsigned long   vdp_clr;
};

extern int	sploff();
extern void	splon(int);
extern int	spl0(void);
extern int	splbio(void);
extern int	splnet(void);
extern int	splclock(void);
extern int	splvm(void);
extern int	splmsg(void);
extern int	splhigh(void);
extern int	splhi(void);
extern int	splimp(void);
extern int	splsched(void);
extern int	splsoftclock(void);
extern int	spltty(void);
extern int	get_spl(void);
#endif	/* _I860PARAGON_SPL_H_ */
