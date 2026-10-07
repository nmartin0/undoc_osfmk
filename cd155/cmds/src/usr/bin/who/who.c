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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: who.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:47:30 $";
#endif
/*
 * COMPONENT_NAME: (CMDSTAT) status
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989, 1990
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 * who.c	1.20  com/cmd/stat,3.1,9013 3/1/90 17:27:46 
 */

#include <NLctype.h>
#include <time.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utmp.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <signal.h>
#include <fcntl.h>
#include <locale.h>
#include "who_msg.h"

nl_catd catd;
#define MSGSTR(Num, Str) catgets(catd, MS_WHO, Num, Str)


#define FAILURE -1
#define MAXLINE 100
#define FLAGS   UTMAXTYPE
#define SIZEUSER        32
#define	LOGF_SIZE	9
#define HOST_SIZE	17
#define MAXUTSZ		9		/* max field width of username */
#define MAXUTLSZ	12		/* max field width of line */
#define UTLSZ	(sizeof(utmp->ut_line))

extern void utmpname(char *newfile);
extern struct utmp *getutent(void);
int usage(char *format, char *arg1);
char *user(struct utmp *utmp);
char *host(struct utmp *utmp);
char *line(struct utmp *utmp);
static void copypad(char *to, char *from, int size);
char *id(struct utmp *utmp);
char *etime(struct utmp *utmp);
char *mode(struct utmp *utmp);
int alarmclk(void);
char *loc(struct utmp *utmp);


char username[SIZEUSER + 1];

int shortflag;          /* Set when fast form of "who" is to run. */
int ttystatus;          /* Set when write to tty status desired.  */
int allflag;            /* Set when all entries are to be dumped. */
int header=0;		/* Print a header                         */
int quickly=0;		/* Just print the number of people and their names */
int totaluser=0;	/* Count how many users logged in.        */

