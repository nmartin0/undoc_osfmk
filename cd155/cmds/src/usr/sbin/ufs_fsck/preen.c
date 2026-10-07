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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 *
 * $Log: preen.c,v $
 * Revision 1.15  1994/11/19  03:18:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.14  1994/09/06  20:22:01  flb
 *  Reviewer: stans
 *  Risk: low
 *  Benefit or PTS #: 10611
 *  Testing: In use on the XPS-150MP ORNL SCAT machine.
 * 	Boot system with a large number of remote raids.
 * 	Watch LEDs to verify that more that multiple
 * 	remote RAID's are fsck'ing at the same time.
 *  Module(s): .../cmds_libs/src/usr/sbin/ufs_fsck/preen.c
 *  Description: Moved #define DO_RFORK 1 under #ifdef OSF_ADFS
 *
 * Revision 1.13  1994/08/31  20:24:19  bradf
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.11.2.1  1994/08/17  17:58:52  stans
 *   Make fsck go faster by fixing 'ufs_fsck' to check more than one parition
 *   per rfork() on a remote i/o node.
 *
 *   Reviewer: Adam M., stans
 *   Risk: medium
 *   PTS Bug #10611
 *   Testing: booted system with 71 RAIDs multiple partitions per RAID.
 *   Module(s): cmds_libs/src/usr/sbin/ufs_fsck/preen.c
 *
 * Revision 1.11  1994/07/15  20:10:32  shala
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Bug #9941
 *  Testing: Made a new ufs_fsck and fsck'ed file systems.
 *  Module(s): cmds_libs/src/usr/sbin/ufs_fsck/main.c
 *             cmds_libs/src/usr/sbin/ufs_fsck/preen.c
 *             cmds_libs/src/usr/sbin/ufs_fsck/utilities.c
 *
 * Close file descriptors before exiting the programs.
 *
 * Revision 1.10  1993/09/08  21:43:01  shala
 * Comment changes. Fixed bug #6506 by stans.
 *
 * Revision 1.9.6.1  1993/09/08  21:34:03  shala
 * Comment changes. Fixed bug #6506 by stans.
 *
 * Revision 1.9  1993/06/24  22:14:08  stans
 * 	Use devstat() syscall directly instead of syscall(DEVSTAT) interface.
 * 	More startdisk() error processing; entrie mess really needs to be
 * 	rewritten. Fixed problem when EPBADNODE was returned and nrun == 0
 * 	the code skipped any following disk. Now we just skip the disk
 * 	which returned EPBADNODE and continue on processing.
 *
 * Revision 1.8  1993/06/24  21:14:44  stans
 *    Handle the case of getServerName() returing a (-1) which indicates the
 *    node handling the device (both physically and logically (DEV_TAB)) is
 *    down. Also embellish the error handling calling startdisk(). Now we
 *    check for EPBADNODE and bail out if true, otherwise errors are ignored
 *    and the {r}fork() is attempted again.
 *
 * Revision 1.7  1993/06/24  16:29:22  stans
 *    Modified fork() failure error message to indicate "rfork()" instead of
 *    always "fork()".
 *
 * Revision 1.6  1993/06/09  19:14:02  stans
 *    Misc comments. "-E" ending pass switch support has been removed in
 *    favor of OSF blessed "-Pstart..end" syntax. Same functionality,
 *    different user cmd line interface.
 *
 * Revision 1.5  1993/05/17  22:18:19  stans
 * Support 'rfork()' by reading DEV_TAB boot magic variable to provide a
 * mapping between where the device is located (inode node number) and
 * the node number of the UX server which provides support for the device.
 *
 * Revision 1.4  1993/04/20  17:38:23  stans
 *   Support '-En' "/etc/fstab" pass switch. "-En" imples fstab processing
 *   will end after fstab pass 'n' has been completed. Switch is utilized
 *   to control fsck & mounting of local and/or remote filesystems during the
 *   multi-user boot sequence.
 *
 * Revision 1.3  1993/03/25  22:19:45  stans
 * Support "-P fstab-pass-number" switch. "-P pass" implies that the
 * filesystem checks should start with pass number 'pas' as defined in
 * '/etc/fstab' passno field. If '/etc/fstab/' is defined according to
 * fstab rules: only the root filesystem at pass number #1 while everything
 * else is > 1. Now by specifing "-P 2" one can run fsck on all filesystem
 * EXCEPT the root. This functionality is required during booting after the
 * mesh has been started.  Real win here is that fsck will do parallel FS
 * checks ONLY if processing fstab, no cmd line FS specifications.
 * Routine checkfstab(), 1 new argument which identifies the fstab pass-number
 * on which to start filesystem checking.
 *
 * Revision 1.2  1992/10/12  22:08:19  shala
 * New version to support maj, min and node numbers.
 *
 * Revision 2.2  92/06/10  17:44:19  pjg
 * 	06/10/92 srl and durriya
 * 	changes for OSF1_ADFS.
 *	Cleaned up use of devstat, and prevented bomb out when run on
 *	non-OSF1_ADFS system.  Don't use message catalog for OSF1_ADFS.
 * 
 * Revision 1.7.2.3  1992/02/25  13:51:40  garyf
 * 	expand buffer used by rawname
 * 	[1992/02/25  13:51:07  garyf]
 *
 * Revision 1.7.2.2  1991/12/31  14:39:13  garyf
 * 	copy string before passing to filesystem checking
 * 	so rawname, unrawname can be called externally
 * 	[1991/12/31  14:36:51  garyf]
 * 
 * Revision 1.7  1991/06/20  12:33:48  devrcs
 * 	Source merge for osc1.1b8
 * 	[91/06/11  14:30:12  devrcs]
 * 
 * 	move preen messages to separate set
 * 	[91/06/07  08:46:17  garyf]
 * 
 * 	add messaging support
 * 	[91/05/13  11:35:05  garyf]
 * 
 * Revision 1.6  91/06/10  16:37:39  devrcs
 * 	add messaging support
 * 	[91/05/13  11:35:05  garyf]
 * 
 * Revision 1.5  91/01/07  17:23:51  devrcs
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:33:23  dwm]
 * 
 * Revision 1.4  90/10/07  22:51:36  devrcs
 * 	Fix typo.
 * 	[90/10/02  10:05:31  gmf]
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	[90/10/02  09:41:02  gmf]
 * 
 * Revision 1.3  90/07/27  11:45:41  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:31:41  pam]
 * 
 * Revision 1.2  90/06/22  22:13:08  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:32:19  gmf]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: preen.c,v $ $Revision: 1.15 $ (OSF) $Date: 1994/11/19 03:18:19 $";
