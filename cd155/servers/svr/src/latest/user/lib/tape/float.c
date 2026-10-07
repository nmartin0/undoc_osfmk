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
 *	float.c 11.1 94/03/22 16:49:24
 */
static char	float_ver[] = "@(#) sourcefile float.c 11.1 94/03/22 16:49:24";

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

#include "convt.h"
#include "converr.h"


/*--------------------------------------------------
 * Istois()
 *
 * INPUT:   ibm_float:  pointer to an array of IBM single precision
 *                      floating point numbers to be converted.
 *          i_float:    pointer to an array of IEEE single precision
 *                      floating point numbers conversion storage.
 *          length:     number of IBM elements to convert
 *
 * OUTPUT:   0:     conversion complete without error
 *          -X:     if any of the conversions overflow or underflow
 *
 * Description:
 *
 *      Convert an array of IBM single precision floating
 *      point numbers to an array of IEEE single precision
 *      floating point numbers.  If the array pointers are
 *      equal then the IBM floats will be converted to IEEE
 *      floats in the same storage space.
 *
 *
 *      EXPONENTS:
 *
 *      IBM exponent can be in the range of -64 to +63 and is based
 *      sixteen.
 *
 *      IEEE exponent can be in the range of -127 to +128 and is based
 *      two.
 *
 *      IBM floating point numbers can be about twice as
 *      small and large as the IEEE floating point numbers, therefore
 *      an overflow or an underflow may occur.
 *
 *      On Overflow  the IEEE will be set to INFINITY (0x7F800000).
 *      On Underflow the IEEE will be set to ZERO     (0x00000000).
 *
 *
 *      FRACTION:
 *
 *      The IBM  fraction is 24 bits.
 *      The IEEE fraction is 23 bits, however the formula has an implied
 *      1.F so the precision of the fraction is really 24 bits.
 *
 *      No loss of precision will occur.
 *
 *
 *
 *      IBM Single Precision Float:
 *
 *          1     7        24      - Number of Bits
 *
 *          S  Exponent  Fraction
 *                |
 *                |
 *                |
 *                - Exponent is biased by 64 (base 16)
 *
 *      Numeric equation:  (s) 0.Fraction * (16**(Exponent - 64))
 *
 *
 *      IEEE Single Precision Float:
 *
 *          1      8          23    - Number of Bits
 *
 *          S   Exponent   Fraction
 *                 |
 *                 |
 *                 - Exponent is biased by 127 (base 2)
 *
 *      Numeric equation:  (s) 1.Fraction * ((2**(Exponent - 127))
 *
 --------------------------------------------------*/


int Istois(float *ibm_float, float *i_float, int length) {


    int i, exponent, return_val = 0;
    unsigned int temp;


	for (i=0; i<length; ++i) {

        temp ^= temp;  /* set temp to zero */

        ((char *)&temp)[0] = ((char *)(ibm_float + i))[3];
		((char *)&temp)[1] = ((char *)(ibm_float + i))[2];
		((char *)&temp)[2] = ((char *)(ibm_float + i))[1];

		/*--------------------------------------------------
		 * Test for  Zero
		 --------------------------------------------------*/
        if (temp == 0) {
			i_float[i] = 0;
			continue;
		}

        /*--------------------------------------------------
         * If IBM is INFINITY set IEEE to INFINITY
         --------------------------------------------------*/

        if ( (*((int *)&ibm_float[i]) & 0xFFFFFF7F) == 0x0000007F) {
            *((int *)&i_float[i]) = 0x7F800000;
            continue;
        }

		/*--------------------------------------------------
		 * Convert the IBM exponent to the IEEE exponent
		 *
		 * ((IBM exponent - 64) * 4) + 127
		 *
		 *           equal to
		 *
		 *  4 * IBM exponent - (4 * 64) + 127
		 *          .
		 *          .
		 *  4 * IBM exponent - 129
		 *
		 --------------------------------------------------*/

		exponent = ((((char *)(ibm_float + i))[0] & 0x7F) * 4) - 129;

		/*--------------------------------------------------
		 * Normalize the IBM fraction to fit the 1.F IEEE
		 * formula. For every multiplication by 2, adjust
		 * the base 2 exponent by -1.
		 --------------------------------------------------*/

		while (!(temp & 0x01000000)) {

			temp <<= 1;  /* multiply fraction by 2 */
			--exponent;  /* and subtract 1 from the base 2 exponent */
		}

		if ((unsigned int)exponent > 0xFF) {

			if (exponent < 0) {
				i_float[i] = 0;
                return_val = cerr_info[ERR_UNDERFLOW].err_code;
			}
			else {
                i_float[i] = 0x7F800000;  /* Infinity */
                return_val = cerr_info[ERR_OVERFLOW].err_code;
			}
			continue;
		}
        ((char *)&temp)[3] = (unsigned char)exponent;

		temp >>= 1; /* adjust float to make room for the sign */
		((char *)&temp)[3] |= (((char *)(ibm_float + i))[0]) & 0x80; /* or in the sign */

        *( (unsigned int *)&i_float[i]) = temp;
	}

    return (return_val);

}

