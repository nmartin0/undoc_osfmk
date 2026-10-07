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
static char rcsid[] = "@(#)$RCSfile: cancel.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:08:09 $";
#endif
/*
 * cancel - cancel requests made to the line printer spooler
 *
 * This interfaces just to lprm, the "official" program.
 */

#include <stdio.h>
#include <ctype.h>
#include <locale.h>
#include "printer_msg.h"
nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_PRINTER,n,s)

#include <NLchar.h>
#include <NLctype.h>

#define LPRM "lprm"
#define LPRMPATH "/usr/bin/lprm"

int	newArgc;		/* the new arguments to LPR */
char	**newArgv;

main(argc, argv)
int argc;
char *argv;
{
  char *malloc();
  void getOptions(), fatal();

  (void) setlocale( LC_ALL, "" );
  catd = catopen(MF_PRINTER,0);
  newArgv = (char **)calloc(argc+1, sizeof(char *));
  if (newArgv == NULL)
     fatal(MSGSTR(CANCEL_1, "out of memory"));
  newArgv[newArgc++] = LPRM;
  getOptions(argc, argv);

  newArgv[newArgc] = NULL;
  execv(LPRMPATH, newArgv);	/* Never should get beyond that call */
  fprintf(stderr, MSGSTR(CANCEL_2, "Can't exec %s\n"), LPRMPATH);
  return (1);			/* That is a exit(1) */
}

void getOptions(argc, argv)
int argc;
char *argv[];
{

   void fatal();
   while (--argc)
   {
      int noNumber = 0;
      char *thisArg = *++argv;
      while (*thisArg)
         if (!isdigit(*thisArg++))
         {
            noNumber = 1;
            break;
         }
      if (noNumber)		/* assume it was a printer name */
      {
        char *malloc(), *strcat();
        char *arg = malloc(strlen(*argv)+3);
        if (arg == NULL)
           fatal(MSGSTR(CANCEL_1, "out of memory"));
        arg[0] = '-';
        arg[1] = 'P';
        arg[2] = '\0';
        newArgv[newArgc++] = strcat(arg, *argv);
      }
      else
        newArgv[newArgc++] = *argv;
   }
}


void fatal(msg)
char *msg;
{
   fprintf(stderr, MSGSTR(CANCEL_3, "Fatal error: %s\n"));
   exit(1);
}

