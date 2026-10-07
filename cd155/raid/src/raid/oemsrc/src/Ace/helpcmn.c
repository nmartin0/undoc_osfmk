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
 ***				 HELPCOMN.C				    ***
 ******************************************************************************
  Name:		$RCSfile: helpcmn.c,v $
  Version:	$Id: helpcmn.c,v 1.1.4.1 1995/06/11 22:19:53 kat Exp $
  Title:	
  Revision:	$Revision: 1.1.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:53 $
		(last change by: $Author: kat $)
  Programmer:	BTC
  Documents:	

  COPYRIGHT 1992, NCR Corporation

  Description:	
*/

#include <stdio.h>
#include <fcntl.h>
#include <malloc.h>
#include "dau_err.h"
#include "acedefs.h"
#include "acurses.h"
#include <curses.h>

#define	drvhelp		"dhelp.txt"
#define	lunhelp		"lhelp.txt"

/*======================================================================*/
/*		Local Definitions - declarations			*/
/*======================================================================*/

typedef struct linep {
	char	*line;
} linep_t;

#ifdef ASCII
static line_ent_t	w10[22];
#else
static line_ent_t	w10[23];
#endif

static linep_t	ltbl[200];
static int	last_entry;
static char	*Buffer;

static int read_help_file ( char * );
static void init_w10 ( void );
static void display_help ( int, char * );
static void clear_w10 ( void );


/*======================================================================*/
/*		External Definitions - Declarations			*/
/*======================================================================*/
extern void popbox ( int, int, int, int, char * );

void get_dhelp ( char * help_fname, char * title )
{
	int		i, len, index, cnt;
	char		*p, *end;

	if ( ( len = read_help_file ( help_fname ) ) == ERR )
		return;

	index = cnt = 0;
	p = Buffer;
	end = Buffer + len -1;

	ltbl[index++].line = p;

	while ( p < Buffer + len ) {
		while ( *p != 10 && *p != 13 )
			++p;
		*p++ = '\0';
		if ( *p == 10 || *p == 13 )
			*p++ = '\0';
		cnt++;
#ifdef ASCII
		if ( cnt == 22 ) {
#else
		if ( cnt == 23 ) {
#endif
			ltbl[index++].line = p;
			cnt = 0;
		}
	}

	ltbl[index].line = NULL;
	last_entry = index;
	init_w10 ();

	display_help ( len, title );
	free ( Buffer );
}

static void display_help ( int len, char * title )
{
	int		index, input, i;
	char		*p;

	index = 0;

#ifdef ASCII
	popbox ( 0, 0, 23, 78, title );
#else
	popbox ( 0, 0, 24, 78, title );
#endif
	attron ( A_REVERSE );
#ifdef ASCII
	mvaddstr ( 23, 4, "\"n\" to move to next screen  \"p\" to move to previous screen  \"q\" to QUIT " );
#else
	mvaddstr ( 24, 2, " <PgDn> to move down one screen  <PgUp> to move up one screen  \"q\" to QUIT " );
#endif
	attroff ( A_REVERSE );
#ifdef PARRAGON860
	move ( 23, 1 );
#else
	move ( 24, 1 );
#endif
	keypad ( stdscr, TRUE );
	while ( TRUE ) {
		clear_w10 ();
		p = ltbl[index].line;
#ifdef ASCII
		for ( i = 0; i < 22; i++ ) {
#else
		for ( i = 0; i < 23; i++ ) {
#endif
			while ( *p == 0 )
				++p;
			if ( p >= Buffer + len )
				break;
			sprintf ( w10[i].data, "%s", p );
			w10[i].bold = OFF;
			while ( *p != 0 )
				++p;
		}
#ifdef ASCII
		paint_window ( w10, 22 );
#else
		paint_window ( w10, 23 );
#endif
		input = getch ();
/* for debug of key codes
		mvprintw ( 22, 30, "key %d %x    ", input,input ); refresh ();
*/
		switch ( input ) {
		case 'q':
			popclr ();
			return;

		case KEY_NPAGE:
		case 'n':
			if ( index + 1 == last_entry ) {
				beep ();
				continue;
			}
			index++;
			break;

		case KEY_PPAGE:
		case 'p':
			if ( index == 0 ) {
				beep ();
				continue;
			}
			index--;
			break;

		case 'e':
/* 		case KEY_END:        deleted by jerry */
			index = last_entry - 1;
			break;

		case 'h':
		case KEY_HOME:
			index = 0;
			break;

		default:
			beep ();
			break;
		}
	}
}

static void clear_w10 ( void )
{
	int	    i;

#ifdef ASCII
	for ( i = 0; i < 22; i++ ) {
#else
	for ( i = 0; i < 23; i++ ) {
#endif
	    mvaddstr ( i + 1, 1, "                                                                             " );
	    w10[i].bold = BLANK;
	}
	refresh ();
}

static void init_w10 ( void )
{
	int	i;

#ifdef ASCII
	for ( i = 0; i < 22; i++ ) {
#else
	for ( i = 0; i < 23; i++ ) {
#endif
		w10[i].row = i +1;
		w10[i].col = 1;
		w10[i].bold = OFF;
	}
} 


static int read_help_file ( char *file )
{
	int		i, handle;
	FILE		*fd;
	long		length;

	if ( (fd = fopen ( file, "rb" ) ) == NULL ) {
		ace_error ( GENERAL_FILE_OPEN_ERROR_MSG, GENERAL_FILE_OPEN_ERROR_MSG );
		return ( ERR );
	}
	
	if ( ( fseek ( fd, 0, SEEK_END ) ) == -1 ) {
		fclose ( fd );
		ace_error ( GENERAL_FILE_OPEN_ERROR_MSG, GENERAL_FILE_OPEN_ERROR_MSG );
		return ( ERR );
	}

	length = ftell ( fd );

	if ( ( fseek ( fd, 0, SEEK_SET ) ) == -1 ) {
		fclose ( fd );
		ace_error ( GENERAL_FILE_OPEN_ERROR_MSG, GENERAL_FILE_OPEN_ERROR_MSG );
		return ( ERR ); 
	}

	if ( ( Buffer = ( char * ) malloc ( ( size_t ) length ) ) == NULL ) {
		ace_error ( OUT_OF_MEMORY_MSG, OUT_OF_MEMORY_MSG );
		return ( ERR );
	}

	fread ( Buffer, ( size_t ) length, 1, fd );

	fclose ( fd );
	return ( ( int ) length );
}
