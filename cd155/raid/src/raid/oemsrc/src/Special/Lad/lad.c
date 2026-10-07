/*
 * 
 * $Copyright
 * Copyright 1992 Intel Corporation
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
 Name:		lad.c
 Title:		List Array Devices Program
 Version:	   
 Revision:	$Revision: 1.2 $
 Update Date:	$Date: 1994/12/13 23:34:29 $
 Programmer:	rmj
 Documents:	1.  UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2.  "Object-Oriented Programming in C", C User's Journal 7/90

 COPYRIGHT 1991-92, NCR Corp.

 Description:	The "lad" program finds all of the disk array storage devices
		connected to the system and prints a list of the corresponding
		NCR UNIX SVR4 device names to standard out.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mkdev.h>
#include "stddefs.h"
#include "dau_err.h"
#include "getrel.h"

/******************************************************************************
 ***				  DEFINITIONS 				    ***
 *****************************************************************************/

#define DEV_MODE	( S_IFCHR | S_IRWXU | S_IRGRP | S_IROTH )

/***************************************************************************
The following code is commented out from Raid Manager 1.00.00. 
Raid Manager 1.01.00 will scan all possible locations. 
Sysadm in Raid Manager 1.01.00 will scan locations once.

#undef	IO_BUSSES_PER_MACHINE
#define	IO_BUSSES_PER_MACHINE		1 ** only scan 1 I/O bus **
#undef	SCSI_CONTROLLERS_PER_IO_BUS
#define SCSI_CONTROLLERS_PER_IO_BUS	2 ** only scan 2 controllers **
#undef	SCSI_BUSSES_PER_CONTROLLER
#define SCSI_BUSSES_PER_CONTROLLER	1 ** only scan 1 bus per controller **

End of commented out code.

*****************************************************************************/

/******************************************************************************
 ***		             VARIABLE DEFINITIONS			    ***
 *****************************************************************************/

int Argc;
char **Argv;
int Interactive = FALSE;
char *flag_string = " ", *id_string = "ADP-92";
static char ident[] = PROJREL "/1.6 - lad.c, List Array Devices Program";
int n_optset=FALSE, pun_cutoff=8, lun_cutoff=8, host_pun=7, first_pun=7;
int io_busses = IO_BUSSES_PER_MACHINE;
int scsi_controllers = NUMBER_OF_CONTROLLERS_PER_IO_BUS;
int scsi_busses = SCSI_BUSSES_PER_CONTROLLER;

/******************************************************************************
 ***		             EXTERNAL REFERENCES			    ***
 *****************************************************************************/

extern char *optarg;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/

main( argc, argv )
int argc;
char *argv[];
{
	int n, opt, ibus, hcon, sbus, pun, lun;

	/* "error" interface needs following two variables set up */
	Argc = argc;
	Argv = argv;

	while ( ( opt = getopt( argc, argv, "n:i:p:l:h:f:s:b:m:" ) ) != EOF ) {
		switch ( opt ) {
			case 'n':
				n_optset = TRUE;
				flag_string = optarg;
				break;
			case 'i':
				id_string = optarg;
				break;
			case 'p': /* un-documented option - no. of puns to 
				look at on each bus */
				pun_cutoff = atoi( optarg );
				/* might want a range-check here */
				break;
			case 'l': /* un-documented option - no. of luns to
				look at on each pun */
				lun_cutoff = atoi( optarg );
				/* might want a range-check here */
				break;
			case 'h': /* un-documented option - ID of the host */
				host_pun = atoi( optarg );
				/* might want a range-check here */
				break;
			case 'f': /* un-documented option - starting pun ID */
				first_pun = atoi( optarg );
				/* might want a range-check here */
				break;
			case 's': /* un-documented option - no. of scsi
				controllers to scan */
				scsi_controllers = atoi( optarg );
				/* might want a range-check here */
				break;
			case 'b': /* un-documented option - no. of scsi
				busses per controller to scan */
				scsi_busses = atoi( optarg );
				/* might want a range-check here */
				break;
			case 'm': /* un-documented option - no. of mca
				busses per machine to scan */
				io_busses = atoi( optarg );
				/* might want a range-check here */
				break;
			default:
				error( COMMAND_LINE_SYNTAX, NO_MESSAGE );
				break;
		} 
	}
	if ( ( first_pun + 1 ) - pun_cutoff  < 0 )
		error( ARGUMENT_RANGE, INVALID_PF_OPTS );
	for ( ibus = 0; ibus < io_busses; ibus++ ) {
 		for (hcon=0; hcon < scsi_controllers; hcon++) {
			for (sbus=0; sbus < scsi_busses; sbus++) {
				for (n=0,pun=first_pun;n<pun_cutoff;n++,pun-- ){
					check_controller( ibus, hcon, sbus,
					    pun );
				}
			}
		}
	}
	exit( 0 );

}

static
check_controller( ibus, hcon, sbus, pun )
int ibus, hcon, sbus, pun;
{
	SCSI_Inquiry_Data_t inq_data;
	int s, lun, n_opt_triggered = FALSE;

	if ( pun == host_pun )
		return;
	for (lun=0; lun < lun_cutoff; lun++) {
		s = inquire( ibus, hcon, sbus, pun, lun, &inq_data );
		debug( "status from inquire = %x\n", s );
		{
			char buf[32];
			strncpy( buf, inq_data.Product_ID, 16 );
			buf[16] = NULL;
			if ( !s ) debug( "product id = %s\n", buf );
		}
		if ( s || strncmp( inq_data.Product_ID, id_string,
		    strlen( id_string ) ) ||
		    ( inq_data.Periph_Device_Type != SCSI_DAD_TYPE &&
		    inq_data.Periph_Device_Type != SCSI_UNKNOWN_TYPE ) ) {
			return;
		} else {
			if ( inq_data.Periph_Qualifier == NOT_CONNECTED ) {
				if ( n_optset && ! n_opt_triggered ) {
					n_opt_triggered = TRUE;
					print_dev(ibus, hcon, sbus, pun, lun);
					printf( " %s\n", flag_string );
				}
			} else  {
				print_dev(ibus, hcon, sbus, pun, lun);
				printf( "\n" );
			}
		}
	}
}

