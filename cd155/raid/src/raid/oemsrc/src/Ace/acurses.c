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
  Name:		$RCSfile: acurses.c,v $
  Version:	$Id: acurses.c,v 1.3.4.1 1995/06/11 22:19:32 kat Exp $
  Title:	Curses Initialization
  Revision:	$Revision: 1.3.4.1 $
  Update Date:	$Date $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272

  COPYRIGHT 1992, NCR Corporation

  Description:	This module contains curses and screen initialization
		routines.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: acurses.c,v $
 * Revision 1.3.4.1  1995/06/11  22:19:32  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.3  1994/12/13  23:58:06  richardg
 * updating after copyright messaged.
 *
 * Revision 1.2  1993/03/04  20:07:17  richardg
 * modified the algorithm that prints the drive status to align from right to
 * left.  This conforms with the physical order of the drives on the front of
 * the Paragon.
 *
 * Revision 1.1  1992/12/28  18:29:41  richardg
 * Initial revision
 *
 * Revision 1.3  1992/05/26  15:13:12  root
 * sdpr u-120 - Several changes as result of user validation testing.
 *
 * Revision 1.2  1992/04/29  12:43:17  root
 * sdpr u-109  Added message per DQA - bem
 *
 * Revision 1.1  1992/03/18  14:00:17  bmyers
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
 ***			      EXTERNAL REFERENCES			    ***
 *****************************************************************************/
line_ent_t	W1[W1LINES];
line_ent_t	W2[W2LINES];
line_ent_t	W3[W3LINES];
line_ent_t	W4[W4LINES];
line_ent_t	W5[W5LINES];
line_ent_t	W6[W6LINES];
line_ent_t	W7[W7LINES];
line_ent_t	W8[W8LINES];
line_ent_t	W9[W9LINES];
void		display_title ( void );
extern grptbl_t grptbl;
/******************************************************************************
 ***			      VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
static int 	savindx = 0;
static WINDOW 	*savscrn[MAX_WINDOWS];
/******************************************************************************
 ***			      INTERNAL PROCEDURES			    ***
 *****************************************************************************/
static void	init_all ( void );
static void	init_w1 ( void );
static void	init_w2 ( void );
static void	init_w3 ( void );
static void	init_w4 ( void );
static void	init_w5 ( void );
static void	init_w6 ( void );
static void	init_w7 ( void );
static void	init_w8 ( void );
static void	init_w9 ( void );
static void	partition ( void );
static void	paint_all ( void );
/******************************************************************************
 ***			       ENTRY DEFINITIONS			    ***
 *****************************************************************************/
void paint_window ( line_ent_t *wptr, int nlines );
void init_curses ( void );
void end_curses ( void );
void paint_all ( void );

/*========================================================================*/
void init_curses ( void )
/*========================================================================*/
{
	initscr ();
	cbreak ();
	keypad ( stdscr, TRUE );
	noecho ();
	nonl();
	init_all ();
}

/*========================================================================*/
void end_curses ( void )
/*========================================================================*/
{
	clear ();
	refresh ();
	endwin ();
}

/*========================================================================*/
static void init_all ( void )
/*========================================================================*/
{
	init_w1 ();
	init_w2 ();
	init_w3 ();
	init_w4 ();
	init_w5 ();
	init_w6 ();
	init_w7 ();
	init_w8 ();
	init_w9 ();
	display_title ();
	mvaddstr ( 0, 4, "Logical View (not physical)");	/* sdpr u-109 */
	partition ();
}

/*========================================================================*/
void paint_all ( void )
/*========================================================================*/
{
	paint_window ( W4, W4LINES );
	paint_window ( W7, W7LINES );
	paint_window ( W8, W8LINES );
}

