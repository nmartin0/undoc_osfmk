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
 * $Id: core.c,v 1.13 1995/04/05 07:20:19 johannes Exp $
 *
 * HISTORY
 * $Log: core.c,v $
 * Revision 1.13  1995/04/05  07:20:19  johannes
 * adjust_thread_info(): new routine for reading user registers from user
 *                       stack, if thread is currently in the emulator,
 *                       and storing it in thread state
 *                       (use of EMUL_INIT_STACK_SIZE to find user stack
 *                       via emulator stack)
 * get_thread_info(): calling adjust_thread_info()
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: low
 *  Benefit or PTS #: 11007 (better fix)
 *  Testing: simple test with kill; kill from pthread; corefile EATs
 *  Module(s): server/paracore/core.c (adjust_thread_info)
 *             emulator/emul_stack.h (define for emulator stack size)
 *             emulator/emul_stack_alloc.c (use of new define)
 *
 * Revision 1.12  1995/04/05  07:06:52  johannes
 * fixes of minor bugs: wrong error check in get_nx_ptype()
 *                      #ifdef MAP_UAREA instead of #if MAP_UAREA
 *
 * Revision 1.11  1995/03/07  09:58:56  johannes
 * search_dir_entry: if an error occurs in remote_lookup of an directory entry
 * 		  this entry will be ignored and searching is continued
 *                   with the next entry
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: 12613
 *  Testing: configuration described in the PTS report
 *  Module(s): server/paracore/core.c
 *
 * Revision 1.10  1994/11/18  20:38:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/10/25  17:07:18  johannes
 * added warnings (done with uprintf) to the user in case of wrong core actions
 * or problems to create/write core/allocinfo files
 *
 *  Reviewer: Karla (user view)
 *  Risk: low
 *  Benefit or PTS #: 10667 and warning for problems with core files
 *  Testing: developer testing
 *  Module(s): server/paracore/allocinfo.c
 * 	    server/paracore/core.c
 * 	    server/paracore/dump.c
 * 	    server/paracore/dvp_pvpcore.c
 *
 * Revision 1.8  1994/09/27  07:53:22  johannes
 * para_core(): additional coredumpsize checks before creating core file and
 * 	     after writing header, correct coredumpsize check while
 *              writing regions (7509, 10344)
 *              no error output when coredumpsize limit is exceeded (10838)
 * get_path():  correct typo in order to deallocate last looked up file port
 * 	     if root is reached (10487)
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: 7509, 10344, 10838, 10487
 *  Testing: developer testing
 *  Module(s): server/paracore/core.c
 * 	    server/paracore/allocinfo.c
 *
 * Revision 1.7  1994/09/21  08:37:38  johannes
 * get_region_info(): store stack of main user thread always
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: 11007
 *  Testing: special test case
 *  Module(s): svr/server/paracore/core.c
 *
 * Revision 1.6  1994/09/13  16:26:28  johannes
 * removed not changed static functions core_write() and isempty() [9266]
 * get_thread_info(): use new utask field 'uu_faulting_thread' to determine
 * 		   index of "active" thread to write 'c_activethread' [10486]
 * get_region_info(): changed joining stack regions and rounding r_sav_off to
 * 		   multiple of page size [10681, 10787]
 * changed some debug output
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: 9266, 10481, 10486, 10681, 10787
 *  Testing: developer tests, corefile EAT
 *  Module(s): svr/server/bsd/kern_sig.c [9266]
 *             svr/server/sys/user.h [10486]
 *             svr/server/bsd/kern_fork.c [10486]
 *             svr/server/bsd/mach_signal.c [10481, 10486]
 *             svr/server/paracore/core.c
 *
 * Revision 1.5  1994/07/27  16:56:59  johannes
 * In para_core() from the new information in the utask the file name is
 * written to the core file. From the directory ports the path names are
 * constructed and also stored in the core file.
 *
 * The new function get_path() constructs the a path name out of a given port
 * and relative to a given root port. This is done by remote_lookup() in all
 * directories up to the root.
 *
 * The new function search_dir_entry() is used to get the name of a file
 * by the given file port and the root port. This is done by calling
 * remote_readdir(), looking up the found files and comparing it with the
 * given file port.
 *
 *  Reviewer: Nandini
 *  Risk: H
 *  Benefit or PTS #: information for absolute exec path in core files
 *  Testing: developer
 *  Module(s): server/sys: user.h
 *             server/bsd: kern_exec.c, kern_exit.c, kern_fork.c
 *             server/tnc: pvps.ops, tnc.defs, rtask_server.c
 *                         rtask_cli_pproc.c, rtask_cli_vproc.c
 *                         rtask_svr_pproc.c, rtask_svr_vproc.c
 *                         chkpnt_vproc.c
 *             server/paracore: core.c
 *
 */

#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>

#ifdef OSF1_SERVER
#include <machine/vmparam.h>
#include <sys/fcntl.h>
#include <sys/vnode.h>
#include <kern/kalloc.h>
#include <kern/parallel.h>
#include <tnc/dpvproc.h>
#endif

#if defined MACH && defined NX && defined __i860__
#include <i860paragon/mcmsg/mcmsg_appl.h>
#endif

