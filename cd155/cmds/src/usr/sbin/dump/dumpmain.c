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
 * $Log: dumpmain.c,v $
 * Revision 1.8  1994/11/19  03:05:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/09/27  23:37:15  shane
 *  Reviewer: None
 *  Risk:Low
 *  Benefit or PTS #:11082/9522
 *  Testing:Running rdump to sun or another paragon
 *  Module(s):dumpmain.c
 *
 * Revision 1.6  1994/07/19  02:06:47  dbm
 * Fixed incorrect code for variable sized superblocks.  The read of the
 * superblock was not happening.
 *
 *  Reviewer:None.
 *  Risk:Low
 *  Benefit or PTS #:10137
 *  Testing: Specific test case.
 *  Module(s):
 * 	usr/sbin/dumpmain.c
 *
 * Revision 1.5  1994/06/29  00:12:46  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.4  1994/01/19  20:24:02  shala
 *  Reviewer: NONE
 *  Risk: LOW
 *  Benefit or PTS #: Bug #7168
 *  Testing: Build a new dump and tested the default device.
 *  Module(s): src/usr/sbin/dump/dumpmain.c
 *
 * Fixed the dump to check for existence of the default device and
 * make sure it is valid device and not a regular file.
 *
 * Revision 1.3  1993/04/06  22:26:28  shala
 * Add defaults support for Paragon tape.
 *
 * Revision 1.2  1992/10/12  21:46:07  shala
 * New version to understand maj, min and node numbers.
 *
 * Revision 2.8  90/10/07  22:08:04  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:09:16  gm]
 * 
 * Revision 2.7  90/08/25  12:28:43  devrcs
 * 	allow bsd style key arguments;
 * 	[90/08/16  09:36:08  nm]
 * 
 * 	remove #ifndef KJI around setlocale();
 * 	[90/08/09  09:32:59  nm]
 * 
 * 	insert rcsid string;
 * 	delete old sccs id string;
 * 	[90/08/09  03:55:42  nm]
 * 
 * Revision 2.6  90/07/27  11:18:50  devrcs
 * 	remote dump devices are treated as regular files.
 * 	#else   REMOTE
 * 	 	medium_flag = REGULAR_FILE;
 * 	[90/06/22  06:36:42  nm]
 * 
 * 	delete chkopr()
 * 	[90/06/13  09:01:10  nm]
 * 
 * 	integration to osc.12
 * 	[90/05/31  04:37:54  nm]
 * 
 * 	147c144
 * 	< 	bzero(u_spcl, sizeof(u_spcl));
 * 	---
 * 	> 	bzero((char *)&u_spcl, sizeof(u_spcl));
 * 	[90/05/08  10:19:23  nm]
 * 
 * Revision 2.4  90/04/14  00:12:47  devrcs
 * 	integration of Jim's code
 * 	[90/04/10  08:10:42  nm]
 * 
 * 	Merged AIX3.1 and BSD4.3 to one source.
 * 	Made major changes fixing that code up. (Inserted comments too!)
 * 	Merged (BSD4.3 --> CMU) changes with what resulted.
 * 	Disabled edump (cannot quite figure out what it wants to do,
 * 		also not supported by rmt).
 * 	[90/01/11            walkerj]
 * 
 * Revision 2.3  90/01/02  18:41:32  gm
 * 	Fixes for first snapshot.
 * 	[90/01/02            gm]
 * 
 * Revision 2.2  89/12/26  08:57:07  gm
 * 	Current version from CMU.
 * 	[89/12/21            gm]
 * 
 * 	Move 'DUMP IS DONE' message and updating dumpdates to after
 * 	the flushing of pending data.
 * 	[89/08/16  13:20:35  ern]
 * 
 * 	Add B switch for blockmode (no record gaps) type device.
 * 	Modified printout for the blockmode to indicate blocks instead of feet.
 * 	Change accounting for this mode to use 1K blocks instead of .1 inch in
 * 	counters.
 * 	[89/07/31  00:27:38  ern]
 * 
 * 	add support for edump.
 * 	improve support for rdump.
 * 	[89/06/12  22:30:49  ern]
 * 
 * $EndLog$
 */
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: dumpmain.c,v $ $Revision: 1.8 $ (OSF) $Date: 1994/11/19 03:05:35 $";
#endif

