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
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * File:	si.c
 *
 * Abstract:	The si program (scheduled interrupt) is used to inform
 *		the (MACD) the beginning/end of a scheduled-interrupt
 * 		period.  The MACD will log the event in its current log file.
 * 		Super-user priviledge is required for the execution.
 *
 * % si [-on | -off] [-tTYPE] -cCOMMENTS
 * % si -boot | -shutdown
 *		beginning of the scheduled interrupt period
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "macd.h"
#include "mac.h"

/*
 * Some defines and stuff which are really important
 */

void
vUsage()
{

   (void) fprintf(stderr,
		  "Usage:  si [-on | -off] [-t type] -c comment\n");
   (void) fprintf(stderr,
		  "        si -boot | -shutdown \n");
}

/******************************************************************************
*
* main(argc, argv)
*
* Function main() parses the argument list and sends the arppropriate
* message to the MACD.
*
******************************************************************************/

int
main(argc, argv)
   int             argc;
   char           *argv[];
{

   struct si_info  data;
   char            c;
   int             i;
   extern char    *optarg;
   int             tFlagOnlyOnce = 0, oFlagOnlyOnce = 0, bFlagOnlyOnce = 0;
   int             sFlagOnlyOnce = 0, cFlagOnlyOnce = 0;

   data.flag = 0;
   data.comment[0] = '\0';

   /* set default type to "unknown" */
   data.type = MAX_ITYPE - 1;
   for (i = 0; i < MAX_ITYPE; i++)
      if (strncmp(UNKNOWN_STRING, stype[i], strlen(UNKNOWN_STRING)) == 0) {
	 data.type = i;
	 break;
      }
   /*-------------------------------------------------------------------------*
    * Parse command line args
    *-------------------------------------------------------------------------*/


   while ((c = getopt(argc, argv, "o:t:c:b:s:")) != -1) {
      switch (c) {
      case 't':
	 if (tFlagOnlyOnce == 0) {
	    tFlagOnlyOnce = 1;
	    for (i = 0; i < MAX_ITYPE; i++)
	       if (strncmp(optarg, stype[i], 5) == 0) {
		  data.type = i;
		  break;
	       }
	    if (i >= MAX_ITYPE) {
	       vUsage();
	       (void) fprintf(stderr, "Undefined type %s\n", optarg);
	       (void) fprintf(stderr, "Defined types:\n");
	       for (i = 0; i < MAX_ITYPE; i++)
		  (void) fprintf(stderr, "%s\n", stype[i]);
	       exit(-1);
	    }
	 } else {
	    (void) fprintf(stderr, "Only one switch -t type is allowed\n");
	    vUsage();
	    exit(-1);
	 }
	 break;

      case 'o':
	 if (oFlagOnlyOnce == 0) {
	    sFlagOnlyOnce = 1;
	    bFlagOnlyOnce = 1;
	    oFlagOnlyOnce = 1;
	    switch (optarg[0]) {
	    case 'n':
	       data.flag = 1;
	       break;
	    case 'f':
	       data.flag = 2;
	       break;
	    default:
	       vUsage();
	       exit(-1);
	    }
	 } else {
	    (void) fprintf(stderr, "Only one switch [-on | off] is allowed\n");
	    vUsage();
	    exit(-1);
	 }
	 break;

      case 'b':
	 if (bFlagOnlyOnce == 0) {
	    sFlagOnlyOnce = 1;
	    bFlagOnlyOnce = 1;
	    oFlagOnlyOnce = 1;
	    if (0 == strcmp(optarg, "oot")) {
	       /* C_BOOT flag, just rebooted */
	       data.flag = 3;
	       strcpy(data.comment, "Not used for boot swicth");
	    } else {
	       vUsage();
	       exit(-1);
	    }
	 } else {
	    (void) fprintf(stderr, "Only one switch -boot is allowed\n");
	    vUsage();
	    exit(-1);
	 }
	 break;

      case 's':
	 if (sFlagOnlyOnce == 0) {
	    sFlagOnlyOnce = 1;
	    bFlagOnlyOnce = 1;
	    oFlagOnlyOnce = 1;
	    if (0 == strcmp(optarg, "hutdown")) {
	       /* C_OFF flag, just shutting down */
	       data.flag = 4;
	       strcpy(data.comment, "Not used for shutdown swicth");
	    } else {
	       vUsage();
	       exit(-1);
	    }
	 } else {
	    (void) fprintf(stderr, "Only one switch -shutdown is allowed\n");
	    vUsage();
	    exit(-1);
	 }
	 break;

      case 'c':
	 if (cFlagOnlyOnce == 0) {
	    cFlagOnlyOnce = 1;
	    (void) strncpy(data.comment, optarg, 63);
	    data.comment[63] = '\0';
	 } else {
	    (void) fprintf(stderr, "Only one switch -c comment is allowed\n");
	    vUsage();
	    exit(-1);
	 }
	 break;

      default:
	 vUsage();
	 exit(-1);
      }
   }
   if (strlen(data.comment) <= 0) {
      vUsage();
      exit(-1);
   }
   /*
    * Open a connection to MACD and send a message
    */
   if (c_open() < 0) {
      fprintf(stderr, "ERROR:  Unable to connect to MACD\n");
      exit(-1);
   }
   switch (data.flag) {
   case 0:
   case 1:
   case 2:
      if (cpuctl(SCHED_DOWN, &data) < 0) {
	 (void) c_close();
	 fprintf(stderr, "ERROR:  Unable to send message to MACD\n");
	 exit(-1);
      }
      break;
   case 3:			/* flag, just rebooted */
      data.flag = 0;		/* we don't want to send any data field */
      if (cpuctl(C_BOOT, &data) < 0) {
	 (void) c_close();
	 fprintf(stderr, "ERROR:  Unable to send message to MACD\n");
	 exit(-1);
      }
      data.flag = 3;		/* but we will need data.flag for the next
				 * switch */
      break;
   case 4:			/* C_OFF flag, just shutting down */
      data.flag = 0;		/* we don't want to send any data field */
      if (cpuctl(C_OFF, &data) < 0) {
	 (void) c_close();
	 fprintf(stderr, "ERROR:  Unable to send message to MACD\n");
	 exit(-1);
      }
      data.flag = 4;		/* but we will need data.flag for the next
				 * switch */
      break;
   default:
      vUsage();
      exit(-1);
   }

   switch (data.flag) {
   case SI_NONE:
      (void) printf("Sending comment only to MACS\n");
      break;
   case SI_START:
      (void) printf("Scheduled-Interrupt period begin\n");
      break;
   case SI_END:
      (void) printf("Scheduled-Interrupt period end\n");
      break;
   case 3:			/* C_BOOT flag, just rebooted */
      (void) printf("Reboot message for MACS sent.\n");
      break;
   case 4:			/* C_OFF flag, just shutting down */
      (void) printf("Shutdown message for MACS sent.\n");
      break;

   default:
      vUsage();
      exit(-1);
   }
   (void) c_close();
   return (0);
}
