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
 Name:		io_test.c
 Title:		SYSIO Test 
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:35:43 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the functions for testing scsi io.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <sys/scsi/SCSI.h>
#include <stdio.h>
#include <errno.h>
#include "io_test.h"
#include "cdb_struct.h"
#include "scsi_dk.h"

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

/* This function replaces an IO_SCSI call and may be used to emulate a disk array */
int
io_test( fd, cdb, direction )
int fd;
u_char *cdb;
u_int direction;
{
	int function;
	int stat;
	struct command cmd;
	char temp[50];
	char *in_buf;
	CDB_t *cdb_t, in_cdb_t;

	/* Initialize return status */
	stat = 0;

	/* Save input Command Descriptor Block */
	cdb_t = (CDB_t *)cdb;
	memcpy( &in_cdb_t, cdb, sizeof(CDB_t) );

	/* Save input buffer */
	in_buf = (char *)malloc( cdb_t->BufferLength );
	memcpy( in_buf, cdb_t->Buffer, cdb_t->BufferLength ); 
 
	/* Obtain data file names and command description for display */
	if( init_cmd( cdb_t, &cmd ) == ERROR)
		exit( 0 );

	/* If mode is auto then return without any display or interaction */
	else if( cmd.mode == 'a' )
	{
		if( strcmp( cmd.data_file, "" ) != 0 )
			cdb_t->BufferLength = get_outdata( cdb_t->Buffer, cmd.data_file );
		if( strcmp( cmd.sense_file, "" ) != 0 )
			get_outdata( (unsigned char *)&SenseData, cmd.sense_file );
		free( in_buf );
		return( stat );
	}

	/* Display input data */
	printf( "\n" );
	printf( "this->fd (%x)   ", fd );
	printf( "direction (%d)   ", direction);
	show_cdb( cdb_t, &cmd );

	do
	{
		/* Display menu */
		printf( " \nEnter a choice:\n" );
		printf( "    0 - quit\n" );
		printf( "    1 - display command block\n" ); 
		printf( "    2 - display input data\n" );
		printf( "    3 - display/edit output data (%s)\n", cmd.data_file );
		printf( "    4 - change output data file \n" );
		printf( "    5 - display/edit sense data (%s)\n", cmd.sense_file );
		printf( "    6 - change sense data file \n" );
		printf( "    7 - return(%d)\n", stat );
		printf( "    8 - change return value\n" );
		printf( "    9 - shell\n" );
		scanf( "%d", &function );
		printf( "\n");

		switch( function )
		{
		case 0:
			/* Exit program */
			break;
		case 1:
			/* Display Command Descriptor Block */
			show_cdb( &in_cdb_t, &cmd );
			break;
		case 2:
			/* Display input data buffer */
			show_indata( &in_cdb_t );
			break;
		case 3:
			/* Allow user to edit output data */ 
			sprintf( temp, "vi %s", cmd.data_file );
			system( temp );
			break;
		case 4:
			/* Change output data source file */
			printf( "Enter the output data file name: \n" );
			scanf( "%s", &cmd.data_file );
			break;
		case 5:
			/* Allow user to edit output sense data */
			sprintf( temp, " vi %s", cmd.sense_file );
			system( temp );
			break;
		case 6:
			/* Change output sense data source file */
			printf( "Enter the sense data file name: \n" );
			scanf( "%s", &cmd.sense_file );
			break;
		case 7:
			/* Return output data to caller */
			if( strcmp( cmd.data_file, "" ) != 0 )
				cdb_t->BufferLength = get_outdata( cdb_t->Buffer, cmd.data_file );
			if( strcmp( cmd.sense_file, "" ) != 0 )
				get_outdata( (unsigned char *)&SenseData, cmd.sense_file );
			free( in_buf );
			return( stat );
		case 8:
			/* Change return status value */
			printf( "-2 = Sense_Data_Present\n\n");
			printf( "Enter the return value for stat: \n" );
			scanf( "%d", &stat );
			break;
		case 9:
			/* Shell to OS */
			system( "sh" );
			break;
		default:
			printf( "unknown choice" );
 			break;
		}
	}
	/* Function 0 terminates program */
	while( function != 0 );
	exit(0);
}

/* Obtain data file names and command description */
static int
init_cmd( cdb_t, cmd )                        
CDB_t *cdb_t;
struct command *cmd;
{

