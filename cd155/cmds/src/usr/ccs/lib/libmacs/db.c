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
 
/*****************************************************************************
 *              Copyright (c) 1990 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:        db.c
 *
 * Abstract:	Disk and memory based hash table routines for use by
 * 		the MACS.  Entire database is kept in core for speedy 
 *		access/update.
 *
 *****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/mode.h>
#include <sys/stat.h>
#include <sys/limits.h>
#include "db.h"
#include "filename.h"

extern char *malloc();

/* amount to increase overflow by default */
#define OVERDEF	100

void datacopy();
int extendover();

static FILE *dbfp;
struct cpulim_info *db;

/* offset from the top that data exists */
static int toffset;

/* magic number for file id */
static u_long magicno;

/* version release number */
static short version[2]={VERSION,RELEASE};

/* current hash table length */
static long hashlength;

/* length of overflow area */
static long overlength;

/* amount used in overflow area */
static long usedover;

/* set if data base has change structure since last dump to disk */
static int increased;

/* amount to extend overflow each time */
static int overinc;

/* site-dependent run-time variables */
static char *dbfile = NULL;
static char *dbnewfile = NULL;
static char *dboldfile = NULL;
static char *dbbakfile = NULL;

#ifdef DB_BUILD

/*
	Build a simple database, usually is only for testing
*/

#ifdef IPSC
#define bzero(s,n) (void) memset(s, 0, n)
#endif
main(argc,argv)
int argc;
char *argv[];
{
        char info[100];
        struct cpulim_info temp;
        struct cpulim_info *tempptr;
        int ret;
        int     agid, uid;
        int     x;
	char buf[256];
	int match;

        if(db_init(10)<0) printf("Failed to initialize MACD database!\n");
        if (--argc < 1) {
                (void) fprintf(stderr,
			"Usage: %s agid uid1 uid2 uid3 uid4\n", 
			argv[0]);
                exit(2);
        }
        agid = atoi(argv[1]);
        bzero(&temp, sizeof(struct cpulim_info));
        temp.id = agid;
        temp.agid = agid;
        temp.timestamp = time(0);
        temp.weight = 1;
        temp.inhibit = 1;
        if (db_insagid(agid,&temp) <0 )
                fprintf(stderr,"Error inserting agid %d\n");

        for (x=2; x<=argc; x++)
        {
                uid=atoi(argv[x]);
                bzero(&temp, sizeof(struct cpulim_info));
                temp.id = uid;
                temp.agid = agid;
                temp.percent = 10000;   /* 100 % */
                temp.timestamp = time(0);
                if (db_insuid(uid,agid,&temp) < 0 )
                        fprintf(stderr,"Error inserting agid %d uid %d\n",
                                agid,uid);
        }
        if(db_writeall()<0)
                (void) fprintf("Error writing\n");
}
#endif

#ifdef DB_DEBUG

/*
	database debugging interface program
*/

main(argc,argv)
int argc;
char *argv[];
{
	char info[100];
	struct cpulim_info temp;
	struct cpulim_info *tempptr;
	int ret;

	printf("Size of struct = %d bytes\n", sizeof(struct cpulim_info));
	if(db_init(10)<0) printf("Failed to initialize MACD database!\n");
	printf("Dump quit write w(n) i(n) g(n) d(n) r(n)\n");
	while(gets(info)!=NULL) {
		if(!strcmp(info, "dump")) dump();
		else
		if(!strcmp(info, "quit")) exit(0);
		else
		if(!strcmp(info, "write")) {
			if(db_writeall()<0) 
				printf("Error writing\n");
		}
		else
		if(info[0]=='w') {
			if(db_writeagid(atoi(&info[1]))<0)
				printf("Error writing one\n");
			else printf("Write successful\n");
		}
		else
		if(info[0]=='i' && atoi(&info[1])%2) {
			if((ret=db_insuid(atoi(&info[1]),1,&temp))<0)
				printf("Error inserting one\n");
			else printf("Insert successful %d\n",ret);
		}
		else
		if(info[0]=='i') {
			if((ret=db_insagid(atoi(&info[1]),&temp))<0)
				printf("Error inserting one\n");
			else printf("Insert successful %d\n",ret);
		}
		else
		if(info[0]=='g' && atoi(&info[1])%2) {
			if(db_getuid(atoi(&info[1]),1)==NULL)
				printf("Error getting\n");
			else printf("Get successful\n");
		}
		else
		if(info[0]=='g') {
			if(db_getagid(atoi(&info[1]))==NULL)
				printf("Error getting\n");
			else printf("Get successful\n");
		}
		else
		if(info[0]=='d') {
			if(db_delagid(atoi(&info[1]))<0)
				printf("Error deleting\n");
			else printf("Delete successful\n");
		}
		else
		if(info[0]=='r') {
			if(rehash(atoi(&info[1]))<0)
				printf("Error rehashing\n");
			else printf("Rehashing successful\n");
		}
		printf("Dump quit write w(n) i(n) g(n) d(n) r(n)\n");
	}
	printf("Trying account 2\n");
	while((tempptr=db_getnextuid(2))!=NULL) 
		printf("%d\n", tempptr->id);
	printf("Trying account 1\n");
	while((tempptr=db_getnextuid(1))!=NULL) 
		printf("%d\n", tempptr->id);
	printf("Trying account 0\n");
	while((tempptr=db_getnextuid(0))!=NULL) 
		printf("%d\n", tempptr->id);
	printf("Trying account 1\n");
	while((tempptr=db_getnextuid(1))!=NULL) 
		printf("%d\n", tempptr->id);
	printf("Trying account 200\n");
	while((tempptr=db_getnextuid(200))!=NULL) 
		printf("%d\n", tempptr->id);

}
#endif


