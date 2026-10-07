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
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_nfork.c,v 1.13 1994/11/19 02:31:49 mtm Exp $
 *
 */


#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

/*
 * Need mcmsg_intf.h for invalid ptype constant, mcmsg_intf.h needs
 * mcmsg_appl.h.
 */
#include <mcmsg/mcmsg_appl.h>
#include <mcmsg/mcmsg_intf.h>

sigset_t nx_sighold();
typedef	long	node_t;

long 
nx_nfork(node_array, node_count, ptype, pid_array) 
node_t	*node_array;		/* Array of logical node number */
int	node_count;		/* Number of nodes */
long	ptype;			/* Ptype */
pid_t	*pid_array;		/* Out array for pids */
{
	register i;

	int *errno_array;		/* array for errnos */
	long retcode;			/* return value from rforkmulti */
	node_t *tmp_narray;
	sigset_t old_mask;


	if ( node_count == 0) {
		errno = EINVAL;
		return(-1);
	}

	/*
	 * If node count is -1, load on all the nodes
	 */

	if( node_count == -1) {
		if ((node_count = _numnodes()) < 0) {
			return(-1);
		}
#ifdef DEBUG
printf("numnodes returned with %d \n",node_count);
#endif
		node_array = ( node_t *) malloc ( sizeof(node_t) * node_count );
		if( node_array == NULL) {
			errno = ENOMEM;
			return(-1);
		}
		for ( i = 0; i < node_count; i++ ) {
		 	node_array[i] = i;
		}
	} else {

		/*
	 	 * Check node_array arguments.
	 	 */
	
		if (_numnodes() < 0) {
			return(-1);
		}
		for ( i = 0; i < node_count; i++ ) {
		    if((node_array[i] < 0) || (node_array[i] > _numnodes())){
				errno = EPBADNODE;
				return(-1);
		    }
		}
	}

	/*
	 * Create an array to receive errnos from rforkmulti
	 */

	errno_array = (int *) malloc ( sizeof(int) * node_count );
	if( errno_array == NULL) {
		errno = ENOMEM;
		return(-1);
	}

	tmp_narray = ( node_t *) malloc ( sizeof(node_t) * node_count );
	if( tmp_narray == NULL) {
		errno = ENOMEM;
		return(-1);
	}
	for ( i = 0; i < node_count; i++ ) 
		 tmp_narray[i] = node_array[i];

	old_mask = nx_sighold( SIGINT );
	if( old_mask == -1 )
			return(-1);
	if ( nx_sighold ( SIGQUIT ) == -1 )
			return(-1);
	if ( nx_sighold ( SIGHUP ) == -1 )
			return(-1);
	if ( nx_sighold ( SIGTERM ) == -1 )
			return(-1);

	retcode =  rforkmulti(&node_count,tmp_narray,errno_array,pid_array);
	if ( nx_sigrelease( SIGTERM, old_mask ) == -1 )
			return(-1);
	if ( nx_sigrelease( SIGHUP, old_mask ) == -1 )
			return(-1);
	if ( nx_sigrelease( SIGQUIT, old_mask ) == -1 )
			return(-1);
	if ( nx_sigrelease( SIGINT, old_mask ) == -1 )
			return(-1);

	if(retcode == -1 ) {
#ifdef DEBUG
printf("rforkmulti failed %d\n",retcode);
#endif
		for( i = 0; i < node_count;  i++ ) 
			if (errno_array[i] != 0){
				errno = errno_array[i];
				break;
			}
		free( (void *) tmp_narray);
		return(-1);
	}
	else if( retcode == 0 ) { /* Child process */
		extern long mcmsg_ptype;
		
	/* 
 	 * The process type of the new process is set to ptype 
 	 */
		
		mcmsg_ptype = INVALID_PTYPE;
		setptype(ptype);
		free( (void *) tmp_narray);
		return(0);
		
	} else if (retcode == -2 ) {
#ifdef DEBUG
printf(" rforkmulti failed partially \n");
#endif
		for( i = 0; i < node_count;  i++ ) 
			if (errno_array[i] != 0){
				errno = errno_array[i];
				break;
			}
		free( (void *) tmp_narray);
		return(-1);
	}
			
	free((void *)tmp_narray);
	return(node_count);
}