/* PAGE  */

/*--------------------------------------------------
 * Istoid()
 *
 * INPUT:   ibm_float:  pointer to an array of IBM single precision
 *                      floating point numbers to be converted.
 *          i_double:   pointer to an array of IEEE double precision
 *                      floating point numbers conversion storage.
 *          length:     number of IBM elements to convert
 *
 * OUTPUT:   NONE
 *
 * Description:
 *
 *      Convert an array of IBM single precision floating
 *      point numbers to an array of IEEE double precision
 *      floating point numbers.
 *
 *      IBM single precision is 32 bits
 *      IEEE double precision is 64 bits, therefore different
 *      arrays must be used for the conversion.  Conversion in
 *      place can not be performed.
 *
 *      EXPONENTS:
 *
 *      IBM exponent can be in the range of -64 to +63 and is based
 *      sixteen.
 *
 *      IEEE exponent can be in the range of -1023 to +1024 and is based
 *      two.
 *
 *      Therefore no overflow or underflow will occur.
 *
 *
 *
 *      FRACTION:
 *
 *      The IBM  fraction is 24 bits.
 *      The IEEE fraction is 52 bits, formula has an implied
 *      1.F so the precision of the fraction is really 53 bits.
 *
 *      No loss of precision will occur.
 *
 *
 *      IBM Single Precision Float:
 *
 *          1     7        24      - Number of Bits
 *
 *          S  Exponent  Fraction
 *                |
 *                |
 *                |
 *                - Exponent is biased by 64 (base 16)
 *
 *      Numeric equation:  (s) 0.Fraction * (16**(Exponent - 64))
 *
 *
 *      IEEE Double Precision Float:
 *
 *          1      11         52    - Number of Bits
 *
 *          S   Exponent   Fraction
 *                 |
 *                 |
 *                 - Exponent is biased by 1023 (base 2)
 *
 *      Numeric equation:  (s) 1.Fraction * ((2**(Exponent - 1023))
 *
 --------------------------------------------------*/


void Istoid(float *ibm_float, double *i_double, int length) {


    int i, exponent;
    unsigned int temp;


	for (i=0; i<length; ++i) {

        temp ^= temp;  /* set temp to zero */

        ((char *)&temp)[0] = ((char *)(ibm_float + i))[3];
		((char *)&temp)[1] = ((char *)(ibm_float + i))[2];
		((char *)&temp)[2] = ((char *)(ibm_float + i))[1];

		/*--------------------------------------------------
		 * Test for  Zero
		 --------------------------------------------------*/
        if (temp == 0) {
            i_double[i] = 0;
			continue;
		}
        /*--------------------------------------------------
         * If IBM is INFINITY set IEEE to INFINITY
         --------------------------------------------------*/

        if ( ((((int *)(ibm_float + i))[0]) & 0xFFFFFF7F) == 0x0000007F) {
            ((int *)i_double + i)[1] = 0x7FF00000;
            ((int *)i_double + i)[0] = 0;
            continue;
        }

		/*--------------------------------------------------
		 * Convert the IBM exponent to the IEEE exponent
		 *
         * ((IBM exponent - 64) * 4) + 1023
		 *
		 *           equal to
		 *
         *  4 * IBM exponent - (4 * 64) + 1023
		 *          .
		 *          .
         *  4 * IBM exponent + 767
		 *
		 --------------------------------------------------*/

        exponent = ((((char *)(ibm_float + i))[0] & 0x7F) * 4) + 767;

		/*--------------------------------------------------
		 * Normalize the IBM fraction to fit the 1.F IEEE
		 * formula. For every multiplication by 2, adjust
		 * the base 2 exponent by -1.
		 --------------------------------------------------*/

		while (!(temp & 0x01000000)) {

			temp <<= 1;  /* multiply fraction by 2 */
			--exponent;  /* and subtract 1 from the base 2 exponent */
		}

        i_double[i] = 0;  /* init result to zero */

        /*--------------------------------------------------
         * Store the exponent portion
         --------------------------------------------------*/
        ((short *)(i_double + i))[3] = (short)((exponent & 0x7FF) << 4);

        /*--------------------------------------------------
         * Store the Fractional portion
         --------------------------------------------------*/
        ((int *)(i_double + i))[1] |= (temp & 0x00FFFFFF) >> 4;
        ((char *)(i_double + i))[3] = (char)((temp & 0x0F) << 4);

        /*--------------------------------------------------
         * Or in the sign
         --------------------------------------------------*/
        ((char *)(i_double + i))[7] |= (((char *)(ibm_float + i))[0]) & 0x80;
	}

    return;
}