/*========================================================================*/
void paint_window ( line_ent_t *wptr, int nlines )
/*========================================================================*/
{
	int	i;

	for ( i = 0; i < nlines; ++i ) {
		move ( wptr->row, wptr->col );
		if ( wptr->bold == BLANK )
			continue;
		if ( wptr->bold == ON )
			attron ( A_REVERSE );
		addstr ( wptr->data );
		attroff ( A_REVERSE );
		wptr++;
	}
#ifdef ASCII
	move ( 23, 79 );
#else
	move ( 24, 79 );
#endif
	refresh ();
}

/*========================================================================*/
static void init_w1 ( void )
/*========================================================================*/
{
	mvaddstr ( W1R1, W1COL + 6, "CHANNEL" );
#ifdef PARAGON860
	mvaddstr ( W1R2, W1COL + 3, "5  4  3  2  1" );
#else
	mvaddstr ( W1R2, W1COL + 3, "1  2  3  4  5" );
#endif
	mvaddstr ( W1R3, W1COL,  "0" );
	mvaddstr ( W1R4, W1COL,  "1" );
	mvaddstr ( W1R5, W1COL,  "2" );
	mvaddstr ( W1R6, W1COL - 6,  "SCSI  3" );
	mvaddstr ( W1R7, W1COL - 5,  "ID   4" );
	mvaddstr ( W1R8, W1COL,  "5" );
	mvaddstr ( W1R9, W1COL, "6" );
	mvaddstr ( W1R10, W1COL, "7" );
}

