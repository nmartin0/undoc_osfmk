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
 * Copyright (c) 1993-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: chkpnt_restart.c,v $
 * Revision 1.3  1995/02/02  00:01:09  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.2  1994/11/18  20:56:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:48:35  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:09:56  slk
 * *** empty log message ***
 * 
 * Revision 2.1.1.5  93/07/20  10:08:27  chrisp
 * 	Introduce RESTART_NOSTOP option to inhibit RESTARTEXEC_STOP
 * 	and the associated stopping/continuing.
 * 
 * Revision 2.1.1.4  93/07/16  12:17:27  hao
 * 	If scandir() fails in chkpnt_getprocinfo(), set errno to indicate the
 * 	error.
 * 
 * Revision 2.1.1.3  93/07/16  10:38:42  chrisp
 * 	Set errno to EINVAL when checkpoint directory contains an odd
 * 	number of entries.
 * 
 * Revision 2.1.1.2  93/07/08  08:08:24  chrisp
 * 	If no processes are found, return ESUCCESS but nproc = 0.
 * 
 * Revision 2.1.1.1  93/06/10  11:55:23  chrisp
 * 	Revision 3.9  93/06/04  11:35:10  chrisp
 * 	Track additions and changes to chkpnt options and to signal argument values.
 * 	Added recursive_unlink() for general usefullness (was in chkpnt.c).
 * 
 * 	Revision 3.8  93/05/25  09:45:24  chrisp
 * 	Introduce chkpnt_async() which maps directly to migrate().
 * 	Eliminate explicit filename sizing assumptions in favor of MAX_NAME.
 * 	Optimize restart() to avoid STOP/CONT synchronization when the
 * 		RESTART_NOSETPGID option is given.
 * 	Improve commentary.
 * 
 * 	Revision 3.7  93/05/19  10:56:14  chrisp
 * 	Improve comments. Remove chkpnt(). Add RESTART_EXECROOT option to restart().
 * 
 * 	Revision 3.6  93/05/17  09:45:23  chrisp
 * 	Add missing return(0);
 * 
 * 	Revision 3.5  93/05/11  15:38:56  hao2
 * 	Check for -g option so that restart does not setpgrp() to chkpnted pgid
 * 	automatically.
 * 
 * 	Revision 3.4  93/04/30  08:45:39  chrisp
 * 	Correct debugging message.
 * 
 * 	Revision 3.3  93/04/29  08:30:42  chrisp
 * 	Add debugging messages to restart() code.
 * 	Add logic in chkpnt() to skip checkpoint file checks in the restart() path.
 * 
 * 	Revision 3.2  93/04/26  12:35:11  chrisp
 * 	Introduce routine chkpnt() to perform checkpoint signaling and waiting for
 * 		completion. Its interface is along POSIX lines.
 * 
 * 	Revision 3.1  93/04/22  09:00:43  chrisp
 * 	Substantial revamp of chkpnt_getprocinfo() to use libc routine scandir()
 * 		for directory analysis plus the general use of malloc'ed space
 * 		instead of using array passed in by the caller.
 * 	Consequential changes to restart() etc.
 * 
 * 	Revision 3.0  93/04/17  13:06:08  chrisp
 * 	This source module contains chkpnt_getprocinfo() and restart().
 * 
 * 	$EndLog$
 * 
 */
#include <stdio.h>
#include <unistd.h>
#undef _KERNEL
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <tnc/chkpnt.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/access.h>
#include <sys/mode.h>

#define	WAIT_ANY		-1

extern int alphasort();

/*
 * This file contains library routines chkpnt_self(), chkpnt_getprocinfo()
 *	and restart().
 */

/*
 * chkpnt_self() is mapped to migrate() - so long as the option maps
 *	acceptably to a negative migrate argument.
 */
