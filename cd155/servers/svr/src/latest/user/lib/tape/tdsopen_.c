/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	tdsopen_.c 11.1 94/03/22 16:50:14
 */
static char	tdsopen__ver[] = "@(#) sourcefile tdsopen_.c 11.1 94/03/22 16:50:14";

/*
 *
 * Open a file on a 3480-type tape device.
 *
 */

#include "stdio.h"

#include "type.h"
#include "tapeio.h"
#include <mtio.h>
/*
#include <tape.h>
*/
#include <sys/types.h>
#include "tape3480.h"
#include "extern.h"
#include "string.h"
#include "protos.h"
#include "defines.h"
#include "time.h"
#include <stdio.h>
#include <fcntl.h>

#include "tioproto.h"

typedef struct {
  char dsname[17];    /* Data Set Name  */
  int dsnlen;         /* Data Set Name Length */
  int volseqno;       /* Volume Sequence Number       */
  int dsseqno;        /* Data Set Sequence Number     */
  char expirec;       /* Expiration Date - century (blank=19, 0=20, etc */
  int expireyy;       /* Expiration Date - year (91, 92, etc)  */
  int expiredd;       /* Expiration Date - day of year  */
  char system_code[13]; /* System Code */
  int syscdlen;       /* System Code Length */
  char recfm;         /* Record Format */
  int blocklen;       /* Block Length */
  int reclen;         /* Record Length  */
  char blkattr;       /* Block Attribute */
  int blocknum;       /* Block ID Number   */
  int read;           /* Opened for read (write if zero)  */
  int buffered_mode;  /* Use buffering on I/O node */
  int block_mode;     /* Read/Write Blocks at a time */
  int num_tapeblk_bufs;
} FDCB;


/**/
tdsopen_(int *handle, FDCB *fdcb)
{
  DCB *dcbptr;
  int i, rc;

  dcbptr = (DCB*) malloc(sizeof(DCB));
  if (dcbptr == NULL) return(TERR_MEM_ALLOC);
  memset(dcbptr, 0x00, sizeof(DCB));

  strncpy((char*)dcbptr->dsname, fdcb->dsname, fdcb->dsnlen);
  for ( i=fdcb->dsnlen; i<(sizeof(dcbptr->dsname)); i++) {
    dcbptr->dsname[i] = '\0';
  }
  dcbptr->volseqno = fdcb->volseqno;
  dcbptr->dsseqno =  fdcb->dsseqno;
  dcbptr->expirec =  fdcb->expirec;
  dcbptr->expireyy = fdcb->expireyy;
  dcbptr->expiredd = fdcb->expiredd;
  strncpy((char*)dcbptr->system_code, fdcb->system_code, fdcb->syscdlen);
  dcbptr->system_code[fdcb->syscdlen] = '\0';
  dcbptr->recfm = fdcb->recfm;
  dcbptr->blocklen = fdcb->blocklen;
  dcbptr->reclen = fdcb->reclen;
  dcbptr->blkattr = fdcb->blkattr;
  dcbptr->blocknum =fdcb->blocknum;
  dcbptr->read = fdcb->read;
  dcbptr->buffered_mode = fdcb->buffered_mode;
  dcbptr->block_mode = fdcb->block_mode;
  dcbptr->num_tapeblk_bufs = fdcb->num_tapeblk_bufs;

  rc = tdsopen(*handle, dcbptr);
  free(dcbptr);
  return(rc);
}
