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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: lp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:08:17 $";
#endif
/*
 * lp - send requests to the line printer spooler
 *
 * This interfaces just to lpr, the "official" spooling program.
 * Things to consider: 
 *    lpr copies fiels by default, lp links them by default
 *    lpr looks at the PRINTER environment variable, lp looks at LPDEST
 *    lpr requires options AND THEN files, lp allows them to be mixed
 */


#include <stdio.h>
#include <locale.h>
#include "printer_msg.h"

nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_PRINTER,n,s)

#define LPR "lpr"
#define LPRPATH "/usr/bin/lpr"

int	newArgc;		/* the  new arguments to LPR */
char	**newArgv;

main(argc, argv)
int argc;
char *argv[];
{
  char *malloc();
  void getOptions(),
       getFiles(), fatal();
  int  pid, status;

  (void) setlocale( LC_ALL, "" );
  catd = catopen(MF_PRINTER,0);
  newArgv = (char **)calloc(argc+4, sizeof(char *));
				/* the '4' is -j, -s, -P, NULL */
  if (newArgv == NULL)
     fatal(MSGSTR(LP_1, "Out of memory"));
  newArgv[newArgc++] = LPR;
  getOptions(argc, argv);	/* sort out the options for lpr */
  getFiles(argc, argv);		/* sort out the files and possibly stdin */

  newArgv[newArgc] = NULL;
  execv(LPRPATH, newArgv);
}

void getOptions(argc, argv)
int argc;
 char *argv[];
{

   void usage(), fatal();
   int  copyFlag = 0,
        destFlag = 0,
        jobFlag = 1;
   while (--argc)
   {
      char *thisArg = *++argv;
      if (thisArg[0] == '-' && thisArg[1] != '\0')
         switch (thisArg[1])
         {
         case 'c' : copyFlag = 1; /* make a copy, nothing to do */
                    break;
         case 'd' :		/* destination printer specified */
                    thisArg[1] = 'P'; /* patch option letter */
                    newArgv[newArgc++] = thisArg;
                    destFlag = 1;
                    break;
         case 'n' :		/* make a # of copies */
                    thisArg[1] = '#'; /* patch option letter */
                    newArgv[newArgc++] = thisArg;
                    break;
         case 'o' : break;	/* not supported */
         case 's' : jobFlag = 0; /* suppress job-number on stdout */
		    break;
         case 't' :		/* title for 1st page */
                    thisArg[1] = 'J'; /* patch option letter */
                    newArgv[newArgc++] = thisArg;
                    break;
         case 'm' :		/* send mail */
         case 'w' : newArgv[newArgc++] = "-m";
                    break;
         default  : usage();
         }
   }
   if (!copyFlag)		/* don't copy, so link */
      newArgv[newArgc++] = "-s";
   if (jobFlag)
       newArgv[newArgc++] = "-j"; /* make lpr print the job-number */
   if (!destFlag)		/* so look for LPDEST in env */
   {
      char *getenv(), *dest;
      if ((dest = getenv("LPDEST")) != NULL && strlen(dest))
      {
         char *malloc();
         newArgv[newArgc] = malloc(strlen(dest)+3);
         if (newArgv[newArgc] == NULL)
            fatal(MSGSTR(LP_1, "Out of memory"));
         newArgv[newArgc][0] = '-';
         newArgv[newArgc][1] = 'P';
         newArgv[newArgc][2] = '\0';
         strcat(newArgv[newArgc++], dest);
      }
   }
}

void getFiles(argc, argv)
int argc;
char *argv[];
{

   while (--argc)
   {
      char *thisArg = *++argv;
      if (thisArg[0] != '-')		/* ordinary filename */
         newArgv[newArgc++] = thisArg;
      else if (thisArg[1] == '\0')	/* a dash in the middle */
         newArgv[newArgc++] = thisArg;
   }
}


void usage()
{
  fprintf(stderr,
	  MSGSTR(LP_2, "Usage: lp [-c] [-ddest] [-m/w] [-nnumber] [-ttitle] [files]\n"));
  exit(1);
}

void fatal(msg)
char *msg;
{
   fprintf(stderr, MSGSTR(LP_3, "Fatal error: %s\n"));
   exit(1);
}