main(int argc, char **argv)
{
	static int entries[FLAGS+1];    /* Flag for each type entry */
	register struct utmp *utmp;
	register int i;
	char *ptr;
	struct passwd *passwd;
	char outbuf[BUFSIZ];
	extern int shortflag,ttystatus,allflag;
	int foundone = 0;               /* found a matching utmp entry */

	(void) setlocale (LC_ALL,"");
	catd = catopen(MF_WHO, 0);

/* Make "stdout" buffered to speed things up. */

	setbuf(stdout, outbuf);

/* Look up our own name for reference purposes. */

	if((passwd = getpwuid(getuid())) == (struct passwd *)NULL) {
		 username[0] = '\0';
	} else {
/* Copy the user's name to the array "username". */
		 strncpy(&username[0],passwd->pw_name,(size_t)SIZEUSER);
		 username[SIZEUSER] = '\0';
	}

/* Set "ptr" to null pointer so that in normal case of "who" */
/* the "ut_line" field will not be looked at.  "ptr" is used */
/* only for "w me" command. */

	ptr = NULL;
	shortflag = 0;          /* not set yet */
	ttystatus = FALSE;

	++argv;
	--argc;

	while(argc > 0) {
/* Is this the "who am i" sequence? */
/* In the MSG version, both traditional "who am i" and local version of
   the "am i" are supported. */

		if ((argc >= 2)
		     && ((strcmp(*argv,"am") == 0 && ((strcmp(*(argv+1),"i")==0
		     || strcmp(*(argv+1),"I") == 0)))
			  || (strcmp(*argv,MSGSTR(AMI,"am i")) == 0)))  {
				entries[USER_PROCESS] = TRUE;

/* Which tty am I at?  Get the name and set "ptr" to just past */
/* the "/dev/" part of the pathname. */

			if((ptr = ttyname((int)fileno(stdin))) == NULL &&
			   (ptr = ttyname((int)fileno(stdout))) == NULL &&
			   (ptr = ttyname((int)fileno(stderr))) == NULL) {
				usage(MSGSTR(NOTERM,"process not attached to terminal"), "");
			}
			ptr += sizeof("/dev/") - 1;
			argc -= 2;
			argv += 2;
			continue;
		} else if(**argv == '-') {

/* Analyze the switches and set the flags for each type of entry */
/* that the user requests to be printed out. */

			while(*++*argv) {
				switch(**argv) {
				case 'r' :
					entries[RUN_LVL] = TRUE;
					break;
				case 'b' :
					entries[BOOT_TIME] = TRUE;
					break;
				case 't' :
					entries[OLD_TIME] = TRUE;
					entries[NEW_TIME] = TRUE;
					break;
				case 'p' :
					entries[INIT_PROCESS] = TRUE;
					break;
				case 'l' :
					entries[LOGIN_PROCESS] = TRUE;
					if(shortflag == 0) shortflag = -1;
					break;
				case 'u' :
					entries[USER_PROCESS] = TRUE;
					if(shortflag == 0) shortflag = -1;
					break;
				case 'd' :
					entries[DEAD_PROCESS] = TRUE;
					if (shortflag ==0) shortflag = -1;
					break;
				case 'A' :
					entries[ACCOUNTING] = TRUE;
					break;
				case 'a' :
					for(i = 1; i < FLAGS; i++) entries[i] = TRUE ;
					if(shortflag == 0) shortflag = -1;
					header++;
					ttystatus = TRUE;
					allflag = TRUE;
					break;
				case 's' :
					shortflag = 1;
					break;
				case 'T' :
					ttystatus = TRUE;
					break;
				case '?' :
				case 'h' :
					usage("", "");
					break;
				case 'q' :
					quickly++;
					break;
				case 'H' :
					header++;
					break;
				default :
					usage(MSGSTR(BADSWTCH, "bad switch"), "");
				}
			}               /* End of "while (*++*argv)" */

/* Advance to next argument and reduce count of arguments left. */

			++argv;
			--argc;
			continue;
		} else if(access(*argv,0) != FAILURE) {
/* Is there an argument left?  If so, assume that it is a utmp */
/* like file.  If there isn't one, use the default file. */

			utmpname(*argv);
			++argv;
			--argc;
			continue;
		} else {
			usage(MSGSTR(NOEXST, "%s doesn't exist or isn't readable"), *argv);
		}
	}

/* Make sure at least one flag is set. */

	for(i=0; i <= FLAGS && entries[i] == FALSE; i++) ;
	if(i > FLAGS) entries[USER_PROCESS] = TRUE;

/* Now scan through the entries in the utmp type file and list */
/* those matching the requested types. */
	if (header) {
		fprintf (stdout,"%-*s",LOGF_SIZE, MSGSTR(NAME,"Name    "));
		if (ttystatus)
			fprintf (stdout,"%2.2s", MSGSTR(TTYSTATUS,"ST"));
		else
			fprintf (stdout,"  ");
		if (entries[ACCOUNTING] == FALSE && !quickly) {
			fprintf (stdout," %-*s %12.12s", MAXUTLSZ,
				MSGSTR(WHO_LINE,"Line "),
				MSGSTR(WHO_TIME,"   Time   "));
			if (shortflag == -1) {
				fprintf (stdout,"%9.9s %8.8s",
					MSGSTR(ACTIVITY,"Activity"),
					MSGSTR(WHO_PID,"PID"));
			}
			else
				fprintf (stdout,"%9.9s ","");
			fprintf(stdout," %s","Hostname"); 
			if (shortflag != 0) 
				fprintf (stdout,"%s", MSGSTR(WHO_EXIT,"/Exit"));
		}
		fprintf (stdout,"\n");
	}
		

	while((utmp = getutent()) != NULL) {
/* Are we looking for this type of entry? */
		if(allflag || ((utmp->ut_type >= 0 && utmp->ut_type <= FLAGS) &&
		   entries[utmp->ut_type] == TRUE) ) {
			if(utmp->ut_type == EMPTY) {
				fprintf(stdout, MSGSTR(EMPTYSLT,"Empty slot.\n"));
				continue;
			}
			if(ptr != NULL && strncmp(utmp->ut_line,ptr,(size_t)sizeof(utmp->ut_line)) != 0)
				continue;
			else {
				foundone = 1;                           /* remember that we saw at least one*/
				if (quickly) {
					fprintf(stdout, "%-*s %-*s\n",LOGF_SIZE,user(utmp), HOST_SIZE, host(utmp));
					totaluser++;
					continue;
				}
			}

			if(utmp->ut_type == RUN_LVL) {
				fprintf(stdout, 
					"%-*s %1.1s %-*s %s     %c     %d     %c\n",
                                        LOGF_SIZE, user(utmp),
                                        mode(utmp),
					LOGF_SIZE+MAXUTLSZ+3, utmp->ut_line,
					etime(utmp),
					utmp->ut_exit.e_termination,
					utmp->ut_pid,
					utmp->ut_exit.e_exit);

				continue;
			}
			if (utmp->ut_type == BOOT_TIME) {
				fprintf(stdout, "%-*s %s\n",
					LOGF_SIZE+MAXUTLSZ+3, 
					utmp->ut_line, etime(utmp));
				continue;
			}

			fprintf(stdout, "%-*s %1.1s %-*s %-21.21s",
					LOGF_SIZE, user(utmp), mode(utmp), 
					MAXUTLSZ, line(utmp), etime(utmp));

			if(shortflag < 0 &&
			   (utmp->ut_type == LOGIN_PROCESS
			    || utmp->ut_type == USER_PROCESS
			    || utmp->ut_type == INIT_PROCESS
			    || utmp->ut_type == DEAD_PROCESS)) {
				fprintf(stdout, "%9d", utmp->ut_pid);
				if(utmp->ut_type == INIT_PROCESS
				   || utmp->ut_type == DEAD_PROCESS) {
					fprintf(stdout, MSGSTR(ID, " id=%7.7s"), id(utmp));
					if(utmp->ut_type == DEAD_PROCESS) {
						fprintf(stdout, MSGSTR(TERMID, " term=%d exit=%d"),
							utmp->ut_exit.e_termination, utmp->ut_exit.e_exit);
					}
				}
				else
					fprintf(stdout, "%-*s", HOST_SIZE, host(utmp));
				fprintf(stdout,"\n"); 
			} else {
				fprintf(stdout, "%-*s", HOST_SIZE, host(utmp));
				fprintf(stdout,"\n"); 
			}
		}

	}               /* End of "while ((utmp = getutent()" */
	if (quickly) 
		fprintf (stdout,"Total users: %d\n", totaluser);
	else if (!foundone && (entries[USER_PROCESS] == TRUE)) {
		/* if no match for who am i, print something */
		/* fix an *unfortunate* reference to a NULL pointer - tej */
		if (ptr == NULL)
			fprintf(stdout, "%-10s \n",username);
		else
			fprintf(stdout, "%-10s %s \n",username,ptr);
	}
	fclose(stdout);
	return(0);
}