/* PAGE  */

/*--------------------------------------------------
 * Idtoid()
 *
 * INPUT:   ibm_double: pointer to an arrya of IBM double precision
 *                      floating point numbers
 *          i_double:   pointer to an array of IEEE double precision
 *                      floating point numbers to be converted.
 *          length:     number of IBM elements to convert
 *
 * OUTPUT:   0:     conversion complete without error
 *          -X:     loss of fraction precision occurred.
 *
 *
 * Description:
 *
 *      Convert an array of IBM double precision floating
 *      point numbers to an array of IEEE double precision
 *      floating point numbers.
 *      IBM double precision is 64 bits
 *      IEEE double precision is 64 bits, therefore.  Conversion in
 *      place can be performed.
 *
 *
 *      EXPONENTS:
 *
 *      IBM exponent can be in the range of -64 to +63 and is based
 *      sixteen.
 *
 *      IEEE exponent can be in the range of -1023 to +1024 and is based
 *      two.
 *
 *      No exponent OVERFLOW or UNDERFLOW errors can occur.
 *
 *      FRACTION:
 *
 *      The IBM  fraction is 56 bits.
 *      The IEEE fraction is 52 bits, however the formula has an implied
 *      1.F so the precision of the fraction is really 53 bits.
 *
 *      A loss of precision error can occur.
 *
 *
 *      IBM Double Precision Float:
 *
 *          1     7        56      - Number of Bits
 *
 *          S  Exponent  Fraction
 *                |
 *                |
 *                |
 *                - Exponent is biased by 64 (base 16)
 *
 *      Numeric equation:  (s) 0.Fraction * (16**(Exponent - 64))
 *
 *
 *      IEEE Double Precision Float:
 *
 *          1      11         52    - Number of Bits
 *
 *          S   Exponent   Fraction
 *                 |
 *                 |
 *                 - Exponent is biased by 1023 (base 2)
 *
 *      Numeric equation:  (s) 1.Fraction * ((2**(Exponent - 1023))
 *
 --------------------------------------------------*/


