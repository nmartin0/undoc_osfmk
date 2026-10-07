/*
 * 
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		apc.c
 Title:		Array Parity Check Program
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:27:43 $ 
 Programmer:	rmj, sjr
 Documents:	1.  UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2.  "Object-Oriented Programming in C", C User's Journal 7/90


 COPYRIGHT 1991, NCR Corp.

 Description:	The apc program provides a user interface for disk
		array parity checking.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stddefs.h"
#include "scsi_dk.h"
#include "zip_dialog.h"
#include "fnm_dialog.h"
#include "dac_event.h"
#include "dau_err.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
/*
 *  maximum number of blocks which should be verified per invocation of the
 *  SCSI [block] verify command.
 */
#define	MAXVBLOCKS (u_short)	10000

/* external declaration */
extern void print_title(int I, char *t);
/*  forward declaration  */
extern void cleanup(ZIP_DIALOG *z, FNM_DIALOG *f, SCSI_DK *s);
extern int raid_level();

/******************************************************************************
 ***		             VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
/*
 * (By convention Argc, Argv, and Interactive are globally accessible -
 * its so much easier than passing them on every constructor)
 *
 */
int Argc;
char **Argv;
u_int Interactive;
static char ident[] = PROJREL "/$Revision: 1.2.4.1 $ - $RCSfile: apc.c,v $, Array Parity Check Program";
char title[] = "Disk Array Parity Check Program";

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

