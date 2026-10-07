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
  Name:		$RCSfile: scsicmd.c,v $
  Version:	$Id: scsicmd.c,v 1.2.4.1 1995/06/11 22:20:05 kat Exp $
  Title:	
  Revision:	$Revision: 1.2.4.1 $
  Update Date:	$Date: 1995/06/11 22:20:05 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272


  COPYRIGHT 1992, NCR Corporation

  Description:	Interface to build the SCSI I/O structure and call sysio.
		Provides locking and unlocking functions.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: scsicmd.c,v $
 * Revision 1.2.4.1  1995/06/11  22:20:05  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1994/12/13  23:58:51  richardg
 * updating after copyright messaged.
 *
 * Revision 1.1  1992/12/28  18:29:56  richardg
 * Initial revision
 *
 * Revision 1.4  1992/03/21  20:03:11  bmyers
 * Added function to destroy sysio object if present.
 *
 * Revision 1.3  1992/03/20  17:33:04  bmyers
 * Changed locking scheme for exclusive access.
 *
 * Revision 1.2  1992/03/19  23:33:05  bmyers
 * Added logic to check for device present for NOVELL
 *
 * Revision 1.1  1992/03/18  14:00:29  bmyers
 * Initial revision
 *
 */
/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "scsi_dk.h"
#include "scsi_dev.h"
#include "dau_err.h"
#include "acedefs.h"

static SYSIO		*sysio;
static int		check_io_path ( int );
static int		first_time = 0;

void 			close_sysio ( void );
#define	OK		0
#define ERR		-1

extern hw_zipcode	hw_addr;

/* Structure to allow for referencing the page length of each page. */
/* This needs to be done to accurately compute the EXACT length of each */
/* page before a mode select is sent to the SDAC.  The sizeof(page) will */
/* compute the length to the next integer boundary.  The SDAC will */
/* not accept a mode select with an inaccurate length. */

typedef struct a_pageheader {
	u_char	Page_Code:				7,
		Parameters_Savable:			1;
	u_char	Page_Length;
} a_PageHeader_t;

/* (Above structure included because standard one in SCSI.h did not define */
/* "Parameters_Savable" bit.  -RMJ)					   */

typedef struct ModeSelectHeader {
	Mode10ParameterHeader_t	Header;
	Mode10BlockDescriptor_t	BlockDescriptor;
	a_PageHeader_t		PageHeader;
	} ModeSelectHeader_t;


/******************************************************************************
 ***				  INTERNAL PROCEDURES			    ***
 *****************************************************************************/
init_cdb ( cdb )
CDB_t *cdb; /* u_char changed to CDB_t for Novell */
{
	u_int i;
	u_char *p;

	for ( i=0, p = ( u_char * ) cdb; i < sizeof ( CDB_t ); i++ )
		p[i] = 0;
}

/*==========================================================================*/
int inquiry ( lun, data, byte_count )
/*==========================================================================*/
u_int lun;
u_char *data;
u_int byte_count;
{
	CDB_t cdb;
	Inquiry_CDB_t *p;

	debug ( "scsicmd: inquiry\n" );
	init_cdb ( &cdb );
	p = ( Inquiry_CDB_t * ) cdb.Byte;
	p->Op_Code = SCSI_Inquiry;
	p->LUN = lun;
	p->Allocation_Length = byte_count;
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	if ( ( check_io_path ( lun ) ) == ERR )
		return ( ERR );
	return ( sysio->controller_command ( sysio, ( u_char * ) &cdb ) );
}

/*==========================================================================*/
int mode_sense ( lun, data, byte_count, pg_ctrl, pg_code )
/*==========================================================================*/
u_int lun;
u_char *data;
u_int byte_count;
u_char pg_ctrl;
u_char pg_code;
{
	CDB_t cdb;
	Mode_Sense_CDB_t *p;

	debug ( "scsicmd: mode_sense\n" );
	init_cdb ( &cdb );
	p = ( Mode_Sense_CDB_t * ) cdb.Byte;
	p->Op_Code = SCSI_ModeSense_10;
	p->LUN = lun;
	p->Pg_Ctrl = pg_ctrl;
	p->Pg_Code = pg_code;
	set_short ( p->Allocation_Length, byte_count );
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	if ( ( check_io_path ( lun ) ) == ERR )
		return ( ERR );
	return ( sysio->controller_command ( sysio, ( u_char * ) &cdb ) );
}