	int hex;
	int status;
	int state;
	int op_code, pg_code;
	unsigned char *bp;
	FILE *fptr;

	/* Open the file containing the list of commands */
	if( (fptr = fopen( "/tmp/test/io_cmds.doc", "rb" )) == NULL)
	{
		printf( "io_test error: file open /tmp/test/io_cmds.doc\n" );
		return( ERROR );
	}

	/* Initialize variables */
	state = 0;
	op_code = (int)cdb_t->Byte[0];

	/* Currently multiple pages only exist for the Mode Sense Command */
	if( SCSI_ModeSense_10 )
		pg_code = (int)( (Mode_Sense_CDB_t *)cdb_t)->Pg_Code;

	do
	{
		switch( state )
		{
		case 0:
			/* The mode will be i for interactive or a for auto or s for suppress cdb display */
			status = get_mode( fptr, &cmd->mode );
			state = 1;
			break; 
		case 1:
			/* Read the next op code and check for match */
			status = get_hex( fptr, &cmd->op_code );
			if( cmd->op_code == op_code || cmd->op_code == -1 )
				state = 2;
			else
				state = 0;
			break;
		case 2:	 
			/* Read the next page code and check for match */
			status = get_hex( fptr, &cmd->pg_code );
			if( cmd->pg_code == -1 )
				state = 3;
			else if( pg_code == cmd->pg_code )
				state = 3;
			else
				state = 0;
			break;
		case 3:
			/* Read the command name */
			status = get_str( fptr, cmd->name );
			state = 4;
			break;
		case 4:
			/* Read the output data file name */
			status = get_str( fptr, cmd->data_file );
			state = 5;
			break;
		case 5:
			/* Read the output sense data file name */
			get_str( fptr, cmd->sense_file );
			status = -1;
			break;
		}
	}			
	/* Continue until case 5 sets status to -1 or end of file (EOF) */
	while( status >= 0 );

	fclose( fptr );
	return( OK );
}

/* Read the mode from the command file */
static int
get_mode( fptr, mode )
FILE *fptr;
unsigned char *mode;
{
	unsigned char ch;

	while( (ch = (unsigned char)getc( fptr )) != ( unsigned char ) EOF )
	{
		/* The mode is preceeded by '-' */
		if( ch == '-' )
		{
			*mode = getc( fptr );
			break;	
		}
	}
	return( ch );
}

/* Read the next byte from the file in hex form */
static int
get_hex( fptr, hex )
FILE *fptr;
int *hex;
{
	int ch;
	int val;

	/* Read next byte */
	while( (ch = getc( fptr )) != EOF )
	{
		/* If the byte is a hex number */
		if( (val = convert_char( ch )) >= 0 ) 
		{
			*hex = val;
			ch = getc( fptr );
			/* If this is a 2 character number */
			if( (val = convert_char( ch )) >= 0 ) 
				*hex = (*hex << 4) | val;
			break;
		}
		/* If this is a default value */
		else if( ch == '*' )
		{
			*hex = -1;
			break;
		}
	}
	return( ch );
}

/* Read the next string from the file */
static int
get_str( fptr, str )
FILE *fptr;
char *str;
{
	unsigned char ch;
	int state;

	state = 0;

	/* Read the next byte */
	while( (ch = getc( fptr )) != ( unsigned char ) EOF )
	{
		/* State will be 1 if the first '"' has been passed */
		if( state == 1 )
		{
			/* '"' here means end of string */
			if( ch == '"' )
			{
				*str = 0;
				break;	
			}
			/* Save this character to string */
			else
				*str++ = ch;
		}
		/* The string will start after this first '"' */
		else if( ch == '"' )
			state = 1;
	}
	return( ch );
}


/* Display the Command Descriptor Block */
static void
show_cdb( cdb_t, cmd )
CDB_t *cdb_t; 
struct command *cmd;
{
	int op_code;

	/* Display the command name */
	printf( "%s Command\n", cmd->name );

	/* If cdb display has been turned off */
	if( cmd->mode == 's' )
		return;

	op_code = cdb_t->Byte[0];

