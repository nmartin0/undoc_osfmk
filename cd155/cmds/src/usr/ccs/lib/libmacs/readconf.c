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
 * File:	readconf.c
 *
 * Abstract:	This file contains readconf(), which reads in the configuration
 *		file for the MACS software.
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "%W% %H%";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "conf.h"
#include "filename.h"

#define BUFFERSIZE (132)

#ifdef DEBUG 
extern int		_debug_;
#endif

/*===========================================================================*
 * Function:    readconf
 *
 * Abstract:	This function reads in the configuration file for the MACS
 *
 * Arguments:	None
 *
 * Return value:
 *		>NULL	valid structure pointer of the configuration data
 *		NULL	error
 *
 * Notes:
 *===========================================================================*/
struct macsconf *readconf()
{
   FILE			*fp;
   char			buffer[BUFFERSIZE];
   char 		*token;
   struct macsconf	*conf;

   /*
    * open the default conf file
    */
   if ((fp = fopen(MACS_CONF_FILE, "r")) == NULL) {
     errno = ENOENT;
     return(NULL);
   }

   /*
    * Get memory for the macsconf structure and initialize the entries
    */
   conf = (struct macsconf *)malloc(sizeof(struct macsconf));
   if (conf == NULL) {
     fclose(fp);
     errno = ENOMEM;
     return(NULL);
   }

   conf->total_nodes	= 32;
   conf->mailer		= "/bin/mail";
   conf->admin		= "root";
   conf->switchlog	= LOGDAILY;
   conf->operator	= "root";
   conf->macdmode	= ACCTONLY;
   conf->enforce	= NOKILL;
   conf->def_qrate	= 0;
   conf->uunt_rate	= 0;
   conf->idle_rate	= 0;
   conf->sync_interval	= DEFAULT_SYNC_INTERVAL;

   /*
    * Get data from the input file until we are finished
    */
  while (fgets(buffer, BUFFERSIZE, fp) != NULL) {

    if (buffer[0] == '#')
      continue;

    if ((token = strtok(buffer, ": \t\n")) == NULL)
      continue;

    if (!strcmp(token, "NODES")) {
      if ((token = strtok((char *)NULL, " \n")) != NULL)
          conf->total_nodes = atoi(token);

    } else if (!strcmp(token, "MAILER")) {
      if ((token = strtok((char *)NULL, "\n")) == NULL)
        conf->mailer = NULL;
      else
        conf->mailer = strdup(token);
  
    } else if (!strcmp(token, "ADMIN")) {
      if ((token = strtok((char *)NULL, "\n")) != NULL)
        conf->admin = strdup(token);

    } else if (!strcmp(token, "SWITCHLOG")) {
      if ((token = strtok((char *)NULL, "\n")) != NULL) {
        if (!strcmp(token, "weekly"))
          conf->switchlog = LOGWEEKLY;
        else if (!strcmp(token, "monthly"))
          conf->switchlog = LOGMONTHLY;
      }

    } else if (!strcmp(token, "OPERATOR")) {
      if ((token = strtok((char *)NULL, " \n")) != NULL)
        conf->operator = strdup(token);

    } else if (!strcmp(token, "MACDMODE")) {
      if ((token = strtok((char *)NULL, " \n")) != NULL)
        if (!strcmp(token, "macwatch"))
          conf->macdmode = MACWATCH;

    } else if (!strcmp(token, "ENFORCE")) {
      while ((token = strtok((char *)NULL, ": \t,\n")) != NULL) {
        if (!strcmp(token, "acctkill"))
          conf->enforce |= ACCTKILL;
        else if (!strcmp(token, "userkill"))
          conf->enforce |= USERKILL;
	else {
	  errno = EINVAL;
	  (void) free (conf);
          fclose(fp);
	  return (NULL);
	}
      }

    } else if (!strcmp(token, "DEF_QRATE")) {
      if ((token = strtok((char *)NULL, " \n")) != NULL)
        conf->def_qrate = atof (token);

    } else if (!strcmp(token, "UNDERUSE")) {
      if ((token = strtok((char *)NULL, " \n")) != NULL)
        conf->uunt_rate = atof (token);

    } else if (!strcmp(token, "IDLERATE")) {
      if ((token = strtok((char *)NULL, " \n")) != NULL)
        conf->idle_rate = atof (token);

    } else if (!strcmp(token, "SYNC_INTERVAL")) {
      if ((token = strtok((char *)NULL, " \n")) == NULL)
        conf->sync_interval = DEFAULT_SYNC_INTERVAL;
      else conf->sync_interval = atoi(token);
    }
  }
  fclose(fp);

#ifdef DEBUG
  if (_debug_) {
    printf("MAILER   : %s\n", conf->mailer);
    printf("ADMIN    : %s\n", conf->admin);
    printf("SWITCHLOG: %d\n", conf->switchlog);
    printf("OPERATOR : %s\n", conf->operator);
    printf("MACDMODE : %s\n", conf->macdmode);
    printf("ENFORCE  : %d\n", conf->enforce);
    printf("DEF_QRATE: %f\n", conf->def_qrate);
    printf("UNDERUSE : %f\n", conf->uunt_rate);
    printf("IDLERATE : %f\n", conf->idle_rate);
    printf("SYNC_INT : %d\n", conf->sync_interval);
  }
#endif

  /*
   * We got all the information we needed - now return with the macsconf 
   * strucutre
   */
   return(conf);
}

#ifdef DEBUG
/*===========================================================================*
 * Function:    main
 *
 * Abstract:    This function tests out the configuration function readconf().		
 *
 * Arguments:
 *
 * Return value:
 *
 * Notes:
 *===========================================================================*/
void main(argc, argv)
  int argc;
  char **argv;
{
  struct macsconf	*conf;
  static char		*log[]	= {"daily", "weekly", "monthly"};
  static char		*mode[]	= {"acctonly", "macwatch"};

  /*
   * Get the configuration information from the argument or the default file
   */

  conf = readconf();
  printf("MAILER   : %s\n", conf->mailer);
  printf("ADMIN    : %s\n", conf->admin);
  printf("SWITCHLOG: %s\n", log[conf->switchlog]);
  printf("OPERATOR : %s\n", conf->operator);
  printf("MACDMODE : %s\n", mode[conf->macdmode]);
  exit(0);
}
#endif
