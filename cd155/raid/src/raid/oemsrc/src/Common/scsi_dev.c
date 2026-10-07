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
 Name:		scsi_dev.c
 Title:		SCSI_DEV Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:38 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the SCSI_DEV class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "scsi_dev.h"

/******************************************************************************
 ***				  MACRO DEFINITIONS                         ***
 *****************************************************************************/
#define DOWNLOAD_MICROCODE 4

/******************************************************************************
 ***				  PROTOTYPES                                ***
 *****************************************************************************/
static int read( void *, u_char *, u_long, u_long, u_short );
static int write( void *, u_char *, u_long, u_long, u_short );
static int mode_sense( void *, u_char *, u_int, u_char, u_char );
static int log_select( void *, u_char *, u_int, u_int, u_int, u_char );
static int log_sense( void *, u_char *, u_int, u_int, u_int, u_char );

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
init_cdb( cdb )
CDB_t *cdb; /* u_char changed to CDB_t for Novell */
{
	u_int i;
	u_char *p;

	for (i=0, p = (u_char *) cdb; i < sizeof (CDB_t); i++)
		p[i] = 0;
}

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

#define THIS	( (SCSI_DEV *) this )

void
destroy_SCSI_DEV( this )
void *this;
{
	destroy_SYSIO( THIS->sysio);
	destroy_OBJECT( this );
}