/*
 * This module contains IBM CONFIDENTIAL code. -- (IBM Confidential Restricted
 * when combined with the aggregated modules for this product) OBJECT CODE ONLY
 * SOURCE MATERIALS (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * Copyright (c) 1980 Regents of the University of California. All
 * rights reserved.  The Berkeley software License Agreement specifies the
 * terms and conditions for redistribution.
 */

/*
 *	Possible enhancements:
 *
 *	1) To reduce the number of processes (currently one per output
 *	volume plus one, which could get quite numerous when using
 *	floppies for output), combine all the global variables into one
 *	structure and for each tape, don't fork, rather recurse, with
 *	the first task being to malloc a new global variable structure,
 *	copy the old values into it, and proceed from there.
 *
 *	2) To reduce disk accesses (inodes in particular), buffer all the
 *	inodes.  This should be more practical now that large address spaces
 *	are available.  To keep the space required from getting out of
 *	hand when forks are done, save them in a shared memory segment.
 */

#include	"dump.h"
#include	<sys/stat.h>
#include	<errno.h>
#include	<sys/ioctl.h>
#include 	<sys/disklabel.h>

static int	bsd_style();
/*
 * We catch these signals
 */

static void		sighup();
static void		sigtrap();
static void		sigfpe();
static void		sigbus();
static void		sigalrm();
static void		sigsegv();
static void		sigterm();
static void		sigintr();

static int		bmapest();
static void		getdevinfo();

static void		handle_signal();
int fflag = 0;

