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
 Name:		io_test.h
 Title:		SYSIO Test Header 
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:35:14 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the header file for testing scsi io.
*/

/******************************************************************************
 ***				   DEFINITIONS		    	    	    ***
 *****************************************************************************/
#define SENSE_DATA_PRESENT   -2
#define OK                    0
#define ERROR		     -1
#define LF                 0x0a
#define CR		   0x0d
#define TAB		   0x09

/******************************************************************************
 ***				   EXTERNAL VARIABLES	    	    	    ***
 *****************************************************************************/
SCSI_Sense_Data_t SenseData;  /* global var used by libio */

/******************************************************************************
 ***				   STRUCTURES 				    ***
 *****************************************************************************/
struct command 
{
	int op_code;
	int pg_code;
	unsigned char mode;
	char name[50];
	char data_file[50];
	char sense_file[50];
};

/******************************************************************************
 ***				  PROTOTYPES                                *** 
 *****************************************************************************/
static int init_cmd( CDB_t *cdb_t, struct command *cmd );
static int get_mode( FILE *fptr, unsigned char *mode );
static int get_hex( FILE *fptr, int *hex );
static int get_str( FILE *fptr, char *str );
static void show_cdb( CDB_t *cdb_t, struct command *cmd );
static void show_inquiry( CDB_t *cdb_t );
static void get_hexdata( FILE *fptr, unsigned char *bp );
static void get_fmtdata( FILE *fptr, unsigned char *bp );
static int get_digit( FILE *fptr );
static int convert_char( int ch );
static unsigned long get_outdata( unsigned char *buffer, char *out_file );
static void show_indata( CDB_t *in_cdb_t );
static void show_read( CDB_t *cdb_t );
static void show_write( CDB_t *cdb_t );
static void show_writebuffer( CDB_t *cdb_t );
static void show_modesense( CDB_t *cdb_t );
static void show_modeselect ( CDB_t *cdb_t );
static void show_requestsense ( CDB_t *cdb_t );
static void show_writeverify( CDB_t *cdb_t );
static void show_blockverify( CDB_t *cdb_t );
static void show_readcapacity( CDB_t *cdb_t );
static void show_format ( CDB_t *cdb_t );
static void show_unknown ( CDB_t *cdb_t );
static void printl( CDB_t *cdb_t, int row );

