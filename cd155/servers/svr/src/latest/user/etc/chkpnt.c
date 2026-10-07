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
 *	Copyright (c) Locus Computing, 1991-92
 * 	This is UNPUBLISHED source code that is
 * 	the property of Locus Computing, containing
 *	proprietary secrets of LCC.  Any disclosure
 *	is strictly prohibited.  Locus makes no warantee,
 *	explicit or implicit, on the functionality of this code.
 */
/*
 * HISTORY
 * $Log: chkpnt.c,v $
 * Revision 1.2  1994/11/18  20:52:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:47:57  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:09:37  slk
 * *** empty log message ***
 * 
 * Revision 2.1.1.3  93/07/28  15:11:12  chrisp
 * 	Add linefeed after timeout message.
 * 
 * Revision 2.1.1.2  93/07/20  09:30:16  chrisp
 * 	Write all messages to standard error.
 * 	Correct text of various messages.
 * 	Make timeout period dependent on the number of processes signalled.
 * 
 * Revision 2.1.1.1  93/06/10  11:54:31  chrisp
 * 	Revision 3.8  93/06/04  11:31:04  chrisp
 * 	Kill after chkpnt option (-k) added.
 * 	Routine recursive_unlink() moved into libtnc.
 * 
 * 	Revision 3.7  93/05/19  10:53:00  chrisp
 * 	Remove chkpnt() from library.
 * 	Add function-level comments; replace file mode octals with symbols.
 * 	Add RESTART_EXECROOT option to restart().
 * 
 * 	Revision 3.6  93/05/11  15:36:28  hao2
 * 	Changed exit() condition to 0 after chkpnted successfully.
 * 
 * 	Revision 3.5  93/04/26  12:34:00  chrisp
 * 	Use libc routine chkpnt() instead of doing it explicitly.
 * 
 * 	Revision 3.4  93/04/22  08:54:56  chrisp
 * 	Uncomment call to chkpnt_getprocinfo() and update to new interface
 * 		returning malloc'ed lists.
 * 
 * 	Revision 3.3  93/04/17  13:23:13  chrisp
 * 	Include chkpnt.h rather than defining chkpnt signal arguments again.
 * 
 * 	Revision 3.2  93/04/08  12:03:29  chrisp
 * 	Amend conversion of relative directory pathname to absolute path so that
 * 		program argument space is not overwritten.
 * 
 * 	Revision 3.1  93/03/29  13:32:25  chrisp
 * 	Let the -d option be a relative pathname by translating it into an
 * 		absolute path before setting the symlink.
 * 
 * 	Revision 3.0  93/03/16  09:50:35  chrisp
 * 	First stab.
 * 
 * 	$EndLog$
 * 
 */
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/access.h>
#include <sys/mode.h>
#include <uxkern/bsd_types.h>
#include <tnc/chkpnt.h>

extern int recursive_unlink(char *dname, char *ename);
int chkpnt(int class, pid_t id, char *path, int	flags); /* forward */

/*
 * This implements the chkpnt and chkpnt_pgrp commands:
 *	chkpnt [options] pid
 * and:
 *	chkpnt_pgrp [options] process_group_id
 * where options may be:
 *	-d	dir:	use specified directory rather than default;
 *	-f	force:	if checkpoint directory is non-empty, delete first.
 */