void
main(argc, argv)
	int			argc;
	char		       *argv[];
{
	char			local_host_name[256];
	int			i;
	char		       *str;
	int			update_flag = TRUE; /* update dump history */
	double			d_est_tot_tapes;
	double			inter_record_gap;
	register struct fstab  *disk_fstab_entry;
	int			optchar;
	int			num_mounted_fs;
	char			mount_info[8192];
	struct vmount	       *mount_tab_entry;
	char		       *mount_device;
	char		       *remote_host_name;
	time_t			end_time;
	long			first_size = 0;
	long			full_size = 0;
	int			mach_record_size, sbsize, sboff;


	catd = catopen(MF_DUMP, 0);

	/* initialize NLS catalog and yes and no strings */
	setlocale(LC_ALL, "");

	if ((str = getenv("YESSTR")) != NULL)
	{
		yes_flag = TRUE;
		yes_str = str;
	}
	if ((str = getenv("NOSTR")) != NULL)
	{
		no_flag = TRUE;
		no_str = str;
	}

	bzero((char *)&u_spcl, sizeof(u_spcl));

	/* get our host name */

	if (gethostname(local_host_name, sizeof(local_host_name)) < 0)
	{
		msg(MSGSTR(CNGLHN, "Cannot get local host name\n"));
		dump_perror("main(): gethostname()");
		abort_dump();
	}

	/* get start time */

	(void) time(&spcl.c_date);

	/* if no args are given, then default arg is 9u, otherwise	*/
	/* u option must be explicitly specified			*/

	if (argc > 1)
	{
		update_flag = FALSE;
	}

	/* process command line arguments */
	/* BSD style uses first argument string to determine the 
	 * meaning of the following arguments. In this case, the
	 * value doesn't follow immediately the option.
	 */

	if ( bsd_style(argc, argv) ) {
	   char *arg;

	   if(argc > 1) {
		argv++;
		argc--;
		arg = *argv;
		if (*arg == '-')
			argc++;
	   }
	   while(*arg)
		switch (*arg++) {
		case 'w':
			lastdump('w');		/* tell us only what has to be done */
			Exit(X_FINOK);

			/* NOTREACHED */

		case 'W':			/* what to do */
			lastdump('W');		/* tell us the current state of what has been done */
			Exit(X_FINOK);		/* do nothing else */

			/* NOTREACHED */

		case 'B':		/* block mode device */
			by_blocks_flag = TRUE;
			break;

		case 'N':		/* do not rewind tape when done */
			no_rewind_flag = TRUE;
			break;

		case 'S':		/* full tape size, blocks or feet */
			if(argc > 1) {
				argv++;
				argc--;
				full_size = atol(*argv);
			}
			break;

		case 'T':		/* tape number */
			if(argc > 1) {
				argv++;
				argc--;
				curr_tape_num = atol(*argv);
			}
			break;

		case 'f':			/* output file */
			if(argc > 1) {
				argv++;
				argc--;
				tape_file_name = *argv;
				fflag = 1;
			}
			break;
	
		case 'd':			/* density, in bytes per inch */
			if (argc > 1) {
				argv++;
				argc--;
				tape_density = atoi(*argv);
			}
			break;
	
		case 's':			/* tape size, feet */
			if(argc > 1) {
				argv++;
				argc--;
				first_size = atol(*argv);
			}
			break;
	
		case 'b':			/* blocks per tape write */
			if(argc > 1) {
				argv++;
				argc--;
				blocks_per_write = atol(*argv);
			}
			break;
	
		case 'c':			/* Tape is cart. not 9-track */
			medium_flag = CARTRIDGE;
			break;
	
		case '0':			/* dump level */
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			incr_num = arg[-1];
			break;
	
		case 'u':			/* update /etc/dumpdates */
			update_flag = TRUE;
			break;
	
		case 'n':			/* notify operators */
			notify_flag = TRUE;
			break;
	
		default:
			(void) msg(MSGSTR(BADKEY, "Bad option: '%c'\n"), arg[-1]);
			Exit(X_FINBAD);
			/* NOTREACHED */
		}
		if(argc > 1) {
			argv++;
			argc--;
			disk_file_name = *argv;
		}
	
	} else {
	   while ((optchar = getopt(argc, argv, "0123456789WwBNS:T:b:cd:f:ns:u")) != EOF)
	   {
		switch (optchar)
		{
		case '-':
			break;

		case '0':		/* dump level */
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			incr_num = optchar;
			break;

		case 'W':		/* tell us the current state of what
					 * has been done */
		case 'w':		/* tell us only what has to be done */
			lastdump(optchar);
			Exit(X_FINOK);

			/* NOTREACHED */

		case 'B':		/* block mode device */
			by_blocks_flag = TRUE;
			break;

		case 'N':		/* do not rewind tape when done */
			no_rewind_flag = TRUE;
			break;

		case 'S':		/* full tape size, blocks or feet */
			full_size = atol(optarg);
			break;

		case 'T':		/* tape number */
			curr_tape_num = atol(optarg);
			break;

		case 'b':		/* blocks per tape write */
			blocks_per_write = atol(optarg);
			break;

		case 'c':		/* tape is cart, not 9-track */
			medium_flag = CARTRIDGE;
			break;

		case 'd':		/* density, in bytes per inch */
			tape_density = atoi(optarg);
			break;

		case 'f':		/* output file */
			tape_file_name = optarg;
			fflag = 1;
			break;

		case 'n':		/* notify operators */
			notify_flag = TRUE;
			break;

		case 's':		/* first tape size, blocks or feet */
			first_size = atol(optarg);
			break;

		case 'u':		/* update /etc/dumpdates */
			update_flag = TRUE;
			break;

		case '?':
		default:
			(void) msg(MSGSTR(BADKEY, "Bad option: '%c'\n"), optchar);
			Exit(X_FINBAD);

			/* NOTREACHED */
		}
	   }

	/* remaining arg is name of disk file system file to dump */

	   if (argc > optind)
	   {
		disk_file_name = argv[optind];
	   }
	} /* end or argument parsing */

#if	! REMOTE
	
	if (!fflag) {

		/* 
		 * Make sure the default device exists and it is a 
  		 * valid device. If not stop the dump and inform user.
		 */

		int    tape_fd;
		struct devstat d;
                if ((tape_fd = open(tape_file_name, O_RDONLY)) < 0) {
			if (errno == ENOENT) {
				printf("The default tape device '%s' does not exist.\n", tape_file_name);
				abort_dump();
			}
		}
		close(tape_fd);
		if(devstat(tape_file_name, &d) < 0) {
			printf("The default tape device '%s' is not a valid device.\n", tape_file_name);
			abort_dump();
		}
	}

	/* test whether output is to stdout, otherwise get output device info */

	if (strcmp(tape_file_name, "-") == 0)
	{
		pipe_out_flag = TRUE;
		tape_file_name = MSGSTR(STDOUT, "standard output");
		medium_flag = REGULAR_FILE;
	}
	else
	{
		getdevinfo(tape_file_name);
	}

#if	AIX

	/* if output devide is a diskette, set blocks_per_tape limit */
	/* according to device info structure */

	if (medium_flag == DISKETTE)
	{
		by_blocks_flag = TRUE;

		if (full_size == 0)
		{
			full_size = devinfo.un.dk.numblks / 2;
		}

		/* each write is twice the sectors per track, */
		/* in other words, one full cylinder */

		if (blocks_per_write == 0)
		{
			blocks_per_write = 2 * devinfo.un.dk.secptrk;
		}
	}

#endif	AIX

#else   REMOTE

	/* There should be a strategy to determine the exact medium_flag
	 * for remote accesses.
	 */
	medium_flag = REGULAR_FILE;
#endif	! REMOTE

	/*
	 * Determine how to default tape size and density
	 *
	 *             density                    tape size
	 * 9-track     1600 bpi (1600 bytes/inch)   2300 ft.
	 * 9-track     6250 bpi (6250 bytes/inch)   2300 ft.
	 * cartridge   8000 bpi (1000 bytes/inch)   1700 ft. (450 * 4 - slop)
	 * For Paragon (__i860__)
	 * cartridge  61000 bpi                     2900 ft.
	 */

	if (pipe_out_flag == FALSE)
	{
		if (by_blocks_flag == TRUE)
		{
			if (full_size != 0)
			{
				full_tape_blocks = full_size;
			}
			else
			{
				full_tape_blocks = 1953125;	/* about 2Gb */
			}

			if (first_size != 0)
			{
				curr_tape_blocks = first_size;
			}
			else
			{
				curr_tape_blocks = full_tape_blocks;
			}

			if (blocks_per_write == 0)
			{
				blocks_per_write = NTREC;
			}
		}
		else
		{
			if (full_size != 0)
			{
				full_tape_inches = full_size * 12;
			}
			else
			{
				if (medium_flag == CARTRIDGE)
				{
#if __i860__
					full_tape_inches = 2900 * 12;
#else
					full_tape_inches = 1700 * 12;
#endif
				}
				else
				{
					full_tape_inches = 2300 * 12;
				}
			}

			if (first_size != 0)
			{
				curr_tape_inches = first_size * 12;
			}
			else
			{
				curr_tape_inches = full_tape_inches;
			}

			if (tape_density == 0)
			{
#if __i860__
				tape_density = (medium_flag == CARTRIDGE)? 61000: 1600;
#else
				tape_density = (medium_flag == CARTRIDGE)? 1000: 1600;
#endif
			}

			if (blocks_per_write == 0)
			{
				if (tape_density >= 6250)
				{
					blocks_per_write = HIGHDENSITYTREC;
				}
				else
				{
					blocks_per_write = NTREC;
				}
			}

			inches_per_write = (double) blocks_per_write * TP_BSIZE / tape_density;

			/*
			 * CDC 92181's and 92185's make 0.8" gaps in 1600-bpi
			 * start/stop mode (see DEC TU80 User's Guide).  The
			 * shorter gaps of 6250-bpi require repositioning after
			 * stopping, i.e, streaming mode, where the gap is
			 * variable, 0.30" to 0.45".  The gap is maximal when
			 * the tape stops.
			 */

#if __i860__
			if (medium_flag == CARTRIDGE || tape_density == 61000)
			{
				inter_record_gap = 0.0;
			}
#else
			if (medium_flag == CARTRIDGE)
			{
				inter_record_gap = 1.6;
			}
#endif
			else if (tape_density == 6250)
			{
				inter_record_gap = 0.45;
			}
			else
			{
				inter_record_gap = 0.8;
			}

			inches_per_write += inter_record_gap;
		}
	}
	else
	{
		if (blocks_per_write == 0)
		{
			/* dump in 5*1024 byte units to stdout */
			blocks_per_write = 5;
		}
	}

#if	EDUMP

	remote_host_name = "remote system";

#else	! EDUMP

#if	REMOTE

	/* decode old tape_file_name in format host:tape into */
	/* remote_host_name and tape_file_name components */

	remote_host_name = tape_file_name;
	tape_file_name = index(remote_host_name, ':');
	if (tape_file_name == NULL || tape_file_name == remote_host_name)
	{
		msg(MSGSTR(NEEDKEY, "Need option 'f' followed by remote device \"host:tape\"\n"));
		Exit(X_FINBAD);

		/* NOTREACHED */
	}
	*tape_file_name = '\0';
	++tape_file_name;

	/* establish connection to remote host */

	rmthost(&remote_host_name);

	(void) setuid(getuid());	/* rmthost() is the only reason to be setuid */

#endif	REMOTE

#endif	! EDUMP

	/* arrange to catch the following signals, provided they */
	/* were not previously ignored, in which case, continue */
	/* to ignore them */

	if (signal(SIGHUP, sighup) == SIG_IGN)
	{
		(void) signal(SIGHUP, SIG_IGN);
	}
	if (signal(SIGTRAP, sigtrap) == SIG_IGN)
	{
		(void) signal(SIGTRAP, SIG_IGN);
	}
	if (signal(SIGFPE, sigfpe) == SIG_IGN)
	{
		(void) signal(SIGFPE, SIG_IGN);
	}
	if (signal(SIGBUS, sigbus) == SIG_IGN)
	{
		(void) signal(SIGBUS, SIG_IGN);
	}
	if (signal(SIGALRM, sigalrm) == SIG_IGN)
	{
		(void) signal(SIGALRM, SIG_IGN);
	}
	if (signal(SIGSEGV, sigsegv) == SIG_IGN)
	{
		(void) signal(SIGSEGV, SIG_IGN);
	}
	if (signal(SIGTERM, sigterm) == SIG_IGN)
	{
		(void) signal(SIGTERM, SIG_IGN);
	}
	if (signal(SIGINT, sigintr) == SIG_IGN)
	{
		(void) signal(SIGINT, SIG_IGN);
	}

	/* get list of members of operator group from /etc/group */

	if (notify_flag == TRUE)
	{
		if (set_operators() < 0)
		{
			notify_flag = FALSE;
		}
	}

	/*
	 * disk can be either the full special file name, the suffix of the
	 * special file name, the special name missing the leading '/', the
	 * file system name with or without the leading '/'.
	 */

	getfstab();

	disk_fstab_entry = fstabsearch(disk_file_name);
	if (disk_fstab_entry != NULL)
	{
		disk_file_name = rawname(disk_fstab_entry->fs_spec);
		strncpy(spcl.c_dev, disk_fstab_entry->fs_spec, NAMELEN);
		strncpy(spcl.c_filesys, disk_fstab_entry->fs_file, NAMELEN);
	}
	else
	{
		strncpy(spcl.c_dev, disk_file_name, NAMELEN);
		strncpy(spcl.c_filesys, MSGSTR(AULFS, "An unlisted file system"), NAMELEN);
	}

	strcpy(spcl.c_label, MSGSTR(NOLBL, "No label"));
	strncpy(spcl.c_host, local_host_name, NAMELEN);
	spcl.c_level = incr_num - '0';
	spcl.c_type = TS_TAPE;

	/* get dates and times of previous dumps from dumpdates file */

	getitime();

	msg(MSGSTR(DUMPH, "Dumping from host %s\n"), local_host_name);
	msg(MSGSTR(DUMPD, "Date of this level %c dump: %s\n"), incr_num, prdate(spcl.c_date));
	msg(MSGSTR(DUMPDL, "Date of last level %c dump: %s\n"), last_incr_num, prdate(spcl.c_ddate));
	msg(MSGSTR(DUMPING, "Dumping %s "), disk_file_name);
	if (disk_fstab_entry != NULL)
	{
		msgtail("(%s) ", disk_fstab_entry->fs_file);
	}

#if	EDUMP

	msgtail(MSGSTR(TOEHST, "to %s on %s\n"), tape_file_name, remote_host_name);

#else	! EDUMP

#if	REMOTE

	msgtail(MSGSTR(TOHOST, "to %s on host %s\n"), tape_file_name, remote_host_name);

#else	! REMOTE

	msgtail(MSGSTR(TOLOCLA, "to %s\n"), tape_file_name);

#endif	! REMOTE

#endif	! EDUMP

#if	AIX

	/* check to see whether disk file system is currently mounted, */
	/* and if so, issue warning message */

	if (disk_fstab_entry != NULL)
	{
		mount_device = disk_fstab_entry->fs_spec;
	}
	else
	{
		mount_device = disk_file_name;
	}

	num_mounted_fs = mntctl(MCTL_QUERY, sizeof(mount_info), mount_info);
	if (num_mounted_fs <= 0)
	{
		msg(MSGSTR(MNTCTLE, "Cannot get status of mounted file systems\n"));
		dump_perror("main(): mntctl()");
		abort_dump();

		/* NOTREACHED */
	}

	mount_tab_entry = (struct vmount *) mount_info;
	for (i = 0; i < num_mounted_fs; ++i)
	{
		if (strcmp(vmt2dataptr(mount_tab_entry, VMT_OBJECT), mount_device) == 0)
		{
			msg(MSGSTR(WARNING1, "File system still mounted -- inconsistent dump possible\n"));
			break;
		}
		mount_tab_entry = (struct vmount *) ((char *) mount_tab_entry + mount_tab_entry->vmt_length))
	}

