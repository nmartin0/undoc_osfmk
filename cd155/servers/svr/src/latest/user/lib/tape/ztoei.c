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
 *	ztoei.c 11.1 94/03/22 16:49:57
 */
static char	ztoei_ver[] = "@(#) sourcefile ztoei.c 11.1 94/03/22 16:49:57";

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
 * ztoei()
 *
 * INPUT:   zoned:   pointer to an array of IBM zoned digits
 *          result:  pointer to an extended integer (esize_t
 *                   struct) for the conversion result
 *          length:  number of zoned digits to convert
 *
 * OUTPUT:   0:     conversion complete without error
 *
 * Description:
 *
 *      Convert an array of IBM zoned digits (unpacked BCD like)
 *      to a 64 bit extended integer (esize_t), the number of zoned
 *      digits is determined by the length parameter.
 *
 *
 --------------------------------------------------*/

#include "convt.h"
#include "converr.h"

int ztoei(unsigned char *zoned, esize_t *result, int length) {

    int i, err_flag = 0, sign;
    esize_t result_val, temp;

    result_val.shigh = result_val.slow = temp.shigh = temp.slow = 0;

    do {
        if (length == 0) {
            err_flag = cerr_info[ERR_INVALID_ZONED_LEN].err_code;
            break;
        }
        for (i=0; i<length-1; ++i) {
            switch(zoned[i]) {
                case 0xF0:
                case 0xF1:
                case 0xF2:
                case 0xF3:
                case 0xF4:
                case 0xF5:
                case 0xF6:
                case 0xF7:
                case 0xF8:
                case 0xF9:
                    temp = _emul(result_val, (long)10);
                    temp = _eadd1(temp, (long)(zoned[i] & 0x0F));
                    if ( _ecmp(result_val, temp) > 0) {
                        err_flag = cerr_info[ERR_OVERFLOW].err_code;
                    }
                    else {
                        result_val = temp;
                    }
                    break;
                default:
                    err_flag = cerr_info[ERR_INVALID_ZONED_NUM].err_code;
            }
            if (err_flag) {
                break;
            }
        }
        if (err_flag) {
            break;
        }
        switch(zoned[i] & 0xF0) {
            case 0xA0:      /* Postive Number */
            case 0xC0:
            case 0xE0:
            case 0xF0:
                sign = 1;
                break;
            case 0xB0:      /* Negative Number */
            case 0xD0:
                sign = -1;
                break;
            default:
                err_flag = cerr_info[ERR_INVALID_SIGN].err_code;
        }

        if (err_flag) {
            break;
        }
        if ((zoned[i] & 0x0F) > 9) {
            err_flag = cerr_info[ERR_INVALID_ZONE_DIGIT].err_code;
            break;
        }
        temp = _emul(result_val, 10);
        temp = _eadd1(temp, (long)(zoned[i] & 0x0F));
        if ( _ecmp(result_val, temp)  > 0) {
            if ((sign == -1) && (temp.shigh == 0x80000000) & (temp.slow == 0)) {
                *result = temp;
            }
            else {
                err_flag = cerr_info[ERR_OVERFLOW].err_code;
            }
            break;
        }

        *result = _emul(temp, (long)sign);

    } while(0);

    if (err_flag) {
        result->shigh = result->slow = 0;
    }
    return(err_flag);
}