	/* Display the command data */
	switch( op_code )
	{
	case SCSI_Inquiry:
		show_inquiry( cdb_t );
		break;
	case SCSI_Read_10:
		show_read( cdb_t );
		break;
	case SCSI_Write_10:
		show_write( cdb_t );
		break;
	case SCSI_WriteBuffer:
		show_writebuffer( cdb_t );
		break;
	case SCSI_ModeSense_10:
		show_modesense( cdb_t );
		break;
	case SCSI_ModeSelect_10:
		show_modeselect( cdb_t );
		break;
	case SCSI_RequestSense:
		show_requestsense( cdb_t );
		break;
	case SCSI_ReadCapacity:
		show_readcapacity( cdb_t );
		break;
	case SCSI_BlockVerify:
		show_blockverify( cdb_t );
		break;
	case SCSI_WriteVerify:
		show_writeverify( cdb_t );
		break;
	case SCSI_Format:
		show_format( cdb_t );
		break;
	default:
		show_unknown( cdb_t );
	}
}
		
/* Display the command data for read capacity */
static void
show_readcapacity( cdb_t )
CDB_t *cdb_t;
{
	Read_Capacity_CDB_t *rcp_cdb_t;

	rcp_cdb_t = (Read_Capacity_CDB_t *)cdb_t;
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", rcp_cdb_t->Op_Code );
	printl( cdb_t, 1 );
	printf( "LUN (%x)  ", rcp_cdb_t->LUN );
	printf( "Resv1 (%x)   ", rcp_cdb_t->Resv1 );
	printf( "RelAdr (%x)", rcp_cdb_t->RelAdr );
	printl( cdb_t, 2 );
	printf( "Logical Block Address[0] (%02x)", rcp_cdb_t->Logical_Blk_Addr[0] );
	printl( cdb_t, 3 );
	printf( "Logical Block Address[1] (%02x)", rcp_cdb_t->Logical_Blk_Addr[1] );
	printl( cdb_t, 4 );
	printf( "Logical Block Address[2] (%02x)", rcp_cdb_t->Logical_Blk_Addr[2] );
	printl( cdb_t, 5 );
	printf( "Logical Block Address[3] (%02x)", rcp_cdb_t->Logical_Blk_Addr[3] );
	printl( cdb_t, 6 );
	printf( "Resv2[0] (%02x)", rcp_cdb_t->Resv2[0] );
	printl( cdb_t, 7 );
	printf( "Resv2[1] (%02x)", rcp_cdb_t->Resv2[1] );
	printl( cdb_t, 8 );
	printf( "Resv3 (%x)   ", rcp_cdb_t->Resv3 );
	printf( "PMI (%x)", rcp_cdb_t->PMI );
	printl( cdb_t, 9 );
	printf( "Control (%02x)", rcp_cdb_t->Control_Byte );
	printf( "\n");
}
 

/* Display the command data for block verify */
static void
show_blockverify( cdb_t )
CDB_t *cdb_t;
{
	Verify_CDB_t *ver_cdb_t;

	ver_cdb_t = (Verify_CDB_t *)cdb_t;
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", ver_cdb_t->Op_Code );
	printl( cdb_t, 1 );
	printf( "LUN (%x)  ", ver_cdb_t->LUN );
	printf( "DPO (%x)  ", ver_cdb_t->DPO );
	printf( "Resv1 (%x)   ", ver_cdb_t->Resv1 );
	printf( "ParityChk (%x)  ", ver_cdb_t->ParityChk );
	printf( "BytChk (%x)  ", ver_cdb_t->BytChk );
	printf( "RelAdr (%x)", ver_cdb_t->RelAdr );
	printl( cdb_t, 2 );
	printf( "Logical Block Address[0] (%02x)", ver_cdb_t->Logical_Blk_Addr[0] );
	printl( cdb_t, 3 );
	printf( "Logical Block Address[1] (%02x)", ver_cdb_t->Logical_Blk_Addr[1] );
	printl( cdb_t, 4 );
	printf( "Logical Block Address[2] (%02x)", ver_cdb_t->Logical_Blk_Addr[2] );
	printl( cdb_t, 5 );
	printf( "Logical Block Address[3] (%02x)", ver_cdb_t->Logical_Blk_Addr[3] );
	printl( cdb_t, 6 );
	printf( "Resv2 (%02x)", ver_cdb_t->Resv2 );
	printl( cdb_t, 7 );
	printf( "Verification Length[0] (%02x)", ver_cdb_t->Verification_Length[0] );
	printl( cdb_t, 8 );
	printf( "Verification Length[1] (%02x)", ver_cdb_t->Verification_Length[1] );
	printl( cdb_t, 9 );
	printf( "Control (%02x)", ver_cdb_t->Control_Byte );
	printf( "\n");
}
		