/*===========================================================================*
 * Function:    olddb_init
 *
 * Abstract:    This function Initializes pre-1.3 data base.
 *
 * Arguments:
 *	dbfd -  open file desciptor for database to be converted.
 *      over -	database overflow area size in term of number
 *		of records that should be increased by each time
 *
 * Return value:
 *	-1	failed
 *	 0	successful
 *===========================================================================*/

int 
olddb_init(dbfd, over)
FILE *dbfd;
int over;
{
	long datalen; /* length of data */
	int ret;
	int i;
	extern char *timstr();

	toffset = sizeof(hashlength) + sizeof(magicno);

	if(over<0) {
	    (void) fprintf(stderr, "Invalid overflow size in olddb_init()\n");
	    return(-2);
	}
	overinc=over;
	if(overinc==0) overinc=OVERDEF;

 	if (dbfd == NULL) {
	    (void) fprintf(stderr, "Invalid file pointer in olddb_init()\n");
	    return(-7);
	}

	if (fseek(dbfd, 0, 2) == (-1)) {	/* go to bottom of file */
	        (void) fprintf(stderr, "Could not fseek() to end of file in olddb_init()\n");
		(void) fclose(dbfd);
		return(-8); 
	}
	datalen = ftell(dbfd);
	datalen -= toffset;
	if(datalen<0) {
	        (void) fprintf(stderr, "Could not ftell() in olddb_init()\n");
		(void) fclose(dbfd);
		return(-9);
	}
	datalen /= sizeof(struct cpulim_info);	
	db=(struct cpulim_info *) malloc(datalen*sizeof(struct cpulim_info));
	if(db==NULL) {
	        (void) fprintf(stderr, "malloc() failed in olddb_init()\n");
		(void) fclose(dbfd);
		return(-10);
	}
	if (fseek(dbfd, 0, 0) == (-1)) { /* reset to top of file */
	        (void) fprintf(stderr, "Could not fseek() to top of file in olddb_init()\n");
		(void) fclose(dbfd);
		return(-11);
	}
	/* read magic number */
	ret=fread(&magicno, sizeof(magicno), 1, dbfd);
	if(ret!=1) {
	        (void) fprintf(stderr, "Could not read magic number in olddb_init()\n");
		(void) fclose(dbfd);
		return(-12);
	}
	/* read length of hashtable */
	ret=fread(&hashlength, sizeof(hashlength), 1, dbfd);
	if(ret!=1) {
	        (void) fprintf(stderr, "Could not read length of hash table in olddb_init()\n");
		(void) fclose(dbfd);
		return(-13);
	}
	/* read table and overflow data */
	ret=fread(db, sizeof(struct cpulim_info), datalen, dbfd);
	if(ret!=datalen) {
	        (void) fprintf(stderr, "Could not read database in olddb_init()\n");
		(void) fclose(dbfd);
		return(-14);
	}
	overlength = datalen - hashlength;
	/* get amount used in overflow */
	usedover=0;
	for(i=0;i<overlength;i++) 
		if(!db[i+hashlength].empty) 
			usedover=i+1;

	(void) fclose(dbfd);
	increased=0;
	return(0);
}


/*===========================================================================*
 * Function:    db_init
 *
 * Abstract:    This function Initializes data base.
 *
 * Arguments:
 *      over -	database overflow area size in term of number
 *		of records that should be increased by each time
 *
 * Return value:
 *	-1	failed
 *	 0	successful
 *
 * Notes:	
 *	It writes to a back-up file first, then switch file name
 *	in case the system crashes while writing.
 *===========================================================================*/