/*========================================================================*/
static void init_w2 ( void )
/*========================================================================*/
{
	int 	i;
#ifdef PARAGON860
	for ( i = 0; i < W2LINES; i++ ) {
		W2[i].bold = OFF;
		W2[i].row = ( i / 5 ) + W2R0 - 1;
		W2[i].col = (12 + W2COL) - ( ( i % 5 ) * 3 ) ;
#else
	for ( i = 0; i < W2LINES; i++ ) {
		W2[i].bold = OFF;
		W2[i].row = ( i / 5 ) + W2R0 - 1;
		W2[i].col = ( ( i % 5 ) * 3 ) + W2COL;
#endif
	}
}

/*========================================================================*/
static void init_w3 ( void )
/*========================================================================*/
{
	mvaddstr ( W3R1, W3COL + 3, "STATUS" );
	mvaddstr ( W3R2, W3COL, "0  Optimal" );
	mvaddstr ( W3R3, W3COL, "1  Non-existent" );
	mvaddstr ( W3R4, W3COL, "2  Spare" );
	mvaddstr ( W3R5, W3COL, "3  Failed" );
	mvaddstr ( W3R6, W3COL, "4  Replaced" );
	mvaddstr ( W3R7, W3COL, "5  Warning" );
	mvaddstr ( W3R8, W3COL, "6  Mismatch" );
	mvaddstr ( W3R9, W3COL, "7  Controller" );
	mvaddstr ( W3R10, W3COL, "8  Formatting" );
	mvaddstr ( W3R11, W3COL, "9  Wrong drive" );
}

/*========================================================================*/
static void init_w4 ( void )
/*========================================================================*/
{
	line_ent_t *p = &W4[0];
	
	p->row = W4R0;
	p->col = W4COL - 1;
	p->bold = ON;
	sprintf ( p->data, "< DRIVE WINDOW >" );
}

/*========================================================================*/
static void init_w5 ( void )
/*========================================================================*/
{
	mvaddstr ( W5R0, W5COL + 4, "MODIFY" );
	mvaddstr ( W5R2, W5COL, "A - Add" );
	mvaddstr ( W5R3, W5COL, "D - Delete" );
	mvaddstr ( W5R4, W5COL, "F - Fail" );
	mvaddstr ( W5R5, W5COL, "R - Reconstruct" );
	mvaddstr ( W5R6, W5COL, "H - Help" );
#ifdef PARAGON860 /* add "monitor reconstruction" command */
	mvaddstr ( W5R7, W5COL, "M - Monitor" );
	mvaddstr ( W5R8, W5COL, "U - Update" );
	mvaddstr ( W5R9, W5COL, "Q - Quit" );
#else
	mvaddstr ( W5R7, W5COL, "Q - Quit" );
#endif
}

/*========================================================================*/
static void init_w6 ( void )
/*========================================================================*/
{
#ifdef PARAGON860 /*adjust spacing to accomodate h/w zipcode display */
	mvaddstr ( W6R0,W6COL, "GRP LUN  STATUS   RAID   DEVICE   SPACE(MB)" );
#else
	mvaddstr ( W6R0,W6COL, "GRP LUN  STATUS    RAID   DEVICE   SPACE(MB)" );
#endif
}

/*========================================================================*/
void init_w7 ( void )
/*========================================================================*/
{
	int		i;
	line_ent_t	*p = &W7[0];
	
	for  ( i = 0; i < W7LINES; i++, ++p ) {
		p->row = 1 + i;
		p->col = W7COL;
		p->bold = BLANK;
	}
}

/*========================================================================*/
static void init_w8 ( void )
/*========================================================================*/
{
	line_ent_t *p = &W8[0];
	
	p->row = W8R0;
	p->col = W8COL + 3;
	sprintf ( p->data, "< LOGICAL UNIT WINDOW >" );
}

/*========================================================================*/
static void init_w9 ( void )
/*========================================================================*/
{
	mvaddstr ( W9R0, W9COL, "C - Create" );
	mvaddstr ( W9R1, W9COL, "D - Delete" );
	mvaddstr ( W9R2, W9COL, "M - Modify/Display" );
	mvaddstr ( W9R3, W9COL, "    parameters" );
	mvaddstr ( W9R0, W9COL + 21, "F - Format" );
	mvaddstr ( W9R1, W9COL + 21, "H - Help" );
#ifdef PARAGON860 /* display the udate command in the menu */
        mvaddstr ( W9R2, W9COL + 21, "U - Update" );
        mvaddstr ( W9R3, W9COL + 21, "Q - Quit" );
#else
        mvaddstr ( W9R2, W9COL + 21, "Q - Quit" );
#endif
}

/*========================================================================*/
static void partition ( void )
/*========================================================================*/
{
	int i;

	move ( HORZ1, 0 );
	for ( i = 0; i < VERT1; i++ )
		printw ( "%c", ( u_char ) HORZL );
/*
	move ( 1, 0 );
	for ( i = 0; i < VERT1; i++ )
		printw ( "%c", ( u_char ) HORZL );
*/
       move ( HORZ4, VERT1 );				/* horz line */
       for ( i = 0; i < VERT1 + 10; i++ )
		printw ( "%c", ( u_char ) HORZL );

#ifdef ASCII
       move ( 22, VERT1 );				/* horz line */
#else
       move ( 23, VERT1 );				/* horz line */
#endif
       for ( i = 0; i < VERT1 + 10; i++ )
		printw ( "%c", ( u_char ) HORZL );

#ifdef ASCII
	for ( i = 1; i < (HORZ1 -1); i++ ) {			/* vert line */
#else
	for ( i = 1; i < HORZ1; i++ ) {			/* vert line */
#endif
		move ( HORZ2 + i, HORZ3 );
		printw ( "%c", ( u_char ) VERTL );
	}

	for ( i = 0; i < MAXROW; i++ ) {		/* vert line */
		move ( i, VERT1 );
		printw ( "%c", ( u_char ) VERTL );
	}


	move ( HORZ1, VERT1 );
	printw ( "%c", ( u_char ) LCON );		/* left connect */
/*
	move ( 1, VERT1 );
	printw ( "%c", ( u_char ) LCON );
*/
	move ( HORZ4, VERT1 );				/* right connect */
	printw ( "%c", ( u_char ) RCON );

#ifdef ASCII
	move ( 22, VERT1 );				/* right connect */
#else
	move ( 23, VERT1 );				/* right connect */
#endif
	printw ( "%c", ( u_char ) RCON );
}