#if defined MACH && defined __i860__
#include <mach/i860/thread_status.h>
#endif

#ifdef MACH
#include <mach/vm_prot.h>
#endif

#include <uxkern/bsd_types.h>
#include <sys/core.h>
#include <paracore/core_types.h>


#ifdef NX
static void
get_nx_info(const struct proc 	*p,
	     long		*numnodes,
	     long		*mynode,
	     APPLINFO_T		*applinfo)
{
	register struct vproc 	*vp;
	register struct pvproc 	*pvp;
        LP_MAP_T                nodelist = (LP_MAP_T) NULL;
        int                     i, err;

	vp = p->p_vproc;
	ASSERT(vp);
	VPROC_HOLD(vp, "get_nx_info");
	pvp = (struct pvproc*) PVP(vp);        
	
	if (nx_in_partition(vp)) {
            CORE_DEBUGF("get_nx_info: call nx_get_info()\n");
            if (nx_get_info(pvp,
                            applinfo,
                            &nodelist,
                            numnodes,
                            TRUE,
                            0,
                            &err)
                == KERN_SUCCESS) {
                for(i = 0; i < *numnodes; i++)
                    if (this_node == nodelist[i]) {
                        *mynode = i;
                        break;
                    }
                vm_deallocate(mach_task_self(), (vm_address_t)nodelist,
                              (*numnodes * sizeof(LP_MAP_ENTRY_T)));
            }
        }
	
	VPROC_RELEASE(vp, "get_nx_info");
}

/*
 * prepend() tacks a directory name onto the front of a pathname.
 */
static char *
prepend(dirname, pathname, pathsize)
	register char *dirname;
	register char *pathname;
	register int  *pathsize;
{
	register int i;			/* directory name size counter */

	for (i = 0; *dirname != '\0'; i++, dirname++)
		continue;
	if (((*pathsize) += i) < PATH_MAX)
		while (i-- > 0)
			*--pathname = *--dirname;
	return (pathname);
}

/*
 * NAME:	search_dir_entry
 *                                                                    
 * FUNCTION:	searchs a entry (given by a port) in
 *		a directory (given by a port)
 *		returns the file name in the directory
 *                                                                    
 * RETURN VALUE DESCRIPTION:	0 on success, errno otherwise
 */  
static int
search_dir_entry(const mach_port_t	dir_port,
		 const mach_port_t	file_port,
		 char			*file_name)
{
	char 				*buf;
	int				bufsize = SMALL_ARRAY_LIMIT;
    	struct dirent 			*entry;
    	off_t				offset;
    	int				error, resid, data_read, loc;
        register struct nameidata 	*ndp = &u.u_nd;
	kern_return_t			ret;
	mach_port_t 			entry_port;
	enum vtype			type;
	dev_t 				rdev;
	node_t				rnode;
	boolean_t			found = FALSE;
    	
    	buf = kalloc(bufsize);
    	if (buf == NULL) {
    		printf("search_dir_entry: kalloc of buf failed\n");
    		return -1;
    	}
    	
    	loc = 0;
    	offset = 0;
    	error = 0;
    		
    	while (! found) {
    		/*
    		 * Read some core directory entries.
    		 */
    		if (loc == 0) {
#ifdef PARACORE_PATH_DEBUG
       			CORE_DEBUGF("search_dir_entry: bufsize=%d, offset=%d",
    			            bufsize, offset);
#endif
    			
    			error = remote_vnreaddir(dir_port,
    						 buf,
    						 bufsize,
    						 &offset,
    						 &resid);
    			data_read = bufsize - resid;

#ifdef PARACORE_PATH_DEBUG
    			CORE_DEBUGF("->%d, resid=%d, data_read=%d\n", 
    			            offset, resid, data_read);
#endif
    			
	    		if (error) {
	    			printf("search_dir_entry: readdir failed(error=0x%x)\n", 
	    			       error);
	    			break;
	    		}
	    		
	    		if (data_read == 0)
	    			break;
  		}

		/*
		 * Take a directory entry.
		 */
		entry = (struct dirent *)(buf + loc);
    		
    		if (entry->d_reclen <= 0 ||
    		    loc + entry->d_reclen > data_read) {
    		    	printf("search_dir_entry: invalid dir entry\n");
    		    	error = EINVAL;
    		    	break;
    		}
    		
#ifdef PARACORE_PATH_DEBUG
    		CORE_DEBUGF("search_dir_entry: in directory found: (%d)%s\n",
    		            entry->d_ino, entry->d_name);
#endif
    		
		/*
		 * Lookup the directory entry.
		 */
		ndp->ni_forwport = dir_port;
		if (error = remote_lookup(ndp, entry->d_name, 
				          LOOKUP | NOFOLLOW, 
				          &type, &rdev, &rnode, 
				          &entry_port)) {
#ifdef PARACORE_PATH_DEBUG
			CORE_DEBUGF("search_dir_entry: remote_lookup failed(0x%x)\n",
			            error);
#endif
		}
		else {
			/*
			 * Compare with given port.
			 */
			if (entry_port == file_port) {
				strcpy(file_name, entry->d_name);
				found = TRUE;
			}
				      
			/* 
			 * Deallocate port got from lookup.
			 */
			if (entry_port != MACH_PORT_NULL) {
				ret = mach_port_deallocate(mach_task_self(),
							   entry_port);
				if (ret != KERN_SUCCESS) {
					printf("search_dir_entry: Could not deallocate entry_port (error=0x%x)\n", ret);
				}
			}
		}
	
		/*
		 * Goto next core directory entry.
		 */
		loc += entry->d_reclen;
		
		/*
		 * If read entries are consumed, prepare next read.
		 */
		 if (loc >= data_read)
			loc = 0;
	}
	
	kfree(buf, bufsize);
	return error;
}