#endif	AIX

	/* open raw device file of disk file system */

	in_disk_fd = open(disk_file_name, O_RDONLY);
	if (in_disk_fd < 0)
	{
		msg(MSGSTR(CANTODISK, "Cannot open file-system file %s\n"), disk_file_name);
		dump_perror("main(): open()");
		abort_dump();

		/* NOTREACHED */
	}

	/* get super block of disk file system */

	sync();

#ifdef	SBOFF
	if (ioctl(in_disk_fd, DIOMRINFO, &mach_record_size) < 0) {
		perror("ioctl DIOMRINFO");
		abort_dump();
	}
	/*
	 * Determine the sector size of the device to enable the correct
	 * super block size to be used.
	 */
	if (mach_record_size > SBSIZE) {
		sbsize = mach_record_size;
		sboff = BBOFF + sbsize;
	} else {
		sbsize = SBSIZE;
		sboff = SBOFF;
	}

        super_block = (struct fs *)malloc(sbsize);
        if (super_block == NULL ) {
                perror("Out of Memory");
		abort_dump();
        }

	dev_bsize = mach_record_size;

	bread(sboff/dev_bsize, (char *) super_block, sbsize);

#else	! SBOFF

	/* tahoe system is too new for us */

	dev_bsize = DEV_BSIZE;
	bread(SBLOCK, (char *) super_block, sbsize);

