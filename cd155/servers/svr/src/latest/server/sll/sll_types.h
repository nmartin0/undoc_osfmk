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
 * $Log: sll_types.h,v $
 * Revision 1.6  1994/11/18  20:38:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/10  14:20:08  stefan
 * Reviewer: none
 * Risk: low
 * Benefit or PTS #: clean-ups to pass lint
 * Testing: built
 * Module(s): sll.c, sll.h sll_types.h
 *
 * Some minor clean-ups to eliminate warnings from lint.
 *
 * Revision 1.4  1993/10/01  16:19:17  stefan
 * Added prototype for sll_check_bm_fork_remote_timeout().
 *
 * Revision 1.3  1993/09/29  21:04:55  jlitvin
 * fast_node_timeout is actually an int, not a long.  This bothers GCC.
 *
 * Revision 1.2  1993/06/23  18:16:23  stefan
 * For better modularity added function to return fast_node.
 * Added function to check for valid node number for fast_node.
 *
 * Revision 1.1  1993/06/01  15:42:05  stefan
 * Renamed sll/sll.h to sll/sll_types.h.
 *
 * Revision 1.3  1993/05/27  17:23:24  stefan
 * Completed copyright notice.
 *
 * Revision 1.2  1993/05/24  16:25:10  stefan
 * Changed default value for boot magic ENABLE_FORK_REMOTE to TRUE.
 *
 * Revision 1.1  1993/05/13  09:10:31  stefan
 * Integrated static load leveling support.
 *
 */

#ifndef	_SLL_TYPES_H_
#define	_SLL_TYPES_H_

#ifdef SLLDEBUG
extern int	sll_debug;

#define SLL_DEBUGF	if ( sll_debug ) printf
#else /* SLLDEBUG */
#define SLL_DEBUGF	if ( 0 )
#endif /* SLLDEBUG */

/*
 * Defines for boot magic variable names.
 */
#define BM_ENABLE_FORK_REMOTE	"ENABLE_FORK_REMOTE"
#define BM_FORK_REMOTE_TIMEOUT	"FORK_REMOTE_TIMEOUT"
#define BM_FORK_REMOTE_SHM	"FORK_REMOTE_SHM"

/*
 * Defines for default values for boot magic variables.
 */
#define	DEF_ENABLE_FORK_REMOTE	TRUE
#define DEF_FORK_REMOTE_TIMEOUT	60
#define DEF_FORK_REMOTE_SHM	FALSE

/*
 * Defines for lock operations.
 */
#define SLL_LOCK(l)		usimple_lock(&l)
#define SLL_UNLOCK(l)		usimple_unlock(&l)
#define SLL_LOCK_INIT(l)	usimple_lock_init(&l)

/*
 * External variables.
 */
udecl_simple_lock_data(extern, sll_lock)	/* sll lock */
extern boolean_t	fork_remote;		/* ENABLE_FORK_REMOTE */
extern unsigned long	fork_remote_timeout;	/* FORK_REMOTE_TIMEOUT */
extern boolean_t	fork_remote_shm;	/* FORK_REMOTE_SHM */
extern int		fork_remote_enabled;	/* set by fork_remote_ctl */
extern node_t		fast_node;		/* most lightly loaded node */
extern long		fast_node_timeout;	/* time-out after that */

/*
 * External functions.
 */
extern int		fork_remote_ctl();
extern void		sll_init();
extern boolean_t	sll_check_bm_fork_remote_timeout();
extern int		sll_timeout();
extern void		sll_set_timeout();
extern int		sll_check_fast_node();
extern boolean_t	sll_has_shm();
extern node_t		sll_fast_node();
#endif	/* _SLL_TYPES_H_ */