int
chkpnt_self(
	int	options)
{
	int	argument;

	/*
	 * Allowed options are kill or async, not both.
	 */
	if ((options & ~(CHKPNT_KILL|CHKPNT_ASYNC)) ||
	    (options == (CHKPNT_KILL|CHKPNT_ASYNC))) {
		errno = EINVAL;
		return(-1);
	}

	if (options & CHKPNT_KILL)
		argument = SIGCHKPNT_KILL_PROC;
	else if (options & CHKPNT_ASYNC)
		argument = SIGCHKPNT_PROC;
	else
		argument = SIGCHKPNT_SYNC_PROC;

	return(migrate(argument));
}

static
int
is_valid_chkpnt_file(
	struct dirent	*dir_entry)
{
	pid_t		pid;
	pid_t		ppid;
	pid_t		pgrp;
	node_t		node;
	char		ext[NAME_MAX+1];
	/*
	 * All chkpnt file names should be in the form of
	 * <pid>.<ppid>.<pgid>.<node>.{stat,core}
	 */
	return(sscanf(dir_entry->d_name, "%d.%d.%d.%d.%s",
		      &pid, &ppid, &pgrp, &node, ext));
}

/*
 * This routine returns information about the contents of a specified
 * checkpoint directory. This can be used to verify the completeness
 * of a checkpoint or to obtain full process id and family relationship
 * information prior to a restart. The restart() library routine itself
 * uses getprocinfo() in this latter capacity.
 */ 
int
chkpnt_getprocinfo(
	char 	*dir_name,		/* pathname to checkpoint dir */
	pid_t	*pgid,			/* returns pid or pgid checkpointed */
	int  	*nproc,			/* returns number of processes */ 
	pid_t	*pid_list[],		/* returns list of process ids */
	pid_t	*ppid_list[],		/* returns list of parent processes */
	node_t	*node_list[])		/* returns list of execution nodes */
{
	struct dirent	**name_list;
	int		nentries;
	int		npairs;
	int		i;
	int		ii;
	pid_t		chkpnt_pgid = 0;
	struct stat     stat_buf;

	/*
	 * Call libc routine scandir to analyze and sort the checkpoint
	 * directory.
	 */
	nentries = scandir(dir_name, &name_list,
			   is_valid_chkpnt_file, alphasort);

	if (nentries < 0) {
		/*
		 * Find out why scandir failed and set the errno.
		 */
		if (stat(dir_name, &stat_buf) == -1)
			return(-1);
		if ((stat_buf.st_mode & S_IFMT) != S_IFDIR)
			errno = ENOTDIR;
		return(-1);
	}
	if (nentries == 0) {
		*nproc = 0;
		return(0);
	} else if (nentries & 0x1) {		/* Must be an even number */
		errno = EINVAL;
		goto out;
	}

	npairs = nentries >> 1;

	/*
	 * Now malloc the return arrays.
	 */
	if (pid_list)
		*pid_list = (pid_t *) malloc(sizeof(pid_t)*npairs); 
	if (ppid_list)
		*ppid_list = (pid_t *) malloc(sizeof(pid_t)*npairs); 
	if (node_list)
		*node_list = (pid_t *) malloc(sizeof(node_t)*npairs); 
	if ((pid_list  && !*pid_list) ||
	    (ppid_list && !*ppid_list) ||
	    (node_list && !*node_list)) {
		errno = ENOMEM;
		goto out;
	}

	/*
	 * Scan through directory entries looking for checkpoint
	 * ".core" and ".stat" files. Since the directory is now sorted,
	 * files will occur in (.core,.stat) pairs for each process.
	 * Any mismatch indicates an inconsistent checkpoint and that's
	 * an error.
	 */
	for (ii = 0, i = 0; ii < nentries; ii += 2, i++) {
		pid_t		pid1, pid2;
		pid_t		ppid1, ppid2;
		pid_t		pgid1, pgid2;
		node_t		node1, node2;
		char		ext1[6], ext2[6];
		
		(void) sscanf(name_list[ii]->d_name, "%d.%d.%d.%d.%s",
			      &pid1, &ppid1, &pgid1, &node1, ext1);
		(void) sscanf(name_list[ii+1]->d_name, "%d.%d.%d.%d.%s",
			      &pid2, &ppid2, &pgid2, &node2, ext2);

		/*
		 * Check that we have a matching pair of files.
		 */
		if ((pid1 != pid2) || (ppid1 != ppid2) ||
		    (pgid1 != pgid2) || (node1 != node2) ||
		    strcmp(ext1, "core") || strcmp(ext2, "stat")) {
			errno = EINVAL;
			goto out;
		}

		/*
		 * Check that the pgid is consistent.
		 */
		if (chkpnt_pgid == 0)
			chkpnt_pgid = pgid1;
		else if (pgid1 != chkpnt_pgid) {
			errno = EINVAL;
			goto out;
		}

		/*
		 * Assign the return values if required.
		 */
		if (pid_list)
			(*pid_list)[i] = pid1; 
		if (ppid_list)
			(*ppid_list)[i] = ppid1; 
		if (node_list)
			(*node_list)[i] = node1; 
	}
	errno = ESUCCESS;

out:
	/*
	 * Free the memory returned by scandir()
	 */
	for (ii = 0; ii < nentries; ii++) {
		free(name_list[ii]);
	}
	free(name_list);

	if (errno == ESUCCESS) {
		*pgid = chkpnt_pgid;
		*nproc = npairs;
		return(ESUCCESS);
	} else {	
		if (pid_list && *pid_list)
			free(*pid_list);
		if (ppid_list && *ppid_list)
			free(*ppid_list);
		if (node_list && *node_list)
			free(*node_list);
		*nproc = 0;
		return(-1);
	}
}

