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
 *	eitoz_.c 11.1 94/03/22 16:50:05
 */
static char	eitoz__ver[] = "@(#) sourcefile eitoz_.c 11.1 94/03/22 16:50:05";

/*
 *  eitoz_.c
 *
 *  ******************  ENGINEERING CHANGE HISTORY ***********************
 *
 *    Date      Engineer                  Description
 *
 *  2/25/92     G. Haycox       Module creation release to BETA
 *
 *
 */

/*--------------------------------------------------
 * eitoz_()
 *
 *     FORTRAN I/F Routine:
 *
 --------------------------------------------------*/

#include "type.h"
#include "convt.h"
#include "cnvproto.h"

int eitoz_(esize_t *eint, unsigned char zone[], int *zone_size) {

    return( eitoz(*eint, zone, *zone_size) );
}


