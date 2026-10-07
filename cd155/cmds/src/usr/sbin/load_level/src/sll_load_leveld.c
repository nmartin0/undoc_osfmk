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
 * Copyright (c) 1993, Intel Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: sll_load_leveld.c,v $
 * Revision 1.7  1994/11/19  03:08:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/11/18  20:53:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/02/16  16:37:08  stefan
 * Merged version 1.4.2.1 into main trunk.
 *
 * Revision 1.4.2.1  1994/02/16  14:44:42  stefan
 *  Reviewer: bolsen (Locus)
 *  Risk: low to medium
 *  Benefit or PTS #: 7899, 8028, 8039
 *  Testing: developer testing
 *  Module(s): svr/user/etc/load_level/load_level_types.h
 *             svr/user/etc/load_level/load_leveld.c
 *             svr/user/etc/load_level/loadlevel
 *             svr/user/etc/load_level/osf1_dep.c
 *             svr/user/etc/load_level/sll_load_level_types.h
 *             svr/user/etc/load_level/sll_load_leveld.c
 *             svr/user/etc/load_level/parameters
 *
 * For PTS #7899 I have added 2 new parameters to the load leveler daemon:
 * root_fs_node_target   boolean that specifies if root_fs_node should be
 *                       used as a target node for load leveling.
 *                       Default: 0
 * root_fs_node_source   boolean that specifies if root_fs_node should be
 *                       used as source node for load leveling.
 *                       Default: 1
 *
 * For PTS #8028 the default behaviour has been changed so that process
 * migration is disabled by default and can be switched on using the command
 * line switch -d.
 *
 * For PTS #8039 a check has been implemented if a load_leveld is already
 * running on a node and if this is the case the new load leveler exits with
 * an error message.
 * Also, I have fixed a problem where load_leveld dumped core in migrate() when
 * the startup node is not included in the nodes_to_use parameter.
 * Instead of migrating to another node load_leveld now exits on the startup
 * node after rforking it's children.
 * In addition, now it is checked if get_tnc_port() returns MACH_PORT_DEAD and
 * in this case it is assumed that the corresponding peer is simply a little
 * late.
 *
 * Revision 1.4  1993/09/30  18:10:50  stefan
 * Made the load leveler multi-threaded
 *
 * Revision 1.3  1993/07/23  17:42:06  stefan
 * Put #ifdef SLL around the code in order to allow builds
 * without -DSLL.
 *
 * Revision 1.2  1993/05/21  20:37:57  stefan
 * Initial checked in version was outdated (fast node algorithm was implemented
 * as described in R0.1 of the SLL IPS). Now fast_node is implemented as
 * described in R0.3 os the SLL IPS.
 *
 * Revision 1.1  1993/05/13  09:21:26  stefan
 * Integrated static load leveling support.
 *
 */

#ifdef SLL

#include <stdlib.h>
#include <sys/types.h>
#include "load_level_com.h"
#include "load_level_types.h"
#include "sll_load_level_types.h"

/*
 * Variable containing the minimum load difference which is needed between
 * the local node and another node for which the other node is to be
 * considered underloaded.
 */
double static_min_load_delta = 1.0;

/*
 * Variable containing the frequency in microseconds with which the most
 * lightly loaded node is detemined and handed to the server
 */
double fast_node_timeout = 5000000.0;

/*
 * Variable which indicates if we do support static load leveling
 */
int	static_load_leveling = 0;

/*
 * External functions.
 */
extern void	lock_load_vec();
extern void	unlock_load_vec();

/*
 * Name:
 *	fast_node - fast_node algorithm.
 * 
 * Synopsis:
 *	void	fast_node();
 * 
 * Parameters:
 *	None.
 *
 * Description:
 *	The function fast_node() will determine which of the nodes(whose
 *	load measures are known by the local node) is the most lightly
 *	loaded node and will hand this node number to the OS.
 *	Some of the code is duplicated from re_dispatch_local_procs().
 *
 * Return Values:
 *	None.
 *
 */
