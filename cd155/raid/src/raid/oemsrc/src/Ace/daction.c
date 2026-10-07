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
 ***				 IDENTIFICATION				    ***
 ******************************************************************************
  Name:		$RCSfile: daction.c,v $
  Version:	$Id: daction.c,v 1.2.4.1 1995/06/11 22:19:44 kat Exp $
  Title:	Drive View Actions
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272

  COPYRIGHT 1992, NCR Corporation

  Description:	This module contains the actions required of the drive view.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: daction.c,v $
 * Revision 1.2.4.1  1995/06/11  22:19:44  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:23  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:47  richardg
 * Initial revision
 *
 * Revision 1.12  1992/07/13  18:56:44  root
 * sdpr u-141 Added call to refresh in two places. - bem
 *
 * Revision 1.11  1992/06/23  19:02:25  root
 * sdpr u-133 - Added logic to test for degraded LUN after reconstruction
 * complete. - bem
 * sdpr u-130 - Changed sequence of logic (swapped three lines) in drive_fail
 * function. - bem
 *
 * Revision 1.10  1992/06/16  18:01:09  root
 * sdpr u-125 - Added logic so drive in warning can be failed. - bem
 *
 * Revision 1.9  1992/05/26  15:23:24  root
 * sdpr u-120 Lengthened time before doing update from 2 to 3 seconds.
 *
 * Revision 1.8  1992/04/23  16:24:00  root
 * sdpr u-106 Added logic to not update screen if reconstruction selected
 * and configuration is RAID level 0.
 *
 * Revision 1.7  1992/04/03  22:58:34  root
 * sdpr #73 Fix multi sublun problem broken by previous revision.
 *
 * Revision 1.6  1992/04/03  22:27:16  root
 * sdpr #73 - Added reconstruction complete messages as per Pubs request.
 *
 * Revision 1.5  1992/04/02  23:12:07  root
 * sdpr #74 - Added data loss warning before failing drive.
 *
 * Revision 1.4  1992/03/27  13:17:06  root
 * Added Pubs message if RAID 0 and reconstruct selected.
 *
 * Revision 1.3  1992/03/25  17:21:30  root
 * Message text change for Pubs.
 *
 * Revision 1.2  1992/03/20  17:10:29  bmyers
 * Added locking parameter to mode_select call.
 *
 * Revision 1.1  1992/03/18  14:00:22  bmyers
 * Initial revision
 *
 */

/******************************************************************************
 ***				    INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "acurses.h"
#include "scsidefs.h"
#include "stddefs.h"
#include "dau_err.h"
#include <curses.h>
/******************************************************************************
 ***			      VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
extern int			current_drive;
extern physical_array_page_t	page2a;
extern logical_array_page_t	page2b;
extern line_ent_t		W2[];
extern grptbl_t			grptbl;
/******************************************************************************
 ***			      INTERNAL PROCEDURES			    ***
 *****************************************************************************/
static int		pop_reconfmt ( void );
static void		pop_illegal_del ( void );
static void		pop_illegal_add ( void );
static void		pop_illegal_fail ( void );
static void		pop_illegal_replace ( void );
static int		pop_fail_data_loss ( void );
#ifndef PARAGON860 /* add "monitor reconstruction" command */
static int		display_recon_progress ( int );
#endif
static void		pop_add_drive ( void );
#ifndef PARAGON860 /* add "monitor reconstruction" command */
static void		pop_recon_complete ( int );
#endif
/******************************************************************************
 ***			      EXTERNAL PROCEDURES			    ***
 *****************************************************************************/
extern void		popbox ( int, int, int, int, char * );
extern void		popclr ( void );
extern int		lun_containing_drv ( int );
extern u_int32		get_four_bytes ( u_char * );
extern void		pop_debug ( void );
extern void		highlight_drv_to_add ( void );
/******************************************************************************
 ***			       ENTRY DEFINITIONS			    ***
 *****************************************************************************/