/*==========================================================================*/
int mode_select ( lun, data, byte_count, pg_format, sp, lock )
/*==========================================================================*/
int lun, lock;
ModeSelectHeader_t *data;
u_int byte_count;
u_int pg_format;
u_int sp;
{
	CDB_t			cdb;
	Mode_Select_CDB_t	*p;
	int			status, cmd_status;

	debug ( "scsicmd: mode_select\n" );
	init_cdb ( &cdb );
	p = ( Mode_Select_CDB_t * ) cdb.Byte;
	p->Op_Code = SCSI_ModeSelect_10;
	p->LUN = lun;
	p->PF = pg_format;
	p->SP = sp;

	/* add 18 to the page length to accurately set page size */
	/* (the Page Length does not include itself or previous byte) */
	/* I.E. ModeSelectHeader10	- 8 bytes */
	/*	BlockDescriptor		- 8 bytes */	
	/*      PS bit & Page Code 	- 1 byte */
	/*      Page Length		- 1 byte */

	byte_count= ( ( data->PageHeader.Page_Length ) + 18 );
	set_short ( p->Param_List_Length, byte_count );
	cdb.Buffer = ( u_char * ) data;
	cdb.BufferLength = byte_count;
	data->Header.ModeDataLength[0]=0x00;
	data->Header.ModeDataLength[1]=0x00;
	data->PageHeader.Parameters_Savable=0;
	if ( ( check_io_path ( lun ) ) == ERR )
		return ( ERR );
	if ( lock == TRUE ) {
		status = sysio->lock ( sysio, hw_addr );
		if ( status ) {
			ace_error ( SCSI_CONDITION, EXCLUSIVE_ACCESS_ERROR );
			return ( LOCK_ERROR );
		}
	}
	cmd_status = sysio->controller_command ( sysio, ( u_char * ) &cdb );
	if ( lock == TRUE )
		status = sysio->unlock ( sysio, hw_addr );
	return ( cmd_status );
}

/*==========================================================================*/
int format_unit ( lun, data, byte_count, fmtdata, cmplist, listfmt, interleave )
/*==========================================================================*/
int lun;
u_char *data;
u_int byte_count, fmtdata, cmplist, listfmt, interleave;
{
	CDB_t cdb;
	Format_Unit_CDB_t *p;
	int			status, cmd_status;

	debug( "scsicmd: format_unit" );
	init_cdb ( &cdb );
	p = ( Format_Unit_CDB_t * ) cdb.Byte;
	p->Op_Code = SCSI_Format;
	p->LUN = lun;
	p->Defect_List_Fmt = listfmt;
	p->CmpLst = cmplist;
	p->FmtData = fmtdata;
	set_short ( p->Interleave, interleave );
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	if ( ( check_io_path ( lun ) ) == ERR )
		return ( ERR );
	status = sysio->lock ( sysio, hw_addr );
	if ( status ) {
		ace_error ( SCSI_CONDITION, EXCLUSIVE_ACCESS_ERROR );
		return ( LOCK_ERROR );
	}
	cmd_status = sysio->controller_command ( sysio, ( u_char * ) &cdb );
	status = sysio->unlock ( sysio, hw_addr );
	return ( cmd_status );
}

/*==========================================================================*/
int request_sense ( lun, data, byte_count, aen )
/*==========================================================================*/
u_int lun;
u_char *data;
u_int byte_count;
u_int aen;
{
	CDB_t cdb;
	Request_Sense_CDB_t *p;

	debug ( "scsicmd: request_sense\n" );
	init_cdb ( &cdb );
	p = ( Request_Sense_CDB_t * ) cdb.Byte;
	p->Op_Code = SCSI_RequestSense;
	p->LUN = lun;
	p->AEN = aen;
	p->Allocation_Length = byte_count;
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	if ( ( check_io_path ( lun ) ) == ERR )
		return ( ERR );
	return ( sysio->controller_command ( sysio, ( u_char * ) &cdb ) );
}

/*==========================================================================*/
int lock ( )
/*==========================================================================*/
{
	return ( sysio->lock ( sysio, sysio->hw_addr ) );
}

/*==========================================================================*/
int unlock ( )
/*==========================================================================*/
{
	return ( sysio->unlock ( sysio, sysio->hw_addr ) );
}

/*==========================================================================*/
static int check_io_path ( int lun )
/*==========================================================================*/
{
	int		exclusive = FALSE;
	hw_zipcode_t	*p = ( hw_zipcode_t * ) &hw_addr;

	if ( first_time == 0 ) {
		sysio = new_SYSIO ( hw_addr, exclusive );
#ifdef NOVELL
		if ( sysio->deviceNumber == -1 )
			return ( ERR );
#endif
		first_time = 1;
		return ( OK );
	}
	if ( lun == p->LUN )
		return ( OK );

	destroy_SYSIO ( sysio );
	p->LUN = lun;
	sysio = new_SYSIO ( hw_addr, exclusive );
#ifdef NOVELL
	if ( sysio->deviceNumber == -1 )
		return ( ERR );
#endif
	return ( OK );
}

/*==========================================================================*/
void close_sysio ( void )
/*==========================================================================*/
{
	if ( first_time ==  1 )
		destroy_SYSIO ( sysio );
}
