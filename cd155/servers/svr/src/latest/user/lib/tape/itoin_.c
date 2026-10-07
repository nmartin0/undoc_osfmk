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
 *	itoin_.c 11.1 94/03/22 16:49:58
 */
static char	itoin__ver[] = "@(#) sourcefile itoin_.c 11.1 94/03/22 16:49:58";

/*
 *
 *  ******************  ENGINEERING CHANGE HISTORY ***********************
 *
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
 * itoin_.c
 *
 *  This module contains the FORTRAN I/F Routines for
 *  the following functions:
 *
 *      FORTRAN converts function names to lower case
 *      so only one routine needs to be defined.
 *
 *      itoin_16()
 *      itoin_32()
 *
 *
 *
 --------------------------------------------------*/


/*--------------------------------------------------
 * itoin_16_()
 *
 *
 --------------------------------------------------*/

void itoin_16_(short intel_int16[], short ibm_int16[], int *length) {

    itoIn_16(intel_int16, ibm_int16, *length);
}



/*--------------------------------------------------
 * itoin_32_()
 *
 *
 --------------------------------------------------*/

void itoin_32_(int intel_int32[], int ibm_int32[], int *length) {

    itoIn_32(intel_int32, ibm_int32, *length);
}