/* Display the command data for write verify */
static void
show_writeverify( cdb_t )
CDB_t *cdb_t;
{
	Write_Verify_CDB_t *wrv_cdb_t;

	wrv_cdb_t = (Write_Verify_CDB_t *)cdb_t;
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", wrv_cdb_t->Op_Code );
	printl( cdb_t, 1 );
	printf( "LUN (%x)  ", wrv_cdb_t->LUN );
	printf( "DPO (%x)  ", wrv_cdb_t->DPO );
	printf( "Resv1 (%x)   ", wrv_cdb_t->Resv1 );
	printf( "ParityChk (%x)  ", wrv_cdb_t->ParityChk );
	printf( "BytChk (%x)  ", wrv_cdb_t->BytChk );
	printf( "RelAdr (%x)", wrv_cdb_t->RelAdr );
	printl( cdb_t, 2 );
	printf( "Logical Block Address[0] (%02x)", wrv_cdb_t->Logical_Blk_Addr[0] );
	printl( cdb_t, 3 );
	printf( "Logical Block Address[1] (%02x)", wrv_cdb_t->Logical_Blk_Addr[1] );
	printl( cdb_t, 4 );
	printf( "Logical Block Address[2] (%02x)", wrv_cdb_t->Logical_Blk_Addr[2] );
	printl( cdb_t, 5 );
	printf( "Logical Block Address[3] (%02x)", wrv_cdb_t->Logical_Blk_Addr[3] );
	printl( cdb_t, 6 );
	printf( "Resv2 (%02x)", wrv_cdb_t->Resv2 );
	printl( cdb_t, 7 );
	printf( "Transfer Length[0] (%02x)", wrv_cdb_t->Transfer_Length[0] );
	printl( cdb_t, 8 );
	printf( "Transfer Length[1] (%02x)", wrv_cdb_t->Transfer_Length[1] );
	printl( cdb_t, 9 );
	printf( "Control (%02x)", wrv_cdb_t->Control_Byte );
	printf( "\n");
}
		

/* Display the command data for inquiry */
static void
show_inquiry( cdb_t )
CDB_t *cdb_t;
{
	Inquiry_CDB_t *inq_cdb_t;

	inq_cdb_t = (Inquiry_CDB_t *)cdb_t;
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", inq_cdb_t->Op_Code );
	printl( cdb_t, 1 );
	printf( "LUN (%x)  ", inq_cdb_t->LUN );
	printf( "Resv1 (%x)  ", inq_cdb_t->Resv1 );
	printf( "EVPD (%x)", inq_cdb_t->EVPD );
	printl( cdb_t, 2 );
	printf( "Page Code (%02x)", inq_cdb_t->Pg_Code );
	printl( cdb_t, 3 );
	printf( "Reserved (%02x)", inq_cdb_t->Reserved );
	printl( cdb_t, 4 );
	printf( "Allocation Length (%02x)", inq_cdb_t->Allocation_Length );
	printl( cdb_t, 5 );
	printf( "Control (%02x)", inq_cdb_t->Control_byte );
	printf( "\n");
}


/* Display the command data for read */
static void
show_read( cdb_t )
CDB_t *cdb_t;
{
	Read_Write_CDB_t *rw_cdb_t;

	rw_cdb_t = (Read_Write_CDB_t *)cdb_t;
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", rw_cdb_t->Op_Code );
	printl( cdb_t, 1 );
	printf( "LUN (%x)   ", rw_cdb_t->LUN );
	printf( "DPO (%x)   ", rw_cdb_t->DPO );
	printf( "FUA (%x)   ", rw_cdb_t->FUA );
	printf( "Resv1 (%x)   ", rw_cdb_t->Resv1 );
	printf( "RelAdr (%x)", rw_cdb_t->RelAdr );
	printl( cdb_t, 2 );
	printf( "Logical Block Address[0] (%02x)", rw_cdb_t->Logical_Blk_Addr[0] );
	printl( cdb_t, 3 );
	printf( "Logical Block Address[1] (%02x)", rw_cdb_t->Logical_Blk_Addr[1] );
	printl( cdb_t, 4 );
	printf( "Logical Block Address[2] (%02x)", rw_cdb_t->Logical_Blk_Addr[2] );
	printl( cdb_t, 5 );
	printf( "Logical Block Address[3] (%02x)", rw_cdb_t->Logical_Blk_Addr[3] );
	printl( cdb_t, 6 );
	printf( "Resv2 (%02x)", rw_cdb_t->Resv2 );
	printl( cdb_t, 7 );
	printf( "Transfer Length[0] (%02x)", rw_cdb_t->Transfer_Length[0] );
	printl( cdb_t, 8 );
	printf( "Transfer Length[1] (%02x)", rw_cdb_t->Transfer_Length[1] );
	printl( cdb_t, 9 );
	printf( "Control (%02x)", rw_cdb_t->Control_Byte );
	printf( "\n");
}

