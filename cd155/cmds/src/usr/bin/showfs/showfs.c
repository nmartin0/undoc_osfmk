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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1993  Intel Corporation.
 *
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/showfs/showfs.c,v 1.8 1994/11/19 01:39:32 mtm Exp $ 
 *
 * Display file system stripe attributes and disk space statistics.
 *
 * HISTORY:
 * $Log: showfs.c,v $
 * Revision 1.8  1994/11/19  01:39:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/10  03:25:38  brad
 * Fixed bug in allocation of statpfs buffer.
 *
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #: Erroneous extra mallocs of buffers avoided.
 *  Testing: Developer tested.
 *  Module(s): usr/bin/showfs.c
 *
 * Revision 1.6  1993/06/09  00:46:34  dbm
 * Made stripe unit size always be displayed in bytes.
 *
 * Revision 1.5  1993/06/09  00:35:44  brad
 * All numbers of blocks reported by statpfs() are now in UBSIZE units
 * (from sys/param.h), to avoid problems in cases where different stripe
 * file systems have different file system block/fragment sizes.
 *
 * Revision 1.4  1993/06/06  00:56:30  brad
 * Use big enough initial statpfs buffer to avoid an extra statpfs syscall.
 * Fix EACCESS error handling on PFS file system.
 *
 * Revision 1.3  1993/04/05  23:33:46  brad
 * Modified output so that sdirectories have more horizontal space.
 *
 * Revision 1.2  1993/04/02  23:34:23  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1  1993/03/22  23:30:28  dbm
 * Aligned some of the fields for PFS file system info.
 *
 * Revision 1.1  1993/03/04  22:40:28  dbm
 * Initial revision
 */

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/file.h>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <nx.h>


/*
 * Initial size of the buffer used to contain stripe attributes.
 * Must be >= sizeof(struct statpfs).
 */
#define	STATPFS_BUFSZ 8192

struct statpfs	*pfsbuf = NULL;
int		len_pfsbuf = STATPFS_BUFSZ;


char	*getmntpt();
int	kflag = 0; 
int	tflag = 0;
int	fstype = MOUNT_NONE;
#define	MAX_WIDTH	128
        