#endif	! SBOFF

	if (super_block->fs_magic != FS_MAGIC)
	{
		msg(MSGSTR(BADFS, "Bad file system: invalid superblock magic\n"));
		abort_dump();

		/* NOTREACHED */
	}

	dev_bsize = super_block->fs_fsize / fsbtodb(super_block, 1);

	if (TP_BSIZE % dev_bsize != 0)
	{
		msg(MSGSTR(TPBSIZE, "TP_BSIZE must be a multiple of DEV_BSIZE\n"));
		abort_dump();

		/* NOTREACHED */
	}

	if ((TP_BSIZE * TP_NINDIR) % super_block->fs_bsize != 0)
	{
		msg(MSGSTR(TPBSNIN, "TP_BSIZE * TP_NINDIR must be a multiple of the dumped file system's block size\n"));
		abort_dump();

		/* NOTREACHED */
	}

	/* alloc inode bit map arrays */
	/* these arrays must be able to accomodate one bit per inode in */
	/* file system */

	imap_size = roundup(howmany(super_block->fs_ipg * super_block->fs_ncg, NBBY), TP_BSIZE);

	not_clear_map = (char *) calloc((unsigned) imap_size, sizeof(char));
	directory_map = (char *) calloc((unsigned) imap_size, sizeof(char));
	to_dump_map = (char *) calloc((unsigned) imap_size, sizeof(char));

	/* start the real work!! */

	/* This first pass completes the inode maps of clear inodes */
	/* and directory inodes.  It also marks inodes changed since */
	/* the last dump (of this increment level) as to-be-dumped */
	/* in the to-be-dumped inode map.  In addition, if any direcories */
	/* are skipped, because they have not changed since the last */
	/* increment, this fact is noted. */

	msg(MSGSTR(MAP1, "Mapping (Pass I) [regular files]\n"));

	pass(mark, NULL);

	/* If any unchanged directories were skipped the in the first */
	/* pass, check up on them now, in the second pass. */

	msg(MSGSTR(MAP2, "Mapping (Pass II) [directories]\n"));

	if (dir_skipped_flag == TRUE)
	{
		dir_added_flag = FALSE;

		pass(add, directory_map);

		while (dir_added_flag == TRUE)
		{
			msg(MSGSTR(MAP2C, "Mapping (Pass II) [directories] (continued)\n"));

			dir_added_flag = FALSE;

			pass(add, directory_map);
		};
	}

	/* update the estimated total block count to account for the sizes */
	/* of the clear and to-dump bit maps */

	est_tot_blocks += bmapest(not_clear_map);
	est_tot_blocks += bmapest(to_dump_map);

	/* Estimate the number of tapes which will be required for */
	/* the entire dump. */

	if (pipe_out_flag == TRUE)
	{
		d_est_tot_tapes = 0.0;
	}
	else
	{
		if (by_blocks_flag == TRUE)
		{
			d_est_tot_tapes = est_tot_blocks / full_tape_blocks;
		}
		else
		{
			double		est_writes;

			est_writes = (double) est_tot_blocks / blocks_per_write;
			est_tot_inches = est_writes * inches_per_write;
			d_est_tot_tapes = est_tot_inches / full_tape_inches;
		}
	}

	/* convert (double) estimate of number of tapes to an */
	/* (int), rounding up */

	est_tot_tapes = (int) (d_est_tot_tapes + 1);

	/* increase the blocks estimate to account for the to-dump map */
	/* on each tape after the first tape */

	for (i = 1; i < est_tot_tapes; ++i)
	{
		est_tot_blocks += bmapest(to_dump_map);
	}

	/* increase the total blocks estimate to account for headers */
	/* on each tape and some trailer blocks on the last tape */

	est_tot_blocks += est_tot_tapes + blocks_per_write;

	msg(MSGSTR(ESTBLK, "Estimate: %ld tape blocks on %3.2f volume(s)\n"), est_tot_blocks, d_est_tot_tapes);

	/* Call open_at_start() to begin output processing */

	open_at_start();

	/* only the child process ever makes it past open_at_start() to here */

	/* put the map of (not) clear inodes on the first tape now */

	bitmap(not_clear_map, TS_CLRI);

	/* now write out all the direcories */

	msg(MSGSTR(MAP3, "Dumping (Pass III) [directories]\n"));

	pass(dirdump, directory_map);

	/* follow the directories with the regular files */

	msg(MSGSTR(MAP4, "Dumping (Pass IV) [regular files]\n"));

	pass(dump, to_dump_map);

	job_trailer();

	msg(MSGSTR(DUMPBLKS, "Actual: %ld tape blocks on %d volume(s)\n"), spcl.c_tapea, curr_volume_num);

	if (pipe_out_flag == FALSE && medium_flag != REGULAR_FILE)
	{
		if (by_blocks_flag == TRUE)
		{
			smsg(MSGSTR(BLKREM, "Blocks remaining on volume: %d"), curr_tape_blocks - blocks_on_curr_tape);
		}
		else
		{
			smsg(MSGSTR(FTREM, "Feet remaining on tape: %d"), (int) (((curr_tape_inches - inches_on_curr_tape) / 12) + 0.5));
		}
		smsg(MSGSTR(VOLUSD, "Volumes used: %d"), spcl.c_volume);
	}

	/* close up output tape processing */

	close_at_end();

	/* update the dump dates file */

	if (update_flag == TRUE)
	{
		putitime();
	}

	/* get end time */

	(void) time(&end_time);

	msg(MSGSTR(DONE, "Dump completed at %s\n"), prdate(end_time));

	if (notify_flag == TRUE)
	{
		broadcast(MSGSTR(DONE2, "DUMP IS DONE!\7\7\n"));
	}