int 
db_init(over)
int over;
{
	long datalen; /* length of data */
	int ret;
	char *db_findgood_err;
	int i;
	struct stat dbstat;
	extern char *timstr();

	toffset = sizeof(hashlength) + sizeof(magicno);

	if(over<0) return(-2);
	overinc=over;
	if(overinc==0) overinc=OVERDEF;

	/*
	  call db_findgood() to initialize dbfile, dbnewfile, dbbakfile,
	  and dboldfile and make sure that dbfile is valid
	 */

	db_findgood_err = malloc(BUFSIZ);
	if(db_findgood_err == NULL) {
	        (void) fprintf (stderr, "%s - malloc() failed in db_init()\n",
			timstr(0));
		(void) fflush(stderr);
	        (void) syslog (LOG_ERR, "malloc() failed in db_init()\n");
		return(-1);
	}
	ret = db_findgood(db_findgood_err);
	switch(ret) {
	case 0: /* no errors */
		break;

	case 1: /* database recovered from backup */
	        (void) fprintf (stderr, "%s - MACS database recovered from backup: %s",
			timstr(0), db_findgood_err);
		(void) fflush(stderr);
	        (void) syslog (LOG_NOTICE, "MACS database recovered from backup:\n");
	        (void) syslog (LOG_NOTICE, db_findgood_err);
	        free(db_findgood_err);
		break;

	case -1: /* no usable database found */
	        (void) fprintf (stderr, "%s - No %s file found; cannot start MACD: %s",
			timstr(0), MACD_DATABASE_FILE, db_findgood_err);
		(void) fflush(stderr);
	        (void) syslog (LOG_ERR, "No MACS database found; cannot start MACD:\n");
	        (void) syslog (LOG_ERR, db_findgood_err);
	        free(db_findgood_err);
		return(-1);

	default: /* unrecognized return code */
	        (void) fprintf (stderr, "%s - Unexpected return code from db_findgood, cannot start MACD: %s",
			timstr(0), db_findgood_err);
		(void) fflush(stderr);
	        (void) syslog (LOG_ERR, "Unexpected return code from db_findgood, cannot start MACD:\n");
	        (void) syslog (LOG_ERR, db_findgood_err);
	        free(db_findgood_err);
		return(-1);
	}
	free(db_findgood_err);

	if ((dbfp = fopen(dbfile, "r")) == (FILE *) NULL)
 	          return(-7);

	if (fseek(dbfp, 0, 2) == (-1)) {	/* go to bottom of file */
		(void) fclose(dbfp);
		return(-8); 
	}
	datalen = ftell(dbfp);
	datalen -= toffset;
	if(datalen<0) {
		(void) fclose(dbfp);
		return(-9);
	}
	datalen /= sizeof(struct cpulim_info);	
	db=(struct cpulim_info *) malloc(datalen*sizeof(struct cpulim_info));
	if(db==NULL) {
		(void) fclose(dbfp);
		return(-10);
	}
	if (fseek(dbfp, 0, 0) == (-1)) { /* reset to top of file */
		(void) fclose(dbfp);
		return(-11);
	}
	/* read magic number */
	ret=fread(&magicno, sizeof(magicno), 1, dbfp);
	if(ret!=1) {
		(void) fclose(dbfp);
		return(-12);
	}
	/* read length of version/release */
	ret=fread((char *)version, sizeof(short), 2, dbfp);
	if(ret!=2) {
		(void) fclose(dbfp);
		return(-13);
	}
	/* read length of hashtable */
	ret=fread(&hashlength, sizeof(hashlength), 1, dbfp);
	if(ret!=1) {
		(void) fclose(dbfp);
		return(-13);
	}
	/* read table and overflow data */
	ret=fread(db, sizeof(struct cpulim_info), datalen, dbfp);
	if(ret!=datalen) {
		(void) fclose(dbfp);
		return(-14);
	}
	overlength = datalen - hashlength;
	/* get amount used in overflow */
	usedover=0;
	for(i=0;i<overlength;i++) 
		if(!db[i+hashlength].empty) 
			usedover=i+1;

	(void) fclose(dbfp);
	increased=0;
	return(0);
}


/*===========================================================================*
 * Function:    db_findgood
 *
 * Abstract:    Examines the MACD_DATABASE_FILE, followed by the .new,
 *		.old, and .bak versions of that file.  The first file
 *		found that is readable, is not too short, and has the
 *		correct checksum and version number is copied (if
 *		necessary) to MACD_DATABASE_FILE.
 *
 * Arguments:   string pointer
 *
 * Return value:
 *	 0	MACD_DATABASE_FILE exists and is usable.  
 *
 *	 1	A usable database file was found and copied to
 *		MACD_DATABASE_FILE.  Error messages describing all the
 *		problems found before that are stored into the string
 *		pointed to by this function's argument.
 *
 *	-1	No usable database file was found.  Error messages
 *		describing all the problems found are stored into the
 *		string pointed to by this function's argument.
 *===========================================================================*/

#define MAINFILE 0  /* MACD_DATABASE_FILE */
#define NEWFILE  1  /* MACD_DATABASE_FILE.new */
#define OLDFILE  2  /* MACD_DATABASE_FILE.old */
#define BAKFILE  3  /* MACD_DATABASE_FILE.bak */
#define NFILES   4  /* number of files listed above */

