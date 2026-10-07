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
 * HISTORY
 * $Log: core_dump.c,v $
 * Revision 1.2  1994/11/19  02:26:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)core_dump.c	1.2 (core_dump.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
#endif

/*
 *	panic.c
 */

#ifndef __CRAY__

#if OSF 
#include "sys_types_OSF.h"	/* in OSF sys_types_OSF.h */
#else
#include <sys/types.h>
#endif

#include <sys/timeb.h>
#include <fcntl.h>
#endif
#include <sys/stat.h>
#if defined(uts) || defined(__CRAY__) || defined(_IBMR2)
#include <time.h>
#else
#include <sys/time.h>
#endif
#include <signal.h>

/*
#include "int64.h"
#include "cap.h"
*/

#include "genglb.h"
#include "errglb.h"

char *		ctime();
struct tm *	localtime();
char *		strcpy();

#ifdef uts
long		time();
#define	TIME	time((long *) 0)
#else
time_t		time();
#define	TIME	time((time_t *)	0)
#endif

extern FILE *	DebugFile;
extern int      errno;
extern char *	sys_errlist[];            /* UNIX system error list */
extern struct   tabentry errtab[];         /* Server error table */

void
TimeStamp ( strg )
char *	strg;
{
    time_t	Clock;
    struct tm *	Tmp;

    Clock = TIME;
    Tmp	= localtime(&Clock);
    SPRINTF(strg,"%2d/%02d/%02d %2d:%02d:%02d",
	Tmp->tm_year,Tmp->tm_mon+1,Tmp->tm_mday,
	Tmp->tm_hour,Tmp->tm_min,Tmp->tm_sec);
    return;
}

void
TimeStamp2 ( strg1, strg2 )
char *	strg1;
char *	strg2;
{
    time_t		Clock;
    struct tm *		T1;
    static struct tm	T2;

#if !OSF
    struct timeb	T3;

    VOID ftime(&T3);
#endif

    Clock = TIME;
    T1	= localtime(&Clock);
    if ( T1->tm_year != T2.tm_year || T1->tm_mon  != T2.tm_mon  ||
	 T1->tm_mday != T2.tm_mday || T1->tm_hour != T2.tm_hour ||
	 T1->tm_min  != T2.tm_min				   ) {
	SPRINTF(strg1,"%2d/%02d/%02d %2d:%02d:00",
	    T1->tm_year,T1->tm_mon+1,T1->tm_mday,T1->tm_hour,T1->tm_min);
	T2 = *T1; }
    else
	*strg1 = '\0';
    SPRINTF(strg2,"%2d/%2d/%2d %02d:%02d:%02d.%03d",
        T1->tm_year,T1->tm_mon+1,T1->tm_mday,
#if OSF
	T1->tm_hour,T1->tm_min,T1->tm_sec,0);
#else
        T1->tm_hour,T1->tm_min,T1->tm_sec,T3.millitm);
#endif
    return;
}

/*
 *	CoreDump -	a routine to print the panic message, cause a dump, and
 *	terminate the process.	The macro PANIC	is defined in genglb.h.	 It is
 *	assumed	that there always exists a local variable Error	of which to
 *	report the value.
 *
 *	Be sure	to turn	off optimization when compiling	on the Cray.  Otherwise
 *	the divide by zero that	produces a dump	will be	declared a compile time
 *	error.
 */