void	fast_node()
{

	struct underload_info {
		int	node;
		double	lm;
		double	underload;
	};

	struct underload_info	load_vec_copy[MAX_NUM_LVEC_ELEMENTS];
	int			i;
	int			num_underloaded_nodes;
	int			num_nodes_with_info;
	double			total_lm;
	double			dlm;
	double			total_underload;
	double			random_num;
	double			local_node_overload;
	double			node_underload;
	double			underload_ratio;


	/*
	 * Lock the load vector.
	 */
	lock_load_vec();

	/*
	 * Initialize underload structure.
	 */
	for ( i = 0; i < num_lvec_elements; i++ ) {
		load_vec_copy[i].node = load_info_ptr->load_vector[i].node;
		load_vec_copy[i].lm = load_info_ptr->load_vector[i].lm;
		load_vec_copy[i].underload = 0.0;
	}

	/*
	 * Unlock the load vector.
	 */
	unlock_load_vec();

	/*
	 * Zero out duplicates field.
	 */
	bzero(duplicate, num_slots);

	/*
	 * Eliminate duplicates.
	 */
	for ( i = 0; i < num_lvec_elements; i++ ) {
		if ( load_vec_copy[i].node != -1 ) {
			if ( *(duplicate + load_vec_copy[i].node) == 1 ) {
				/*
				 * This is a duplicate - mark as invalid.
				 */
				load_vec_copy[i].node = -1;
				load_vec_copy[i].lm = -1;
			}
			else  {
				*(duplicate + load_vec_copy[i].node) = 1;
			}
		}
	}

	/*
	 * Calculate the total load measures for all nodes in the load vector.
	 */
	total_lm = 0.0;
	num_nodes_with_info = 0;
	for ( i = 0; i < num_lvec_elements; i++ ) {
		if ( load_vec_copy[i].node != -1 ) {
			/*
			 * Check if we should consider ROOT_FS_NODE.
			 */
			if ( load_vec_copy[i].node == root_fs_node &&
				 ! root_fs_node_target ) {
				/*
				 * Mark as invalid.
				 */
				load_vec_copy[i].node = -1;
				continue;
			}
			
			total_lm += load_vec_copy[i].lm;
			num_nodes_with_info++; 
		}
	}

	/*
	 * Compute the desired load measure.
	 */
	dlm = total_lm / (double)num_nodes_with_info;


	/*
	 * No locking necessary, as the 0th element is only accessed by the
	 * main thread.
	 */

	/*
	 * If the local node is not overloaded we are done.
	 * The local node is most lightly loaded node.
	 */
	local_node_overload = (load_info_ptr->load_vector[0].lm - dlm);
	if ( local_node_overload <= 0 ) {
		hand_fast_node_to_os((node_t) local_node_num);
		return;
	}

	/*
	 * Determine the underloaded amounts for each node in the load vector.
	 */
	total_underload = 0.0;
	num_underloaded_nodes = 0;
	for ( i = 0; i < num_lvec_elements; i++ ) {
		if ( load_vec_copy[i].node != -1 ) {
			/*
			 * Calculate underload amount of this node.
			 */
			node_underload = dlm - load_vec_copy[i].lm;

			if ( node_underload <= 0 ) {
				/*
				 * This node is not underloaded.
				 */
				load_vec_copy[i].underload = 0.0;
				continue;
			}

			if ( (node_underload >= min_underload &&
				local_node_overload >= min_overload) ||
				(load_vec_copy[0].lm - load_vec_copy[i].lm >=
				static_min_load_delta) ) {
				/*
				 * This node is considered to be underloaded.
				 */
				load_vec_copy[i].underload = node_underload;
				total_underload += load_vec_copy[i].underload;
				num_underloaded_nodes++;
			}
			else {
				/*
				 * This node is considered to be
				 * not underloaded.
				 */
				load_vec_copy[i].underload = 0.0;
			}
		}
	}

	/*
	 * If there are no underloaded nodes we are done.
	 * The local node is most lightly loaded node.
	 */
	if ( num_underloaded_nodes == 0 ) {
		hand_fast_node_to_os((node_t) local_node_num);
		return;
	}

	/*
	 * Determine the most lightly loaded node.
	 */
	random_num = rand();
	random_num /= RAND_MAX;
	for ( i = 0; i < num_lvec_elements; i++ ) {

		if ( load_vec_copy[i].node == -1  ||
			load_vec_copy[i].underload <= 0) {
			continue;
		}

		/*
		 * Calculate underload ratio of this node which is used in
                 * the random selection of the most lightly loaded node.
		 */
		underload_ratio = load_vec_copy[i].underload / total_underload;

		if ( underload_ratio < random_num ) {
			random_num -= underload_ratio;
			continue;
		}

		/*
		 * Hand the most lightly loaded node to the OSF/1AD server.
		 */
		hand_fast_node_to_os((node_t) load_vec_copy[i].node);
		break;
	}
}

#endif /* SLL */
