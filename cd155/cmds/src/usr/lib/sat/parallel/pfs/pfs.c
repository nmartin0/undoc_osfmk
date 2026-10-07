/*************************************************************************
 *
 *  Title:  node.c
 *
 *  Description:  Parallel File System SAT
 *
 *  Syntax:
 *    node.nx [-u<ufs_file>] [-p<pfs_file>] [-b<blocksize>] [-f<filesize>] [-x]
 *
 *    where:
 *      <ufs_file>	Pathname of UFS scratch file (no default).
 *
 *      <pfs_file>	Pathname of PFS scratch file (no default).
 *
 *      <blocksize>	Size (in bytes) of each read/write operation, up
 *					to 1 MByte (default 64K).
 *
 *      <filesize>	Size (in bytes) of test file to create.  Will be
 *					rounded down to the nearest multiple of <blocksize>
 *					if necessary (default 32M).
 *
 *       -x			If present, performance is used as pass/fail criteria.
 *					Otherwise, this is a functional test only.
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
 *		 7/ 2/93  gregt New test.
 *       7/15/93  gregt Modified to support GByte files.
 *       7/21/93  gregt Adjusted performance pass/fail criteria.
 *       9/ 9/93  gregt Initialized perf. numbers, added check for
 *                      optimal stripefile blocksize.
 *       9/22/93  gregt Removed multiple-node UFS test to avoid scaling
 *                      problems when exceeding 32:1.
 *      10/05/93  gregt Modified pass/fail criteria for T13 norms.
 *      11/01/93  killops Added "#include <mach/port.h>" because OSF1_ADFS
 *                        is defined in this R1.2.  mount_port_t, defined
 *                        in mach/port.h, is now used in the mount structure.
 *      06/22/94  adam  Removed "#include <mach/port.h>", bug was fixed so
 *                      <sys/mount.h> no includes the Mach include file.
 *                      Customer codes shouldn't have to include Mach stuff.
 *		06/24/94  adam	Added extensive return value checking on most
 *						system calls, especially during the multi-node
 *						portions. Also added data validations on file
 *						reads.
 *
 *************************************************************************/

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <nx.h>
#include <pfs/pfs.h>
#include <sys/mount.h>

#define TIME	  0
#define MULTIUFS  0
#define MAXBLOCK  1024*1024
#define MAXSTRIPE 1500
#define DFLTBLOCK 64*1024;
#define DFLTFILE  32*1024*1024
#define MOUNT_PFS 7

/* NX msg types */
#define ERRTYPE		20
#define ERRCNTTYPE	30

/* NX msg data */
#define	QUIT		32
#define	CONTINUE	23

/*
** forward decls
*/
void	assert();

/* 
** globals 
*/

/* write/read buffers are too big to fit on main()'s stack, put em on heap */ 
int		wbuffer[MAXBLOCK/sizeof(int)],	/* For writes				*/
		rbuffer[MAXBLOCK/sizeof(int)];	/* For reads				*/

int		err;	/* errors where we couldn't cleanly exit during 	*/
				/* multi-node processing							*/

int	 	 node;	/* mynode()											*/
int		 ptype;	/* myptype()										*/