void
CoreDump (	File, Line, Error )
char *	File;
int	Line;
int	Error;
{
    int                 fd;
    int			i, j, k;
    char		lfn[13];
    struct stat		statbuf;
    char                TheMsg[256];
    char		TheTime[20];
    struct tm *		tmp;

    /* Print out panic message and flush the buffer. */
    TimeStamp(TheTime);
    FPRINTF(DebugFile,"%s CoreDump in file %s at line %d, error %d\n",
            TheTime,File,Line,Error);
#ifndef __CRAY__
    fd = open("/dev/console",O_WRONLY);
    if ( fd >= 0 ) {
	SPRINTF(TheMsg,"%s CoreDump in file %s at line %d, error %d\n",
	        TheTime,File,Line,Error);
	(void) write(fd,TheMsg,(unsigned) strlen(TheMsg));
	(void) close(fd); }
#endif
    FPRINTF(DebugFile,"%s Dump taken\n",TheTime);
    VOID fflush(DebugFile);

    /* Set aside an old	core file, we're about to make a new one. */
    if ( stat("core",&statbuf) == 0 ) {
	tmp = localtime(&statbuf.st_atime);
	(void) strcpy(lfn,"core00000000");
	lfn[ 4]	+= (tmp->tm_mon	+ 1) / 10;
	lfn[ 5]	+= (tmp->tm_mon	+ 1) % 10;
	lfn[ 6]	+=  tmp->tm_mday     / 10;
	lfn[ 7]	+=  tmp->tm_mday     % 10;
	lfn[ 8]	+=  tmp->tm_hour     / 10;
	lfn[ 9]	+=  tmp->tm_hour     % 10;
	lfn[10]	+=  tmp->tm_min	     / 10;
	lfn[11]	+=  tmp->tm_min	     % 10;
	(void) link("core",lfn);
	(void) unlink("core"); }

    /* Cause a core dump and program termination. */
    i =	j = 0;
    k =	i / j;
    i =	k;

    /*
     *  If divide by zero failed go ahead and call abort
     */

    abort();

    /* We should never get here, but just in case ... */
    fprintf(DebugFile, "Unable to terminate process, bring down manually\n");
    fflush(DebugFile);
    exit(-1);
}

/*
 * LogMessage - This routine writes a message to the DebugFile and also
 * writes the current time.  The message string should not have \n at the
 * end.  This routine supplies the \n for every message.
 */

void
LogMessage ( Msg )
char *	Msg;
{
    char	TheTime1[20], TheTime2[20];

    /* Print out message and flush the buffer. */
    TimeStamp2(TheTime1,TheTime2);
/*
    if ( TheTime1[0] != '\0' )
	FPRINTF(DebugFile,"%s\n",TheTime1);
*/
    FPRINTF(DebugFile,"%s %s\n",TheTime2,Msg);
    VOID fflush(DebugFile);

    return;
}

/*
 *  LogError
 *
 *  This function is used to print an error message to the appropriate
 *  log file.  The current log file is given by the value of the file
 *  descriptor DebugFile.  The function uses the local error tables 
 *  "errtab.h" found in each of the servers, and the global apst error
 *  table.
 */


FILE *DebugFile;
char error_loc[]="gjk_error_loc";
/* OSF globals */

void LogError(table, errnum, dest, err1, err2, str)
struct tabentry *table;
int errnum;
unsigned short dest;	/* 1=log, 2=cons, 3=log&cons */
int err1;
int err2;
char *str;
{
    int 	index;
    int 	strnum;
    FILE 	*sys_console;
    int 	format_type=0;
    char 	logtime[20];
    void 	format_line();
/*    extern 	char *error_loc; */

    TimeStamp(logtime);   /* get time of this log */

    if( dest & CONS_OUTPUT ) {
      sys_console = fopen("/dev/console", "w");
      if ( sys_console == NULL ) {
	fprintf(DebugFile, "Could not open /dev/console for errors\n");
	dest ^= CONS_OUTPUT;
      }
    }

    if (dest & CONS_OUTPUT) {
      if (sys_console != NULL) {
	fprintf(sys_console, "\7");    /* Alert console operator */
	fprintf(sys_console, "*** %s ***\n", logtime);
      }
    }

    for (index=0; (table+index)->tabindex != ENDOFTABLE; index++) {
      if ((table+index)->tabindex == errnum) {
	for (strnum=0; (table+index)->errmsg[strnum] != NULL; strnum++) {
	  if (dest & LOG_OUTPUT)
	    FPRINTF(DebugFile, "%s", (table+index)->errmsg[strnum]);
	  if (dest & CONS_OUTPUT) {
	    if (sys_console != NULL)
	      fprintf(sys_console, "%s", (table+index)->errmsg[strnum]);
          }
        }
	if (dest & LOG_OUTPUT) {
	  format_type = (table+index)->fmttype;
	  format_line(format_type, err1, err2, str);
	}
	break;
      }
    }
    if ((table+index)->tabindex == ENDOFTABLE) {
      if (dest & CONS_OUTPUT) {
        if (sys_console != NULL)
	    fprintf(sys_console, "Error not found in table;  ");
      }
      if (dest & LOG_OUTPUT) {
	  FPRINTF(DebugFile,
	    "Error not found in table, errnum is %d\n", errnum);
      }
   }
   if (dest & CONS_OUTPUT) {
        if (sys_console != NULL) {
	  fprintf(sys_console, "Check log file (%s)\n",error_loc);
	  fclose(sys_console);
        }
   }
}

