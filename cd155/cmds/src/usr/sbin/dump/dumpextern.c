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
 * $Log: dumpextern.c,v $
 * Revision 1.4  1994/11/19  03:05:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/06/29  00:13:48  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.2  1992/10/12  21:46:02  shala
 * New version to understand maj, min and node numbers.
 *
 * Revision 1.6  90/10/07  22:07:53  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:09:00  gm]
 * 
 * Revision 1.5  90/08/25  12:28:35  devrcs
 * 	insert rcsid string;
 * 	delete old sccs id string;
 * 	[90/08/09  03:53:59  nm]
 * 
 * Revision 1.4  90/07/27  11:18:39  devrcs
 * 	integration to osc.12
 * 	[90/05/31  04:37:29  nm]
 * 
 * Revision 1.3  90/06/22  22:11:45  devrcs
 * 	integration to osc.12
 * 	[90/05/31  04:37:29  nm]
 * 
 * Revision 1.2  90/04/14  00:12:41  devrcs
 * 	integration of Jim's code
 * 	[90/04/10  08:09:45  nm]
 * 
 * 	Merged AIX3.1 and BSD4.3 to one source.
 * 	Made major changes fixing that code up. (Inserted comments too!)
 * 	Merged (BSD4.3 --> CMU) changes with what resulted.
 * 	Disabled edump (cannot quite figure out what it wants to do,
 * 		also not supported by rmt).
 * 	[90/01/11            walkerj]
 * 
 * 	New file, so that extern variables not multiply declared.
 * 	[90/01/05            walkerj]
 * 
 * $EndLog$
 */
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: dumpextern.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 03:05:34 $";
#endif

#include	"dump.h"

/* actual instance of important spcl record */

union u_spcl	u_spcl;

/*	local external variables	*/

int		imap_size = 0;		/* inode map size in bytes	*/
char	       *not_clear_map = NULL;	/* bit map of inodes which are	*/
					/* (not) clear at time of dump	*/
					/* (0 == clear, 1 == not clear)	*/
char	       *directory_map = NULL;	/* bit map of inodes which are	*/
					/* directories and eligible for	*/
					/* dumping			*/
					/* (0 == not dir, 1 == is dir)	*/
char	       *to_dump_map = NULL;	/* bit map of inodes which are	*/
					/* still to be dumped		*/
					/* (0 == do not dump, 1 == do)	*/

char   *disk_file_name = DEFAULT_DISK;		/* name of the disk file */
char   *tape_file_name = DEFAULT_TAPE;		/* name of the tape file */
char   *dump_hist_file_name = DUMP_HISTORY;	/* name of the file with
						 * dump history information */

/* if no argument is given, then 9u is implied */

char		incr_num = '9';		/* increment number */
char		last_incr_num;		/* increment number of previous
					 * dump */

ino_t		curr_inum;		/* current inumber */

int		new_tape_flag = TRUE;	/* new tape flag */
int		curr_tape_num = 1;	/* current tape number */
int		curr_volume_num = 1;	/* current volume number */

int		dir_added_flag;		/* true if added directory to to-be-
					 * dumped list on last pass-two */
int		dir_skipped_flag = FALSE; /* set TRUE in mark() if any
					 * directories are skipped; this
					 * lets us avoid map pass 2 in
					 * some cases */

int		by_blocks_flag = FALSE; /* TRUE when measuring output by blocks,
					 * FALSE when by inches */

long		full_tape_blocks = 0;	/* full tape size in blocks */
long		curr_tape_blocks = 0;	/* current tape size in blocks */
double		full_tape_inches = 0.0;	/* full tape size in inches */
double		curr_tape_inches = 0.0;	/* current tape size in inches */
long		blocks_on_curr_tape = 0; /* number of blocks written on
					 * current tape */
double		inches_on_curr_tape = 0.0; /* number of inches written on
					 * current tape */
int		tape_density = 0;	/* density in bytes/inch */
long		est_tot_blocks = 0;	/* estimated total number of blocks */
double		est_tot_inches;		/* estimated total number of inches */
int		est_tot_tapes;		/* estimated number of tapes */
int		blocks_per_write = 0;	/* blocking factor on tape */
double		inches_per_write;	/* inches for each tape write */

long		dev_bsize;	/* machine device block size in bytes */

int		in_disk_fd;		/* disk file descriptor */
int		out_tape_fd;		/* tape file descriptor */

int		pipe_out_flag = FALSE;	/* true => output to standard output */
int		medium_flag = NO_MEDIUM;/* medium not yet known */
int		no_rewind_flag = FALSE;	/* do not rewind tape */
int		notify_flag = FALSE;	/* notify operators */

int		num_idate_records = 0;	/* number of idate records (might
					 * be zero) */
struct idates **idate_array = NULL;	/* pointer to array of pointers to */
					/* dump_history idates structures */

/*	 buffer for the file system super block		*/

struct fs      *super_block = NULL;

/*	NLS stuff	*/
nl_catd		catd;
char	       *yes_str = "yes";
char	       *no_str = "no";
int		yes_flag = FALSE;
int		no_flag = FALSE;
