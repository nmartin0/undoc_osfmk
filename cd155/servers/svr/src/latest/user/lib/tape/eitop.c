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
 *	eitop.c 11.1 94/03/22 16:50:19
 */
static char	eitop_ver[] = "@(#) sourcefile eitop.c 11.1 94/03/22 16:50:19";

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
 * eitop()
 *
 * INPUT:   eint:           extended integer (esize_t) to be converted
 *          packed:         pointer to buffer for the packed result
 *          packed_length:  length of the packed array in bytes
 *
 * OUTPUT:  int:     0 if conversion OK
 *                   negative if an ERROR occurs
 *
 * Description:
 *
 *      Convert an intel 64 bit extended integer (esize_t) into an array
 *      of IBM packed digits. The packed array length parameter
 *      indicates the number of bytes to be used for
 *      the conversion.  The packed array will have leading
 *      zeros (0) if the integer is smaller than the
 *      size of the packed array.  If the integer value
 *      will overflow the number of digits in the packed
 *      array then a negative value will be returned.
 *
 *      The extended integer number must be less than 2**61.
 *
 --------------------------------------------------*/

#include "convt.h"
#include "converr.h"


int eitop(esize_t eint, unsigned char packed[], int packed_length) {

    int return_val = 0, nib = 0, len, sign = 0;
    char asc[40], *p;

    p = asc;

	do {
		if (packed_length > 0) {
            if(eint.shigh || eint.slow) {

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
                 *  enough. Length of the extended integer string
                 *  divided by two plus 1 for the sign nibble
                 --------------------------------------------------*/

                if ( (((len = strlen(p)) / 2) + 1)  > packed_length ) {
                    return_val = cerr_info[ERR_OVERFLOW_PACKED_LEN].err_code;
                    break;
                }

                /*---------------------------------------------------
                 * Work through the asc buffer backwards, do the
                 * sign first then start with the last digit.
                 --------------------------------------------------*/

                if (sign) {
                    packed[--packed_length] = 0x0D;
                }
                else {
                    packed[--packed_length] = 0x0C;
                }
				++nib;

                /*--------------------------------------------------
                 * Convert the asc buffer to packed digits.
                 --------------------------------------------------*/

                while (len) {
					if (nib & 1) {
                        packed[packed_length] |= ((p[--len] & 0x0F) << 4);
						--packed_length;
					}
					else {
                        packed[packed_length] = (p[--len] & 0x0F);
					}
					++nib;
                }

                /*--------------------------------------------------
                 * Add leading 0's
                 --------------------------------------------------*/
                if (nib & 1) {
                    --packed_length;
                }

                while (packed_length >= 0) {
                    packed[packed_length] = 0;
                    --packed_length;
                }

            }
            else {   /* ZERO */
                packed[--packed_length] = 0x0C;  /* last digit has sign */
                while (packed_length--)
                    packed[packed_length] = 0;
            }
        }
        else   /* packed length < 1 */
            return_val = cerr_info[ERR_INVALID_PACKED_LEN].err_code;

    } while (0);

    return(return_val);
}

