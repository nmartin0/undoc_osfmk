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
  Name:		$RCSfile: laction.c,v $
  Version:	$Id: laction.c,v 1.2.4.1 1995/06/11 22:19:55 kat Exp $
  Title:	LUN View Actions
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:55 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272

  COPYRIGHT 1992, NCR Corporation

  Description:	This module contains the format and delete actions
		required of the LUN view.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log $
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
extern int			current_lun;
extern int			current_group;
extern logical_array_page_t	page2b;
extern int			Exit_Status;
/******************************************************************************
 ***			      EXTERNAL PROCEDURES			    ***
 *****************************************************************************/
extern void		popbox ( int, int, int, int, char * );
extern void		popclr ( void );
extern int		lun_containing_drv ( int );
extern void		pop_debug ( void );
extern void		pop_illegal_group ( void );
/******************************************************************************
 ***			       ENTRY DEFINITIONS			    ***
 *****************************************************************************/
int			delete_lun ( int );
int			format_lun ( int, int );
int			pop_data_loss ( char * );
/*==========================================================================*/
int delete_lun ( int lun )
/*==========================================================================*/
{
	logical_array_page_t	*p = &page2b;
	int			status;

	if ( lun == -1 ) {
		pop_illegal_group ();
		return ( NONE );
	}

	if ( pop_data_loss ( "Deletion" ) == QUIT )
		return ( NONE );

	if ( ( status = get_page2b ( lun ) ) == ERR ) {
		ace_error ( SCSI_CONDITION, MODE_SENSE_ERROR );
		return ( NONE );
	}

	p->Action_Status = DELETE_LUN;

	status = mode_select ( lun, p, sizeof ( logical_array_page_t ),
			PAGE_FORMAT_SCSI_2, SAVE_PARAMETERS, TRUE );

	if ( status == LOCK_ERROR )
		return ( NONE );

	if ( status )
		ace_error ( SCSI_CONDITION, MODE_SELECT_ERROR );

	Exit_Status = 1;

	return ( ALL );
}

/*==========================================================================*/
int format_lun ( int lun, int flag )
/*==========================================================================*/
{
	int		status;

	if ( flag == FROM_MAINMENU )
		if ( pop_data_loss ( "Formatting" ) == QUIT )
			return ( NONE );

	popbox ( 9, 24, 13, 55 , "< INFORMATION >" );
	mvprintw ( 11, 28, "Formatting Logical Unit %x", lun );
	refresh ();
	status = format_unit ( lun, ( char * ) 0, 0, 0, 0, 0, 0 );

	popclr ();

	if ( status == LOCK_ERROR )
		return ( NONE );

	if ( status )
		ace_error ( SCSI_CONDITION, FORMAT_ERROR );

	return ( ALL );
}

/*==========================================================================*/
int pop_data_loss ( char * s )
/*==========================================================================*/
{
	int		answer;

	popbox ( 8, 19, 18, 64, "< WARNING >" );
	do {
		mvprintw ( 11, 26, "%s of the selected logical", s );
		mvaddstr ( 12, 26, "unit will result in loss of data." );
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
