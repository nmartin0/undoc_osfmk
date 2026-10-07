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
 * $Log: dumpcheck.c,v $
 * Revision 1.7  1994/11/19  03:05:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/01/08  00:19:37  shala
 *  Reviewer: Jerrie Coffman
 *  Risk: Low
 *  Benefit or PTS #: Cleanup
 *  Testing: Build a new dump command and tested it.
 *  Module(s): src/usr/sbin/dump/dumpcheck.c
 *
 * It is not necessary to read header in case of norewind. So, check is
 * added to skip reading header.
 *
 * Revision 1.5  1994/01/06  18:15:35  shala
 *  Reviewer: Jerrie Coffman
 *  Risk: Low
 *  Benefit or PTS #: Bug #7171
 *  Testing: Made a new dump command and tested it.
 *  Module(s): src/usr/sbin/dump/dumpcheck.c
 *
 * Added check for the norewind case when reading the header. In the
 * norewind case the tape is positioned at the end, so the header is
 * NULL.
 *
 * Revision 1.4  1993/06/15  17:58:28  shala
 * Use MT_ISSCSI instead of hard coded 7.
 *
 * Revision 1.3  1993/04/06  21:50:13  shala
 * Include valid type for Paragon tape (not sure why the type is 7).
 *
 * Revision 1.2  1992/10/12  21:46:00  shala
 * New version to understand maj, min and node numbers.
 *
 * Revision 1.7  90/10/07  22:07:48  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:08:52  gm]
 * 
 * Revision 1.6  90/08/25  12:28:31  devrcs
 * 	insert rcsid string;
 * 	delete old sccs id string;
 * 	[90/08/09  03:53:20  nm]
 * 
 * Revision 1.5  90/07/27  11:18:33  devrcs
 * 	integration to osc.12
 * 	[90/05/31  04:37:18  nm]
 * 
 * Revision 1.4  90/06/22  22:11:42  devrcs
 * 	integration to osc.12
 * 	[90/05/31  04:37:18  nm]
 * 
 * Revision 1.3  90/04/14  00:12:38  devrcs
 * 	integration of Jim's code
 * 	[90/04/10  08:09:11  nm]
 * 
 * 	Merged AIX3.1 and BSD4.3 to one source.
 * 	Made major changes fixing that code up. (Inserted comments too!)
 * 	Merged (BSD4.3 --> CMU) changes with what resulted.
 * 	Disabled edump (cannot quite figure out what it wants to do,
 * 		also not supported by rmt).
 * 	[90/01/11            walkerj]
 * 
 * Revision 1.2  90/01/02  18:41:38  gm
 * 	Fixes for first snapshot.
 * 	[90/01/02            gm]
 * 
 * Revision 1.1  89/12/26  08:56:49  gm
 * 	Current version from CMU.
 * 	[89/12/21            gm]
 * 
 * 	- Make drive status check a little more sane.
 * 	- Use a local instead of the global for the temporary tape
 * 	  drive access (don't trash the gloal).
 * 	- Fix the bug where the dump program did not use very large
 * 	  reads to check the header...if we wrote large then a small
 * 	  read will fail on some tape drives.
 * 	- Fix overwrite warning so the warning comes BEFORE the action not
 * 	  during.
 * 	[88/02/18            ern]
 * 
 * 	Fixed to omit date check when dumping to the standard output.
 * 	[86/08/29            mja]
 * 
 * 	Adjusted tape status fetching mechanism to be more general.
 * 	[86/08/18            jm]
 * 
 * 	This module was created to allow remedial checking of the tapes
 * 	to make sure that the operators were not overwriting a still current
 * 	tape in the backup scheme by accident. Any "errors" that are found
 * 	are brought to the operator's attention and then it is up to them
 * 	to decide the correct action.
 * 	[83/04/04            jm]
 * 
 * $EndLog$
 */
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: dumpcheck.c,v $ $Revision: 1.7 $ (OSF) $Date: 1994/11/19 03:05:33 $";
#endif

#include	"dump.h"
#include	<sys/file.h>
#include	<sys/ioctl.h>
#include	<sys/mtio.h>
#include	<sys/stat.h>

static int	read_header();
static int	at_load_point();
static int	check_checksum();
static void	reposition_tape();

/*
 * check the tape for a valid date. return TRUE on success.
 */

int
check_tape()
{
	struct s_spcl  *tape_header;
	int		status;
	int		tape_fd;

	if ( medium_flag == REGULAR_FILE )
		return( TRUE );

	while ((tape_fd = open(tape_file_name, O_RDONLY)) < 0)
	{
		msg(MSGSTR(CNOTFD, "Cannot open tape for date check\n"));
		if (query(MSGSTR(DYWTR, "Do you want to retry")) == NO)
		{
			abort_dump();
		}
	}

	status = TRUE;

	if (at_load_point(tape_fd) == TRUE)
	{
		tape_header = (struct s_spcl *) malloc((unsigned) max(blocks_per_write, HIGHDENSITYTREC) * TP_BSIZE);

		if (read_header(tape_fd, tape_header) == FALSE)
		{
			msg(MSGSTR(BWTDT1, "Cannot get the previous write date of tape\n"));
			msg(MSGSTR(BWTDT2, "Tape may be either 1) not a dump tape, 2) a new,\n"));
			msg(MSGSTR(BWTDT3, "unused tape, or 3) not at the load-point\n"));
			if (query(MSGSTR(DYWTOT, "Do you want to overwrite this tape")) == NO)
			{
				status = FALSE;
			}
		}
		else if (spcl.c_date - tape_header->c_date < (int) (DAY * 6.5))
		{

			/* less than 6.5 days old */

			msg(MSGSTR(RUIN1, "This tape was written last on %s\n"), ctime(&tape_header->c_date));
			msg(MSGSTR(RUIN2, "That was less than a week ago!\n"));
			msg(MSGSTR(RUIN3, "You may be RUINING a good tape!  Be careful!\n"));
			if (query(MSGSTR(DYRWTO, "Do you really wish to overwrite this tape")) == NO)
			{
				status = FALSE;
			}
		}
		else
		{
			msg(MSGSTR(VLDDT, "Mounted tape has a valid date\n"));
		}

		reposition_tape(tape_fd);

		free(tape_header);
	}

	(void) close(tape_fd);

	return(status);
}

static int
at_load_point(tape_fd)
	int		tape_fd;
{
	struct mtget	mtget;

	if (ioctl(tape_fd, MTIOCGET, &mtget) < 0)
	{
		msg(MSGSTR(UTCDS, "Unable to check drive status\n"));
		dump_perror("at_load_point(): ioctl()");
		if (query(MSGSTR(DYWTCA, "Do you want to continue anyway")) == NO)
		{
			abort_dump();
		}
		return(FALSE);
	}

	switch (mtget.mt_type) {
#if     __i860__

        case MT_ISSCSI:
		if (no_rewind_flag == TRUE)
			return(FALSE);
		else
                	return(TRUE);
#endif  __i860__

#if	vax

	case MT_ISHT: 
		if (mtget.mt_dsreg & HTDS_BOT)
		{
			return(TRUE);
		}
		return(FALSE);

	case MT_ISUT: 
		if (mtget.mt_dsreg & UTDS_BOT)
		{
			return(TRUE);
		}
		return(FALSE);

	case MT_ISTS: 
		if (mtget.mt_dsreg & TS_BOT)
		{
			return(TRUE);
		}
		return(FALSE);

	case MT_ISTM: 
		if (mtget.mt_dsreg & TMER_BOT)
		{
			return(TRUE);
		}
		return(FALSE);

#endif	vax

#if	sun

	case MT_ISXY:
		if (mtget.mt_dsreg & XTS_BOT)
		{
			return(TRUE);
		}
		return(FALSE);

#endif sun

	default: 
		msg(MSGSTR(UTDT, "Unknown tape drive type: %d (0x%x)\n"), mtget.mt_type, mtget.mt_dsreg);
		msg(MSGSTR(SDCHK, "Skipping date check\n"));
		return(FALSE);
	}

	/*NOTREACHED*/
}

/*
 *	Examine the header to see that it is OK. That is, is it a header
 *	Return FALSE on error.
 */

static int
read_header(tape_fd, tape_header)
	int		tape_fd;
	struct s_spcl  *tape_header;
{
	int cnt;
	if ((cnt=read(tape_fd, (char *) tape_header, blocks_per_write * TP_BSIZE)) < 0)
	{
		msg(MSGSTR(TREWTH, "Tape read error while trying to read tape header\n"));
		dump_perror("read_header(): read()");
		return(FALSE);
	}
	if (tape_header->c_magic != NFS_MAGIC && tape_header->c_magic != OFS_MAGIC)
	{
		if ((cnt == 0 || tape_header->c_magic == 0) && no_rewind_flag == TRUE) {
			return(TRUE);
		} else {
			msg(MSGSTR(BDMGK, "Bad magic number in tape header\n"));
			return(FALSE);
		}
	}
	if (check_checksum((int *) tape_header) == FALSE)
	{
		msg(MSGSTR(CKSMERR, "Checksum error in tape header\n"));
		return(FALSE);
	}
	if (tape_header->c_type != TS_TAPE)
	{
		msg(MSGSTR(THNTTH, "Tape header record not of type tape-header\n"));
		return(FALSE);
	}
	return(TRUE);
}

static int
check_checksum(buffer)
	register int	*buffer;
{
	register int	i, j;

	j = sizeof(union u_spcl) / sizeof(int);
	i = 0;

	do
	{
	    i += *buffer++;
	}
	while (--j > 0);

	if (i != CHECKSUM)
	{
		return(FALSE);
	}
	return(TRUE);
}

/*
 * Reposition the tape at load_point
 */

static void
reposition_tape(tape_fd)
	int		tape_fd;
{
	struct mtop	mtop;

	mtop.mt_op = MTREW;
	mtop.mt_count = 1;

	if (ioctl(tape_fd, MTIOCTOP, &mtop) < 0)
	{
		if (ioctl(tape_fd, MTIOCTOP, &mtop) < 0)
		{
			msg(MSGSTR(TRRF, "Tape repositioning (rewind) failed\n"));
			dump_perror("reposition_tape(): ioctl()");
			msg(MSGSTR(CKPTBC, "Check position of tape before continuing\n"));
		}
	}
}