/*
 * This routine is called recursively by the restart() library routine
 * to restart a family hierarchy. If this_pid identifies a parent process
 * from the list of checkpointed pids, then the children will be forked and
 * exec_restart()ed.
 */
static
int
restart_family(
	char	*checkpoint_dirpath,	/* pathname to checkpoint dir */
	pid_t	this_pid,		/* caller's pid */
	int	options,		/* bit mask containing option flags */
	pid_t	pgid,			/* process group id checkpointed */
	int	nproc,			/* number of processes checkpointed */
	pid_t	pid_list[],		/* process ids checkpointed */
	pid_t	ppid_list[],		/* parent process ids checkpointed */
	pid_t	new_ppid_list[],	/* parent process ids to restart */
	node_t	node_list[])		/* execution nodes */
{
	int	nchildren;
	int	i;
	int	ret;
	pid_t	*child_pid_list = (pid_t *) malloc(sizeof(pid_t)*nproc);

	if (child_pid_list == NULL) {
		errno = ENOMEM;
		return(-1);
	}

	/*
	 * Sift through the processes to discover our children.
	 */
	nchildren = 0;
	for (i = 0; i < nproc; i++) {
		if (new_ppid_list[i] == this_pid)
			child_pid_list[nchildren++] = pid_list[i];
	}
	if (nchildren == 0)
		return(0);
		
	/*
	 * Now the restart begins in anger - fork ourselves to produce
	 * children with the required pids.
	 */ 
	RESTART_DBG(options, \
		    ("[%d] restart_family() before forkfamily(%d,..)\n", \
		    this_pid, nchildren));
	ret = forkfamily(nchildren, child_pid_list);
	if (ret == 0) {
		/*
		 * We're a restarting child.
		 * Get memory for checkpoint filename manipulations,
		 *	giving up the ghost if we can't.
		 */
		char *chkpnt_prefix = (char *)malloc(strlen(checkpoint_dirpath)
							+ NAME_MAX + 1);
		if (chkpnt_prefix == NULL)
			exit(ENOMEM);

		/*
		 * If we're a process group leader we must setpgid()
		 * before stopping.
		 * Once continued, as lowly process group members,
		 * we can set process group to the correct pgid,
		 * safe in the knowledge that the leader exists. 
		 */
		this_pid = getpid();
		if (!(options & RESTART_NOSETPGID) && this_pid == pgid) {
			ret = setpgid(0,0);
			RESTART_DBG(options, \
				    ("[%d] setpgid(0,0) returns %d\n", \
				    this_pid, ret));
			if (ret != ESUCCESS)
				exit(ret);
		}
		/*
		 * We may ourselves be a parent, make a recursive call.
		 */
		ret = restart_family(checkpoint_dirpath,
				     this_pid,
				     options,
				     pgid,
				     nproc,
				     pid_list,
				     ppid_list,
				     new_ppid_list,
				     node_list);
		if (ret != 0)
			return(ret);

		/*
		 * If we want to re-establish process groups
		 * we stop to gain synchronization and thus assure
		 * that the intended pgrp exists.
		 */
		if (!(options & RESTART_NOSETPGID)) {
			RESTART_DBG(options, \
				    ("[%d] before stopping\n", this_pid));
			kill(this_pid, SIGSTOP);
			if (this_pid != pgid) {
				/*
				 * Change to checkpointed process group. Ignore
				 * any error - we'll default to the restarter's
				 * group.
				 */
				ret = setpgid(0,pgid);
				RESTART_DBG(options, \
					    ("[%d] setpgid(0,%d) returns %d\n",\
					    this_pid, pgid, ret));
			}
		}

		/*
		 * Must discern which checkpoint files we are to restart
		 * from. This involves looking up our pid in the info
		 * arrays originally returned by chkpnt_getprocinfo().
		 */
		for (i = 0; i < nproc; i++)
			if (pid_list[i] == this_pid)
				break;
		sprintf(chkpnt_prefix, "%s/%d.%d.%d.%d",
			checkpoint_dirpath,
			pid_list[i], ppid_list[i], pgid, node_list[i]);

		/*
		 * Before exec_restart()ing, migrate to the right node.
		 */
		if (!(options & RESTART_NOMIGRATE) &&
		    node_list[i] != node_self())
			migrate(node_list[i]);

		RESTART_DBG(options, \
			    ("[%d] before exec_restart(%s,..)\n", \
			    this_pid, chkpnt_prefix));
		ret = exec_restart(chkpnt_prefix,
				   (options & RESTART_NOSTOP) ?
					0 : RESTARTEXEC_STOP);
		if (ret != ESUCCESS)
			exit(ret);

	} else {
		int	n_stopped = 0;

		/*
		 * Here if parent.
		 * If we're re-establishing process group, we wait for all
		 * processes to stop. This guarantees that the process group
		 * leader (and hence pgrp) exists.
		 */
		if (!(options & RESTART_NOSETPGID)) {
			/*
			 * Wait for all children to stop.
			 * Maybe there should be a timeout here.
			 */
			RESTART_DBG(options, \
				    ("[%d] waiting for %d children\n", \
				    this_pid, nchildren));
			for (i=0; i < nchildren; i++) {
				int	status;
				waitpid(WAIT_ANY, &status, WUNTRACED);
				if (WIFSTOPPED(status))
					n_stopped++;
			}
			/*
			 * If any child exited rather than stopped, kill the family.
			 */
			if (n_stopped != nchildren) {
				for (i=0; i < nchildren; i++)
					kill(child_pid_list[i], SIGKILL);
				errno=ECHILD;
				return(-1);
			}
			/*
			 * Now continue all children - allowing them to
			 * exec_restart().
			 */
			RESTART_DBG(options, \
				    ("[%d] continuing children\n", this_pid));
			for (i=0; i < nchildren; i++)
				kill(child_pid_list[i], SIGCONT);
		}

		/*
		 * Wait for all chilren to stop immediately prior to
		 * to returning from their exec_restart()s.
		 */
		if (!(options & RESTART_NOSTOP)) {
			n_stopped = 0;
			for (i=0; i < nchildren; i++) {
				int	status;
				waitpid(WAIT_ANY, &status, WUNTRACED);
				if (WIFSTOPPED(status))
					n_stopped++;
			}
			RESTART_DBG(options, \
				    ("[%d] children exec_restarted\n", this_pid));
			if (n_stopped != nchildren) {
				for (i=0; i < nchildren; i++)
					kill(child_pid_list[i], SIGKILL);
				errno=ECHILD;
				return(-1);
			}
		}
	}
	free(child_pid_list);
	return(0);
}