int db_findgood(log_msg) 
char *log_msg;
{
  int dbfd;
  char sName[PATH_MAX];
  int whichfile;
  char log_tmp[BUFSIZ];
  char cp_cmd[BUFSIZ];
  u_long magicno, stored, computed;
  short version, release;
  u_long errmsgno=0;
  extern int checkSum();

  *log_msg = '\0';

  /* allocate space and initialize database file pathnames */

  dbfile = malloc(strlen(MACD_DATABASE_FILE)+1);
  if(dbfile == NULL) {
	  (void) sprintf(log_tmp, "malloc() failed for dbfile!\n");
	  strcat(log_msg, log_tmp);
	  return(-1);
  }
  (void) sprintf(dbfile, "%s", MACD_DATABASE_FILE);

  dbnewfile = malloc(strlen(MACD_DATABASE_FILE)+5);
  if(dbnewfile == NULL) {
	  (void) sprintf(log_tmp, "malloc() failed for dbnewfile!\n");
	  strcat(log_msg, log_tmp);
	  return(-1);
  }
  (void) sprintf(dbnewfile, "%s.new", MACD_DATABASE_FILE);

  dboldfile = malloc(strlen(MACD_DATABASE_FILE)+5);
  if(dboldfile == NULL) {
	  (void) sprintf(log_tmp, "malloc() failed for dboldfile!\n");
	  strcat(log_msg, log_tmp);
	  return(-1);
  }
  (void) sprintf(dboldfile, "%s.old", MACD_DATABASE_FILE);

  dbbakfile = malloc(strlen(MACD_DATABASE_FILE)+5);
  if(dbbakfile == NULL) {
	  (void) sprintf(log_tmp, "malloc() failed for dbbakfile!\n");
	  strcat(log_msg, log_tmp);
	  return(-1);
  }
  (void) sprintf(dbbakfile, "%s.bak", MACD_DATABASE_FILE);

  /* examine each file in turn until a valid file is found */

  for(whichfile = 0; whichfile < NFILES; whichfile++) {
      switch(whichfile) {
      case MAINFILE: strcpy(sName, dbfile); 
                     break;

      case NEWFILE:  strcpy(sName, dbnewfile); 
                     break;

      case OLDFILE:  strcpy(sName, dboldfile); 
                     break;

      case BAKFILE:  strcpy(sName, dbbakfile); 
                     break;

      default:       (void) sprintf(log_tmp, 
				    "Internal error in db_findgood(): too many passes!\n");
	             strcat(log_msg, log_tmp);
                     return(-1);
      } /* switch */

      if ((dbfd = open(sName, O_RDWR, 0000600)) == -1) {
          (void) sprintf(log_tmp, "Could not open %s for reading... ", sName);
	  strcat(log_msg, log_tmp);
      } else {
	  switch (checkSum(dbfd, &stored, &computed, &version, &release)) {
	  case 0: /* Checksum OK */
                  close(dbfd); 
	          if (version != VERSION || release != RELEASE) {
		      (void) sprintf(log_tmp, 
				     "%s is out of date (version %d.%d, use \"dbconvert\" to update it to version %d.%d)... ", 
				     sName, version, release, VERSION, RELEASE);
	              strcat(log_msg, log_tmp);
		      break;
	          } else {
		      /* checksum OK, version and release OK */
                      if (whichfile == MAINFILE) {
                          return (0);
		      } else {
			  (void) sprintf(log_tmp, "%s is valid... ", sName);
			  strcat(log_msg, log_tmp);
			  (void) sprintf(cp_cmd, "cp %s %s\n", 
					 sName, MACD_DATABASE_FILE);
			  if (system(cp_cmd) != 0) {
                              (void) sprintf(log_tmp, 
					     "Could not copy %s to %s... ", 
					     sName, MACD_DATABASE_FILE);
	                      strcat(log_msg, log_tmp);
			      break;
			  } else {
                              (void) sprintf(log_tmp, 
					     "Successfully copied %s to %s!\n", 
					     sName, MACD_DATABASE_FILE);
	                      strcat(log_msg, log_tmp);
                              return (1);
			  }
		      }
	          }
                  (void) sprintf(log_tmp, "Internal error in db_findgood(): should never reach this point!\n");
	          strcat(log_msg, log_tmp);
		  return(-1);

          case 1: /* File size shorter than the length of checksum field */
                  (void) sprintf(log_tmp, 
                                 "%s is too short, probably corrupted... ", 
				 sName);
	          strcat(log_msg, log_tmp);
		  break;

          case 2: /* Error in reading database file */
                  (void) sprintf(log_tmp, "Error reading %s... ", sName);
	          strcat(log_msg, log_tmp);
		  break;

          case 3: /* Checksum does not match */
                  (void) sprintf(log_tmp, 
                                 "%s is either pre-1.3 or corrupted: checksum mismatch, stored=%ld, computed=%ld (use \"dbconvert\" to convert an older database)... ", 
				 sName, stored, computed);
	          strcat(log_msg, log_tmp);
		  break;

          default: 
		  (void) sprintf(log_tmp, "Internal error in db_findgood(): unexpected return value from checkSum()!\n");
	          strcat(log_msg, log_tmp);
                  return(-1);
          } /* switch */
      } /* else */
  } /* for */

  (void) sprintf(log_tmp, "No more database files to try!\n");
  strcat(log_msg, log_tmp);
  return (-1);
}