#endif
/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*** "preen.c	5.6 (Berkeley) 8/7/90"; ***/

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fstab.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include <mach/boolean.h>

#ifdef  OSF1_ADFS

#define DO_RFORK 1	/* go remote: use rfork() instead of fork() */

#define MSGSTR(n,s) s

#else	/* if not OSF1_ADFS */

#include <locale.h>
#include "ufs_fsck_msg.h"

#ifndef	DEBUG
#define	DEBUG 0
#endif

extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_UFS_FSCK,n,s) 
#endif  /* OSF1_ADFS */

char	*rawname(), *unrawname(), *blockcheck();
void	addpart();

struct part {
	struct	part *next;		/* forward link of partitions on disk */
	char	*name;			/* device name */
	char	*fsname;		/* mounted filesystem name */
	long	auxdata;		/* auxillary data for application */
} *badlist, **badnext = &badlist;

struct disk {
	char	*name;			/* disk base name */
	struct	disk *next;		/* forward link for list of disks */
	struct	part *part;		/* head of list of partitions on disk */
	int	pid;			/* If != 0, pid of proc working on */
} *disks;

int	nrun, ndisks;
char	hotroot;
extern	RFork;

#define	MAX_DISKNAME (PATH_MAX + 1)

#define AD_DO_ALL_PARTITONS 0x80000000

extern int fsreadfd;
extern int fswritefd;

/*
 * check the filesystem listed in '/etc/fstab'
 *
 * inputs:
 *	startingPass	fstab pass number to start with, defaults to 1.
 *	endingPass	fstab pass number to stop after, defaults to 0.
 *	preen		boolean flag "-p"
 *	maxrun		max # of parallel filesystem checks.
 *	docheck		function pointer: check if FS needs to be fsck'ed.
 *	chkit		function pointer: actually check the FS
 *
 * outputs:
 *
 */
