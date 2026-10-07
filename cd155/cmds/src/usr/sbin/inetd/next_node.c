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
 *      Copyright 1992, 1993  Intel Corporation.
 *
 */
/*
 * HISTORY
 * $Log: next_node.c,v $
 * Revision 1.6  1994/11/19  03:07:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/02/07  17:40:49  stefan
 * Merged fix for PTS #8083 from R1_2 branch into main trunk.
 *
 * Revision 1.4.4.1  1994/02/07  15:31:48  stefan
 * If inetd was (re)started on a node other than ROOT_FS_NODE, it had a
 * wrong idea about which node is ROOT_FS_NODE. This could result in
 * inetd scheduling network jobs onto ROOT_FS_NODE instead of skipping
 * ROOT_FS_NODE.
 * This fix uses the boot magic strings and table(TBL_PHYSNODEINFO) for
 * finding out the correct node number for ROOT_FS_NODE.
 *
 *  Reviewer: shala
 *  Risk: low to medium
 *  Benefit or PTS #: 8083
 *  Testing: developer testing
 *  Module(s): cmds_libs/src/usr/sbin/inetd/next_node.c
 *             cmds_libs/src/usr/sbin/inetd/Makefile
 *             cmds_libs/src/usr/sbin/inetd/getbootenv.c
 *
 * Revision 1.4  1993/06/21  10:49:39  stefan
 * Modified the load leveling algorithm to do round robin scheduling of jobs
 * unless the load of next node to be selected is at least by MIN_LOAD_DELTA
 * (1.0) higher than the load of the most lightly loaded node (which is
 * selected in this case).
 *
 * Revision 1.3  1993/05/12  15:51:50  shala
 * Include sys/syslimits.h include file to define PATH_MAX.
 *
 * Revision 1.2  1993/03/02  19:43:31  stefan
 * Instead of getting ROOT_FS_NODE from the microkernel which doesn't work
 * because a physical node number is returned, we now skip the local node
 * number which should be identical with ROOT_FS_NODE.
 *
 * Revision 1.1  1993/02/23  10:21:56  stefan
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

#include <sys/syslimits.h>
#include <sys/table.h>
#include <syslog.h>
#include <stdio.h>

#ifdef SKIP_ROOT_FS_NODE

int	root_fs_node = -1;

#endif /* SKIP_ROOT_FS_NODE */

/*
 * Array of logical node numbers.
 */
static	int	*node_number;

/*
 * Number of nodes in service partition.
 */
static	int	node_count;


extern int	debug;

/*
 * select_next_node()
 *
 * 	Select node on which to rfork/rexec 
 */
int	select_next_node()
{
	static	int	node_index = -1;
	char		*cp;
	int		node;
#ifdef USE_FAST_NODE
	extern int	get_fastest_node();
#endif /* USE_FAST_NODE */

	if (node_index == -1) {
		/*
		 * Get number of nodes in service partition, and a list of
		 * the node numbers.
		 */
		if ( debug ) {
			syslog(LOG_DEBUG, "(select_next_node()) initializing node array\n");
		}

		node_count = get_node_array(&node_number);
		node_index = 0;

		if ( debug ) {
			syslog(LOG_DEBUG, "(select_next_node()) found %d nodes\n", node_count);
		}

#ifdef SKIP_ROOT_FS_NODE
		if (node_count > 1) {
			/*
			 * Remember the ROOT_FS_NODE which is awfully busy.
			 */
			root_fs_node = get_root_fs_node();

			if ( debug ) {
				syslog(LOG_DEBUG, "(select_next_node()) root_fs_node is %d\n", root_fs_node);
			}
		}
#endif /* SKIP_ROOT_FS_NODE */
	}

/*
 *	It makes little sense to rexec() to the local node.
 *
 *	if (node_count == 0) {
 */
	if (node_count <= 1) {
		/*
		 * Only one node or not running under TNC server.
		 */
		if ( debug ) {
			syslog(LOG_DEBUG, "(select_next_node()) returning -1\n");
		}

		return (-1);
	}

	node = node_number[node_index++];

	if (node_index >= node_count) {
		node_index = 0;
	}

#ifdef SKIP_ROOT_FS_NODE
	if ( node == root_fs_node ) {
		/*
		 * Skip this node.
		 */
		if ( debug ) {
			syslog(LOG_DEBUG, "(select_next_node()) skipping node %d\n", node);
		}

		node = node_number[node_index++];

		if (node_index >= node_count) {
			node_index = 0;
		}
	}
#endif /* SKIP_ROOT_FS_NODE */

#ifdef USE_FAST_NODE
	if ( node_count > 1 ) {
		/*
		 * Get fastest load from the load leveling daemon.
		 */
		node = get_fastest_node(node);

		if ( debug ) {
			syslog(LOG_DEBUG, "(select_next_node()) get_fastest_node() returned %d\n", node);
		}
	}
#endif /* USE_FAST_NODE */

	if ( debug ) {
		syslog(LOG_DEBUG, "(select_next_node()) returning %d\n", node);
	}

	return (node);
}