static int
get_path(mach_port_t 			root_port,
         mach_port_t 			cwd_port,
         char 				*pathname)
{
	path_name_t 			pathbuf;
	char 				*pnptr = &pathbuf[(sizeof pathbuf)-1];
	int 				pathsize;
	mach_port_t 			dir_port;
	mach_port_t 			file_port;
	path_name_t 			file_name;
        register struct nameidata 	*ndp = &u.u_nd;
        mach_port_t 			orig_root_port;
	enum vtype 			type;
	dev_t 				rdev;
	node_t 				rnode;
        int 				lookup_error, search_error;
        kern_return_t 			ret;
        boolean_t			no_parent;

	pathsize = 0;
	*pnptr = '\0';
	
	orig_root_port = ndp->ni_rdirport;
	ndp->ni_rdirport = root_port;
	
	dir_port = cwd_port;
	no_parent = FALSE;

	for (;;) {
		if (dir_port == root_port || no_parent)
			break;		/* reached root directory */
		
		file_port = dir_port;
		
		/*
		 * Lookup the parent directory.
		 */
		ndp->ni_forwport = file_port;
		if (lookup_error = remote_lookup(ndp, "..", 
				         	 LOOKUP | FOLLOW, 
				          	 &type, &rdev, &rnode, 
				          	 &dir_port))
			printf("get_path: remote_lookup failed(0x%x)\n",
			       lookup_error);
			       
		no_parent = (dir_port == file_port);
		
		/*
		 * Search corresponding directory entry.
		 */
		if (!lookup_error && !no_parent)
		    if (search_error = search_dir_entry(dir_port,
					                file_port,
					                file_name))
			printf("get_path: search_dir_entry failed(0x%x)\n",
			       search_error);
		    else {
			    pnptr = prepend(file_name, pnptr, &pathsize);
			    pnptr = prepend("/", pnptr, &pathsize);
#ifdef PARACORE_PATH_DEBUG
			    CORE_DEBUGF("get_path: path=%s\n", pnptr);
#endif
		    }
		
		/* 
		 * Deallocate port got from last lookup.
		 */
		if (file_port != MACH_PORT_NULL &&
		    file_port != cwd_port) {
			ret = mach_port_deallocate(mach_task_self(),
						   file_port);
			if (ret != KERN_SUCCESS) {
				printf("get_path: Could not deallocate file_port (error=0x%x)\n", ret);
			}
		}
		
		/* 
		 * Deallocate port got from this lookup,
		 * if error or root found.
		 */
		if (!lookup_error && 
		    (search_error || 
		     no_parent || dir_port == root_port)) {
			ret = mach_port_deallocate(mach_task_self(),
						   dir_port);
			if (ret != KERN_SUCCESS) {
				printf("get_path: Could not deallocate dir_port (error=0x%x)\n", ret);
			}
		}
		
		/*
		 * Return if an error has occurred.
		 */
		if (lookup_error)
			return lookup_error;
		
		if (search_error)
			return search_error;
	}
	
	ndp->ni_rdirport = orig_root_port;
	
	if (*pnptr == '\0')		/* current dir == root dir */
		strcpy(pathname, "/");
	else
		strcpy(pathname, pnptr);
		
	return 0;
}

