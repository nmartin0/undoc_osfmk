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
 Name:		amd.c
 Title:		Array Monitor Daemon
 Version:	
 Revision:	$Revision: 1.6.4.1 $
 Update Date:	$Date: 1995/06/11 23:27:14 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C", C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This program implements a UNIX daemon process that runs
		continuously, periodically monitoring disk array controller
		events. 
*/

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/
#include "stddefs.h"
#include "console.h"
#include "dac_time.h"
#include "dac_event.h"
#include "zip_dialog.h"
#include "dau_err.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef PARAGON_860
/******************************************************************************
 ***                          GLOBAL VARIABLES                              ***
 *****************************************************************************/
char		Devname[100];
#endif
/******************************************************************************
 ***		             VARIABLE DEFINITIONS			    ***
 *****************************************************************************/
int Argc;
char **Argv;
int Interactive = FALSE;
static char ident[] = PROJREL "/$Revision: 1.6.4.1 $ - $RCSfile: amd.c,v $, Array Monitor Daemon";
int getopt (int argc, char * const *argv, const char *optstring);
extern *optarg;
extern int optind, opterr, optopt;

/******************************************************************************
 ***				  PROCEDURES				    ***
 *****************************************************************************/
main( argc, argv )
int argc;
char **argv;
{
	extern DAC_EVENT *make_dacevent();
	hw_zipcode hw_addr;
	hw_zipcode_t *hw_addr_ptr;
	char *bracket_pos=NULL;
	char *logfile_name, *str_ptr, log_location[256], str_hw_addr[256];
	char msg_num_str[256], msg_txt[256], buf[256], tempbuf[256];
	u_long date_time, i=0, j=0, k=0, msg_num=0;
	int c, rf_window=0, poll_interval=0, refresh_cycle=0, sync=FALSE;
        int goodzip=FALSE, delete_msgs=FALSE, display_msgs=FALSE, full=TRUE;
	int this_msg_deleted=FALSE, msg_not_deleted=FALSE, copy_flag=TRUE;
	int s=COMMAND_LINE_SYNTAX, msg_num_str_len=0, msg_to_delete=0;
	int comma_pos=0, rewind_amount=0, errflg=0, status=0;
	FILE *log_fp, *templog_fp;

	DAC_TIME *dt;
	DAC_EVENT *e;
	CONSOLE *console;
	ZIP_DIALOG *zip_dialog;

	int fd;		/* file descriptor used in making this a daemon */
#ifdef PARAGON860
	int luncntr;
#endif

	Argc = argc;
	Argv = argv;

        logfile_name=(char *) NULL;
        opterr=0;
#ifdef PARAGON_860
        while ((c=getopt(argc, argv, "f:l:n:i:r:k:ds"))!=EOF)
          	switch (c) {
         		case 'f': 
			strcpy(Devname,optarg);
			hw_addr = zip_edit (convert_array_name(optarg), &ok);
/*        			zip_dialog=new_ZIP_DIALOG();
				hw_addr = (hw_zipcode) zip_dialog->prompt
                                	(( ZIP_DIALOG *) zip_dialog );
					*/
				goodzip=TRUE;
				break;
         		case 'l': 
         			logfile_name=(char *) optarg;
				if (logfile_name == NULL) {
					errflg++;
             				s=ARGUMENT_RANGE;
				}
				break;
#else
        while ((c=getopt(argc, argv, "z:f:n:i:r:k:ds"))!=EOF)
          	switch (c) {
         		case 'z': 
        			zip_dialog=new_ZIP_DIALOG();
				hw_addr = (hw_zipcode) zip_dialog->prompt
                                	(( ZIP_DIALOG *) zip_dialog );
				goodzip=TRUE;
				hw_addr_ptr = (hw_zipcode_t *) & hw_addr;
				break;
         		case 'f': 
         			logfile_name=(char *) optarg;
				if (logfile_name == NULL) {
					errflg++;
             				s=ARGUMENT_RANGE;
				}
#ifdef PARAGON860
				errflg++;
#endif
				break;
#endif
         		case 'n': 
         			rf_window=atoi(optarg);
				if (rf_window <= 0) {
					errflg++;
             				s=ARGUMENT_RANGE;
				}
				break;
         		case 'i': 
    				poll_interval=atoi(optarg);
				if (poll_interval <= 0) {
					errflg++;
             				s=ARGUMENT_RANGE;
				}
				break;
         		case 'r': 
                        	refresh_cycle=atoi(optarg);
				if (refresh_cycle <= 0) {
					errflg++;
             				s=ARGUMENT_RANGE;
				}
				break;
         		case 'k': 
    				delete_msgs=TRUE;
				strcpy(msg_num_str,optarg);
				break;
         		case 'd': 
    				display_msgs=TRUE;
				break;
         		case 's': 
    				sync=TRUE;
				break;
         		case '?': 
        	        	errflg++;
        	}


	if (((delete_msgs) || (display_msgs)) && (goodzip)) {
	/****** open the proper message log for deleting or displaying ******/
		strcpy (log_location,LOGDIR);
		convert_zip( hw_addr, str_hw_addr );
		for (k=0; k<=(sizeof(str_hw_addr)); k++)
			if (str_hw_addr[k] == LUN_DELIM)
				str_hw_addr[k] = NULL;
		strcat (log_location, "/");
		strcat (log_location, str_hw_addr);
		log_fp=fopen(log_location,"a+");
		if (log_fp == NULL)
			error (DEVICE_OPEN, FILE_OPEN_ERROR);
		status=lock_logfile_amd(log_fp); 
	}
	/****** ok - the proper message log is now ready ******/

	/****** see if they wish to display the message log ******/
	if ((display_msgs) && (goodzip)) {
		/* read then print the message log */
		while (fgets(buf,sizeof(buf),log_fp) != NULL) {
			fputs(buf,stdout);
		}
#ifdef PARAGON860
		status=unlock_logfile_amd(log_fp);
#endif
		fclose(log_fp);
		exit(0);
	}    

	/****** see if they are trying to clear the log ******/
	if ((delete_msgs) && (goodzip) && (strcmp(msg_num_str,"all")==0)) {
		/* if so, close present file and re-create an empty one */
#ifdef PARAGON860
		status=unlock_logfile_amd(log_fp);
#endif
		fclose(log_fp);
		log_fp=fopen(log_location,"w");
		if (log_fp == NULL)
			error (DEVICE_OPEN, FILE_OPEN_ERROR);
#ifdef PARAGON860
		status=unlock_logfile_amd(log_fp);
#endif
		fclose(log_fp);
		exit(0);
	}		

	if ((delete_msgs) && (goodzip)) {
	/****** parse the message to delete and continue to delete it******/
#ifdef PARAGON860
		status=unlock_logfile_amd(log_fp);
#endif
		fclose(log_fp);
		msg_num_str_len=strlen(msg_num_str);
		str_ptr=msg_num_str;
		while (i < msg_num_str_len) {
			msg_to_delete=strtol(&msg_num_str[i],&str_ptr,10);
			i = (int) ((str_ptr - msg_num_str) + 1);
			this_msg_deleted=FALSE;

		 	debug("message to delete is %ld\n",msg_to_delete);
		/****** the message to delete is now in msg_to_delete ******/

		/****** now ck each line for the message to delete ******/
			templog_fp=tmpfile();
			log_fp=fopen(log_location,"a+");
			if (log_fp == NULL)
				error (DEVICE_OPEN, FILE_OPEN_ERROR);
			status=lock_logfile_amd(log_fp);
			while (fgets(buf,sizeof(buf),log_fp) != NULL) {
			    strcpy(msg_txt,buf);
			    msg_txt[8]=NULL;
			    /* ck for message # stamp */		
			    if (strcmp(msg_txt,"[Message")==0) { 
                                /* found a msg #, so extract its # */
			        strcpy(msg_txt,buf);
				bracket_pos = (char *) strchr(msg_txt,']');
				*bracket_pos=NULL;
				msg_num=atoi(&msg_txt[9]);
				/* msg_num is the message number we are now
				   located at */

				debug("msg_num is %d\n",msg_num);
				debug("str is %s\n",&msg_num_str[i]);
				debug("msg_to_delete is %d\n",msg_to_delete);

				/* ck if this msg is the one to delete */
				if (msg_to_delete==msg_num) { 
					this_msg_deleted=TRUE;
					copy_flag=FALSE;
				}
				else {	
					copy_flag=TRUE;
				}
			    }
			    /* copy to temp file if not deleted message */
			    if (copy_flag) fputs(buf,templog_fp);
			}
		/****** rewrite the message file to the original log ******/
			fflush(templog_fp);  /* set the file to the beginning */
			rewind(templog_fp);
#ifdef PARAGON860
		status=unlock_logfile_amd(log_fp);
#endif
			fclose(log_fp);
			log_fp=fopen(log_location,"w");
			if (log_fp == NULL)
				error (DEVICE_OPEN, FILE_OPEN_ERROR);
			status=lock_logfile_amd(log_fp);
			while (fgets(buf,sizeof(buf),templog_fp) != NULL) {
				fputs(buf,log_fp);
			}
			if (!this_msg_deleted)
				msg_not_deleted=TRUE;
#ifdef PARAGON860
		status=unlock_logfile_amd(log_fp);
#endif
			fclose(log_fp);
			fclose(templog_fp);
		}
		/****** the non-deleted messages should now be back in the 
			original message file ******/

	if (msg_not_deleted)
		error (NON_EXISTENT_MESSAGE, NO_ERROR_MESSAGE);
	exit(0);
	}

       	if ((goodzip==FALSE) || (rf_window==0) || (poll_interval==0) ||
          	(refresh_cycle==0)) errflg++;
        if (errflg!=0) {
        	usage();
		debug("\ns is %d\n",s);
        	exit (s);
       	}
		debug("\nzipcode is %f\n",hw_addr);
		debug("logfile_name is %s\n",logfile_name);
		debug("rf_window is %d\n",rf_window);
		debug("poll_interval is %d\n",poll_interval);
		debug("refresh_cycle is %d\n",refresh_cycle);
		debug("sync is %d\n",sync);
		debug("delete_msgs is %d\n",delete_msgs);
		debug("display_msgs is %d\n",display_msgs);

	if ( sync ) {
		Interactive = TRUE; /*  fake out dac_time so it does not
					terminate upon error */	
		dt = new_DAC_TIME( NULL, hw_addr );
		date_time = time( NULL );
		dt->set( dt, date_time );
		dt->destroy( dt );
		Interactive = FALSE; /* reset */
	}

#ifdef PARAGON860  /* if we can't open the device don't fork */
	{
	SCSI_DEV *sd;
	SCSI_Inquiry_Data_t inqdata;
	int exclusive;

	sd=new_SCSI_DEV(hw_addr,exclusive=FALSE);
	if ( sd->inquiry( sd, (u_char * ) &inqdata, sizeof (inqdata)) != 0)
		exit(2);
	if (((inqdata.Periph_Qualifier & LUN_NOT_PRESENT) != LUN_NOT_PRESENT) 
		&& (strncmp(inqdata.Product_ID,"ADP-9",5)==0))
		destroy_SCSI_DEV (sd);
	else
		exit(2);
	}
#endif

	/*
         * The following code is to make this a true daemon.  This code is 
         * input as outlined in the article "How to write a UNIX Daemon"
         * by Dave Lennert.
	 */
	/* If launched by init (process 1), there's no need to detach.
 	 * 
	 * Note:  this test is unreliable due to an unavoidable race
 	 * condition if the process is orphaned.  
	*/	
	if (getppid() == 1)
		goto out;
	/* Ignore terminal stop signals */
#ifdef SIGTTOU
        signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
        signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
        signal(SIGTSTP, SIG_IGN);
#endif
	/* Allow parent shell to continue.
	 * Ensure the process is not a process group leader.
	*/
#ifdef PARAGON860
	if (join_root_part() < 0 )
		perror("join_root_part()");
	hw_addr_ptr = (hw_zipcode_t *) &hw_addr;
	debug("node number %d\n",hw_addr_ptr->IO_Bus);
	if(rfork(hw_addr_ptr->IO_Bus)!=0)
#else
	if (fork() !=0)
#endif
		exit (0); /* parent */  
	/* child */
	/* Disassociate from controlling terminal and process group.
	 * 
	 * Ensure the process can't reaquire a new controlling terminal.
	 * 
	 * AT&T won't assign a new controlling terminal
	 * because process is not a process group leader.
	*/

	setpgrp(); /* lose controlling terminal & change process group */    
	signal(SIGHUP, SIG_IGN); /* immune from pgrp leader death */
#ifdef PARAGON860
	if(rfork(hw_addr_ptr->IO_Bus)!=0)
#else
	if (fork() !=0) /*  become non-pgrp-leader */
#endif
		exit (0); /* first child */
	/* second child */
out:
	for (fd=0;fd < _NFILE; fd++)
		close(fd); /* close all file descriptors */
	chdir("/"); /* move current directory off mounted filesystem */
#ifndef PARAGON860
	umask(0); /* clear any inherited file mode creation mask */
#endif

	/* end of code making this a daemon */

	console = new_CONSOLE( rf_window );
	for ( i = 1; TRUE; i++ ) {
		hw_addr_ptr = (hw_zipcode_t *) &hw_addr;
		hw_addr_ptr->LUN = 0;
		if ( ( i % refresh_cycle ) == 0 ) 
			console->refresh( console, hw_addr, rf_window );
#ifdef PARAGON860
		luncntr = 0;
#endif
		for ( j = 1; j <= MAX_LUNS; j++ ) {
			int exclusive;
			SCSI_Inquiry_Data_t inqdata;
			SCSI_DEV *sd;

			sd=new_SCSI_DEV(hw_addr,exclusive=FALSE);
			if ( sd->inquiry( sd, (u_char * ) &inqdata,
				sizeof (inqdata)) == 0)
	   		   if (((inqdata.Periph_Qualifier & LUN_NOT_PRESENT)
				!= LUN_NOT_PRESENT) &&
#ifdef PARAGON860
			  	(strncmp(inqdata.Product_ID,"ADP-9",5)==0)){
				luncntr++;
#else
			  	(strncmp(inqdata.Product_ID,"ADP-9",5)==0))
#endif
				while (e=make_dacevent(hw_addr,NULL,full=TRUE))
					console->put( console, (u_long) e );
#ifdef PARAGON860
				}
#endif
			hw_addr_ptr->LUN++;
			destroy_SCSI_DEV (sd);
			debug("\nhw_addr->LUN is %ld",hw_addr_ptr->LUN);
			debug("\nhw_addr is %ld",hw_addr);
		}
#ifdef PARAGON860
		if(!luncntr)
			exit(6);
#endif
		sleep( poll_interval );
	}
}


int lock_logfile_amd (FILE *log_fp)
{
#ifdef PARAGON860 /* typedef to satisfy the compiler */
typedef struct flock flock_t;
#endif
	flock_t lock_struct;
	int status=0;

	lock_struct.l_type=F_WRLCK; 
	lock_struct.l_whence=0;
	lock_struct.l_start=0;
	lock_struct.l_len=0;
	status=fcntl(fileno(log_fp),F_SETLKW,&lock_struct); 
	if (status !=0)
		error (DEVICE_OPEN, FILE_LOCK_ERROR);
	return(status);
}

#ifdef PARAGON860
int unlock_logfile_amd (FILE *log_fp)
{
#ifdef PARAGON860 /* typedef to satisfy the compiler */
typedef struct flock flock_t;
#endif
	flock_t lock_struct;
	int status=0;

	lock_struct.l_type=F_UNLCK; 
	lock_struct.l_whence=0;
	lock_struct.l_start=0;
	lock_struct.l_len=0;
	status=fcntl(fileno(log_fp),F_SETLKW,&lock_struct); 
	if (status !=0)
		error (DEVICE_OPEN, FILE_LOCK_ERROR);
	return(status);
}
#endif
