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
 *	talloc_.c 11.1 94/03/22 16:50:10
 */
static char	talloc__ver[] = "@(#) sourcefile talloc_.c 11.1 94/03/22 16:50:10";

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


tapealloc_(int (*exlst)(), int *useexlst, char *vollist, int *numvols, char *fpath, int *fpathlen)
{
  char charactr, cpath[128];
  VCB  *vcbptr;
  char *vlstptr, *tempfvlst, *tempvvlst;
  int rc, i;

  vlstptr = (char*) malloc(7*(*numvols));
  vcbptr  = (VCB*)  malloc(sizeof(VCB));

  if ((vlstptr == NULL) || (vcbptr == NULL)) 
    return(TERR_MEM_ALLOC);

  memset(vlstptr, 0x00, (7*(*numvols)));
  memset((char *)vcbptr,  0x00, (sizeof(VCB)));

  if (*useexlst) 
	vcbptr->exlst = exlst;

  vcbptr->vollist = (VOLSER*) vlstptr;

  for ( i=0, tempfvlst=vollist, 
	tempvvlst=(char*) vcbptr->vollist; i<(*numvols*6); i++ ) {
    *(tempvvlst+(i+i/6)) = *(tempfvlst+i);
  }

  strncpy(cpath, fpath, *fpathlen);
  cpath[*fpathlen] = '\0';

printf(" tapealloc_ : before tapealloc \n");
  rc = tapealloc(vcbptr, cpath);
printf(" tapealloc_ : after tapealloc \n");

  free(vlstptr); 
  free(vcbptr); 

  return(rc);
}
