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
 *	ptoa_.c 11.1 94/03/22 16:50:02
 */
static char	ptoa__ver[] = "@(#) sourcefile ptoa_.c 11.1 94/03/22 16:50:02";

/*
 *
 *
 *  ******************  ENGINEERING CHANGE HISTORY ***********************
 *    Date      Engineer                  Description
 *
 *  2/25/92     G. Haycox       Module creation release to BETA
 *
 *
 */

#include "type.h"
#include "convt.h"
#include "cnvproto.h"

/*--------------------------------------------------
 * ptoa_()
 *
 *      FORTRAN I/F Routine
 --------------------------------------------------*/

int ptoa_(unsigned char *packed, char *asc_buf, int *length) {

    /*--------------------------------------------------
     *  Call the ptoa routine with the 'null_term'
     *  parameter set to '0' so the ascii array will
     *  NOT be null terminated.
     --------------------------------------------------*/

    return ( ptoa_rtn(packed, asc_buf, *length, 0) );

}


