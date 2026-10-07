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
 
/******************************************************************************
 *              Copyright (c) 1990 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:        log.c
 *
 * Abstract:    This file contains routines manipulate log files
 *****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <utmp.h>
#include <time.h>
#include "macpd.h"
#include "filename.h"

/*****************************************************************************
*
* ttywrite()
*
* Abstract:	This routine writes message to terminals of persons
*		specified and returns the number of terminals it has 
*		written to.
*
* Arguments:	who -	login name of the person to whose terminals 
*			it writes a message
*		message -	the message to be sent
*
* Return value: >=0	number of terminals written
*		<0	unknown error
*
*****************************************************************************/

#define ATATIME (128)
#define MAXPTTYLEN (32)

int ttywrite(who, message)
char *who;
char *message;

{
    int i, fd, ulen, nterms, wholen, ttyfd;
    struct utmp utmps[ATATIME];
    char ptty[MAXPTTYLEN];

    /*
      Open up the utmp file to get the tty associated with who
    */

    nterms = 0;
    wholen = strlen(who);

    if ((fd = open(UTMP_FILE, O_RDONLY)) < 0) return(nterms);

    do {
	if ((ulen = read(fd, utmps, sizeof(utmps))) < 0) {
	   (void) close(fd);
	   return(nterms);
	}

	/*
	  If we match a tty, then write to it - do it to all of them
	*/

	for (i = 0; i < ulen/sizeof(struct utmp); i++)
	   if (!strncmp(utmps[i].ut_name, who, wholen)) {
	      (void) sprintf(ptty, "/dev/%s", utmps[i].ut_line);
	      if ((ttyfd = open(ptty, O_WRONLY || O_NDELAY)) >= 0) {
	         if (fcntl(ttyfd, F_SETFL, FNDELAY) >= 0) {
	            (void) write(ttyfd, message, strlen(message));
	            nterms++;
	         }
	         (void) close(ttyfd);
	      }
	   }
    } while (ulen == sizeof(utmps));

    (void) close(fd);
    return(nterms);
}

/*****************************************************************************
*
* mailroot (reason)
*
* Function mailroot() sends mail to the root with info correspond to reason.
*
*****************************************************************************/
 
void mailroot(reason)
int reason;
 
{
    FILE *mail;
    char buf[BUFSIZ], cmd[256];
 
    /*
      Open up a pipe to the mail command and set up the buffering
    */
 
    sprintf(cmd, "/bin/mail root ");
    if ((mail = popen(cmd, "w")) == (FILE *) NULL) return;
    setbuf(mail, buf);
 
    if (reason == NXACCT_0) {
        (void) fprintf (mail,
        "%s file has 0 length, MACS stopped\n", NXACCOUNT_FNAME);
    }
    else if (reason == NXACCT_STAT) {
        (void) fprintf (mail,
        "Failed in getting %s file's status, MACS stopped\n", NXACCOUNT_FNAME);
    }
    else if (reason == NXACCT_NONE) {
        (void) fprintf (mail,
        "%s file does not exist, MACS stopped\n", NXACCOUNT_FNAME);
    }
    else if (reason == MACPDLOG_ERR) {
        (void) fprintf (mail,
	"Failed in opening %s/macpd.log, MACS stopped\n", MACS_LOG_PATH);
    }
    else if (reason == GETHOST_ERR) {
        (void) fprintf (mail,
	"Error in getting local host name, please check /etc/hosts.  MACS stopped");
    }
    else if (reason == MACDDB_0) {
        (void) fprintf (mail,
        "%s file has 0 length, MACS stopped\n", MACD_DATABASE_FILE);
    }
    else if (reason == MACDDB_STAT) {
        (void) fprintf (mail,
        "Failed in getting %s file's status, MACS stopped\n", MACD_DATABASE_FILE);
    }
    else if (reason == MACDDB_NONE) {
        (void) fprintf (mail,
        "No valid %s file found!  MACS stopped.\nSee %s for more information.", 
	MACD_DATABASE_FILE, "/var/adm/syslog/daemon.log");
    }
    else if (reason == MACDDB_OPEN) {
        (void) fprintf (mail,
        "Failed open %s for reading, MACS stopped\n", MACD_DATABASE_FILE);
    }
    else if (reason == MACDDB_FMT) {
        (void) fprintf (mail,
        "%s file format is unrecognizable, MACS stopped\n", MACD_DATABASE_FILE);
    }
    else if (reason == MACDDB_SIZE) {
        (void) fprintf (mail,
        "%s file size error, MACS stopped\n", MACD_DATABASE_FILE);
    }
    else if (reason == MACDDB_RECOVERED) {
        (void) fprintf (mail,
        "%s file was corrupt or unreadable, but has been\nautomatically recovered from backup.  See %s\nfor more information.", 
	MACD_DATABASE_FILE, "/var/adm/syslog/daemon.log");
    }
    if (reason == MACSCONF_0) {
        (void) fprintf (mail,
        "%s file has 0 length, MACS stopped\n", MACS_CONF_FILE);
    }
    else if (reason == MACSCONF_STAT) {
        (void) fprintf (mail,
        "Failed in getting %s file's status, MACS stopped\n", MACS_CONF_FILE);
    }
    else if (reason == MACSCONF_NONE) {
        (void) fprintf (mail,
        "%s file does not exist, MACS stopped\n", MACS_CONF_FILE);
    }
    else if (reason == MACSCONF_ERR) {
        (void) fprintf (mail,
        "Error in %s file, MACS stopped\n", MACS_CONF_FILE);
    }
    else if (reason == NRD_NQSTABLE) {
        (void) fprintf (mail,
	"%s is unreadable, MACS stopped\n", MACS_NQSTABLE_FILE);
    }
    else if (reason == NUL_NQSTABLE) {
        (void) fprintf (mail,
	"%s is empty, MACS stopped\n", MACS_NQSTABLE_FILE);
    }
    else if (reason == CNT_NQSTABLE) {
        (void) fprintf (mail,
	"Format error in %s, first line should be an integer, MACS stopped\n", MACS_NQSTABLE_FILE);
    }
    else if (reason == FMT_NQSTABLE) {
        (void) fprintf (mail,
	"Format error in %s, MACS stopped\n", MACS_NQSTABLE_FILE);
    }
    else if (reason == DIF_NQSTABLE) {
        (void) fprintf (mail,
	"Queue count stated differs from queues defined in %s, MACS stopped\n", MACS_NQSTABLE_FILE);
    }
    fprintf(mail, "\nSincerely,\nMACPD process\n");
    (void) pclose(mail);
}

#ifdef DEBUG
/*****************************************************************************
*
* main(argc, argv)
*
* Function main() writes message argv[2] to user argv[1].
*
*****************************************************************************/

void main(argc, argv)
int argc;
char **argv;

{
    int nterms;

    nterms = ttywrite(argv[1], argv[2]);
    (void) printf("Wrote to %d terminals\n", nterms);
}
#endif
