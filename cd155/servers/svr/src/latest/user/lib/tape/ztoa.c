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
 *	ztoa.c 11.1 94/03/22 16:49:54
 */
static char	ztoa_ver[] = "@(#) sourcefile ztoa.c 11.1 94/03/22 16:49:54";

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
 * ztoa() - 'C' slick for the ztoa conversion
 *
 * INPUT:   zoned:   pointer to an array of IBM zoned digits
 *          asc_buf: pointer to a buffer for the ASCII characters
 *          length:  number of zoned digits to convert
 *
 * OUTPUT:       0:  Conversion O.K.
 *              -X:  Error Occurred.
 *
 * Description:
 *
 *      Convert an array of IBM zoned digits (unpacked BCD like)
 *      to a NULL (0x00) terminated ASCII string.
 *
 --------------------------------------------------*/

int ztoa(unsigned char *zoned, char *asc_buf, int length) {

    /*--------------------------------------------------
     *  Call the ztoa routine with the 'null_term'
     *  parameter set to '1' to null terminate the
     *  string.
     --------------------------------------------------*/

    return ( ztoa_rtn(zoned, asc_buf, length, 1) );

}

/* PAGE  */


/*--------------------------------------------------
 * ztoa_rtn()
 *
 * INPUT:   zoned:   pointer to an array of IBM zoned digits
 *          asc_buf: pointer to a buffer for the ASCII characters
 *          length:  number of zoned digits to convert
 *          null_term: flag to tell whether or not to NULL terminate
 *                     the string.
 *
 * OUTPUT:       0:  Conversion O.K.
 *              -X:  Error Occurred.
 *
 * Description:
 *
 *      Convert an array of IBM zoned digits (unpacked BCD like)
 *      to a NULL (0x00) terminated ASCII string.
 *
 --------------------------------------------------*/



int ztoa_rtn(unsigned char *zoned, char *asc_buf, int length, int null_term) {

    int i, a, strip = 1, offset = 0, err_flag = 0;

    do {
        if (length == 0) {
            err_flag = cerr_info[ERR_INVALID_ZONED_LEN].err_code;
            break;
        }

        switch(zoned[length - 1] & 0xF0) {
            case 0xA0:      /* Postive Number */
            case 0xC0:
            case 0xE0:
            case 0xF0:
                break;
            case 0xB0:      /* Negative Number */
            case 0xD0:
                asc_buf[offset] = '-';
                ++offset;
                break;
            default:
                err_flag = cerr_info[ERR_INVALID_SIGN].err_code;
        }
        if (err_flag) {
            break;
        }
        zoned[length - 1] |= 0xF0;  /* make sign valid number */

        for (a = offset, i = 0; i < length; ++i) {
            switch(zoned[i]) {
                case 0xF0:
                    if (strip) {
                        continue;
                    }
                case 0xF1:
                case 0xF2:
                case 0xF3:
                case 0xF4:
                case 0xF5:
                case 0xF6:
                case 0xF7:
                case 0xF8:
                case 0xF9:
                    asc_buf[a++] = ((zoned[i] & 0x0F) + '0');
                    strip = 0;
                    break;
                default:
                    err_flag = cerr_info[ERR_INVALID_ZONED_NUM].err_code;
            }
            if (err_flag) {
                break;
            }
        }
        if (strip) {
            asc_buf[a++] = '0';
        }
        if ( null_term ) {
            asc_buf[a] = 0;  /* NULL termination */
        }

    } while(0);

    if (err_flag) {
        asc_buf = 0;
    }
    return(err_flag);
}

