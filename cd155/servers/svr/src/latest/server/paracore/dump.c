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
 * $Id: dump.c,v 1.10 1994/11/18 20:38:04 mtm Exp $
 *
 * HISTORY
 * $Log: dump.c,v $
 * Revision 1.10  1994/11/18  20:38:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/10/25  17:07:30  johannes
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
 * Revision 1.8  1994/10/04  17:43:51  stefan
 * Checked this in for johannes:
 * In order to prevent that such files are created (until PFS
 * implementation has been changed) it is checked if the directory
 * where the core dump has to be written to is not in the PFS.
 * For this the remote_lookup() function is extended to notify
 * in the rdev parameter that a directory is in PFS. Thus PFS property
 * can be check by calling remote_lookup.
 *
 *  Reviewer: stefan
 *  Risk: low
 *  Benefit or PTS #: 10483
 *  Testing: developer testing
 *  Module(s): svr/server/paracore/dump.c
 *             svr/server/uxkern/fsvr_msg.c
 *
 * Revision 1.7  1994/09/29  10:22:37  johannes
 * para_core_dump(): cleanup and remove existing core directory
 *                   when core dumping of a serial application
 * cleanup_core_dir(): remove corefiles with 1-digit pids also (10706)
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: 10705, 10706
 *  Testing: developer testing, corefile EAT
 *           compiling/running with other configurations (LITE, test,
 *           no PARACORE)
 *  Module(s): svr/server/paracore/dump.c
 *             svr/server/uxkern/fsvr_msg.c
 *             svr/server/uxkern/fsvr.defs
 *             svr/server/uxkern/fsvr_server_side.c
 *
 * Revision 1.6  1994/07/27  15:53:16  johannes
 * changed header
 * added a missing VPROC_HOLD()
 * (caused a hang of the shell, running a faulting application,
 * under some circumstances)
 *
 */

#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>

#ifdef OSF1_SERVER
#include <sys/fcntl.h>
#include <sys/vnode.h>
#include <tnc/dpvproc.h>
#include <kern/kalloc.h>
#endif

#include <uxkern/bsd_types.h>
#include <sys/core.h>
#include <paracore/core_types.h>

#ifdef COREDEBUG
/*
 * Debug on/off
 */
int core_debug = 1;
#endif /* COREDEBUG */


/*
 * NAME:	nvmatch
 *                                                                    
 * FUNCTION:	see if 's1' matches 's2'.  's1' is either "name" or
 *		"name=value" and 's2' is "name=value".
 *                                                                    
 * RETURN VALUE DESCRIPTION:	s2 if they match, else NULL
 */  

static char *
nvmatch(s1, s2)
char *s1, *s2;
{
	while (*s1 == *s2++)
		if (*s1++ == '=')
			return(s2);

	if (*s1 == '\0' && *(s2-1) == '=')
		return(s2);

	return(NULL);
}

/*
 * NAME:	lookup_env
 *                                                                    
 * FUNCTION:	search environment variable 'name' in environment
 *		'*envp' with size 'env_size' read out of context of 'p'.
 *		The content of the environment variable is stored 
 *		in 'value' which is 'value_size' chars long.
 *                                                                    
 * RETURN VALUE DESCRIPTION:	0 	on success
 *				-1	if variable don't exist
 *				other	return value of vm_read
 */  

static int
lookup_env(const struct	proc *p, 
	   const char 	*envp, 
	   const int 	env_size, 
	   const char 	*name,
	   char 	*value,
	   const int	value_size)
{
	char *addr, *v = NULL;
	int i, size;
	int error;
		  
	if ((error = vm_read(p->p_task, 
	     		    (vm_address_t) envp, 
			    (vm_size_t) env_size, 
	     		    (vm_offset_t*) &addr, 
    			    (mach_msg_type_number_t*) &size)) 
			    != KERN_SUCCESS) {
		printf("lookup_env(): vm_read failed(0x%x)\n", error);
		return error;
	}

	if (addr != NULL)
		for (i = 0; i < size;) {
			if ((v = nvmatch(name, &addr[i])) != NULL)
				break;
			/* goto end of this string */
			while (i < size && addr[i] != '\0')
				i++;
			/* goto next string */
			i++;
		}

	if (v != NULL)
		strncpy(value, v, value_size - 1);
	value[value_size - 1] = '\0';

	(void) vm_deallocate(mach_task_self(),
			     (vm_address_t) addr,
			     (vm_size_t) size);

	return v == NULL ? -1 : 0;
}

