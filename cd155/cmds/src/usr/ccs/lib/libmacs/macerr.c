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
 * File:        macerr.c
 *
 * macerr()
 *
 * Abstract:	print error message according to errno returned by MACD
 *
 * Arguments:	None
 *
 * Return value: None
 *
 * Notes:
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "@(#)macerr.c	2.3 10/15/92";
#endif

#include <stdio.h>
#include <stdlib.h>
#include "mac.h"
#include <errno.h>

#include "macerr.h"

int	macerrno = ER_NONE;

static void	printout __((const char *msg));

void macerr()
{
  switch(errno) {
  case EIO:
    fprintf(stderr," Incomplete cpuctl request ");
    break;
  case EACCTONLY:
    fprintf(stderr," Invalid request under accounting-only mode ");
    break;
  case EINVAL:
    fprintf(stderr," Invalid request ");
    break;
  case ENOENT:
    fprintf(stderr," No database entry ");
    break;
  case EEXIST:
    fprintf(stderr," Account already exists ");
    break;
  case EACCES:
    fprintf(stderr, " Insufficient privileges, permission denied\n");
    break;
  case ENOTTY:
    fprintf(stderr, " Cpuctl request not currently available\n");
    break;
  case EPIPE:
    fprintf(stderr, " Error communicating with MACD ");
    fprintf(stderr, "(broken pipe) \n     ");
    break;
  default:
    perror("perror");
  }
}

/*===========================================================================*
 * Function:	macerr_fatal
 *
 * Abstract:	This function 
 *
 * Arguments:
 *
 * Return value:
 *
 * Notes:
 *===========================================================================*/
void macerr_fatal(msg)
  const char	*msg;
{
  printout(msg);
}

/*===========================================================================*
 * Function:	macerr_report
 *
 * Abstract:	This function 
 *
 * Arguments:
 *
 * Return value:
 *
 * Notes:
 *===========================================================================*/
void macerr_report(msg)
  const char	*msg;
{
  printout(msg);
  exit(-1);
}

/*===========================================================================*
 * Function:	printout
 *
 * Abstract:	This function 
 *
 * Arguments:
 *
 * Return value:
 *
 * Notes:
 *===========================================================================*/
static void printout(msg)
  const char	*msg;
{
  if (msg != NULL) {
    fprintf(stderr, "%s : ", msg);
  }
  switch (macerrno) {
  case ER_NONE:
    fprintf(stderr,"no error");
    break;

  case ER_BADLOCK:
    fprintf(stderr, "Fail getting lock on  \"/etc/nxaccount\"\n");
    break;

  case ER_BADUNLOCK:
    fprintf(stderr, "Fail releasing lock on  \"/etc/nxaccount\"\n");
    break;

  case ER_FAILOPEN:
    fprintf(stderr, "Fail opening \"/etc/nxaccount.mac\"\n");
    break;

  case ER_FAILOPENC:
    fprintf(stderr, "Fail opening \"/etc/nxaccount\"\n");
    break;

  case ER_FAILOPEN_TMP:
    fprintf(stderr, "Fail opening \"/etc/nxaccomm.mac\"\n");
    break;

  case ER_FAILOPENC_TMP:
    fprintf(stderr, "Fail opening \"/etc/nxaccomm\"\n");
    break;

  case ER_BADAGID:
    fprintf(stderr, "Invalid agid\n");
    break;

  case ER_BADUID:
    fprintf(stderr, "Invalid uid\n");
    break;

  case ER_BADREC:
    fprintf(stderr, "Line exeed 128 char. in \"/etc/nxaccount\"\n");
    break;

  case ER_AGIDEXISTS:
    fprintf(stderr, "Account ID exists in \"/etc/nxaccount\"\n");
    break;

  case ER_ANAMEEXISTS:
    fprintf(stderr, "Account name exists in \"/etc/nxaccount\"\n");
    break;

  default:
    fprintf(stderr,"Unknown error");
    break;
  }
}

