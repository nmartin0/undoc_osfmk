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
 Name:		can_action.c
 Title:		CAN_ACTION Class Implementation
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 22:44:34 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the CAN_ACTION class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "scsi_dev.h"
#include "can_action.h"
#include "zip_dialog.h"

/******************************************************************************
 ***		             EXTERNAL REFERENCES			    ***
 *****************************************************************************/
extern ZIP_DIALOG *zip_dialog;
extern char *convert_zip();





/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static u_long
Do( this )
void *this;
#define THIS	( (CAN_ACTION *) this )
{

	char str_hw_addr[256];
	char command_buf[256];
	int status=0, exclusive;
	SCSI_DEV *sd;

	convert_zip(THIS->hw_addr, str_hw_addr);
#ifndef MSDOS
	strcpy(command_buf,"/bin/sh ");
#else
	command_buf[0] = 0;
#endif
	strcat(command_buf, THIS->ccf_shell_script_name);
	strcat(command_buf, " ");
	strcat(command_buf, str_hw_addr);
	status=system(command_buf);
	if (status != 0) error (NON_SPECIFIC_ERROR, SYSTEM_CALL_ERROR);
	return(0);
}

void
destroy_CAN_ACTION( this )
void *this;
{
	debug( "entered destroy_CAN_ACTION\n" );
	destroy_ACTION( (ACTION *) this );
}

CAN_ACTION *
new_CAN_ACTION( hw_addr, i )
hw_zipcode hw_addr;
int i;
{
	ACTION *s;
	CAN_ACTION *this;
	static int ccf_entry_num;
	static can_file_t ccf[64];
	int j=0, k=0;
	char buf[256], ccf_location[256], *delim_pos=NULL;
	FILE *fp;

	s = new_ACTION();
	this = (CAN_ACTION *) zalloc( sizeof( CAN_ACTION ) );
	memcpy( this, s, sizeof( ACTION ) ); /* inherit from ACTION */
	free( s );
	this->hw_addr = hw_addr;
	this->destroy = destroy_CAN_ACTION;
	this->Do = Do;

	/* build the canned configuration array from the ccf file */
	if (ccf_entry_num ==0) {
		/* open the canned configuration file */
#ifndef MSDOS
		strcpy (ccf_location,STDCFDIR);
		strcat (ccf_location, "/acf");
#else
		strcpy (ccf_location, "acf");
#endif
		fp=fopen(ccf_location,"r");
		if (fp==NULL) { 
			error (DEVICE_OPEN, CAN_FILE_OPEN_ERROR);
			free (this);
			return(NULL);
		}
		/* build the ccf array */
		while (fgets(buf,sizeof(buf),fp) != NULL) {
			delim_pos = (char *) strchr(buf,':');
			*delim_pos = NULL;
	    		strcpy(ccf[ccf_entry_num].key_value,buf);
			j = (int) ((delim_pos - buf) + 1);
			delim_pos = (char *) strchr(&buf[j],':');
			*delim_pos = NULL;
	    		strcpy(ccf[ccf_entry_num].description,&buf[j]);
			j = (int) ((delim_pos - buf) + 1);
			/* don't include the newline character */
			delim_pos = (char *) strchr(&buf[j],'\x0a');
			*delim_pos = NULL;
	    		strcpy(ccf[ccf_entry_num].shell_script_name,&buf[j]);

			debug("ccf[%d].key_value is %s \n",
			   ccf_entry_num,ccf[ccf_entry_num].key_value);
			debug("ccf[%d].description is %s \n",
			   ccf_entry_num,ccf[ccf_entry_num].description);
			debug("ccf[%d].shell_script_name is %s \n",
			   ccf_entry_num,ccf[ccf_entry_num].shell_script_name);
			debug("ccf_entry_num is %ld\n\n\n",ccf_entry_num);

			ccf_entry_num++;
		}
	}
	if (i >= ccf_entry_num) {
		free (this);
		return (NULL);
	}

	strcpy(this->keyval,&ccf[i].key_value);
 	this->description = (char *) &ccf[i].description;
	strcpy(this->ccf_shell_script_name, ccf[i].shell_script_name);

	debug( "leaving new_CAN_ACTION\n" );
	return( this );
}