checkfstab(startingPass, endingPass, preen, maxrun, docheck, chkit)
	int	startingPass, endingPass, preen, maxrun;
	int	(*docheck)(), (*chkit)();
{
	register struct fstab *fsp;
	register struct disk *dk, *nextdisk;
	register struct part *pt;
	int rc, pid, retcode, passno, sumstatus, status;
	long auxdata, node;
	char *tempname, name[MAX_DISKNAME];


#if	DO_RFORK
	/* get my node number (OS root partition) */
	norma_node_self(mach_task_self(), &node);

	/* must defeat partitioning to be able to rfork() to any node */
	if ( RFork ) {
		if (join_root_part() < 0 )
			perror("join_root_part()");
	}
#endif
	/*
	 * Scan fstab twice. First time, if not preening, do checks on all 
	 * entries corresponding to selected pass range. If preening, and pass
	 * 1 is in selected pass range, check it. Second time around, build a 
	 * list of entries selected by pass range excluding pass 1 entries for
	 * use below.
	 */
	sumstatus = 0;
	for (passno = 1; passno <= 2; passno++) {
		if (setfsent() == 0) {
			fprintf(stderr, MSGSTR(NOCHKLST, "Can't open checklist file: %s\n"),
			    _PATH_FSTAB);
			return (8);
		}
		while ((fsp = getfsent()) != 0) {
			if ((auxdata = (*docheck)(fsp)) == 0)
				continue;
			/*
			 * Do we want to do this pass?
			 */
			if ( fsp->fs_passno < startingPass )
				continue; 
			if ( endingPass && (fsp->fs_passno > endingPass) )
				continue; 

			if (preen == 0 || passno == 1 && fsp->fs_passno == 1) {
				tempname = blockcheck(fsp->fs_spec);
				if (tempname) {
					strcpy(name, tempname);
					if (sumstatus = (*chkit)(name, 
						fsp->fs_file, auxdata, 0))
						return (sumstatus);
				} else if (preen)
					return (8);
			} else if (passno == 2 && fsp->fs_passno > 1) {
				if ((tempname = blockcheck(fsp->fs_spec)) == NULL) {
					fprintf(stderr, MSGSTR(BADDISK, "BAD DISK NAME %s\n"),
						fsp->fs_spec);
					sumstatus |= 8;
					continue;
				}
				/* 
				 * add partition to corresponding disk in
				 * disk list "disks"
				 */
				strcpy(name, tempname);
				addpart(name, fsp->fs_file, auxdata);
			}
		}
		if (preen == 0)
			return (0);
	}
	if (preen) {
		if (maxrun == 0)
			maxrun = ndisks;
		if (maxrun > ndisks)
			maxrun = ndisks;
#if DEBUG
fprintf(stderr,"%d parallel checks\n",maxrun);
#endif
		/*
		 * start 'maxrun' children, each checking a disk. Multiple
		 * partitions are checked if the disk is on a remote node.
		 * Assume: a local fork() is cheap compared to a remote rfork().
		 * If a disk is remote, then do as much work as possible while
		 * there. For the local case, fork() for each partition in the
		 * disk.
		 */
		nextdisk = disks;
		for (passno = 0; passno < maxrun; ++passno) {
			/*
			 * startdisk() returns 0 for success.
			 * 'nrun' is bumped in startdisk()
			 */
			while((rc=startdisk(nextdisk,chkit,node)) && nrun > 0) {
				if ( rc == EPBADNODE ) {
					/* skip this disk */
					break;
				}
				sleep(10); /* error - retry */
			}
			/*
			 * if a bad node, device rmknod'ed wrong, then just
			 * skip this disk.
			 */
			if (rc && (rc != EPBADNODE) )
				return (rc);

			/* check next disk */
			nextdisk = nextdisk->next;
		}

		/*
		 * wait for the 'maxrun. children to finish, if more
		 * disks/partitions to check then serially start a child to
		 * to check, wait(); repeat until done.
		 */
		while ((pid = wait(&status)) != -1) {
			/* find exiting child's disk struct */
			for (dk = disks; dk; dk = dk->next)
				if (dk->pid == pid)
					break;
			if (dk == 0) {
				printf(MSGSTR(BADPID, "Unknown pid %d\n"), pid);
				continue;
			}

#if DEBUG
fprintf(stderr,"finished %d\n",dk->pid);
#endif

			if (WIFEXITED(status))
				retcode = WEXITSTATUS(status);
			else
				retcode = 0;
			if (WIFSIGNALED(status)) {
				printf(MSGSTR(EXITSIG, "%s (%s): EXITED WITH SIGNAL %d\n"),
					dk->part->name, dk->part->fsname,
					WTERMSIG(status));
				retcode = 8;
			}

			if (retcode != 0) {
				sumstatus |= retcode;
				*badnext = dk->part;
				badnext = &dk->part->next;
				dk->part = dk->part->next;
				*badnext = NULL;
			} else {
				/*
				 * for a remote FS all partitions are checked
				 * in one fell swoop. Indicated by the flag in
				 * auxdata.
				 */
				if (dk->part->auxdata & AD_DO_ALL_PARTITONS) {
					free_partition_list(dk->part);
					dk->part = NULL;
				}
				else
					dk->part = dk->part->next;
			}

			dk->pid = 0;
			nrun--;	/* one less child running */

			if (dk->part == NULL)
				ndisks--;	/* one less disk */

			/* finished with all disks? */
			if (nextdisk == NULL) {
				if (dk->part) {
					while ((rc=startdisk(dk, chkit, node))
					    && nrun > 0) {
						if ( rc == EPBADNODE ) {
							/* skip this disk */
							break;
						}
						sleep(10); /* error - retry */
					}
					if (rc && (rc != EPBADNODE) )
						return (rc);
				}
			} else if (nrun < maxrun && nrun < ndisks) {
				/*
				 * more disks to check, find a partition.
				 */
				for ( ;; ) {
					if ((nextdisk = nextdisk->next) == NULL)
						nextdisk = disks;
					if (nextdisk->part != NULL &&
					    nextdisk->pid == 0)
						break;
				}
				/*
				 * found a disk/partition, create one child to
				 * check it.
				 */
				while ((rc=startdisk(nextdisk, chkit, node))
				    && nrun > 0) {
					if ( rc == EPBADNODE ) {
						/* skip this disk */
						break;
					}
					sleep(10); /* error - retry */
				}
				if (rc && (rc != EPBADNODE) )
					return (rc);
			}
		}
	}
	if (sumstatus) {
		if (badlist == 0)
			return (sumstatus);
		fprintf(stderr, MSGSTR(FOLLOWING,
 "THE FOLLOWING FILE SYSTEMS HAD AN UNEXPECTED INCONSISTENCY:\n\t"));
		for (pt = badlist; pt; pt = pt->next)
			fprintf(stderr, "%s (%s)%s", pt->name, pt->fsname,
			    pt->next ? ", " : "\n");
		return (sumstatus);
	}
	(void)endfsent();
	return (0);
}

