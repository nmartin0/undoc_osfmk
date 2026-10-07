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
*
* conf.h
* 
* File conf.h defines the configuration structure for the MACS
* software.
*
*****************************************************************************/

#ifndef CONF_H
#define CONF_H

struct macsconf
  {
   int  total_nodes;		/* total number of nodes in the system */

   char *mailer;		/* mailing program used to mail accounting */
				/* information to users			   */

   char *admin;			/* if not "", then accounting information */
				/* is mailed to this account in addition  */
				/* to the user                            */

   char *operator;		/* name of the operator to notify in case iPSC */
				/* cubes are down (naw, that couldn't happen..)*/

   int  switchlog;		/* how often to switch the logs in the MACD */
				/* - LOGDAILY, LOGWEEKLY, LOGMONTHLY     */

#define LOGDAILY (0)		/* switch MACD logs daily */
#define LOGWEEKLY (1)		/* switch MACD logs weekly */
#define LOGMONTHLY (2)		/* switch MACD logs monthly */

   int macdmode;		/* how macd is to be run - one of ACCTONLY or */
				/* MACWATCH                                   */

#define ACCTONLY (0)		/* only do accounting - do not watch allocations */
#define MACWATCH (1)		/* look at user resource usage */

   int enforce;			/* allocation enforcement */

#define NOKILL (0)		/* Do NOT kill jobs */
#define ACCTKILL (1)		/* kill when account allocation overrun */
#define USERKILL (2)		/* kill when user allocation overrun */
#define KILL (3)		/* kill when user|account allocation overrun */
#define FLAGKILL (4)		/* kill according to user|account settings */

   float def_qrate;		/* default cpu charge rate for undefined queues */

   float uunt_rate;		/* under-used-node-time charge rate */

   float idle_rate;		/* idle node-time charge rate */
   int sync_interval;		/* time interval in minutes to issue status
				request to SM and write database to disk */
#define DEFAULT_SYNC_INTERVAL	15
  };

struct macsconf *readconf();

#endif