int Idtoid(double *ibm_double, double *i_double, int length) {


    int i, exponent, shift, return_val = 0;
    esize_t temp_ieee;
    unsigned char sav_bits;


	for (i=0; i<length; ++i) {

        /*--------------------------------------------------
         * If IBM is ZERO set IEEE to ZERO
         --------------------------------------------------*/

        if (ibm_double[i] == 0) {
            i_double[i] = 0;
            continue;
        }

        /*--------------------------------------------------
         * If IBM is INFINITY set IEEE to INFINITY
         --------------------------------------------------*/

        if ( (((((int *)(ibm_double + i))[0]) & 0xFFFFFF7F) == 0x0000007F) &&
                   (((int *)(ibm_double + i))[1] == 0)  ) {
            ((int *)i_double + i)[1] = 0x7FF00000;
            ((int *)i_double + i)[0] = 0;
            continue;
        }

        /*--------------------------------------------------
         * Get the exponent, IBM bits 62 - 56 of the IBM
         * double (bits 7 - 0 of the first byte).
         *
		 * Convert the IBM exponent to the IEEE exponent
		 *
         * ((IBM exponent - 64) * 4) + 1023
		 *
		 *           equal to
		 *
         *  4 * IBM exponent - (4 * 64) + 1023
		 *          .
		 *          .
         *  4 * IBM exponent + 767
		 *
		 --------------------------------------------------*/

        exponent = ((((char *)(ibm_double + i))[0] & 0x7F) * 4) + 767;

        if ((unsigned)exponent > 0x7FF) {
           if (exponent < 0) {
                return_val = cerr_info[ERR_UNDERFLOW].err_code;
                i_double[i] = 0;
           }
           else {
                return_val = cerr_info[ERR_OVERFLOW].err_code;
                ((int *)i_double + i)[1] = 0x7FF00000;
                ((int *)i_double + i)[0] = 0;
           }
           continue;
        }

        /*-------------------------------------------------
         * Get the fraction portion of the IBM double
         * bits 55 - 0. The IBM bytes have to be swapped
         * to fit the IEEE format.  The bytes have to be
         * arranged so that the 56 bit fractions is aligned
         * into the IEEE at bit 51 and below.  The last 4
         * bits of the IBM fraction is saved and used to
         * shift into the IEEE word when the 1.F alignment
         * takes place.
         --------------------------------------------------*/

        temp_ieee.shigh ^= temp_ieee.shigh;  /* clear temp_ieee.shigh */

        ((char *)&temp_ieee.shigh)[1] = ((char *)(ibm_double + i))[1];
        ((char *)&temp_ieee.shigh)[0] = ((char *)(ibm_double + i))[2];
        temp_ieee.shigh <<= 4;
        ((char *)&temp_ieee.shigh)[0] |=
                ((((unsigned char *)(ibm_double + i))[3] & 0xF0) >> 4);

        ((char *)&temp_ieee.slow)[3] = ((char *)(ibm_double + i))[4];
        ((char *)&temp_ieee.slow)[2] = ((char *)(ibm_double + i))[5];
        ((char *)&temp_ieee.slow)[1] = ((char *)(ibm_double + i))[6];
        ((char *)&temp_ieee.slow)[0] = ((char *)(ibm_double + i))[7];

        sav_bits = (((unsigned char *)&temp_ieee.slow)[0] & 0x0F) << 4;
        temp_ieee.slow >>= 4;
        ((char *)&temp_ieee.slow)[3] =
                (((char *)&temp_ieee.slow)[3] & 0x0F) |
                    ((((char *)(ibm_double + i))[3] & 0x0F) << 4) ;

		/*--------------------------------------------------
		 * Normalize the IBM fraction to fit the 1.F IEEE
		 * formula. For every multiplication by 2, adjust
		 * the base 2 exponent by -1.
		 --------------------------------------------------*/

        while (!(temp_ieee.shigh & 0x00100000)) {

            temp_ieee.shigh <<= 1;  /* multiply fraction by 2 */
            if (temp_ieee.slow & 0x80000000) {
                temp_ieee.shigh |= 1;
            }
            temp_ieee.slow <<= 1;
            if (sav_bits & 0x80) {
                temp_ieee.slow |= 1;
                sav_bits <<= 1;
            }
			--exponent;  /* and subtract 1 from the base 2 exponent */
		}
        if (sav_bits) {
            return_val = cerr_info[ERR_LOSS_OF_PRECISION].err_code;
        }

        /*--------------------------------------------------
         * Set the sign bit if the IBM sign bit is on.
         --------------------------------------------------*/

        if ( (((char *)(ibm_double + i))[0]) & 0x80) {
            exponent |= 0x0800;
        }

        ((short *)&temp_ieee.shigh)[1] =
                (((short *)&temp_ieee.shigh)[1] & 0x000F) | (exponent << 4);

        /*--------------------------------------------------
         * Store the result into the IEEE array location
         --------------------------------------------------*/

        ((int *)(i_double + i))[0] = temp_ieee.slow;
        ((int *)(i_double + i))[1] = temp_ieee.shigh;

	}
    return (return_val);
}

/* PAGE  */

