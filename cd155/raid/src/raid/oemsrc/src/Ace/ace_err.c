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
  Name:		$RCSfile: ace_err.c,v $
  Version:	$Id: ace_err.c,v 1.2.4.1 1995/06/11 22:19:28 kat Exp $
  Title:	'ace' error reporting 
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:19:28 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272

  COPYRIGHT 1992, NCR Corporation

  Description:	Error reporting routine for 'ace'
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: ace_err.c,v $
 * Revision 1.2.4.1  1995/06/11  22:19:28  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:02  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:19  richardg
 * Initial revision
 *
 * Revision 1.1  1992/03/18  14:00:14  bmyers
 * Initial revision
 *
 */
/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "stddefs.h"
#define IN_ERROR_C
#include "dau_err.h"
#include <curses.h>
/******************************************************************************
 ***		             VARIABLE DEFINITIONS		    ***
 ******************************************************************************/

/******************************************************************************
 ***		             EXTERNAL REFERENCES			    ***
 ******************************************************************************/

extern int Argc;
extern char **Argv;

/******************************************************************************
 ***				  PROCEDURE 				    ***
 *****************************************************************************/
void ace_error( int errcode, int msgcode )
{
	int	i;
	char	*progname;

	if ( ( progname = ( char * ) strrchr ( Argv[0], '/') ) != NULL )
		progname++;
	else
		progname = Argv[0];

	popbox ( 6, 8, 16, 66, "< ERROR >" );
	mvprintw ( 8, 33, "[ %s ]", progname );
	mvprintw ( 10, 11, "%s", error_table[msgcode] );
	if ( errcode == COMMAND_LINE_SYNTAX )
#ifdef PARAGON_860
		mvaddstr ( 12, 17, "usage:  ace -f filename" );
#else
		mvaddstr ( 12, 17, "usage:  ace -z hw_addr" );
#endif
	mvprintw ( 14, 17, "Press any key to continue  " );
	refresh ();
	i = getch ();
	popclr ();
}