main(int argc, char *argv[])
{
	int		error = 0;
	int		exit_code = 0;
	extern char	*optarg;
	extern int	optind;
	char		ch;
	char		*my_name;
	char		*last_slash;
	char		*chkpnt_dir = NULL;
	path_name_t	chkpnt_dirpath;
	path_name_t	chkpnt_symlink;
	boolean_t	force = FALSE;
	boolean_t	pgrp = FALSE;
	boolean_t	kill_afterwards = FALSE;
	boolean_t	set_symlink = FALSE;
	pid_t		id;
	struct stat	stat_buf;
	DIR		*dfd;
	struct dirent	*dp;

	/*
	 * Look at the command name by we've been invoked to tell
	 * whether we're checkpointing an individual process or an
	 * entire process group.
	 */
	last_slash = strrchr(argv[0], '/');
	if (last_slash == NULL)
		my_name = argv[0];
	else
		my_name = last_slash + 1;
	pgrp = (strcmp(my_name, "chkpnt_pgrp") == 0);

	/*
	 * Option parsing ...
	 */
	while ((ch = getopt(argc, argv, "d:fk")) != EOF) {
		switch (ch) {
		case 'd':
			chkpnt_dir = optarg;
			break;
		case 'f':
			force++;
			break;
		case 'k':
			kill_afterwards++;
			break;
		case '?':
		default:
			error++;
		}
	}
	
	if (optind != (argc - 1) || argv[optind] == NULL)
		error++;
	if (error) {
		printf("usage: %s [-f] [-k] [-d directory] %s\n",
		       my_name, pgrp ? "process_group_id" : "pid");
		exit(1);
	}

	/*
	 * First of many checks - does the process or pgrp exist?
	 */
	id = atoi(argv[optind]);
	error = kill(pgrp ? -id : id, 0);
	if (error != ESUCCESS) {
		fprintf(stderr, "invalid process%s specified\n",
			pgrp ? " group" : "");
		exit(1);
	}

	/*
	 * Does the main /chkpnt directory exist?
	 */
	error = stat("/chkpnt", &stat_buf);
	if (error != ESUCCESS || (stat_buf.st_mode & S_IFMT) != S_IFDIR) {
		fprintf(stderr, "cannot access directory /chkpnt\n");
		exit(1);
	}

	/*
	 * Compose the pathname of, or through which, the checkpoint
	 * directory is accessed.
	 */
	sprintf(chkpnt_symlink, "/chkpnt/%s.%d",
		pgrp ? "pgrp" : "proc", abs(id));
	sprintf(chkpnt_dirpath, "%s", chkpnt_dir ? chkpnt_dir : "");

	/*
	 * Further details depend on whether a checkpoint directory
	 * was specified in the command line.
	 */
	if (*chkpnt_dirpath != '\0') {
		if (*chkpnt_dirpath != '/') {
			/*
			 * Have a relative pathname which needs conversion
			 * into an absolute path - so prepend our current
			 * working directory.
			 */
			path_name_t	cwd;
			path_name_t	rel_dir;
			(void) getcwd(cwd, sizeof(cwd));
			strcpy(rel_dir, chkpnt_dirpath);
			sprintf(chkpnt_dirpath, "%s/%s", cwd, rel_dir);
		}
		error = stat(chkpnt_dirpath, &stat_buf);
		if (error != ESUCCESS ||
		    (stat_buf.st_mode & S_IFMT) != S_IFDIR) {
			fprintf(stderr, "cannot access directory %s\n",
				chkpnt_dirpath);
			exit(1);
		}
		error = access(chkpnt_symlink, F_OK);
		if (error == ESUCCESS ) {
			/*
			 * /chkpnt/pxxx.nnn exists, delete it
			 */
			error = unlink(chkpnt_symlink);
			if (error) {
				fprintf(stderr, "cannot remove %s\n",
					chkpnt_symlink);
				exit(1);
			}
			
		}
		error = symlink(chkpnt_dirpath, chkpnt_symlink);
		if (error) {
			fprintf(stderr, "unable to create %s symlink\n",
				chkpnt_symlink);
			exit(1);
		}
		set_symlink = TRUE;
	} else {
		/*
		 * The checkpoint directory is the default.
		 */
		error = access(chkpnt_symlink, F_OK);
		if (error == ESUCCESS ) {
			/*
			 * /chkpnt/pxxx.nnn exists, check it's a directory
			 */
			error = stat(chkpnt_symlink, &stat_buf);
			if ((stat_buf.st_mode & S_IFMT) != S_IFDIR) {
				fprintf(stderr, "%s is not a directory\n",
					chkpnt_dirpath);
				exit(1);
			}
		} else {
			error = mkdir(chkpnt_symlink,
				      S_IRWXU | S_IXGRP | S_IXOTH);
			if (error) {
				fprintf(stderr, "cannot create directory %s\n",
					chkpnt_symlink);
				exit(1);
			}
		}
	}
	/*
	 * Here with /chkpnt/pxxx.nnnn as a directory or a symlink to a
	 * directory. Now check whether this is non-empty and attempt to
	 * delete contents if the force option has been given.
	 */
	dfd = opendir(chkpnt_symlink);
	if (dfd == NULL) {
		fprintf(stderr, "cannot open checkpoint directory %s\n",
			chkpnt_symlink);
		exit(1);
	}
	while (((dp = readdir(dfd)) != NULL)) {
		if (strcmp(dp->d_name, ".") == 0 ||
		    strcmp(dp->d_name, "..") == 0)
			continue;
		if (force) {
			/*
			 * Delete the file and its contents if a directory
			 */
			error = recursive_unlink(chkpnt_symlink, dp->d_name);
			if (error) {
				fprintf(stderr,
					"cannot recursively delete directory %s (%d)\n",
					chkpnt_symlink, errno);
				exit(1);
			}
		} else {
			fprintf(stderr, "checkpoint directory non-empty\n");
			exit(1);
		}
	}
	(void) closedir(dfd);


	/*
	 * And about time... do the checkpoint.
	 */
	error = chkpnt((pgrp ? CHKPNT_FAMILY : CHKPNT_PROC) |
		           (kill_afterwards ? CHKPNT_KILL : 0),
		       id,
		       chkpnt_symlink,
		       0);
	if (error != ESUCCESS) {
		if (errno == ETIMEDOUT)
			fprintf(stderr, "timeout occurred\n");
		else
			perror("checkpoint unsuccessful");
		exit(1);
	}

	/*
	 * If we checkpointed to a nominated directory through a sysmlink,
	 * we can now remove the symlink.
	 */
	if (set_symlink) {
		error = unlink(chkpnt_symlink);
		if (error != ESUCCESS) {
			fprintf(stderr, "unable to remove symlink %s\n",
				chkpnt_symlink);
			exit(1);
		}
	}
		
	exit(0);
}

