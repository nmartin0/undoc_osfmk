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
 Name:		console.c
 Title:		CONSOLE Class Implementation
 Version:	
 Revision:	$Revision: 1.3.4.1 $
 Update Date:	$Date: 1995/06/11 23:27:21 $ 
 Programmer:	ach
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains the methods for the CONSOLE class.
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include <stdio.h>
#include "console.h"
#include "dac_event.h"
#include "object_set.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
static int
put( this, dac_event )
void *this;
OBJECT *dac_event;
{
	static int status;
	hw_zipcode hw_addr;
	char log_location[256], temp[256];
	char buf[256], msg_txt [256], str_hw_addr[256];
	char *bracket_pos;
	int i, msg_num=0;
	FILE *fp;
	hw_addr = dac_event->hw_address((OBJECT *) dac_event );

fprintf(stderr,"put:hw_addr = %f\n",hw_addr);
/****** open and lock the message log file ******/
	strcpy (log_location,LOGDIR);
	convert_zip( hw_addr, str_hw_addr );
fprintf(stderr,"put:str_hw_addr = %s\n",str_hw_addr);
	for (i=0; i<=(sizeof(str_hw_addr)); i++)
		if (str_hw_addr[i] == LUN_DELIM) str_hw_addr[i] = NULL;
	strcat (log_location, "/");
	strcat (log_location, str_hw_addr);
	((CONSOLE *)this)->console_fp=fopen(log_location,"a+");
	if ((((CONSOLE*)this)->console_fp) == NULL) {
		fp=fopen("/dev/console","a");
		if (fp !=NULL) {
		 fprintf(fp,"\nError - Could not open the amd message file\n");
		}
		fclose(fp);
	}
	status=lock_logfile_con(((CONSOLE *)this)->console_fp);

	/****** only perform the following operations if we have a good ******/
        /****** amd msg ptr which is successfully locked ******/
	if (((((CONSOLE*)this)->console_fp) != NULL) && (status==0)) {
		/****** find out what the last message entry # is ******/
		fflush(((CONSOLE *)this)->console_fp); /* start at the beg. */
		rewind(((CONSOLE *)this)->console_fp); /* of the file */
		while (fgets(buf,sizeof(buf),((CONSOLE *)this)->console_fp)
			!= NULL) {
			strcpy(msg_txt,buf);
			msg_txt[8]=NULL;
			/* ck for message # stamp */		
			if (strcmp(msg_txt,"[Message")==0) { 
                		/* found a msg #, so extract its # */
				bracket_pos = (char *) strchr(buf,']');
				*bracket_pos=NULL;
				msg_num=atoi(&buf[9]);
	    		}
		}
		/* add 1 to the last message entry # */
		msg_num++;
	/****** msg_num is the next message number to put ******/

	/****** Display message info to the message log file ******/
        	dac_event->redirect(dac_event,((CONSOLE *)this)->console_fp);
		fprintf(((CONSOLE *)this)->console_fp,"[Message %8d]\n",
			msg_num);
        	dac_event->describe((void *) dac_event);
#ifdef PARAGON860
	status=unlock_logfile_con(((CONSOLE *)this)->console_fp);
#endif
        	status=fclose(((CONSOLE *) this)->console_fp); 
	/****** Message info is now in message log file ******/

	/****** Display message info to the system console ******/
		((CONSOLE *)this)->console_fp=fopen("/dev/console","a");
		if (((CONSOLE *)this)->console_fp) {
        	dac_event->redirect(dac_event,((CONSOLE *)this)->console_fp);
		fprintf(((CONSOLE *) this)->console_fp,"[Message %8d]\n", 
			msg_num);
        	dac_event->describe((void *) dac_event);
        	status=fclose(((CONSOLE *) this)->console_fp); 
		}
	/****** Message info is now displayed to the system console ******/

	}
	return(status);
}

