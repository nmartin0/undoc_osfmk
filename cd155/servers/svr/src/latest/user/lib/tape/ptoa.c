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
 *	ptoa.c 11.1 94/03/22 16:49:32
 */
static char	ptoa_ver[] = "@(#) sourcefile ptoa.c 11.1 94/03/22 16:49:32";

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
#include "converr.h"

/*--------------------------------------------------
 * ptoa() - 'C' slick for the ptoa conversion.
 *
 * INPUT:   packed:  pointer to an array of IBM packed digits
 *          asc_buf: pointer to integer for conversion result
 *          length:  number of bytes to convert
 *
 * OUTPUT:   0:     conversion complete without error
 *          -X:     some error occurred
 *
 * Description:
 *
 *      Convert an array of IBM packed digits (packed BCD like)
 *      to a NULL (0x00) terminated ASCII string, the number
 *      of bytes is determined by the length parameter.
 *
 --------------------------------------------------*/


int ptoa(unsigned char *packed, char *asc_buf, int length) {

    /*--------------------------------------------------
     *  Call the ptoa routine with the 'null_term'
     *  parameter set to '1' to null terminate the
     *  string.
     --------------------------------------------------*/

    return ( ptoa_rtn(packed, asc_buf, length, 1) );

}


/* PAGE  */

/*--------------------------------------------------
 * ptoa_rtn()
 *
 * INPUT:   packed:  pointer to an array of IBM packed digits
 *          asc_buf: pointer to integer for conversion result
 *          length:  number of bytes to convert
 *          null_term: flag to tell whether or not to NULL terminate
 *                     the string.
 *
 * OUTPUT:   0:     conversion complete without error
 *          -X:     some error occurred
 *
 * Description:
 *
 *      Convert an array of IBM packed digits (packed BCD like)
 *      to a NULL (0x00) terminated ASCII string, the number
 *      of bytes is determined by the length parameter.
 *
 --------------------------------------------------*/


int ptoa_rtn(unsigned char *packed, char *asc_buf, int length, int null_term) {

    int i, a, offset = 0, nib, err_flag = 0, digit, strip = 1;

    do {
        if (length == 0) {
            err_flag = cerr_info[ERR_INVALID_PACKED_LEN].err_code;
            break;
        }

        switch(packed[length - 1] & 0x0F) {
            case 0xC:      /* Postive Number */
            case 0xF:
                break;
            case 0xD:       /* negative number */
                asc_buf[offset] = '-';
                ++offset;
                break;
            default:
                err_flag = cerr_info[ERR_INVALID_SIGN].err_code;
        }
        if (err_flag) {
            break;
        }

        for (a = offset, i=0, nib = 0; nib < (length * 2) - 1; ++nib) {
            if (nib & 1) {
                digit = packed[i] & 0x0F;
                ++i;
            }
            else {
                digit = (packed[i] & 0xF0) >> 4;
            }
            if (strip && (digit == 0)) {
                continue;
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
                    asc_buf[a++] = digit + '0';
                    strip = 0;
                    break;
                default:
                        err_flag = cerr_info[ERR_INVALID_PACKED_NUM].err_code;
            }
        }
        if (strip) {
            asc_buf[a++] = '0';
        }
        if ( null_term ) {
            asc_buf[a] = 0;  /* NULL termination */
        }

    }while (0);

    if (err_flag) {
        asc_buf[0] = 0;
    }
    return(err_flag);
}