/*
 * get_node_array()
 *
 * Return number of nodes in service partition and initialize pointer
 * to node array. 
 */
int	get_node_array(node_array)
int	**node_array;
{
	int	node_count;

	/* get number of logical nodes in partition */
	if ((node_count = table(TBL_NODEINFO, 0, (char *)0, 32767, 0)) == -1) {
		/*
		 * Table call failed, must not be using TNC server.
		 */
		*node_array = NULL;
		return (0);
	}

	/* allocate array for node numbers */
	*node_array = (int *) malloc(node_count  * sizeof(int));
	if ( *node_array == NULL ) {
		syslog(LOG_ERR, "(get_node_array()) out of memory\n");
		*node_array = NULL;
		return (0);
	}

	/* get logical node numbers */
	if (table(TBL_NODEINFO, 0, (char *) *node_array, node_count,
	    sizeof(int)) == -1)
	{
		syslog(LOG_ERR, "(get_node_array()) cannot obtain the logical node numbers\n");
		free(*node_array);
		*node_array = NULL;
		return(0);
	}

	return (node_count);
}


/*
 * get_phys_node_array()
 *
 * Return number of nodes in service partition and initialize pointer
 * to node array. 
 */
int	get_phys_node_array(node_array)
int	**node_array;
{
	int	node_count;

	/* get number of physical nodes in partition */
	if ((node_count = table(TBL_PHYSNODEINFO, 0, (char *)0, 32767, 0)) == -1) {
		/*
		 * Table call failed, must not be using TNC server.
		 */
		*node_array = NULL;
		return (0);
	}

	/* allocate array for node numbers */
	*node_array = (int *) malloc(node_count  * sizeof(int));
	if ( *node_array == NULL ) {
		syslog(LOG_ERR, "(get_phys_node_array()) out of memory\n");
		*node_array = NULL;
		return (0);
	}

	/* get physical node numbers */
	if (table(TBL_PHYSNODEINFO, 0, (char *) *node_array, node_count,
	    sizeof(int)) == -1)
	{
		syslog(LOG_ERR, "(get_phys_node_array()) cannot obtain the physical node numbers\n");
		free(*node_array);
		*node_array = NULL;
		return(0);
	}

	return (node_count);
}

/*
 * get_root_fs_node()
 *
 * Return the logical node number of ROOT_FS_NODE.
 */
int	 get_root_fs_node()
{
	int	i;
	int	num_phys_nodes;
	int	*phys_node_num_ptr;
	int	phys_root_fs_node;
	char	*bootvalue;
	char	*getbootenv();


	/*
	 * First get bootmagic ROOT_FS_NODE.
	 */
	bootvalue = getbootenv("ROOT_FS_NODE");
	if ( bootvalue == NULL ) {
		syslog(LOG_ERR, "(get_root_fs_node()) cannot get bootmagic ROOT_FS_NODE\n");
		return(-1);
	}

	/*
	 * Convert to node number.
	 */
	if ( isdigit(*bootvalue) ) {
		phys_root_fs_node = atoi(bootvalue);
	}
	else {
		syslog(LOG_ERR, "(get_root_fs_node()) invalid value for ROOT_FS_NODE\n");
		return(-1);
	}

	/*
	 * Get number of physical nodes in service partition.
	 */
    	num_phys_nodes = get_phys_node_array(&phys_node_num_ptr);
	if ( num_phys_nodes == 0 ) {
		return(-1);
	}

	/*
	 * Look for phys_root_fs_node in the list of physical node numbers.
	 */
	for ( i = 0; i < num_phys_nodes; i++ ) {
		if ( *(phys_node_num_ptr + i) == phys_root_fs_node ) {
			break;
		}
	}

	/*
	 * Free the list of physical node numbers.
	 */
	free(phys_node_num_ptr);

	/*
	 * Check if we were sucessful.
	 */
	if ( i >= num_phys_nodes || i >= node_count ) {
		syslog(LOG_ERR, "(get_root_fs_node()) cannot obtain logical node number for node %d\n", phys_root_fs_node);
		return(-1);
	}

	/*
	 * We are done.
	 */
	return(*(node_number + i));
}