static void
get_nx_ptype(struct proc 	*p,
	      long		*ptype)
{
	struct utask		*utaskp = &p->p_utask;
	ipd_msg_info_t		*ipd_msg_info_p;
	long			*read_ptype;
	mach_msg_type_number_t 	ipd_msg_info_size, ptype_size;
	int			error;
	
	*ptype = -1; /* invalid ptype */
	
        CORE_DEBUGF("get_nx_ptype: utaskp->uu_ipd_msg_info=0x%x\n",
                    utaskp->uu_ipd_msg_info);
	
	if (utaskp->uu_ipd_msg_info == 0)
		return;

	if ((error = vm_read(p->p_task, 
	     		    (vm_address_t) utaskp->uu_ipd_msg_info, 
			    (vm_size_t) sizeof(ipd_msg_info_t), 
	     		    (vm_offset_t*) &ipd_msg_info_p, 
    			    &ipd_msg_info_size)) 
			    != KERN_SUCCESS) {
		printf("get_nx_ptype: vm_read failed(0x%x)\n", error);
		return;
	}

/*
        CORE_DEBUGF("get_nx_ptype: ipd_msg_info_p->buffer_p=0x%x\n",
                    ipd_msg_info_p->buffer_p);
        CORE_DEBUGF("get_nx_ptype: ipd_msg_info_p->buffer_size=0x%x\n",
                    ipd_msg_info_p->buffer_size);
        CORE_DEBUGF("get_nx_ptype: ipd_msg_info_p->nxreq=0x%x\n",
                    ipd_msg_info_p->nxreq);
        CORE_DEBUGF("get_nx_ptype: ipd_msg_info_p->nxreq_size=0x%x\n",
                    ipd_msg_info_p->nxreq_size);
        CORE_DEBUGF("get_nx_ptype: ipd_msg_info_p->ipd_plist=0x%x\n",
                    ipd_msg_info_p->ipd_plist);
        CORE_DEBUGF("get_nx_ptype: ipd_msg_info_p->num_ptypes=0x%x\n",
                    ipd_msg_info_p->num_ptypes);
        CORE_DEBUGF("get_nx_ptype: ipd_msg_info_p->current_ptype=0x%x\n",
                    ipd_msg_info_p->current_ptype);
        CORE_DEBUGF("get_nx_ptype: ipd_msg_info_p->app_id=0x%x\n",
                    ipd_msg_info_p->app_id);
        CORE_DEBUGF("get_nx_ptype: ipd_msg_info_p->nodenum=0x%x\n",
                    ipd_msg_info_p->nodenum);
        CORE_DEBUGF("get_nx_ptype: ipd_msg_info_p->ipd_msg_aux=0x%x\n",
                    ipd_msg_info_p->ipd_msg_aux);
*/

	if ((error = vm_read(p->p_task, 
	     		    (vm_address_t) ipd_msg_info_p->current_ptype, 
			    (vm_size_t) sizeof(long), 
	     		    (vm_offset_t*) &read_ptype, 
    			    &ptype_size)) 
			    != KERN_SUCCESS) {
		printf("get_nx_ptype: vm_read failed(0x%x)\n", error);
	}
	else {
		CORE_DEBUGF("get_nx_ptype: *read_ptype=%d\n", *read_ptype);
		*ptype = *read_ptype;

		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) read_ptype,
				     (vm_size_t) ptype_size);
	}

	(void) vm_deallocate(mach_task_self(),
			     (vm_address_t) ipd_msg_info_p,
			     (vm_size_t) ipd_msg_info_size);
}
#endif /* NX */

static void
adjust_thread_info(const struct proc	    *p,
		   struct i860_thread_state *thread_state)
{
	vm_address_t 			sp = thread_state->sp;
	vm_address_t 			esp, *user_frame_p;
	mach_msg_type_number_t		user_frame_p_size;
    	struct emul_exception_frame 	*user_frame;
	mach_msg_type_number_t		user_frame_size;
	int				error;

	if (EMULATOR_BASE <= sp && sp <= EMULATOR_END) {
		CORE_DEBUGF("adjust_thread_info: sp=0x%x\n", sp);
		
		/* 
		 * ATTENTION: The following calculations depend
		 * 	      on the code in emulator/emul_vector.s
		 *	      at syscall entry _emul_common and
		 *	      also on do_emul_stack_allocate()
		 *	      in emulator/emul_stack_alloc.c.
		 *	      They are also based on the initial
		 * 	      emulator stack size and not on the
		 *	      real, but that is never changed(!?).
		 */
		esp = sp & ~(EMUL_INIT_STACK_SIZE - 1); /* bottom */
		esp += EMUL_INIT_STACK_SIZE;		/* top */
		esp -= sizeof(struct emul_stack_top);
		esp &= ~0xf;		/* align to 16 bytes */
		esp -= 32;		/* bottom of struct emul_stack */
		CORE_DEBUGF("adjust_thread_info: esp=0x%x\n", esp);
		
		/*
		 * Read the address of the user exception frame.
		 */
		if ((error = vm_read(p->p_task, 
		     		    esp, 
				    (vm_size_t) sizeof(vm_address_t), 
		     		    (vm_offset_t*) &user_frame_p, 
	    			    &user_frame_p_size)) 
				    != KERN_SUCCESS) {
			printf("adjust_thread_info: vm_read failed(0x%x)\n", error);
			return;
		}
		
		/*
		 * Read the user exception fram.
		 */
		if ((error = vm_read(p->p_task, 
		     		    *user_frame_p, 
				    (vm_size_t) sizeof(struct emul_exception_frame), 
		     		    (vm_offset_t*) &user_frame, 
	    			    &user_frame_size)) 
				    != KERN_SUCCESS) {
			printf("adjust_thread_info: vm_read failed(0x%x)\n", error);
		}
		else {
			/*
			 * If there is a valid user stack pointer,
			 * set the user thread state (no floating point!).
			 */
#ifdef __i860__
			if (user_frame->u_sp != 0) {
				bcopy((char *)&user_frame->u_r0, (char *)thread_state, 32*4 );
				thread_state->psr = user_frame->u_psr;
				thread_state->epsr = user_frame->u_epsr;
				thread_state->pc = user_frame->u_fir;
			}
#endif
			CORE_DEBUGF("adjust_thread_info: usp=0x%x\n", 
				    thread_state->sp);
	
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t) user_frame,
					     (vm_size_t) user_frame_size);
		}
	
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) user_frame_p,
				     (vm_size_t) user_frame_p_size);
	}
}