static int
refresh( this, hw_addr, rf_window )
void *this;
hw_zipcode hw_addr;
u_long rf_window;
{
	DAC_EVENT *a;
	u_long msg_num_tot=0, msg_num_strt=0, msg_num_pos=0;
	char log_location[256], msg_num_str[256], temp[256];
	char buf[256], msg_txt [256];
	char str_hw_addr[256];
	int status=0, i=0;
	FILE *fp;
	fp=fopen("/dev/console","a");  /* open the sys console for output */

/****** open and lock the message log file ******/
	log_location[0]=NULL;
	strcpy (log_location,LOGDIR);
	convert_zip( hw_addr, str_hw_addr );
	for (i=0; i<=(sizeof(str_hw_addr)); i++)
		if (str_hw_addr[i] == LUN_DELIM) str_hw_addr[i] = NULL;
	strcat (log_location, "/");
	strcat (log_location, str_hw_addr);
	((CONSOLE *)this)->console_fp=fopen(log_location,"a+");
	if ((((CONSOLE*)this)->console_fp) == NULL) {
		if (fp != NULL) {
		  fprintf(fp,"\nError - Could not open the amd message file\n");
		}
	}
	status=lock_logfile_con(((CONSOLE *)this)->console_fp);

/****** only perform the following operations if we have a good ******/
/****** amd msg ptr which is successfully locked ******/
	if (((((CONSOLE*)this)->console_fp) != NULL) && (status==0)) {
	/****** find out how many message entries there are ******/
		while (fgets(buf,sizeof(buf),((CONSOLE *)this)->console_fp)
			!= NULL) {
			msg_txt[0]=NULL;
			strcpy(msg_txt,buf);
			msg_txt[8]=NULL;
			/* ck for message # stamp */		
			if (strcmp(msg_txt,"[Message")==0) { 
                		/* found a msg #, so add to the total */
				msg_num_tot++;
	    		}
		}
	/****** the total message number entries is in msg_num_tot ******/

	/****** subtract the message number total minus the rf_window
		to find the starting point ******/
		msg_num_strt = (msg_num_tot - rf_window) + 1;
		if (msg_num_tot  < rf_window) msg_num_strt = 1;

	/****** print out log entries once at the starting point ******/
		fflush(((CONSOLE *)this)->console_fp); /* start at the beg. */
		rewind(((CONSOLE *)this)->console_fp); /* of the file */
		while (fgets(buf,sizeof(buf),((CONSOLE *)this)->console_fp)
			!= NULL) {
			msg_txt[0]=NULL;
			strcpy(msg_txt,buf);
			msg_txt[8]=NULL;
			/* ck for message # stamp */		
			if (strcmp(msg_txt,"[Message")==0) { 
                		/* found a msg #, so add to the position */
				msg_num_pos++;
	    		}
			/* print out to the console once at starting position */
			if (msg_num_pos >= msg_num_strt) {
				if (fp != NULL) {
				  fprintf(fp, buf);
				}
			}
		}
	/****** the rf_window number of messages should be now displayed ******/

	/****** display warning message if entire file is not printed ******/
		if (msg_num_tot > rf_window)
		if (fp != NULL) {
	 	   fprintf(fp,
		   "\nWARNING - Some amd message log entries not displayed\n");
		}
#ifdef PARAGON860
	status=unlock_logfile_con(((CONSOLE *)this)->console_fp);
#endif
        	status=fclose(((CONSOLE *) this)->console_fp); 
	}
	fclose(fp);
	return(status);
/****** done! ******/
}

void
destroy_CONSOLE( this )
void *this;
#define THIS ((CONSOLE *) this)
{

	THIS->dac_events->destroy( THIS->dac_events );
	destroy_SINK( (SINK *) this );
}
#undef THIS

int lock_logfile_con (log_fp)
FILE *log_fp;
{
#ifdef PARAGON860
typedef struct flock flock_t;
#endif
	flock_t lock_struct;
	int status=0;
	FILE *fp;

	lock_struct.l_type=F_WRLCK;
	lock_struct.l_whence=0;
	lock_struct.l_start=0;
	lock_struct.l_len=0;
	status=fcntl(fileno(log_fp),F_SETLKW,&lock_struct);
	if (status !=0) {
		fp=fopen("/dev/console","a");
		if (fp != NULL) {
		  fprintf(fp,"\nError - Could not lock the amd message file\n");
		}
		fclose(fp);
	}
	return(status);
}

CONSOLE *
new_CONSOLE( nevents )
int nevents;
{
	SINK *s;
	CONSOLE *this;
	debug( "entered new_CONSOLE\n" );
	s = new_SINK();
	this = (CONSOLE *) zalloc( sizeof( CONSOLE ) );
	memcpy( this, s, sizeof( SINK ) ); /* inherit from SINK */
	free( s );
	this->description = "console object";
	this->dac_events = new_OBJECT_SET( nevents );
	this->destroy = destroy_CONSOLE;
	this->put = put;
	this->refresh = refresh;
	debug( "leaving new_CONSOLE\n" );
	return( this );
}
#ifdef PARAGON860
int unlock_logfile_con (log_fp)
FILE *log_fp;
{
#ifdef PARAGON860
typedef struct flock flock_t;
#endif
	flock_t lock_struct;
	int status=0;
	FILE *fp;

	lock_struct.l_type=F_UNLCK;
	lock_struct.l_whence=0;
	lock_struct.l_start=0;
	lock_struct.l_len=0;
	status=fcntl(fileno(log_fp),F_SETLKW,&lock_struct);
	if (status !=0) {
		fp=fopen("/dev/console","a");
		if (fp != NULL) {
		  fprintf(fp,"\nError - Could not lock the amd message file\n");
		}
		fclose(fp);
	}
	return(status);
}
#endif