int replace_drive ( void );
int fail_drive ( void );
int delete_drive ( void );
int add_drive ( void );
int show_recon_percent ( int );
#ifdef PARAGON860 /* add "monitor reconstruction" command */
int display_recon_progress( int );
int pop_recon_complete (int);
#endif

/*==========================================================================*/
int replace_drive ( void )
/*==========================================================================*/
{
	int	rindex, index, ch, id, action, status;
	int	i = 0;

	while ( W2[i].bold == OFF )
		i++;
	if ( ( W2[i].status & 0x07 ) != FAILED ) {
		pop_illegal_replace ();
		return ( NONE );
	}
	ch = ( current_drive >> 4 ) & 0x07;
	id = current_drive & 0x07;
	rindex = index = ( id * 5 ) + ch;
	if ( index > 4 )
		index += ( id * 10 );

	action = pop_reconfmt ();

	if ( action == FALSE )
		page2a.Drive_Status[index] = ( u_char ) REPLACE_DRIVE;
	else
		page2a.Drive_Status[index] = ( u_char ) REPLACE_DRIVE_FORMAT;

	status = mode_select ( 0, &page2a, sizeof ( physical_array_page_t ),
			PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS, FALSE );

	if ( status ) {
		ace_error ( SCSI_CONDITION, MODE_SELECT_ERROR );
		return ( ALL );
	}

	status = display_recon_progress ( rindex );
	if ( status != QUIT  && status == OPTIMAL )
		pop_recon_complete ( action );

	return ( ALL );
}

/*==========================================================================*/
int delete_drive ( void )
/*==========================================================================*/
{
	int	index, ch, id, status;
	int	i = 0;

	while ( W2[i].bold == OFF )
		i++;
	if ( W2[i].lun != -1 && W2[i].status != SPARE ) {
		pop_illegal_del ();
		return ( NONE );
	}
	ch = ( current_drive >> 4 ) & 0x07;
	id = current_drive & 0x07;
	index = ( id * 5 ) + ch;
	if ( index > 4 )
		index += ( id * 10 );
	page2a.Drive_Status[index] = ( u_char ) DELETE_DRIVE;

	status = mode_select ( 0, &page2a, sizeof ( physical_array_page_t ),
			PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS, FALSE );

	if ( status ) {
		ace_error ( SCSI_CONDITION, MODE_SELECT_ERROR );
		return ( ALL );
	}
	return ( ALL );
}

/*==========================================================================*/
int fail_drive ( void )
/*==========================================================================*/
{
	int	index, ch, id, status, lun;
	int	i = 0;

	while ( W2[i].bold == OFF )
		i++;
	if ( W2[i].status != OPTIMAL_DRV && W2[i].status != WARNING ) {
		pop_illegal_fail ();
		return ( NONE );
	}
	lun = W2[i].lun;
	ch = ( current_drive >> 4 ) & 0x07;
	id = current_drive & 0x07;
	index = ( id * 5 ) + ch;
	if ( index > 4 )
		index += ( id * 10 );

	if ( ( status = pop_fail_data_loss () ) == QUIT )
		return ( NONE );

	page2a.Drive_Status[index] = ( u_char ) FAIL_DRIVE;

	status = mode_select ( lun, &page2a, sizeof ( physical_array_page_t ),
			PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS, TRUE );

	if ( status == LOCK_ERROR )
		return ( NONE );

	if ( status )
		ace_error ( SCSI_CONDITION, MODE_SELECT_ERROR );

	return ( ALL );
	
}

/*==========================================================================*/
int add_drive ( void )
/*==========================================================================*/
{
	int	status, index, ch, id;
	int	i = 0;

	pop_add_drive ();

	highlight_drv_to_add ();

	popclr ();

	while ( W2[i].bold == OFF )
		i++;

	if ( W2[i].status != NON_EXIST ) {
		pop_illegal_add ();
		return ( NONE );
	}
	ch = ( current_drive >> 4 ) & 0x07;
	id = current_drive & 0x07;
	index = ( id * 5 ) + ch;
	if ( index > 4 )
		index += ( id * 10 );
	page2a.Drive_Status[index] = ( u_char ) ADD_DRIVE;

	status = mode_select ( 0, &page2a, sizeof ( physical_array_page_t ),
			PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS, FALSE );

	if ( status )
		ace_error ( SCSI_CONDITION, MODE_SELECT_ERROR );

	return ( ALL );
}

