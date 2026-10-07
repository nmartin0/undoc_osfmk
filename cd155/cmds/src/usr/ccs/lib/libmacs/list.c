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
 * File:        list.c
 *
 * Abstract:	This file contains the main routine for list MACD database
 *		information
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "@(#)list.c	2.9 10/15/92";
#endif

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <pwd.h>
#include <nx/nxacct.h>

#include "mac.h"
#include "macerr.h"
#include "ipc.h"

/* usage header format */
#define UHFMT "\n%9s %5s %5s %7s %7s %16s %16s %7s\n"

/* usage format for users account with allocation defined */
#define UDFMT "%9s %5d %5d %7s  %3d.%02d %10.0f:%02d:%02d %10.0f:%02d:%02d "

/* usage format for users account with unlimited allocation */
#define UUFMT "%9s %5d %5d %7s  %3d.%02d        unlimited %10.0f:%02d:%02d "

/* usage format for account with allocation defined */
#define ADFMT "%9s       %5d %7s         %10.0f:%02d:%02d %10.0f:%02d:%02d "

/* usage format for account with unlimited allocation */
#define AUFMT "%9s       %5d %7s                unlimited %10.0f:%02d:%02d "

#define SECONDS(X) ((int) fmod((double)(X), 60.0))
#define MINUTES(X) ((int) fmod((double)(X) / 60.0, 60.0))
#define HOURS(X)   ((int) ((double)(X)/3600.0))

/*===========================================================================*
 * Function:    mac_list
 *
 * Abstract:    This function request information from MACD and display
 *		them on stdout.  It assumes an open connection exists
 *		between it and the MACD.
 *
 * Arguments: 	pagid -	pointer to agid array
 *		puid - pointer to uid array
 *		naccts - number of accounts involved
 *		nusers - number of users involved
 *		cpuflg - real cpu reported instead of sbus
 *		remflg - remaining alloc reported instead of use
 *
 * Return value:
 *		0 	successful
 *		-1	error
 *
 * Notes:
 *===========================================================================*/
