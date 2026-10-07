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
  Name:		$RCSfile: ace.c,v $
  Version:	$Id: ace.c,v 1.2.4.1 1995/06/11 22:19:26 kat Exp $
  Title:	Array Configuration Editor ( ace )
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:26 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272

  COPYRIGHT 1992, NCR Corporation

  Description:	This is the 'main' module for the ace utility.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: ace.c,v $
 * Revision 1.2.4.1  1995/06/11  22:19:26  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:57:59  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:17  richardg
 * Initial revision
 *
 * Revision 1.12  1992/07/14  19:12:00  root
 * sdpr u-140 removed work_on call from delete_lun case. bem
 *
 * Revision 1.11  1992/07/13  18:50:20  root
 * sdpr u-140 Added logic for 'Working' message. - bem
 *
 * Revision 1.10  1992/06/30  13:53:16  root
 * Added missing check to lun_view for access to debug - bem 6/30/92
 *
 * Revision 1.9  1992/06/16  18:04:19  root
 * sdpr u-126 - Added logic to check for LUN 0 at exit and, if not present,
 * display warning message. - bem
 *
 * Revision 1.8  1992/05/26  15:12:05  root
 * sdpr u-120 - Added arrow key input and logic to keep cursor on same
 * selection if screen updated manually or by time.
 *
 * Revision 1.7  1992/04/21  21:21:13  root
 * sdpr u-094 Added sigignore (SIGINT) statement.
 *
 * Revision 1.6  1992/03/23  23:09:20  bmyers
 * Added Exit_Status for sysadm
 *
 * Revision 1.5  1992/03/23  04:39:08  bchampag
 * added help stuff - BTC
 *
 * Revision 1.4  1992/03/21  20:06:21  bmyers
 * Added logic to deny access to ace if SDAC software not up to pre-determined
 * release level.
 *
 * Revision 1.3  1992/03/20  22:07:41  bmyers
 * Changed access to debug from page_down to F12
 *
 * Revision 1.2  1992/03/20  17:09:19  bmyers
 * Paramatized access to debug.
 *
 * Revision 1.1  1992/03/18  14:00:13  bmyers
 * Initial revision
 *
 */

/******************************************************************************
 ***				    INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "acedefs.h"
#include "stddefs.h"
#include "dau_err.h"
#include "sysio.h"
#include <signal.h>
#include <curses.h>
/******************************************************************************
 ***			      GLOBAL VARIABLES  			    ***
 *****************************************************************************/
int		current_view = LUN_VIEW;
int		last_view = -1;
int		current_drive = 0;
int		current_group = 0;
int		current_lun = 0;
int		update_needed = ALL;
int		Exit_Status = 0;
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode	IOBus;
#else
int		IOBus;
#endif
int		Argc;
char		**Argv;
#ifdef PARAGON_860
char		Devname[100];
#endif
grptbl_t	grptbl;
hw_zipcode	hw_addr;
/******************************************************************************
 ***			      VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
static int	input;
static int	access_to_debug = FALSE;
static int	access_to_prior_release = FALSE;
/******************************************************************************
 ***			      EXTERNAL PROCEDURES			    ***
 *****************************************************************************/
extern void	init_curses ( void );
extern void	end_curses ( void );
extern int	check_sdac_release ( int );
extern void	setup_current_view ( void );
extern void     highlight_next_lun ( int );
extern void	highlight_next_drive ( int, int );
extern void	highlight_next_drive_in_grp ( void );
extern int	replace_drive ( void );
extern int	fail_drive ( void );
extern int	delete_drive ( void );
extern int	add_drive ( void );
extern void	pop_drive_help ( void );
extern int	create_lun ( void );
extern int	pop_modify_form ( int );
extern void	get_array_info ( int );
extern int	delete_lun ( int );
extern int	modify_lun ( int );
extern int	format_lun ( int, int );
extern void	pop_lun_help ( void );
extern int	monitor_input ( long );
extern int	get_hl_lun_nbr ( void );
#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
extern hw_zipcode hc_ptol( hw_zipcode );
#else
extern int	hc_ptol ( hw_zipcode );
#endif
extern void 	pop_debug ( void );
extern void 	close_sysio ( void );
extern int	get_next_nonexistant_lun ( void );
extern void	work_on ( void );
extern void	work_off ( void );
extern char	*optarg;
extern int	sigignore ( int );
#ifdef PARAGON860 /* add "monitor reconstruction" command */
extern int      display_recon_progress ( int );
extern hw_zipcode zip_edit( u_char *, int *);
extern void     pop_recon_complete ( int );
#endif
/******************************************************************************
 ***			      INTERNAL PROCEDURES			    ***
 *****************************************************************************/