main(argc, argv)
	int argc;
	char **argv;
{
	extern int errno, optind;
        extern char *optarg;
	int err, ch, i, pass;
	long width, maxwidth=MAX_WIDTH, mntsize, getmntinfo();
	char *mntpt, *mktemp(), **saved_argv;
	struct stat stbuf;
	struct statfs statfsbuf, *mntbuf;
	struct ufs_args mdev;

	while ((ch = getopt(argc, argv, "kt:")) != EOF)
		switch(ch) {
			case 'k':
				kflag = 1;
				break;
                	case 't':
                       		tflag = 1;
				fstype = MOUNT_NONE;
                                if (!strcmp("ufs", optarg)) {
					fstype = MOUNT_UFS;
				} else if (!strcmp("nfs", optarg)) { 
					fstype = MOUNT_NFS;
				} else if (!strcmp("mfs", optarg)) { 
					fstype = MOUNT_MFS;
				} else if (!strcmp("pc", optarg)) { 
					fstype = MOUNT_PC;
				} else if (!strcmp("s5fs", optarg)) { 
					fstype = MOUNT_S5FS;
				} else if (!strcmp("pfs", optarg)) { 
					fstype = MOUNT_PFS;
				} else {
                                	fprintf(stderr,
                                        	"showfs: %s: unknown file system type.\n", optarg);
                                	exit(1);
                        	}
                        	break;

		default:
			fprintf(stderr,
			    	"usage: showfs [-k] [-t nfs|ufs|s5fs|pfs] [file | file_system ...]\n");
			exit(1);
		}

	argc -= optind;
	argv += optind;

	/*
	 * scan through to calculate the the maximum
	 * width.  If type specified, only use widths of that fs type.
	 */
	mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
	maxwidth = 0;
	for (i = 0; i < mntsize; i++) {
		if (!tflag || (tflag && mntbuf[i].f_type == fstype)) {
			width = strlen(mntbuf[i].f_mntonname);
			if (width > maxwidth)
				maxwidth = width;
		}
	}

	if (!*argv) {
		/*
		 * scan through to calculate the the maximum
		 * width.  If type specified, only use widths of that fs type.
		 */
		mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
		maxwidth = 0;
		for (i = 0; i < mntsize; i++) {
			if (!tflag || (tflag && mntbuf[i].f_type == fstype)) {
				width = strlen(mntbuf[i].f_mntonname);
				if (width > maxwidth)
					maxwidth = width;
			}
		}
		/*
		 * Print information for all mounted file systems, 
		 * discriminating by type if tflag specified.
		 */
		mntsize = getmntinfo(&mntbuf, MNT_WAIT);
		for (i = 0; i < mntsize; i++)
                        if (!tflag || (tflag && mntbuf[i].f_type == fstype))
                                prtstat(&mntbuf[i], maxwidth);
		if (pfsbuf)
			free(pfsbuf);
		exit(0);
	}

	pass = 0;
	maxwidth = 0;
	saved_argv = argv;

pass1:
	for (; *argv; argv++) {

		if (stat(*argv, &stbuf) < 0) {
			err = errno;
			if ((mntpt = getmntpt(*argv)) == 0) {
				if (pass != 0) {
					errno = err;
					perror(*argv);
				}
				continue;
			}
		} else if ((stbuf.st_mode & S_IFMT) == S_IFBLK) {
			if ((mntpt = getmntpt(*argv)) == 0) {
				mntpt = mktemp("/showfs.XXXXXX");
				mdev.fspec = *argv;
				if (!mkdir(mntpt,0700) &&
				    !mount(MOUNT_UFS, mntpt, M_RDONLY, &mdev) &&
				    !statfs(mntpt, &statfsbuf)) {
					if (pass == 0) {
					    width = 
						strlen(statfsbuf.f_mntonname);
					    if (width > maxwidth)
						maxwidth = width;
					} else {
					    statfsbuf.f_mntonname[0] = '\0';
					    prtstat(&statfsbuf, maxwidth);
					}
				} else if (pass != 0)
					perror(*argv);
				(void)umount(mntpt, MNT_NOFORCE);
				(void)rmdir(mntpt);
				continue;
			}
		} else
			mntpt = *argv;

		if (statfs(mntpt, &statfsbuf) < 0) {
			if (pass != 0)
				perror(mntpt);
			continue;
		}
		if (pass == 0) {
			width = strlen(statfsbuf.f_mntonname);
			if (width > maxwidth)
				maxwidth = width;
		} else 
                	prtstat(&statfsbuf, maxwidth);
	}
	if (pass == 0) {
		pass++;
		argv = saved_argv;
		goto pass1;
	}
	if (pfsbuf)
		free(pfsbuf);
	exit(0);
}

char *
getmntpt(name)
	char *name;
{
	long mntsize, i;
	struct statfs *mntbuf;

	mntsize = getmntinfo(&mntbuf, 0);
	for (i = 0; i < mntsize; i++) {
		if (!strcmp(mntbuf[i].f_mntfromname, name))
			return (mntbuf[i].f_mntonname);
	}
	return (0);
}

/*
 * Print out status about a filesystem.
 */
