/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/*
	Audit Reduction/Selection Interface Routines
	Copyright (c) 1988, SecureWare, Inc. All Rights Reserved.

	This module contains a collection of routines that provide
	an interface for third-party products to read audit trails
	and apply audit selection criteria against those records.

	audit_open(session)-open an audit session
	audit_close()-close an audit session
	audit_read_next()-read the next sequential record
	audit_set_select(select)-set audit selection criteria
	audit_select(record)-determine if record selected
*/

#ident "@(#)auditlib.c	2.1 16:15:58 4/20/90 SecureWare"
#ident "@(#)auditlib.c	2.2.3.1 17:21:35 1/4/90 SecureWare"

#include <sys/types.h>

#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

#include <sys/security.h>
#include <sys/audit.h>
#include "libsecurity.h"

static int valid_pathnames = 0;
static int log_fd = -1;
static int compact_fd = -1;
static int count = 0;
static struct audit_header *audit_hdr;
static char *buffer = (char *) 0;
static char *compact_buffer = (char *) 0;
static unsigned long buff_ptr;
static struct log_header log_hdr;
static char pathname[AUDIT_PATHSIZE];
static char filename[] = AUDIT_LOG_FILENAME;

static audit_getfile();
static audit_readnext();
static audit_cleanup();
static audit_itoa();

audit_open(session)
int session;
{
	if(log_fd >= 0)
		return(-1);

	strcpy(pathname,AUDIT_LOGDIR);
	audit_itoa(session,&filename[11],5);
	strcat(pathname,filename);

	/* Open the audit log file for session to get basic parameters. */

	log_fd = open(pathname,O_RDONLY);
	if(log_fd == -1) {
		printf(MSGSTR(AUDITLIB_1, "Reduce: error on session log file open.\n"));
		printf(MSGSTR(AUDITLIB_2, "Session log name: '%s' error-%d\n"),pathname,errno);
		printf(MSGSTR(AUDITLIB_3, "Audit session may not exist, check session number.\n"));
		return(-1);
	}

	/* Read the audit session log file header */

	if((count = read(log_fd,&log_hdr,sizeof(struct log_header))) == -1) {
	   perror(MSGSTR(AUDITLIB_4, "Error on read of logfile header"));
	   return(-1);
	}

	/* Get the space for compaction and uncompaction buffers */

	buffer = (char *) malloc(log_hdr.read_size * 2);
	compact_buffer = (char *) malloc(log_hdr.read_size * 2);
	if((buffer == (char *)0) || (compact_buffer == (char *)0)) {
		printf(MSGSTR(AUDITLIB_5, "Error on malloc of buffer space"));
		exit(1);
	}
	
	/* Get the first pathname and data block for audit data file */

	if(audit_getfile() == 1) {
		valid_pathnames = 1;
		return(0);
	}
	else {
		valid_pathnames = 0;
		count = 0;
		return(-1);
	}

}

/*
	audit_close()-close the current audit session, releasing the
	malloc buffers and resetting the file descriptors such that
	another session could be opened.
*/

audit_close()
{
	if(log_fd == -1)
		return(0);

	audit_cleanup();
	return(0);
}

/*
	audit_read()-read the next sequential record from the audit
	trail.
*/

struct audit_header *
audit_read()
{
	int ret;
	struct audit_header	*audit_header_ptr ;

	/* If no pathnames, error or EOF occurred */

	if(valid_pathnames == 0)
		return((struct audit_header *) 0);

	if(count > 0) {
#ifdef DEBUG
		printf(MSGSTR(AUDITLIB_6, "Record size-%d, pid-%d, event-%d, record-%d, id-%d\n"),
		   audit_hdr->rec_length,audit_hdr->pid,
		   audit_hdr->event_type,audit_hdr->record_type,
		   audit_hdr->event_id);
#endif
		audit_header_ptr = audit_hdr ;
		count -= audit_hdr->rec_length;
		buff_ptr += audit_hdr->rec_length;
		audit_hdr = (struct audit_header *)buff_ptr;
		return(audit_header_ptr) ;
	}

	/* Current buffer is exhausted, read next block */

	if((ret = audit_readnext()) == -1) {
		printf(MSGSTR(AUDITLIB_7, "Error on reading next data block-%d\n"),errno);
		return((struct audit_header *) 0);
	}
	else if(ret == 0) {
		if(audit_getfile() != 1) {
			valid_pathnames = 0;
			count = 0;
			audit_cleanup();
			return((struct audit_header *) 0);
		}
	}

	return(audit_hdr);
}

/*
	audit_getfile()-read the next pathname from the log file and
	open the audit data file for reading.
*/

static
audit_getfile()
{

	if((count = read(log_fd,pathname,AUDIT_PATHSIZE)) == -1) {
		printf(MSGSTR(AUDITLIB_8, "Error on read of log file for pathname (%d)\n"),errno);
		return(-1);
	}
	else if(count == 0) {
#ifdef DEBUG
		printf(MSGSTR(AUDITLIB_9, "End of pathnames in log file\n"));
#endif
		return(-1);
	     }
	/* Close previous compacted output file, if any */
	if (compact_fd > 0) {
		close(compact_fd);
	}

	/* Open the actual audit data file for next path name */

	compact_fd = open(pathname,O_RDONLY);
	if(compact_fd == -1) {
		printf(MSGSTR(AUDITLIB_10, "Error on compaction file open-%s (%d)\n"),
		   pathname,errno);
		return(-1);
	}

	return(audit_readnext());
}

/*
	audit_readnext()-read the next block of audit data from the
	current audit data file.
*/

static
audit_readnext()
{
	int size;

	count = read(compact_fd,&size,sizeof(int));

	if(count == 0)
		return(0);
	else if(count == -1) {
		printf(MSGSTR(AUDITLIB_11, "audit_read: error on read of buffer count (%d)\n"),
		   errno);
		return(-1);
	}

	if(log_hdr.flag == COMPACTED) {
		count = read(compact_fd,compact_buffer,size);
		count = uncompact(compact_buffer,buffer,count);
	}
	else count = read(compact_fd,buffer,size);

	if(count == -1) {
		printf(MSGSTR(AUDITLIB_12, "audit_read: error on read of data file (%d)\n"),errno);
		return(-1);
	}

#ifdef DEBUG
	printf(MSGSTR(AUDITLIB_13, "Buffer size-%d\n"),count);
#endif
	buff_ptr = (unsigned long)buffer;
	audit_hdr = (struct audit_header *)buff_ptr;
	return(1);
}

/*
	audit_cleanup()-cleanup in case of error or EOF.
*/

static
audit_cleanup()
{
	close(log_fd);
	close(compact_fd);

	if(compact_buffer != (char *) 0)
		free(compact_buffer);

	if(buffer != (char *) 0)
		free(buffer);

	log_fd = compact_fd = -1;
	return(0);
}

/*
	audit_itoa()-convert integer to ascii
*/

static
audit_itoa(num,s,digits)
unsigned int num;
char *s;
int digits;
{
	register int i;

	for(i=0; i < digits; i++, s--) {
		*s = (num % 10) + '0';
		num = num /10;
	}
	return;
}