struct disk *
finddisk(name)
	char *name;
{
	register struct disk *dk, **dkp;
	register char *p;
	size_t len;

	for (p = name + strlen(name) - 1; p >= name; --p)
		if (isdigit(*p)) {
			len = p - name + 1;
			break;
		}
	if (p < name)
		len = strlen(name);

	for (dk = disks, dkp = &disks; dk; dkp = &dk->next, dk = dk->next) {
		if (strncmp(dk->name, name, len) == 0 &&
		    dk->name[len] == 0) {
			return (dk);
		}
	}
	if ((*dkp = (struct disk *)malloc(sizeof(struct disk))) == NULL) {
		fprintf(stderr, MSGSTR(NOMEM0, "out of memory"));
                (void)close(fsreadfd);
                (void)close(fswritefd);
		exit (8);
	}
	dk = *dkp;
	if ((dk->name = malloc(len + 1)) == NULL) {
		fprintf(stderr, MSGSTR(NOMEM0, "out of memory"));
                (void)close(fsreadfd);
                (void)close(fswritefd);
		exit (8);
	}
	(void)strncpy(dk->name, name, len);
	dk->name[len] = '\0';
	dk->part = NULL;
	dk->next = NULL;
	dk->pid = 0;
	ndisks++;
	return (dk);
}

