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
 
/*++ nqsmkdirs.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/setup/nqsmkdirs.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Construct all of the subdirectories used by NQS to store
 *	request control, data, output, and transaction state files.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	March 6, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.2 $ $Date: 1994/11/19 02:53:44 $ $State: Exp $)
 * $Log: nqsmkdirs.c,v $
 * Revision 1.2  1994/11/19  02:53:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/10/17  00:05:38  rkl
 * NQS setup help strings.
 *
 * Revision 3.2  91/02/11  16:59:20  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:12:18  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqsmkdirs.c	1.2 (nqsmkdirs.c OSF/1 NQS2.0 GJK) 6/3/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include "nqs.h"			/* NQS types and definitions */
#include "nqsdirs.h"			/* NQS directory and file hierarchy */

extern int errno;			/* System call error number */
extern struct group *getgrnam();	/* Get group structure by name */
extern struct passwd *getpwnam();	/* Get passwd structure by name */
#if 	UNICOS & NEWPWD
extern void getpwsysent();		/* Force use of new password file */
#endif
extern uid_t getuid();			/* Get real user-id */
extern void pack6name();		/* Compute name on 6-bit alphabet */
extern void sync();			/* Schedule system buffers for */
					/* writing */
extern char *sys_errlist[];		/* Error list explanation */
extern	char	*strcat();
extern	char	*strcpy();

/*** main
 *
 *
 *	int main():
 */
main (argc,argv)
int	argc;
char	*argv[];
{
	struct group *group;		/* Group entry for NQS_GROUP */
	struct passwd *passwd;		/* Password entry for NQS_OWNER */
	void makedirectory();		/* Make directory procedure */
	char base_dir[256];

	if (getuid() != 0) {
		/*
		 *  You must be running as root to run this utility.
		 */
		printf ("Utility [nqsmkdirs]:  User is not root.\n");
		printf ("Utility [nqsmkdirs]:  Aborting.\n");
		fflush (stdout);
		fflush (stderr);
		exit (1);
	}
	/*
	 *  We are definitely running as root.
	 *  Verify that the NQS_GROUP as defined in the NQS Makefile
	 *  exists, and set our real group-id to be NQS_GROUP.
	 */
#ifndef	NQS_GROUP
The symbol NQS_GROUP must be exported from the Makefile invocation.
#else
	group = getgrnam (NQS_GROUP);
#endif
	if (group == (struct group *) 0) {
		/*
		 *  The NQS_GROUP as configured is not known on the
		 *  local system!
		 */
		printf ("Utility [nqsmkdirs]:  NQS_GROUP def invalid.\n");
		printf ("Utility [nqsmkdirs]:  Aborting.\n");
		fflush (stdout);
		fflush (stderr);
		exit (2);
	}
#if UNICOS & NEWPWD
	if (acctid (0,passwd->pw_acid[0]) == -1) {
		/*
		 * 	The account was invalid!
		 */
		printf ("Utility [nqsmkdirs]:  Invalid account ID\n");
		fflush (stdout);
		fflush (stderr);
		exit (3);	
	}
#endif
	if (setgid (group->gr_gid) == -1) {
		/*
		 *  The setgid() call failed!
		 */
		printf ("Utility [nqsmkdirs]:  Unable to setgid (%1d).\n",
			group->gr_gid);
		printf ("Utility [nqsmkdirs]:  Reason: %s.\n",
			sys_errlist [errno]);
		printf ("Utility [nqsmkdirs]:  Aborting.\n");
		fflush (stdout);
		fflush (stderr);
		exit (3);
	}
	/*
	 *  Verify that the NQS_OWNER as defined in the NQS Makefile
	 *  exists.
	 */
#ifndef	NQS_OWNER
The symbol NQS_OWNER must be exported from the Makefile invocation.
#else
#if 	UNICOS & NEWPWD
	getpwsysent();			/* Force use of new password file */
#endif
	passwd = getpwnam (NQS_OWNER);
#endif
	if (passwd == (struct passwd *) 0) {
		/*
		 *  The NQS_OWNER as configured is not known on the
		 *  local system!
		 */
		printf ("Utility [nqsmkdirs]:  NQS_OWNER def invalid.\n");
		printf ("Utility [nqsmkdirs]:  Aborting.\n");
		fflush (stdout);
		fflush (stderr);
		exit (4);
	}
	/*
	 *  Change our directory to the NQS root directory to begin
	 *  our work.  Construct the root directory name by prefixing
	 *  the Nqs_root definition with the first argument specified
	 *  on the command line.
	 */

	base_dir[0] = '\0';
	if (argc > 1) {
		strcpy (base_dir,argv[1]);	/* Prefix directory */
	}
	strcat (base_dir,Nqs_root);

	if (chdir (base_dir) == -1) {
		/*
		 *  Unable to change directory to the NQS root directory.
		 */
		printf ("Utility [nqsmkdirs]:  Unable to chdir to %s (base_dir).\n",base_dir);
		printf ("Utility [nqsmkdirs]:  Reason: %s.\n",
			sys_errlist [errno]);
		printf ("Utility [nqsmkdirs]:  Aborting.\n");
		fflush (stdout);
		fflush (stderr);
		exit (5);
	}
	/*
	 *  The subdirectories that we are going to create, must be created
	 *  with a mode of 777.  Therefore, we clear the umask....
	 */
	umask (0);
	/*
	 *  Build the necessary subdirectories.
	 */
	makedirectory (MAX_CTRLSUBDIRS, Nqs_control,
		      (uid_t) passwd->pw_uid, (gid_t) group->gr_gid);
	makedirectory (MAX_DATASUBDIRS, Nqs_data,
		      (uid_t) passwd->pw_uid, (gid_t) group->gr_gid);
	makedirectory (MAX_OUTPSUBDIRS, Nqs_output,
		      (uid_t) passwd->pw_uid, (gid_t) group->gr_gid);
	makedirectory (MAX_TDSCSUBDIRS, Nqs_transact,
		      (uid_t) passwd->pw_uid, (gid_t) group->gr_gid);
	printf ("Utility [nqsmkdirs]:  NQS database subdirectory ");
	printf ("construction is complete.\n");
	printf ("Utility [nqsmkdirs]:  Exiting.\n");
	fflush (stdout);
	fflush (stderr);
	sync();				/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks refering to the */
					/* newly created files will be */
					/* securely written to disk */
	exit (0);
}


