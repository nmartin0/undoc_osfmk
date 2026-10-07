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
 Name:		apr.c
 Title:		Array Parity Repair Program
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:28:00 $ 
 Programmer:	rmj
 Documents:	1.  UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2.  "Object-Oriented Programming in C", C User's Journal 7/90


 COPYRIGHT 1991, NCR Corp.

 Description:	The apr program provides a user interface for disk
		array parity repair.
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
#define MAXSTRING	256

/* external declaration */
extern void print_title(int I, char *t);
/*  forward declaration  */
extern void cleanup(ZIP_DIALOG *z, FNM_DIALOG *f, SCSI_DK *s);
extern raid_level();

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
static char ident[] = PROJREL "/$Revision: 1.2.4.1 $ - $RCSfile: apr.c,v $, Disk Array Parity Repair Program";
char title[] = "Disk Array Parity Repair Program";

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

main( argc, argv )
int argc;
char *argv[];
{
	SCSI_DK *Dac;
	ZIP_DIALOG *zip_dialog;
	FNM_DIALOG *file_dialog;
	DAC_EVENT *event;
	FILE *default_fp, *inf;
	int exclusive, create;
	hw_zipcode hw_addr;
	int RtnStat;
	char InpBufr[MAXSTRING], InpBufPtr;
	u_long BlockNum;
	u_int ParityCk = TRUE;
	int detail;
	char errmsg[80];
	u_long LBA;
	u_long Max_LBA;	/* Maximum Logical_Block_Address on the disk */
	u_int PMI;
	u_long Block_Size;
	u_char *Block_Buffer;
	u_char Read_Cap_Data[8], *Block_Size_Data;
	int EvStat;
	int rl;
	int sdacrel3 = FALSE;
	SCSI_Inquiry_Data_t inqdata;

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
	file_dialog = new_FNM_DIALOG( default_fp = stdin, create = FALSE );
	inf = (FILE *) file_dialog->prompt( file_dialog );

	/* Verify the RAID level */
	if ( ( rl = raid_level( hw_addr ) ) == -1 )
		exit( NON_SPECIFIC_ERROR );
	else if ( rl == 0 ) {
		error( INAPPROPRIATE_STATE, INAPPROPRIATE_RAIDLVL_MSG );
		exit( INAPPROPRIATE_STATE );
	}

	/*  Initialize variables needed to access SCSI device  */
	if ( (Dac = new_SCSI_DK( hw_addr, exclusive=FALSE )) != NULL )
		debug( "got dac\n" );
	else {
		error( DEVICE_OPEN, NEW_SCSI_DEV_FAIL );
		exit( DEVICE_OPEN );
	}

	/* Determine SDAC sw level and set appropriate flag */
	debug("determining SDAC sw level\n");
	if ( Dac->inquiry(Dac,(u_char *) &inqdata, sizeof(inqdata))==0)
	{
		debug("\n%s\n",inqdata.Revision_Level);
		if ( strcmp(inqdata.Revision_Level, "0300") >= 0 )
		{
			sdacrel3 = TRUE;
		}
	}
	else {
		error( DEVICE_OPEN, NO_INQUIRY_DATA );
		exit( DEVICE_OPEN );
	}

	/* Now see if we can get exclusive access to the volume */
	/* Only get exclusive if the SDAC is less that sw rev 3.0 */
	if (sdacrel3 == FALSE)
	{
		if ( Dac->lock( Dac ) != 0 ) {
			/* lock call failed */
			if ( Dac->is_root( Dac ) ) {
			    if ( runlevel() != SINGLE_USER_MODE ) {
				error(INAPPROPRIATE_STATE, NOT_SINGLE_USER_MSG);
				exit( INAPPROPRIATE_STATE );
			    }
			}
			else {
				error( DEVICE_OPEN, NOT_OFFLINE_MSG );
				exit( DEVICE_OPEN );
			}
		}
	}

#ifndef PARAGON860
	if ( enabled( "[StdWrVerify]" ) )
		ParityCk = FALSE;
#endif

	/*  Initialize variables needed to do read_capacity  */
	/*  Partial_Medium_Indicator, set to 0 to get full drive capacity.  */
	PMI = 0;
	/*  Logical_Block_Address must be 0 if PMI=0, otherwise  */
	/*  CHECK_CONDITION will result.			 */
	LBA = 0;

	/*  Get block capacity info for the disk  */
	debug("doing read capacity\n");
	RtnStat = Dac->read_capacity( Dac, Read_Cap_Data, LBA, PMI );

	/*  Check RtnStat for SCSI/other errors  */
	sprintf( errmsg, "READ CAPACITY failed" );
	if ( ( EvStat = chk4_dacevent( RtnStat, errmsg ) ) != 0 ) {
		if ( EvStat == 1 ) {
			event = make_dacevent( hw_addr, RtnStat, detail = (int) FALSE );
			if ( event != NULL )
				event->describe( event );
			event->destroy( event );
		}
		cleanup(zip_dialog, file_dialog, Dac);
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
/* now allocate memory based on Block Size for read and write functions */
	Block_Buffer = ((u_char *) zalloc(Block_Size));

	/*  Loop on input block numbers, do read block, then write-verify  */
	while ( fgets( InpBufr, MAXSTRING, inf ) != NULL ) {

		InpBufr[strlen(InpBufr)-1] = '\0';
		debug( "InpBufr = :%s:, len = %d\n", InpBufr, strlen(InpBufr) );
		BlockNum = atol( InpBufr );
		/*  probably need to add some error checking on input HERE  */

		/*  Read a block  */
		if ( isatty( 2 ) ) /* Is stderr a terminal? */
			fprintf(stderr, "  Processing block number %lu\n", BlockNum );
		debug("\ndoing dac read\n");
		RtnStat = Dac->read( (SCSI_DEV *) Dac,
		    Block_Buffer,
		    Block_Size,
		    BlockNum,
		    (u_short) 1 );
		debug("\nRtnStat after read is %d",RtnStat);

		/*  Check for SCSI/other errors  */
/* if rel3 and miscompare or recovered error ignore the read error */
		sprintf( errmsg, "READ of block %ld failed", BlockNum );
		if ( ( EvStat = chk4_dacevent( RtnStat, errmsg ) ) != 0 ) {
			if ( EvStat == 1 ) {
				event = make_dacevent( hw_addr, RtnStat, detail = (int) FALSE );
				if ( event != NULL )
				{
			/*Ignore a raid level 3 miscompare or recovered error*/
				  debug("\nkey is %ld\n",
				  event->sense_data.Sense_Key);
				  debug("\nrl is %ld\n",rl);
				  if (((event->sense_data.Sense_Key !=
				      Key_Recovered_Error) &&  
				      (event->sense_data.Sense_Key !=
				      Key_Miscompare)) || (rl != 3)) 
					event->describe( event );
				  event->destroy( event );
				}
			}
			/*Ignore a raid level 3 miscompare or recovered error*/
			if (((event->sense_data.Sense_Key != 
				Key_Recovered_Error)
			  && (event->sense_data.Sense_Key != 
				Key_Miscompare))
			    || (rl != 3))
			{
				cleanup( zip_dialog, file_dialog, Dac );
				error( SCSI_CONDITION, SCSI_READ_ERROR );
				exit( SCSI_CONDITION );
			}
		}
		/*  Attempt to do a repair parity on the block  */
		/*  as though SDAC 3.0 sw is loaded.  If the */
		/*  repair parity fails, attempt to do a write_verify */
		/*  on the block as though an earlier version of */
		/*  SDAC sw is loaded */
		if (sdacrel3) {
			struct {
			u_char	Reserved1; /* header */
			u_char	Reserved2;
			u_char 	Defect_List_Length[2];
			u_char	Defect_Logical_Block_Address[4];
			} Defect_List;

			u_long  val;
			Bytes_In_Long_t *bval;

			debug("Repairing parity on SDAC 3.0 sw");
			Defect_List.Defect_List_Length[0]=0;
			Defect_List.Defect_List_Length[1]=4;
			val=BlockNum;
			bval=(Bytes_In_Long_t *) &val;
			Defect_List.Defect_Logical_Block_Address[0]=bval->MSB;
			Defect_List.Defect_Logical_Block_Address[1]=bval->MIB_1;
			Defect_List.Defect_Logical_Block_Address[2]=bval->MIB_2;
			Defect_List.Defect_Logical_Block_Address[3]=bval->LSB;
			RtnStat = Dac->repair_parity (Dac,
			    (u_char *) &Defect_List,
		    	    (u_int) sizeof(Defect_List));
			debug("\nRtnStat after repair parity is %d\n",RtnStat);
		}
		else
		{
			RtnStat = Dac->write_verify(Dac,
		    	    Block_Buffer,
		    	    Block_Size,
		    	    BlockNum,
		    	    (u_short) 1,
		    	    ParityCk );
			debug("\nRtnStat after write buffer is %ld\n",RtnStat);
		}

		/*  Check for SCSI/other errors  */
		if (sdacrel3) {
			sprintf( errmsg,"REPAIR_PARITY of block %ld failed", BlockNum );
		}
		else
		{
			sprintf( errmsg,"WRITE_VERIFY of block %ld failed", BlockNum );
		}
		if ( ( EvStat = chk4_dacevent( RtnStat, errmsg ) ) != 0 ) {
			if ( EvStat == 1 ) {
				event = make_dacevent( hw_addr, RtnStat, detail = (int) FALSE );
				if ( event != NULL )
					event->describe( event );
				event->destroy( event );
			}
			cleanup( zip_dialog, file_dialog, Dac );
			if (sdacrel3)
				error( SCSI_CONDITION, SCSI_REPPARITY_ERROR );
			else
				error( SCSI_CONDITION, SCSI_WRTVERIFY_ERROR );
			exit( SCSI_CONDITION );
		}

	}

	/*  Cleanup and return with good status  */
	cleanup( zip_dialog, file_dialog, Dac );
	return( 0 );
}


void
cleanup( zdlg, fdlg, dac )
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
	int fd, _enabled_;
#ifdef PARAGON860
	char fn[128];

	strcpy(fn,"/etc/array/");
#else
	char fn[128] = "/etc/array/";
#endif

	if ( ( fd = open( strcat( fn, s ), O_RDONLY ) ) != -1 )
		_enabled_ = TRUE;
	else
		_enabled_ = FALSE;
	close( fd );
	return( _enabled_ );
}