/*===========================================================================*
 * Function:    cvt_writeall
 *
 * Abstract:    This function writes a converted data base to disk.
 *
 * Arguments:   data pointer
 *
 * Return value:
 *	-1	failed
 *	 0	successful
 *===========================================================================*/

int cvt_writeall(dbfd)
FILE *dbfd;
{
	long datalen; /* length of data */
	int ret;
	extern u_long calcSum();

	datalen = hashlength + overlength;

	magicno = calcSum ((char *)version, (char *)&hashlength, 
		(char *)db, (long) (sizeof(struct cpulim_info)*datalen));
	ret=fwrite(&magicno, sizeof(magicno), 1, dbfd);
 
        if (0 != fflush(dbfd)) {
		printf("Couldn't flush database (#1) in cvt_writeall()\n");
                return(-1);
	}

	if(ret!=1) {
		printf("Couldn't write magic number in cvt_writeall()\n");
		(void) fclose(dbfd);
		return(-1);
	}
 
	ret=fwrite((char *)version, sizeof(short), 2, dbfd);
	printf ("new database version=%d.%d\n",version[0], version[1]); 
        if (0 != fflush(dbfd)) {
		printf("Couldn't flush database (#2) in cvt_writeall()\n");
                return(-1);
	}

	if(ret!=2) {
		printf("Couldn't write version in cvt_writeall()\n");
		(void) fclose(dbfd);
		return(-1);
	}
 
	ret=fwrite(&hashlength, sizeof(hashlength), 1, dbfd);

        if (0 != fflush(dbfd)) {
		printf("Couldn't flush database (#3) in cvt_writeall()\n");
                return(-1);
	}

	if(ret!=1) {
		printf("Couldn't write hashlength in cvt_writeall()\n");
		(void) fclose(dbfd);
		return(-1);
	}

	ret=fwrite(db, sizeof(struct cpulim_info), datalen, dbfd);

        if (0 != fflush(dbfd)) {
		printf("Couldn't flush database (#4) in cvt_writeall()\n");
                return(-1);
	}

	if(ret!=datalen) {
		printf("Couldn't write database in cvt_writeall()\n");
		(void) fclose(dbfd);
		return(-1);
	}

	(void) fclose(dbfd);
	return(0);
}


/*===========================================================================*
 * Function:    db_writeall
 *
 * Abstract:    This function writes the entire data base to disk.
 *
 * Arguments:
 *      None
 *
 * Return value:
 *	-1	failed
 *	 0	successful
 *
 * Notes:	
 *	It writes to a back-up file first, then switch file name
 *	in case the system crashes while writing.
 *===========================================================================*/