int mac_list (pagid, puid, naccts, nusers, cpuflg, remflg)
   int	*pagid;
   int	*puid;
   int	naccts;
   int	nusers;
   int	cpuflg;
   int 	remflg;
{
   int			i, j, *uptr, *aptr;
   int			tunit3, tunit2, tunit6, tunit5;
   double		tunit1, tunit4, dtime;

   char			access_flags[8];
   struct nxacct	*nxstuff;
   struct passwd	*pwent;

   struct cpu_ctrl data, *pdata = &data;

#ifdef DEBUG
   fprintf(stderr,
           "Enter mac_list (pagid=%d, puid=%d, naccts=%d, nusers=%d, cpuflg=%d, remflg=%d)\n",
           (int)pagid, (int)puid, naccts, nusers, cpuflg, remflg);

   if (pagid) {
      for (i = 0; i < naccts; i++) {
         fprintf (stderr, "account[%d]=%d\n", i, pagid[i]);
      }
   }

   if (puid) {
      for (i = 0; i < nusers; i++) {
         fprintf (stderr, "users[%d]=%d\n", i, puid[i]);
      }
   }
#endif

   if ( naccts != 1) {
      if (nusers > 1) {
         fprintf(stderr, "\n Incorrect usage of mac_list \n");
         return(-1);
      }
   }

   data.uid = getuid();

   /*
    * Output report column header
    */
   if (cpuflg != 1) {
      if (remflg != 1) {
         fprintf(stdout, UHFMT,
                 "name", "uid", "agid", "access", "percent",
                 "allocation",
                 "used",
                 "maxnode");
      } else {
         fprintf(stdout, UHFMT,
                 "name", "uid", "agid", "access", "percent",
                 "allocation",
                 "remaining",
                 "maxnode");
      }
   } else {
      fprintf(stdout, UHFMT,
              "name", "uid", "agid", "access", "percent",
              "time",
              "node-time",
              "maxnode");
   }

   /*
    * Get one record a time, and output one line of report per record
    */
   for (i = 0, aptr = pagid; i < naccts; i++, aptr++) {
      data.agid = *aptr;
      if ((nxstuff = nx_getaid(*aptr)) == NULL) {
         fprintf(stderr,"\n Invalid account id %d\n", *aptr);
         continue;
      }
      for (j=0, uptr=puid; j<nusers; j++, uptr++) {
         bzero(&(data.info), sizeof(struct cpu_lblk));
         data.info.id = *uptr;
         if (( pwent = getpwuid (*uptr)) == NULL) {
            fprintf(stderr,"\n Invalid uid %d \n", *uptr);
            continue;
         }

         if ( cpuctl (C_GETLIMIT, pdata) < 0) {
#ifdef DEBUG
            macerr();
            fprintf(stderr, "for account %d(%s) user %d(%s)\n",
                    *aptr, an, *uptr, pwent->pw_name);
#endif
         } else {
            access_flags[0] = (pdata->info.modify)   ? 'M' : ' ';
            access_flags[1] = (pdata->info.transfer) ? 'T' : ' ';
            access_flags[2] = (pdata->info.use)      ? 'U' : ' ';
            access_flags[3] = (pdata->info.inhibit)  ? 'I' : ' ';
            access_flags[4] = ' ';
            access_flags[5] = '\0';

            if (cpuflg != 1) {
               dtime = pdata->info.authorized;
               tunit3 = SECONDS(dtime);
               tunit2 = MINUTES(dtime);
               tunit1 = HOURS(dtime);

               if (remflg != 1) {
                  dtime = pdata->info.sbu_time;
                  tunit6 = SECONDS(dtime);
                  tunit5 = MINUTES(dtime);
                  tunit4 = HOURS(dtime);

               } else {
                  dtime = (pdata->info.authorized - pdata->info.sbu_time);
                  tunit4 = HOURS(dtime);
                  if (dtime < 0.0) dtime = -dtime;
                  tunit5 = MINUTES(dtime);
                  tunit6 = SECONDS(dtime);
               }

            } else {
               dtime = pdata->info.used_time;
               tunit1 = HOURS(dtime);
               tunit2 = MINUTES(dtime);
               tunit3 = SECONDS(dtime);
               dtime = pdata->info.sbu_time;
               tunit4 = HOURS(dtime);
               tunit5 = MINUTES(dtime);
               tunit6 = SECONDS(dtime);
            }

            if (pdata->info.unlimit) {
               fprintf(stdout, UUFMT,
                       pwent->pw_name,
                       *uptr,
                       *aptr,
                       access_flags,
                       pdata->info.percent/100,
                       pdata->info.percent%100,
                       tunit4,
                       tunit5,
                       tunit6);
            } else {
               fprintf(stdout, UDFMT,
                       pwent->pw_name,
                       *uptr,
                       *aptr,
                       access_flags,
                       pdata->info.percent/100,
                       pdata->info.percent%100,
                       tunit1,
                       tunit2,
                       tunit3,
                       tunit4,
                       tunit5,
                       tunit6);
            }
	    if (pdata->info.maxnodes == -1)
               fprintf(stdout, "sys.lim\n");
            else fprintf(stdout, "%7d\n", pdata->info.maxnodes);
         }
      }

      /* Get account information after individual user information */
      data.agid = *aptr;
      bzero (&(data.info), sizeof(struct cpu_lblk));
      data.info.id = *aptr;
      if ((nxstuff = nx_getaid(*aptr)) == NULL) {
         fprintf(stderr,"\n Invalid account id %d\n", *aptr);
         continue;
      }

      if (cpuctl (C_GETACCT, pdata) < 0) {
         macerr();
         fprintf(stderr, "for account %d(%s)\n", *aptr, nxstuff->acct_name);

      } else {
         access_flags[0] = (!pdata->info.weight)  ? ' ' : ' ';
         access_flags[1] = (!pdata->info.weight)  ? ' ' : 'W';
         access_flags[2] = ' ';
         access_flags[3] = ' ';
         access_flags[4] = (pdata->info.killjobs) ? ' ' : 'N';
         access_flags[5] = (pdata->info.lockjobs) ? 'L' : ' ';
         access_flags[6] = (pdata->info.inhibit)  ? 'I' : ' ';
         access_flags[7] = '\0';
         if (cpuflg != 1) {
            dtime = pdata->info.authorized;
            tunit3 = SECONDS(dtime);
            tunit2 = MINUTES(dtime);
            tunit1 = HOURS(dtime);
            if (remflg != 1) {
               dtime = pdata->info.sbu_time;
               tunit6 = SECONDS(dtime);
               tunit5 = MINUTES(dtime);
               tunit4 = HOURS(dtime);

            } else {
               dtime = (pdata->info.authorized - pdata->info.sbu_time);
               tunit4 = HOURS(dtime);
               if (dtime < 0.0) dtime = -dtime;
               tunit5 = MINUTES(dtime);
               tunit6 = SECONDS(dtime);
            }

         } else {
            dtime = pdata->info.used_time;
            tunit1 = HOURS(dtime);
            tunit2 = MINUTES(dtime);
            tunit3 = SECONDS(dtime);
            dtime = pdata->info.sbu_time;
            tunit4 = HOURS(dtime);
            tunit5 = MINUTES(dtime);
            tunit6 = SECONDS(dtime);
         }

         if (pdata->info.unlimit) {
            fprintf(stdout, AUFMT,
                    nxstuff->acct_name,
                    *aptr,
                    access_flags,
                    tunit4,
                    tunit5,
                    tunit6);
         } else {
            fprintf(stdout, ADFMT,
                    nxstuff->acct_name,
                    *aptr,
                    access_flags,
                    tunit1,
                    tunit2,
                    tunit3,
                    tunit4,
                    tunit5,
                    tunit6);
         }
	 if (pdata->info.maxnodes == -1)
            fprintf(stdout, "sys.lim\n");
         else fprintf(stdout, "%7d\n", pdata->info.maxnodes);
      }
   }
   return (0);
}