/*--------------------------------------------------
 * istoIs()
 *
 * INPUT:   i_float:    pointer to an array of IEEE single precision
 *                      floating point numbers to be converted
 *          ibm_float:  pointer to an array of IBM single precision
 *                      floating point numbers.
 *          length:     number of IEEE elements to convert
 *
 * OUTPUT:   0:     Conversions completed with no errors
 *          -X:     If any loss in fraction precision occurs.
 *
 * Description:
 *
 *      Convert an array of IEEE single precision floating
 *      point numbers to an array of IBM single precision
 *      floating point numbers.  If the array pointers are
 *      equal then the IEEE floats will be converted to IBM
 *      floats in the same storage space.
 *
 *
 *      EXPONENTS:
 *
 *      IBM exponent can be in the range of -64 to +63 and is based
 *      sixteen.
 *
 *      IEEE exponent can be in the range of -127 to +128 and is based
 *      two.
 *
 *      IBM floating point numbers can be about twice as
 *      small and large as the IEEE floating point numbers, therefore
 *      no overflow or an underflow conditions will occur.
 *
 *
 *      FRACTION:
 *
 *      The IBM  fraction is 24 bits.
 *      The IEEE fraction is 23 bits, however the formula has an implied
 *      1.F so the precision of the fraction is really 24 bits.
 *
 *      During the conversion process there can be a loss of fraction
 *      precision which will cause a non-zero return code to be returned.
 *
 *
 *
 *      IBM Single Precision Float:
 *
 *          1     7        24      - Number of Bits
 *
 *          S  Exponent  Fraction
 *                |
 *                |
 *                |
 *                - Exponent is biased by 64 (base 16)
 *
 *      Numeric equation:  (s) 0.Fraction * (16**(Exponent - 64))
 *
 *
 *      IEEE Single Precision Float:
 *
 *          1      8          23    - Number of Bits
 *
 *          S   Exponent   Fraction
 *                 |
 *                 |
 *                 - Exponent is biased by 127 (base 2)
 *
 *      Numeric equation:  (s) 1.Fraction * ((2**(Exponent - 127))
 *
 --------------------------------------------------*/


int istoIs(float *i_float, float *ibm_float, int length) {


    int i, exponent, return_val = 0;
    unsigned int temp_ibm, temp;


	for (i=0; i<length; ++i) {

        /*--------------------------------------------------
         * If IEEE is ZERO set IBM to ZERO
         --------------------------------------------------*/

        if (i_float[i] == 0) {
            ibm_float[i] = 0;
            continue;
        }

        /*--------------------------------------------------
         * If IEEE is INFINITY set IBM to INFINITY
         --------------------------------------------------*/

        if ( ((*((int *)&i_float[i])) & 0x7FFFFFFF) == 0x7F800000 ) {
            ibm_float[i] = 0x0000007F;  /* INFINITY */
            continue;
        }

        /*--------------------------------------------------
         * Get the exponent, bits 30 - 23 of the IEEE.
         * shift left 1 to align on a byte boundary.
         * Put the upper 8 bits into the exponent variable.
         *
         --------------------------------------------------*/

        temp = (*((int *)&i_float[i]) & 0x7F800000) << 1;
        exponent = (unsigned char)(((char *)&temp)[3]);


        /*-------------------------------------------------
         * Get the fraction portion of the IEEE lower 23
         * bits and add in the implied 1.F formula bit then
         * convert to the IBM 0.F formula.
         *
         * This effectively divides the fraction by two so
         * the exponent must be incremented by 1.
         *
         * Then subtract the 127 bias from the exponent.
         *
         * exponent + 1 - 127 = exponent - 126
         --------------------------------------------------*/

        temp_ibm = ( *((int *)&i_float[i]) & 0x007FFFFF) | 0x00800000;
        exponent -= 126;

        /*--------------------------------------------------
         * If exponent mod 4 is true then the exponent must
         * be adjusted so it can be converted to base 16.
         *
         *
         * Divide the fraction by two (<<1) until the base 2
         * exponent can be converted to a base 16 exponent.
         * For every shift of the fraction the exponent needs
         * to be adjusted by 1 (x2 adjustment).
         *
         * exponent & 3 is the same as mod-4
         --------------------------------------------------*/

        while (exponent & 3) {
            if (temp_ibm & 1) {
                return_val = cerr_info[ERR_LOSS_OF_PRECISION].err_code;
            }
            temp_ibm >>= 1;
            ++exponent;
        }

        /*--------------------------------------------------
         * Adjust the exponent to base 16 and add in the
         * IBM exponent bias of 64
         --------------------------------------------------*/

        exponent = (exponent >> 2) + 64;

        /*--------------------------------------------------
         * Set the sign bit if the IEEE sign bit is on.
         --------------------------------------------------*/

        if ( *((int *)&i_float[i]) & 0x80000000 ) {
            exponent |= 0x0080;
        }

        /*--------------------------------------------------
         * Byte swap from the temp_ibm variable to the
         * ibm_float user array.
         --------------------------------------------------*/

        ((char *)(ibm_float + i))[0] = (char)exponent;
        ((char *)(ibm_float + i))[1] = ((char *)&temp_ibm)[2];
        ((char *)(ibm_float + i))[2] = ((char *)&temp_ibm)[1];
        ((char *)(ibm_float + i))[3] = ((char *)&temp_ibm)[0];

	}

    return (return_val);
}