main(argc, argv)
		int 	 argc;
		char	*argv[];
{

int		 nnodes,			/* numnodes()								*/
		 ufsfd,				/* UFS file fd								*/
		 pfsfd,				/* PFS file fd								*/
		 blocksize,			/* size of each I/O, in bytes				*/
		 perfflag,			/* use performance for PASS/FAIL?			*/
		 myios,				/* w/mult. nodes, num I/Os I need to do 	*/
		 numiow,			/* for loop counter							*/
		 numior,			/* for loop counter							*/
		 i,					/* for loop counter							*/
		 multiplier,		/* for K, M, G suffix						*/
		 errflag,
		 uflag,				/* -u specified?							*/
		 pflag,				/* -p specified?							*/
		 fflag,				/* -f specified?							*/
		 xflag,				/* -x specified?							*/
		 bflag,				/* -b specified?							*/
		 isnotpfs;			/* if <pfs_file> is not really on PFS		*/

unsigned int
		 ufsfilesize,		/* size of ufs file to create			*/
		 uint;				/* scratch area							*/

esize_t	 filesize,			/* size	of pfs file to create			*/
		 esize_scr;			/* scratch area							*/

double	 ufs1w_perf,		/* Calculated performance:				*/
		 ufs1r_perf,		/*  xxx.._perf => ufs or pfs			*/
		 pfs1w_perf,		/*  ...x._perf => 1 or M(ultiple) nodes	*/
		 pfs1r_perf,		/*  ....x_perf => r(ead) or w(rite)		*/
		 ufsMw_perf,		/*										*/
		 ufsMr_perf,		/* Values are in MBytes/sec.			*/
		 pfsMw_perf,
		 pfsMr_perf;

int		 ufs_blksz,			/* UFS blocksize						*/
		 pfs_blksz;			/* PFS blocksize						*/
int		 ufs_availb,		/* UFS free blocks						*/
		 pfs_availb;		/* PFS free blocks						*/

double	 starttime,			/* Clocks for calculating elapsed time	*/
		 endtime;

int		 msgbuf[20];		/* For NX msgs	*/

char	 tmpbuf[80],		/* For sprintf(), others				*/
		 filesizestr[20],	/* ASCII filesize						*/
		 pstr[100],			/* string with -p (PFS filename)		*/
		 ustr[100],			/* string with -u (UFS filename)		*/
		 bstr[20],			/* string with -b (blocksize)			*/
		 fstr[20];			/* string with -f (filesize)			*/


int		 pattern,						/* Pattern for writes		*/
		 stripeiotab[MAXSTRIPE],/* For mapping stripe file to I/O node	*/
		 sfactor;				/* PFS file stripe factor 				*/

#if TIME
char	 timebuf[20];		/* Format wall clock time				*/
time_t	 tp;				/* For wall clock time					*/
struct tm *stm;
#endif

struct estatfs	 fs_buffer;	/* For statfs(), pfstat()				*/
struct statfs	 stripe_fs;
struct statpfs	 pfs_buffer,
				*pfs_ptr;
pathname_t		*sdir;

extern char	*optarg;		/* For getopt()							*/
extern int	 optind;
extern int	 opterr;
extern char	 optopt;


	/* ---------------------------------------
	** Initialize some variables.
	** ---------------------------------------
	*/
	node = mynode();
	nnodes = numnodes();
	ptype = myptype();
	perfflag = 1;
	uflag = 0;
	pflag = 0;
	bflag = 0;
	fflag = 0;
	xflag = 0;
	ustr[0] = 0;
	pstr[0] = 0;
	bstr[0] = 0;
	fstr[0] = 0;
	isnotpfs = 0;
	sfactor = 1;
	for (i=0; i<MAXSTRIPE; i++)
		stripeiotab[i] = -1;
	pfs1w_perf = 10;			/* Initialize performance to PASSING values */
	pfs1r_perf = 10;
	pfsMw_perf = 10;
	pfsMr_perf = 10;

#if TIME
	tp = time((time_t *) NULL);
	stm = localtime(&tp);
	strftime(timebuf, 80, "%H:%M:%S", stm);
#endif

	/* ---------------------------------------
	** Check arguments.
	** ---------------------------------------
	*/
	optarg = NULL;
	opterr = 0;
	errflag = 0;
	while (!errflag && (i = getopt(argc, argv, "u:p:b:f:x")) != -1) {
		switch(i) {
			case 'u':
				if (uflag == 1)
					errflag++;
				uflag = 1;
				strncpy(ustr, optarg, 100);
				break;

			case 'p':
				if (pflag == 1)
					errflag++;
				pflag = 1;
				strncpy(pstr, optarg, 100);
				break;

			case 'b':
				if (bflag == 1)
					errflag++;
				bflag = 1;
				strncpy(bstr, optarg, 20);
				break;

			case 'f':
				if (fflag == 1)
					errflag++;
				fflag = 1;
				strncpy(fstr, optarg, 20);
				break;

			case 'x':
				if (xflag == 1)
					errflag++;
				xflag = 1;
				break;

			default:
				errflag++;
		}
	}

	if ((errflag != 0) || (optind != argc) || (argc < 2)) {
		if (node == 0) {
			printf("Usage:\n%s [-u <ufsfile>] [-p <pfsfile>] [-b <blocksize>] [-f <filesize>] [-x]\n",
				argv[0]);
		}
		exit(1);
	}

	if (node == 0) {
		printf("*** Start Parallel File System SAT ***\n");
	}

	if ((uflag == 0) && (pflag == 0)) {
		if (node == 0) {
			printf("ERROR: Must specify at least one ufs or pfs file\n");
			printf("*** PFS SAT Terminated ***\n");
		}
		exit(1);
	}


	/* ---------------------------------------
	** <blocksize>
	** ---------------------------------------
	*/
	if (bflag) {
		strncpy(tmpbuf, bstr, 15);		/* Handle mMkK suffix */
		i = strlen(tmpbuf);
		multiplier = 1;
		switch (tmpbuf[i-1]) {
			case 'm':
			case 'M':
				multiplier = 1024;
			case 'k':
			case 'K':
				multiplier = 1024 * multiplier;
	
				tmpbuf[i-1] = 0;
	
			default:
				break;
		}
	
		if ((strspn(tmpbuf, "0123456789") != strlen(tmpbuf) ) ||
	    	(strlen(tmpbuf) == 0)) {
			if (node == 0) {			/* Found illegal character */
				printf("ERROR: Illegal blocksize: %s\n", bstr);
				printf("*** PFS SAT Terminated ***\n");
			}
			exit(1);
		}
	
		blocksize = atoi(tmpbuf) * multiplier;
		if ((blocksize <= 0) || (blocksize > MAXBLOCK)) {
			if (node == 0) {
				printf("ERROR: Blocksize %s invalid:  0 < blocksize < %d\n",
					bstr, MAXBLOCK+1);
				printf("*** PFS SAT Terminated ***\n");
			}
			exit(1);
		}
	}
	else								/* Default */
		blocksize = DFLTBLOCK;


	/* ---------------------------------------
	** <filesize>
	** ---------------------------------------
	*/
	if (fflag) {
		strncpy(tmpbuf, fstr, 15);		/* Handle gGmMkK suffix */
		i = strlen(tmpbuf);
		multiplier = 1;
		switch (tmpbuf[i-1]) {
			case 'g':
			case 'G':
				multiplier = 1024;
			case 'm':
			case 'M':
				multiplier = 1024 * multiplier;
			case 'k':
			case 'K':
				multiplier = 1024 * multiplier;
	
				tmpbuf[i-1] = 0;
	
			default:
				break;
		}
	
		if ((strspn(tmpbuf, "0123456789") != strlen(tmpbuf) ) ||
	    	(strlen(tmpbuf) == 0)) {
			if (node == 0) {			/* Found illegal character */
				printf("ERROR: Illegal filesize: %s\n", fstr);
				printf("*** PFS SAT Terminated ***\n");
			}
			exit(1);
		}
	
		filesize = stoe(tmpbuf);
		filesize = emul(filesize, multiplier);
		esize_scr.shigh = 0;
		esize_scr.slow = blocksize;
		if (ecmp(esize_scr, filesize) > 0) {
			if (node == 0) {
				printf("ERROR: File size %s invalid:  blocksize <= filesize\n",
					fstr, MAXBLOCK+1);
				printf("*** PFS SAT Terminated ***\n");
			}
			exit(1);
		}
	
		filesize.slow = ediv(filesize, blocksize);
		filesize.shigh = 0;
		filesize = emul(filesize, blocksize);	/* Now a multiple of blocksize */
	
		esize_scr.slow = 32*1024*1024;
		if ((node == 0) && (ecmp(filesize, esize_scr) < 0)) {
			printf("NOTE: Filesize too small for meaningful performance results\n");
			perfflag = 0;
		}

		if ((filesize.shigh != 0) || ((filesize.slow & 0x80000000) == 0x80000000)) {
			ufsfilesize = 32*1024*1024;
			if (node == 0)
				printf("NOTE: File size greater than UFS max. - will use %d for UFS filesize\n", ufsfilesize);
		}
		else
			ufsfilesize = filesize.slow;

		if ((filesize.shigh == 0) && (filesize.slow < (blocksize * nnodes))) {
			filesize.slow = blocksize * nnodes;
			if (node == 0)
				printf("NOTE: File too small for PFS - will use %d for PFS filesize\n", filesize.slow);
		}
	}
	else {										/* Default */
		ufsfilesize = DFLTFILE;
		filesize.slow = ufsfilesize;
		filesize.shigh = 0;
		if (filesize.slow < (blocksize * nnodes)) {
			filesize.slow = blocksize * nnodes;
			if (node == 0)
				printf("NOTE: Default file too small for PFS - will use %d for PFS filesize\n", filesize.slow);
		}
	}


	/* ---------------------------------------
	** <ufs_file>, <pfs_file>
	** ---------------------------------------
	*/
	if (node == 0) {
		if (uflag) {
			/*
			* open <ufs_file>
			*/
			assert((ufsfd = open(ustr, O_TRUNC|O_CREAT|O_RDWR|O_SYNC, 0666)) > -1, TRUE, "open(ustr)");
			if (ufsfd <= -1) {
				printf("*** PFS SAT Terminated ***\n");
				msgbuf[0] = QUIT;
				if (nnodes > 1)
					csend(ERRTYPE, msgbuf, sizeof(int), -1, 0);	/* Tell other nodes */
				exit(1);
			}
	
			/*
			** Check the ufs file attributes
			*/
			fstatpfs(ufsfd, &fs_buffer, &pfs_buffer, sizeof(pfs_buffer));
			if (fs_buffer.f_type != MOUNT_UFS) {
				printf("NOTE: %s is not a UFS file\n", ustr);
				perfflag = 0;
			}
			ufs_blksz = fs_buffer.f_fsize;
			ufs_availb = fs_buffer.f_bavail.slow;
			if ((fs_buffer.f_bavail.shigh == 0) &&
		    	(ufs_availb < ufsfilesize/ufs_blksz)) {
				printf("ERROR: Insufficient free disk space to create %s\n", ustr);
				printf("*** PFS SAT Terminated ***\n");
				msgbuf[0] = QUIT;
				if (nnodes > 1)
					csend(ERRTYPE, msgbuf, sizeof(int), -1, 0);	/* Tell other nodes */
				exit(1);
			}
		}
	
		/*
		* open <pfs_file>
		*/
		if (pflag) {
			assert((pfsfd = open(pstr, O_TRUNC|O_CREAT|O_RDWR|O_SYNC, 0666)) > -1, TRUE, "open(pstr)");
			if (pfsfd == -1) {
				printf("*** PFS SAT Terminated ***\n");
				msgbuf[0] = QUIT;
				if (nnodes > 1)
					csend(ERRTYPE, msgbuf, sizeof(int), -1, 0);	/* Tell other nodes */
				exit(1);
			}
	
			/*
			** Check the pfs file attributes
			*/
			fstatpfs(pfsfd, &fs_buffer, &pfs_buffer, sizeof(pfs_buffer));
			pfs_ptr = malloc(pfs_buffer.p_reclen);
			fstatpfs(pfsfd, &fs_buffer, pfs_ptr, pfs_buffer.p_reclen);
			sfactor = pfs_ptr->p_sfactor;
	
			if (fs_buffer.f_type != MOUNT_PFS) {
				printf("NOTE: %s is not a PFS file\n", pstr);
				perfflag = 0;
				isnotpfs = 1;
			}
			if (fs_buffer.f_bsize == -1) {
				printf("NOTE: Stripe directories have differing blocksizes\n");
				perfflag = 0;
			} else if (fs_buffer.f_bsize != 65536) {
				printf("NOTE: Stripe directories blocksize not set for optimal SAT performance\n");
				perfflag = 0;
			}
			if (pfs_ptr->p_sunitsize != 65536) {
				printf("NOTE: Stripe unit size is not set for optimal SAT performance\n");
				perfflag = 0;
			}

			pfs_blksz = fs_buffer.f_fsize;
			pfs_availb = fs_buffer.f_bavail.slow;
			if ((fs_buffer.f_bavail.shigh == 0) &&
		    	(pfs_availb < ediv(filesize, pfs_blksz) )) {
				printf("ERROR: Insufficient free disk space to create %s\n", pstr);
				printf("*** PFS SAT Terminated ***\n");
				msgbuf[0] = QUIT;
				if (nnodes > 1)
					csend(ERRTYPE, msgbuf, sizeof(int), -1, 0);	/* Tell other nodes */
				exit(1);
			}

			errflag = 0;					/* Print message only once */
			sdir = &pfs_ptr->p_sdirs;
			for (i = 0; i < pfs_ptr->p_sfactor; i++) {
				statfs(sdir->name, &stripe_fs);
				if (stripe_fs.f_type != MOUNT_UFS) {
					if ((errflag != 10) && (errflag != 110)) {
						printf("NOTE: Stripe directory not on UFS file system\n", ustr);
						perfflag = 0;
						errflag = errflag + 10;
					}
				} else {
					if (stripeiotab[ atoi(&stripe_fs.f_mntfromname[7]) ] == -1) {
						stripeiotab[ atoi(&stripe_fs.f_mntfromname[7]) ] = i;
					} else {
						if ((errflag != 100) && (errflag != 110)) {
							printf("NOTE: Multiple stripe directories on 1 I/O node\n");
							perfflag = 0;
							errflag = errflag + 100;
						}
					}
				}
				sdir = NEXTPATH(sdir);
			}
		}
	}
	

	/*
	** Let the other nodes know there were no errors in the files specified.
	** A message of 0 was sent above if node 0 detected a fatal error.
	*/
	if (nnodes > 1) {
		if (node == 0) {
			msgbuf[0] = CONTINUE;
			csend(ERRTYPE, msgbuf, sizeof(int), -1, 0);/* Tell other nodes */
		} else {
			crecv(ERRTYPE, msgbuf, sizeof(int));
			if (msgbuf[0] == QUIT) exit(1);
		}
	}


	/* ---------------------------------------
	** performance flag.
	** ---------------------------------------
	*/
	if (sfactor > 4)
		sfactor = 4;
	if ((node == 0) && (blocksize != (sfactor * 65536)) && (pflag)) {
		printf("NOTE: Blocksize not optimal for PFS SAT performance\n");
		perfflag = 0;
	}

	if (xflag) {
		if ((node == 0) && (perfflag == 0)) {
			printf("-x flag IGNORED\n");
		}
	} else {
		perfflag = 0;
	}


	/* ---------------------------------------
	** Print out parameters for this run.
	** ---------------------------------------
	*/
	if (node == 0) {
		printf("\n");

		if (uflag)
			printf(" UFS file = %s\n", ustr);

		if (pflag) {
			printf(" PFS file = %s\n", pstr);
			if (isnotpfs == 0) {
				printf("  stripe unit = %d\n", pfs_ptr->p_sunitsize);
				printf("  stripe factor = %d\n", pfs_ptr->p_sfactor);
				if (fs_buffer.f_bsize != -1) 
					printf("  stripedirs filesystem blocksize = %d\n", fs_buffer.f_bsize);

				sdir = &pfs_ptr->p_sdirs;
				for (i = 0; i < pfs_ptr->p_sfactor; i++) {
					if (i == 0)
						printf("  stripedirs: ");
					else
						printf("              ");
					printf("%s", sdir->name);
					assert(statfs(sdir->name, &stripe_fs), 0, "statfs");
					if (stripe_fs.f_type != MOUNT_UFS) {
						printf("       not a UFS file\n", ustr);
						perfflag = 0;
					} else {
						printf("  from %s", &stripe_fs.f_mntfromname);
						if (stripeiotab[atoi(&stripe_fs.f_mntfromname[7])] == i) {
							printf("\n");
						} else {
							printf(" same I/O node as stripe %d\n",stripeiotab[atoi(&stripe_fs.f_mntfromname[7])]);
							perfflag = 0;
						}
					}
					sdir = NEXTPATH(sdir);
				}
			}
		}


		printf(" I/O block size = %d\n", blocksize);

		etos(filesize, filesizestr);
		if ((filesize.shigh != 0) ||
		    ((filesize.slow & 0x80000000) == 0x80000000) ||
			(filesize.slow != ufsfilesize)) {
			if (uflag)
				printf(" UFS file size  = %d\n", ufsfilesize);
			if (pflag)
				printf(" PFS file size  = %s\n", filesizestr);
		}
		else
			printf(" File size  = %s\n", filesizestr);

		printf(" Number of nodes = %d\n", nnodes);

		if (perfflag == 0)
			printf(" Performance checking disabled\n");
		else
			printf(" Performance checking enabled\n");
	}

	gsync();

	/*
	** From here on just increment err on error, and print message, but don't
	** exit. The err counters of all nodes will be summed at the finale.
	*/
	err = 0;

	/* ---------------------------------------
	** Initialize write buffer.
	** ---------------------------------------
	*/

	pattern = node + 0xa5;
	for (i = 0; i < blocksize/sizeof(pattern); ++i) {
		wbuffer[i] = pattern;
	}

	/* ---------------------------------------
	** Single compute node tests.
	** ---------------------------------------
	*/
	if (node == 0)
		printf("\nSingle compute node:\n");

	if (node == 0) {
		if (uflag) {
			/*
			** Write UFS file.
			*/
			starttime = dclock();
			for (numiow = 0; numiow < ufsfilesize/blocksize; numiow++) {
				assert(_cwrite(ufsfd, (char *)wbuffer, blocksize), blocksize, "cwrite(ufsfd)");
			}
			endtime = dclock();
			close(ufsfd);
			ufs1w_perf = ufsfilesize/(1024*1024*(endtime - starttime));
			printf(" UFS: wrote %d bytes in %03.3f seconds (%03.3f MB/sec.)\n",
				ufsfilesize, endtime - starttime, ufs1w_perf);


			/*
			** Read UFS file.
			*/
			assert((ufsfd = open(ustr, O_RDWR|O_SYNC, 0666)) > -1, TRUE, "open(ustr)");

			starttime = dclock();
			for (numior = 0; numior < numiow; numior++) {
				assert(_cread(ufsfd, (char *)rbuffer, blocksize), blocksize, "cread(ufsfd)");
			}
			endtime = dclock();

			/* check the data */

			assert(lseek(ufsfd, 0, SEEK_SET), 0, "lseek(ufsfd)");
			for (numior = 0; numior < numiow; numior++) {
				initbuf(rbuffer, ~pattern, blocksize/sizeof(pattern));
				assert(_cread(ufsfd, (char *)rbuffer, blocksize), blocksize, "cread(ufsfd)");
				assert(chkbuf(rbuffer, pattern, blocksize/sizeof(pattern)), 0, "UFS read: data mismatch");
			}
			
			ufs1r_perf = ufsfilesize/(1024*1024*(endtime - starttime));
			printf(" UFS: read  %d bytes in %03.3f seconds (%03.3f MB/sec.)\n",
				ufsfilesize, endtime - starttime, ufs1r_perf);

			close(ufsfd);
		}

		if (pflag) {
			/*
			** Write PFS file.
			*/
			starttime = dclock();
			for (numiow = 0; numiow < ediv(filesize, blocksize); numiow++) {
				assert(_cwrite(pfsfd, (char *)wbuffer, blocksize), blocksize, "cwrite(pfsfd)");
			}
			endtime = dclock();
			if (filesize.shigh == 0) 
				uint = 4;
			else 
				uint = filesize.shigh * 4;
			pfs1w_perf = ediv(filesize, uint)/((1024*1024*(endtime - starttime)/uint) );
			printf(" PFS: wrote %s bytes in %03.3f seconds (%03.3f MB/sec.)\n",
				filesizestr, endtime - starttime, pfs1w_perf);


			/*
			** Read PFS file.
			*/
			assert(lseek(pfsfd, 0, SEEK_SET), 0, "lseek(pfsfd)");

			starttime = dclock();
			for (numior = 0; numior < numiow; numior++) {
				assert(_cread(pfsfd, (char *)rbuffer, blocksize), blocksize, "cread(pfsfd)");
			}
			endtime = dclock();

			/* check the data */

			assert(lseek(pfsfd, 0, SEEK_SET), 0, "lseek(pfsfd)");
			for (numior = 0; numior < numiow; numior++) {
				initbuf(rbuffer, ~pattern, blocksize/sizeof(pattern));
				assert(_cread(pfsfd, (char *)rbuffer, blocksize), blocksize, "cread(pfsfd)");
				assert(chkbuf(rbuffer, pattern, blocksize/sizeof(pattern)), 0, "PFS read: data mismatch");
			}
			
			if (filesize.shigh == 0) 
				uint = 4;
			else 
				uint = filesize.shigh * 4;
			pfs1r_perf = ediv(filesize, uint)/((1024*1024*(endtime - starttime)/uint) );
			printf(" PFS: read  %s bytes in %03.3f seconds (%03.3f MB/sec.)\n",
				filesizestr, endtime - starttime, pfs1r_perf);

			close(pfsfd);
		}
	}
	gsync();


	/* ---------------------------------------
	** Multiple compute node tests.
	** ---------------------------------------
	*/
	if (nnodes > 1) {
#if MULTIUFS
		if (node == 0)
			printf("\nMultiple (%d) compute nodes:\n", nnodes);

		if (uflag) {
			assert((ufsfd = _gopen(ustr, O_TRUNC|O_RDWR|O_SYNC, M_SYNC, 0666)) > -1, TRUE, "gopen(ustr)");

			myios = ufsfilesize/blocksize/nnodes;

			/*
			** Write UFS file.
			*/
			gsync();
			starttime = dclock();

			for (numiow = 0; numiow < myios; numiow++) {
				assert(_cwrite(ufsfd, (char *)wbuffer, blocksize), blocksize, "cwrite(ufsfd)");
			}

			if ((myios*blocksize*nnodes + ((node + 1) * blocksize)) <= ufsfilesize)
				assert(_cwrite(ufsfd, (char *)wbuffer, blocksize), blocksize, "cwrite(ufsfd)");
			else
				assert(_cwrite(ufsfd, (char *)wbuffer, 0), 0, "cwrite(ufsfd)");

			gsync();
			endtime = dclock();

			ufsMw_perf = ufsfilesize/(1024*1024*(endtime - starttime));
			if (node == 0)
				printf(" UFS: wrote %d bytes in %03.3f seconds (%03.3f MB/sec.)\n",
					ufsfilesize, endtime - starttime, ufsMw_perf);

			assert(close(ufsfd), 0, "close(ufsfd)");

			/*
			** Read UFS file.
			*/
			assert((ufsfd = _gopen(ustr, O_RDWR, M_SYNC, 0666)) > -1, TRUE, "gopen(ustr)");

			starttime = dclock();
			for (numior = 0; numior < myios; numior++) {
				assert(_cread(ufsfd, (char *)rbuffer, blocksize), blocksize, "cread(ufsfd)");
			}
			assert(_setiomode(ufsfd, M_SYNC), 0, "setiomode(ufsfd)");
			if ((myios*blocksize*nnodes + ((node + 1) * blocksize)) <= ufsfilesize)
				assert(_cread(ufsfd, (char *)rbuffer, blocksize), blocksize, "cread(ufsfd)");
			else
				assert(_cread(ufsfd, (char *)rbuffer, 0), 0, "cread(ufsfd)");

			gsync();
			assert(close(ufsfd), 0, "close(ufsfd)");
			endtime = dclock();

			/* check the data */

			assert((ufsfd = _gopen(ustr, O_RDWR, M_SYNC, 0666)) > -1, TRUE, "gopen(ustr)");
			for (numior = 0; numior < numiow; numior++) {
				initbuf(rbuffer, ~pattern, blocksize/sizeof(pattern));
				assert(_cread(ufsfd, (char *)rbuffer, blocksize), blocksize, "cread(ufsfd)");
				assert(chkbuf(rbuffer, pattern, blocksize/sizeof(pattern)), 0, "UFS read: data mismatch");
			}
			if ((myios*blocksize*nnodes + ((node + 1) * blocksize)) <= ufsfilesize) {
				initbuf(rbuffer, ~pattern, blocksize/sizeof(pattern));
				assert(_cread(ufsfd, (char *)rbuffer, blocksize), blocksize, "cread(ufsfd)");
				assert(chkbuf(rbuffer, pattern, blocksize/sizeof(pattern)), 0, "UFS read: data mismatch");
			} else {
				assert(_cread(ufsfd, (char *)rbuffer, 0), 0, "cread(ufsfd)");
			}

			gsync();
			assert(close(ufsfd), 0, "close(ufsfd)");
			
			ufs1r_perf = ufsfilesize/(1024*1024*(endtime - starttime));
			ufsMr_perf = ufsfilesize/(1024*1024*(endtime - starttime));
			if (node == 0)
				printf(" UFS: read  %d bytes in %03.3f seconds (%03.3f MB/sec.)\n",
			ufsfilesize, endtime - starttime, ufsMr_perf);

		}
#endif

		if (pflag) {
			if (node == 0)
				printf("\nMultiple (%d) compute nodes:\n", nnodes);
			assert((pfsfd = _gopen(pstr, O_TRUNC|O_RDWR|O_SYNC, M_SYNC, 0666)) > -1, TRUE, "gopen(pstr)");

			myios = ediv(filesize, blocksize*nnodes);
			esize_scr.slow = myios * nnodes + node + 1;
			esize_scr.shigh = 0;
			esize_scr = emul(esize_scr, blocksize);

			/*
			** Write PFS file.
			*/
			gsync();
			starttime = dclock();

			for (numiow = 0; numiow < myios; numiow++) {
				assert(_cwrite(pfsfd, (char *)wbuffer, blocksize), blocksize, "cwrite(pfsfd)");
			}
			if (ecmp(esize_scr, filesize) <= 0)
				assert(_cwrite(pfsfd, (char *)wbuffer, blocksize), blocksize, "cwrite(pfsfd)");
			else
				assert(_cwrite(pfsfd, (char *)wbuffer, 0), 0, "cwrite(pfsfd)");

			gsync();
			endtime = dclock();

			assert(close(pfsfd), 0, "close(pfsfd)");

			if (filesize.shigh == 0) 
				uint = 4;
			else 
				uint = filesize.shigh * 4;
			pfsMw_perf = ediv(filesize, uint)/((1024*1024*(endtime - starttime)/uint) );
			if (node == 0)
				printf(" PFS: wrote %s bytes in %03.3f seconds (%03.3f MB/sec.)\n",
					filesizestr, endtime - starttime, pfsMw_perf);


			/*
			** Read PFS file.
			*/
			assert((pfsfd = _gopen(pstr, O_RDWR, M_RECORD, 0666)) > -1, TRUE, "gopen(pstr)");

			starttime = dclock();

			for (numior = 0; numior < myios; numior++) {
				assert(_cread(pfsfd, (char *)rbuffer, blocksize), blocksize, "cread(pfsfd)");
			}
			assert(_setiomode(pfsfd, M_SYNC), 0, "setiomode(pfsfd)");
			if (ecmp(esize_scr, filesize) <= 0)
				assert(_cread(pfsfd, (char *)rbuffer, blocksize), blocksize, "cread(pfsfd)");
			else
				assert(_cread(pfsfd, (char *)rbuffer, 0), 0, "cread(pfsfd)");

			gsync();
			endtime = dclock();

			assert(close(pfsfd), 0, "close(pfsfd)");
			
			/* check the data */

			assert((pfsfd = _gopen(pstr, O_RDWR, M_RECORD, 0666)) > -1, TRUE, "gopen(pstr)");
			for (numior = 0; numior < numiow; numior++) {
				initbuf(rbuffer, ~pattern, blocksize/sizeof(pattern));
				assert(_cread(pfsfd, (char *)rbuffer, blocksize), blocksize, "cread(pfsfd)");
				assert(chkbuf(rbuffer, pattern, blocksize/sizeof(pattern)), 0, "PFS read: data mismatch");
			}
			assert(_setiomode(pfsfd, M_SYNC), 0, "setiomode(pfsfd)");
			if (ecmp(esize_scr, filesize) <= 0) {
				initbuf(rbuffer, ~pattern, blocksize/sizeof(pattern));
				assert(_cread(pfsfd, (char *)rbuffer, blocksize), blocksize, "cread(pfsfd)");
				assert(chkbuf(rbuffer, pattern, blocksize/sizeof(pattern)), 0, "PFS read: data mismatch");
			} else {
				assert(_cread(pfsfd, (char *)rbuffer, 0), 0, "cread(pfsfd)");
			}

			gsync();

			assert(close(pfsfd), 0, "close(pfsfd)");

			ufs1r_perf = ufsfilesize/(1024*1024*(endtime - starttime));
			ufsMr_perf = ufsfilesize/(1024*1024*(endtime - starttime));
			if (filesize.shigh == 0) 
				uint = 4;
			else 
				uint = filesize.shigh * 4;
			pfsMr_perf = ediv(filesize, uint)/((1024*1024*(endtime - starttime)/uint) );
			if (node == 0)
				printf(" PFS: read  %s bytes in %03.3f seconds (%03.3f MB/sec.)\n",
					filesizestr, endtime - starttime, pfsMr_perf);


		}
	} /* nnodes > 1 */

	/* ---------------------------------------
	** Unlink files.
	** ---------------------------------------
	*/
	if (node == 0) {
		if (uflag) {
			assert(unlink(ustr), 0, "unlink(ustr)");
		}
		if (pflag) {
			assert(unlink(pstr), 0, "unlink(pstr)");
		}
	}

	/* ---------------------------------------
	** Collect error counts from other nodes
	** ---------------------------------------
	*/
	if (nnodes > 1) {
		if (node == 0) {
			for (i = 0; i < nnodes -1; ++i) {
				crecv(ERRCNTTYPE, msgbuf, sizeof(err));
				err += msgbuf[0];
			}
		} else {
			csend(ERRCNTTYPE, &err, sizeof(err), 0, ptype);
		}
	}

	/* ---------------------------------------
	** Print results.
	** ---------------------------------------
	*/
#if TIME
	tp = time((time_t *) NULL);
	stm = localtime(&tp);
	strftime(timebuf, 80, "%H:%M:%S", stm);
#endif

	if (err) {
		if (node == 0) {
			printf("I/O errors occurred sometime during processing\n");
			printf("*** Parallel File System SAT FAILED ***\n");
		}
		exit(1);
	}

	if (node == 0) {
		printf("\n");
		if (perfflag) {
			if (pfs1w_perf < sfactor * 1) {
				printf("Single node PFS write performance less than expected\n");
				perfflag = 0;
			}
			if (pfs1r_perf < sfactor * 1) {
				printf("Single node PFS read performance less than expected\n");
				perfflag = 0;
			}
			if (pfsMw_perf < sfactor * 1) {
				printf("Multiple node PFS write performance less than expected\n");
				perfflag = 0;
			}
			if (pfsMr_perf < sfactor * 1) {
				printf("Multiple node PFS read performance less than expected\n");
				perfflag = 0;
			}
			if (perfflag == 0) {
				printf("*** Parallel File System SAT FAILED ***\n");
				exit(2);
			}
		}
		printf("*** Parallel File System SAT PASSED ***\n");
	}
	
	exit(0);
}