int
db_writeall()
{
	long datalen; /* length of data */
	int ret;
	int dbfd;
	int cdbfd, cdbvalid;
	u_long cstored, ccomputed;
	short cversion, crelease;
	char cp_cmd[BUFSIZ];
	extern char *timstr();
	extern u_long calcSum();

	datalen = hashlength + overlength;

	/*
	  Write the data to a backup file
	  If system crashes, we don't want to corrupt data
	*/

        (void) unlink(dbnewfile);
        if((dbfd=open(dbnewfile, O_SYNC|O_RDWR|O_CREAT|O_TRUNC, S_IWUSR))==0) {
                return(-1);
        }

        if((dbfp=fdopen(dbfd, "w"))==NULL) {
                return(-1);
        }

/* 
        the old code without O_SYNC
	
        (void) unlink(dbnewfile);
	if((dbfp=fopen(dbnewfile, "w"))==NULL) {
		return(-1);
	}	
*/
	magicno = calcSum ((char *)version, (char *)&hashlength, 
		(char *)db, (long) (sizeof(struct cpulim_info)*datalen));
	ret=fwrite(&magicno, sizeof(magicno), 1, dbfp);
 
        if (0 != fflush(dbfp)) 
                return(-1);

	if(ret!=1) {
		(void) fclose(dbfp);
		return(-1);
	}
 
	ret=fwrite((char *)version, sizeof(short), 2, dbfp);
 
        if (0 != fflush(dbfp)) 
                return(-1);

	if(ret!=2) {
		(void) fclose(dbfp);
		return(-1);
	}
 
	ret=fwrite(&hashlength, sizeof(hashlength), 1, dbfp);

        if (0 != fflush(dbfp)) 
                return(-1);

	if(ret!=1) {
		(void) fclose(dbfp);
		return(-1);
	}

	ret=fwrite(db, sizeof(struct cpulim_info), datalen, dbfp);

        if (0 != fflush(dbfp)) 
                return(-1);

	if(ret!=datalen) {
		(void) fclose(dbfp);
		return(-1);
	}

        if (0 != fsync(dbfd))
                return(-1);

	(void) fclose(dbfp);
	(void) close(dbfd); /* to be 100 % sure it goes directly to disk */

	/*
	  Now that the data is saved, make it the current database file
	*/

	/* Remove previous .old file */
	(void) unlink (dboldfile);

	/* Is the current dbfile valid? */
	cdbvalid=1;
        if ((cdbfd = open(dbfile, O_RDONLY, 0000600)) == -1) {
       	    (void) printf("WARNING  : %s - Old %s is unreadable\n",
                    timstr(0), dbfile);
		    cdbvalid=0;
        } else {
	    if (checkSum(cdbfd, &cstored, &ccomputed, &cversion, &crelease) != 0) {
       	        (void) printf("WARNING  : %s - Old %s has invalid checksum\n",
                        timstr(0), dbfile);
		        cdbvalid=0;
	    } else {
	        if (cversion != VERSION || crelease != RELEASE) {
       	            (void) printf("WARNING  : %s - Old %s has invalid database version\n",
                            timstr(0), dbfile);
		            cdbvalid=0;
		}
	    }
            close(cdbfd); 
        }

	/* If the current dbfile is valid, rename it to .old;
	   otherwise, copy the .new file to .old */
	if(cdbvalid) {
	    if (rename(dbfile, dboldfile) < 0) {
       	        (void) printf("WARNING  : %s - Could not rename old %s to %s\n",
                        timstr(0), dbfile, dboldfile);
	    }
	} else {
	    (void) sprintf(cp_cmd, "cp %s %s\n", 
			   dbnewfile, dboldfile);
	    if (system(cp_cmd) != 0) {
       	        (void) printf("WARNING  : %s - Could not copy %s to %s\n",
                        timstr(0), dbnewfile, dboldfile);
	    } else {
       	        (void) printf("MACDINFO : %s - Copied current database from memory to %s as well as %s\n",
                        timstr(0), dboldfile, dbfile);
	    }
	}

	/* Rename .new file to take the place of the current dbfile */
	if (rename(dbnewfile, dbfile) < 0) {
       	    (void) printf("WARNING  : %s - Could not rename %s to %s\n",
                    timstr(0), dbnewfile, dbfile);
	}

	increased=0;
	return(0);
}


/*===========================================================================*
 * Function:    db_writeuid
 *
 * Abstract:    This function writes the memory struct for a specified 
 *		user/account to disk.
 *		(The current implementation simply calls db_writeall() 
 *              to write the whole database to disk.)
 *
 * Arguments:
 *      uid -	user id
 *	agid -	account id
 *
 * Return value:
 *	-1	failed
 *	 0	successful
 *
 * Notes:	
 *===========================================================================*/

int
db_writeuid(uid, agid)
int uid, agid;
{
	return(db_writeall());
}


/*===========================================================================*
 * Function:    db_writeagid
 *
 * Abstract:    This function writes the memory struct for a specified 
 *		account to disk.
 *		(The current implementation simply calls db_writeall() 
 *              to write the whole database to disk.)
 *
 * Arguments:
 *	agid -	account id
 *
 * Return value:
 *	-1	failed
 *	 0	successful
 *
 * Notes:	
 *===========================================================================*/

int
db_writeagid(agid)
int agid;
{
	return(db_writeall());
}


/*===========================================================================*
 * Function:    db_getuid
 *
 * Abstract:    This function returns a pointer to the memory struct for 
 *		a specified user/account.
 *
 * Arguments:
 *	uid -	user id
 *	agid -	account id
 *
 * Return value:
 *	NULL	failed
 * 	!= NULL	successful
 *
 * Notes:	
 *===========================================================================*/

struct cpulim_info *
db_getuid(uid, agid)
int uid, agid;
{
	int curr;

	for(curr=uid%hashlength;curr>=0;curr=db[curr].next) {
		if(db[curr].empty) break;
		if(db[curr].uid_pred && 
		   db[curr].id == uid && db[curr].agid == agid)
			return(&(db[curr]));
	}

	return(NULL);
}


/*===========================================================================*
 * Function:    db_getagid
 *
 * Abstract:    This function returns a pointer to the memory struct for 
 *		a specified account.
 *
 * Arguments:
 *	agid -	account id
 *
 * Return value:
 *	NULL	failed
 * 	!= NULL	successful
 *
 * Notes:	
 *===========================================================================*/

struct cpulim_info *
db_getagid(agid)
int agid;
{
	int curr;

	for(curr=agid%hashlength;curr>=0;curr=db[curr].next) {
		if(db[curr].empty) break;
		if(!db[curr].uid_pred && db[curr].id == agid)
			return(&(db[curr]));
	}

	return(NULL);
}


/*===========================================================================*
 * Function:    db_getnextuid
 *
 * Abstract:    This function returns a pointer to the memory struct of 
 *		next user under a specified account.
 *
 * Arguments:
 *	agid -	account id
 *
 * Return value:
 *	NULL	failed
 * 	!= NULL	successful
 *
 * Notes:	
 *===========================================================================*/