/*
 * This is the library routine implementing restart(). Given a pathname
 * to a checkpoint directory, it uses chkpnt_getprocinfo() to obtain
 * details of the checkpointed processes before setting about their
 * restarts. Routine restart_family() is called to do most of the work
 * of recursively forking the family tree.
 * Valid options are:
 *	RESTART_SIGALL		continue all restarted processes;
 *	RESTART_NOSTOP		do no stop restarted processes;
 *	RESTART_EXECROOT	if an ancestor or "root" process exists
 *				in the checkpoint set, exec_restart() the
 *				calling process rather than re-creating a
 *				separate process;
 *	RESTART_NOSETPGID	don't restore the checkpointed process group
 *				but let the restarted processes inherit the
 *				process group of the restarter;
 *	RESTART_DEBUG		if compiled with debugging, issue messages
 *				at key stages during the restart procedure.
 */
int
restart(
	char	*checkpoint_dirpath,	/* path to checkpoint dir */
	pid_t	*pid,			/* return id of process or group */
	int	options)		/* sundry variants to our behavior */
{
	int	ret;
	int	nproc;
	int	i, j;
	pid_t	pgid;
	pid_t	*pid_list = NULL;
	pid_t	*ppid_list = NULL;
	node_t	*node_list = NULL;
	pid_t	*new_ppid_list = NULL;
	pid_t	this_pid = getpid();
	pid_t	root_pid_idx = -1;
	boolean_t exec_root = FALSE;

	/*
	 * Firstly, verify the directory is complete and get all the
	 * proces dope.
	 */
	ret = chkpnt_getprocinfo(checkpoint_dirpath,
				 &pgid,
				 &nproc,
				 &pid_list,
				 &ppid_list,
				 &node_list);
	if (ret != ESUCCESS)
		goto out;
	RESTART_DBG(options, \
		    ("[%d] restart() gets nproc=%d from getprocinfo\n", \
		    this_pid, nproc));
	if (nproc == 0) {
		/*
		 * No checkpointed processes to be found.
		 */ 
		errno = ESUCCESS;
		goto out;
	}


	/*
	 * Filter the parent pid array generating a new array with pids
	 * outside the family replaced by the restarter's own pid.
	 * This is performed simplistically using a linear search for parent id
	 * amongst the full list and optimizations exist. However, since
	 * getprocinfo() returns an ordered pid list and in many cases
	 * the process group leader will be the lowest pid and the parent
	 * of all other processes, it wont turn out that bad.
	 */
	new_ppid_list = (pid_t *) malloc(sizeof(pid_t)*nproc);
	if (new_ppid_list == NULL) {
		errno = ENOMEM;
		goto out;
	}
	for (i = 0; i < nproc; i++) {
		int	num_orphans = 0;
		for (j=0; j < nproc; j++) {
			if (pid_list[j] == ppid_list[i])
				break;
		}
		if (j == nproc) {
			num_orphans++;
			if (num_orphans == 1)
				root_pid_idx = i;
			new_ppid_list[i] = this_pid;
		} else
			new_ppid_list[i] = ppid_list[i];
	}

	/*
	 * Now restart our immediate children. Note that this
	 * function returns only in the parent.
	 * If the RESTART_ROOTEXEC option is requested and indeed on
	 * root process exists for the family, then start with it.
	 */ 
	exec_root = (options & RESTART_EXECROOT) && (root_pid_idx >= 0);
	ret = restart_family(checkpoint_dirpath,
			     exec_root ? pid_list[root_pid_idx] : this_pid,
			     options,
			     pgid,
			     nproc,
			     pid_list,
			     ppid_list,
			     new_ppid_list,
			     node_list);
	if (ret != ESUCCESS)
		goto out;

	/*
	 * If the user doesn't want to continue the family explicitly
	 * we do it now.
	 */
	if (options & RESTART_SIGALL && !(options & RESTART_NOSTOP)) {
		RESTART_DBG(options, \
			    ("[%d] before continuing processes\n", this_pid)); \
		for (i=0; i < nproc; i++) {
			if (exec_root && (i == root_pid_idx))
				continue;
			(void) kill(pid_list[i], SIGCONT);
		}
	}
	if (exec_root) {
		char *chkpnt_prefix = (char *)malloc(strlen(checkpoint_dirpath)
							+ NAME_MAX + 1);
		if (chkpnt_prefix == NULL) {
			errno = ENOMEM;
			goto out;
		}
		sprintf(chkpnt_prefix, "%s/%d.%d.%d.%d",
			checkpoint_dirpath,
			pid_list[root_pid_idx],
			ppid_list[root_pid_idx],
			pgid,
			node_list[root_pid_idx]);

		/*
		 * Before exec_restart()ing, migrate to the right node.
		 */
		if (node_list[root_pid_idx] != node_self())
			migrate(node_list[root_pid_idx]);

		RESTART_DBG(options, \
			    ("[%d] root process before exec_restart(%s,..)\n", \
			    this_pid, chkpnt_prefix));
		ret = exec_restart(chkpnt_prefix,
				   (options & RESTART_SIGALL &&
				    !(options & RESTART_NOSTOP)) ?
					0 : RESTARTEXEC_STOP);
		if (ret != ESUCCESS)
			exit(ret);
	}
		
out:
	/*
	 * Return pid if single process, -pgid if group.
	 */
	if (errno == ESUCCESS) {
		if (nproc > 0)
			*pid = (nproc == 1) ?
					pid_list[0] :
					(options & RESTART_NOSETPGID) ?
						-getpgrp() :
						-pgid;
	} else {
		/*
		 * In the of error, attempt to kill any children that
		 * just might have been created. Note that the original
		 * cause of failure is saved and restored over the kill()s.
		 */
		int error = errno;
		if (nproc > 0)
			for (i=0; i < nproc; i++)
				(void) kill(pid_list[i], SIGKILL);
		errno = error;
		nproc = -1;
	}

	if (pid_list)
		free(pid_list);
	if (ppid_list)
		free(ppid_list);
	if (new_ppid_list)
		free(new_ppid_list);
	if (node_list)
		free(node_list);

	RESTART_DBG(options, ("[%d] restart() returning %d\n",this_pid,nproc));
	return(nproc);
}