void
addpart(name, fsname, auxdata)
	char *name, *fsname;
	long auxdata;
{
	struct disk *dk = finddisk(name);
	register struct part *pt, **ppt = &dk->part;

	for (pt = dk->part; pt; ppt = &pt->next, pt = pt->next)
		if (strcmp(pt->name, name) == 0) {
			printf(MSGSTR(FSTAB1, "%s in fstab more than once!\n"), name);
			return;
		}
	if ((*ppt = (struct part *)malloc(sizeof(struct part))) == NULL) {
		fprintf(stderr, MSGSTR(NOMEM0, "out of memory"));
                (void)close(fsreadfd);
                (void)close(fswritefd);
		exit (8);
	}
	pt = *ppt;
	if ((pt->name = malloc(strlen(name) + 1)) == NULL) {
		fprintf(stderr, MSGSTR(NOMEM0, "out of memory"));
                (void)close(fsreadfd);
                (void)close(fswritefd);
		exit (8);
	}
	(void)strcpy(pt->name, name);
	if ((pt->fsname = malloc(strlen(fsname) + 1)) == NULL) {
		fprintf(stderr, MSGSTR(NOMEM0, "out of memory"));
                (void)close(fsreadfd);
                (void)close(fswritefd);
		exit (8);
	}
	(void)strcpy(pt->fsname, fsname);
	pt->next = NULL;
	pt->auxdata = auxdata;
}

/*
 * release memory associated with partition list.
 */
free_partition_list(pt)
	register struct part *pt;
{
	register struct part *pt_sav;

	while( pt ) {
		free( pt->name );
		free( pt->fsname );
		pt_sav = pt;
		pt = pt->next;	/* next part struct */
		free(pt_sav);
	}
}

/*
 * {r}fork() a child to perform a filesystem check.
 *
 * inputs:
 *	dk		ptr to disk descriptor
 *	checkit		a function pointer to check the filesystem
 *	local_node	node number (OS root partition) of where we are
 *			executing.
 *
 * outputs:
 *	0	success
 *	otherwise an error code (errno.h).
 */

startdisk(dk, checkit, local_node)
	struct disk	*dk;
	int		(*checkit)();
	int		local_node;	
{
	register struct part	*pt = dk->part;
	int			node;
	long			st=0;

	dk->pid = -1;	/* assume the worst */

#if	DO_RFORK
	/*
	 * retrieve the node number where this filesystem is serviced.
	 */
	if ( (node = getServerNode(pt->name)) == -1) {
		extern int	errno;

		/* node is not up, skip the disk check */
		fprintf(stderr,"  '%s', rfork(%d): %s\n",pt->name,errno,
			sys_errlist[EPBADNODE]);
		return( EPBADNODE );
	}

#if DEBUG
fprintf(stderr,"start %s @ %d\n",pt->name,node);
#endif

	/*
	 * local disk drive or remote ?
	 */
	if ( RFork && (node != local_node) ) {
		/*
		 * flag we will do ALL partitions on a remote node instead of
		 * an rfork() per partition.
		 */
		pt->auxdata |= AD_DO_ALL_PARTITONS;
		dk->pid = rfork(node);	/* go remote */
	}
	else {
		dk->pid = fork();	/* stay local */
	}
#else	/* RFORK */
	dk->pid = fork();
#endif	/* RFORK */

	/*
	 * fork errors?
	 */
	if (dk->pid < 0) {
#if	DO_RFORK
		perror(MSGSTR(FORK, "rfork()"));
#else	/* DO_RFORK */
		perror(MSGSTR(FORK, "fork()"));
#endif	/* DO_RFORK */
		return (ENOEXEC);
	}
	/*
	 * child does the work...
	 */
	if (dk->pid == 0) {
		/*
		 * Are we doing ALL partions? (i.e., we are a remote node)
		 */
		if ( pt->auxdata & AD_DO_ALL_PARTITONS ) {
#if DEBUG
			fprintf(stderr,"[%d] remote fsck(%d):\n",node,getpid());
#endif
			/*
			 * do all requested paritions on this drive as the
			 * rfork()'s to get here is expensive. We rfork()'ed
			 * here so all i/o to the drive would be local hence
			 * we could remove NORMA IPC from the device path.
			 * Do as much as possible (all paritions listed) on
			 * this drive while we are here.
			 */
			do {
#if DEBUG
				fprintf(stderr,"  %s\n",pt->name);
#endif
				/* call FS check routine: auxdata is ignored */
				st |= (*checkit)(pt->name, pt->fsname,
								pt->auxdata, 1);
			} while( (pt = pt->next) );
		}
		else {
#if DEBUG
			fprintf(stderr,"[%d] local %s(%d)\n",
				node,pt->name,getpid());
#endif
			/*
			 * Local fork()'s are cheap plus we want to minimize
			 * contention for the disk controller. Do a single
			 * local partition (1st one in the list). If there are
			 * additional partitions our caller will notice the
			 * case and call us again.
			 * call FS check routine: auxdata is unused.
			 */
			st = (*checkit)(pt->name, pt->fsname, pt->auxdata, 1);
		}

                (void)close(fsreadfd);
                (void)close(fswritefd);
		exit( st );
	}

	/*
	 * Parent
	 */
	nrun++;		/* one more child to wait for */

	return (0);
}