struct cpulim_info *
db_getnextuid(agid)
int agid;
{
	static int ac = (-1);
	static long pos = 0;
	int total;

	if(agid!=ac) {
		pos=0;
		ac=agid;
	}

	for(total=hashlength+overlength;pos<total;pos++) 
		if(!db[pos].empty && db[pos].uid_pred &&
		   db[pos].agid == agid) {
			pos++;
			return(&(db[pos-1]));
		}
	
	ac = (-1);
	return(NULL);
}


/*===========================================================================*
 * Function:    db_insuid
 *
 * Abstract:    This function inserts a given entry for user/account into 
 *		the database in memory
 *
 * Arguments:
 *	uid -	user id
 *	agid -	account id
 *	entry -	pointer to a database entry to be inserted
 *
 * Return value:
 *	-1	failed
 *	 0	successful
 * 	 1	successful with overflow area had to be increased
 *
 * Notes:	
 *	This functions uses it's own memory for the entry leaving
 *	the caller free to destroy the entry passed to the function.
 *===========================================================================*/

int
db_insuid(uid, agid, entry)
int uid, agid;
struct cpulim_info *entry;
{
	int curr,ret;

	curr=uid%hashlength;

	if(!db[curr].empty) {
		if(overlength==usedover) {  /* FULL */
			ret=extendover();
			if(ret<0) return(ret);
			ret=db_insuid(uid,agid,entry);
			if(ret<0) return(ret);
			return(1);
		}
		for(;db[curr].next >= 0;curr = db[curr].next);
		db[curr].next = hashlength+usedover;
		curr = hashlength+usedover;
		usedover++;
	}
	
	(void) datacopy(&(db[curr]), entry);
	db[curr].empty = 0;
	db[curr].next = (-1);	
	db[curr].uid_pred = 1;
	db[curr].id = uid;
	db[curr].agid = agid;
	return(0);
}


/*===========================================================================*
 * Function:    db_insagid
 *
 * Abstract:    This function inserts a given account entry into the database
 *		in memory
 *
 * Arguments:
 *	agid -	account id
 *	entry -	pointer to a database entry to be inserted
 *
 * Return value:
 *	-1	failed
 *	 0	successful
 * 	 1	successful with overflow area had to be increased
 *
 * Notes:	
 *	This functions uses it's own memory for the entry leaving
 *	the caller free to destroy the entry passed to the function.
 *===========================================================================*/

int
db_insagid(agid, entry)
int agid;
struct cpulim_info *entry;
{
	int curr,ret;

	curr=agid%hashlength;

	if(!db[curr].empty) {
		if(overlength==usedover) {  /* FULL */
			ret=extendover();
			if(ret<0) return(ret);
			ret=db_insagid(agid,entry);
			if(ret<0) return(ret);
			return(1);
		}
		for(;db[curr].next>=0;curr=db[curr].next);
		db[curr].next=hashlength+usedover;
		curr=hashlength+usedover;
		usedover++;
	}
	
	(void) datacopy(&(db[curr]), entry);
	db[curr].empty=0;
	db[curr].next = (-1);	
	db[curr].uid_pred=0;
	db[curr].id = agid;
	return(0);
}


/*===========================================================================*
 * Function:    db_deluid
 *
 * Abstract:    This function deletes a entry of a specified user/account
 *		from the database in memory
 *
 * Arguments:
 *	uid -	user id
 *	agid -	account id
 *
 * Return value:
 *	-1	failed
 *	 0	successful
 *
 * Notes:	
 *===========================================================================*/

int
db_deluid(uid, agid)
int uid, agid;
{
	int curr,next;
	int found = (-1);
	int prev = (-1);

	for(curr=uid%hashlength;curr>=0;prev=curr,curr=db[curr].next) {
		if(db[curr].empty) break;
		if(db[curr].uid_pred && 
		   db[curr].id == uid && db[curr].agid == agid) {
			found=curr;
			break;
		}
	}

	if(found<0) return(-1);

	next=db[found].next;
	if(next<0) {   /* end of chain */
		db[found].empty=1;
		db[found].next = (-1);
		if(prev>=0) db[prev].next = (-1);
	}
	else {			/* chain after this element */
		(void) datacopy(&(db[found]), &(db[next]));
		db[next].empty=1;
		db[next].next = (-1);
	}
	return(0);

}


/*===========================================================================*
 * Function:    db_delagid
 *
 * Abstract:    This function deletes a entry of a specified account
 *		from the database in memory.  The account must have 0
 *		members (user).
 *
 * Arguments:
 *	agid -	account id
 *
 * Return value:
 *	-1	failed
 *	 0	successful
 *
 * Notes:	
 *===========================================================================*/