/*
 * NAME:	get_core_path
 *                                                                    
 * FUNCTION:	read environment variable "CORE_PATH".
 *		The content of "CORE_PATH" is stored in 'core_path'.
 *		If "CORE_PATH" doesn't exist the default is used.
 *                                                                    
 */  

void
get_core_path(const struct proc	*p,
	      char		*core_path,
	      const int		core_path_size)
{
	if (lookup_env(p, u.u_envp, 
		       u.u_env_size, 
		       "CORE_PATH", 
		       (char *) core_path,
		       core_path_size) != 0)
		strcpy(core_path, ".");		/* cwd is default */

	CORE_DEBUGF("get_core_path: CORE_PATH=%s\n", core_path);
}

/*
 * NAME:	get_core_action
 *                                                                    
 * FUNCTION:	read environment variable '*action_type'.
 *		The type of the desired action is stored in 'action'.
 *		If the desired action is not defined '*action' isn't changed.
 *                                                                    
 */  

void
get_core_action(const struct proc	*p,
		const char		*action_type,
		int			*action)
{
	char		core_action_var[80];

	if (lookup_env(p, u.u_envp, 
		       u.u_env_size, 
	               action_type, 
	               core_action_var,
	               sizeof core_action_var) != 0)
	        /* environment variable not define
	           => let default action unchanged */
	        return;	
	else if (strcmp(core_action_var, "FULL") == 0)
		*action = CORE_ACTION_FULL;
	else if (strcmp(core_action_var, "TRACE") == 0)
		*action = CORE_ACTION_TRACE;
	else if (strcmp(core_action_var, "KILL") == 0)
		*action = CORE_ACTION_KILL;
	else if (strcmp(core_action_var, "CONT") == 0)
		*action = CORE_ACTION_CONT;
	else
		uprintf("Invalid core action, \"%s=%s\", ignored.\n", action_type, core_action_var);
	
	CORE_DEBUGF("get_core_action: %s=%s\n", action_type, core_action_var);
}

/*
 * NAME:	is_core_file
 *                                                                    
 * FUNCTION:	decide if a given file is a valid file in a core dir. 
 *                                                                    
 * RETURN VALUE DESCRIPTION:	TRUE,	if a valid file
 *				FALSE,	if not a valid file
 */
 
#define CORE_FILE_NAME		"core."
#define CORE_FILE_NAME_LEN 	5
  
static boolean_t
is_core_file(const char		*file_name)
{
	int			i, file_name_len = strlen(file_name);
	
	/*
	 * Check if it's a allocinfo file.
	 */
	if (strcmp(file_name, "allocinfo") == 0)
		return TRUE;
	
	/* 
	 * Check if file name is long enough.
	 */
	if (file_name_len < CORE_FILE_NAME_LEN + 1)
		return FALSE;
		
	/*
	 * Check if the beginning of the file name matches.
	 */
	if (strncmp(file_name, CORE_FILE_NAME, CORE_FILE_NAME_LEN) != 0)
		return FALSE;
		
	/*
	 * Check if the rest of the file name may be a pid number.
	 */
	for (i = CORE_FILE_NAME_LEN; i < file_name_len; i++)
		if (file_name[i] < '0' || file_name[i] > '9')
			return FALSE;
			
	return TRUE;
}

/*
 * NAME:	cleanup_core_dir
 *                                                                    
 * FUNCTION:	cleanup an existing core directory 
 *		specified by a mach port.
 *		All core files in this directory are removed.
 *                                                                    
 * RETURN VALUE DESCRIPTION:	0 on success, errno otherwise
 */  
