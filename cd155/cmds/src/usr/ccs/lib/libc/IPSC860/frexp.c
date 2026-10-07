/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 

/*
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libc/IPSC860/frexp.c,v 1.4 1994/11/19 02:11:21 mtm Exp $
 *
 * 
 */

#include <errno.h>

typedef union 
{
	struct {
		unsigned fraction_low : 32 ;
		unsigned bits : 20 ;
		unsigned exponent : 11 ;
		unsigned sign     : 1 ;
	} inf_parts ;
	struct {
		unsigned fraction_low : 32 ;
		unsigned bits     : 19 ;
		unsigned qnan_bit : 1 ;
		unsigned exponent : 11 ;
		unsigned sign     : 1 ;
	} nan_parts ;
	double d ;
} dnan ;
	
double
frexp ( value , eptr )
double value ;  
register int * eptr ;
{
        register double absvalue ;
	double check_value;

	check_value = value;

	if (check_value != value) {
	        errno = 33;
       		return(value); 		/* value is NaN */
        }
  
 
        if ( ( ( ( dnan * ) & ( value ) ) -> nan_parts . exponent == 0x7ff ) )
        {
            if ( ( ( ( dnan * ) & ( value ) ) -> inf_parts . bits == 0 && ( ( dnan * ) & ( value ) ) -> inf_parts . fraction_low == 0 ) ) {  
                                * eptr = 2147483647 ;
                                return ( 0.0 ) ;
                              }
             else             {
                                * eptr = - 2147483647 - 1 ;
                                return ( value ) ;
                      }
        }
        * eptr = 0 ;
        if ( value == 0.0 )  
                return ( value ) ;
        absvalue = ( value > 0.0 ) ? value : - value ;
        for ( ; absvalue >= 1.0 ; absvalue *= 0.5 )
                ++ * eptr ;
        for ( ; absvalue < 0.5 ; absvalue += absvalue )
                -- * eptr ;
        return ( value > 0.0 ? absvalue : - absvalue ) ;
}