main( argc, argv )
int argc;
char *argv[];
{
	extern u_long cs_swap_4();
	SCSI_DK *dac;
	FILE *default_fp;
	int ofd;
	int exclusive, create;
	hw_zipcode hw_addr;
	ZIP_DIALOG *zip_dialog;
	FNM_DIALOG *file_dialog;
	int RtnStat;
	int detail;
	u_long LBA;
	u_long Max_LBA;	/* Maximum Logical_Block_Address on the disk */
	u_int PMI;
	u_long Block_Size;
	u_char Read_Cap_Data[8], *Block_Size_Data;
	u_int pcheck = TRUE;
	u_long Start_Block;
	u_long End_Block;
	u_short Vfy_Blk_Cnt;
	u_char *Miscomp_Bk_Data;
	u_long Miscomp_Block;
	u_int Bad_Block_Found;
	u_long Bad_Blocks;
	u_int done;
	DAC_EVENT *event;
	char errmsg[80];
	int EvStat;
	int rl;


	Argc = argc;
	Argv = argv;
#ifdef PARAGON860 /* force non-interactive mode */
#define NOINTERACTIVE
#endif
#ifdef NOINTERACTIVE
	Interactive = FALSE;
	if (argc == 1)
		error(COMMAND_LINE_SYNTAX, CMDLINE_SYNTAX_ERROR);
#else
	Interactive = ( argc == 1 );
#endif

	print_title( (int) Interactive, title );

	/*  Initialize and allocate required data structures  */
	zip_dialog = new_ZIP_DIALOG();
	hw_addr = (hw_zipcode) zip_dialog->prompt( zip_dialog );
	file_dialog = new_FNM_DIALOG( default_fp = stdout, create = TRUE );
	ofd = (int) file_dialog->prompt( file_dialog );
	if ( ( rl = raid_level( hw_addr ) ) == -1 )
		exit( NON_SPECIFIC_ERROR );
	else if ( rl == 0 ) {
		error( INAPPROPRIATE_STATE, INAPPROPRIATE_RAIDLVL_MSG );
		exit( INAPPROPRIATE_STATE );
	}
	/*  Get SCSI_DK object  */
	if ( (dac = new_SCSI_DK( hw_addr, exclusive=FALSE )) != NULL )
		debug( "got dac\n" );
	else {
		error( DEVICE_OPEN, NEW_SCSI_DEV_FAIL );
		exit( DEVICE_OPEN );
	}

	/*  Initialize variables needed to do read_capacity  */
	/*  Partial_Medium_Indicator, set to 0 to get full drive capacity.  */
	PMI = 0;
	/*  Logical_Block_Address must be 0 if PMI=0, otherwise  */
	/*  CHECK_CONDITION will result.			 */
	LBA = 0;

	/*  Get block capacity info for the disk  */
	RtnStat = dac->read_capacity( dac, Read_Cap_Data, LBA, PMI );

	/*  Check RtnStat for SCSI/other errors  */
	sprintf( errmsg, "READ CAPACITY failed" );
	if ( ( EvStat = chk4_dacevent( RtnStat, errmsg ) ) != 0 ) {
		if ( EvStat == 1 ) {
			event = make_dacevent( hw_addr, RtnStat, detail = (int) FALSE );
			if ( event != NULL )
				event->describe( event );
			event->destroy( event );
		}
		cleanup(zip_dialog, file_dialog, dac);
		error( SCSI_CONDITION, READ_CAPACITY_FAIL );
		exit( SCSI_CONDITION );
	}

	/*  Convert Read_Cap_Data[8] to block capacity and block size  */
	/*  Read_Cap_Data[0-3] is LBA of last block on disk	*/
	/*  Read_Cap_Data[4-7] is block size (in bytes)		*/
	Block_Size_Data = ( Read_Cap_Data + 4 );
	Max_LBA = cs_swap_4( Read_Cap_Data );
	Block_Size = cs_swap_4( Block_Size_Data );
	debug( "Read_Cap_Data[0-7] = %x %x %x %x %x %x %x %x (hex)\n",
	    Read_Cap_Data[0], Read_Cap_Data[1],
	    Read_Cap_Data[2], Read_Cap_Data[3],
	    Read_Cap_Data[4], Read_Cap_Data[5],
	    Read_Cap_Data[6], Read_Cap_Data[7]);
	if ( isatty( 2 ) )
		fprintf( stderr, "  Beginning parity verification of disk blocks numbered 0-%lu (decimal) ...\n", Max_LBA);

	/*  Prepare to verify the disk.  SCSI-2 does not specify a limit on  */
	/*  the verification [block] length.  But there are only two bytes   */
	/*  for it in the CDB, so assume it is 65536.  Experiments with      */
	/*  non-DAC disks (Conner & WREN VI) showed SCSI bus time-out        */
	/*  problems (with 53C90/700) for verify lengths > 10000.  So        */
	/*  MAXVBLOCKS is defined above to limit the verification chunk	     */
	/*  sizes.  Entire disk will be verified in MAXVBLOCKS chunk sizes.  */
	/*  If a chunk fails, the sense_data information bytes will be read  */
	/*  to find the first failed block.  That block will be reported as  */
	/*  failed, and verification will continue from failed block+1 thru  */
	/*  the end of the chunk.					     */

	/*  Initialize variables needed for verify process  */
	if ( enabled( "[StdVerify]" ) )
		pcheck = FALSE;	/* set parity_check bit TRUE (vendor unique) */
	Vfy_Blk_Cnt = 0;
	done = FALSE;
	Bad_Block_Found = FALSE;
	Start_Block = 0;
	End_Block = 0;
	Bad_Blocks = 0;

	/* It would most probably be safer to lock disk while running the    */
	/* verify portion of apc; the problem is running apc on root disk,   */
	/* lock will always fail with root mounted.  So leave lock	     */
	/* commented OUT.						     */
/*
	if ( dac->lock( dac ) != 0 ) {
		error( DEVICE_OPEN, SCSI_LOCK_FAIL );
		exit( DEVICE_OPEN );
	}
*/

	/*  MAIN LOOP  */
	while ( ! done ) {
		/*  Always try to bite off MAXVBLOCKS, even if a bad block   */
		/*  was just found.  Then set & check End_Block.  Adjust     */
		/*  End_Block if we are at/near the end of the disk.	     */
		/*  NOTE: Start_Block is reset at the bottom of this while   */
		/*  ( ! done ) loop.					     */
		Vfy_Blk_Cnt = MAXVBLOCKS;
		if ( (End_Block = Start_Block + Vfy_Blk_Cnt - 1) > Max_LBA ) {
			End_Block = Max_LBA;
			Vfy_Blk_Cnt = End_Block - Start_Block + 1;
		}

		if ( isatty( 2 ) )
			fprintf( stderr, "  Verifying blocks %06lu-%06lu (decimal) ...\n",
		    Start_Block, End_Block );

		Bad_Block_Found = FALSE;

		RtnStat = dac->verify( dac, NULL, 0L, Start_Block, Vfy_Blk_Cnt, pcheck );

		/*  Check RtnStat.  If it's not zero then check further to   */
		/*  make sure it was caused be verification failure	     */
		/*  (i.e., sense key == MISCOMPARE).  Begin a block-by-block */
		/*  verification routine for this chunk of Vfy_Blk_Cnt disk  */
		/*  blocks.						     */
		sprintf( errmsg, "VERIFY failed" );
		if ( ( EvStat = chk4_dacevent( RtnStat, errmsg ) ) != 0 ) {
			if ( EvStat == 1 ) {
				event = make_dacevent( hw_addr, RtnStat, detail = (int) FALSE );
				if ( event != NULL ) {
					/*  OK, check to make sure that	     */
					/*  sense key is miscompare and then */
					/*  retrieve first failed block from */
					/*  sense data information bytes.    */
					if ( event->sense_data.Sense_Key == Key_Miscompare ) {
						Miscomp_Bk_Data =
						    (u_char *) &(event->sense_data.Information_MSB);
#ifdef DEBUG_ON
						dump_sense_data( &(event->sense_data) );
#endif
						debug( "  Info_Bytes[0-3] = %x %x %x %x (hex)\n",
						    Miscomp_Bk_Data[0], Miscomp_Bk_Data[1],
						    Miscomp_Bk_Data[2], Miscomp_Bk_Data[3] );
						Miscomp_Block = cs_swap_4( Miscomp_Bk_Data );
						if ( Start_Block <= Miscomp_Block && Miscomp_Block <= End_Block ) {
							/*  write block number to ofd	*/
							fprintf( (FILE *) ofd, "%lu\n", Miscomp_Block);
							Bad_Blocks++;
							Bad_Block_Found = TRUE;
						}
						else {
							cleanup(zip_dialog, file_dialog, dac);
							if ( isatty( 2 ) )
								fprintf( stderr, "Miscompared block, %lu, not in range %lu-%lu\n",
							    Miscomp_Block, Start_Block, End_Block );
							error( NON_SPECIFIC_ERROR, MISCOMP_RANGE_ERROR );
							exit( NON_SPECIFIC_ERROR );
						}
					}
					else {
						/*  Something unexpected has */
						/*  happened; report it and  */
						/*  then return with	     */
						/*  SCSI_CONDITION.	     */
						event->describe( event );
						event->destroy( event );
						cleanup(zip_dialog, file_dialog, dac);
						error( SCSI_CONDITION, SCSI_VERIFY_ERROR );
						exit( SCSI_CONDITION );
					}
				}
				else {
					/*  We don't ever expect a NULL	     */
					/*  dac_event; if it happens, notify */
					/*  caller and try to continue.	     */
					if ( isatty( 2 ) )
						fprintf( stderr, "NULL EVENT occurred during verify\n" );
				}
			}
			else {
				cleanup(zip_dialog, file_dialog, dac);
				error( NON_SPECIFIC_ERROR, VERIFY_FAIL_NOSENSE );
				exit( NON_SPECIFIC_ERROR );
			}
		}

		/*  adjust start of range to verify	*/
		if ( Bad_Block_Found ) {
			if ( (Start_Block = Miscomp_Block + 1) > Max_LBA )
				done = TRUE;
		}
		else {
			if ( (Start_Block += Vfy_Blk_Cnt) > Max_LBA )
				done = TRUE;
		}
	}

	cleanup(zip_dialog, file_dialog, dac);
	exit( 0 );
}


void
cleanup(zdlg, fdlg, dac)
ZIP_DIALOG *zdlg;
FNM_DIALOG *fdlg;
SCSI_DK *dac;
{
	zdlg->destroy( zdlg );
	fdlg->destroy( fdlg );
	dac->destroy( dac );
}

static int
enabled( s )
char *s;
{
	int fd, is_enabled;
#ifdef PARAGON860
	char fn[128];

	strcpy(fn,"/etc/array/");
#else
	char fn[128] = "/etc/array/";
#endif

	strcat(fn,s);

	if ( ( fd = open(fn, O_RDONLY ) ) != -1 )
		is_enabled = TRUE;
	else
		is_enabled = FALSE;
	close( fd );
	return( is_enabled );
}