static void	drive_view ( int );
static void	lun_view ( int );
static void	primary_loop ( void );
static int	pop_no_lun_zero ( void );
/*===========================================================================*
/*  Function:  main ( argc, argv )                                           */
/*===========================================================================*/

main ( argc, argv )
int	argc;
char	*argv[];

{
	int		lun_zero_present = FALSE;

	Argc = argc;
	Argv = argv;

	sigignore ( SIGINT );

#ifdef PARAGON860
/*
 * check for superuser access
 */
	if (geteuid() != 0) {
	  fprintf(stderr, "%s must be run by superuser\n", argv[0]);
	  exit(-1);
	}

#endif
	init_curses ();

	if ( ( parse_argv () ) == ERR ) {
		end_curses ();
		exit ( Exit_Status );
	}

#ifdef UV4
	IOBus = hc_ptol ( (hw_zipcode) hw_addr );
#endif

	if ( check_sdac_release ( access_to_prior_release ) == OK ) {
		while ( lun_zero_present == FALSE ) {
			primary_loop ();
			if ( ( get_next_nonexistant_lun () ) == 0 ) {
				if ( pop_no_lun_zero () == QUIT )
					lun_zero_present = TRUE;
			} else lun_zero_present = TRUE;
		}
	}
	close_sysio ();

	end_curses ();

	exit ( Exit_Status );
}

/*===========================================================================*/
/*  Function:  static void primary_loop ()                                   */
/*===========================================================================*/

static void primary_loop ( void )

{
	while ( TRUE ) {
		if ( update_needed != NONE ) {
			work_on ();
			get_array_info ( update_needed );
			if ( input != 0 && tolower ( input ) != 'u')
				last_view = -1;
			update_needed = NONE;
		}

		setup_current_view ();

		work_off ();

		input = monitor_input ( 30 );

		if ( ( tolower ( input ) ) == 'q' || input == KEY_F(6) )
			return;

		switch ( current_view ) {
		case DRIVE_VIEW:
			drive_view ( input );
			break;

		case LUN_VIEW:
			lun_view ( input );
			break;
		}
	}
}

/*===========================================================================*/
/*  Function:  static void drive_view ( int input )                          */
/*===========================================================================*/

static void drive_view ( int input )

{
#ifdef PARAGON860 /* add "monitor reconstruction" command */
	int     rindex, index, ch, id, action, status;
#endif
	switch ( input ) {
	case TAB:
		current_view = LUN_VIEW;
		break;

	case KEY_RIGHT:
	case SPACE:
		highlight_next_drive ( TRUE, 1 );
		break;

	case KEY_DOWN:
		highlight_next_drive ( TRUE, 5 );
		break;

	case KEY_LEFT:
		highlight_next_drive ( FALSE, 1 );
		break;

	case KEY_UP:
		highlight_next_drive ( FALSE, 5 );
		break;

	case NEWLINE:
		highlight_next_drive_in_grp ();
		break;

	case 'a':
	case 'A':
		update_needed = add_drive ();
		break;

	case 'd':
	case 'D':
		update_needed = delete_drive ();
		break;

	case 'f':
	case 'F':
		update_needed = fail_drive ();
		break;

	case 'r':
	case 'R':
		update_needed = replace_drive ();
		break;

	case 'h':
	case 'H':
	case KEY_F(1):
		pop_drive_help ();
		break;

	case 'u':
	case 'U':
		wrefresh ( curscr );
		update_needed = ALL;
		break;
#ifdef PARAGON860 /* add "monitor reconstruction" command */
	case 'm':
	case 'M':
		work_on();
		ch = ( current_drive >> 4 ) & 0x07;
        	id = current_drive & 0x07;
        	rindex = index = ( id * 5 ) + ch;
        	if ( index > 4 )
               	 	index += ( id * 10 );
		work_off();
		status = display_recon_progress ( rindex );
	        if ( status != QUIT  && status == OPTIMAL )
       		         pop_recon_complete ( action );
		break;
#endif
#ifdef PARAGON860 /* ^A instead of KEY_F(12) to enter ace debug */
	case '':
#else
	case KEY_F(12):
#endif
		if ( access_to_debug == TRUE )
			pop_debug ();
		break;
	}
	last_view = DRIVE_VIEW;
}

