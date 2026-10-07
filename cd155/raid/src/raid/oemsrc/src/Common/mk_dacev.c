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
 Name:		mk_dacev.c
 Title:		Make Dacevent
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:30 $ 
 Programmer:
 Documents:	1. UNIX V.4 Disk Array Utilities FS, no. 348-0027726
		2. Object-Oriented Programming in C, C Users Journal, 7/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module converts a SCSI status into a dacevent.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "scsi_dev.h"
#include "dac_abtcmd.h"
#include "dac_datpro.h"
#include "dac_hrderr.h"
#include "dac_illreq.h"
#include "dac_mederr.h"
#include "dac_miscmp.h"
#include "dac_nosens.h"
#include "dac_notrdy.h"
#include "dac_rcverr.h"
#include "dac_untatn.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

/* chk4_dacevent() - Examines RtnStat & SCSI sense data to determine if	     */
/*   useful sense data exists.  Returns 0 if RtnStat == 0, 1 if useful	     */
/*   sense data exists, or -1 if Rtnstat != 0 and there is no sense data.    */
/*   If system error has occurred, perror() is called to display it.	     */
int
chk4_dacevent( RtnStat, msg )
int RtnStat;
char *msg;
{
	scsi_stat_t *scsi_sense;
	char *syserrmsg;

	if ( RtnStat == 0 )
		return(0);
	scsi_sense = (scsi_stat_t *) &RtnStat;
	if ( scsi_sense->Errno != 0 ) {
		syserrmsg = (char *) strerror( (int) scsi_sense->Errno );
		fprintf(stderr, "%s: %s\n", msg, syserrmsg );
	}
	if ( scsi_sense->Sense_Present )
		return(1);
	else
		return(-1);
}

DAC_EVENT *
make_dacevent( hw_addr, SCSI_status, full )
hw_zipcode hw_addr;
u_long SCSI_status;
int full;
{
	SCSI_DEV *s;
	DAC_EVENT *event;
	int exclusive, aen;
	SCSI_Sense_Data_t sense;
	scsi_stat_t *ss = (scsi_stat_t *) &SCSI_status;
	int RtnStat;

	debug( "Entering make_dacevent\n" );
	/*debug( "  ss->Error=%d\n  ss->Sense_Present=%d\n  ss->Errno = %d\n",
	    ss->Error, ss->Sense_Present, ss->Errno); /* */
	if ( SCSI_status && ((ss->Error==FALSE) || (ss->Sense_Present==FALSE)) )
		return( (DAC_EVENT *) NULL );
	s = new_SCSI_DEV( hw_addr, exclusive = FALSE );
	aen = (SCSI_status == 0);
	if ( s->request_sense( s, (u_char *) &sense, sizeof(sense), aen ) )
		event = NULL;
	else
		switch ( sense.Sense_Key ) {
			case Key_No_Sense:
				if ( aen )
					event = NULL;
				else
					event = (DAC_EVENT *) new_DAC_NOSENS(
						hw_addr, &sense, full );
				break;

			case Key_Recovered_Error:
				event = (DAC_EVENT *) new_DAC_RCVERR( hw_addr,
						&sense, full );
				break;

			case Key_Not_Ready:
				event = (DAC_EVENT *) new_DAC_NOTRDY( hw_addr,
						&sense, full );
				break;

			case Key_Medium_Error:
				event = (DAC_EVENT *) new_DAC_MEDERR( hw_addr,
						&sense, full );
				break;
			case Key_Hardware_Error:
				event = (DAC_EVENT *) new_DAC_HRDERR( hw_addr,
					 	&sense, full );
				break;

			case Key_Illegal_Request:
				event = (DAC_EVENT *) new_DAC_ILLREQ( hw_addr,
					 	&sense, full );
				break;

			case Key_Unit_Attention:
				event = (DAC_EVENT *) new_DAC_UNTATN( hw_addr,
						&sense, full );
				break;
			case Key_Data_Protect:
				event = (DAC_EVENT *) new_DAC_DATPRO( hw_addr,
					 	&sense, full );
				break;
			case Key_Aborted_Command:
				event = (DAC_EVENT *) new_DAC_ABTCMD( hw_addr,
						&sense, full );
				break;
			case Key_Miscompare:
				event = (DAC_EVENT *) new_DAC_MISCMP( hw_addr,
						&sense, full );
				break;

			default:
				event = new_DAC_EVENT( hw_addr, &sense, full );
				break;
		}
	s->destroy( s );
	return( event );
}
