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
 Name:		hc_ptol.c
 Title:		Slot-to-Logical-Adapter-Number Translation
 Version:	   
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:35:40 $
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991-92, NCR Corp.

 Description:	This module contains the function hc_ptol (host controller
		physical-to-logical) function.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "getrel.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/mkdev.h>
#include <fcntl.h>

/******************************************************************************
 ***				   MACRO DEFINITIONS		    	    ***
 *****************************************************************************/
#define DEV_MODE	S_IFCHR | S_IRWXU | S_IRGRP | S_IROTH

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static void
make_nil_IO_device( iod )
struct IO_DeviceParameters *iod;
{
	/* Stuff some bogus values into iod */
	iod->dp_IO_BusNumber = -1;
	iod->dp_BaseAddress = -1;
	iod->dp_IO_BusSlot = 0xFF;
	iod->dp_SCSI_BusNumber = 0xFF;
	iod->dp_PUN = 0xFF;
	iod->dp_LUN = 0xFF;
}

#ifdef PARAGON860 /* remove 3 bit storage limitation for IOBus */
hw_zipcode
#else
int
#endif
hc_ptol( hw_addr ) /* host controller physical-to-logical */
hw_zipcode hw_addr;
{
	OS1_Device_t minor1;
	OS2_Device_t minor2;
	minor_t *SCSI_Minor1 = ( minor_t *)&minor1;
	minor_t *SCSI_Minor2 = ( minor_t *)&minor2;
	u_long release_num;
	int ctlr_num, RtnV;
	int fd;
	u_int mode;
	dev_t dev_num;
	char *dev_path = "/tmp/dev_XXXXXX";
	int DevOpen_Success = FALSE, SlotMatch = FALSE;
	hw_zipcode_t *hw = (hw_zipcode_t *) &hw_addr;
	struct IO_DeviceParameters SCSI_HA_Params;

#ifdef PARAGON860 /* distinguish between hc_ptol and translate_slot_to_ctlr
		   * in sysio.c */
	debug( "Entered hc_ptol\n" );
#else
	debug( "Entered translate_slot_to_ctlr\n" );
#endif

	/* We know everything we need for the minor (OS_Device_t struct),    */
	/* except for SCSI_Controller number and OS release number.          */

	/* determine the OS release */
	release_num=get_release();
#ifdef PARAGON860 /* force release level */
	release_num = 299;
#endif


	/* build the minor number depending on the OS release */
	if (release_num < 200) {
		minor1.OS_Major = 0;
		minor1.IO_Bus = hw->IO_Bus;
		minor1.SCSI_Controller = 0; /* Initialize to 0 since we don't */
					    /* know what proper value is yet. */
		minor1.SCSI_Bus = hw->SCSI_Bus;
		minor1.LUN = hw->LUN;
		minor1.PUN = hw->PUN;
		minor1.Partition = 0;
	}
	else {
		minor2.OS_Major = 0;
		minor2.IO_Bus = hw->IO_Bus;
		minor2.SCSI_Controller = 0; /* Initialize to 0 since we don't */
					    /* know what proper value is yet. */
		minor2.SCSI_Bus = hw->SCSI_Bus;
		minor2.LUN = hw->LUN;
		minor2.PUN = hw->PUN;
#ifdef PARAGON860
		minor2.Partition = 15;
#else
		minor2.Partition = 0;
#endif
	}

	for (ctlr_num=0;ctlr_num<NUMBER_OF_CONTROLLERS_PER_IO_BUS;ctlr_num++) {

		debug( "For loop: Trying controller number %d\n", ctlr_num );

		/* Initialize SCSI_HA_Params with some bogus values so,	     */
		/* later on, we can tell if IO_GetDeviceParameters() worked. */
		make_nil_IO_device( &SCSI_HA_Params );

		if (release_num < 200) { /* use the minor number depending */
					/* on the OS release	          */
			minor1.SCSI_Controller = ctlr_num;
			/* device name is composed of major#_minor# ie. 35_32 */
			sprintf( (char *) dev_path, "/tmp/%d_%d",
				SCSI_MAJOR, minor1 );
			/* make the device node */
			dev_num = makedev( SCSI_MAJOR, *SCSI_Minor1 );
		}
		else {
			minor2.SCSI_Controller = ctlr_num;
			/* device name is composed of major#_minor# ie. 35_32 */
#ifdef PARAGON860 /* add the IO_Bus (node number) to make the filename
		   * unique. */
			sprintf( (char *) dev_path, "/tmp/.%d_%d_%d",
				SCSI_MAJOR, (short)*SCSI_Minor2, hw->IO_Bus );
#else
			sprintf( (char *) dev_path, "/tmp/%d_%d",
				SCSI_MAJOR, minor2 );
#endif 
			/* make the device node */
			dev_num = makedev( SCSI_MAJOR, *SCSI_Minor2 );
		}

		if ( dev_num != NODEV ) {
#ifdef PARAGON860 /* use rmknod instead of mknod */
		
			RtnV = rmknod( (char *) dev_path, DEV_MODE, dev_num,
					hw->IO_Bus);
	debug("mknod path=%s, mode=%x, dev_num=%x node=%d \n", dev_path, DEV_MODE, dev_num, hw->IO_Bus);
			if ( (RtnV < 0) && (errno != EEXIST) ) {
				debug("mknod path=%s, mode=%x, dev_num=%x node=%x failed, errno=%d\n", dev_path, DEV_MODE, dev_num, hw->IO_Bus, errno);
			}
#else
			RtnV = mknod( (char *) dev_path, DEV_MODE, dev_num );
			if ( (RtnV < 0) && (errno != EEXIST) ) {
				debug("mknod path=%s, mode=%x, dev_num=%x failed, errno=%d\n", dev_path, DEV_MODE, dev_num, errno);
			}
#endif
			else {
				DevOpen_Success = TRUE;
			}
		}
		else {
			if (release_num < 200) {
				debug("makedev(%d, %d) failed\n",
				SCSI_MAJOR, minor1);
			}
			else {
				debug("makedev(%d, %d) failed\n",
				SCSI_MAJOR, minor2);
			}
		}

		if ( DevOpen_Success ) {
			/* open the device node and get an fd/handle */
			/* if open fails return an error status */
			mode = O_RDONLY;
			fd = DEV_Open( (char *) dev_path, mode );
			if ( fd < 0 ) {
				debug( "IO_open of %s failed\n", dev_path );
				DevOpen_Success = FALSE;
			}
		}

		if ( DevOpen_Success ) {
			/* Double check slot number by calling	*/
			/* IO_GetDeviceParameters()		*/
			if ( ( RtnV = IO_GetDeviceParameters( fd, &SCSI_HA_Params ) ) != 0 ) {
				debug( "IO_GetDeviceParameters: bad return value = %d\n", RtnV );
			}
			else {
#ifdef PARAGON860 /* confirm that node numbers match */
                        	if ( SCSI_HA_Params.dp_IO_BusNumber 
                               		 == (long) hw->IO_Bus)
                               		 ;
                        	else {
                               	 printf("node number mismatch:\n\
                                        SCSI_HA_Params.dp_IO_BusNumber = %d\n\
                                        hw->IO_Bus                     = %d\n",
                               	 SCSI_HA_Params.dp_IO_BusNumber,hw->IO_Bus);
                        	}
#endif
				if ( SCSI_HA_Params.dp_IO_BusSlot
				    == (u_char) hw->SCSI_Controller ) {
					SlotMatch = TRUE;
					debug( "Slot matched OK: slot=%d\n", hw->SCSI_Controller );
					break;
				}
				else {
					debug( "Slot mismatch\n" );
					IO_Close ( fd );
				}
			}
			DevOpen_Success = FALSE;
		}
		close( fd );
		unlink( dev_path );
	}

	close( fd );
	unlink( dev_path );
#ifdef PARAGON860 /* distinguish between hc_ptol and translate_slot_to_ctlr
		   * in sysio.c */
	debug( "Leaving hc_ptol\n" );
#else
	debug( "Leaving translate_slot_to_ctlr\n" );
#endif
	if ( DevOpen_Success && SlotMatch )
		return( ctlr_num );
	else
		return( -1 );
}
