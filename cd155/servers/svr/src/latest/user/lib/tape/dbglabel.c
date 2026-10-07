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
 *	dbglabel.c 11.1 94/03/22 16:49:21
 */
static char	dbglabel_ver[] = "@(#) sourcefile dbglabel.c 11.1 94/03/22 16:49:21";

/*
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
/**/
dbglabel(int handle)
{
LVCB *lvcbptr;
  /*--------------------------------------------------------------------
   * Verify that the handle is valid (points to LVCB).
   *--------------------------------------------------------------------*/ 
  lvcbptr = tapemain.lvcblist[handle];
  if (lvcbptr == NULL) {
    return(TERR_INV_HANDLE);
  }
  lvcbptr->debug_msgs = 1;
  return(0);
}
