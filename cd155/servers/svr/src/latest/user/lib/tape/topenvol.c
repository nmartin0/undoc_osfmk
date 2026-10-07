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
 *	topenvol.c 11.1 94/03/22 16:49:49
 */
static char	topenvol_ver[] = "@(#) sourcefile topenvol.c 11.1 94/03/22 16:49:49";

/*
 *
 */

#include "stdio.h"


#include "type.h"
#include "tapeio.h"
#include "tape3480.h"
#include "tapeglob.h"
#include "string.h"
#include "protos.h"
#include "defines.h"

extern int errno;

int
tapealloc(VCB *vcb, char *path)
{
	LVCB   *lvcbptr=NULL;
	int     flag=0, i=0, rc=0;

  /*
   * The user's main process already allocated a Tapemain control
   * block.  Reserve the tape device for this process.
   */
  if (vcb->vollist == NULL) {  
	/* error if no vol serial list specfied */
    return(TERR_NO_VOLSER);
  }

  /*
  rc = reserve_tape((char *) path);
  if (rc != 0) {
    return(TERR_COULD_NOT_RESERVE_TAPE);
  }
  */


  /*
   * Tape is reserved.  
   * Allocate an LVCB, and initialize a tapemain entry.
   */
  lvcbptr = (LVCB*) malloc(sizeof(LVCB));

  if (lvcbptr == NULL) 
    return(TERR_MEM_ALLOC);
  memset((char *)lvcbptr, 0x00, sizeof(LVCB));

  for (i=0; ((i<MAX_DEVICES) && (flag==0)); i++) {
    if (tapemain.lvcblist[i] == NULL) {
      tapemain.lvcblist[i] = lvcbptr;
      lvcbptr->tindex = i;
      flag=1;
    }
  }

	/*
	 * Copy fields to the local vcb.
	 */
	lvcbptr->exlst = vcb->exlst;
	strcpy(lvcbptr->path, path);

	for (i=0; vcb->vollist[i].serialno[0] != 0; i++) {
		strcpy(lvcbptr->vollist[i+1].serialno, 
			vcb->vollist[i].serialno);
	}
	lvcbptr->volcount = (i);
	lvcbptr->cur_volseqno = 1;
	return(lvcbptr->tindex);
}
