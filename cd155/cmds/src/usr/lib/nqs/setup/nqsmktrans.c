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
 
/*++ nqsmktrans.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/setup/nqsmktrans.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Construct all of the necessary transaction descriptors as
 *	determined by the MAX_TRANSACTS parameter in nqs.h.
 *
 *
 *	WARNING:  If the bitwise definition of a transaction
 *		  descriptor is ever changed (see ../lib/transact.c),
 *		  then this program must be changed appropriately!
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	March 20, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.2 $ $Date: 1994/11/19 02:53:45 $ $State: Exp $)
 * $Log: nqsmktrans.c,v $
 * Revision 1.2  1994/11/19  02:53:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/10/17  00:05:41  rkl
 * NQS setup help strings.
 *
# Revision 3.2  91/02/11  16:59:16  root
# Version 2.0 Source
# 
 * Revision 2.2  87/04/22  15:12:25  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include "nqs.h"			/* NQS types and definitions */
#include "nqsdirs.h"			/* NQS directory and file hierarchy */
#if	BSD42 | BSD43 | ULTRIX 
#include <sys/time.h>
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
struct utimbuf {
	time_t acctime;			/* Access time */
	time_t modtime;			/* Modification time */
};
#else
BAD SYSTEM TYPE
#endif
#endif
#include <sys/stat.h>			/* Must be included after nqs.h */
					/* since nqs.h includes sys/types.h */


extern int errno;			/* System call error number */
extern struct group *getgrnam();	/* Get group structure by name */
extern struct passwd *getpwnam();	/* Get passwd structure by name */
#if 	UNICOS & NEWPWD
extern void getpwsysent();		/* Force use of new password file */
#endif
extern uid_t getuid();			/* Get real user-id */
extern void nqssleep();			/* Sleep for N seconds */
extern void pack6name();		/* Compute name on 6-bit alphabet */
extern void sync();			/* Schedule system buffers for */
					/* writing */
extern char *sys_errlist[];		/* Error list explanation */
extern time_t time();			/* Get time */
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
	struct stat stat_buf;		/* Stat buffer */
	char path [MAX_PATHNAME+1];	/* NQS pathname */
	char linkpath [MAX_PATHNAME+1];	/* NQS link pathname */
	struct group *group;		/* Group entry for NQS_GROUP */
	struct passwd *passwd;		/* Password entry for NQS_OWNER */
	register int i;			/* Iteration counter */
	int fd;				/* File-descriptor */
	char base_dir[256];

	if (getuid() != 0) {
		/*
		 *  You must be running as root to run this utility.
		 */
		printf ("Utility [nqsmktrans]:  User is not root.\n");
		printf ("Utility [nqsmktrans]:  Aborting.\n");
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
		printf ("Utility [nqsmktrans]:  NQS_GROUP def invalid.\n");
		printf ("Utility [nqsmktrans]:  Aborting.\n");
		fflush (stdout);
		fflush (stderr);
		exit (2);
	}
#if UNICOS & NEWPWD
	if (acctid (0,passwd->pw_acid[0]) == -1) {
		/*
		 * 	The account was invalid!
		 */
		printf ("Utility [nqsmktrans]:  Invalid account ID\n");
		fflush (stdout);
		fflush (stderr);
		exit (3);	
	}
