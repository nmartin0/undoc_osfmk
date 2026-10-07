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
 * $Log: sll.c,v $
 * Revision 1.14  1995/03/23  18:48:49  yazz
 *  Reviewer: Ray ANderson
 *  Risk: Lo
 *  Benefit or PTS #: 11960 C-0
 *  Testing: 40 iterations of testcase, EATs controlc, os_interfaces
 *  Module(s): server/tnc/sll.c
 * When disqualifying a process from static load levelling due to shared
 * memory segments, be sure to recognize all types of mmap() segments.
 * Also use the read lock when scanning a proc's mmap list.
 *
 * Revision 1.13  1994/11/18  20:38:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.12  1994/04/19  15:37:57  stefan
 * Resolved a merge problem.
 *
 * Revision 1.11  1994/04/19  13:17:20  stefan
 * Somehow the log for revision 1.9 has been lost - put it in again.
 *
 * Revision 1.10  1994/04/19  12:58:38  stefan
 * Avoid that SLL_UNLOCK is called twice - cosmetic modification.
 *
 * Revision 1.9  1994/04/19  12:24:46  stefan
 * Merged Rersion 1.6.2.1 from R_1 branch into main trunk.
 * Bug fix for PTS #9000.
 *
 * Revision 1.8  1994/02/09  17:10:00  stefan
 * Fixed a minor glitch which could cause problems on -nx servers.
 *
 *  Reviewer: none
 *  Risk: very low
 *  Benefit or PTS #: works with -nx servers.
 *  Testing: developer testing
 *  Module(s): sll.c
 *
 * Revision 1.7  1994/01/10  14:19:34  stefan
 * Reviewer: none
 * Risk: low
 * Benefit or PTS #: clean-ups to pass lint
 * Testing: built
 * Module(s): sll.c, sll.h sll_types.h
 *
 * Some minor clean-ups to eliminate warnings from lint.
 *
 * Revision 1.6.2.1  1994/04/15  17:02:46  stefan
 * The static load leveling code has been modified so that sll_lock is
 * released before nx_map_node_proc() is called. This prevents a deadlock
 * in the static load leveling code path.
 *
 *  Reviewer: Charlie Johnson
 *  Risk: Low to Medium
 *  Benefit or PTS #: PTS #9000
 *  Testing: developer testing
 *  Module(s): server/sll/sll.c
 * 	    server/bsd/cmu_syscalls.c
 *
 * Revision 1.6  1993/10/01  16:18:18  stefan
 * Added a check for BSD shared memory segments (mmap). For better modularity
 * a new function sll_has_shm() was created where this check and the check
 * for SYSV shm (a` la cfj) is done.
 * Also removed some checks from sll_init() that are unnecessary with the
 * new boot magic parser that was introduced in the 1.0.4 server. Instead I
 * moved the last necessary check into the function
 * sll_check_bm_fork_remote_timeout() which is now called by the boot magic
 * parser.
 *
 * Revision 1.5  1993/08/19  18:51:42  cfj
 * This fixes bug #5644.  Since there isn't an accurate flag kept locally
 * for Sys V shared memory usage, kludge by using the more generic Sys V
 * IPC flag in the u-area.  This kludge will not be needed when we go to
 * an NMK14 base.
 *
 * Revision 1.4  1993/06/23  18:15:29  stefan
 * For better modularity added function to return fast_node.
 * Added function to check for valid node number for fast_node.
 *
 * Revision 1.3  1993/06/01  16:15:08  stefan
 * Renamed sll/sll.h to sll/sll_types.h.
 *
 * Revision 1.2  1993/05/27  17:21:31  stefan
 * Completed Copyright notice.
 * Now symbolic constants out of sys/sll.h are used for the parameter to
 * fork_remote_ctl().
 *
 * Revision 1.1  1993/05/13  09:10:00  stefan
 * Integrated static load leveling support.
 *
 */

#include <sys/secdefines.h>
#include <sys/proc.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sll/sll_types.h>
#include <sll/sll.h>

#ifdef SLLDEBUG
/*
 * Debug on/off
 */
int sll_debug = 1;
#endif /* SLLDEBUG */

/*
 * Lock for static load leveling.
 */
udecl_simple_lock_data(, sll_lock)

/*
 * Boot magic FORK_REMOTE.
 */
boolean_t	fork_remote = DEF_ENABLE_FORK_REMOTE;

/*
 * Boot magic FORK_REMOTE_TIMEOUT.
 */
unsigned long	fork_remote_timeout = 60;

/*
 *  Boot magic FORK_REMOTE_SHM.
 */
boolean_t	fork_remote_shm = DEF_FORK_REMOTE_SHM;

/*
 * Variable which indicates if remote process creation is enabled (set by
 * fork_remote_ctl() system call.
 */
int	fork_remote_enabled = 0;

/*
 * Variable which contains the (logical) node number of the most lightly
 * loaded node.
 */
node_t	fast_node = -1;

/*
 * Variable which contains the time after which fast_node is considered
 * to be timed out.
 */
long	fast_node_timeout = -1;		/* fast_node is timed out after that */


/*
 * Name:
 *	fork_remote_ctl() - fork_remote_ctl() system call.
 *
 * Synopsis:
 *	int	 fork_remote_ctl(p, args, retval);
 *	register struct proc	*p;
 *	void			*args;
 *	int			*retval;
 *
 * Parameters:
 *	p	:	calling process	(standard system call parameter)
 *	args	:	user arguments	(standard system call parameter)
 *	retval	:	return value	(standard system call parameter)
 *
 * Description:
 *	fork_remote_ctl() implements the fork_remote_ctl() system call
 *	which is used to enable or disable remote process creation.
 *	args points to a singel argument which determines if remote sould be
 *	enabled (1) or disabled (0).
 *	Enabling remote process creation only succeeds it the boot magic
 *	variable ENABLE_FORK_REMOTE is set to 1 at boot time of the system.
 *	fork_remote_ctl() is only available for the superuser(root).
 *	
 * Return Values:
 *	EINVAL	:	parameter was not 0 or 1.
 *	ENOSYS	:	the boot magic variable ENABLE_FORK_REMOTE has been
 *			set to 0 at boot time.
 *	EPERM	:	the effective UID of the callling process was not
 *			superuser.
 *	ESUCCESS:	fork_remote_ctl() succeeded.
 *		
 */

/* ARGSUSED */
int	fork_remote_ctl(p, args, retval)
register struct proc	*p;
void			*args;
int			*retval;
{
	struct args {
		int	flag;
	}	*uap = (struct args *)args;
	int	error;

	/*
	 * Check parameter.
	 */
	if ( uap->flag != ENABLE_FORK_REMOTE &&
		uap->flag != DISABLE_FORK_REMOTE ) {
		SLL_DEBUGF("fork_remote_ctl(%d) invalid parameter \n", uap->flag);
		return(EINVAL);
	}

	/*
	 * Check if ENABLE_FORK_REMOTE is set to TRUE.
	 */
	if ( !fork_remote ) {
		SLL_DEBUGF("fork_remote_ctl(%d) %s is 0\n", uap->flag, BM_ENABLE_FORK_REMOTE);
		return(ENOSYS);
	}

#if     SEC_BASE
	/*
	 * Must have the SEC_SYSATTR privilege.
	 */
	if ( !privileged(SEC_SYSATTR, EPERM) ) {
		SLL_DEBUGF("fork_remote_ctl(%d) no SEC_SYSATTR permission\n", uap->flag);
		return (EPERM);
	}
#else
	/*
	 * Must be super user
	 */
	if ( error = suser(u.u_cred, &u.u_acflag) ) {
		SLL_DEBUGF("fork_remote_ctl(%d) not super user\n", uap->flag);
		return (error);
	}
#endif	/* SEC_BASE */

	/*
	 * Lock data structures.
	 */
	SLL_LOCK(sll_lock);

	/*
	 * Set fork_remote_enabled.
	 */
	fork_remote_enabled = uap->flag;

	/*
	 * Unlock data structures.
	 */
	SLL_UNLOCK(sll_lock);

	return (ESUCCESS);
}


/*
 * Name:
 *	sll_init() - initialize static load leveling.
 *
 * Synopsis:
 *	void	 sll_init();
 *
 * Parameters:
 *	none.
 *
 * Description:
 *	sll_init() initializes static load leveling.
 *	
 * Return Values:
 *	none.
 *		
 */

void	sll_init()
{
	/*
	 * Initialize sll lock.
	 */
	SLL_LOCK_INIT(sll_lock);
}


/*
 * Name:
 *	sll_check_bm_fork_remote_timeout() - check value of boot magic
 *					     variable BM_FORK_REMOTE_TIMEOUT.
 *
 * Synopsis:
 *	boolean_t	sll_check_bm_fork_remote_timeout(sel, val, matchNode);
 *	void		*sel;
 *	ulong		*val;
 *	node_t		*matchNode;
 *
 * Parameters:
 *	sel		:	not used.
 *	val		:	pointer to the value of BM_FORK_REMOTE_TIMEOUT.
 *	matchNode	:	node number.
 *
 * Description:
 *	sll_check_bm_fork_remote_timeout() checks for correct values of the
 *	boot variable BM_FORK_REMOTE_TIMEOUT.
 *	
 * Return Values:
 *	TRUE if the value is correct, FALSE else.
 *		
 */

/* ARGSUSED */
boolean_t	sll_check_bm_fork_remote_timeout(sel, val, matchNode)
void		*sel;
ulong		*val;
node_t		*matchNode;
{

	/*
	 * Check correctness of boot magic BM_FORK_REMOTE_TIMEOUT.
	 */

	if ( *val == 0 ) {
		printf("Node %d: Warning: config variable %s: Not an allowed value: at value \"0\"\n", matchNode, BM_FORK_REMOTE_TIMEOUT);
		return(FALSE);
	}

	return(TRUE);
}


/*
 * Name:
 *	sll_timeout() - check if fast_node is timed out.
 *
 * Synopsis:
 *	int	 sll_timeout();
 *
 * Parameters:
 *	none.
 *
 * Description:
 *	sll_timeout() checks if fast_node is timed out. If a time-out is
 *	occured 1 is returned otherwise 0 is returned.
 *	Called with sll_lock held.
 *	
 * Return Values:
 *	1	:	fast_node is timed out.
 *	0	:	fast_node is up to date.
 *		
 */
int	sll_timeout()
{
	struct timeval	time_now;

	/*
	 * Get current time.
	 */
	raw_microtime(&time_now);

	/*
	 * Check if timed out.
	 */
	if ( time_now.tv_sec > fast_node_timeout ) {
		/*
		 * Time-out.
		 */
		SLL_DEBUGF("sll_check_timeout: time_now=%d timeout=%d ==> TIMEOUT\n", time_now.tv_sec, fast_node_timeout);

		fast_node = -1;
		return (1);
	}

	/*
	 * No time-out.
	 */
	return(0);
}


/*
 * Name:
 *	sll_set_timeout() - set time out for fast_node.
 *
 * Synopsis:
 *	void	 sll_set_timeout();
 *
 * Parameters:
 *	none.
 *
 * Description:
 *	sll_set_timeout() sets the time-out value for fast_node.
 *	Called with sll_lock held.
 *	
 * Return Values:
 *	none.
 *		
 */
void	sll_set_timeout()
{
	struct timeval	time_now;

	/*
	 * Get current time.
	 */
	raw_microtime(&time_now);

	/*
	 * Calculate the time after that fast_node is considered to be
	 * timed out.
	 */
	fast_node_timeout = time_now.tv_sec + fork_remote_timeout;
}


/*
 * Name:
 *	sll_check_fast_node() - check for valid node number.
 *
 * Synopsis:
 *	int	 sll_check_fast_node();
 *
 * Parameters:
 *	none.
 *
 * Description:
 *	sll_check_fast_node() checks if fast_node is a correct node number.
 *	
 * Return Values:
 *	0 if fast_node has correct value, -1 otherwise.
 *		
 */
int	sll_check_fast_node()
{
	node_t		node;


	/*
	 * Lock data structures.
	 */
	SLL_LOCK(sll_lock);

	/*
	 * A value of -1 for fast_node is legal.
	 */
	if ( fast_node == -1 ) {
		SLL_UNLOCK(sll_lock);
		return(0);
	}

	/*
	 * Any other negative value is illegal.
	 */
	if ( fast_node < 0 ) {
		/*
		 * Mark fast_node as invalid.
		 */
		fast_node = -1;

		/*
		 * Return error.
		 */
		SLL_UNLOCK(sll_lock);
		return(-1);
	}

	/*
	 * Remember fast_node;
	 */
	node = fast_node;

#ifdef NX
	/*
	 * Unlock data structures.
	 */
	SLL_UNLOCK(sll_lock);

	/*
	 * Check if fast_node translates to a valid physical node number.
	 */
	if ( nx_map_node_proc(u.u_procp, node) == -1 ) {
		/*
		 * Lock data structures again.
		 */
		SLL_LOCK(sll_lock);

		SLL_DEBUGF("nx_map_node_proc returns -1 ==> illeagal value for fast_node\n");
		/*
		 * Mark fast_node as invalid if fast_node has not
		 * changed in the meantime.
		 */
		if ( node == fast_node ) {
			fast_node = -1;
		}

		/*
		 * Return error.
		 */
		SLL_UNLOCK(sll_lock);
		return(-1);
	}
#else /* NX */
	/*
	 * Check if we got a valid node.
	 */
	if ( ! is_node_valid(fast_node) ) {
		SLL_DEBUGF("illeagal value for fast_node\n");
		/*
		 * Mark fast_node as invalid.
		 */
		fast_node = -1;

		/*
		 * Return error.
		 */
		SLL_UNLOCK(sll_lock);
		return(-1);
	}

	/*
	 * Unlock data structures.
	 */
	SLL_UNLOCK(sll_lock);
#endif /* NX */

	/*
	 * fast_node is OK.
	 */
	return(0);
}


/*
 * Name:
 *	sll_has_shm() - check if process has shared memory segment
 *
 * Synopsis:
 *	boolean_t	sll_has_shm();
 *
 * Parameters:
 *	none.
 *
 * Description:
 *	sll_has_shm() checks if the calling process has a shared memory
 *	segment attached.
 *	
 * Return Values:
 *	TRUE if  calling process has a shared memory segment attached,
 *	FALSE else.
 *		
 */
boolean_t	sll_has_shm()
{

	struct vm_mmap	*cur;
	struct utask	*utask = &u.uu_procp->p_utask;

	/*
	 * Have we got a SYSV shm segment?
	 */
	if ( u.u_shmsegs > 0 ) {
		SLL_DEBUGF("u.u_shmsegs > 0\n");
		return(TRUE);
	}

	/*
	 * Have we got a BSD mmap shm segment?
	 */
	U_MMAP_READ_LOCK(utask);
	for (cur = utask->uu_mmap; cur; cur = cur->link) {
		if ( (cur->flags & VM_MMAP_FORCE_KEEP_ON_EXEC) == 0 ) {
			U_MMAP_UNLOCK(utask);
			SLL_DEBUGF("mmap SHARED\n");
			return(TRUE);
		}
	}
	U_MMAP_UNLOCK(utask);


	/*
	 * XXXSTXXX the following check is a workaround for a LOCUS bug
	 * that makes the check for u.u_shmsegs (above) non-functional.
	 * It is slightly faster than the other possible work-around that
	 * is commented out below but has the disadvantage that it may
	 * prevent processes from being
	 * load leveled that only use SYSV semaphores or message queues.
	 */
	if ( u.u_svipc_flag != 0 ) {
		SLL_DEBUGF("u.u_svipc_flag != 0\n");
		return(TRUE);
	}

/*	/*
/*	 * XXXSTXXX the following check is a workaround for a LOCUS bug
/*	 * that makes the check for u.u_shmsegs non-functional.
/*	 */
/*	{
/*		vm_address_t	address = 0;
/*		vm_size_t	size = 0xFFFFFFFF;
/*		vm_prot_t	protection;
/*		vm_prot_t	max_protection;
/*		vm_inherit_t	inheritance;
/*		boolean_t	shared;
/*		mach_port_t	object_name;
/*		vm_offset_t	offset;
/*
/*		ux_server_thread_blocking();
/*		while ( vm_region(u.uu_procp->p_task, &address, &size,
/*					&protection, &max_protection,
/*					&inheritance, &shared,
/*					&object_name, &offset)
/*				== KERN_SUCCESS ) {
/*			if ( inheritance == VM_INHERIT_SHARE || shared ) {
/*				ux_server_thread_unblocking();
/*				return(TRUE);
/*			}
/*			address += size;
/*			size = 0xFFFFFFFF;
/*		}
/*		ux_server_thread_unblocking();
/*	}
*/
	/*
	 * XXXSTXXX end of work-around for LOCUS bug.
	 */

	return(FALSE);
}


/*
 * Name:
 *	sll_fast_node() - determine most lightly loaded node.
 *
 * Synopsis:
 *	node_t	 sll_fast_node();
 *
 * Parameters:
 *	none.
 *
 * Description:
 *	sll_fast_node() determines the most lightly loaded node and returns
 *	it's node number. If no load information is available -1 is returned.
 *	
 * Return Values:
 *	Node number of most lightly loaded node or -1 if no load information
 *	is available.
 *		
 */
node_t	sll_fast_node()
{
	node_t		node;
	node_t		phys_node;
	extern node_t	this_node;


	/*
	 * Lock data structures.
	 */
	SLL_LOCK(sll_lock);

	/*
	 * Check if remote process creation is enabled.
	 */
	if ( ! fork_remote_enabled ) {
		SLL_UNLOCK(sll_lock);
		return(-1);
	}

	/*
	 * Check if load information is available.
	 */
	if ( fast_node == -1 ) {
		SLL_UNLOCK(sll_lock);
		return(-1);
	}

	/*
	 * Check if load information is timed out.
	 */
	if ( sll_timeout() ) {
		SLL_UNLOCK(sll_lock);
		return(-1);
	}

	/*
	 * Determine the most lightly loaded node.
	 */
	node = fast_node;

	/*
	 * Unlock data structures.
	 */
	SLL_UNLOCK(sll_lock);

#ifdef NX
	/*
	 * Translate logical to physical node number.
	 */
	phys_node = nx_map_node_proc(u.u_procp, node);
	if ( phys_node == -1 ) {
		return(-1);
	}
#else /* NX */
	phys_node = node;
#endif /* NX */

	/*
	 * Check if not the same as the local node.
	 */
	if ( phys_node == this_node ) {
		return(-1);
	}

	/*
	 * Check if we may rfork() shared memory
	 */
	if ( ! fork_remote_shm && sll_has_shm() ) {
		SLL_DEBUGF("process has shm ==> node = -1\n");
		return(-1);
	}

	/*
	 * We have been successful so far - return the node number of
	 * the most lightly loaded node.
	 */
	return(node);
}