static int
cleanup_core_dir(const mach_port_t	core_dir)
{
	char 				*buf;
	int				bufsize = SMALL_ARRAY_LIMIT;
    	struct dirent 			*entry;
    	off_t				offset;
    	int				error, resid, data_read, loc;
    	
    	buf = kalloc(bufsize);
    	if (buf == NULL) {
    		printf("cleanup_core_dir: kalloc of buf failed\n");
    		return -1;
    	}
    	
    	loc = 0;
    	offset = 0;
    	error = 0;
    		
    	for (;;) {
    		/*
    		 * Read some core directory entries.
    		 */
    		if (loc == 0) {
    			CORE_DEBUGF("cleanup_core_dir: bufsize=%d, offset=%d",
    			            bufsize, offset);
    			
    			error = remote_vnreaddir(core_dir,
    						 buf,
    						 bufsize,
    						 &offset,
    						 &resid);
    			data_read = bufsize - resid;

    			CORE_DEBUGF("->%d, resid=%d, data_read=%d\n", 
    			            offset, resid, data_read);
    			
	    		if (error) {
	    			printf("cleanup_core_dir: readdir failed(error=0x%x)\n", 
	    			       error);
	    			break;
	    		}
	    		
	    		if (data_read == 0)
	    			break;
  		}

		/*
		 * Take a core directory entry.
		 */
		entry = (struct dirent *)(buf + loc);
    		
    		if (entry->d_reclen <= 0 ||
    		    loc + entry->d_reclen > data_read) {
    		    	printf("cleanup_core_dir: invalid dir entry\n");
    		    	error = EINVAL;
    		    	break;
    		}
    		
    		CORE_DEBUGF("cleanup_core_dir: in core dir found: (%d)%s\n",
    		            entry->d_ino, entry->d_name);
    		
		/*
		 * Remove an existing core file.
		 */
    		if (entry->d_ino != 0 && 
    		    is_core_file(entry->d_name)) {   
    		        register struct nameidata 	*ndp = &u.u_nd;
			int 				unlink_error;
    		         		
			ndp->ni_dirp = (caddr_t) entry->d_name;
			ndp->ni_forwport = core_dir;
			if (unlink_error = remote_unlink(ndp)) {
				CORE_DEBUGF("cleanup_core_dir: remote_unlink(%s) failed(0x%x)\n", 
				            entry->d_name, unlink_error);
				/*
				 * We are ignoring such an error for now.
				 * It's not due to file permissions because unlink
				 * don't check them. If it's not a regular core file,
				 * e.g. a directory, we will get no confusions with it.
				 * If it's due to core dir permissions we will get another
				 * error on opening a core file.
				 */
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

/*
 * NAME:	setup_core_dir
 *                                                                    
 * FUNCTION:	create a clean core directory called "core" and
 *		and located in '*core_path'.
 *                                                                    
 * RETURN VALUE DESCRIPTION:	mach port of the core directory
 */  

mach_port_t setup_core_dir(char 	*core_path)
{
	register struct nameidata 	*ndp = &u.u_nd;
	int 				error;
	kern_return_t			ret;
	boolean_t			existing;
	path_name_t			core_dir;
	mach_port_t 			core_dir_port;
	enum vtype			type;
	dev_t 				rdev;
	node_t				rnode;
	
	/* 
	 * Check if core directory would be in PFS.
	 */
	
	if (ndp->ni_cdirport == MACH_PORT_NULL)
		/* current directory still root */
		ndp->ni_forwport = ndp->ni_rdirport;
	else
		ndp->ni_forwport = ndp->ni_cdirport;

	error = remote_lookup(ndp, core_path, LOOKUP | FOLLOW, 
			      &type, &rdev, &rnode, 
			      &core_dir_port);
			      
	if (error) {
		printf("setup_core_dir: remote_lookup failed(0x%x)\n",
		       error);
		return MACH_PORT_NULL;
	}

	/* 
	 * Deallocate port got from lookup.
	 */
	if (core_dir_port != MACH_PORT_NULL) {
		ret = mach_port_deallocate(mach_task_self(),
					   core_dir_port);
		if (ret != KERN_SUCCESS) {
			printf("setup_core_dir: Could not deallocate core_dir port (error=0x%x)\n", ret);
		}
	}
	
	/*
	 * Do the check; 
	 * redev is used in order not to change the interface.
	 */
	if (type == VDIR && rdev) {
		printf("setup_core_dir: PFS not allowed for core directory\n",
		       error);
		return MACH_PORT_NULL;
	}
	
	strcpy(core_dir, core_path);
	strcat(core_dir, "/core");
	
again:
	ndp->ni_dirp = (caddr_t) core_dir;
	ndp->ni_forwport = *core_dir == '/' ? ndp->ni_rdirport :
					      ndp->ni_cdirport;
	error = remote_mkdir(ndp, 0777);
	CORE_DEBUGF("setup_core_dir: remote_mkdir(%s) -> 0x%x\n", 
	            core_dir, error);
	            
	existing = (error == EEXIST);
	
	if (error != 0 && ! existing)
		return MACH_PORT_NULL;
	
	if (ndp->ni_cdirport == MACH_PORT_NULL)
		/* current directory still root */
		ndp->ni_forwport = ndp->ni_rdirport;
	else
		ndp->ni_forwport = ndp->ni_cdirport;

	error = remote_lookup(ndp, core_dir, LOOKUP | FOLLOW, 
			      &type, &rdev, &rnode, 
			      &core_dir_port);
			      
	if (error) {
		printf("setup_core_dir: remote_lookup failed(0x%x)\n",
		       error);
		return MACH_PORT_NULL;
	}
			      
	if (existing) {
	    if (type == VDIR) {
	    	/*
	    	 * Cleanup existing core directory.
	    	 */
	    	if (error = cleanup_core_dir(core_dir_port)) {
	    		printf("setup_core_dir: cleanup failed(0x%x)\n", error);
			/* 
			 * We are ignoring such an erro for now. 
			 * If there is a real problem with the core dir 
			 * we will get another error on opening a core file.
			 */
		}
	    }
	    else {
		/* 
		 * Deallocate port got from lookup.
		 */
		if (core_dir_port != MACH_PORT_NULL) {
			ret = mach_port_deallocate(mach_task_self(),
						   core_dir_port);
			if (ret != KERN_SUCCESS) {
				printf("setup_core_dir: Could not deallocate core_dir port (error=0x%x)\n", ret);
			}
		}

		/*
		 * Remove an existing core file.
		 */
		ndp->ni_dirp = (caddr_t) core_dir;
		ndp->ni_forwport = *core_dir == '/' ? ndp->ni_rdirport :
						      ndp->ni_cdirport;
		if (error = remote_unlink(ndp)) {
			printf("setup_core_dir: remote_unlink(%s) failed(0x%x)\n", 
			       core_dir, error);
			return MACH_PORT_NULL;
		}
			            
		goto again;
	    }
	}

	return core_dir_port;
}


/*
 * Create a core image for a process.  It does not have
 * to be the current process.
 *
 * Must be called from a U*X process, so that there is
 * a u-area to make point to the proc structure.
 */

int para_core_dump(const register struct proc *p,
		   const int signo)
{
	struct proc 	*save_proc;
	boolean_t	faulting;
	struct timeval	fault_time;
	path_name_t	core_path;
	path_name_t	core_file;
	mach_port_t	core_dir = MACH_PORT_NULL;
	mach_port_t	root_dir = MACH_PORT_NULL;
	boolean_t	dealloc_core = FALSE, dealloc_root = FALSE;
	int		core_action;
	int		core_type;
	int 		core_status = -1; /* no core */
	int		error;
	kern_return_t	ret;
#ifdef NX
	boolean_t	is_nx_proxy = FALSE;
#endif

	save_proc = u.u_procp;
	u.u_procp = (struct proc*)p;
	
	/*
	 * Check if we were a faulting process.
	 */
	faulting = signo != SIGKILL ||
	           p->p_utask.uu_core_action == CORE_ACTION_CONT;
	CORE_DEBUGF("para_core_dump: faulting=%d\n", faulting);

	if (faulting && signo == SIGKILL)
		/* SIGKILL as faulting signal won't produce core */
		goto out;		/* no core */
		
	raw_microtime(&fault_time);

#ifdef NX
	if (nx_application(p)) {
		struct vproc 	*vp;
		unsigned long 	pvpflag;

		/*
		 * Get the corresponding vproc.
		 */
		vp = p->p_vproc;
		ASSERT(vp);
		VPROC_HOLD(vp, "para_core_dump pid");
		
		/*
		 * Check if we are a NX proxy process.
		 */
		is_nx_proxy = (PVP(vp)->pvp_flag & PV_PGRPLEADER);

		/*
		 * If we are a faulting process call PVPOP_PGRP_CORE
		 * to initiate the necessary action on the other
		 * members of the application.
		 */
		if (faulting) {
			pid_t		pgid;
			pid_t		fault_pid;
			struct vproc 	*gvp;
			boolean_t	dealloc_needed;
			boolean_t	first_faulting;
			
			/*
			 * Get PID of faulting process.
			 */
			fault_pid = p->p_pid;
	
			/*
			 * Get PID of process group leader (pgid ).
			 */
			pgid = p->p_pgid;
			CORE_DEBUGF("para_core_dump: Process group leader for pid %d is %d\n",
				    fault_pid,  pgid);
	
			/*
			 * Get the corresponding vproc.
			 */
			gvp = LOCATE_VPROC_PID(pgid);
			ASSERT(gvp);
			
			CORE_DEBUGF("para_core_dump: call PVPOP_PGRP_CORE\n");
			 
			PVPOP_PGRP_CORE(gvp, fault_pid, p->p_cred,
					&core_action, 
					&core_dir, 
					&root_dir,
					&dealloc_needed,
					&first_faulting);
					
			dealloc_core = dealloc_root = dealloc_needed;

			VPROC_RELEASE(gvp, "para_core_dump pgid");
			
			/*
			 * If core dumping is required and core directory exists,
			 * i.e. core directory is valid, allocinfo file is written
			 * initiated by first faulting process.
			 */
			if (first_faulting && core_dir != MACH_PORT_NULL)
				(void)write_allocinfo(vp,
						      core_dir,
						      root_dir);
		}
		else {
			/*
			 * It's a "core aborted" process.
			 */
			core_dir = p->p_utask.uu_core_directory;
			root_dir = p->p_utask.uu_root_directory;
			core_action = p->p_utask.uu_core_action;
			/* port deallocation is done outside */
		}
	
		CORE_DEBUGF("para_core_dump: core_dir=0x%x, root_dir=0x%x, core_action=%d\n", 
			    core_dir, root_dir, core_action);
		
		VPROC_RELEASE(vp, "para_core_dump pid");
	        
	        /*
	         * set core file to core.<pid>
	         */
	        sprintf(core_file, "core.%d", p->p_pid);
	}
	else
#endif /* NX */
	
	/* 
	 * normal UNIX process: use CORE_PATH and CORE_ACTION_FIRST
	 */
	{
		register struct nameidata 	*ndp = &u.u_nd;
		enum vtype			type;
		dev_t 				rdev;
		node_t				rnode;
		mach_port_t			existing_dir = MACH_PORT_NULL;
		
		core_action=CORE_ACTION_FULL;	/* default */
		get_core_action(p, "CORE_ACTION_FIRST", &core_action);
		if (core_action == CORE_ACTION_CONT) {
			uprintf("Invalid core action, \"CORE_ACTION_FIRST=CONT\", ignored.\n");
			core_action = CORE_ACTION_FULL;
		}
		
		if (core_action == CORE_ACTION_KILL)
			goto out;
		else {
			get_core_path(p, core_path, sizeof core_path);
			
			root_dir = ndp->ni_rdirport;
			
			if (ndp->ni_cdirport == MACH_PORT_NULL)
				/* current directory still root */
				ndp->ni_forwport = ndp->ni_rdirport;
			else
				ndp->ni_forwport = ndp->ni_cdirport;
	
			if (error = remote_lookup(ndp, core_path, LOOKUP | FOLLOW, 
					          &type, &rdev, &rnode, &core_dir)) {
				printf("para_core_dump: remote_lookup failed(0x%x)\n",
				       error);
				goto out;
			}
	
			dealloc_core = TRUE;	/* new port got from lookup */
					      
			/*
			 * Check if core file would be in PFS; 
			 * redev is used in order not to change the interface.
			 */
			if (type == VDIR && rdev) {
				printf("para_core_dump: PFS not allowed for core file\n",
				       error);
				goto out;
			}
			
			strcpy(core_file, "core");
	
			/* remove existing core directory */
			ndp->ni_forwport = core_dir;
			error = remote_lookup(ndp, core_file, LOOKUP | FOLLOW, 
					      &type, &rdev, &rnode, &existing_dir);
					      
			if (error && error != ENOENT) {
				printf("para_core_dump: remote_lookup failed(0x%x)\n",
				       error);
			}
			
			if (!error && type == VDIR) {
				if (error = cleanup_core_dir(existing_dir)) {
		    			printf("para_core_dump: cleanup failed(0x%x)\n", error);
					/* 
					 * We are ignoring such an erro for now. 
					 * If there is a real problem with removing 
					 * an existing core directory we will get 
					 * another error on opening the core file.
					 */
				}
				
				ndp->ni_forwport = core_dir;
				ndp->ni_dirp = (caddr_t) core_file;
				if (error = remote_rmdir(ndp)) {
		    			printf("para_core_dump: rmdir failed(0x%x)\n", error);
					/* 
					 * We are ignoring such an erro for now. 
					 * If there is a real problem with removing 
					 * an existing core directory we will get 
					 * another error on opening the core file.
					 */
				}
			}
	
			if (existing_dir != MACH_PORT_NULL) {
				CORE_DEBUGF("para_core_dump: Deallocate existing_dir port\n");
				ret = mach_port_deallocate(mach_task_self(),
							   existing_dir);
				if (ret != KERN_SUCCESS) {
					printf("para_core_dump: Could not deallocate existing_dir port (error=0x%x)\n", ret);
				}
			}
		}
	}
		
	switch (core_action) {
	case CORE_ACTION_FULL:
		core_type = CORE_TYPE_FULL;
		break;
	case CORE_ACTION_TRACE:
		core_type = CORE_TYPE_TRACE;
		break;
	case CORE_ACTION_KILL:
	case CORE_ACTION_CONT:
		core_type = 0;
		break;
	default:
		printf("para_core_dump: unknown core action(%d)\n",
		       core_action);
		core_type = 0;
		break;
	}
	
	if (core_type) {
		
		if (core_dir == MACH_PORT_NULL) {
			CORE_DEBUGF("para_core_dump: invalid core path\n");
			goto out;
		}
	        
		core_status = para_core(root_dir,
					core_dir,
					core_file, 
					core_type, 
					signo,
					&fault_time);
	}

#ifdef NX
	/*
	 * If we are a NX proxy process and a core directory
	 * was created (!= NULL) set core_status to 0
	 * in order to indicate "core dumped" to the parent.
	 */
	if (is_nx_proxy && core_dir != MACH_PORT_NULL)
		core_status = 0;
#endif /* NX */

out:
	CORE_DEBUGF("para_core_dump: core_action=%d, core_type=0x%x, core_status=%d\n", 
		    core_action, core_type, core_status);
	
	if (dealloc_core && core_dir != MACH_PORT_NULL) {
		/*
		 * Deallocate the core_dir port.
		 */
		CORE_DEBUGF("para_core_dump: Deallocate core_dir port\n");
		ret = mach_port_deallocate(mach_task_self(),
					   core_dir);
		if (ret != KERN_SUCCESS) {
			printf("para_core_dump: Could not deallocate core_dir port (error=0x%x)\n", ret);
		}
	}

	if (dealloc_root && root_dir != MACH_PORT_NULL) {
		/*
		 * Deallocate the root_dir port got from proxy.
		 */
		CORE_DEBUGF("para_core_dump: Deallocate root_dir port\n");
		ret = mach_port_deallocate(mach_task_self(),
					   root_dir);
		if (ret != KERN_SUCCESS) {
			printf("para_core_dump: Could not deallocate root_dir port (error=0x%x)\n", ret);
		}
	}

	u.u_procp = save_proc;
	return core_status;
}