/*==========================================================================*/
int display_recon_progress ( int index )
/*==========================================================================*/
{
	int		i, stat;
	int		grp = grp_containing_drv ( index );
	grp_entry_t	*g = &grptbl.group[grp];

	for ( i = 0; i < g->nbr_real_luns_in_grp; i++ ) {
		if ( g->lun_table[i].raid_lvl != 0 ) {
   			/* allow time for the dac to do house keeping */
			/* so the display gets updated properly       */
			sleep ( 3 );
			get_array_info ( ALL );
			setup_current_view ();
#ifdef PARAGON860 /* force a refresh */
			wrefresh(curscr);
#endif
		}
		stat = show_recon_percent ( g->lun_table[i].lun_nbr );
		if ( stat == QUIT )
			return ( stat );
	}
	return ( stat );
}

/*==========================================================================*/
int show_recon_percent ( int lun )
/*==========================================================================*/
{
	u_int32			size, reconsize;
	int			stat, pct, rval;
	logical_array_page_t 	*p = &page2b;

	rval = NONE;
	popbox ( 7, 20, 15, 60, "< RECONSTRUCTION >" );
	mvprintw ( 9, 33, "Logical Unit %x", lun );

	while ( TRUE ) {
		if ( ( stat = get_page2b ( lun ) ) != OK ) {
			ace_error ( SCSI_CONDITION, MODE_SENSE_ERROR );
			break;
		}
		rval = p->Action_Status;
		if ( rval == REPLACED_DRV_FORMATING ||
				rval == REPLACED_RD0_FORMATING )
			mvaddstr ( 11, 36, "Formatting" );

		if ( rval == RECONSTLUN ) {
			size = get_four_bytes ( p->LUN_Number_of_Blocks );
			reconsize = get_four_bytes ( p->Block_Reconstructed );
			reconsize *= 100;
			pct = reconsize / size;
			mvprintw ( 11, 25, "Reconstruction:  %3d", pct );
			addstr ( " % complete" );
		}
		if ( rval == OPTIMAL || rval == DEAD || rval == DEGRADED ) 
			break;
#ifdef ASCII
		move ( 23, 0 );
#else
		move ( 24, 79 );
#endif
		refresh ();
		stat = monitor_input ( 1 );
		if ( stat == 'q' ) {
			rval = QUIT;
			break;
		}
	}
	if ( rval != QUIT ) {
		if ( p->RAID_Level != 0 && rval == OPTIMAL ) {
			mvaddstr ( 11, 25, "Reconstruction:  100 % complete" );
#ifdef ASCII
			move ( 23, 0 );
#else
			move ( 24, 79 );
#endif
			refresh ();
		}
		sleep ( 2 );	/* allow dac time to update itself so when we */
		 		/* re-display the screen, things are correct  */
	}
	popclr ();
	return ( rval );
}

/*==========================================================================*/
static int pop_reconfmt ( void )
/*==========================================================================*/
{
	int		answer;

	popbox ( 8, 16, 17, 66, "< RECONSTRUCTION >" );
	do {
		mvaddstr ( 11, 19, "Format before reconstruction? [ n ]" );
		mvaddstr ( 13, 19, "NOTE:  'y' forces a drive format, and 'n'");
		mvaddstr ( 14, 26, "gives the array controller the option" ); 
		mvaddstr ( 15, 26, "to format the drive if needed.");
		move ( 11, 51 );
		refresh ();
		answer = getch ();
		if ( answer == '\n' )
			answer = 'n';
	} while ( tolower ( answer ) != 'y' && tolower ( answer ) != 'n' );
	popclr ();
	if ( tolower ( answer ) == 'y' )
		return ( TRUE );
	else
		return ( FALSE );
}

