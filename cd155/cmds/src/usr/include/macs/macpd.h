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
*
* macpd.h
*
*
* This is the persistent daemon for main MACS daemon (macd).
* This daemon (macpd) fork macd and watches it all the time. If the macd
* dies for any reason it will fork a new one. It logs everything to the
* log file (macpd.log)
*
*****************************************************************************/

#define INTEL

#ifdef SUN
#undef INTEL
#endif

#define NXACCT_0      (1)	/* nxaccount file is 0 length */
#define NXACCT_STAT   (2)	/* fail in getting nxaccount status */
#define NXACCT_NONE   (3)	/* nxaccount file does not exist */
#define MACSCONF_0    (4)	/* macs.conf file is 0 length */
#define MACSCONF_STAT (5)	/* fail in getting macs.conf status */
#define MACSCONF_NONE (6)       /* macs.conf file does not exist */
#define MACSCONF_ERR  (7)       /* macs.conf file does not exist */
#define NRD_NQSTABLE  (8)       /* nqstable file non-readable */
#define NUL_NQSTABLE  (9)       /* nqstable file empty */
#define CNT_NQSTABLE  (10)       /* nqstable first line format error */
#define FMT_NQSTABLE  (11)      /* nqstable file format error */
#define DIF_NQSTABLE  (12)      /* count is differ from list */
#define MACDDB_0      (13)	/* macd.data file is 0 length */
#define MACDDB_STAT   (14)	/* fail in getting macd.data status */
#define MACDDB_NONE   (15)	/* macd.data file could not be found */
#define MACDDB_OPEN   (16)	/* macd.data fail open read */
#define MACDDB_FMT    (17)	/* macd.data file format unrecognizable */
#define MACDDB_SIZE   (18)	/* macd.data file size wrong */
#define MACPDLOG_ERR  (19)	/* fail in open macpd.log */
#define GETHOST_ERR   (20)	/* fail in getting hostname */
#define MACDDB_RECOVERED (21)	/* macd.data file recovered from backups */

#define NUM_PIDS 1

#define SRVR_PID 0

#define SLEEP_TIME 10            /*  10 seconds  minimum sleep */
#define TIME_DIFF SLEEP_TIME+60 /* 66 seconds, if it is forking new macd too
                                 * often */

#define MAX_SLEEP_TIME 15*60     /* max retry delay 15 minutes */

int             pid[NUM_PIDS];

#ifdef INTEL
#define MACD_STR "macd"
#endif

void LogMessageToFile();
int StartChildProcess();
int KillChildProcess();








