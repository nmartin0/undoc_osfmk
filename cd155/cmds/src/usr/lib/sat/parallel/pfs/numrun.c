/*************************************************************************
 *
 *  Title:  numrun.c
 *
 *  Description:  Parallel File System SAT
 *
 *  Syntax:
 *    numrun.nx <pfs_file>
 *
 *    where:
 *      <pfs_file>	Pathname of PFS scratch file (no default).
 *
 * $Copyright
 * Copyright 1993 Intel Corporation
 *			INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices, trademarks, and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 *  Algorithm:
 *
 * 
 *  History:
 *       9/22/93  gregt New program, to count optimal number of nodes on
 *                      which to run the PFS SAT.
 *       9/27/93  bks   Fixed logic that decremented numion, computing
 *                      the number of unique io nodes.
 *      10/05/93  gregt Further restricted SAT to 32:1.
 *      11/01/93  killops Added "#include <mach/port.h>" because OSF1_ADFS
 *                        is defined in this R1.2.  mount_port_t, defined
 *                        in mach/port.h, is now used in the mount structure.
 *
 *************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <nx.h>
#include <pfs/pfs.h>
#include <mach/port.h>
#include <sys/mount.h>

#define MAXSTRIPE 1500
#define MOUNT_PFS 7

main(argc, argv)
		int 	 argc;
		char	*argv[];
{

int		 nnodes,			/* numnodes()							*/
		 pfsfd,				/* PFS file fd							*/
		 isnotpfs,			/* if <pfs_file> is not really on PFS	*/
		 numrun,
		 numion,
		 i;

int		 stripeiotab[MAXSTRIPE];/* For mapping stripe file to I/O node */

struct estatfs	 fs_buffer;	/* For statfs(), pfstat()				*/
struct statfs	 stripe_fs;
struct statpfs	 pfs_buffer,
				*pfs_ptr;
pathname_t		*sdir;


	/* ---------------------------------------
	** Initialize some variables.
	** ---------------------------------------
	*/
	if (mynode() != 0)
		exit(0);
	nnodes = numnodes();
	for (i=0; i<MAXSTRIPE; i++)
		stripeiotab[i] = -1;


	/* ---------------------------------------
	** Check arguments.
	** ---------------------------------------
	*/
	if (argc != 2) {
		printf("Usage:\n%s <pfsfile>\n", argv[0]);
		exit(1);
	}


	/* ---------------------------------------
	** <pfs_file>
	** ---------------------------------------
	*/
	pfsfd = open(argv[1], O_TRUNC|O_CREAT|O_RDWR|O_SYNC, 0666);
	if (pfsfd == -1) {
		nx_perror(argv[1]);
		exit(1);
	}
	
	/*
	** Check the pfs file attributes
	*/
	fstatpfs(pfsfd, &fs_buffer, &pfs_buffer, sizeof(pfs_buffer));
	pfs_ptr = malloc(pfs_buffer.p_reclen);
	fstatpfs(pfsfd, &fs_buffer, pfs_ptr, pfs_buffer.p_reclen);
	numion = pfs_ptr->p_sfactor;
	
	if (fs_buffer.f_type != MOUNT_PFS) {
		numion = 1;
	} else {
		sdir = &pfs_ptr->p_sdirs;
		for (i = 0; i < pfs_ptr->p_sfactor; i++) {
			statfs(sdir->name, &stripe_fs);
			if (stripe_fs.f_type != MOUNT_UFS) {
				printf("NOTE: Stripe directory not on UFS filesys\n");
			} else {
				if (stripeiotab[ atoi(&stripe_fs.f_mntfromname[7]) ] == -1) {
					stripeiotab[ atoi(&stripe_fs.f_mntfromname[7]) ] = i;
				} else {
					numion--;
				} 
			}
			sdir = NEXTPATH(sdir);
		}
	}


	close(pfsfd);
	i = unlink(argv[1]);
	if (i != 0) {
		nx_perror("unlink");
	}

	numrun = numion * 32;
	if (numrun > nnodes)
		numrun = nnodes;

	printf("%d\n", numrun);
	exit(0);
}

