/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * HISTORY
 * $Log: sll.h,v $
 * Revision 1.6  1994/11/18  20:38:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/10  14:19:55  stefan
 * Reviewer: none
 * Risk: low
 * Benefit or PTS #: clean-ups to pass lint
 * Testing: built
 * Module(s): sll.c, sll.h sll_types.h
 *
 * Some minor clean-ups to eliminate warnings from lint.
 *
 * Revision 1.4  1993/06/02  07:17:38  stefan
 * Moved sys/sll.h to sll/sll.h (after renaming sll/sll.h to sll/sll_types.h).
 *
 * Revision 1.3  1993/05/27  13:26:15  stefan
 * Added missing #endif.
 *
 * Revision 1.2  1993/05/27  13:13:57  stefan
 * Removed a typo in the comment section.
 *
 * Revision 1.1  1993/05/27  13:04:11  stefan
 * New user include file containing prototype and defines for fork_remote_ctl()
 * system call.
 *
 */

#ifndef	_SLL_H_
#define	_SLL_H_

/*
 * Defines for fork_remote_ctl system call.
 */
#define ENABLE_FORK_REMOTE	1
#define DISABLE_FORK_REMOTE	0

#ifndef KERNEL
/*
 * Prototype for fork_remote_ctl system call.
 */
#ifndef _NO_PROTO
extern int	fork_remote_ctl(int flag);
#else /* _NO_PROTO */
extern int	fork_remote_ctl();
#endif /* _NO_PROTO */
#endif /* KERNEL */

#endif	/* _SLL_H_ */