/*===========================================================================*/
/*  Function:  static void lun_view ( int input )                            */
/*===========================================================================*/

static void lun_view ( int input )

{
	switch ( input ) {

	case TAB:
		current_view = DRIVE_VIEW;
		break;

	case SPACE:
	case KEY_DOWN:
		highlight_next_lun ( TRUE );
		break;

	case KEY_UP:
		highlight_next_lun ( FALSE );
		break;

	case 'c':
	case 'C':
		update_needed = create_lun ( );
		break;

	case 'd':
	case 'D':
		if ( current_group != 0 )
			update_needed = delete_lun ( get_hl_lun_nbr () );
		break;

	case 'm':
	case 'M':
		if ( current_group != 0 )
			update_needed = modify_lun ( FROM_MAINMENU );
		break;

	case 'f':
	case 'F':
		if ( current_group != 0 )
			update_needed = format_lun ( get_hl_lun_nbr (), FROM_MAINMENU );
		break;

	case 'h':
	case 'H':
	case KEY_F(1):
		pop_lun_help ();
		break;

	case KEY_F(12):
	case '':
		if ( access_to_debug == TRUE )
			pop_debug ();
		break;

	case 'u':
	case 'U':
		wrefresh ( curscr );
		update_needed = ALL;
		break;
	}
	last_view = LUN_VIEW;
}

/*===========================================================================*/
static int parse_argv ( )
/*===========================================================================*/
{
	int		c, ok;
	int 		goodzip = FALSE;

	hw_zipcode_t *hw;
#ifdef PARAGON_860
	while ( ( c = getopt ( Argc, Argv, "f:dp" ) ) != EOF ) {
		switch ( c ) {
		case 'f':
			strcpy(Devname,optarg);
			hw_addr = zip_edit (convert_array_name(optarg), &ok);
#else
	while ( ( c = getopt ( Argc, Argv, "z:dp" ) ) != EOF ) {
		switch ( c ) {
		case 'z':
			hw_addr =   zip_edit ( (u_char *)optarg, (int *)&ok  ); 
			hw = (hw_zipcode_t *) &hw_addr;  
#endif
			if ( ok ) 
				goodzip = TRUE;
			break;

		case 'd':
			access_to_debug = TRUE;
			break;

		case 'p':
			access_to_prior_release = TRUE;
			break;
		}
	}
	if ( goodzip == FALSE ) {
		ace_error ( COMMAND_LINE_SYNTAX, CMDLINE_SYNTAX_ERROR );
		return ( ERR );
	}
	return ( OK );
}

/*==========================================================================*/
static int pop_no_lun_zero ( void )
/*==========================================================================*/
{
	int		answer;

	popbox ( 6, 13, 19, 66, "< WARNING >" );
	do {
		mvaddstr (  9, 20, "You do not have a LUN 0 defined.  Some" );
		mvaddstr ( 10, 20, "operating systems can NOT access any" );
		mvaddstr ( 11, 20, "other logical units without a LUN 0." );
		mvaddstr ( 13, 20, "Create a LUN 0 before exiting 'ace'." );
		mvaddstr ( 15, 20, "Press 'c' to continue." );
		mvaddstr ( 16, 20, "Press 'q' to quit and exit.");
		refresh ();
		answer = getch ();
	} while ( tolower ( answer ) != 'q' && tolower ( answer ) != 'c' );
	popclr ();
	if ( tolower ( answer ) == 'q' )
		return ( QUIT );
	else
		return ( CONTINUE );
}