static	int	restarted = FALSE;
void
restarting()
{
	/*
	 * Leave a mark of the restart call.
	 */
	restarted = TRUE;
}

/*
 * The following implements the POSIX draft interface for the chkpnt
 * system kill. Here we map the function onto kill3() etc.
 */
int
chkpnt(
	int	class,
	pid_t	id,
	char	*path,
	int	flags)
{
	int	nproc;
	pid_t	pid_or_pgid = (class & CHKPNT_FAMILY) ? -id : id;
	int	timeout_count = 5;
	int	argument;

	/*
	 * Define an on_restart routine so that we subsequently know
	 * that we're restarting (and not still checkpointing).
	 * This is strictly only necessary if the calling process is
	 * also a target of the checkpoint - but we do it regardless.
	 */
	restarted = FALSE;
	(void) on_restart((int (*)()) restarting);

	/*
	 * Signal process or process group - SIGMIGRATE argument tells
	 * target processes whether the whole group is being signaled.
	 */
	if (class & CHKPNT_FAMILY) {
		if (class & CHKPNT_KILL)
			argument = SIGCHKPNT_KILL_PGRP;
		else
			argument = SIGCHKPNT_PGRP;
	} else {
		if (class & CHKPNT_KILL)
			argument = SIGCHKPNT_KILL_PROC;
		else
			argument = SIGCHKPNT_PROC;
	}
	nproc = kill3(pid_or_pgid, SIGMIGRATE, argument);
	if (nproc <= 0)
		return(-1);

	/*
	 * Now wait for all the expected checkpoint files to be created.
	 * This is a dumb polling loop.
	 */
	timeout_count += 2 * nproc;	/* extra timeout of 2 sec per proc */
	while (!restarted) {
		pid_t	pgid;
		int	mproc;
		int	error;

		sleep(1);
		error = chkpnt_getprocinfo(path,
					   &pgid, &mproc,
					   NULL, NULL, NULL);
		if (error != ESUCCESS && errno != EINVAL)
			return(-1);
		if (mproc == nproc)
			break;
		if (--timeout_count <= 0) {
			errno = ETIMEDOUT;
			return(-1);
		}
	}

	/* If restarted, we're done */
	if (restarted)
		return(0);

	return(0);
}

