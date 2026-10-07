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
 *	itoi.c 11.1 94/03/22 16:49:25
 */
static char	itoi_ver[] = "@(#) sourcefile itoi.c 11.1 94/03/22 16:49:25";

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

/*--------------------------------------------------
 * itoi.c
 *
 *  This module contains the following functions:
 *
 *      itoI_16()   intel to IBM  16 bit integer
 *      itoI_32()   intel to IBM  32 bit integer
 *
 *      Itoi_16()   IBM to intel 16 bit integer
 *      Itoi_32()   IBM to intel 32 bit integer
 *
 *
 --------------------------------------------------*/
#include <type.h>


/*--------------------------------------------------
 * itoI_16()
 *
 *      This routine will swap the bytes within an
 *      intel 16 bit integer to create an IBM 16
 *      bit integer.
 *
 *      Input:  short   intel integer (16bit)
 *      Output: short   ibm integer  (16bit)
 *
 --------------------------------------------------*/

ubit16 
itoI_16(ubit16 intel_int16) 
{
    bit16 result;

    ((char *)&result)[0] = ((char *)&intel_int16)[1];
    ((char *)&result)[1] = ((char *)&intel_int16)[0];
    return((ubit16) result);
}

/*--------------------------------------------------
 * itoI_32()
 *
 *      This routine will swap the bytes within
 *      each word and the words within the 32 bit
 *      long to convert from an intel 32 bit integer
 *      to an IBM 32 bit integer.
 *
 *      Input:  int   intel integer (32bit)
 *      Output: int   ibm integer (32bit)
 *
 --------------------------------------------------*/

ubit32 itoI_32(ubit32 intel_int32) {

    bit32 result;


    ((char *)&result)[0] = ((char *)&intel_int32)[3];
    ((char *)&result)[1] = ((char *)&intel_int32)[2];
    ((char *)&result)[2] = ((char *)&intel_int32)[1];
    ((char *)&result)[3] = ((char *)&intel_int32)[0];

    return((ubit32) result);

}



/* PAGE  */


/*--------------------------------------------------
 * Itoi_16()
 *
 *      This routine will swap the bytes within an
 *      IBM 16 bit integer to create an intel 16
 *      bit integer.
 *
 *      Input:  short   ibm integer (16bit)
 *      Output: short   intel integer (16bit)
 *
 --------------------------------------------------*/


ubit16 Itoi_16(ubit16 ibm_int16) {

    bit16 result;

    ((char *)&result)[0] = ((char *)&ibm_int16)[1];
    ((char *)&result)[1] = ((char *)&ibm_int16)[0];

    return((ubit16)result);

}

/* PAGE  */

/*--------------------------------------------------
 * Itoi_32()
 *
 *      This routine will swap the bytes within
 *      each word and the words within the 32 bit
 *      long to convert from an IBM 32 bit integer
 *      to an intel 32 bit integer.
 *
 *      Input:  int   ibm integer (32bit)
 *      Output: int   intel integer (32bit)
 *
 --------------------------------------------------*/

ubit32 Itoi_32(ubit32 ibm_int32) {

    bit32 result;


    ((char *)&result)[0] = ((char *)&ibm_int32)[3];
    ((char *)&result)[1] = ((char *)&ibm_int32)[2];
    ((char *)&result)[2] = ((char *)&ibm_int32)[1];
    ((char *)&result)[3] = ((char *)&ibm_int32)[0];

    return((ubit32)result);

}


