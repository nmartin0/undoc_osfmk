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
  Name:		$RCSfile: chkrel.c,v $
  Version:	$Id: chkrel.c,v 1.2.4.1 1995/06/11 22:19:39 kat Exp $
  Title:	Check SDAC Software Revision Level
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0  FS 348-0024272


  COPYRIGHT 1992, NCR Corporation

  Description:	Insure that the SDAC software is at a pre-determined level.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: chkrel.c,v $
 * Revision 1.2.4.1  1995/06/11  22:19:39  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:16  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:43  richardg
 * Initial revision
 *
 * Revision 1.5  1992/06/23  18:58:01  root
 * sdpr u-132 - Added logic to test for non-existent device and display
 * error message if address bad. - bem
 *
 * Revision 1.4  1992/04/17  18:08:25  root
 * sdpr u-093 Added refresh after out putting the message.  bem
 *
 * Revision 1.3  1992/04/08  12:02:40  root
 * sdpr #U77 - Corrected logic to test for equal to or greater than
 * 	    (rel) 03 - bem
 *
 * Revision 1.2  1992/03/25  17:22:06  root
 * Message text change for Pubs.
 *
 * Revision 1.1  1992/03/21  20:07:43  bmyers
 * Initial revision
 *
 */
/******************************************************************************
 ***				    INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "acedefs.h"
#include "scsidefs.h"
#include <string.h>
#include <curses.h>
/******************************************************************************
 ***			      EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern SCSI_Inquiry_Data_t	InquiryData;
extern int			do_inquiry ( int );
extern void			popbox ( int, int, int, int, char * );
/******************************************************************************
 ***			      INTERNAL PROCEDURES			    ***
 *****************************************************************************/
static void		pop_not_supported ( void );
static void		pop_wrong_address ( void );
/******************************************************************************
 ***			      VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
static char		rev[6];
/******************************************************************************
 ***			       ENTRY DEFINITIONS			    ***
 *****************************************************************************/
int		 	check_sdac_release ( int );


/*=======================================================================*/
int check_sdac_release ( int access_to_prior_release )
/*=======================================================================*/
{
	int 			status, i;
	SCSI_Inquiry_Data_t 	*p = &InquiryData;

	p->Product_ID[0] = 0x07;
	status = inquiry ( 0, p, sizeof ( SCSI_Inquiry_Data_t) );


	if ( status == OK ) {
		if ( ( status = strncmp(p->Product_ID, "ADP-92", 6 ) ) != 0 ) {
			pop_wrong_address ();
			return ( ERR );
		}
#ifdef PARAGON860 /* skip check of release level as we have several raid's
		   * that have a blank version field */
			return ( OK );
#endif
		for ( i = 0; i < 4; i++ )
			rev[i] = p->Revision_Level[i];
		rev[i] = 0;
		if ( ( status = strncmp ( rev, "03", 2 ) ) >= 0 )
			return ( OK );
	} else {
		pop_wrong_address ();
		return ( ERR );
	}

	if ( access_to_prior_release == TRUE )
		return ( OK );

	pop_not_supported ( ); 
	return ( ERR );
}

/*=======================================================================*/
static void pop_not_supported ( void )
/*=======================================================================*/
{
	int 			status;

	popbox ( 6, 10, 17, 60, "< INFORMATION >" );
	mvaddstr ( 8, 19, "This utility does not support the" );
	mvaddstr ( 9, 19, "version level of the controller" );
	mvprintw ( 10, 19, "firmware currently on the drives" );
 	mvprintw ( 11, 19, "(Version Level %s).", rev );
	mvaddstr ( 13, 19, "Upgrade the controller firmware." );
	mvaddstr ( 15, 19, "Press any key to continue." );
	refresh ();
	getch ();
	popclr ();
}

/*=======================================================================*/
static void pop_wrong_address ( void )
/*=======================================================================*/
{
	int 			status;

	popbox ( 6, 10, 17, 60, "< INFORMATION >" );
	mvaddstr ( 9, 19, "This utility cannot find an array" );
	mvaddstr ( 10, 19, "controller at this address." );
	mvaddstr ( 11, 19, "Please insure the correct device" );
 	mvaddstr ( 12, 19, "was selected when the utility was" );
	mvaddstr ( 13, 19, "invoked." );
	mvaddstr ( 15, 19, "Press any key to continue." );
	refresh ();
	getch ();
	popclr ();
}
