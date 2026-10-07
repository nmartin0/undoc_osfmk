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
 *	twrite_.c 11.1 94/03/22 16:50:12
 */
static char	twrite__ver[] = "@(#) sourcefile twrite_.c 11.1 94/03/22 16:50:12";

/*
 *
 *  ******************  ENGINEERING CHANGE HISTORY ***********************
 *
 *    Date      Engineer                  Description
 *
 *  2/25/92     G. Haycox       Module creation release to BETA
 *
 */

/*--------------------------------------------------------------------------
 *
 *  twrite_.c
 *
 *  INPUT:   handle: pointer to the handle from the TALLOCTAPE
 *           buffer: pointer where the user data is for write
 *           length: pointer to the length variable for the user buffer
 *
 *  OUTPUT:  zero:   Write completed without error
 *
 *           -or-
 *
 *           negative: indicating an error
 *
 *  Description:
 *
 *     FORTRAN I/F Routine for twrite():
 *
 -------------------------------------------------------------------------*/

int twrite_(int *handle, char *buffer, int *length) {

    return ( twrite(*handle, buffer, *length) );

}