static int
get_thread_info(const struct proc 	 *p,
		struct i860_thread_state **thread_state,
		long			 *nthreads,
		long			 *activethread)
{
	thread_array_t			 thread_table;
	int 				 error, i;
	
	CORE_DEBUGF("get_thread_info: call task_threads\n");
	
	if ((error = task_threads(p->p_task, 
			 	  &thread_table, 
			 	  (mach_msg_type_number_t *) nthreads)) 
		     != KERN_SUCCESS) {
		CORE_DEBUGF("get_thread_info: task_threads failed(0x%x)\n", error);
		return error;
	}
	
	if (*nthreads <= 0)
		return KERN_FAILURE;
		
#ifdef __i860__
	*thread_state = (struct i860_thread_state *) 
		        kalloc(*nthreads * sizeof **thread_state);
#endif
	
	for (i = 0; i < *nthreads; i++) {
		unsigned int count;

		thread_suspend(thread_table[i]);
		thread_abort(thread_table[i]);

#ifdef __i860__
		count = i860_THREAD_STATE_COUNT;
		CORE_DEBUGF("get_thread_info: call thread_get_state\n");
		if (*thread_state != NULL)
			(void) thread_get_state(thread_table[i],
					        i860_THREAD_STATE,
					        (thread_state_t)&(*thread_state)[i],
					        &count);
#endif
		
		adjust_thread_info(p, &(*thread_state)[i]); 
		
		CORE_DEBUGF("get_thread_info: thread %d: 0x%x\n", i, thread_table[i]);
		CORE_DEBUGF("get_thread_info: thread %d: pc=0x%x\n", i, (*thread_state)[i].pc);
		CORE_DEBUGF("get_thread_info: thread %d: sp=0x%x\n", i, (*thread_state)[i].sp);
		
		if (thread_table[i] == p->p_utask.uu_faulting_thread)
			*activethread = i;
		
		(void) mach_port_deallocate(mach_task_self(),
					    thread_table[i]);
	}
	
	(void) vm_deallocate(mach_task_self(), 
			     (vm_address_t) thread_table, 
			     *nthreads * sizeof *thread_table );

	CORE_DEBUGF("get_thread_info: active_thread=%d\n", *activethread);
	
	return KERN_SUCCESS;
}

struct region_list {
	struct core_region_desc	desc;
	struct region_list 	*next
	};
	
static int
get_region_info(const struct proc	*p,
		const struct i860_thread_state *thread_state,
		const long		nthreads,
		const int		core_type,
		struct region_list	**region_info,
		long			*nregions)
{
	struct region_list 		**curr_region_elem;
	vm_address_t			addr_old, addr_new;
	vm_size_t			size_old, size_new;
	vm_prot_t			prot_old, prot_new; 
	vm_prot_t			max_prot_old, max_prot_new;
	vm_inherit_t			inheritance;
	boolean_t			shared;
	mach_port_t			object_name;
	vm_offset_t			object_offset;
	boolean_t 			stack_old = FALSE, stack_new;
	int				i;
	
	*nregions = 0;
	addr_new = 0;
	size_old = 0;
	curr_region_elem = region_info;
	
	ux_server_thread_blocking(); /* vm_region may block ? */
	
	while (vm_region(p->p_task,
			 &addr_new, &size_new,
			 &prot_new, &max_prot_new,
			 &inheritance, &shared,
			 &object_name, &object_offset)
	       == KERN_SUCCESS) {
		
		vm_address_t thread_sp = 0xFFFFFFFF;
		stack_new = FALSE;
		CORE_DEBUGF("get_region_info: search for minimum sp in region\n");
		for (i = 0; i < nthreads; i++) {
			vm_address_t sp = thread_state[i].sp;
			if (addr_new <= sp && sp < addr_new + size_new) {
			    	CORE_DEBUGF("get_region_info: sp=0x%x found\n", sp);
				stack_new = TRUE;
				if (thread_state[i].sp < thread_sp)
					thread_sp = sp;
			}
		}
		
		CORE_DEBUGF("get_region_info: region #%d: addr=0x%x, size=0x%x, stack=%s\n",
		            *nregions, addr_new, size_new, 
		            stack_new ? "TRUE" : "FALSE");
		CORE_DEBUGF("                       prot=0x%x, max_prot=0x%x,\n",
		            prot_new, max_prot_new);
		
		if (size_old != 0 &&
		    addr_old + size_old == addr_new &&
		    prot_old == prot_new &&
		    max_prot_old == max_prot_new) { 
			/* join with previous region */
			register struct region_list *reg = 
				*curr_region_elem;
			CORE_DEBUGF("get_region_info: join with previous region\n");
			size_old += size_new;
			if (stack_new) {
				reg->desc.r_sav_off = 
					trunc_page(thread_sp - addr_old);
				CORE_DEBUGF("get_region_info: sav_off=0x%x\n",
				       reg->desc.r_sav_off);
			} else if (reg->desc.r_sav_off == 
			           reg->desc.r_size) {
				reg->desc.r_sav_off = size_old;
				CORE_DEBUGF("get_region_info: sav_off=0x%x\n",
				       reg->desc.r_sav_off);
			}
			reg->desc.r_size = size_old;			          
		}
		else {	/* new region descriptor */
			register struct region_list *reg;

			CORE_DEBUGF("get_region_info: new region descriptor\n");
			if ((reg = (struct region_list*)
				   kalloc(sizeof(struct region_list)))
			           == NULL)
				return KERN_FAILURE;
			
			(*nregions)++;
			
			reg->desc.r_offset = 0;
			
			reg->desc.r_vaddr = addr_new;
			reg->desc.r_size = size_new;

			if (stack_new)
				/* save region partly */ 
				reg->desc.r_sav_off = 
					trunc_page(thread_sp - addr_new);
			else if ((core_type == CORE_TYPE_FULL ||
			          addr_new >= USRSTACK - MAXSSIZ &&
			          addr_new < USRSTACK) &&
			         prot_new & VM_PROT_WRITE)
			         /* save region completely */
			         reg->desc.r_sav_off = 0;
			else
				/* don't save region */
				reg->desc.r_sav_off = size_new;
			CORE_DEBUGF("get_region_info: sav_off=0x%x\n",
			            reg->desc.r_sav_off);
			
			reg->desc.r_prot = prot_new;
			reg->desc.r_max_prot = max_prot_new;
			
			reg->next = NULL;
			if (*curr_region_elem == NULL)
				*curr_region_elem = reg;
			else {
				(*curr_region_elem)->next = reg;
				curr_region_elem = &(*curr_region_elem)->next;
			}
			
			addr_old = addr_new;
			size_old = size_new;
			prot_old = prot_new;
			max_prot_old = max_prot_new;
			stack_old = stack_new;
		}
	
		addr_new += size_new;
	}
	ux_server_thread_unblocking();
	
	return KERN_SUCCESS;
}