/*
 *  NAME:  usage
 *
 *  FUNCTION:  print out the usage statement
 *	      
 *  RETURN VALUE:  	 exit 1
 */

usage(char *format, char *arg1)
{
	fprintf(stderr,format,arg1);
	fprintf(stderr,MSGSTR(USAGE, "\nUsage:\twho [-AHTabdlpqrstu] [am i] [file]\n"));
	fprintf(stderr,MSGSTR(USEINFO, "\nu\tuseful information\n"));
	fprintf(stderr,MSGSTR(TTYMSG, "T\tstatus of tty (+ writable, - not writable, x exclusive open, ? hung)\n"));
	fprintf(stderr,MSGSTR(PROCCNT, "l\tlogin processes\nd\tdead processes\n"));
	fprintf(stderr,MSGSTR(BOOTTM, "b\tboot time\nt\ttime changes\n"));
	fprintf(stderr,MSGSTR(OPTIONS, "a\tall (THabdlpqrstu options)\n"));
	fprintf(stderr,MSGSTR(SHRTFRM, "s\tshort form of who (no time since last output or pid)\n"));
	exit(1);
}

/*
 *  NAME:  user
 *
 *  FUNCTION:  	return a space padded buffer of the user
 *		name in a utmp structure.
 *	      
 *  RETURN VALUE: a pointer to the array is returned.
 */