/* PAGE  */

/*--------------------------------------------------
 * idtoIs()
 *
 * INPUT:   i_double:   pointer to an array of IEEE double precision
 *                      floating point numbers
 *          ibm_float:  pointer to an array of IBM single precision
 *                      floating point numbers to be converted.
 *          length:     number of IEEE elements to convert
 *
 * OUTPUT:   0:     conversion complete without error
 *          -X:     if any of the conversions overflow or underflow
 *                  a IBM single precision float or any precision is
 *                  lost in the Fraction portion of the number.
 *
 *
 * Description:
 *
 *      Convert an array of IEEE double precision floating
 *      point numbers to an array of IBM single precision
 *      floating point numbers.
 *      IBM single precision is 32 bits
 *      IEEE double precision is 64 bits, therefore different
 *      arrays must be used for the conversion.  Conversion in
 *      place can not be performed.
 *
 *
 *      EXPONENTS:
 *
 *      IBM exponent can be in the range of -64 to +63 and is based
 *      sixteen.
 *
 *      IEEE exponent can be in the range of -1023 to +1024 and is based
 *      two.
 *
 *      Both overflow and underflow errors can occur.
 *
 *
 *
 *      FRACTION:
 *
 *      The IBM  fraction is 24 bits.
 *      The IEEE fraction is 52 bits, however the formula has an implied
 *      1.F so the precision of the fraction is really 53 bits.
 *
 *
 *      Loss of precision on the fraction portion can occur.
 *
 *
 *
 *      IBM Single Precision Float:
 *
 *          1     7        24      - Number of Bits
 *
 *          S  Exponent  Fraction
 *                |
 *                |
 *                |
 *                - Exponent is biased by 64 (base 16)
 *
 *      Numeric equation:  (s) 0.Fraction * (16**(Exponent - 64))
 *
 *
 *      IEEE Double Precision Float:
 *
 *          1      11         52    - Number of Bits
 *
 *          S   Exponent   Fraction
 *                 |
 *                 |
 *                 - Exponent is biased by 1023 (base 2)
 *
 *      Numeric equation:  (s) 1.Fraction * ((2**(Exponent - 1023))
 *
 --------------------------------------------------*/