/* Display the command data for write */
static void
show_write( cdb_t )
CDB_t *cdb_t;
{
	Read_Write_CDB_t *rw_cdb_t;

	rw_cdb_t = (Read_Write_CDB_t *)cdb_t;
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", rw_cdb_t->Op_Code );
	printl( cdb_t, 1 );
	printf( "LUN (%x)   ", rw_cdb_t->LUN );
	printf( "DPO (%x)   ", rw_cdb_t->DPO );
	printf( "FUA (%x)   ", rw_cdb_t->FUA );
	printf( "Resv1 (%x)   ", rw_cdb_t->Resv1 );
	printf( "RelAdr (%x)", rw_cdb_t->RelAdr );
	printl( cdb_t, 2 );
	printf( "Logical Block Address[0] (%02x)", rw_cdb_t->Logical_Blk_Addr[0] );
	printl( cdb_t, 3 );
	printf( "Logical Block Address[1] (%02x)", rw_cdb_t->Logical_Blk_Addr[1] );
	printl( cdb_t, 4 );
	printf( "Logical Block Address[2] (%02x)", rw_cdb_t->Logical_Blk_Addr[2] );
	printl( cdb_t, 5 );
	printf( "Logical Block Address[3] (%02x)", rw_cdb_t->Logical_Blk_Addr[3] );
	printl( cdb_t, 6 );
	printf( "Resv2 (%02x)", rw_cdb_t->Resv2 );
	printl( cdb_t, 7 );
	printf( "Transfer Length[0] (%02x)", rw_cdb_t->Transfer_Length[0] );
	printl( cdb_t, 8 );
	printf( "Transfer Length[1] (%02x)", rw_cdb_t->Transfer_Length[1] );
	printl( cdb_t, 9 );
	printf( "Control (%02x)", rw_cdb_t->Control_Byte );
	printf( "\n");
}

/* Display the command data for write buffer */
static void
show_writebuffer( cdb_t )
CDB_t *cdb_t;
{
	Write_Buffer_CDB_t *wb_cdb_t;

	wb_cdb_t = (Write_Buffer_CDB_t *)cdb_t;
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", wb_cdb_t->Op_Code );
	printl( cdb_t, 1 );
	printf( "LUN (%x)   ", wb_cdb_t->LUN );
	printf( "Reserved (%x)   ", wb_cdb_t->Reserved );
	printf( "Mode (%x)", wb_cdb_t->Mode );
	printl( cdb_t, 2 );
	printf( "Buffer ID (%02x)", wb_cdb_t->_8_bytes.Buffer_Info.Buffer_ID );
	printl( cdb_t, 3 );
	printf( "Buffer Offset[0] (%02x)", wb_cdb_t->_8_bytes.Buffer_Info.Buffer_Offset[0] );
	printl( cdb_t, 4 );
	printf( "Buffer Offset[1] (%02x)", wb_cdb_t->_8_bytes.Buffer_Info.Buffer_Offset[1] );
	printl( cdb_t, 5 );
	printf( "Buffer Offset[2] (%02x)", wb_cdb_t->_8_bytes.Buffer_Info.Buffer_Offset[2] );
	printl( cdb_t, 6 );
	printf( "Parameter List Length[0] (%02x)", wb_cdb_t->_8_bytes.Buffer_Info.Param_List_Length[0] );
	printl( cdb_t, 7 );
	printf( "Parameter List Length[1] (%02x)", wb_cdb_t->_8_bytes.Buffer_Info.Param_List_Length[1] );
	printl( cdb_t, 8 );
	printf( "Parameter List Length[2] (%02x)", wb_cdb_t->_8_bytes.Buffer_Info.Param_List_Length[2] );
	printl( cdb_t, 9 );
	printf( "Control (%02x)", wb_cdb_t->_8_bytes.Buffer_Info.Control_Byte );
	printf( "\n");
}

