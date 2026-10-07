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
 Name:		arc.c
 Title:		Array Reconstruction Program
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:28:28 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	The arc program provides a user interface for disk
		array controller reconstruction.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "main_menu.h"
#include "dac_param.h"
#include "dac_rcfreq.h"
#include "dac_rcamt.h"
#include "dac_rcstrt.h"
#include "zip_dialog.h"
#include "dau_err.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/
/* external declaration */
extern void print_title(int I, char *t);

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
static char ident[] = PROJREL "/$Revision: 1.1.4.1 $ -$RCSfile: arc.c,v $, Array Reconstruction Program";
char title[] = "Disk Array Reconstruction Program";

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
main( argc, argv )
int argc;
char *argv[];
{
	long param;
	hw_zipcode hw_addr;
	DAC_PARAM *dp;
	MAIN_MENU *main_menu;
	ZIP_DIALOG *zip_dialog;

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

	zip_dialog = new_ZIP_DIALOG();
	hw_addr = (hw_zipcode) zip_dialog->prompt( zip_dialog );

	main_menu = new_MAIN_MENU();
	/* "Reconstruction frequency" */
	new_DAC_RCFREQ( (MENU *) main_menu, hw_addr );
	/* "Reconstruction amount"    */
	new_DAC_RCAMT( (MENU *) main_menu, hw_addr );
	/* "Reconstruction progress"  */
	new_DAC_RCPROG( main_menu, hw_addr );
	/* "Initiate reconstruction"  */
	new_DAC_RCSTRT( (MENU *) main_menu, hw_addr );

	dp = (DAC_PARAM *) TRUE;
	while ( (u_long) dp ) {
		dp = (DAC_PARAM *) main_menu->prompt( main_menu );
		if ( (u_long) dp ) {
			param = (long) dp->prompt( dp );
			if ( param == -1 )
				dp->display( dp );
			else
				dp->set( dp, param );
			dp->reset( dp );
		}

	}
	zip_dialog->destroy( zip_dialog );
	main_menu->destroy( main_menu );
	exit(0);
}