/*==========================================================================*/
static void pop_illegal_del ( void )
/*==========================================================================*/
{
	popbox ( 8, 19, 18, 63, "< ERROR >" );
	mvaddstr ( 11, 26, "The drive selected is part of" );
	mvaddstr ( 12, 26, "an existing LUN.  It cannot" );
	mvaddstr ( 13, 26, "be deleted." );
	mvaddstr ( 15, 26, "Press any key to continue.");
	refresh ();
	getch ();
	popclr ();
}

/*==========================================================================*/
static void pop_illegal_add ( void )
/*==========================================================================*/
{
	popbox ( 8, 19, 17, 63, "< ERROR >" );
	mvaddstr ( 11, 26, "The add drive action is only" );
	mvaddstr ( 12, 26, "legal on a non-existent drive." );
	mvaddstr ( 15, 26, "Press any key to continue.");
	refresh ();
	getch ();
	popclr ();
}

/*==========================================================================*/
static void pop_illegal_fail ( void )
/*==========================================================================*/
{
	popbox ( 8, 19, 17, 63, "< ERROR >" );
	mvaddstr ( 11, 26, "The fail drive action is only" );
	mvaddstr ( 12, 26, "legal on an optimal drive or" );
	mvaddstr ( 13, 26, "a drive in warning." );
	mvaddstr ( 15, 26, "Press any key to continue.");
	refresh ();
	getch ();
	popclr ();
}

/*==========================================================================*/
static void pop_illegal_replace ( void )
/*==========================================================================*/
{
	popbox ( 8, 19, 17, 63, "< ERROR >" );
	mvaddstr ( 11, 26, "The reconstruct drive action is" );
	mvaddstr ( 12, 26, "only legal on a failed drive." );
	mvaddstr ( 15, 26, "Press any key to continue.");
	refresh ();
	getch ();
	popclr ();
}

/*==========================================================================*/
static void pop_add_drive ( void )
/*==========================================================================*/
{
	popbox ( 0, 35, 9, 78, "< ADD DRIVE >" );
	mvaddstr ( 3, 38, "Use the SPACE bar to highlight the" );
	mvaddstr ( 4, 38, "desired drive. When the correct" );
	mvaddstr ( 5, 38, "drive is highlighted, press <ENTER>" );
	mvaddstr ( 6, 38, "and the drive will be added." );
	refresh ();
}

/*==========================================================================*/
#ifdef PARAGON860
int pop_recon_complete ( int action )
#else
static void pop_recon_complete ( int action )
#endif
/*==========================================================================*/
{
	int	ch;

	popbox ( 5, 18, 20, 60, "< INFORMATION >" );
	if ( action == TRUE )
		mvaddstr ( 8, 21, "The drive format is complete." );
	mvaddstr ( 9, 21, "The array is restored." );
	mvaddstr ( 12, 21, "NOTE: If you have a RAID 0 array," );
	mvaddstr ( 13, 21, "copy the data from the back-up media" );
	mvaddstr ( 14, 21, "(file-by-file) to the array." );
	mvaddstr ( 17, 21, "Press any key to continue." );
	refresh ();
	ch = getch ();
	popclr ();
}

/*==========================================================================*/
static int pop_fail_data_loss ( void )
/*==========================================================================*/
{
	int		answer;

	popbox ( 8, 19, 18, 64, "< WARNING >" );
	do {
		mvprintw ( 11, 26, "If you fail a drive, you may lose" );
		mvaddstr ( 12, 26, "data redundancy or data." );
		mvaddstr ( 14, 26, "Press 'c' to continue." );
		mvaddstr ( 15, 26, "Press 'q' to quit.");
		refresh ();
		answer = getch ();
	} while ( tolower ( answer ) != 'q' && tolower ( answer ) != 'c' );
	popclr ();
	if ( tolower ( answer ) == 'q' )
		return ( QUIT );
	else
		return ( CONTINUE );
}

