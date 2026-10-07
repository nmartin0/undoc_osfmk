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
* macd.h
*
* This file defines data structures needed by the MACD
*
*****************************************************************************/

#ifndef MACD_H
#define MACD_H

#define MACD_PORT (2001)	/* port address for the MACD */
#define MAXCUBES (32)		/* maximum number of cubes at one time */

/* debugging levels */
#define NO_SMD	2

struct msg_in
  {
   int socket;			/* socket making the request */
   int command;			/* type of request made to MACD */
   int size;			/* number of bytes read into data buffer */
   char *data;			/* data buffer - read in from socket */
  };
#define header(r) ((char *) &(r->command))

/* reply header fields */
#define reply_return 	(*(reply_intptr))
#define reply_errno	(*(reply_intptr+1))
#define reply_datasize	(*(reply_intptr+2))

/* request header fields */
#define reqst_reqnum	(*(reqst_intptr))
#define reqst_datasize	(*(reqst_intptr+1))

/* header size */
#define C_REQUEST_HDRLEN	2*sizeof(int)
#define C_REPLY_HDRLEN		3*sizeof(int)

struct macd_reply
  {
   int returnvalue;		/* return value from the request */
   int errno;			/* if request failed, reason for failure */
   int size;			/* size of data buffer which will follow */
  };

struct client_request
  {
   int command;			/* type of request made to MACD */
   int size;			/* number of data bytes following */
  };
 
#define DB_OVERFLOW (503)
#define DB_REHASH (1013)

#define UP	1
#define DOWN	0

/* error code >= SMD_DISCONN will be reported to ADMIN */

#define USER_STOP        (0)            /* user allocation overrun, killed */
#define ACCT_STOP        (1)            /* account allocation overrun, killed */
#define ACCTNOACCT    (1<<2)            /* user has no authorized account */
#define ACCTNONODES   (1<<3)            /* user allocated too many nodes */
#define USEROVER      (1<<4)            /* user over user allocation */
#define ACCTOVER      (1<<5)            /* user over account allocation */
#define USERNOUSE     (1<<6)		/* user account set to no-use */
#define SMD_DISCONN   (1<<8)            /* fail in reconnect to SMD */
#define DEF_NQSTABLE  (1<<9)		/* fail in reading queue charges */
#define DB_INIT       (1<<10)		/* fail in initializing database file */
#define ERR_NQSTABLE  (1<<11)		/* Format error in nqstable file */
#define QNL_NQSTABLE  (1<<12)		/* Q name too long (>32) in nqstable file */
#define ERR_MACHEART  (1<<13)		/* Fail in touching heart.beat file */
#define ERR_MACLOG    (1<<14)		/* fail in writing to MACD logfile */


void portReply();		/* send a response back to a client */
void portCloseConnection();	/* close a particular socket number */
void portRequest();		/* get a request from a client */
int portOpen();			/* open up the sockets for connections */
void portClose();		/* close up the server accept socket */

#endif