#if	EDUMP

	/* say the dump is done before exiting */

	smsg(MSGSTR(DMPOK, "Dump completed ok"));

#endif	EDUMP

	/* good bye */

	if (super_block) {
		free(super_block);
	}
	Exit(X_FINOK);

	/* NOTREACHED */
}

/* These routines are where the program goes when any of the */
/* corresponding signals (which are not being ignored) are */
/* received */

static void
sighup()
{
	msg(MSGSTR(TRYREW, "SIGHUP received -- Try rewriting\n"));
	handle_signal();
}

static void
sigtrap()
{
	msg(MSGSTR(TRYREW1, "SIGTRAP received -- Try rewriting\n"));
	handle_signal();
}

static void
sigfpe()
{
	msg(MSGSTR(TRYREW2, "SIGFPE received -- Try rewriting\n"));
	handle_signal();
}

static void
sigbus()
{
	msg(MSGSTR(TRYREW3, "SIGBUS received -- Try rewriting\n"));
	handle_signal();
}

static void
sigalrm()
{
	msg(MSGSTR(TRYREW4, "SIGALRM received -- Try rewriting\n"));
	handle_signal();
}

static void
sigsegv()
{
	msg(MSGSTR(ABORTING, "SIGSEGV received -- ABORTING!\n"));
	abort();
}