#define UTSZ	(sizeof(utmp->ut_user))
char *user(struct utmp *utmp)
{
	static char uuser[UTSZ + 4];
	static int  uwidth = UTSZ;

	if (UTSZ > MAXUTSZ) 
		uwidth = MAXUTSZ;
	copypad(uuser,utmp->ut_user,uwidth);
	return(uuser);
}

/*
 *  NAME: host
 *
 *  FUNCTION: return a space padded buffer of the host
 *             in a utmp structure.
 *
 *  RETURN VALUE: a pointer to the array is returned
*/
#define UTHSZ  	(sizeof(utmp->ut_host))
char *host(struct utmp *utmp)
{
	
	static char uhost[UTHSZ + 4];

	if (*utmp->ut_host != 0) {
		strcpy(uhost," (");
		strcat(uhost,utmp->ut_host);
		strcat(uhost,")");
		return(uhost);
	} 
	else
		return("");
}

/*
 *  NAME:  line
 *
 *  FUNCTION:  	return a space padded buffer of the tty line
 *		name in a utmp structure.
 *	      
 *  RETURN VALUE: a pointer to the array is returned.
 */

char *line(struct utmp *utmp)
{
	static char uline[UTLSZ+1];
	static int  lwidth = UTLSZ;

	if (UTLSZ > MAXUTLSZ) 
		lwidth = MAXUTLSZ;

	copypad(&uline[0],&utmp->ut_line[0],lwidth);
	return(&uline[0]);
}

/*
 *  NAME:  copypad
 *
 *  FUNCTION:  	Basically do a strcpy and pad the end with spaces.
 *	      
 *  RETURN VALUE: void
 */

static void copypad(char *to, char *from, int size)
{
	register int i;
	register char *ptr;
	short printable;
	int halfway;

	size--; /* Leave room for null */

/* Scan for something textual in the field.  If there is */
/* nothing except spaces, tabs, and nulls, then substitute */
/* '-' for the contents. */

	for(printable = FALSE,i=0,ptr = from;
	    *ptr != '\0' && i < size;i++,ptr++) {
/* Break out if a printable character found.  Note that the test */
/* for "> ' '" eliminates all control characters and spaces from */
/* consideration as printing characters. */

		if(NCisgraph (NCdechr (ptr))) {
			printable = TRUE;
			break;
		}
		else if (NCisshift(*ptr)) ptr++;
	}
	i = 0;
	halfway = 0;
	if(printable) {
		for(; *from != '\0' && i < size;i++) *to++ = *from++ ;
	} else {
		halfway = (size+1)/2 - 1;       /* Where to put "-" */
	}

/* Add pad at end of string consisting of spaces and a '\0'. */
/* Put an asterisk at the halfway position.  (This only happens */
/* when padding out a null field.) */

	for(; i < size;i++) {
		*to++ = (i == halfway ? '.' : ' ');
	}
	*to = '\0';     /* Add null at end. */
}


/*
 *  NAME:  id
 *
 *  FUNCTION:  return the uid string of the utmp structure.
 *
 */

char *id(struct utmp *utmp)
{
	static char uid[sizeof(utmp->ut_id)+1];
	register char *ptr;
	register int i;

	for(ptr= &uid[0],i=0; i < sizeof(utmp->ut_id);i++) {
		if(isprint((int)utmp->ut_id[i]) || utmp->ut_id[i] == '\0') {
			*ptr++ = utmp->ut_id[i];
		} else {
			*ptr++ = '^';
			*ptr = (utmp->ut_id[i] & 0x17) | 0100;
		}
	}
	*ptr = '\0';
	return(&uid[0]);
}


/*
 *  NAME:  etime
 *
 *  FUNCTION:  	Figure out the elapsed time since last user activity
 *		on a users tty line.
 *	      
 *  RETURN VALUE:	Return a string pointer to this information.
 */