/* Display the command data for mode sense */
static void
show_modesense( cdb_t )
CDB_t *cdb_t;
{
	Mode_Sense_CDB_t *msn_cdb_t;

	msn_cdb_t = (Mode_Sense_CDB_t *)cdb_t;
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", msn_cdb_t->Op_Code );
	printl( cdb_t, 1 );
	printf( "LUN (%x)   ", msn_cdb_t->LUN );
	printf( "Resv2 (%x)   ", msn_cdb_t->Resv2 );
	printf( "DBD (%x)   ", msn_cdb_t->DBD );
	printf( "Resv1 (%x)  ", msn_cdb_t->Resv1 );
	printl( cdb_t, 2 );
	printf( "Page Control(%x)   ", msn_cdb_t->Pg_Ctrl );
	printf( "Page Code (%x)", msn_cdb_t->Pg_Code );
	printl( cdb_t, 3 );
	printf( "Resv3[0] (%02x)", msn_cdb_t->Resv3[0]);
	printl( cdb_t, 4 );
	printf( "Resv3[1] (%02x)", msn_cdb_t->Resv3[1]);
	printl( cdb_t, 5 );
	printf( "Resv3[2] (%02x)", msn_cdb_t->Resv3[2]);
	printl( cdb_t, 6 );
	printf( "Resv3[3] (%02x)", msn_cdb_t->Resv3[3]);
	printl( cdb_t, 7 );
	printf( "Allocation Length[0] (%02x)", msn_cdb_t->Allocation_Length[0] );
	printl( cdb_t, 8 );
	printf( "Allocation Length[1] (%02x)", msn_cdb_t->Allocation_Length[1] );
	printl( cdb_t, 9 );
	printf( "Control (%02x)", msn_cdb_t->Control_Byte );
	printf( "\n");
}

/* Display the command data for mode select */
static void
show_modeselect( cdb_t )
CDB_t *cdb_t;
{
	Mode_Select_CDB_t *msl_cdb_t;

	msl_cdb_t = (Mode_Select_CDB_t *)cdb_t;
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", msl_cdb_t->Op_Code );
	printl( cdb_t, 1 );
	printf( "LUN (%x)   ", msl_cdb_t->LUN );
	printf( "PF (%x)   ", msl_cdb_t->PF );
	printf( "Reserved (%x)   ", msl_cdb_t->Reserved );
	printf( "SP (%x)", msl_cdb_t->SP );
	printl( cdb_t, 2 );
	printf( "Resv1[0] (%02x)  ", msl_cdb_t->Resv1[0] );
	printl( cdb_t, 3 );
	printf( "Resv1[1] (%02x)  ", msl_cdb_t->Resv1[1] );
	printl( cdb_t, 4 );
	printf( "Resv1[2] (%02x)  ", msl_cdb_t->Resv1[2] );
	printl( cdb_t, 5 );
	printf( "Resv1[3] (%02x)  ", msl_cdb_t->Resv1[3] );
	printl( cdb_t, 6 );
	printf( "Resv1[4] (%02x)  ", msl_cdb_t->Resv1[4] );
	printl( cdb_t, 7 );
	printf( "Param_List_Length[0] (%02x)", msl_cdb_t->Param_List_Length[0] );
	printl( cdb_t, 8 );
	printf( "Param_List_Length[1] (%02x)", msl_cdb_t->Param_List_Length[1] );
	printl( cdb_t, 9 );
	printf( "Control (%02x)", msl_cdb_t->Control_Byte );
	printf( "\n");
}

/* Display the command data for request sense */
static void
show_requestsense( cdb_t )
CDB_t *cdb_t;
{
	Request_Sense_CDB_t *rsn_cdb_t;

	rsn_cdb_t = (Request_Sense_CDB_t *)cdb_t;
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", rsn_cdb_t->Op_Code );
	printl( cdb_t, 1 );
	printf( "LUN (%x)  ", rsn_cdb_t->LUN );
	printf( "Reserved (%x)  ", rsn_cdb_t->Reserved );
	printf( "AEN (%x)", rsn_cdb_t->AEN );
	printl( cdb_t, 2 );
	printf( "Resv1 (%02x)", rsn_cdb_t->Resv1 );
	printl( cdb_t, 3 );
	printf( "Resv2 (%02x)", rsn_cdb_t->Resv2 );
	printl( cdb_t, 4 );
	printf( "Allocation Length (%02x)", rsn_cdb_t->Allocation_Length );
	printl( cdb_t, 5 );
	printf( "Control (%02x)", rsn_cdb_t->Control_Byte );
	printf( "\n");
}


