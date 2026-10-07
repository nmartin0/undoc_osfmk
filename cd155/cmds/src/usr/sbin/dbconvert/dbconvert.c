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
* Copyright (c) 1990 San Diego Supercomputer Center.
* All rights reserved.  The SDSC software License Agreement
* specifies the terms and conditions for redistribution.
*
* File:	dbconvert.c
*
* Abstract:	This file contains conversion utility for MACD database
*
******************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/mode.h>
#include "db.h"
#include "macd.h"
#include "filename.h"

/*****************************************************************************
*
* main()
*
*****************************************************************************/

void main(argc, argv)
int argc;
char **argv;

{
   FILE *fd_in, *fd_out;
   struct stat sbuf;
   char errmsg[BUFSIZ];
   int i, b;

   if (argc != 3) {
	(void) fprintf(stderr, "Usage: dbconvert <oldfile> <newfile>\n");
	(void) fprintf(stderr, "Converts a pre-R%d.%d MACS database to R%d.%d format.\n",
			VERSION, RELEASE, VERSION, RELEASE);
	(void) fprintf(stderr, "<oldfile> is typically %s.\n", 
			MACD_DATABASE_FILE);
	(void) fprintf(stderr, "<newfile> must be moved to %s after conversion.\n", MACD_DATABASE_FILE);
	exit(2);
   }

   if (stat(argv[2], &sbuf) == 0) {
	(void) printf ("\"%s\" exists; overwrite? ", argv[2]);
	fflush(stdout);
        i = b = getchar();
        while(b != '\n' && b > 0) {
                b = getchar();
	}
	if(i != 'y' && i != 'Y') {
		(void) printf("Aborted.\n");
		exit(1);
	}
   }

   if ((fd_in = fopen(argv[1], "r")) == (FILE *) NULL) {
	(void) sprintf (errmsg, "Cannot open \"%s\" for reading", argv[1]);
	perror(errmsg);
	exit(1);
   }

   if ((olddb_init(fd_in, DB_OVERFLOW)) < 0) {
        (void) fprintf (stderr, 
              "Could not initialize MACD database from %s.  Exit\n", argv[1]);
        exit(1);
   }

   if ((fd_out = fopen(argv[2], "w")) == (FILE *) NULL) {
	(void) sprintf (errmsg, "Cannot open \"%s\" for writing", argv[2]);
	perror(errmsg);
	exit(1);
   }

   if ((cvt_writeall(fd_out)) < 0) {
	(void) fprintf (stderr, "Could not create new database.  Exit\n");
   }

   (void) printf ("Database conversion completed successfully\n");
   exit(0);
}
