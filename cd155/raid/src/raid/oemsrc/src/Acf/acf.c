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
 Name:		acf.c
 Title:		Array Configuration Program
 Version:	
 Revision:	$Revision: 1.3.4.1 $
 Update Date:	$Date: 1995/06/11 22:44:24 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	The acf program provides a user interface for disk array
		controller configuration.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "main_menu.h"
#include "ucf_action.h"
#include "ecf_action.h"
#include "dcf_action.h"
#include "dld_action.h"
#include "fmt_action.h"
#include "inq_action.h"
#include "ccf_action.h"
#include "zip_dialog.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/

extern void print_title(int I, char *t);	/* external function */

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
/* ident is the what string identifier - execute `what -s acf` to use this  */
static char ident[] = PROJREL "/$Revision: 1.3.4.1 $ - $RCSfile: acf.c,v $, Array Configuration Program";
char title[] = "Disk Array Configuration Program";
ZIP_DIALOG *zip_dialog;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
main( argc, argv )
int argc;
char *argv[];
{
	ACTION *selection;
	MAIN_MENU *main_menu;

	Argc = argc;
	Argv = argv;
#ifdef PARAGON860 /* force non-interactive */
#define NOINTERACTIVE
#endif

#ifdef NOINTERACTIVE
	Interactive = FALSE;
#ifdef PARAGON860 /* force non-interactive */
	if (argc < 3)
		
#else
	if (argc == 1)
#endif
		error(COMMAND_LINE_SYNTAX, CMDLINE_SYNTAX_ERROR);
#else
	Interactive = ( argc == 1 );
#endif
	if (argc == 3)
	switch (argv[1][1]) {

		case	'i':
		case	'f':
				break;

		default:
			error(COMMAND_LINE_SYNTAX, CMDLINE_SYNTAX_ERROR);
	}

#ifdef MSDOS
	if (! Interactive )
		if ( argv[1][0] == 'c' )  /* ahem, this is for sdiag */
			Interactive = 1;
#endif

	print_title( (int) Interactive, title );

	main_menu = new_MAIN_MENU();
	new_INQ_ACTION( (MENU *) main_menu );	/* "Inquiry" */
	new_DLD_ACTION( (MENU *) main_menu );	/* "Download" */
	new_CCF_ACTION( (MENU *) main_menu );	/* "Canned configurations" */
	new_FMT_ACTION( (MENU *) main_menu );	/* "Format option" */
	new_UCF_ACTION( (MENU *) main_menu );	/* "Unit configuration" */
	new_ECF_ACTION( (MENU *) main_menu );	/* "Error configuration" */
	new_DCF_ACTION( (MENU *) main_menu );	/* "Drive configuration" */

	zip_dialog = new_ZIP_DIALOG();
	selection = (ACTION *) TRUE;
	while ( (u_long) selection ) {
		selection = (ACTION *) main_menu->prompt( main_menu );
		if ( (u_long) selection ) {
			debug( "got selection = %lx\n", selection );
			selection->Do( selection );
		}
	}
	zip_dialog->destroy( zip_dialog );
	main_menu->destroy( main_menu );
	exit( 0 );
}
