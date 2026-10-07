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
 *	ptoi.c 11.1 94/03/22 16:49:34
 */
static char	ptoi_ver[] = "@(#) sourcefile ptoi.c 11.1 94/03/22 16:49:34";

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
 * ptoi()
 *
 * INPUT:   packed:  pointer to an array of IBM packed digits
 *          result:  pointer to integer for conversion result
 *          length:  number of bytes to convert
 *
 * OUTPUT:   0:     conversion complete without error
 *          -1:     overflow of a 32 bit integer
 *
 * Description:
 *
 *      Convert an array of IBM packed digits (packed BCD like)
 *      to a 32 bit integer (i860 format), the number of packed
 *      digits is determined by the length parameter.
 *
 --------------------------------------------------*/

#include "converr.h"
#include "convec.h"


int ptoi(unsigned char *packed, int *result, int length) {

    int i, nib, err_flag = 0, result_val = 0, digit, temp;

    do {
        if (length == 0) {
            err_flag = cerr_info[ERR_INVALID_PACKED_LEN].err_code;
            break;
        }
        for (i=0, nib = 0; nib < (length * 2) - 1; ++nib) {
            if (nib & 1) {
                digit = packed[i] & 0x0F;
                ++i;
            }
            else {
                digit = (packed[i] & 0xF0) >> 4;
            }
            switch(digit) {
                case 0x0:
                case 0x1:
                case 0x2:
                case 0x3:
                case 0x4:
                case 0x5:
                case 0x6:
                case 0x7:
                case 0x8:
                case 0x9:
                    if ((temp = ((result_val * 10) + digit)) < result_val) {
                        err_flag = cerr_info[ERR_OVERFLOW].err_code;
                    }
                    else
                        result_val = temp;
                    break;
                default:
                        err_flag = cerr_info[ERR_INVALID_PACKED_NUM].err_code;
            }
        }

        /*--------------------------------------------------
         * Check for the sign bit on the last nibble.
         * If the err_flag == OVERFLOW then allow to pass
         * to see if it is the smallest negative number.
         --------------------------------------------------*/

        if (!err_flag || (err_flag == CERR_OVERFLOW)) {
            switch(packed[i] & 0x0F) {
                case 0xC:      /* Postive Number */
                case 0xF:
                    if (!err_flag) {
                        *result = result_val;
                    }
                    break;
                case 0xD:       /* negative number */
                    if (!err_flag) {
                        *result = result_val * -1;
                    }
                    else if (temp == 0x80000000) {
                        err_flag = 0;
                        *result = temp;
                    }
                    break;
                default:
                    err_flag = cerr_info[ERR_INVALID_SIGN].err_code;
            }

        }
    }while (0);
    if (err_flag) {
        *result = 0;
    }
    return(err_flag);
}