/*
 * determine if 'name' is a valid block device.
 *
 * inputs:
 *	name	string adrs, block device name
 *
 * outputs:
 *	0 == invalid block device, else string pointer to RAW device name.
 */
char *
blockcheck(name)
	char *name;
{
	struct stat stslash, stblock, stchar;
	char *raw;
	int retried = 0;
#ifdef OSF1_ADFS
        struct devstat dstblock;
	ulong_t        mountid;
	void           *sig;
#endif
 
	hotroot = 0;
	if (stat("/", &stslash) < 0) {
		perror("/");
		printf(MSGSTR(NOSTAT, "Can't stat root\n"));
		return (0);
	}
retry:
	if (stat(name, &stblock) < 0) {
		perror(name);
		printf(MSGSTR(NOSTAT1, "Can't stat %s\n"), name);
		return (0);
	}
	if ((stblock.st_mode & S_IFMT) == S_IFBLK) {
#ifdef OSF1_ADFS
	        /*
	         * Ignore SIGILL signals for backwards compatibility 
		 * with OS's that don't understand this system call.
		 */
	        sig = signal(SIGILL, SIG_IGN);
	        if (devstat(name, &dstblock) < 0) {
		        mountid = stblock.st_rdev;
                } else {
		        mountid = dstblock.dst_mountid;
	        }
		signal(SIGILL, sig);
                if (stslash.st_dev == mountid)
#else
		if (stslash.st_dev == stblock.st_rdev)
#endif   /* OSF1_ADFS */
			hotroot++;
		raw = rawname(name);
		if (stat(raw, &stchar) < 0) {
			perror(raw);
			printf(MSGSTR(NOSTAT1, "Can't stat %s\n"), raw);
			return (name);
		}
		if ((stchar.st_mode & S_IFMT) == S_IFCHR) {
			return (raw);
		} else {
			printf(MSGSTR(NOTCHAR, "%s is not a character device\n"), raw);
			return (name);
		}
	} else if ((stblock.st_mode & S_IFMT) == S_IFCHR && !retried) {
		name = unrawname(name);
		retried++;
		goto retry;
	}
	printf(MSGSTR(NOSENSE, "Can't make sense out of name %s\n"), name);
	return (0);
}

char *
unrawname(name)
	char *name;
{
	char *dp;
	struct stat stb;

	if ((dp = rindex(name, '/')) == 0)
		return (name);
	if (stat(name, &stb) < 0)
		return (name);
	if ((stb.st_mode & S_IFMT) != S_IFCHR)
		return (name);
	if (*(dp + 1) != 'r')
		return (name);
	(void)strcpy(dp + 1, dp + 2);
	return (name);
}

char *
rawname(name)
	char *name;
{
	static char rawbuf[MAX_DISKNAME];
	char *dp;

	/* 
	 * Raw name is always one character longer... Make sure the name 
	 * doesn't overflow the buffer 
	 */ 
	if (strlen(name) + 1 >= MAX_DISKNAME) 
		return(0);
	if ((dp = rindex(name, '/')) == 0)
		return (0);
	*dp = 0;
	(void)strcpy(rawbuf, name);
	*dp = '/';
	(void)strcat(rawbuf, "/r");
	(void)strcat(rawbuf, dp + 1);
	return (rawbuf);
}

#if DEBUG
dump_disks()
{
        register struct disk *dk;
        register struct part *pt;

	for(dk=disks; dk; dk = dk->next) {
		printf("disk %s partitions:\n",dk->name);
		for(pt=dk->part; pt; pt = pt->next)
			printf("  dev %s mount %s\n",pt->name,pt->fsname);
	}
}
#endif