static int
inquire( ibus, hcon, sbus, pun, lun, inq_data )
int ibus, hcon, sbus, pun, lun;
SCSI_Inquiry_Data_t *inq_data;
{
	int s, fd;
	dev_t dev;
	OS1_Device_t minor1;
	OS2_Device_t minor2;
	minor_t *SCSI_Minor1 = ( minor_t * ) &minor1;
	minor_t *SCSI_Minor2 = ( minor_t * ) &minor2;
	u_long release_num;
	char *devfile = "/tmp/dev_XXXXXX";
 
	/* determine the OS release */
	release_num=get_release();

	/* build the minor number depending on the OS release */
	if (release_num < 200) {
		minor1.Partition = 0;
		minor1.PUN = pun;
		minor1.LUN = lun;
		minor1.SCSI_Bus = sbus;
		minor1.SCSI_Controller = hcon;
		minor1.IO_Bus = ibus;
		minor1.OS_Major = 0;
	}
	else {
		minor2.Partition = 0;
		minor2.PUN = pun;
		minor2.LUN = lun;
		minor2.SCSI_Bus = sbus;
		minor2.SCSI_Controller = hcon;
		minor2.IO_Bus = ibus;
		minor2.OS_Major = 0;
	}

	/* determine release so that correct minor # struct is used */
	if (release_num < 200) {
		debug( "ibus, etc = %d %d %d %d %d\n",ibus,hcon,sbus,pun,lun);
		if ( ( dev = makedev( SCSI_MAJOR, *SCSI_Minor1 ) ) <= 0 ) {
			debug( "bailing out of inquire after makedev\n" );
			return( -1 );
		}
	}
	else {
		debug( "ibus, etc = %d %d %d %d %d\n",ibus,hcon,sbus,pun,lun);
		if ( ( dev = makedev( SCSI_MAJOR, *SCSI_Minor2 ) ) <= 0 ) {
			debug( "bailing out of inquire after makedev\n" );
			return( -1 );
		}
	}

	debug( "returned dev = %x\n", dev );
	mktemp( devfile );
	if ( mknod( devfile, DEV_MODE, dev ) < 0 ) {
		debug( "bailing out of inquire after mknod\n" );
		return( -1 );
	}
	if ( ( fd = IO_Open( devfile, O_RDONLY ) ) < 0 ) {
		debug( "bailing out of inquire after IO_Open\n" );
		unlink( devfile );
		return( -1 );
	}
	s = IO_Inquiry( fd, inq_data );
	{
		extern int errno;

		debug( "did the IO_Inquiry, s = %d, errno = %d\n", s, errno );
		debug( "     fd = %x, inq_data = %x\n", fd, inq_data );
	}
	if ((lun > 0) || (release_num < 103)) {  /* for UNIX V.4 release 1.03
					            and later keep lun 0 open */
		IO_Close( fd ); 		/* it closes when lad is done */
	}
	unlink( devfile );
	return( s );
}

#define	ROOT	( (uid_t) 0 )
#define	SYSGRP	( (gid_t) 3 )

static
print_dev( ibus, hcon, sbus, pun, lun )
int ibus, hcon, sbus, pun, lun;
{
	dev_t dev;
	char devfile[64];
	OS1_Device_t minor1;
	OS2_Device_t minor2;
	minor_t *SCSI_Minor1 = ( minor_t * ) &minor1;
	minor_t *SCSI_Minor2 = ( minor_t * ) &minor2;
	u_long release_num;

	if ( ibus == 0 )
		if ( hcon == 0 )
			sprintf( devfile, "/dev/rdsk/c%xt%xd%xs0", sbus,pun,lun );
		else
			sprintf( devfile, "/dev/rdsk/c%x%xt%xd%xs0", hcon,sbus,pun,lun );
	else
		sprintf( devfile, "/dev/rdsk/c%x%x%xt%xd%xs0", ibus,hcon,sbus,pun,lun );
	printf( "%s", devfile );

	/* Go ahead and make the node */

	/* determine the OS release */
	release_num=get_release();

	/* build the minor number depending on the OS release */
	if (release_num < 200) {
		minor1.Partition = 0;
		minor1.PUN = pun;
		minor1.LUN = lun;
		minor1.SCSI_Bus = sbus;
		minor1.SCSI_Controller = hcon;
		minor1.IO_Bus = ibus;
		minor1.OS_Major = 0;
	}
	else {
		minor2.Partition = 0;
		minor2.PUN = pun;
		minor2.LUN = lun;
		minor2.SCSI_Bus = sbus;
		minor2.SCSI_Controller = hcon;
		minor2.IO_Bus = ibus;
		minor2.OS_Major = 0;
	}

	if (release_num < 200) {
		if ( ( dev = makedev( SCSI_MAJOR, *SCSI_Minor1 ) ) > 0 )
			if ( mknod( devfile, ( S_IFCHR | 0600 ), dev ) >=0 )
				chown( devfile, ROOT, SYSGRP );
	}
	else {
		if ( ( dev = makedev( SCSI_MAJOR, *SCSI_Minor2 ) ) > 0 )
			if ( mknod( devfile, ( S_IFCHR | 0600 ), dev ) >=0 )
				chown( devfile, ROOT, SYSGRP );
	}
}
