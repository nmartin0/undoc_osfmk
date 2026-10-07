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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992,1993  Intel Corporation.
 *
 */
/*
 * HISTORY
 * $Log: fast_node.c,v $
 * Revision 1.3  1994/11/19  03:07:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/06/21  10:46:29  stefan
 * Modified the load leveling algorithm to do round robin scheduling of jobs
 * unless the load of next node to be selected is at least by MIN_LOAD_DELTA
 * (1.0) higher than the load of the most lightly loaded node (which is
 * selected in this case).
 *
 * Revision 1.1  1993/02/23  10:19:02  stefan
 * inetd now skips ROOT_FS_NODE instead of logical node 0. This fixes
 * bug #4253. ROOT_FS_NODE is obtained from the mikrokernel using code
 * similar to getmagic (actually lots of this code is copied from getmagic).
 * Also, the sources have been restructured: the functions select_netxt_node()
 * and get_node_array() have been moved to a seperate file next_node.c, the file
 * boot_env.c contains the functions getbootenv(), convert_to_internal() and
 * get_boot_magic() for obtaining a boot magic (i.e. ROOT_FS_NODE) from the
 * microkernel and the file fast_node.c now contains the functions
 * get_fastest_node() and get_fastest_node_from_shm() for accessing the load
 * information from the load leveling daemon.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/errno.h>
#include <syslog.h>
#include "load_level_com.h"

#ifdef SKIP_ROOT_FS_NODE
extern int	root_fs_node;
#endif /* SKIP_ROOT_FS_NODE */

extern int	debug;

extern int	node_self();

extern char *key_dir = { KEY_DIR };

#define PREPOSTEROUS_LOAD 999999.99
#define MIN_LOAD_DELTA 1.0


/*
 * Synopsis:	int get_fastest_node(next_node)
 *		int next_node;
 *
 * Description:	get_fastest_node() returns the number of the lightest
 *		loaded node if the load difference to next_node is higher
 *		than MIN_LOAD_DELTA. If the load difference is not high
 *		enough or no load information is available next_node is
 *		returned.
 */

int	get_fastest_node(next_node)
int	next_node;
{

	int	 		i;
	int			new_node;
	int			fast_node;
	static int 		shmid = -1;
	double			new_load;
	double			fast_load;
	double			next_load;
	struct load_info	*load_info_ptr;


	if ( shmid == -1 ) {
		/*
		 * Generate key file name.
		 */
		GET_KEY_FILE(key_file, key_dir);

		/*
		 * Generate shm key.
		 */
		GET_KEY(key, key_file);
		if ( key == (key_t) -1 ) {
			/*
			 * Couldn't get shm key.
			 */
			if ( debug ) {
                        	syslog(LOG_DEBUG, "(get_fastest_nod()) GET_KEY failed\n");
                	}
			return(next_node);
		}

		/*
		 * Get shm id.
		 */
		shmid = shmget(key, 0, 0);
		if ( shmid == -1 ) {
			/*
			 * Couldn't get shm id.
			 */
		if ( debug ) {
                        syslog(LOG_DEBUG, "(get_fastest_nod()) shmget failed\n");
                }
			return(next_node);
		}
	}


	load_info_ptr = (struct load_info*) shmat(shmid, 0, SHM_RDONLY);
	if ( load_info_ptr == (struct load_info*) -1 ) {
		/*
		 * Couldn't attach to shm.
		 */
		if ( debug ) {
                        syslog(LOG_DEBUG, "(get_fastest_nod()) shmat failed\n");
                }
		return(next_node);
	}

	/* 
	 * Scan through the load vector in shm to find the most lightly
	 * loaded node. This is done backwards to find the most up-to-date
	 * information at the beginning of the load vector.
	 */
	next_load = PREPOSTEROUS_LOAD;
	fast_load = PREPOSTEROUS_LOAD;
	fast_node = -1;

	for ( i = load_info_ptr->num_elements - 1; i >= 0; i-- ) { 
		new_node = load_info_ptr->load_vector[i].node;
		new_load = load_info_ptr->load_vector[i].lm;

		/*
		 * Check if information for next_node.
		 */
		if ( new_node == next_node ) {
			/*
			 * Remember load of next_node.
			 */
			next_load = new_load;
		}

		/* 
		 * Check if this info is the fastest node so far.
		 * Check if this is a valid entry (new_node != -1).
		 */
		if ( new_load <= fast_load && new_node != -1 ) {
#ifdef SKIP_ROOT_FS_NODE
			if ( new_node != root_fs_node ) {
#endif /* SKIP_ROOT_FS_NODE */
				fast_node = new_node;
				fast_load = new_load;
#ifdef SKIP_ROOT_FS_NODE
			}
			else if ( debug ) {
				syslog(LOG_DEBUG, "(get_fastest_node()) skipping node %d\n", new_node);
			}
#endif /* SKIP_ROOT_FS_NODE */
		}  
	} 

	if ( fast_node == -1 ) {
		/*
		 * Didn't find load information.
		 */
		if ( debug ) {
			syslog(LOG_DEBUG, "(get_fastest_node()) didn't find valid load information\n");
		}
		fast_node = next_node;
	}

	/*
	 * Check if fast_load is significant lower than load of next_node.
	 */
	if ( next_load - fast_load < MIN_LOAD_DELTA ) {
		/*
		 * next_node is almost as fast.
		 */
		if ( debug ) {
			syslog(LOG_DEBUG, "(get_fastest_nod()) load delta too small\n");
		}
		fast_node = next_node;
	}

	if ( shmdt((char *) load_info_ptr) == -1 ) {
		/*
		 * Couldn't detach shm.
		 * As this causes rexec() and rfork(0 to fail with NORMA13,
		 * we return -1 which causes inetd to fork to the local node.
		 */
		if ( debug ) {
                        syslog(LOG_DEBUG, "(get_fastest_nod()) shmdt failed\n");
                }
		return(-1);
	}

	return(fast_node);
}
