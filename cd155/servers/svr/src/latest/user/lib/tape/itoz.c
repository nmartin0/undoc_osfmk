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
 *	itoz.c 11.1 94/03/22 16:49:29
 */
static char	itoz_ver[] = "@(#) sourcefile itoz.c 11.1 94/03/22 16:49:29";

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
 * itoz()
 *
 * INPUT:   int_var:    integer to be converted
 *          zoned:      pointer to buffer for the zoned result
 *          zone_size:  size of the zoned array in bytes
 *
 * OUTPUT:  int:     0 if conversion OK
 *                   negative if an OVERFLOW occurs
 *
 * Description:
 *
 *      Convert an intel 32 bit integer (signed) into an array
 *      of IBM zoned digits. The zone array size parameter
 *      indicates the number of zoned digits to be used for
 *      the conversion.  The zoned array will have leading
 *      zeros (0xF0) if the integer is smaller than the
 *      size of the zoned array.  If the integer value
 *      will overflow the number of digits in the zoned
 *      array then a negative value will be returned.
 *
 *
 --------------------------------------------------*/

#include "converr.h"


int itoz(int intel_int, unsigned char zone[], int zone_size) {

    int return_val = 0;

    do {
        if (zone_size > 0) {
            if(intel_int) {
                if (intel_int < 0) {
                    intel_int = ~intel_int + 1;  /* 2's Complement */
                    zone[--zone_size] = 0xD0;
                }
                else {
                    zone[--zone_size] = 0xF0;
                }
                zone[zone_size] |= (((unsigned int)intel_int) % 10);
                intel_int = ((unsigned int)intel_int) / 10;

                /*--------------------------------------------------
                 * Convert the integer to zoned digits (abs val).
                 * If the number of digits exceed the size of the
                 * zoned array then return an OVERFLOW error
                 * condition.
                 --------------------------------------------------*/

                while (intel_int && zone_size) {
                    zone[--zone_size] = ((((unsigned int)intel_int) % 10) | 0xF0);
                    intel_int = ((unsigned int)intel_int) / 10;
                }
                if (intel_int) {
                    return_val = cerr_info[ERR_OVERFLOW_ZONE_LEN].err_code;
                    break;
                }

                /*--------------------------------------------------
                 * Add leading 0's (0xF0)
                 --------------------------------------------------*/

                while (--zone_size >= 0)
                    zone[zone_size] = 0xF0;

            }
            else {   /* ZERO */
                while (zone_size--)
                    zone[zone_size] = 0xF0;
            }
        }
        else   /* zone size < 1 */
            return_val = cerr_info[ERR_INVALID_ZONED_LEN].err_code;

    } while (0);

    return(return_val);
}


