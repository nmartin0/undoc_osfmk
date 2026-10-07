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
 
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <malloc.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

extern char *malloc();
extern int errno;
#define NUM_PTRS	3000
char *namptr[NUM_PTRS];
#define MAX_FILES 4096
#define MACD_FILENAME_LEN 12

char **vDirSearch(cp)
   char           *cp;
{
   DIR            *dirp;	/* Open queue order dir structure */
   struct dirent  *dp;		/* Ptr to directory entry */
   int             vpBase, iSwitched = 0;
   int             iC = 0, i, j;
   char           *cTmp;
   char    **cpArray = (char **)malloc(sizeof (char *) * MAX_FILES);

   cpArray[0] = NULL;

   if ((dirp = opendir(cp)) == (DIR *) 0) {
      return (NULL);
   }
   for (dp = readdir(dirp); dp != (struct dirent *) NULL; dp = readdir(dirp)) {
      if (strcmp(dp->d_name, ".") == 0)
	 continue;
      if (strcmp(dp->d_name, "..") == 0)
	 continue;
      if (0 == strncmp(dp->d_name, "macd", 4)) {
	 if (strlen(dp->d_name) == MACD_FILENAME_LEN) {
	    cpArray[iC++] = malloc(MACD_FILENAME_LEN + 1);
	    if (cpArray[iC - 1] == NULL) {
	       return (NULL);	/* malloc failed */
	    }
	    strcpy(cpArray[iC - 1], dp->d_name);
	 }
	 /* ic is the counter */
      }
      if (iC >= MAX_FILES) {
	 return (NULL);		/* to much files */
      }
   }
   cpArray[iC] = NULL;

#ifdef DEBUG
   for (i = 0; i < MAX_FILES; i++) {
      if (cpArray[i] == NULL) {
	 continue;
      }
      printf("Entry1='%s', i=%d\n", cpArray[i], i);
   } 
#endif

   /* bubble sort */
   do {
      iSwitched = 0;
      for (i = 0; i < iC; i++) {
	 for (j = i; j < iC; j++) {
	    if (strcmp(cpArray[i], cpArray[j]) > 0) {
	       cTmp = cpArray[j];
	       cpArray[j] = cpArray[i];
	       cpArray[i] = cTmp;
	       iSwitched = 1;
	       /* printf("switched i=%d, j=%d\n", i, j); */
	    }
	 }
      }
   } while (iSwitched);

#ifdef DEBUG
   for (i = 0; i < MAX_FILES; i++) {
      if (cpArray[i] == NULL) {
	 continue;
      }
      printf("Entry2='%s', i=%d\n", cpArray[i], i);
   } 
#endif

   if (0 != closedir(dirp)) {
      /* we should do something about error code */
      return (NULL);
   }
   return (cpArray);
}


/*
 * This library call check logfile entries, return an array of file
 * name strings which contain all the accounting data for the period
 * starting at time_t st and ending at time_t et.  It also returns int
 * *nfiles to indicate the number of files in the array.
 *
 * Upon error condition, lognames returns NULL pointer and stores a
 * negative integer in *nfiles to indicate the type of error encountered
 *
 *	-1	Invalid input parameters: st >= et
 *	-2	error in reading log directory
 *	-3	error in reading file names
 *	-4	No data for the given st and et
 *	-5	error in input parameters: logpath
 */