/* Display the format command */
static void
show_format( cdb_t )
CDB_t *cdb_t;
{
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", cdb_t->Byte[0] );
	printl( cdb_t, 1 );
	printf( "LUN / Reserved / Format Type (%02x)", cdb_t->Byte[1] );
	printl( cdb_t, 2 );
	printf( "Transfer Length[0] (%02x)", cdb_t->Byte[2] );
	printl( cdb_t, 3 );
	printf( "Transfer Length[1] (%02x)", cdb_t->Byte[3] );
	printl( cdb_t, 4 );
	printf( "Transfer Length[2] (%02x)", cdb_t->Byte[4] );
	printl( cdb_t, 5 );
	printf( "Control (%02x)", cdb_t->Byte[5] );
	printf( "\n");
}


/* Check the input data file type for hex or formated */
static int 
is_hex( fptr )
FILE *fptr;
{
	int ch;
	int hex;

	hex = FALSE;

	/* Loop through the file looking for ':' indicating formated data */
	while( (ch = getc( fptr )) != EOF )
	{
		if( ch == ':' )
		{
			hex = TRUE; 
			break;
		}
	}

	/* Return file pointer to start of file */
	fseek( fptr, 0L, SEEK_SET );
	return( hex );
}
		
/* Display command data for unknown command */
static void
show_unknown( cdb_t )
CDB_t *cdb_t;
{
	printl( cdb_t, 0 );
	printf( "Op Code (%02x)", cdb_t->Byte[0] );
	printl( cdb_t, 1 );
	printl( cdb_t, 2 );
	printl( cdb_t, 3 );
	printl( cdb_t, 4 );
	printl( cdb_t, 5 );
	printl( cdb_t, 6 );
	printl( cdb_t, 7 );
	printl( cdb_t, 8 );
	printl( cdb_t, 9 );
	printf( "\n");
}

/* Display the next byte of data from the cdb */
static void
printl( cdb_t, row )
int row;
CDB_t *cdb_t;
{
	printf( "\n%02x:   %02x   ", row, cdb_t->Byte[row] );
}

/* Read and format the output data */
static unsigned long
get_outdata( buffer, data_file )
unsigned char *buffer;
char *data_file;
{
	int hex;
	unsigned char *bp;
	FILE *fptr;

	if( (fptr = fopen( data_file, "rb" )) == NULL)
	{
		printf( "io_test error: file open %s\n", data_file );
		return( 0 );
	}

	/* Check for file type as hex or formated */
	hex = is_hex( fptr );
	bp = buffer;

	/* Read and format data */
	if( hex == TRUE )
		get_hexdata( fptr, bp );
	else
		get_fmtdata( fptr, bp );

	fclose( fptr );
	return( bp - buffer );
}

/* Read a hex type output data file and store data for return to calling program
*/
static void
get_hexdata( fptr, bp )
FILE *fptr;
unsigned char *bp;
{
	int ch;

	/* Loop through data file */
	while( (ch = get_digit( fptr )) >= 0)     
	{
		/* Each byte is represented by two hex ascii characters */
		*bp = (u_char)ch << 4;
		ch = get_digit( fptr );
		*bp++ += (u_char) ch;
	}	
}

/* Read an ascii character and convert to hex */ 
static int
get_digit( fptr )
FILE *fptr;
{
	int ch, digit;
	static int state = 0;

	/* Read until next valid data character located */
	while( (ch = getc( fptr )) != EOF )
	{
		switch( ch )
		{
		/* End of line so set state to look for next ':' */
		case LF:
			state = 0;
			break;
		/* End of line so set state to look for next ':' */
		case CR:
			state = 0;
			break;
		/* ':' signifies start of data for that line */
		case ':':
			state = 1;
			break;
		}

		/* State will be 1 only after a ':' has been read for each line */
		if( state == 1 )
		{ 
			/* Convert ascii to hex */
			if( (digit = convert_char( ch )) >= 0 )
				return( digit );
		}
	}
	state = 0;
	return(ERROR);
}