/*
 * Create a core image on the file `core_file`.
 * If you are looking for protection glitches,
 * there are probably a wealth of them here
 * when this occurs to a suid command.
 *
 * Return:
 *	-1		if we did not attempt do dump core
 *	0		if we successfully dumped core
 *	>0 (errno)	if we failed dumping core
 */

int paracoreprint = 0;	/* XXX Just for debugging */

para_core(const mach_port_t	root_dir,
	  const mach_port_t	core_dir,
	  const path_name_t	core_file,
     	  const int		core_type,
     	  const int		signo,
     	  const struct timeval	*fault_time)
{
	mach_port_t		vp = MACH_PORT_NULL;
	struct vattr 		vattr;
	int 			error, unlink_error;
	register struct nameidata *ndp = &u.u_nd;
	mach_port_t		orig_root_port, orig_cwd_port, orig_fwd_port;
	enum vtype		type;
	struct proc		*p;
	off_t			current_offset = 0, desc_offset;
	struct core_hdr		header;
	struct core_proc_info 	proc_info;
	path_name_t		root_path;
	path_name_t		cwd_path;
	APPLINFO_T		appl_info;
	struct region_list 	*region_info = NULL, **curr_region_elem;
#ifdef __i860__
	struct i860_thread_state *thread_state = NULL;
#endif
	boolean_t		paracore_file = strncmp(core_file, "core", 5);

	ASSERT(syscall_on_master());
	p = u.u_procp;
	
	if (paracoreprint)	/* XXX Just for debugging */
		uprintf("(%s) pid %d uid %d dumps core to %s\n",
		       u.u_comm, u.u_procp -> p_pid, p->p_ruid, core_file);

	PROC_LOCK(p);
	if (p->p_flag & SXONLY) {
		PROC_UNLOCK(p);
		return -1;
	}
	crfree(u.u_cred);
	u.u_cred = p->p_rcred;
	crhold(p->p_rcred);
	p->p_rcred->cr_uid = p->p_ruid;
	p->p_rcred->cr_gid = p->p_rgid;
	PROC_UNLOCK(p);

	/* check core file size limit */
	if (sizeof header + sizeof proc_info + sizeof appl_info >
	    u.u_rlimit[RLIMIT_CORE].rlim_cur) {
		CORE_DEBUGF("para_core: core file size limit exceeded\n");
		return -1;
	}
	
	orig_root_port = ndp->ni_rdirport;
	orig_cwd_port = ndp->ni_cdirport;
	orig_fwd_port = ndp->ni_forwport;
	
	ndp->ni_rdirport = root_dir;
	ndp->ni_cdirport = core_dir;
	ndp->ni_forwport = core_dir;
	ndp->ni_dirp = (caddr_t) core_file;
	ndp->ni_segflg = UIO_SYSSPACE;

#if     SEC_BASE
	error = remote_vnopen(ndp, FCREAT|FWRITE, SEC_CORE_MODE, &vp, &type);
#else
	error = remote_vnopen(ndp, FCREAT|FWRITE, 0644, &vp, &type);
#endif
	
	if (error) {
		printf("para_core: remote_vnopen failed(0x%x)\n", error);
		if (paracore_file)
			uprintf("Cannot create core file \"%s\".\n", core_file);
		goto out;
	}
	
	if (type != VREG) {
		error = EFAULT;
		if (paracore_file)
			uprintf("Cannot create core file \"%s\".\n", core_file);
		goto out;
	}
	error = remote_getattr(vp, &vattr);
	if (error || vattr.va_nlink != 1) {
		if (error == 0)
			error = EFAULT;
		if (paracore_file)
			uprintf("Cannot create core file \"%s\".\n", core_file);
		goto out;
	}
	/*
	 * 4.4 has some code here ifdef MMAP to unmap devices.
	 * We have not included that code.
	 */
	vattr_null(&vattr);
	vattr.va_size = 0;
	error = remote_setattr(vp, &vattr);

	U_HANDY_LOCK();
	u.u_acflag.fi_flag |= ACORE;
	U_HANDY_UNLOCK();

	/* fill core header */
	header.c_magic = CORE_MAGIC;
	header.c_swap = CORE_SWAP;
	header.c_version = CORE_VERSION;
	header.c_type = core_type;
	header.c_timdat = *fault_time;
	header.c_signo = signo;
	header.c_sigcode = u.u_code;

#ifdef NX
	get_nx_info(p, 
		    &header.c_numnodes, 
		    &header.c_node,
		    &appl_info);
	
	CORE_DEBUGF("para_core: header.c_numnodes=%d\n", header.c_numnodes);
	CORE_DEBUGF("para_core: header.c_node=%d\n", header.c_node);
	
	if (nx_application(p)) {
		header.c_numnodes--; /* exclude proxy */
		get_nx_ptype(p, &header.c_ptype);
	}
	else 
		header.c_numnodes = -1;
	
	CORE_DEBUGF("para_core: header.c_ptype=%d\n", header.c_ptype);
#endif /* NX */
	
	/* header written later */
	current_offset += sizeof header;
	
	/* proc_info */
	header.c_procinfo = current_offset;
	proc_info.c_pid = p->p_pid;
	proc_info.c_ppid = p->p_ppid;
	proc_info.c_pgid = p->p_pgid;
	/* proc_info written later */
	current_offset += sizeof proc_info;

		/* prgname */
	proc_info.c_prgname = current_offset;
	
	CORE_DEBUGF("prgname=%s\n", p->p_utask.uu_exec_prg_name);
	proc_info.c_prglen = strlen(p->p_utask.uu_exec_prg_name) + 1;
	
	CORE_DEBUGF("para_core: write program name\n");
 	if (error = remote_vnrdwr(UIO_WRITE, vp, 
 				  (caddr_t)p->p_utask.uu_exec_prg_name, 
			          proc_info.c_prglen, proc_info.c_prgname,
			          IO_UNIT,  (int *)0))
		goto error_out;
	
	current_offset += proc_info.c_prglen;
	
		/* rootname */
	proc_info.c_rootname = current_offset;
	
	if (p->p_utask.uu_exec_utnd.utnd_rdir.vpx_port == MACH_PORT_NULL)
		/* root not set, assume "/" */
		strcpy(root_path, "/");
	else
		if (error = get_path(root_dir,
				     p->p_utask.uu_exec_utnd.utnd_rdir.vpx_port,
				     root_path))
			goto error_out;

	CORE_DEBUGF("root_path=%s\n", root_path);
	proc_info.c_rootlen = strlen(root_path) + 1;
	
	CORE_DEBUGF("para_core: write root path\n");
 	if (error = remote_vnrdwr(UIO_WRITE, vp, (caddr_t)root_path, 
			          proc_info.c_rootlen, proc_info.c_rootname,
			          IO_UNIT,  (int *)0))
		goto error_out;
	
	current_offset += proc_info.c_rootlen;
	
		/* cwdname */
	proc_info.c_cwdname = current_offset;
	
	if (p->p_utask.uu_exec_utnd.utnd_cdir.vpx_port == MACH_PORT_NULL)
		/* cwd not yet set, take root */
		strcpy(cwd_path, "/");
	else
		if (error = get_path(p->p_utask.uu_exec_utnd.utnd_rdir.vpx_port,
			     	     p->p_utask.uu_exec_utnd.utnd_cdir.vpx_port,
			     	     cwd_path))
			goto error_out;

	CORE_DEBUGF("cwd_path=%s\n", cwd_path);
	proc_info.c_cwdlen = strlen(cwd_path) + 1;
	
	CORE_DEBUGF("para_core: write cwd path\n");
 	if (error = remote_vnrdwr(UIO_WRITE, vp, (caddr_t)cwd_path, 
			          proc_info.c_cwdlen, proc_info.c_cwdname,
			          IO_UNIT,  (int *)0))
		goto error_out;
	
	current_offset += proc_info.c_cwdlen;
	
	/* write proc_info */
	CORE_DEBUGF("para_core: write proc_info\n");
 	if (error = remote_vnrdwr(UIO_WRITE, vp, (caddr_t)&proc_info, 
			      sizeof proc_info, header.c_procinfo,
			      IO_UNIT,  (int *)0))
		goto error_out;
	
	/* appl_info */
	header.c_applinfo = current_offset;
	CORE_DEBUGF("para_core: write appl_info\n");
 	if (error = remote_vnrdwr(UIO_WRITE, vp, (caddr_t)&appl_info, 
			      sizeof appl_info, current_offset,
			      IO_UNIT,  (int *)0))
		goto error_out;
	current_offset += sizeof appl_info;
		
	/* threads */
	if ((error = get_thread_info(p, 
				     &thread_state, 
				     &header.c_nthreads,
				     &header.c_activethread))
		    != KERN_SUCCESS)
		goto error_out;
		
	if (thread_state == NULL) {
		error = KERN_FAILURE;
		goto error_out;
	}
	/* thread_state written later */

	/* regions descriptors */
	if ((error = get_region_info(p,
				     thread_state,
				     header.c_nthreads,
				     core_type,
				     &region_info,
				     &header.c_nregions))
		    != KERN_SUCCESS)
		goto error_out;
			
	header.c_firstregion= current_offset;
	/* region descriptors written later */
	current_offset += header.c_nregions * 
			  sizeof(struct core_region_desc);
			  
	/* check core file size limit */
	if (current_offset + 
	    header.c_nthreads * sizeof *thread_state >
	    u.u_rlimit[RLIMIT_CORE].rlim_cur) {
		CORE_DEBUGF("para_core: core file size limit exceeded\n");
		error = -1;
		goto error_out;
	}
	
	/* write thread_state */
	CORE_DEBUGF("para_core: write thread_state\n");
  	if (error = remote_vnrdwr(UIO_WRITE, vp, (caddr_t)thread_state, 
				  header.c_nthreads * sizeof *thread_state, 
				  current_offset,
				  IO_UNIT,  (int *)0))
		goto error_out;
	header.c_firstthread = current_offset;
	current_offset += header.c_nthreads * sizeof *thread_state;
	
	/* write header */
	CORE_DEBUGF("para_core: write header\n");
 	if (error = remote_vnrdwr(UIO_WRITE, vp, (caddr_t)&header, 
			          sizeof header, (off_t)0,
			          IO_UNIT,  (int *)0))
		goto error_out;

	/* regions */
	desc_offset = header.c_firstregion;
	curr_region_elem = &region_info;
	while (*curr_region_elem != NULL) {
		register struct region_list *reg = *curr_region_elem;
		
		/* check core file size limit */
		if (current_offset + 
		    reg->desc.r_size - reg->desc.r_sav_off >
		    u.u_rlimit[RLIMIT_CORE].rlim_cur) {
			CORE_DEBUGF("para_core: core file size limit exceeded\n");
			error = -1;
			goto error_out;
		}
		
		/* write region */
		if (reg->desc.r_sav_off != reg->desc.r_size) {
		    	CORE_DEBUGF("para_core: write region: addr=0x%x, written size=0x%x\n",
		    	            reg->desc.r_vaddr + reg->desc.r_sav_off, 
		    	            reg->desc.r_size - reg->desc.r_sav_off);
			if (error = core_write(vp, ndp->ni_cred, 
					       reg->desc.r_vaddr + reg->desc.r_sav_off,
					       reg->desc.r_size - reg->desc.r_sav_off,
		    			       current_offset))
		    		goto error_out;
		    	reg->desc.r_offset = current_offset;
		    	current_offset += reg->desc.r_size - 
		    			  reg->desc.r_sav_off;
		}
	    	
	    	/* write region descriptor */
		CORE_DEBUGF("para_core: write region descriptor @ 0x%x\n",
		            reg->desc.r_vaddr);
	 	if (error = remote_vnrdwr(UIO_WRITE, vp, 
	 				  (caddr_t)&reg->desc, 
				          sizeof reg->desc, 
				          (off_t)desc_offset,
				          IO_UNIT,  (int *)0))
			goto error_out;

		/* goto next region */
		desc_offset += sizeof reg->desc;		
		curr_region_elem = &reg->next;
	}
	
	goto out;
		
error_out:
	if (error != -1) {
		printf("para_core: core dump failed(0x%x)\n", error);
		if (paracore_file)
			uprintf("Cannot dump core to file \"%s\".\n", core_file);
	}
	
	/* delete not completely written core file */
	ndp->ni_rdirport = root_dir;
	ndp->ni_cdirport = core_dir;
	ndp->ni_forwport = core_dir;
	ndp->ni_dirp = (caddr_t) core_file;
	
	if (unlink_error = remote_unlink(ndp))
		printf("para_core: remote_unlink(%s) failed(0x%x)\n", 
		       core_file, unlink_error);
	
out:
	if (thread_state != NULL) {
		kfree(thread_state, header.c_nthreads * 
				    sizeof *thread_state);
	}
	
	curr_region_elem = &region_info;
	while (*curr_region_elem != NULL) {
		register struct region_list *reg = *curr_region_elem;
		curr_region_elem = &reg->next;
		kfree(reg, sizeof *reg);
	}
	
	if (vp != MACH_PORT_NULL)
		mach_port_deallocate(mach_task_self(), vp);
	
	ndp->ni_rdirport = orig_root_port;
	ndp->ni_cdirport = orig_cwd_port;
	ndp->ni_forwport = orig_fwd_port;
	
	return error;
}