initbuf(buf, pattern, nitems)
int	*buf;
int	pattern;
int	nitems;
/*
**	initbuf
**
**	Initialize a read buffer prior to a read. Just set a few entries to
** 	the pattern for performance reasons.
*/	
{
	buf[0] = pattern;
	buf[nitems -1] = pattern;

	return(0);
}


chkbuf(buf, pattern, nitems)
int	*buf;
int	pattern;
int	nitems;
/*
**	chkbuf
**
**	Verify the contents of an integer buffer array. See that each entry
**	contains the pattern.
*/
{
	int	i;
	int	errs = 0;

	for (i = 0; i < nitems; i++) {
		if (buf[i] != pattern) errs++;
	}

	return(errs);
}

void
assert(got,exp,str) 
int		got;
int		exp;
char	*str;
/*
**	assert
**
**	Test an assertion. Increment a global error counter if expected and
**	actual integer values mismatch. Print an error to stderr also. The
**	expected and actual values are assumed to be function return values
**	but aren't limited to this, although if the actual value is -1 it
** 	is assumed the global value errno is set because of a failing system
**	or library call just prior to calling this routine.
*/
{

	if (got != exp) {
		++err;
		if (got == -1) {
			nx_perror(str);
		} else {
			fprintf(stderr, "(node %d, ptype %d) %s\n", node, ptype, str);
		}
	}
}