/*
 * Removes a file, performing a recursive descent if the file
 * is a directory.
 */
int
recursive_unlink(
	char	*dname,
	char	*ename)
{
	int		error = ESUCCESS;
	struct stat	stat_buf;
	char		*fname = (char *) malloc(strlen(dname)+strlen(ename)+2);

	if (fname == NULL) {
		errno = ENOMEM;
		return(-1);
	}
	sprintf(fname, "%s/%s", dname, ename);

	/*
	 * Find out what entry we have.
	 */
	error = stat(fname, &stat_buf);
	if (error != ESUCCESS)
		goto out;

	/*
	 * Make sure we have a chance of deleting the thing.
	 */
	(void) chmod(fname, S_IRWXU | S_IRWXG | S_IRWXO);

	/*
	 * Is it a plain file?
	 */
	if ((stat_buf.st_mode & S_IFMT) != S_IFDIR) {
		error = unlink(fname);
		goto out;
	} else {
		/*
		 * It's a directory.
		 */
		struct dirent	*dp;
		DIR		*dfd;

		dfd = opendir(fname);
		if (dfd == NULL) {
			errno = ENOENT;
			error = -1;
			goto out;
		}

		/*
		 * Deal with directory entries
		 */
		while (((dp = readdir(dfd)) != NULL)) {
			if (strcmp(dp->d_name, ".") == 0 ||
			    strcmp(dp->d_name, "..") == 0)
				continue;
			error = recursive_unlink(fname, dp->d_name);
			if (error) {
				(void) closedir(dfd);
				goto out;
			}
		}

		(void) closedir(dfd);

		error = rmdir(fname);

	}

out:
	free(fname);
	return(error);
}
