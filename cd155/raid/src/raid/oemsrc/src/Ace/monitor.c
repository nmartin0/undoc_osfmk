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
  Name:		$RCSfile: monitor.c,v $
  Version:	$Id: monitor.c,v 1.2.4.1 1995/06/11 22:20:01 kat Exp $
  Title:	Monitor Input
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:20:01 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272

  COPYRIGHT 1992, NCR Corporation

  Description:	This module monitors the keyboard for input while
		running under a timer.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: monitor.c,v $
 * Revision 1.2.4.1  1995/06/11  22:20:01  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:39  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:53  richardg
 * Initial revision
 *
 * Revision 1.1  1992/03/18  14:00:27  bmyers
 * Initial revision
 *
 */

/******************************************************************************
 ***				    INCLUDES				    ***
 *****************************************************************************/
#include <time.h>
#include "acedefs.h"
#include <curses.h>
/******************************************************************************
 ***			      EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern int	update_needed;
/*==========================================================================*/
int monitor_input ( long time_interval )
/*==========================================================================*/
{
	long 		oldtime, newtime, i;
	int		input;

	nodelay ( stdscr, TRUE ); 		/* non-blocking char I/O */
	oldtime = time ( &oldtime );
	newtime = time ( &newtime );

	for  ( i = 0; i <=  time_interval; i++ ) {
		while  ( oldtime == newtime ) {
			if ( ( input = getch () ) != ERR ) {
				nodelay ( stdscr, FALSE );
				return ( input );
			}
			newtime = time ( &newtime );
		}
		oldtime = newtime;
	}
	update_needed = ALL;
	nodelay ( stdscr, FALSE );		/* turn blocking on */
	return ( 0 );
}
