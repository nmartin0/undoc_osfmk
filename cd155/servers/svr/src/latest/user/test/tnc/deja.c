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
 * $Log: deja.c,v $
 * Revision 1.2  1994/11/18  21:04:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/03/14  17:50:39  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Chris Peak, chrisp@locus.com
 *  Risk: Low
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, individual checkpoint restart by hand
 *  Module(s):
 *
 * Revision 2.2  93/11/10  12:19:16  slk
 * Check into main tree for checkpoint restart merge.
 * 
 * Revision 2.1.1.2  93/07/07  13:12:52  chrisp
 * 	Take note of return value from chkpnt_self() - >0 => restarted.
 * 
 * Revision 2.1.1.1  93/06/10  11:55:44  chrisp
 * 	$EndLog$
 * 
 */
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#undef _KERNEL
#include <tnc/chkpnt.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/access.h>
#include <sys/mode.h>
#include <sys/fcntl.h>
#include <uxkern/bsd_types.h> 


extern int recursive_unlink(
	char	*dname,
	char	*ename);

int	global_int = 0;

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
	path_name_t	chkpnt_prefix;
	boolean_t	force = FALSE;
	boolean_t	pgrp = FALSE;
	boolean_t	set_symlink = FALSE;
	boolean_t	stop = FALSE;
	pid_t		id;
	struct stat	stat_buf;
	DIR		*dfd;
	struct dirent	*dp;
	int		nproc;
	int		tty_des;

	int		stack_int;

	last_slash = strrchr(argv[0], '/');
	if (last_slash == NULL)
		my_name = argv[0];
	else
		my_name = last_slash + 1;

	while ((ch = getopt(argc, argv, "d:fs")) != EOF) {
		switch (ch) {
		case 'd':
			chkpnt_dir = optarg;
			break;
		case 'f':
			force++;
			break;
		case 's':
			stop++;
			break;
		case '?':
		default:
			error++;
		}
	}
	
	if (error) {
		printf("usage: %s [-f] [-d directory]\n", my_name);
		exit(1);
	}

	id = getpid();
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
		"proc", abs(id));
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
			error = mkdir(chkpnt_symlink, 0000711);
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
				fprintf(stderr, "cannot recursively delete directory %s (%d)\n",
					chkpnt_symlink, errno);
				exit(1);
			}
		} else {
			fprintf(stderr, "checkpoint directory non-empty\n");
			exit(1);
		}
	}
	(void) closedir(dfd);

	global_int = -1; stack_int = -1;
	printf("Before checkpoint: global int: %d, stack int: %d\n",
		global_int, stack_int);

	/*
	 * And about time... do the checkpoint.
	 */
	error = chkpnt_self(CHKPNT_ASYNC);
	if (error < 0) {
		perror("checkpoint failed");
		exit(1);
	}

	global_int = 0; stack_int = 0;
	printf("After %s: global int: %d, stack int: %d\n",
		error == 0 ? "checkpoint" : "restart", global_int, stack_int);

	sleep(2);

	if (set_symlink) {
		error = unlink(chkpnt_symlink);
		if (error != ESUCCESS) {
			fprintf(stderr, "unable to remove symlink %s\n",
				chkpnt_symlink);
			exit(1);
		}
	}

	printf("Waiting 5 seconds ...\n");
	for (;global_int < 5; global_int++, stack_int++) {
		printf("Counting: global int: %d, stack int: %d\n",
			global_int, stack_int);
		sleep(1);
	}
	printf("exec()ing vu...\n");

	sprintf(chkpnt_prefix, "%s/%d.%d.%d.%d", 
		set_symlink ? chkpnt_dirpath : chkpnt_symlink,
		id, getppid(), getpgrp(), node_self());
	error = execl("vu", "vu", chkpnt_prefix, stop ? "1" : "0");

	printf("Whoops, execl() error=%d, errno=%d\n", error, errno);

	exit(exit_code);
}