char *etime(struct utmp *utmp)
{
	struct tm *timestr;
	static char eetime[27];
	long lastactivity;
	char device[UTLSZ+5];
	struct stat statbuf;
	extern int shortflag;

	timestr = localtime (&utmp->ut_time);
	/* this code assumes strftime returns null terminated string */
	/* and that the width and precision are for display purposes  */
	strftime (&eetime[0], 15, "%6.6sD %5.5sT",timestr);

	if(shortflag < 0
	   && (utmp->ut_type == INIT_PROCESS
	   || utmp->ut_type == LOGIN_PROCESS
	   || utmp->ut_type == USER_PROCESS
	   || utmp->ut_type == DEAD_PROCESS)) {
		sprintf(&device[0],"/dev/%s",&utmp->ut_line[0]);

/* If the device can't be accessed, add a hyphen at the end. */

		if(stat(&device[0],&statbuf) == FAILURE) {
			strcat(&eetime[0],"   .  ");
		} else {
/* Compute the amount of time since the last character was sent to */
/* the device.  If it is older than a day, just exclaim, otherwise */
/* if it is less than a minute, put in an '-', otherwise put in */
/* the hours and the minutes. */

			lastactivity = time((time_t *)NULL) - statbuf.st_mtime;
			if(lastactivity > 24L*3600L) {
				strcat(&eetime[0],"  old ");
			} else if(lastactivity < 60L) {
				strcat(&eetime[0],"   .  ");
			} else {
				sprintf(&eetime[12]," %2u:%2.2u",
					(unsigned)(lastactivity/3600),
					(unsigned)((lastactivity/60)%60));
			}
		}
	}
	return(&eetime[0]);
}

static int badsys;      /* Set by alarmclk() if open or close times out. */


/*
 *  NAME:  mode
 *
 *  FUNCTION:  
 * 		Check "access" for writing to the line.  To avoid
 *		getting hung, set up any alarm around the open.  If
 * 		the alarm goes off, print a question mark.
 *	      
 *  RETURN VALUE: 	"+" 	writable
 *			"-"	no write permission
 *			"?"	unknown
 */

char *mode(struct utmp *utmp)
{
	char device[UTLSZ+5];
	int fd;
	struct stat statbuf;
	register char *answer;
	extern ttystatus;

	if(ttystatus == FALSE
	   || utmp->ut_type == RUN_LVL
	   || utmp->ut_type == BOOT_TIME
	   || utmp->ut_type == OLD_TIME
	   || utmp->ut_type == NEW_TIME
	   || utmp->ut_type == ACCOUNTING
	   || utmp->ut_type == DEAD_PROCESS) return(" ");

	sprintf(&device[0],"/dev/%s",&utmp->ut_line[0]);


	badsys = FALSE;
	(void) signal(SIGALRM,(void (*)(int))alarmclk);
	alarm((unsigned int)3);
#ifdef  CBUNIX
	fd = open(&device[0],O_WRONLY);
#else
	fd = open(&device[0],O_WRONLY|O_NDELAY);
#endif
	alarm((unsigned int)0);
	if(badsys) return("?");

	if(fd == FAILURE) {
/* If our effective id is "root", then send back "x", since it */
/* must be exclusive use. */

		if(geteuid() == 0) return("x");
		else return("-");
	}

/* If we are effectively root or this is a login we own then we */
/* will have been able to open this line except when the exclusive */
/* use bit is set.  In these cases, we want to report the state as */
/* other people will experience it. */

	if(geteuid() == 0 || strncmp(&username[0],&utmp->ut_user[0],
	   (size_t)SIZEUSER) == 0) {
		if(fstat(fd,&statbuf) == FAILURE) answer = "-";
		else answer = "+";
	} else answer = "+";

/* To avoid getting hung set up any alarm around the close.  If */
/* the alarm goes off, print a question mark. */

	badsys = FALSE;
	(void) signal(SIGALRM,(void (*)(int))alarmclk);
	alarm((unsigned int)3);
	close(fd);
	alarm((unsigned int)0);
	if(badsys) answer = "?";
	return(answer);
}

alarmclk(void)
{
/* Set flag saying that "close" timed out. */

	badsys = TRUE;
}
