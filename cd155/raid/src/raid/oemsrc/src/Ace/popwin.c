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
  Name:		$RCSfile: popwin.c,v $
  Version:	$Id: popwin.c,v 1.2.4.1 1995/06/11 22:20:03 kat Exp $
  Title:	Window poping mechanism
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:20:03 $
		(last change by: $Author: kat $)
  Programmer:	bem ( thanks to Charles Sharp for the concept )

  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272
		UNIX SYSTEM V RELEASE 4 Programmers Guide:
		User Interface ( FMLI and ETI )

  COPYRIGHT 1992, NCR Corporation

  Description:	This module manages the window creation and deletion.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: popwin.c,v $
 * Revision 1.2.4.1  1995/06/11  22:20:03  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:43  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:54  richardg
 * Initial revision
 *
 * Revision 1.2  1992/05/26  21:43:05  root
 * sdpr u-120 - Enabled keypad after popping new window
 *
 * Revision 1.1  1992/03/18  14:00:28  bmyers
 * Initial revision
 *
 */

/******************************************************************************
 ***				    INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "acurses.h"
#include <curses.h>
/******************************************************************************
 ***			      VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
static WINDOW	*savscrn[MAX_WINDOWS];
static int	savindx = 0;
/******************************************************************************
 ***			      INTERNAL PROCEDURES			    ***
 *****************************************************************************/
static void	popopn ( void );
static int	stplen ( char * );
/******************************************************************************
 ***			       ENTRY DEFINITIONS			    ***
 *****************************************************************************/
void	popbox ( int cstart, int rstart, int rend, int cend, char * heading );
void	popclr ( void );

/*==========================================================================*/
void popbox ( int cstart, int rstart, int rend, int cend, char * heading )
/*==========================================================================*/
{
	int i, j, k, xhline, xvline;
	char buf[80];

	popopn ();

	xhline = ( u_char ) HORZL;
	xvline = ( u_char ) VERTL;
	attron ( A_REVERSE );
	move ( cstart, rstart );
	for ( i = rstart; i <= cend; i++ )
		printw ( "%c", ( u_char ) xhline );

	move ( rend, rstart );
	for ( i = rstart; i <= cend; i++ )
		printw ( "%c", ( u_char ) xhline );

	for ( i = cstart + 1; i < rend; i++ ) {
		move ( i, rstart );
		printw ( "%c", ( u_char ) xvline );
		move ( i, cend );
		printw ( "%c", ( u_char ) xvline );
	}

	move ( cstart, rstart );
	printw ( "%c", ( u_char ) ULC );
	move ( cstart, cend );
	printw ( "%c", ( u_char ) URC );
	move ( rend, rstart );
	printw ( "%c", ( u_char ) BLC );
	move ( rend, cend );
	printw ( "%c", ( u_char ) BRC );
	attroff ( A_REVERSE );

	j = cend - rstart - 2;
	for ( i = 0; i < j; buf[i++] = HORZL );

	buf[j] = 0;
	if ( k = stplen ( heading ) )
		strncpy ( buf + ( ( j-k ) / 2 ), heading, k );

	attron ( A_REVERSE );
	mvaddstr ( cstart, rstart + 2, buf );
	attroff ( A_REVERSE );

	for ( i = cstart + 1; i < rend; i++ ) {
		move ( i, rstart + 1 );
		for ( j = rstart + 1; j < cend; j++ )
			printw ( "%c", ( u_char ) ' ' );
	}
	return;
}

/*==========================================================================*/
void popclr ( void )
/*==========================================================================*/
{
	delwin ( stdscr );
	stdscr = savscrn[--savindx];
	touchwin ( stdscr );
	refresh ();
	return;
}

/*==========================================================================*/
static void popopn ( void )
/*==========================================================================*/
{
	savscrn[savindx] = stdscr;
	stdscr = newwin ( 0, 0, 0, 0 );
	overwrite ( savscrn[savindx], stdscr );
	clearok ( stdscr, FALSE );
	keypad ( stdscr, TRUE );
	savindx++;
	return;
}

/*==========================================================================*/
static int stplen ( char * heading )
/*==========================================================================*/
{
	if ( heading != (char *) NULL )
		return strlen ( heading );
	else
		return 0;
}