prtstat(sfsp, maxwidth)
	register struct statfs *sfsp;
        long maxwidth;
{
	long used, availblks, inodes, tmp_bavail;
        static int timesthrough;
        int fsys_len = strlen("Mounted on") + 1;
	int num_chars = 0;
	struct estatfs estatfsbuf;

	if (maxwidth < fsys_len)
		maxwidth = fsys_len;

        if (++timesthrough == 1) {
                printf("%-*.*s%s     avail  capacity",
                   		    maxwidth, maxwidth, "Mounted on",
                       		    kflag ? "    kbytes" : "  512-blks");

                printf("  sunit sfactor\n");
        }

	num_chars += printf("%-*.*s", maxwidth, maxwidth, sfsp->f_mntonname);

	if (sfsp->f_type == MOUNT_PFS) {
		/*
		 * Need to do a statpfs to get the correct block
		 * counts and to obtain the stripe information.
		 */
		for(;;) {
			if (pfsbuf == NULL) {
				pfsbuf = (struct statpfs *)malloc(len_pfsbuf);
				if (pfsbuf == NULL) {
					fprintf(stderr,
						"showfs: Not enough memory");
					exit(1);
				}
			}

			if (statpfs(sfsp->f_mntonname, &estatfsbuf, pfsbuf, 
			            len_pfsbuf) < 0){
				fprintf(stderr,
					"showfs: cannot statpfs mounted file system %s\n",
					sfsp->f_mntonname);
				perror((char *)NULL);
				printf("\n");
				return;
			}

			/*
			 * Exit the loop if the buffer was big enough, 
			 * otherwise allocate a bigger buffer and try again.
			 */
			if (pfsbuf->p_reclen <= len_pfsbuf)
				break;
			len_pfsbuf = pfsbuf->p_reclen;
			free(pfsbuf);
			pfsbuf = NULL;
		}
	}

	if (sfsp->f_type == MOUNT_PFS) {
		pathname_t	*sdir_path = (pathname_t *)&pfsbuf->p_sdirs;
		int		sdir_pos;
		int		sdir;
		int		total_avail;
		int		total_blks;
		esize_t		e_used, e_availblks, e_tmp_bavail, e_tmp;
		esize_t		e_total_bytes, e_zero = {0, 0};

		e_used = _esub(estatfsbuf.f_blocks, estatfsbuf.f_bfree);
		e_availblks = _eadd(estatfsbuf.f_bavail, e_used);
		e_tmp_bavail = estatfsbuf.f_bavail;
		if (_ecmp(e_tmp_bavail, e_zero) < 0) {
			e_tmp_bavail = e_zero;
	        	e_availblks = e_zero;
		}

		/*
		 * Compute total blocks.  Remember that statpfs() reports
		 * numbers of blocks in UBSIZE units.
		 */
		e_total_bytes = _emul(estatfsbuf.f_blocks, UBSIZE);
		total_blks = _ediv(e_total_bytes,  kflag ? 1024 : 512);

		/*
		 * Compute total available.
		 */
		e_tmp = (esize_t)_emul(e_tmp_bavail , UBSIZE);
		total_avail = _ediv(e_tmp, kflag ? 1024 : 512);

		num_chars += printf("%10ld%10ld", total_blks, total_avail);
		
		/*
		 * Compute capacity: (e_used / e_avail) * 100.0
		 */
		used = _ediv(e_used, 1024);
		availblks = _ediv(e_availblks, 1024); 

		num_chars += printf("%6.0f%%",
				    availblks == 0 ? 100.0 :
				    (double)used / (double)availblks * 100.0);

		/*
		 * Print out the sunit and sfactor PFS info.
		 */
		num_chars += printf("%10ld%8ld\n", pfsbuf->p_sunitsize, 
				     pfsbuf->p_sfactor);

		/*
		 * Print out the PFS sdirectory info. Remember what position
		 * on the line to start the next stripe path. 
		 */
		sdir_pos = printf("    sdirs: ");
		printf("%s\n", sdir_path->name);
		sdir_path = NEXTPATH(sdir_path);

		for (sdir = 1; sdir < pfsbuf->p_sfactor; sdir++) {
			printf("%*s%s\n", sdir_pos, " ", sdir_path->name);
			sdir_path = NEXTPATH(sdir_path);
		}

	} else {
		used = sfsp->f_blocks - sfsp->f_bfree;
		availblks = sfsp->f_bavail + used;
		tmp_bavail = sfsp->f_bavail;
		if (tmp_bavail < 0) {
			tmp_bavail = 0;
	       	 	availblks = 0;
		}
		num_chars += printf("%10ld%10ld",
				    sfsp->f_blocks * sfsp->f_fsize /
				    (kflag ? 1024 : 512),
	    	tmp_bavail * sfsp->f_fsize / (kflag ? 1024 : 512));
		num_chars += printf("%6.0f%%",
				    availblks == 0 ? 100.0 :
				    (double)used / (double)availblks * 100.0);
		printf(" \n");
	}
}