/*
 *    Formats and writes error variables to log file
 */

void format_line(format, err1, err2, str)
int format, err1, err2;
char *str;
{
    switch (format) {
      case  1 : FPRINTF(DebugFile, "Last system error was: %s;",
	                sys_errlist[errno]);
                FPRINTF(DebugFile, "     error: %d\n", err1);
		break;
      case  2 : FPRINTF(DebugFile, "error: %d;  terror: %d\n", err1, err2);
		break;
      case  3 : FPRINTF(DebugFile, "error: %d;  count: %d\n", err1, err2);
		break;
      case  4 : FPRINTF(DebugFile, "error: %d;  drive err du: 0x%x\n",
			err1, err2);
		break;
      case  5 : FPRINTF(DebugFile, "error: %d;  drive err dev: 0x%x\n",
			err1, err2);
		break;
      case  6 : FPRINTF(DebugFile, "System error: %s;  device: %s\n",
			sys_errlist[errno], str);
		break;
      case  7 : FPRINTF(DebugFile, "error: %d;  RcvAnyCount: %s\n",err1,str);
		break;
      case  8 : FPRINTF(DebugFile, "device: %d;  error: %d\n",err1,err2);
		break;
      case  9 : FPRINTF(DebugFile, "%d UPDATE:  recv error %d\n", err1, err2);
		break;
      case 10 : FPRINTF(DebugFile, "%d UPDATE:  fs error %d\n", err1, err2);
		break;
      case 11 : FPRINTF(DebugFile, "%d UPDATE:  tkn count %d\n", err1, err2);
		break;
      case 12 : FPRINTF(DebugFile, "%d\n", err1);
		break;
      case 13 : FPRINTF(DebugFile, "System error: %s	CapA: %s\n",
			sys_errlist[errno], str);
		break;
      case 14 : FPRINTF(DebugFile, "System error: %s\n", sys_errlist[errno]);
		break;
      case 15 : FPRINTF(DebugFile, "%s, error:  %d\n", str, err1);
		break;
      case 16 : FPRINTF(DebugFile, "error:  0x%04x\n", err1);
		break;
      case 17 : FPRINTF(DebugFile, "error:  0x%x\n", err1);
		break;
      case 18 : FPRINTF(DebugFile, "%d trying to lock dir %d\n", err1, err2);
		break;
      case 19 : FPRINTF(DebugFile, "%d trying to unlock dir %d\n", err1, err2);
		break;
      case 20 : FPRINTF(DebugFile, "Number of objects: %d, ", err1);
		FPRINTF(DebugFile, "Number of files: %d\n", err2);
		break;
      case 21 : FPRINTF(DebugFile, "ByteLeng: %d, buffersize: %d\n", err1,err2);
		break;
      case 22 : FPRINTF(DebugFile, "Can't open %s;  System error: %s\n",
			str, sys_errlist[errno]);
		break;
      case 24 : FPRINTF(DebugFile, "entries %d and %d\n",err1,err2);
		break;
      case 23 : FPRINTF(DebugFile, " %d; %d\n",err1,err2);
      default : FPRINTF(DebugFile,"\n");
		break;
    }
}

#ifndef __CRAY__

/*
 *	This module is called when a SIGUSR1 (signal number 16)	is
 *	received.  It shuts down the server without a core dump.
 */

int
IntSignal ( )
{

    /* Print out message regarding reciept of signal. */
    LogMessage("SIGUSR1 (16) received, process will exit");

    /* Exit. */
    EXIT(-2);
}

#endif
