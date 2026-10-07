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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_loadve.c,v 1.27 1995/03/17 21:32:31 sdh Exp $
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <nx/h.h>
#include <nx/defines.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>

#define	MAXPTYPE_STRING	(12) /* Big enough to hold string with 32 bit integer */

/***************************** nx_loadve ***********************
 *
 *      Calling Sequence:
 *             nx_loadve(node_array, node_count, ptype, pid_array, path,
 *							 argv, envp);
 *
 *      Description:
 *		nx_loadve() executes the specified file on the specified
 *		set of nodes. First a process is created on all nodes
 *		and then each process execs on all the rest of the nodes.
 *
 *      Parameters:
 *              node_array : an array of node_ts 
 *		node_count : number of nodes in node_array
 *		ptype      : The process type for each child process
 *		pid_array  : OUT array for pids
 *		path       : The relative or absolute pathname of the file
 *		argv       : command line arguments
 *		argc       : number of arguments
 *		envp       : environment
 *
 *      Returns:
 *              number of child processes or -1
 *
 *
 */


long 
nx_loadve(node_array, node_count, ptype, pid_array, path, argv, envp) 
node_t	*node_array;		/* Array of logical node number */
long	node_count;		/* Number of nodes */
long	ptype;			/* Ptype */
pid_t	*pid_array;		/* Pointer to the Out array for pids */
char 	*path;			/* executable file */
char	*const argv[];		/* array of character pointers to arguments */
char	*const envp[];		/* environment of the new process */
{
	register i;

	int	*errno_array;	/* array of errno from rforkmulti */
	long	retcode;
	int	argc;		/* Number of arguments in the list */
	char    **newargv;	/* argv with the special flag */
	char    **saveargv;	/* to save argv */
	int     index;		/* index to the argv */
	int     newargc ;	/* argc with the special flag */
	char	*ptypestr;	/* ptype in the string format */
	char	*pfdstr;	/* pfd in the string format */
	char    special[] = SPECIAL;	/* Special flag */
	node_t	*tmp_narray;	/* buffer to hold node array for rforkmulti
				 * this is a patch because rforkmulti is modifying
				 * the node list
				*/
	sigset_t	old_mask;
	int pfds[2];		/* pipe file descriptors for error returns */
	NX_LOADVE_STAT_T statbuf, *stat; /* buffer for getting status from children */
	int error;		/* place holder for errors passed from children */
	int done;
	pid_t *hold_pid;	/* array to hold the pids while waiting for status
				   to be returned. The pids get zeroed as status is 
				   returned */
	int k;
	int ch_status;		/* child status from waitpid */
	int total_nodes;	/* total number of nodes being loaded */
	fd_set ctl_mask;
	fd_set read_mask;
	struct timeval timeout;
	int n;
	int nfds;
        struct sigaction new_sigaction;
        struct sigaction old_sigaction;
	int eflag;
	int ret;
	int read_done;

	if (node_count == 0) {
		errno = EINVAL;
		return(-1);
	}

	/*     
	 * If node count is -1, load on all the nodes
	 */
	if( node_count == -1) {
		if ((node_count = _numnodes()) < 0){
			return(-1);
		}
		node_array = ( node_t *) malloc ( sizeof(node_t) * node_count );
		if ((node_array == NULL) && (node_count > 0)) {
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
	if (errno_array == NULL) {
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

	/* create a pipe for the child to return status */
	if (pipe(pfds) < 0)
		return(-1);

	old_mask = nx_sighold ( SIGINT );
        if( old_mask == -1 )
                        return(-1);
        if ( nx_sighold ( SIGQUIT ) == -1 )
                        return(-1);
        if ( nx_sighold ( SIGHUP ) == -1 )
                        return(-1);
        if ( nx_sighold ( SIGTERM ) == -1 )
                        return(-1);

	retcode =  rforkmulti(&node_count,tmp_narray,errno_array,
					pid_array );
        if ( nx_sigrelease( SIGTERM, old_mask ) == -1 )
                        return(-1);
        if ( nx_sigrelease( SIGHUP, old_mask ) == -1 )
                        return(-1);
        if ( nx_sigrelease( SIGQUIT , old_mask ) == -1 )
                        return(-1);
        if ( nx_sigrelease( SIGINT, old_mask ) == -1 )
                        return(-1);

	if ( retcode < 0 ) {
#ifdef DEBUG
printf("rforkmulti failed %d\n",retcode);
#endif
		free( (void *) tmp_narray);
		for(i = 0; i < node_count; i++)
			if (errno_array[i] != 0){
				errno = errno_array[i];
				break;
			}
		return(-1);
	} else if ( retcode == 0 ) { /* child */

	        close(pfds[0]); /* close read pipe */
		free( (void *) tmp_narray);
		/*
		 * Build an argument list with a special flag and 
		 * the ptype.
		 */
		if (argv == NULL) {
			/* The new argv needs to at least contain the
			 * program pathname. This will allow IPD to work
			 * when the passed in argv is NULL
			*/
			argc = 1;
		}
		else {
			for (argc = 0; argv[argc] != NULL; argc++); 
		}

		newargv = (char **) malloc(sizeof(char *) * (argc + 4));
		if (newargv == NULL) {
			errno = ENOMEM;
			return(-1);
		}
		saveargv = newargv; 

		newargc = 0;
		if (argv != NULL) {
			/*
			 * Copy arguments from original argv to new argv
			 */
			for ( index = 0; index < argc; index++) {
				*newargv = argv[index];
				newargv++;
				newargc++;
			}
		}
		else{
			/* A null argv was given, we need to change it
			 * so that argv[0] contains the pathname
			*/
			*newargv = path;
			newargv++;
			newargc++;
		}

		/* add pipe for return status */
		pfdstr = (char *) malloc(MAXPTYPE_STRING);
		if ( pfdstr == NULL ) {
			errno = ENOMEM;
			return(-1);
		}
		sprintf( pfdstr, "%d", pfds[1]);
		*newargv = pfdstr;
		newargv++;
		newargc++;
		/* add special value for nx_setup to key off of */
		*newargv = special;
		newargv++;
		newargc++;
		/* add ptype */
		ptypestr = (char *) malloc(MAXPTYPE_STRING);
		if ( ptypestr == NULL ) {
			errno = ENOMEM;
			return(-1);
		}
		sprintf( ptypestr, "%d", ptype);
		*newargv = ptypestr;
		newargv++;
		newargc++;
		*newargv = (char *) 0;
		newargv = saveargv;
		
		/* 
	 	 * Load the executable 
	 	 */
			

		if(execve(path, newargv, envp )  == -1 ) {
#ifdef DEBUG
printf("execve failed \n");
fflush(stdout);
#endif
			exit(1);
		}

	    } else {   /* parent */
		/* close the write side of the pipe and listen
		 * on the read side for status from the children.
		 */
		close(pfds[1]); /* close write pipe */
		/*
		 * Set the read side of the pipe to no delay so
		 * we can implement a timeout without using alarm()
		 */
		fcntl(pfds[0], F_SETFL, O_NDELAY);
		total_nodes = node_count;
		/*
		 * Wait for each of the children to respond. The timeout is set for
		 * 30 seconds and is reset after each message is received.
		 */

		/* allocate an array to store the pids if the children */
		hold_pid = (pid_t *)calloc(total_nodes, sizeof(pid_t));
		
		/* copy pid array from rforkmulti to hold_pid */
		bcopy(pid_array, hold_pid, total_nodes *sizeof(pid_t));

		/*
		 * This loop reads the pipe, in order to get status back from the 
		 * children. After 30 seconds from the last status received or 
		 * the last timeout, the code will walk the list of pids that 
		 * have not responded and do a waitpid on each. If the pid is no 
		 * longer valid, then the pid is cleared from the list of pids.
		 * If the read is successful and one of the children has returned 
		 * status, then the status is checked and if the child passed back 
		 * errno, then the pid is cleared and the node count is decremented.
		 */
		error = 0;
		eflag = FALSE;
		done = 0;
		timeout.tv_sec = 30;
                timeout.tv_usec = 0;
		FD_ZERO (&ctl_mask);
		FD_SET(pfds[0], &ctl_mask);
		nfds = getdtablesize();

                /* Ignore SIGPIPE before the loop, so a read/select failure doesn't
                 * cause the program to terminate */
                new_sigaction.sa_handler = SIG_IGN;
                if (sigaction(SIGPIPE, &new_sigaction, &old_sigaction) == -1){
                        perror("nx_setup: sigaction");
                }

		while (!done) {
		    read_mask = ctl_mask;
		    n = select(nfds, &read_mask, 0, 0, &timeout);
		    switch(n) {
		    case 0:
			/*
			 * timeout - walk the list of pids to see if any are still alive 
			 */
			for (k=0; k<total_nodes ;k++) {
			    if (hold_pid[k]){ 
				/* 
				 * Then this pid is still running and hasn't responded 
				 * with a status, so we do a waitpid cal to see if he is still 
				 * alive. If not, the we clear the pid in the hold_pid array.
				 */
				if ((waitpid(pid_array[k], &ch_status, WNOHANG)) != 0) {
				    /* child is not there, so clear hold_pid */
#ifdef DEBUG
				    printf("Child %ld not there and no status\n", pid_array[k]);
#endif
				    hold_pid[k] = 0;
				    pid_array[k] = 0;
				    node_count--;
				}
			    }
			}
			/*
			 * Now check to see if there are any pids still active that haven't
			 * returned status. If not, we are done.
			 */
			for (k=0; k<total_nodes; k++) {
			    if (hold_pid[k])
				break;
			}
			if (k == total_nodes) {
			    /* we're done */
			    done++;
			}
			break;
		    case 1:
			/* got a status */
			read_done = FALSE;
			while (!read_done) {
				ret = read(pfds[0], &statbuf, sizeof(statbuf));
				if (ret < 0) {
					if (errno == EINTR) {
						/* just an EINTR so try the read again */
#ifdef DEBUG
						printf("Read returned an EINTR\n");	
#endif
					} else {
						/* a real error occurred */
			    			error = errno;
					    	eflag = TRUE;
						read_done = TRUE;
					}		
				} else {
					/* read was successful */
					read_done = TRUE;
				}
			}

			if (eflag) {
				done++;
				break;
			}
			
			/*
			 * Check the status returned by the child. If the errno is set
			 * clear out the pid from pid_array and decrement the node count.
			 */
			stat = (NX_LOADVE_STAT_T *)&statbuf;
			/* clear this pid, so we know it returned a status */
			for (k=0;k<total_nodes;k++){
			    if (stat->pid == hold_pid[k]) {
				hold_pid[k] = 0;
				break;
			    }
			}
			
			if (stat->errno != 0) {
			    error = stat->errno;
			    *(pid_array + k) = 0;
			    node_count--;
			}
			/*
			 * Now check to see if there are any pids still active that haven't
			 * returned status. If not, we are done.
			 */
			for (k=0; k<total_nodes; k++) {
			    if (hold_pid[k])
				break;
			}
			if (k == total_nodes)
			    /* we're done */
			    done++;
			break;
		    case -1:
			if (errno == EINTR) {
				/* try again */	
#ifdef DEBUG
				printf("Select failed with EINTR\n");
#endif
			} else { 
#ifdef DEBUG
				printf("Select failed with -1 errno %d\n", errno);
#endif
				/* select failed */
				error = errno;
				eflag = TRUE;
				/* a error occured on the select */
				done++;
			}
			break;
		    default:
			/* select returned more than 1 but we only listen 
			 * on one fd
			 */
			error = EINVAL;
			eflag = TRUE;
			done++;
			break;
		    }
		}
	
	/* reset the old action */
	if (sigaction(SIGPIPE, &old_sigaction, &old_sigaction) == -1){
		perror("nx_setup: sigaction");
	}
	
	/*
	 *pick up any error passed from the children and
	 * return it in errno.
	 */
	if (error)
	    errno = error;

	close(pfds[0]);
	free( (void *) tmp_narray);
	free( (void *) hold_pid);
        if ((node_count == 0) || /* no node loaded successfully */
	   (eflag == TRUE)) { /* a read or select failure */
	    return(-1);
	} else {
	    return(node_count);
	}
    }
   
}