static int
inquiry( this, data, byte_count )
void *this;
u_char *data;
u_int byte_count;
{
	CDB_t cdb;
	Inquiry_CDB_t *p;

	debug ("scsi_dev: inquiry\n");
	init_cdb( &cdb );
	p = (Inquiry_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_Inquiry;
	p->LUN = get_LUN( THIS->hw_addr );
	p->Allocation_Length = byte_count;
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
read( this, data, byte_count, block_addr, block_count )
void *this;
u_char *data;
u_long byte_count;
u_long block_addr;
u_short block_count;
{
	CDB_t cdb;
	Read_Write_CDB_t *p;

	debug( "scsi_dev: read byte_count=%x, block_addr=%x\n",
	    byte_count, block_addr);
	init_cdb( &cdb );
	p = (Read_Write_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_Read_10;
	p->LUN = get_LUN( THIS->hw_addr );
	set_long ( p->Logical_Blk_Addr, block_addr );
	set_short( p->Transfer_Length, block_count );
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
write( this, data, byte_count, block_addr, block_count )
void *this;
u_char *data;
u_long byte_count;
u_long block_addr;
u_short block_count;
{
	CDB_t cdb;
	Read_Write_CDB_t *p;

	debug( "scsi_dev: write\n");
	init_cdb( &cdb );
	p = (Read_Write_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_Write_10;
	p->LUN = get_LUN( THIS->hw_addr );
	set_long ( p->Logical_Blk_Addr, block_addr );
	set_short( p->Transfer_Length, block_count);
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
write_buffer( this, data, byte_count, offset, keep )
void *this;
u_char *data;
u_long byte_count;
u_long offset;
u_int keep;
{
	CDB_t cdb;
	Write_Buffer_CDB_t *p;
	Bytes_t *s;

	debug( "scsi_dev: write_buffer\n" );
	init_cdb( &cdb );
	p = (Write_Buffer_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_WriteBuffer;
	p->LUN = get_LUN( THIS->hw_addr );
	p->Mode = DOWNLOAD_MICROCODE | keep;
	set_long ( p->_8_bytes.Array_Specific.Ctlr_Addr, offset);
	s = (Bytes_t *) &byte_count;
	p->_8_bytes.Array_Specific.Transfer_Length[0] = s->MIB_1;
	p->_8_bytes.Array_Specific.Transfer_Length[1] = s->MIB_2;
	p->_8_bytes.Array_Specific.Transfer_Length[2] = s->LSB;
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
mode_sense( this, data, byte_count, pg_ctrl, pg_code )
void *this;
u_char *data;
u_int byte_count;
u_char pg_ctrl;
u_char pg_code;
{
	CDB_t cdb;
	Mode_Sense_CDB_t *p;

	debug( "scsi_dev: mode_sense\n");
	init_cdb( &cdb );
	p = (Mode_Sense_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_ModeSense_10;
	p->LUN = get_LUN( THIS->hw_addr );
	p->Pg_Ctrl = pg_ctrl;
	p->Pg_Code = pg_code;
	set_short( p->Allocation_Length, byte_count);
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
mode_select( this, datap, byte_count, pg_format, sp )
void *this;
u_char *datap;
u_int byte_count;
u_int pg_format;
u_int sp;
{
	CDB_t cdb;
	Mode_Select_CDB_t *p;
	ModeSelectHeader_t *data = (ModeSelectHeader_t *) datap;
	Bytes_t *s;

	debug( "scsi_dev: mode_select\n");
	init_cdb( &cdb );
	p = (Mode_Select_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_ModeSelect_10;
	p->LUN = get_LUN( THIS->hw_addr );
	p->PF = pg_format;
	p->SP = sp;

	/* add 18 to the page length to accurately set page size */
	/* (the Page Length does not include itself or previous byte) */
	/* I.E. ModeSelectHeader10	- 8 bytes */
	/*	BlockDescriptor		- 8 bytes */	
	/*      PS bit & Page Code 	- 1 byte */
	/*      Page Length		- 1 byte */
	byte_count=((data->PageHeader.Page_Length) + 18);
	set_short( p->Param_List_Length, byte_count );
	cdb.Buffer = (u_char *) data;
	cdb.BufferLength = byte_count;
	data->Header.ModeDataLength[0]=0x00;
	data->Header.ModeDataLength[1]=0x00;
	data->PageHeader.Parameters_Savable=0;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
send_diag( this, data, byte_count, pg_format )
void *this;
u_char *data;
u_int byte_count;
u_int pg_format;
{
	CDB_t cdb;
	Send_Recv_Diagnostic_CDB_t *p;
	Bytes_t *s;

	debug( "scsi_dev: send_diag\n");
	init_cdb( &cdb );
	p = (Send_Recv_Diagnostic_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_SendDiagnostic;
	p->LUN = get_LUN( THIS->hw_addr );
	p->Pg_Format = pg_format;
	set_short( p->Length, byte_count );
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
receive_diag( this, data, byte_count )
void *this;
u_char *data;
u_int byte_count;
{
	CDB_t cdb;
	Send_Recv_Diagnostic_CDB_t *p;
	Bytes_t *s;

	debug( "scsi_dev: receive_diag\n");
	init_cdb( &cdb );
	p = (Send_Recv_Diagnostic_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_ReceiveDiagnostic;
	p->LUN = get_LUN( THIS->hw_addr );
	set_short( p->Length, byte_count );
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
request_sense( this, data, byte_count, aen )
void *this;
u_char *data;
u_int byte_count;
u_int aen;
{
	CDB_t cdb;
	Request_Sense_CDB_t *p;

	debug( "scsi_dev: request_sense\n");
	init_cdb( &cdb );
	p = (Request_Sense_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_RequestSense;
	p->LUN = get_LUN( THIS->hw_addr );
	p->AEN = aen;
	p->Allocation_Length = byte_count;
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
log_select( this, data, byte_count, pcr, sp, pg_ctrl )
void *this;
u_char *data;
u_int byte_count;
u_int pcr;
u_int sp;
u_char pg_ctrl;
{
	CDB_t cdb;
	Log_Select_CDB_t *p;
	Bytes_t *s;

	debug( "scsi_dev: log_select\n");
	init_cdb( &cdb );
	p = (Log_Select_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_LogSelect;
	p->LUN = get_LUN( THIS->hw_addr );
	p->SP = sp;
	p->PCR = pcr;
	p->Pg_Ctrl = pg_ctrl;
	set_short( p->Param_List_Length, byte_count );
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
log_sense( this, data, byte_count, ppc, sp, pg_ctrl )
void *this;
u_char *data;
u_int byte_count;
u_int ppc;
u_int sp;
u_char pg_ctrl;
{
	CDB_t cdb;
	Log_Sense_CDB_t *p;
	Bytes_t *s;

	debug( "scsi_dev: mode_sense\n");
	init_cdb( &cdb );
	p = (Log_Sense_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_LogSense;
	p->LUN = get_LUN( THIS->hw_addr );
	p->PPC = ppc;
	p->SP = sp;
	p->Pg_Ctrl = pg_ctrl;
	set_short( p->Allocation_Length, byte_count );
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
lock( this )
void *this;
{
	return( THIS->sysio->lock( THIS->sysio, THIS->hw_addr ));
}

static int
unlock( this )
void *this;
{
	return( THIS->sysio->unlock( THIS->sysio, THIS->hw_addr ));
}

#undef THIS

SCSI_DEV *
new_SCSI_DEV( hw_addr, exclusive )
hw_zipcode hw_addr;
int exclusive;
{
	u_int stat;
	OBJECT *s;
	SYSIO *p;
	SCSI_DEV *this;
	p = new_SYSIO( hw_addr, exclusive );
	s = new_OBJECT();
	this = (SCSI_DEV *) zalloc( sizeof(SCSI_DEV) );
	memcpy( this, s, sizeof( OBJECT ) ); /* inherit from OBJECT */
	free( s );
	this->inquiry = inquiry;
	this->read = read;
	this->write = write;
	this->write_buffer = write_buffer;
	this->mode_sense = mode_sense;
	this->mode_select = mode_select;
	this->send_diag = send_diag;
	this->receive_diag = receive_diag;
	this->request_sense = request_sense;
	this->log_select = log_select;
	this->log_sense = log_sense;
	this->lock = lock;
	this->unlock = unlock;
	this->sysio = p;
	this->hw_addr = hw_addr;
	this->destroy = destroy_SCSI_DEV;
	this->description = "scsi_dev object";
	return( this );
}
