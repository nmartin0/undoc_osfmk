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
 *      eitoz.c 11.1 94/03/22 16:50:20
 */
static char     eitoz_ver[] = "@(#) sourcefile eitoz.c 11.1 94/03/22 16:50:20";

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
 * eitoz()
 *
 * INPUT:   eint_var:   extended integer (esize_t) to be converted
 *          zoned:      pointer to buffer for the zoned result
 *          zone_size:  size of the zoned array in bytes
 *
 * OUTPUT:  int:     0 if conversion OK
 *                   negative if an ERROR occurs
 *
 * Description:
 *
 *      Convert an intel 64 bit extended integer (esize_t) into
 *      an array of IBM zoned digits. The zone array size
 *      parameter indicates the number of zoned digits to
 *      be used for the conversion.  The zoned array will
 *      have leading zeros (0xF0) if the integer is smaller
 *      than the size of the zoned array.  If the integer value
 *      will overflow the number of digits in the zoned
 *      array then a negative value will be returned.
 *
 *      The extended integer number must be less than 2**61.
 *
 *
 --------------------------------------------------*/

#include <stdio.h>
#include "converr.h"
#include "convt.h"


int eitoz(esize_t eint, unsigned char zone[], int zone_size) {

    int sign = 0, return_val = 0, len;
    char asc[40], *p;

    p = asc;

    do {
        if (zone_size > 0) {
            if (eint.shigh || eint.slow) {

                if ( _etos(eint, asc) ) {
                    return_val = cerr_info[ERR_OVERFLOW].err_code;
                    break;
                }
                /*--------------------------------------------------
                 *  Set the sign flag if the number is negative
                 *  and advance pointer past the '-' sign
                 --------------------------------------------------*/

                if ( *p == '-' ) {
                    sign = 1;
                    ++p;
                }

                /*--------------------------------------------------
                 *  Check to ensure that the zone array is large
                 *  enough.
                 --------------------------------------------------*/

                if ( (len = strlen(p)) > zone_size ) {
                    return_val = cerr_info[ERR_OVERFLOW_ZONE_LEN].err_code;
                    break;
                }

                /*---------------------------------------------------
                 * Work through the asc buffer backwards, do the
                 * last digit first and include the sign digit.
                 --------------------------------------------------*/

                if (sign) {
                    zone[--zone_size] = 0xD0;
                }
                else {
                    zone[--zone_size] = 0xF0;
                }
                zone[zone_size] |= (p[--len] & 0x0F);


                /*--------------------------------------------------
                 * Convert the rest of the asc buffer to zoned
                 * digits.
                 --------------------------------------------------*/

                while (len) {
                    zone[--zone_size] = (p[--len] | 0xF0);
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

