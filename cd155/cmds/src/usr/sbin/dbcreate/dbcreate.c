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
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * File:	dbcreate.c
 *
 * Abstract:	This utility creates an empty database for the MACS
 *
 * Arguments:	argc -	argument count
 *		argv -	argument list, include database size and overflow size
 *
 * Return value: None
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include "db.h"

struct cpulim_info *db;
static u_long magicno;
static short version[2]={VERSION,RELEASE};
static int toffset;

main(argc,argv)
int argc;
char *argv[];
{
	FILE *fp;
	long hashlength, overlength;
	long totallength,length;
	long i;
	extern char *calloc();

	toffset = sizeof(hashlength)+sizeof(magicno);
	if(argc!=4) {
		(void) fprintf(stderr,
		  "Usage: dbcreate <hash size> <overflow size> <fname>\n");
		exit(1);
	}
	hashlength = atol(argv[1]);
	overlength = atol(argv[2]);
	totallength = hashlength+overlength;
	(void) printf("hash = %d, over = %d\n", hashlength, overlength);
	length = totallength*sizeof(struct cpulim_info);
	db = (struct cpulim_info *) calloc(1,length);
	if(db==NULL) {
		(void) fprintf(stderr, "Error: Unable to obtain memory\n");
		exit(1);
	}
	for(i=0;i<totallength;i++) {
		db[i].empty = 1;
		db[i].next = (-1);
	}
	if((fp = fopen(argv[3], "w"))==NULL) {
		(void) fprintf(stderr, "Can't open file\n");
		exit(1);		
	}
	magicno = calcSum ((char *)&version, (char *)&hashlength, (char *)db, 
			   (long) (sizeof(struct cpulim_info)*totallength));
	(void) fwrite(&magicno, sizeof(magicno), 1, fp);
        (void) fwrite(&version, sizeof(short), 2, fp);
	(void) fwrite(&hashlength, sizeof(hashlength), 1, fp);
	(void) fwrite(db, sizeof(struct cpulim_info), totallength, fp);
	(void) fclose(fp);
}
