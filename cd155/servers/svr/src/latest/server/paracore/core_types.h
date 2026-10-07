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
 * Copyright 1994 , 1995 Intel Corporation
 * All rights reserved
 *
 * $Id: core_types.h,v 1.4 1994/11/18 20:38:01 mtm Exp $
 *
 * HISTORY
 * $Log: core_types.h,v $
 * Revision 1.4  1994/11/18  20:38:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/07/27  14:47:17  johannes
 * changed header
 *
 */

#ifndef	_CORE_TYPES_H_
#define	_CORE_TYPES_H_

#include <uxkern/bsd_types.h>

/*
#define COREDEBUG
*/

#ifdef COREDEBUG
extern int	core_debug;
#define CORE_DEBUGF	if ( core_debug ) printf
#else /* COREDEBUG */
#define CORE_DEBUGF	if ( 0 )
#endif /* COREDEBUG */


/*
 * Defines for lock operations.
 */
#define CORE_LOCK(l)		usimple_lock(&(l))
#define CORE_UNLOCK(l)		usimple_unlock(&(l))
#define CORE_LOCK_INIT(l)	usimple_lock_init(&(l))


/*
 * Defines core actions.
 */
#define	CORE_ACTION_CONT	0
#define	CORE_ACTION_KILL	1
#define	CORE_ACTION_TRACE	2
#define	CORE_ACTION_FULL	3


/*
 * Structure pvp_core_data is allocated for a NX proxy process
 * on behalf of nx_initve() and a pointer to this data is kept in
 * the pvproc structure of the proxy.
 */
typedef struct {
	struct utask_nd utnd;			/* from proxy on nxinitve */
	struct mutex	core_lock;		/* core lock */
	boolean_t	core_dumped;		/* there was >= 1 dump */
	mach_port_t	core_directory;		/* core directory */
	int             core_action_first;	/* action of first faulting */
	int             core_action_fault;	/* action of faulting process */
	int		core_action_other;	/* action of other processes */
	path_name_t	env_core_path;		/* saved CORE_PATH */
} pvp_core_data_t;


/*
 * Lock definitions for pvp_core_data.core_lock.
 */
#define VPROC_CORE_LOCK_INIT(v) do {					\
		pvp_lock_init(&PVP((v))->pvp_core_data->core_lock);	\
		} while(0)

#define VPROC_CORE_LOCK_EXCL(v,s) do {					\
		VPROC_LOCK_DEBUG(v,s,"lock core lock");			\
		pvp_lock_lock(&PVP((v))->pvp_core_data->core_lock);	\
		} while(0)

#define VPROC_CORE_UNLOCK_EXCL(v,s) do {				\
		VPROC_LOCK_DEBUG(v,s,"unlock core lock");		\
		pvp_lock_unlock(&PVP((v))->pvp_core_data->core_lock);	\
		} while(0)

#ifdef NX
	/*
	 * This is a copy from /usr/include/allocsys.h
	 * because nx_part_info_t is not defined in the server.
	 */
typedef struct {
        uid_t           uid;            /* User Id */
        gid_t           gid;            /* Group Id */
        int             access;         /* Access Permissions */
        int             sched;          /* NX_STD or NX_GANG */
        unsigned long   rq;             /* Rollin Quantum */
        int             epl;         	/* Effective priority limit */
        int             nodes;          /* Number of nodes in the partition */
	/* NOTES: mesh_x and mesh_y are only set if the mesh is a contiguous
	 * rectangle. Otherwise the are -1.
	*/
	int		mesh_x;		/* X dimension of partition */
	int		mesh_y;		/* Y dimension of partition */
	/* The enclose_mesh_x and enclose_mesh_y are the
	 * minimum rectangular dimensions that will enclose
	 * the partition. These dimensions may contain nodes enclose
	 * nodes that are not part of the specified partition
	*/
	int		enclose_mesh_x;
	int		enclose_mesh_y;
	int		flags_or_size;	/* Internal Use only */
	int		part_id;	/* Internal Use only */
	int		free;		/* Internal Use only */   
	int		reserved[7];
} nx_part_info_t;
#endif /* NX */

#endif	/* _CORE_TYPES_H_ */
