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
 *	itop_.c 11.1 94/03/22 16:49:59
 */
static char	itop__ver[] = "@(#) sourcefile itop_.c 11.1 94/03/22 16:49:59";

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
 * itop_()
 *
 *      FORTRAN I/F Routine
 *
 --------------------------------------------------*/


int itop_(int *intel_int, unsigned char packed[], int *packed_length) {

    return( itop(*intel_int, packed, *packed_length) );
}

