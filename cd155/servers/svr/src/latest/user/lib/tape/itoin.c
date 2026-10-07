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
 *	itoin.c 11.1 94/03/22 16:49:27
 */
static char	itoin_ver[] = "@(#) sourcefile itoin.c 11.1 94/03/22 16:49:27";

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
 * itoin.c
 *
 *  This module contains the following functions:
 *
 *      itoIn_16()   intel to IBM  16 bit integer array
 *      itoIn_32()   intel to IBM  32 bit integer array
 *
 *      Itoin_16()   IBM to intel 16 bit integer array
 *      Itoin_32()   IBM to intel 32 bit integer array
 *
 *
 --------------------------------------------------*/

#include <type.h>


/*--------------------------------------------------
 * itoIn_16()
 *
 *      Convert an array of intel 16bit integers
 *      to an array of IBM 16bit integers.
 *
 *      Input:  short *  pointer to an array of intel
 *                       integers (16bit)
 *
 *              short *  pointer to an array of IBM
 *                       integers  (16bit)
 *
 *              int      the number of array elements
 *
 --------------------------------------------------*/
void 
itoIn_16(bit16 intel_int16[], bit16 ibm_int16[], int length) 
{
    int i;
    bit16 temp;

    for (i=0; i < length; ++i) {

        temp = intel_int16[i];

        ((unsigned char *)&ibm_int16[i])[0] = ((unsigned char *)&temp)[1];
        ((unsigned char *)&ibm_int16[i])[1] = ((unsigned char *)&temp)[0];
    }
}


/*--------------------------------------------------
 * itoIn_32()
 *
 *      Convert an array of intel 32bit integers
 *      to an array of IBM 32bit integers.
 *
 *      Input:  int *    pointer to an array of intel
 *                       integers (32bit)
 *
 *              int *    pointer to an array of IBM
 *                       integers  (32bit)
 *
 *              int      the number of array elements
 *
 --------------------------------------------------*/

void itoIn_32(bit32 intel_int32[], bit32 ibm_int32[], int length) {

    int i, temp;

    for (i=0; i < length; ++i) {

        temp = intel_int32[i];

        ((unsigned char *)&ibm_int32[i])[0] = ((unsigned char *)&temp)[3];
        ((unsigned char *)&ibm_int32[i])[1] = ((unsigned char *)&temp)[2];
        ((unsigned char *)&ibm_int32[i])[2] = ((unsigned char *)&temp)[1];
        ((unsigned char *)&ibm_int32[i])[3] = ((unsigned char *)&temp)[0];

    }
}

/* PAGE  */


/*--------------------------------------------------
 * Itoin_16()
 *
 *
 *      Convert an array of IBM 16bit integers
 *      to an array of intel 16bit integers.
 *
 *      Input:  short *  pointer to an array of IBM
 *                       integers (16bit)
 *
 *              short *  pointer to an array of intel
 *                       integers  (16bit)
 *
 *              int      the number of array elements
 *
 *
 *
 --------------------------------------------------*/

void Itoin_16(bit16 ibm_int16[], bit16 intel_int16[], int length) {

    int i;
    bit16 temp;
    unsigned char boo;

printf("before the for loop in Itoin_16\n");
printf("size of bit16 is %d\n", sizeof(temp));
printf("size of unsigned char is %d\n", sizeof(boo));
    for (i=0; i < length; ++i) {

        temp = ibm_int16[i];

        ((unsigned char *)&intel_int16[i])[0] = ((unsigned char *)&temp)[1];
        ((unsigned char *)&intel_int16[i])[1] = ((unsigned char *)&temp)[0];

    }
printf("after the for loop in Itoin_16\n");
}

/* PAGE  */


/*--------------------------------------------------
 * Itoin_32()
 *
 *
 *      Convert an array of IBM 32bit integers
 *      to an array of intel 32bit integers.
 *
 *      Input:  int *    pointer to an array of IBM
 *                       integers (32bit)
 *
 *              int *    pointer to an array of intel
 *                       integers  (32bit)
 *
 *              int      the number of array elements
 *
 --------------------------------------------------*/

void Itoin_32(bit32 ibm_int32[], bit32 intel_int32[],int length) {

    int i, temp;

    for (i=0; i < length; ++i) {

        temp = ibm_int32[i];

        ((unsigned char *)&intel_int32[i])[0] = ((unsigned char *)&temp)[3];
        ((unsigned char *)&intel_int32[i])[1] = ((unsigned char *)&temp)[2];
        ((unsigned char *)&intel_int32[i])[2] = ((unsigned char *)&temp)[1];
        ((unsigned char *)&intel_int32[i])[3] = ((unsigned char *)&temp)[0];

    }
}