#endif
	if (setgid (group->gr_gid) == -1) {
		/*
		 *  The setgid() call failed!
		 */
		printf ("Utility [nqsmktrans]:  Unable to setgid (%1d).\n",
			group->gr_gid);
		printf ("Utility [nqsmktrans]:  Reason: %s.\n",
			sys_errlist [errno]);
		printf ("Utility [nqsmktrans]:  Aborting.\n");
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
		printf ("Utility [nqsmktrans]:  NQS_OWNER def invalid.\n");
		printf ("Utility [nqsmktrans]:  Aborting.\n");
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
		printf ("Utility [nqsmktrans]:  Unable to chdir (base_dir).\n");
		printf ("Utility [nqsmktrans]:  Reason: %s.\n",
			sys_errlist [errno]);
		printf ("Utility [nqsmktrans]:  Aborting.\n");
		fflush (stdout);
		fflush (stderr);
		exit (5);
	}
	/*
	 *  The transaction descriptors that we are going to create, should be
	 *  created with a mode of 777.  Therefore, we clear the umask....
	 */
	umask (0);
	/*
	 *  Build the necessary transaction descriptors, being very careful
	 *  not to disturb any existing transaction descriptors.  We are
	 *  also very careful to construct things in an order in which we
	 *  can gracefully recover from a system crash during the construction
	 *  process.
	 */
	printf ("Utility [nqsmktrans]:  Beginning transaction descriptor ");
	printf ("construction:\n\n");
	fflush (stdout);
	for (i=1; i <= MAX_TRANSACTS; i++) {
		/*
		 *  Remove any leftover temporary link to a transaction
		 *  descriptor element #0 for a transaction descriptor
		 *  that was under construction.
		 */
		pack6name (linkpath, Nqs_transact,
			  (int) ((i-1) % MAX_TDSCSUBDIRS), "TEMP_",
			  (long) i-1, 6, 0L, 0, 0, 1);
		unlink (linkpath);
		/*
		 *  Find out whether or not the transaction descriptor we
		 *  are trying to create already exists.  If it does, then
		 *  DO NOT change it.  The transaction descriptor MIGHT be
		 *  describing an existing NQS request!
		 */
		pack6name (path, Nqs_transact, (int) ((i-1) % MAX_TDSCSUBDIRS),
			  (char *) 0, (long) i-1, 6, 0L, 0, 0, 1);
		if (stat (path, &stat_buf) == -1) {
			/*
			 *  We were unable to stat element #0 of the
			 *  transaction descriptor that we were testing
			 *  to see if it already existed.
			 */
			if (errno != ENOENT) {
				/*
				 *  Aaaak!  Bill the cat.
				 */
				printf ("\nUtility [nqsmktrans]:  Stat() ");
				printf ("error.\n");
				printf ("Utility [nqsmktrans]:  Reason: %s.\n",
					sys_errlist [errno]);
				printf ("Utility [nqsmktrans]:  Aborting.\n");
				fflush (stdout);
				fflush (stderr);
				sync();	/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks referring to the */
					/* newly created transaction descrs, */
					/* and the transaction descrs them- */
					/* selves), are securely written to */
					/* disk */
				exit (6);
			}
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
			/*
			 *  This transaction descriptor does not exist.
			 *  Create it as an unallocated descriptor.
			 *
			 *  Note that we create elements #1, #2, and #3
			 *  first.  Element #0 is created last under
			 *  a temp name, utimed, and then linked in
			 *  under the permanent name so that when a
			 *  transaction descriptor completely appears,
			 *  it is also completely initialized as
			 *  unallocated.
			 */
			pack6name (path, Nqs_transact,
				  (int) ((i-1) % MAX_TDSCSUBDIRS),
				  (char *) 0, (long) i-1, 6, 0L, 0, 1, 1);
/*  DEBUGGING	*/
printf("path=%s, uid=%d, gid=%d\n", path, (int)passwd->pw_uid, (int)group->gr_gid);
			if ((fd = creat (path, 0777)) == -1 ||
			    chown (path, (int) passwd->pw_uid,
				  (int) group->gr_gid) == -1) {
				/*
				 *  This should have worked.
				 */
perror("creat");

				printf ("\nUtility [nqsmktrans]:  Error ");
				printf ("creating element #1 of trans: %1d.\n",
					i);
				printf ("Utility [nqsmktrans]:  Reason: %s.\n",
					sys_errlist [errno]);
				printf ("Utility [nqsmktrans]:  Aborting.\n");
				fflush (stdout);
				fflush (stderr);
				sync();	/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks referring to the */
					/* newly created transaction descrs, */
					/* and the transaction descrs them- */
					/* selves), are securely written to */
					/* disk */
				exit (7);
			}
			close (fd);
			pack6name (path, Nqs_transact,
				  (int) ((i-1) % MAX_TDSCSUBDIRS),
				  (char *) 0, (long) i-1, 6, 0L, 0, 2, 1);
			if ((fd = creat (path, 0777)) == -1 ||
			    chown (path, (int) passwd->pw_uid,
				  (int) group->gr_gid) == -1) {
				/*
				 *  This should have worked.
				 */
				printf ("\nUtility [nqsmktrans]:  Error ");
				printf ("creating element #2 of trans: %1d.\n",
					i);
				printf ("Utility [nqsmktrans]:  Reason: %s.\n",
					sys_errlist [errno]);
				printf ("Utility [nqsmktrans]:  Aborting.\n");
				fflush (stdout);
				fflush (stderr);
				sync();	/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks referring to the */
					/* newly created transaction descrs, */
					/* and the transaction descrs them- */
					/* selves), are securely written to */
					/* disk */
				exit (8);
			}
			close (fd);
			pack6name (path, Nqs_transact,
				  (int) ((i-1) % MAX_TDSCSUBDIRS),
				  (char *) 0, (long) i-1, 6, 0L, 0, 3, 1);
			if ((fd = creat (path, 0777)) == -1 ||
			    chown (path, (int) passwd->pw_uid,
				  (int) group->gr_gid) == -1) {
				/*
				 *  This should have worked.
				 */
				printf ("\nUtility [nqsmktrans]:  Error ");
				printf ("creating element #3 of trans: %1d.\n",
					i);
				printf ("Utility [nqsmktrans]:  Reason: %s.\n",
					sys_errlist [errno]);
				printf ("Utility [nqsmktrans]:  Aborting.\n");
				fflush (stdout);
				fflush (stderr);
				sync();	/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks referring to the */
					/* newly created transaction descrs, */
					/* and the transaction descrs them- */
					/* selves), are securely written to */
					/* disk */
				exit (9);
			}
			close (fd);
			if (i == MAX_TRANSACTS) {
				/*
				 *  We are completing the construction of
				 *  the final transaction descriptor.  Since
				 *  the NQS daemon when booting tests for
				 *  the existence of all transaction descr-
				 *  iptors (indicated by the MAX_TRANSACTS
				 *  parameter), by reading the very last
				 *  transaction descriptor (descr number
				 *  MAX_TRANSACTS), we MUST make sure that
				 *  ALL previously created transaction
				 *  descriptors are safely written to disk,
				 *  before continuing.
				 */
				sync();	/* Schedule all dirty disk cache */
					/* buffers for writing */
				printf ("\nUtility [nqsmktrans]:  Waiting ");
				printf ("for permanent storage update.\n");
				printf ("Utility [nqsmktrans]:  Wait time ");
				printf ("= 10 seconds.\n");
				fflush (stdout);
				nqssleep (10);
					/* Wait a conservative length of */
					/* time for the permanent storage */
					/* of the system to record the */
					/* state of the previously */
					/* created transaction descriptors */
				printf ("Utility [nqsmktrans]:  Completing ");
				printf ("construction of final transaction ");
				printf ("descriptor.\n");
				fflush (stdout);
			}
			/*
			 *  Now, create what will become element #0 of
			 *  the transaction descriptor under a temporary
			 *  name.
			 */
			pack6name (path, Nqs_transact,
				  (int) ((i-1) % MAX_TDSCSUBDIRS), "TEMP_",
				  (long) i-1, 6, 0L, 0, 0, 1);
			if ((fd = creat (path, 0777)) == -1 ||
/*
 *	Note that we set the modification time of element 0 to reflect
 *	an unallocated transaction descriptor.  This is CRITICAL.
 *	If the format of a transaction descriptor ever changes (see
 *	../lib/transact.c), then this code must change appropriately.
 */
			    set_mtime (path, (unsigned long)
				      (TRA_UNALLOCATED << 30)) == -1 ||
			    chown (path, (int) passwd->pw_uid,
				  (int) group->gr_gid) == -1) {
				/*
				 *  This should have worked.
				 */
				printf ("\nUtility [nqsmktrans]:  Error ");
				printf ("creating element #0 of trans: %1d.\n",
					i);
				printf ("Utility [nqsmktrans]:  Reason: %s.\n",
					sys_errlist [errno]);
				printf ("Utility [nqsmktrans]:  Aborting.\n");
				fflush (stdout);
				fflush (stderr);
				sync();	/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks referring to the */
					/* newly created transaction descrs, */
					/* and the transaction descrs them- */
					/* selves), are securely written to */
					/* disk */
				exit (10);
			}
			close (fd);
			/*
			 *  Now, link-in the last element of the descriptor
			 *  being created.
			 */
			pack6name (linkpath, Nqs_transact,
				  (int) ((i-1) % MAX_TDSCSUBDIRS),
				  (char *) 0, (long) i-1, 6, 0L, 0, 0, 1);
			if (link (path, linkpath) == -1 ||
			    unlink (path) == -1) {
				/*
				 *  This should have worked.
				 */
				printf ("\nUtility [nqsmktrans]:  Error ");
				printf ("completing construction of\n");
				printf ("trans-descr: %1d.\n", i);
				printf ("Utility [nqsmktrans]:  Reason: %s.\n",
					sys_errlist [errno]);
				printf ("Utility [nqsmktrans]:  Aborting.\n");
				fflush (stdout);
				fflush (stderr);
				sync();	/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks referring to the */
					/* newly created transaction descrs, */
					/* and the transaction descrs them- */
					/* selves), are securely written to */
					/* disk */
				exit (11);
			}
#else
#if	UNICOS
			/*
			 *  This transaction descriptor does not exist.
			 *  Create it as an unallocated descriptor.
			 *
			 *  Note the we create element #1 first.
			 *  Element #0 is created last under a temp name,
			 *  utimed, and then linked in under the permanent
			 *  name so that when a transaction descriptor
			 *  completely appears, it is also completely
			 *  initialized as unallocated.
			 */
			pack6name (path, Nqs_transact,
				  (int) ((i-1) % MAX_TDSCSUBDIRS),
				  (char *) 0, (long) i-1, 6, 0L, 0, 1, 1);
			if ((fd = creat (path, 0777)) == -1 ||
			    chown (path, (int) passwd->pw_uid,
				  (int) group->gr_gid) == -1) {
				/*
				 *  This should have worked.
				 */
				printf ("\nUtility [nqsmktrans]:  Error ");
				printf ("creating element #1 of trans: %1d.\n",
					i);
				printf ("Utility [nqsmktrans]:  Reason: %s.\n",
					sys_errlist [errno]);
				printf ("Utility [nqsmktrans]:  Aborting.\n");
				fflush (stdout);
				fflush (stderr);
				sync();	/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks referring to the */
					/* newly created transaction descrs, */
					/* and the transaction descrs them- */
					/* selves), are securely written to */
					/* disk */
				exit (7);
			}
			close (fd);
			if (i == MAX_TRANSACTS) {
				/*
				 *  We are completing the construction of
				 *  the final transaction descriptor.  Since
				 *  the NQS daemon when booting tests for
				 *  the existence of all transaction descr-
				 *  iptors (indicated by the MAX_TRANSACTS
				 *  parameter), by reading the very last
				 *  transaction descriptor (descr number
				 *  MAX_TRANSACTS), we MUST make sure that
				 *  ALL previously created transaction
				 *  descriptors are safely written to disk,
				 *  before continuing.
				 */
				sync();	/* Schedule all dirty disk cache */
					/* buffers for writing */
				printf ("\nUtility [nqsmktrans]:  Waiting ");
				printf ("for permanent storage update.\n");
				printf ("Utility [nqsmktrans]:  Wait time ");
				printf ("= 10 seconds.\n");
				fflush (stdout);
				nqssleep (10);
					/* Wait a conservative length of */
					/* time for the permanent storage */
					/* of the system to record the */
					/* state of the previously */
					/* created transaction descriptors */
				printf ("Utility [nqsmktrans]:  Completing ");
				printf ("construction of final transaction ");
				printf ("descriptor.\n");
				fflush (stdout);
			}
			/*
			 *  Now, create what will become element #0 of
			 *  the transaction descriptor under a temporary
			 *  name.
			 */
			pack6name (path, Nqs_transact,
				  (int) ((i-1) % MAX_TDSCSUBDIRS), "TEMP_",
				  (long) i-1, 6, 0L, 0, 0, 1);
			if ((fd = creat (path, 0777)) == -1 ||
/*
 *	Note that we set the modification time of element 0 to reflect
 *	an unallocated transaction descriptor.  This is CRITICAL.
 *	If the format of a transaction descriptor ever changes (see
 *	../lib/transact.c), then this code must change appropriately.
 */
			    set_mtime (path, (unsigned long)
				      (TRA_UNALLOCATED << 62)) == -1 ||
			    chown (path, (int) passwd->pw_uid,
				  (int) group->gr_gid) == -1) {
				/*
				 *  This should have worked.
				 */
				printf ("\nUtility [nqsmktrans]:  Error ");
				printf ("creating element #0 of trans: %1d.\n",
					i);
				printf ("Utility [nqsmktrans]:  Reason: %s.\n",
					sys_errlist [errno]);
				printf ("Utility [nqsmktrans]:  Aborting.\n");
				fflush (stdout);
				fflush (stderr);
				sync();	/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks referring to the */
					/* newly created transaction descrs, */
					/* and the transaction descrs them- */
					/* selves), are securely written to */
					/* disk */
				exit (8);
			}
			close (fd);
			/*
			 *  Now, link-in the last element of the descriptor
			 *  being created.
			 */
			pack6name (linkpath, Nqs_transact,
				  (int) ((i-1) % MAX_TDSCSUBDIRS),
				  (char *) 0, (long) i-1, 6, 0L, 0, 0, 1);
			if (link (path, linkpath) == -1 ||
			    unlink (path) == -1) {
				/*
				 *  This should have worked.
				 */
				printf ("\nUtility [nqsmktrans]:  Error ");
				printf ("completing construction of\n");
				printf ("trans-descr: %1d.\n", i);
				printf ("Utility [nqsmktrans]:  Reason: %s.\n",
					sys_errlist [errno]);
				printf ("Utility [nqsmktrans]:  Aborting.\n");
				fflush (stdout);
				fflush (stderr);
				sync();	/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks referring to the */
					/* newly created transaction descrs, */
					/* and the transaction descrs them- */
					/* selves), are securely written to */
					/* disk */
				exit (9);
			}
#else
BAD SYSTEM TYPE
#endif
#endif
		}
		printf ("%6d", i);
		if (i % 13 == 0) putchar ('\n');
		fflush (stdout);
	}
	printf ("\nUtility [nqsmktrans]:  NQS transaction descriptor ");
	printf ("construction complete.\n");
	printf ("Utility [nqsmktrans]:  Exiting.\n");
	fflush (stdout);
	fflush (stderr);
	sync();				/* Schedule all dirty system I/O */
					/* buffers for writing (so that */
					/* directory blocks referring to the */
					/* newly created transaction descrs, */
					/* (and the transation descrs them- */
					/* selves), are securely written to */
					/* disk */
	exit (0);
}


/*** set_mtime
 *
 *
 *	int set_mtime():
 *
 *	Set the modification time of the specified transaction
 *	descriptor element inode.
 *
 *	Returns:
 *		 0: if successful.
 *		-1: if unsuccessful (errno has reason).
 */
static int set_mtime (path, ulongval)
char *path;				/* Transaction descriptor path */
unsigned long ulongval;			/* Unsigned long datum */
{
#if	BSD42 | BSD43 | ULTRIX
	struct timeval utimbuf [2];	/* Utimes() buffer */
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
	struct utimbuf utimbuf;		/* utime() buffer */
#else
BAD SYSTEM TYPE
#endif
#endif
#if	BSD42 | BSD43 | ULTRIX
	time (&utimbuf [0].tv_sec);	/* Access time = current time */
	utimbuf [0].tv_usec = 0;
	utimbuf [1].tv_sec = ulongval;
	utimbuf [1].tv_usec = 0;
	return (utimes (path, utimbuf));
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
	time (&utimbuf.acctime);	/* Access time = current time */
	utimbuf.modtime = ulongval;
	return (utime (path, &utimbuf));
#else
BAD SYSTEM TYPE
#endif
#endif
}
