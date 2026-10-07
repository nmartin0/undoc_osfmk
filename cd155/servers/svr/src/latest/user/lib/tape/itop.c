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
 *	itop.c 11.1 94/03/22 16:49:28
 */
static char	itop_ver[] = "@(#) sourcefile itop.c 11.1 94/03/22 16:49:28";

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
 * itop()
 *
 * INPUT:   int_var:        integer to be converted
 *          packed:         pointer to buffer for the packed result
 *          packed_length:  length of the packed array in bytes
 *
 * OUTPUT:  int:     0 if conversion OK
 *                   negative if an OVERFLOW occurs
 *
 * Description:
 *
 *      Convert an intel 32 bit integer (signed) into an array
 *      of IBM packed digits. The packed array length parameter
 *      indicates the number of packed digits to be used for
 *      the conversion.  The packed array will have leading
 *      zeros (0) if the integer is smaller than the
 *      size of the packed array.  If the integer value
 *      will overflow the number of digits in the packed
 *      array then a negative value will be returned.
 *
 *
 --------------------------------------------------*/

#include "converr.h"


int itop(int intel_int, unsigned char packed[], int packed_length) {

    int return_val = 0, nib = 0;

	do {
		if (packed_length > 0) {
			if(intel_int) {
				if (intel_int < 0) {
                    packed[packed_length-1] = 0x0D;
					intel_int = ~intel_int + 1;
				}
				else {
                    packed[packed_length-1] = 0x0C;
				}

				++nib;

                /*--------------------------------------------------
				 * Convert the integer to packed digits.
				 * If the number of digits exceed the size of the
				 * packed array then return an OVERFLOW error
				 * condition.
				 --------------------------------------------------*/

				while (intel_int && packed_length) {
					if (nib & 1) {
                        packed[packed_length-1] |=
							((((unsigned int)intel_int) % 10) << 4);
						--packed_length;
					}
					else {
                        packed[packed_length-1] = intel_int % 10;
					}
					++nib;
					intel_int = ((unsigned int)intel_int) / 10;
				}
				if (intel_int) {
                    return_val = cerr_info[ERR_OVERFLOW_PACKED_LEN].err_code;
					break;
                }

                /*--------------------------------------------------
                 * Add leading 0's
                 --------------------------------------------------*/
                if (nib & 1) {
                    --packed_length;
                }

                while (packed_length > 0) {
                    packed[packed_length-1] = 0;
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