/*** makedirectory()
 *
 *
 *	void makedirectory():
 *
 *	Construct the subdirectories for the specified NQS database parent
 *	directory.
 */
static void makedirectory (no_of_subdirs, parent_name, owner_uid, owner_gid)
int no_of_subdirs;			/* #of subdirectories to create */
char *parent_name;			/* Name of parent directory */
uid_t owner_uid;			/* Owner user-id */
gid_t owner_gid;			/* Owner group-id */
{
	char *argv [3];			/* Argv() to mkdir() */
	char *envp [1];			/* Envp() to mkdir() */
	char path [MAX_PATHNAME+1];	/* NQS pathname */
	register int i;			/* Iteration counter */
	int waitstatus;			/* Child wait status */
	int childpid;			/* Child process-id */

	/*
	 *  Loop to create the control-file directory subdirectories.
	 */
	for (i = 0; i < no_of_subdirs; i++) {
		childpid = fork();
		if (childpid == -1) {
			/*
			 *  The fork was not successful!
			 */
			printf ("Utility [nqsmkdirs]:  Unable to fork() ");
			printf ("mkdir process.\n");
			printf ("Utility [nqsmkdirs]:  Reason: %s.\n",
				sys_errlist [errno]);
			printf ("Utility [nqsmkdirs]:  Aborting.\n");
			fflush (stdout);
			fflush (stderr);
			sync();		/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks refering to the */
					/* newly created files will be */
					/* securely written to disk */
			exit (6);
		}
		if (childpid == 0) {
			/*
			 *  We are the child mkdir process.
			 */
			pack6name (path, parent_name, (int) i, (char *) 0,
				   0L, 0, 0L, 0, 0, 0);
#if OSF
                        argv [0] = "mkdir";
#else
			argv [0] = "-mkdir";
#endif
			argv [1] = path;
			argv [2] = (char *) 0;
			envp [0] = (char *) 0;
			/*
			 *  Create the named directory.
			 */
			printf ("Utility [nqsmkdirs]:  Creating: %s.\n",
				path);
			execve ("/bin/mkdir", argv, envp);
			/*
			 *  The execve() failed!
			 */
			printf ("Utility [nqsmkdirs]:  Unable to execve() ");
			printf ("mkdir program.\n");
			printf ("Utility [nqsmkdirs]:  Reason: %s.\n",
				sys_errlist [errno]);
			printf ("Utility [nqsmkdirs]:  Exiting.\n");
			fflush (stdout);
			fflush (stderr);
			kill (getpid(), 9);	/* Kill ourselves so */
						/* that the parent aborts */
						/* as well */
		}
		/*
		 *  As the parent, we wait for the mkdir process
		 *  to complete.
		 */
		while (wait (&waitstatus) == -1 && errno == EINTR)
			;
		/*
		 *  The child has exited.
		 */
		if ((waitstatus & 0xFF) == 0) {
			/*
			 *  The mkdir process exited normally.
			 */
			printf ("Utility [nqsmkdirs]:  mkdir exited (%1d).\n",
			       (waitstatus >> 8) & 0xFF);
		}
		else if ((waitstatus & 0xFF00) == 0) {
			/*
			 *  The mkdir process was terminated by a signal!
			 */
			printf ("Utility [nqsmkdirs]:  mkdir terminated ");
			printf ("by signal (%1d).\n", waitstatus & 0x7F);
			printf ("Utility [nqsmkdirs]:  Aborting.\n");
			fflush (stdout);
			fflush (stderr);
			sync();		/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks refering to the */
					/* newly created files will be */
					/* securely written to disk */
			exit (7);
		}
		else {
			/*
			 *  The mkdir program stopped!
			 */
			printf ("Utility [nqsmkdirs]:  mkdir stopped by ");
			printf ("signal (%1d).\n", (waitstatus >> 8) & 0xFF);
			printf ("Utility [nqsmkdirs]:  Aborting.\n");
			fflush (stdout);
			fflush (stderr);
			sync();		/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks refering to the */
					/* newly created files will be */
					/* securely written to disk */
			exit (8);
		}
		/*
		 *  Change the mode of any previously existing subdirectories
		 *  to 777.
		 */
		chmod (path, 0777);
		/*
		 *  Change the owner and group of any previously existing
		 *  subdirectories to NQS_OWNER and NQS_GROUP.
		 */
		chown (path, (int) owner_uid, (int) owner_gid);
	}
}
