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
 
#include <stdio.h>
#include <string.h>
#include <macs/actable.h>
#include <macs/filename.h>

#define BUFFERSIZE 132

/*===========================================================================*
 * Function:    get_actab()
 *
 * Abstract:    This function reads in the configuration file for the
 *              accounting.
 *
 * Arguments:   None
 *
 * Return value:
 *              >NULL   valid structure pointer of the configuration data
 *              NULL    error
 *
 * Notes:
 *===========================================================================*/
struct actable *get_actab ()
{
   FILE *fp;
   int i, *bins, *actlevel;
   struct actable *conf;
   char *token, buffer[BUFFERSIZE], *ptr;
   extern char *malloc();

#ifdef DEBUG
(void) fprintf (stderr, "Enter get_actab(%s)\n", MACS_ACTABLE_FILE);
#endif

   /*
    * open the actable file
    */
   if ((fp = fopen(MACS_ACTABLE_FILE, "r")) == NULL) {
     return(NULL);
   }

#ifdef DEBUG
(void) fprintf (stderr, "Fopen (%s) successful\n", MACS_ACTABLE_FILE);
#endif
 
   /*
    * Get memory for the actable structure and initialize the entries
    */
   conf = (struct actable *)malloc(sizeof(struct actable));
   if (conf == NULL) {
     fclose(fp);
     return(NULL);
   }

#ifdef DEBUG
(void) fprintf (stderr, "Allocate struct actable successful\n");
#endif

   conf->maxnodes = 0;
   conf->n_bins = 0;
   conf->bins = NULL;
   conf->n_actlevel = 0;
   conf->actlevel = NULL;
   conf->n_batchq = 0;
   conf->batchq = NULL;
   conf->n_dedicateq = 0;
   conf->dedicateq = NULL;

  while (fgets(buffer, BUFFERSIZE, fp) != NULL) {
    if (buffer[0] == '#')
      continue;
    if ((token = strtok(buffer, ": \t\n")) == NULL)
      continue;
    if (!strcmp(token, "N_BINS")) {
      if ((token = strtok((char *)NULL, "\n")) != NULL)
        conf->n_bins = atoi(token);
      if (conf->n_bins > 0) 
	conf->bins = (int *)malloc(sizeof(int) * conf->n_bins);
    } else if (!strcmp(token, "BINS")) {
      if (conf->n_bins > 0) 
	for (ptr=buffer+strlen(token)+1,i=0; i<conf->n_bins; 
		ptr+=strlen(token)+1,i++) {
	  if ((token = strtok(ptr, ": \t\n")) == NULL) break;
	  conf->bins[i] = atoi(token);
	}
      continue;
    }
    else if (!strcmp(token, "ACT_GRP_LEVEL")) {
      if ((token = strtok((char *)NULL, "\n")) != NULL)
        conf->n_actlevel = atoi(token);
      if (conf->n_actlevel > 0) {
	conf->actlevel = (int *)malloc(sizeof(int) * conf->n_actlevel);
	conf->actlabel = (char **)malloc(sizeof(char *) * conf->n_actlevel);
      }
    }
    else if (!strcmp(token, "ACT_GRP_LABEL")) {
      if (conf->n_actlevel > 0) 
	for (ptr=buffer+strlen(token)+1,i=0; i<conf->n_actlevel; 
		ptr+=strlen(token)+1,i++) {
	  if ((token = strtok(ptr, ": \t\n")) == NULL) break;
	  conf->actlabel[i] = strdup(token);
	}
      continue;
    }
    else if (!strcmp(token, "ACT_NAME_SIM")) {
      if (conf->n_actlevel > 0) 
	for (ptr=buffer+strlen(token)+1,i=0; i<conf->n_actlevel; 
		ptr+=strlen(token)+1,i++) {
	  if ((token = strtok(ptr, ": \t\n")) == NULL) break;
	  conf->actlevel[i] = atoi(token);
	}
      continue;
    }
    else if (!strcmp(token, "N_BATCH_QUEUE")) {
      if ((token = strtok((char *)NULL, "\n")) != NULL)
        conf->n_batchq = atoi(token);
      if (conf->n_batchq > 0) 
	conf->batchq = (char **)malloc(sizeof(char *) * conf->n_batchq);
    } else if (!strcmp(token, "BATCH_QUEUE")) {
      if (conf->n_batchq > 0) 
	for (ptr=buffer+strlen(token)+1,i=0; i<conf->n_batchq; 
		ptr+=strlen(token)+1,i++) {
	  if ((token = strtok(ptr, ": \t\n")) == NULL) break;
	  conf->batchq[i] = strdup(token);
	}
      continue;
    }
    else if (!strcmp(token, "N_DEDICATE_QUEUE")) {
      if ((token = strtok((char *)NULL, "\n")) != NULL)
        conf->n_dedicateq = atoi(token);
      if (conf->n_dedicateq > 0) 
	conf->dedicateq = (char **)malloc(sizeof(char *) * conf->n_dedicateq);
    } else if (!strcmp(token, "DEDICATE_QUEUE")) {
      if (conf->n_dedicateq > 0) 
	for (ptr=buffer+strlen(token)+1,i=0; i<conf->n_dedicateq; 
		ptr+=strlen(token)+1,i++) {
	  if ((token = strtok(ptr, ": \t\n")) == NULL) break;
	  conf->dedicateq[i] = strdup(token);
	}
      continue;
    }
  }

  (void) fclose(fp);

#ifdef DEBUG
/* dump struct actable */
(void) fprintf (stderr, "conf->n_bins = %d\n", conf->n_bins);
if (conf->n_bins > 0) {
	(void) fprintf (stderr, "conf->bins =");
	for (i=0; i<conf->n_bins; i++)
		(void) fprintf (stderr, " %d", conf->bins[i]);
	(void) fprintf (stderr, "\n");
}
(void) fprintf (stderr, "conf->n_actlevel = %d\n", conf->n_actlevel);
if (conf->n_actlevel > 0) {
	(void) fprintf (stderr, "conf->actlevel =");
	for (i=0; i<conf->n_actlevel; i++)
		(void) fprintf (stderr, " %d", conf->actlevel[i]);
	(void) fprintf (stderr, "\n");
}
(void) fprintf (stderr, "conf->n_batchq = %d\n", conf->n_batchq);
if (conf->n_batchq > 0) {
	(void) fprintf (stderr, "conf->batchq =");
	for (i=0; i<conf->n_batchq; i++)
		(void) fprintf (stderr, " %s", conf->batchq[i]);
	(void) fprintf (stderr, "\n");
}
(void) fprintf (stderr, "conf->n_dedicateq = %d\n", conf->n_dedicateq);
if (conf->n_batchq > 0) {
	(void) fprintf (stderr, "conf->dedicateq =");
	for (i=0; i<conf->n_dedicateq; i++)
		(void) fprintf (stderr, " %s", conf->dedicateq[i]);
	(void) fprintf (stderr, "\n");
}
#endif

  return (conf);
}
