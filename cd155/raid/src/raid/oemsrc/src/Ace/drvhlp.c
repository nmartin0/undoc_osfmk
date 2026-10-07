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
 ***				 DRVHLP.C				    ***
 ******************************************************************************
  Name:		$RCSfile: drvhlp.c,v $
  Version:	$Id: drvhlp.c,v 1.2.4.1 1995/06/11 22:19:46 kat Exp $
  Title:	
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:46 $
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
static void paint_drive_screen ( void );

/*======================================================================*/
/*		External Definitions - Declarations			*/
/*======================================================================*/
extern void popbox ( int, int, int, int, char * );
extern void popclr ( void );
extern void pop_help_box ( char * );
extern void get_dhelp ( char *, char * );


/*======================================================================*/
/* Function:   void pop_drive_help ( void )				*/
/*======================================================================*/

void pop_drive_help ( void )

{
	char  HelpFileName [60];

	paint_drive_screen ();

	while ( TRUE ) {

		switch ( input = getch () ) {
		case 'n':
		case 'N':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "NAVDRST.HLP" );
			get_dhelp ( HelpFileName, "Navigating In Ace" );
			break;

		case 'o':
		case 'O':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "ODRVST.HLP" );
			get_dhelp ( HelpFileName, "Overview - Modifying/Displaying The Drive Status" );
			break;

		case 's':
		case 'S':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "DISDRST.HLP" );
			get_dhelp ( HelpFileName, "Status Of Drives" );
			break;

#ifdef PARAGON860 /* changed meaning of the letter "M" */
		case 'c':
		case 'C':
#else
		case 'm':
		case 'M':
#endif
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "MODDRST.HLP" );
			get_dhelp ( HelpFileName, "Modifying Drive Status" );
			break;

		case 'a':
		case 'A':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "ADRV.HLP" );
			get_dhelp ( HelpFileName, "Adding A Drive" );
			break;

		case 'd':
		case 'D':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "DDRV.HLP" );
			get_dhelp ( HelpFileName, "Deleting A Drive" );
			break;

		case 'f':
		case 'F':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "FDRV.HLP" );
			get_dhelp ( HelpFileName, "Failing A Drive" );
			break;

		case 'r':
		case 'R':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "RDRV.HLP" );
			get_dhelp ( HelpFileName, "Reconstructing Drive Data" );
			break;

		case 'h':
		case 'H':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "HDRV.HLP" );
			get_dhelp ( HelpFileName, "Help Using The Help" );
			break;

#ifdef PARAGON860 /* add "monitor reconstruction" command */
		case 'm':
		case 'M':
			strcpy ( HelpFileName, ACE_HELP_DIR );
			strcat ( HelpFileName, "MRECON.HLP" );
			get_dhelp ( HelpFileName, "Monitor Reconstruction" );
			break;
#endif
		case 'q':
		case 'Q':
			popclr ();
			return;
		}
	}
}

/*======================================================================*/
/* Function:  static void paint_drive_screen ( void )			*/
/*======================================================================*/

static void paint_drive_screen ( void )

{
	int uly = 5;
	int ulx = 10;
	int lry = 19;
	int lrx = 67;
	popbox ( uly, ulx, lry, lrx, "< HELP FOR \"DRIVE STATUS\" >" );
	mvaddstr ( uly+2, ulx+3, "N - Navigating In Ace" );
	mvaddstr ( uly+3, ulx+3, "O - Overview - Modifying/Displaying The Drive Status" );
	mvaddstr ( uly+4, ulx+3,  "S - Status Of Drives" );
#ifdef PARAGON860 /* used the letter "M" for the Monitor command */
	mvaddstr ( uly+5, ulx+3,  "C - Changing/Modifying Drive Status" );
#else
	mvaddstr ( uly+5, ulx+3,  "M - Modifying Drive Status" );
#endif
	mvaddstr ( uly+6, ulx+3,  "A - Adding A Drive" );
	mvaddstr ( uly+7, ulx+3,  "D - Deleting A Drive" );
	mvaddstr ( uly+8, ulx+3,  "F - Failing A Drive" );
	mvaddstr ( uly+9, ulx+3,  "R - Reconstructing Drive Data" );
	mvaddstr ( uly+10, ulx+3, "H - Help Using The Help" );
#ifdef PARAGON860 /* add "monitor reconstruction" command */
	mvaddstr ( uly+11, ulx+3, "M - Monitor Drive Reconstruction" );
	mvaddstr ( uly+12, ulx+3, "Q - Quit" );
#else
	mvaddstr ( uly+11, ulx+3, "Q - Quit" );
#endif
	mvaddstr ( uly+13, ulx+6,    "Enter a Help option, or \"q\" to quit: " );
	move ( uly+13, ulx+43 );
	refresh ();
}
