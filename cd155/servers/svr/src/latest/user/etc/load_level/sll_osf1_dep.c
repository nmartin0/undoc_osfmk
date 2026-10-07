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
 * $Log: sll_osf1_dep.c,v $
 * Revision 1.7  1994/11/19  03:08:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/11/18  20:53:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/09/30  18:12:06  stefan
 * Made the load leveler multi-threaded
 *
 * Revision 1.4  1993/07/27  08:06:57  stefan
 * We now use the real system call fork_remote_ctl() instead of
 * syscall().
 *
 * Revision 1.3  1993/07/23  17:43:09  stefan
 * Put #ifdef SLL around the code in order to allow builds
 * without -DSLL.
 *
 * Revision 1.2  1993/05/21  20:43:58  stefan
 * Completed copyright notice.
 *
 * Revision 1.1  1993/05/13  09:20:47  stefan
 * Integrated static load leveling support.
 *
 */

#ifdef SLL

#include <sys/types.h>
#include <sys/limits.h>
#include <sys/table.h>
#include <sll/sll.h>
#include "load_level_types.h"

extern void error_nonfatal();


/*
 * Name:
 *	enable_remote_process_creation - enable remote process creation.
 *
 * Synopsis:
 *	int	enable_remote_process_creation();
 *
 * Parameters:
 *      None.
 *
 * Description:
 *	This function enable remote process creation in the OSF/1AD server.
 *
 * Return Values:
 *	0 on success -1 on error.
 *
 */
int	enable_remote_process_creation()
{
	node_t		dummy_node;


	/*
	 * Use table_node(..., TBL_FASTNODE, ....) to determine if we are
	 * running on a server that supports static load leveling.
	 */
	if ( table_node(local_node_num, TBL_FASTNODE, 0, &dummy_node, 1,
			sizeof(dummy_node)) == -1 ) {
		/*
		 * Seems not to support static load leveling.
		 */
		return (-1);
	}
	
	/*
	 * Call fork_remote_ctl system call with 1 as parameter.
	 */
	if ( fork_remote_ctl(ENABLE_FORK_REMOTE) == -1 ) {
		/*
		 * OOPS - error.
		 */
		error_nonfatal("fork_remote_ctl() failed\n");
		return (-1);
	}

	return(0);
}

/*
 * Name:
 *	hand_fast_node_to_os - hand lightly loaded node number to the server.`
 *
 * Synopsis:
 *	void	hand_fast_node_to_os(fast_node);
 *	node_t	fast_node;
 *
 * Parameters:
 *	fast_node:	node number of most lightly loaded node.
 *
 * Description:
 *	This function hand the node number of the most lightly loaded node
 *	to the OSF/1AD server.
 *
 * Return Values:
 *      None.
 *
 */
void	hand_fast_node_to_os(fast_node)
node_t	fast_node;
{
	/*
	 * Hand the number of the most lightly loaded node to the OSF/1AD
	 * server using table_node().
	 */
	if ( table_node(local_node_num, TBL_FASTNODE, 0, &fast_node, -1,
			sizeof(fast_node)) == -1 ) {
		error_nonfatal("table_node failed\n");
	}
}

#endif /* SLL */
