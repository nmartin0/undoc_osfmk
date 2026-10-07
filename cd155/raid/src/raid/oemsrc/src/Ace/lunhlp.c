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
 ***				 LUNHLP.C				    ***
 ******************************************************************************
  Name:		$RCSfile: lunhlp.c,v $
  Version:	$Id: lunhlp.c,v 1.1.4.1 1995/06/11 22:19:57 kat Exp $
  Title:	
  Revision:	$Revision: 1.1.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:57 $
		(last change by: $Author: kat $)
  Programmer:	BTC
  Documents:	

  COPYRIGHT 1992, NCR Corporation

  Description:	
*/

#include <stdio.h>
#include "acurses.h"
#include <curses.h>

#define ACE_HELP_DIR	"/etc/array/acehelp/"

/*======================================================================*/
/*		Local Definitions - declarations			*/
/*======================================================================*/
static int	input;
static char	*Buffer;
static void paint_lun_screen ( void );

/*======================================================================*/
/*		External Definitions - Declarations			*/
/*======================================================================*/
extern void popbox ( int, int, int, int, char * );
extern void popclr ( void );
extern void get_dhelp ( char *, char * );


/*======================================================================*/
/* Function:  void pop_lun_help ( void )				*/
/*======================================================================*/

void pop_lun_help ( void )

{
	char HelpFileName [60];

	paint_lun_screen ();

	while ( TRUE ) {

		switch ( input = getch () ) {
		case 'n':
		case 'N':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "NAVLUN.HLP" );
			get_dhelp ( HelpFileName, "Navigating in ace" );
			break;

		case 'o':
		case 'O':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "OLUN.HLP" );
			get_dhelp ( HelpFileName, "Overview - Configuring A Logical Unit" );
			break;

		case 'c':
		case 'C':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "CLUN.HLP" );
			get_dhelp ( HelpFileName, "Creating A Logical Unit" );
			break;

		case 'd':
		case 'D':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "DLUN.HLP" );
			get_dhelp ( HelpFileName, "Deleting A Logical Unit" );
			break;

		case 'm':
		case 'M':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "MLUN.HLP" );
			get_dhelp ( HelpFileName, "Modifying/Displaying Parameters" );
			break;

		case 'f':
		case 'F':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "FLUN.HLP" );
			get_dhelp ( HelpFileName, "Formatting A Logical Unit" );
			break;

		case 'h':
		case 'H':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "HLUN.HLP" );
			get_dhelp ( HelpFileName, "Help Using The Help" );
			break;

		case 'q':
		case 'Q':
			popclr ();
			return;
		}
	}
}

/*======================================================================*/
/* Function:  static void paint_lun_screen ( void )			*/
/*======================================================================*/

static void paint_lun_screen ( void )

{
	int uly = 6;
	int ulx = 17;
	int lry = 18;
	int lrx = 62;
	popbox ( uly, ulx, lry, lrx, "< HELP FOR ""CONFIGURE LOGICAL UNIT"" >" );
	mvaddstr ( uly+2, ulx+3, "N - Navigating In Ace" );
	mvaddstr ( uly+3, ulx+3, "O - Overview - Configuring a Logical Unit" );
	mvaddstr ( uly+4, ulx+3, "C - Creating A Logical Unit" );
	mvaddstr ( uly+5, ulx+3, "D - Deleting A Logical Unit" );
	mvaddstr ( uly+6, ulx+3, "M - Modifying/Displaying Parameters" );
	mvaddstr ( uly+7, ulx+3, "F - Formatting A Logical Unit" );
	mvaddstr ( uly+8, ulx+3, "H - Help Using The Help" );
	mvaddstr ( uly+9, ulx+3, "Q - Quit" );
	mvaddstr ( uly+11, ulx+6, "Enter a Help option, or \"q\" to quit:" );
	move ( uly+11, ulx+43 );
	refresh ();
}