int idtoIs(double *i_double, float *ibm_float, int length) {


    int i, exponent, return_val = 0;
    unsigned int temp_ibm;


	for (i=0; i<length; ++i) {

        /*--------------------------------------------------
         * If IEEE is ZERO set IBM to ZERO
         --------------------------------------------------*/

        if (i_double[i] == 0) {
            ibm_float[i] = 0;
            continue;
        }

        /*--------------------------------------------------
         * If IEEE is INFINITY set IBM to INFINITY
         --------------------------------------------------*/

        if ( (((((int *)(i_double + i))[1]) & 0x7FFFFFFF) == 0x7FF00000) &&
                   (((int *)(i_double + i))[0] == 0)  ) {
            ibm_float[i] = 0x0000007F;  /* INFINITY */
            continue;
        }

        /*--------------------------------------------------
         * Get the exponent, bits 62 - 52 of the IEEE double.
         * Shift right 4 to align into a 16bit word.
         *
         --------------------------------------------------*/

        exponent = ((((short *)(i_double + i))[3]) & 0x7FF0) >> 4;


        /*-------------------------------------------------
         * Get the fraction portion of the IEEE double
         * bits 51 - 29 and or in the implied 1.F formula bit
         * to convert to the IBM 0.F formula.
         *
         * This effectively divides the fraction by two so
         * the exponent must be incremented by 1.
         *
         * Then subtract the 1023 bias from the exponent.
         *
         * exponent + 1 - 1023 = exponent - 1022
         --------------------------------------------------*/

        temp_ibm = ((((int *)(i_double + i))[1]) & 0x000FFFFF) | 0x00100000;
        temp_ibm = (temp_ibm << 3) |
                    (((((unsigned char *)(i_double + i))[3]) & 0xE0) >> 5);

        if ( ((((int *)(i_double + i))[0]) & 0x1FFFFFFF) != 0) {
            return_val = cerr_info[ERR_LOSS_OF_PRECISION].err_code;
        }

        exponent -= 1022;

        /*--------------------------------------------------
         * Adjust the IEEE base 2 exponent until it can be
         * converted to an IBM base 16 exponent.
         *
         * If the least significant bit of the fraction is
         * on when a shift right must be performed then a
         * loss of precision on the fraction is flagged.
         --------------------------------------------------*/

        while (exponent & 3) {

            if (temp_ibm & 1) {
                return_val = cerr_info[ERR_LOSS_OF_PRECISION].err_code;
            }
            temp_ibm >>= 1;
            ++exponent;
        }

        /*--------------------------------------------------
         * Adjust the exponent to base 16 and add in the
         * IBM exponent bias of 64
         --------------------------------------------------*/

        exponent = (exponent >> 2) + 64;

        if ((unsigned)exponent > 0x7F) {
           if (exponent < 0) {
                return_val = cerr_info[ERR_UNDERFLOW].err_code;
                ibm_float[i] = 0;
           }
           else {
                return_val = cerr_info[ERR_OVERFLOW].err_code;
                ibm_float[i] = 0x0000007F;  /* INFINITY */
           }
           continue;
        }


        /*--------------------------------------------------
         * Set the sign bit if the IEEE sign bit is on.
         --------------------------------------------------*/

        if ( (((int *)(i_double + i))[1]) & 0x80000000) {
            exponent |= 0x0080;
        }

        /*--------------------------------------------------
         * Byte swap from the temp_ibm variable to the
         * ibm_float user array.
         --------------------------------------------------*/

        ((char *)(ibm_float + i))[0] = (char)exponent;
        ((char *)(ibm_float + i))[1] = ((char *)&temp_ibm)[2];
        ((char *)(ibm_float + i))[2] = ((char *)&temp_ibm)[1];
        ((char *)(ibm_float + i))[3] = ((char *)&temp_ibm)[0];

	}
    return (return_val);
}

/* PAGE  */

/*--------------------------------------------------
 * idtoId()
 *
 * INPUT:   i_double:   pointer to an arrya of IEEE double precision
 *                      floating point numbers
 *          ibm_double: pointer to an array of IBM double precision
 *                      floating point numbers to be converted.
 *          length:     number of IEEE elements to convert
 *
 * OUTPUT:   0:     conversion complete without error
 *          -X:
 *
 *
 * Description:
 *
 *      Convert an array of IEEE double precision floating
 *      point numbers to an array of IBM double precision
 *      floating point numbers.
 *      IBM double precision is 64 bits
 *      IEEE double precision is 64 bits, therefore.  Conversion in
 *      place can be performed.
 *
 *
 *      EXPONENTS:
 *
 *      IBM exponent can be in the range of -64 to +63 and is based
 *      sixteen.
 *
 *      IEEE exponent can be in the range of -1023 to +1024 and is based
 *      two.
 *
 *      Both overflow and underflow errors can occur.
 *
 *
 *      FRACTION:
 *
 *      The IBM  fraction is 56 bits.
 *      The IEEE fraction is 52 bits, however the formula has an implied
 *      1.F so the precision of the fraction is really 53 bits.
 *
 *      No loss of precision will occur.
 *
 *
 *      IBM Double Precision Float:
 *
 *          1     7        56      - Number of Bits
 *
 *          S  Exponent  Fraction
 *                |
 *                |
 *                |
 *                - Exponent is biased by 64 (base 16)
 *
 *      Numeric equation:  (s) 0.Fraction * (16**(Exponent - 64))
 *
 *
 *      IEEE Double Precision Float:
 *
 *          1      11         52    - Number of Bits
 *
 *          S   Exponent   Fraction
 *                 |
 *                 |
 *                 - Exponent is biased by 1023 (base 2)
 *
 *      Numeric equation:  (s) 1.Fraction * ((2**(Exponent - 1023))
 *
 --------------------------------------------------*/