int
db_delagid(agid)
int agid;
{
	int curr,next;
	int found = (-1);
	int prev = (-1);

	for(curr=agid%hashlength;curr>=0;prev=curr,curr=db[curr].next) {
		if(db[curr].empty) break;
		if(!db[curr].uid_pred && db[curr].id == agid) {
			found=curr;
			break;
		}
	}

	if(found<0) return(-1);

	next=db[found].next;
	if(next<0) {   /* end of chain */
		db[found].empty=1;
		db[found].next = (-1);
		if(prev>=0) db[prev].next = (-1);
	}
	else {		/* chain after this element */
		(void) datacopy(&(db[found]), &(db[next]));
		db[next].empty=1;
		db[next].next = (-1);
	}
	return(0);
}


/*===========================================================================*
 * Function:    rehash
 *
 * Abstract:    This function mallocs a new block of memory with the
 *		specified hash number and current overflow size and
 *		rehash from the old memory into the new area.
 *
 * Arguments:
 *	hashsize -	in term of number of records (entries)
 *
 * Return value:
 *	-1	failed
 *	 0	successful
 *
 * Notes:	
 *===========================================================================*/

int
rehash(hashsize)
int hashsize;
{
	struct cpulim_info *tempdb;
	int totallength, length;
	int i,ret;
	int oldusedover,oldhashlength;

	oldusedover=usedover;
	oldhashlength=hashlength;
	tempdb=db;

	totallength = hashsize+overlength;
	length = totallength*sizeof(struct cpulim_info);
	db = (struct cpulim_info *) malloc(length);
	if(db==NULL) {
		db=tempdb;
		return(-1);
	}
	for(i=0;i<totallength;i++) {
		db[i].empty = 1;
		db[i].next = (-1);
	}
	usedover=0;
	hashlength=hashsize;
	totallength = oldhashlength+overlength;
	for(i=0;i<totallength;i++) {
		if(tempdb[i].empty) continue;
		else
		if(tempdb[i].uid_pred) {
			ret=db_insuid(tempdb[i].id,tempdb[i].agid,&(tempdb[i]));
			if(ret<0) {
				(void) free(db);
				db=tempdb;
				usedover=oldusedover;
				hashlength=oldhashlength;
				return(-1);
			}
		}
		else {
			ret=db_insagid(tempdb[i].id,&(tempdb[i]));
			if(ret<0) {
				(void) free(db);
				db=tempdb;
				usedover=oldusedover;
				hashlength=oldhashlength;
				return(-1);
			}
		}
	}
	increased=1;
	(void) free(tempdb);
	return(0);
}


/*===========================================================================*
 * Function:    datacopy
 *
 * Abstract:    This function copies entry2's contents into entry1.
 *
 * Arguments:
 *      entry1 -        pointer to a new entry
 *      entry2 -        pointer to an old entry
 *
 * Return value:
 *	None
 *
 * Notes:
 *===========================================================================*/

void
datacopy(entry1, entry2)
struct cpulim_info *entry1, *entry2;
{
	entry1->id = entry2->id;
	entry1->agid = entry2->agid;
	entry1->empty = entry2->empty;
	entry1->uid_pred = entry2->uid_pred;
	entry1->next = entry2->next;
	entry1->ref_cnt = entry2->ref_cnt;
	entry1->shutoff = entry2->shutoff;
	entry1->weight = entry2->weight;
	entry1->inhibit = entry2->inhibit;
	entry1->modify = entry2->modify;
	entry1->transfer = entry2->transfer;
	entry1->use = entry2->use;
	entry1->percent = entry2->percent;
	entry1->killjobs = entry2->killjobs;
	entry1->lockjobs = entry2->lockjobs;
	entry1->unlimit = entry2->unlimit;
	entry1->unused2 = entry2->unused2;
	entry1->maxnodes = entry2->maxnodes;
	entry1->authorized = entry2->authorized;
	entry1->used_time = entry2->used_time;
	entry1->sbu_time = entry2->sbu_time;
	entry1->non_cpu_time = entry2->non_cpu_time;
	entry1->timestamp = entry2->timestamp;
}


/*===========================================================================*
 * Function:    extendover
 *
 * Abstract:    This function extends the overflow table by the amount
 *		specified by the global variable overinc
 *
 * Arguments:
 *	None
 *
 * Return value:
 *      -1      failed
 *       0      successful
 *
 * Notes:
 *===========================================================================*/

int
extendover()
{
	int i,totallength;
	extern char *realloc();

	totallength =(overlength+overinc+hashlength)*sizeof(struct cpulim_info);
	db=(struct cpulim_info *)realloc(db, totallength);
	if(db==NULL) return(-1);
	overlength+=overinc;
	increased=1;
	totallength = hashlength+overlength;
	for(i=totallength-overinc;i<totallength;i++) {
		db[i].empty=1;
		db[i].next = (-1);
	}
	return(0);
}


#ifdef DB_DEBUG
dump()
{
	printf("hashlength = %d\n", hashlength);
	printf("overlength = %d\n", overlength);
	printf("usedover = %d\n", usedover);
	printf("magicno = %d\n", magicno);
}
#endif