/* Read a formated output data file and store data for return to calling program
*/
static void
get_fmtdata( fptr, bp )
FILE *fptr;
unsigned char *bp;
{
	int ch, val, save;
	int shift, shift_val;
	int swap;
	int state;

	state = 0;
	shift = -1;
	swap = FALSE;
	val = shift_val = 0;

	/* Read through file */
	while( (ch = getc( fptr )) != EOF )
	{
		/* Ignore these characters */
		if( (ch != ' ') && (ch != TAB) && (ch != CR) && (ch != LF) )
		{
		switch( state )
		{
		/* Look for start of field */
		case 0:
			switch( ch )
			{
			case '-':
				state = 3;
				break;
			case '"':
				state = 5;
				break;
			case '(':
				if( shift >= 0 )
					state = 6;
				else 
					state = 1;
				break;
			default:
				swap = FALSE;
			}
			break;

		/* Get first hex ascii byte character */
		case 1:
			if( ch == ')' )
			{
				swap = FALSE;
				state = 0;
			}
			else
			{
				if( (val = convert_char( ch )) >= 0 ) 
				{
					state = 2;
				}
			}
			break;      

		/* Get first hex ascii byte character */
		case 2:
			if( ch == ')' )
			{
				*bp++ = (unsigned char)val; 
				swap = FALSE;
				state = 0;
			}
			else if( swap == TRUE  )
			{
				save = (val << 4) + convert_char( ch );
				state = 7;
			}
			else
			{
				*bp++ = (unsigned char)( (val << 4) + convert_char( ch ) ); 
				state = 1;
			}
			break;      

		/* Read field modifier r for shift bits, s for swap bytes */
		case 3:
			if( ch == 'r' )
				state = 4;
			else if( ch == 's' )
			{
				swap = TRUE;
				state = 0;
			}
			else
				state = 0;
			break;

		/* Convert character to be bit shifted to hex */
 		case 4:
			shift = convert_char( ch );
			state = 0;
			break;

		/* Get string field or terminate field on '"' */
		case 5:
			if( ch == '"' )
				state = 0;
			else
				*bp++ = ch;
			break;

		/* Get first character of second byte for swapping */
		case 6:
			if( ch == ')' )
			{
				if( shift > 0 )
					shift_val |= val << shift;
				else if( shift == 0 )
				{
					*bp++ = (unsigned char)(shift_val | val);
					shift_val = 0;
					shift = -1;
				}
				val = 0;
				state = 0;
			}
			else
				val = (val << 4) + convert_char( ch );
			break;

		/* Get second character of second byte for swapping */
		case 7:
			val = convert_char( ch );
			state = 8;
			break;      

		/* Swap bytes */
		case 8:
			*bp++ = (unsigned char)( (val << 4) + convert_char( ch ) );
			*bp++ = (unsigned char)save; 
			state = 1;
			break;      
		}
		}
	}	
}

/* Convert ascii character to hex */
static int
convert_char( ch )
int ch;   
{
	if( ch >= '0' && ch <= '9' ) 
		ch -= '0';
	else if( ch >= 'a' && ch <= 'f' )
		ch += 0x0a - 'a';
	else if( ch >= 'A' && ch <= 'F' )
		ch += 0x0a - 'A';
	else
		ch = -1;

	return(ch);
}
		
/* Display the input data */
static void
show_indata( in_cdb_t )
CDB_t *in_cdb_t;
{
	int i, cols, rows;
	FILE *fd;

	/* Open a file to be used for data display */
	if( (fd = fopen( "/tmp/test/datatemp", "wb" )) == NULL )
	{
		perror( "Error opening file\n" );
		return;
	}

	/* Write the formated data to the file */
	fprintf( fd, "                              Input Buffer\n\n" );
	fprintf( fd, "         cdb_t->Buffer (%x)   ", in_cdb_t->Buffer );
	fprintf( fd, "cdb_t->BufferLength (%x)\n\n", in_cdb_t->BufferLength );

	/* Display a row of data */
	cols = 0;
	rows = 0;
	for( i = 0; i < in_cdb_t->BufferLength; i++ )
	{
		if( cols-- == 0 )
		{
			fprintf(fd, " \n ");
			fprintf(fd, " %04x: ", rows );
			cols = 0x0f;
		 	rows += 0x10;
		}
		fprintf( fd, " %02x ", in_cdb_t->Buffer[i] );
	}
	fprintf( fd, "\n\0" ); 
	fclose( fd );

	/* Use vi to diaplay data */
	system( "vi datatemp" );
	remove( "datatemp" );
}