static void
sigterm()
{
	msg(MSGSTR(TRYREW5, "SIGTERM received -- Try rewriting\n"));
	handle_signal();
}

static void
sigintr()
{
	msg(MSGSTR(INTR, "Interrupt received.\n"));

#if	EDUMP

	abort_dump();

	/* NOTREACHED */

#else	! EDUMP

	if (query(MSGSTR(ABORTDUM, "Do you want to abort dump")) == YES)
	{
		abort_dump();

		/* NOTREACHED */
	}
	(void) signal(SIGINT, sigintr);

#endif	! EDUMP

}

static void
handle_signal()
{
	if (pipe_out_flag == TRUE)
	{
		msg(MSGSTR(UNKSIG, "Unexpected signal -- cannot recover\n"));
		abort_dump();

		/* NOTREACHED */
	}

	msg(MSGSTR(REWRITE1, "Unexpected signal -- attempting to rewrite from last checkpoint\n"));

	/* call rewrite_tape to close up tape and kill any child */
	/* processes */

	rewrite_tape();

	/* NOTREACHED */
}

/*	rawname takes a non-raw device name and returns its	*/
/*	corresponding raw device name by placing an 'r' in the	*/
/*	front of its basename					*/
/*	e.g.:  rawname("/dev/disk") == "/dev/rdisk"		*/

