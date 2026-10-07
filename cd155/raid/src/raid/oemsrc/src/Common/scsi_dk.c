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
 Name:		scsi_dk.c
 Title:		SCSI_DK Class Implementation
 Version:	
 Revision:	$Revision: 1.1.4.1 $
 Update Date:	$Date: 1995/06/11 23:30:41 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal 07/90

 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the SCSI_DK class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "scsi_dk.h"

/******************************************************************************
 ***				  PROTOTYPES				    ***
 *****************************************************************************/
static int verify( void *, u_char *, u_long, u_long, u_short, u_int );
static int write_verify( void *, u_char *, u_long, u_long, u_short, u_int );

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

#define THIS	( (SCSI_DK *) this )

static int
read_capacity( this, data, block_addr, pmi )
void *this;
u_char *data;
u_long block_addr;
u_int pmi;
{
	CDB_t cdb;
	Read_Capacity_CDB_t *p;

	debug( "scsi_dk: read_capacity\n");
	init_cdb( &cdb );
	p = (Read_Capacity_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_ReadCapacity;
	p->LUN = get_LUN( THIS->hw_addr );
	set_long( p->Logical_Blk_Addr, block_addr );
	p->PMI = pmi;
	cdb.Buffer = data;
	cdb.BufferLength = sizeof (Read_Capacity_Data_t);
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
verify( this, data, byte_count, block_addr, block_count, pcheck )
void *this;
u_char *data;
u_long byte_count;
             		/*  NOTE: data and byte_count are DUMMY args for now
                  	 *  since the DAC does not currently support the
			 *  ByteChk option bit (which performs byte-by-byte
			 *  comparison of data transferred to data on medium).
			 *  This also makes verify() more consistant with
			 *  read() & write() (in scsi_dev.c), and with
			 *  write_verify() (below).  SJR
			 */
u_long block_addr;
u_short block_count;
u_int pcheck;
{
	CDB_t cdb;
	Verify_CDB_t *p;

	debug( "scsi_dk: verify\n");
	init_cdb( &cdb );
	p = (Verify_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_BlockVerify;
	p->LUN = get_LUN( THIS->hw_addr );
	set_long ( p->Logical_Blk_Addr, block_addr );
	set_short( p->Verification_Length, block_count );
	p->ParityChk = pcheck;
	cdb.Buffer = NULL;
	cdb.BufferLength = (long) 0;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
write_verify( this, data, byte_count, block_addr, block_count, pcheck )
void *this;
u_char *data;
u_long byte_count;		/*  better be = sizeof(data[]) ! */
u_long block_addr;
u_short block_count;
u_int pcheck;
{
	CDB_t cdb;
	Write_Verify_CDB_t *p;

	debug( "scsi_dk: write_verify\n");
	init_cdb( &cdb );
	p = (Write_Verify_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_WriteVerify;
	p->LUN = get_LUN( THIS->hw_addr );
	set_long ( p->Logical_Blk_Addr, block_addr );
	set_short( p->Transfer_Length, block_count );
	p->ParityChk = pcheck;
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
format_unit( this, data, byte_count, fmtdata, cmplist, listfmt, interleave )
void *this;
u_char *data;
u_int byte_count, fmtdata, cmplist, listfmt, interleave;
{
	CDB_t cdb;
	Format_Unit_CDB_t *p;

	debug( "scsi_dk: format_unit\n");
	init_cdb( &cdb );
	p = (Format_Unit_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_Format;
	p->LUN = get_LUN( THIS->hw_addr );
	p->Defect_List_Fmt = listfmt;
	p->CmpLst = cmplist;
	p->FmtData = fmtdata;
	set_short( p->Interleave, interleave );
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

static int
is_root( this )
void *this;
{
	return( THIS->sysio->is_root( THIS->sysio ) );
}

static int
repair_parity( this, data, byte_count )
void *this;
u_char *data;
u_int byte_count;
{
	CDB_t cdb;
	Repair_Parity_CDB_t *p;

	debug ("scsi_dev: repair parity\n");
	init_cdb( &cdb );
	p = (Repair_Parity_CDB_t *) cdb.Byte;
	p->Op_Code = SCSI_RepairParity;
	p->LUN = get_LUN( THIS->hw_addr );
	cdb.Buffer = data;
	cdb.BufferLength = byte_count;
	return( THIS->sysio->controller_command(THIS->sysio, (u_char *) &cdb) );
}

void
destroy_SCSI_DK( this )
void *this;
{
	destroy_SCSI_DEV( this );
}

#undef THIS

SCSI_DK *
new_SCSI_DK( hw_addr, exclusive )
hw_zipcode hw_addr;
int exclusive;
{
	SCSI_DEV *s;
	SCSI_DK *this;

	s = new_SCSI_DEV( hw_addr, exclusive );
	this = (SCSI_DK *) zalloc( sizeof(SCSI_DK) );
	/* inherit from SCSI_DEV */
	memcpy( this, s, sizeof( SCSI_DEV ) );
	free( s );
	this->read_capacity = read_capacity;
	this->verify = verify;
	this->write_verify = write_verify;
	this->format_unit = format_unit;
	this->is_root = is_root;
	this->repair_parity = repair_parity;
	this->destroy = destroy_SCSI_DK;
	this->description = "scsi_dk object";
	return( this );
}