char **
lognames(st, et, nfiles, logpath)
time_t *st, et;
int *nfiles;
char *logpath;
{
	int fname_int, start_int, end_int;
	int scan, i, j;
	char *fname, *fname_next;
	char *cmd, *cur_ptr, *pre_ptr, *tmp_ptr, **pstr;
	struct tm *start_tm, *end_tm, *tmp_tm;
	extern char *ctime();

#ifdef DEBUG
	(void) fprintf (stderr, 
		"start lognames(st=%d,et=%d,nfiles=%d,logpath=%s)\n",
		*st,et,*nfiles,logpath);
	(void) fflush (stderr);
#endif

	if (logpath == NULL || strlen(logpath) == 0) {
		*nfiles = -5;
		return (NULL);
	}

	if (*st >= et || *st >= time(0)) {
		*nfiles = -1;
		return (NULL);
	}

	for (scan = 1; scan < NUM_PTRS; scan++) {
		namptr[scan] = NULL;
	}
	*nfiles = 0;

	start_tm = localtime (st);
	start_int = (start_tm->tm_year +1900) * 10000 +
		(start_tm->tm_mon +1) * 100 + start_tm->tm_mday;
	end_tm = localtime (&et);
        end_int = (end_tm->tm_year +1900) * 10000 +
                (end_tm->tm_mon +1) * 100 + end_tm->tm_mday;

#ifdef DEBUG
	(void) fprintf (stderr, "done initialization\n");
	(void) fflush (stderr);
#endif
	if ((pstr = vDirSearch (logpath)) == NULL) {
	    (void) fprintf(stderr, "Unable to read directory %s\n", logpath);
	    (void) fflush (stderr);
	    *nfiles = -2;
	    return(NULL);
        }

#ifdef DEBUG
	(void) fprintf (stderr, "start reading from pipe\n");
	(void) fflush (stderr);
#endif
	/* read the first valid file name */
	for (i=0, fname=pstr[0]; i<MAX_FILES; i++, fname=pstr[i]) {
		if (fname == NULL) continue;
		fname[MACD_FILENAME_LEN] = '\0'; /* take off the \n */
		if ((scan = sscanf(fname,"macd%d",&fname_int)) != 1) {
#ifdef DEBUG
			(void) fprintf (stderr, "read in macd%d\n", fname_int);
			(void) fflush (stderr);
#endif
			continue;
		}
		if (fname_int > end_int) {
			*nfiles = -4;
			return (NULL);
		}
		if (fname_int > start_int) {
			char buf[256], dummy[256];
			int rval;
			time_t t_stamp;
			FILE *fstr;
			(void) sprintf (buf, "%s%s",logpath,fname);
			if ((fstr = fopen (buf, "r")) == NULL) {
				(void) fprintf (stderr, 
					"Cannot open file %s to get the earliest time\n",
					buf);
				(void) fflush (stderr);
				*nfiles = -6;
				return (NULL);
			}
			while (fgets (buf, 256, fstr) != NULL)
                		if ((rval = sscanf (buf, "%s : (%ld)",
                        		dummy, &t_stamp)) == 2) {
					*st = t_stamp;
					(void) fclose (fstr);
					break;
				}
			(void) fclose (fstr);
			if (*st != t_stamp || *st > et) {
				(void) fprintf (stderr, 
					"Start_time reset error, new st=%d\n",
					*st);
				(void) fflush (stderr);
				*nfiles = -6;
				return (NULL);
			}
			start_int = fname_int;
			(void) fprintf (stderr,
				"Lognames:  Reset start time, no data before %s\n", 
				ctime(st));
	    		(void) fflush (stderr);
		}
	
		break;
	}

	/* no valid file name found */
	if (fname == NULL) {
            	*nfiles = -3;
            	return(NULL);
        }

	for (pre_ptr=fname,cur_ptr=pstr[++i];
		i<MAX_FILES; cur_ptr = pstr[++i]) {
		if (cur_ptr == NULL) continue;
		cur_ptr[MACD_FILENAME_LEN] = '\0'; /* take off the \n */
		if ((scan = sscanf(cur_ptr,"macd%d",&fname_int)) != 1) {
#ifdef DEBUG
			(void) fprintf (stderr, "read in macd%d\n", fname_int);
			(void) fflush (stderr);
#endif
			continue;
		}
#ifdef DEBUG
		(void) fprintf (stderr, "read in macd%d\n", fname_int);
		(void) fflush (stderr);
#endif
		if (fname_int <= start_int) {
		    tmp_ptr=pre_ptr; 
		    pre_ptr=cur_ptr; 
		    cur_ptr=tmp_ptr;
		    continue;
		}
		if (fname_int > end_int) break;
#ifdef DEBUG
		(void) fprintf (stderr, "fname_int(%d) > start_int(%d)\n", 
			fname_int, start_int);
		(void) fflush (stderr);
#endif

		/* previous file should be in */
		if ((namptr[*nfiles] = 
			malloc(strlen(pre_ptr) + strlen(logpath) + 1)) 
			== NULL ) {
			perror("lognames, malloc error");
	    		*nfiles = 0;
			return(NULL);
		}
		(void) sprintf(namptr[(*nfiles)++], "%s%s",
			logpath, pre_ptr);
#ifdef DEBUG
		(void) fprintf (stderr,
			"nfiles=%d, fname=%s\n", *nfiles, namptr[*nfiles]);
		(void) fflush (stderr);
#endif
	    tmp_ptr=pre_ptr; 
	    pre_ptr=cur_ptr; 
	    cur_ptr=tmp_ptr;
	}

	/* previous file should be in */
	if ((namptr[*nfiles] = malloc(strlen(pre_ptr) + strlen(logpath) + 1)) 
		== NULL ) {
		perror("lognames, malloc error");
    		*nfiles = 0;
		return(NULL);
	}
	(void) sprintf(namptr[(*nfiles)++],"%s%s",logpath, pre_ptr);
	return(namptr);		
}