char	       *
rawname(cp)
	char	       *cp;
{
	static char	rawbuf[1024];
	char	       *lastslash;
	
	if ((lastslash = rindex(cp, '/')) == NULL)
	{
		return(NULL);
	}
	*lastslash = '\0';
	(void) sprintf(rawbuf, "%s/r%s", cp, lastslash + 1);
	*lastslash = '/';
	return(rawbuf);
}

#if	! REMOTE

/* Getdevinfo() gets info about the "tape" output file, */
/* primarily to determine whether or not it is a diskette. */

static void
getdevinfo(medium_file_name)
	char	       *medium_file_name;
{
	int		device_fd;
	struct stat	tape_status;

	/* is medium_flag set by options ? */
	if ( medium_flag != NO_MEDIUM )
		return;

	/* treat non existing medium as regular file, which is created later */
	if ( access(medium_file_name, F_OK) < 0 )
	{
		medium_flag = REGULAR_FILE;
		return;
	}

	while ( stat(medium_file_name, &tape_status) < 0 )
	{
		msg(MSGSTR(CNOTST, "Cannot get status of tape or file\n"));
		if (query(MSGSTR(DYWTR, "Do you want to retry")) == NO)
		{
			abort_dump();
		}
	}

	if ( S_ISREG(tape_status.st_mode) )
	{
		medium_flag = REGULAR_FILE;
		return;
	}

	/* all other media are tapes (or diskettes), even if you want
	 * to dump to /dev/null or local disks. In these cases, user
	 * should dump to stdout and pipe to the dump medium.
	 */
	medium_flag = TAPE;

#if	AIX

	if ((device_fd = open(medium_file_name, O_RDONLY)) < 0)
	{
		msg(MSGSTR(MEDIAOPEN, "Cannot open device file %s\n"), medium_file_name);
		dump_perror("getdevinfo(): open()");
		abort_dump();

		/* NOTREACHED */
	}

	if (ioctl(device_fd, IOCINFO, &devinfo) < 0)
	{
		msg(MSGSTR(IOCFAIL, "Cannot get device info for %s\n"), medium_file_name);
		dump_perror("getdevinfo(): ioctl()");
		abort_dump();

		/* NOTREACHED */
	}

	(void) close(device_fd);

	if (devinfo.devtype == DD_DISK && (devinfo.flags & DF_FIXED) == 0)
	{
		medium_flag = DISKETTE;
	}

#endif	AIX

}

#endif	! REMOTE

/* Bmapest() returns the number of tape blocks which will be needed to */
/* dump the specified bit map, plus one for the TS_CLRI/TS_BITS record */

static int
bmapest(map_to_size)
	register char	       *map_to_size;
{
	register int		i;

	/* starting with the last byte in the bit map array, look for */
	/* the last non-zero byte */

	for (i = imap_size - 1; i >= 0; --i)
	{
		if (map_to_size[i] != (char) 0)
		{
			break;
		}
	}

	/* if the bit map was entirely zeroes, return zero */

	if (i < 0)
	{
		return(0);
	}

	return(1 + howmany((i + 1) * sizeof(map_to_size[0]), TP_BSIZE));
}


static int
bsd_style(argn, argp)
int argn;
char *argp[];
{
	int i;

	for( i = 1; i < argn; i++ ) {
		/* look for -option and avoid "-" as default output file */
		if ( argp[i][0] == '-' && argp[i][1] != '\0' )
			return(0);
	}
	return(1);
}
