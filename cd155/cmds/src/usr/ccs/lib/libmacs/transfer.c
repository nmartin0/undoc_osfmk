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
 *		Copyright (c) 1990 San Diego Supercomputer Center.
 *		All rights reserved.  The SDSC software License Agreement
 *		specifies the terms and conditions for redistribution.
 *
 * File:        transfer.c
 *
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "@(#)transfer.c	2.7 10/15/92";
#endif

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <nx/nxacct.h>

#include "mac.h"
#include "macerr.h"
#include "ipc.h"

#define PCNT 		(10000)

/*===========================================================================*
 * Function:    mac_transfer
 *
 * Abstract:    This function transfers CPU node-time or percentage of CPU
 *		node-time between accounts by sending an appropriate
 *		request message to MACD.  It assumes an open-connection to
 *		MACD.
 *
 * Arguments:
 *	agid_from -	account id from which some allocation is to be
 *			transferred out
 *	agid_to -	account id in which some allocation is to be
 *                      transferred in
 *	amount - CPU allocation in node-seconds or percentage*100
 *	percent_flag -	indicate the amount is percentage (1) or not (0)
 *
 * Return value: 0	successful
 *		-1	failure, errno is also set
 *
 * Notes:
 *===========================================================================*/
int mac_transfer(agid_from, agid_to, amount, percent_flag)
   int		agid_from;
   int		agid_to;
   double	amount;
   int		percent_flag;
{
   struct cpu_ctrl	user_data;
   struct cpu_ctrl	acct_from;
   struct cpu_ctrl	acct_to;
   double		trnsf_amt;
   double		remain;

#ifdef DEBUG
   fprintf(stderr,
      "Enter mac_transfer(agid_from=%d, agid_to=%d, amount=%f, percent_flag=%d)\n",
      agid_from, agid_to, amount, percent_flag);
#endif

   /*
    * Fill in information for GETLIMIT request
    */
   bzero((char*)&user_data, sizeof(user_data));
   user_data.uid	= getuid();
   user_data.agid	= agid_from;
   user_data.info.id	= user_data.uid;

   /* 
    * If not a super-user, get and check user's modify privileges for account
    */
   if (user_data.uid !=0 ) {
      /*
       * Get the potential pi's entry from MACD
       */
      if (cpuctl(C_GETLIMIT, &user_data) < 0) {
         macerr();
         return(-1);
      }

      /*
       * If the user's cpulimit entry doesn't have the transfer bit set
       * ding them 
       */
      if (user_data.info.transfer == 0) {
      errno = EACCES;
         macerr();
         return(-1);
      }
   }

   /*
    * Fill in information for GETACCT request on both accounts
    */
   bzero((char*)&acct_to, sizeof(acct_to));
   bzero((char*)&acct_from, sizeof(acct_from));
   acct_from.uid	= user_data.uid;
   acct_to.uid		= user_data.uid;
   acct_from.agid	= user_data.agid; 
   acct_to.agid		= agid_to; 
   acct_to.info.id	= agid_to;
   acct_from.info.id	= agid_from;

   /*
    * Get the transfer-destination account entry
    */
   if (cpuctl(C_GETACCT, &acct_to) < 0) {
      macerr();
      return(-1);
   }

   /*
    * Get the transfer-source account entry
    */
   if (cpuctl(C_GETACCT, &acct_from) < 0) {
      macerr();
      return(-1);
   }
   
   /*
    * Neither account should be unlimit account
    */
   if (acct_from.info.unlimit || acct_to.info.unlimit) {
      fprintf(stderr, "Cannot transfer to or from an unlimit account\n");
      return(-1);
   }
   
   /*
    * Convert percent input or time input into time in unit stored in records
    */
   if (percent_flag) { 
      if (amount<0.0 || amount>(double)PCNT) {
         fprintf(stderr,"Invalid percent value for transfer\n");
         return(-1);
      }
      trnsf_amt = ((acct_from.info.authorized * amount)/PCNT);

   } else {
      if (amount < 0.0) {
         fprintf(stderr,"Invalid alloc value for transfer\n");
         return(-1);
      }
      trnsf_amt = amount;
   }
   
   /*
    * Transfer time must not exceed time remain in the account
    */
   remain = acct_from.info.authorized - acct_from.info.sbu_time;
   if (trnsf_amt > remain) {
      fprintf(stderr, "Transfer amount exceeds remaining allocation for %s\n",
      nx_getaid(agid_from)->acct_name);
      return(-1);
   }
   
   /*
    * Transfer the time -- Set the new time in both accounts
    */
   acct_to.info.authorized	+= trnsf_amt;
   acct_from.info.authorized	-= trnsf_amt;
   if (acct_from.info.authorized <= 0.0) {
      acct_from.info.authorized = 1.0;
   }
   
   /*
    * Send both account entries to MACD
    */
   if (cpuctl(C_SETACCT, &acct_from) < 0) {
      macerr();
      return(-1);
   }
   if (cpuctl(C_SETACCT, &acct_to) < 0) {
      macerr();
      return(-1);
   }
   return(0);
}


/*===========================================================================*
 * Function:    trandump
 *
 * Abstract:    This function is for debugging
 *
 * Arguments:
 *	data - info to be printed out
 *
 * Return value:
 *
 * Notes:
 *===========================================================================*/
void trandump(data)
   struct cpu_ctrl *data;
{
   printf("struct cpu_ctrl:\n");
   printf("\tuid=%d\n", data->uid);
   printf("\tagid=%d\n", data->agid);
   printf("\ttimestamp=%d\n", data->info.timestamp);
   printf("\tstruct cpu_lblk info:\n");
   printf("\t\tid=%d\n", data->info.id);
   printf("\t\tinhibit=%d\n", data->info.inhibit);
   printf("\t\tmodify=%d\n", data->info.modify);
   printf("\t\ttransfer=%d\n", data->info.transfer);
   printf("\t\tuse=%d\n", data->info.use);
   printf("\t\tpercent=%d\n", data->info.percent);
   printf("\t\tauthorized=%f\n", data->info.authorized);
   printf("\t\tused=%f\n", data->info.used_time);
   printf("\tseq=%d\n", data->seq);
}