int idtoId(double *i_double, double *ibm_double, int length) {


    int i, exponent, shift, return_val = 0;
    esize_t temp_ibm;


	for (i=0; i<length; ++i) {

        /*--------------------------------------------------
         * If IEEE is ZERO set IBM to ZERO
         --------------------------------------------------*/

        if (i_double[i] == 0) {
            ibm_double[i] = 0;
            continue;
        }

        /*--------------------------------------------------
         * If IEEE is INFINITY set IBM to INFINITY
         --------------------------------------------------*/

        if ( (((((int *)(i_double + i))[1]) & 0x7FFFFFFF) == 0x7FF00000) &&
                   (((int *)(i_double + i))[0] == 0)  ) {
            ((int *)ibm_double + i)[0] = 0x0000007F;
            ((int *)ibm_double + i)[1] = 0;
            continue;
        }

        /*--------------------------------------------------
         * Get the exponent, bits 62 - 52 of the IEEE double.
         * Shift right 4 to align into a 16bit word.
         *
         --------------------------------------------------*/

        exponent = ((((short *)(i_double + i))[3]) & 0x7FF0) >> 4;


        /*-------------------------------------------------
         * Get the fraction portion of the IEEE double
         * bits 51 - 0 and or in the implied 1.F formula bit
         * to convert to the IBM 0.F formula.
         *
         * This effectively divides the fraction by two so
         * the exponent must be incremented by 1.
         *
         * Then subtract the 1023 bias from the exponent.
         *
         * exponent + 1 - 1023 = exponent - 1022
         --------------------------------------------------*/

        temp_ibm.shigh = ((((int *)(i_double + i))[1]) & 0x000FFFFF) | 0x00100000;
        temp_ibm.shigh = (temp_ibm.shigh << 3) |
                    (((((unsigned char *)(i_double + i))[3]) & 0xE0) >> 5);
        shift = 3;

        exponent -= 1022;

        /*--------------------------------------------------
         * Adjust the IEEE base 2 exponent until it can be
         * converted to an IBM base 16 exponent.
         *
         * Maintain the shift value for the final shift on
         * the low order 32 bits.
         --------------------------------------------------*/

        while (exponent & 3) {

            temp_ibm.shigh >>= 1;
            ++exponent;
            --shift;
        }

        /*--------------------------------------------------
         * Adjust the exponent to base 16 and add in the
         * IBM exponent bias of 64
         --------------------------------------------------*/

        exponent = (exponent >> 2) + 64;

        if ((unsigned)exponent > 0x7F) {
           if (exponent < 0) {
                return_val = cerr_info[ERR_UNDERFLOW].err_code;
                ibm_double[i] = 0;
           }
           else {
                return_val = cerr_info[ERR_OVERFLOW].err_code;
                ((int *)ibm_double + i)[0] = 0x0000007F;
                ((int *)ibm_double + i)[1] = 0;
           }
           continue;
        }


        /*--------------------------------------------------
         * Set the sign bit if the IEEE sign bit is on.
         --------------------------------------------------*/

        if ( (((int *)(i_double + i))[1]) & 0x80000000) {
            exponent |= 0x0080;
        }

        /*--------------------------------------------------
         * Now do the final shift on the LS 32 bit word
         --------------------------------------------------*/

        temp_ibm.slow = ((int *)(i_double + i))[0] << (3 - shift);

        /*--------------------------------------------------
         * Byte swap from the temp_ibm variable to the
         * ibm_float user array.
         --------------------------------------------------*/

        ((char *)(ibm_double + i))[0] = (char)exponent;
        ((char *)(ibm_double + i))[1] = ((char *)&temp_ibm.shigh)[2];
        ((char *)(ibm_double + i))[2] = ((char *)&temp_ibm.shigh)[1];
        ((char *)(ibm_double + i))[3] = ((char *)&temp_ibm.shigh)[0];
        ((char *)(ibm_double + i))[4] = ((char *)&temp_ibm.slow)[3];
        ((char *)(ibm_double + i))[5] = ((char *)&temp_ibm.slow)[2];
        ((char *)(ibm_double + i))[6] = ((char *)&temp_ibm.slow)[1];
        ((char *)(ibm_double + i))[7] = ((char *)&temp_ibm.slow)[0];

	}
    return (return_val);
}

