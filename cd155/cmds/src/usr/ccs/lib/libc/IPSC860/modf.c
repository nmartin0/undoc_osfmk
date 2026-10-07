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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libc/IPSC860/modf.c,v 1.4 1994/11/19 02:12:36 mtm Exp $
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
 
typedef	struct { int r [ 1 ] ; } *	physadr ;
typedef	long		daddr_t ;	 
typedef	char *		caddr_t ;	 
typedef	unsigned char	unchar ;
typedef	unsigned short	ushort ;
typedef	unsigned int	uint ;
typedef	unsigned long	ulong ;
typedef	ushort		ino_t ;		 
typedef	short		cnt_t ;		 
typedef	long		time_t ;		 
typedef	int		label_t [ 6 ] ;
typedef	short		dev_t ;		 
typedef	long		off_t ;		 
typedef	unsigned long	paddr_t ;	 
typedef	int		key_t ;		 
typedef	unsigned char	use_t ;		 
typedef	short		sysid_t ;
typedef	short		index_t ;
typedef	short		lock_t ;		 
typedef	unsigned int	size_t ;		 
typedef ushort		sel_t ;		 
 
typedef	char *		faddr_t ;	 
 
typedef unsigned char   u_char ;
typedef unsigned short  u_short ;
typedef unsigned int    u_int ;
typedef unsigned long   u_long ;
 
 
 
typedef int sig_atomic_t ;
void ( * signal ( int ,
	       void ( * ) ( int ) ) ) ( int ) ;
int    raise  ( int ) ;
double
modf ( double value , register double * iptr )
               
                      
{
	register double absvalue ;
        double check_value;

        check_value = value;

        if (check_value != value) {
                errno = 33;
                return(value);          /* value is NaN */
        }

	if ( ( ( ( dnan * ) & ( value ) ) -> nan_parts . exponent == 0x7ff ) ) {  
		if ( ( ( ( dnan * ) & ( value ) ) -> inf_parts . bits == 0 && ( ( dnan * ) & ( value ) ) -> inf_parts . fraction_low == 0 ) ) {  
			* iptr = value ;
			return ( 0.0 ) ;
		}
 
 
                else { * iptr = value ;
                       return ( value ) ;
                      }
	}
	if ( ( absvalue = ( value >= 0.0 ) ? value : - value ) >= ( ( double ) ( 1L << ( 8 * ( int ) sizeof ( long ) ) - 2 ) * ( 1L << ( ( 8 * ( int ) sizeof ( double ) ) - 11 + 1 - 1 ) - ( 8 * ( int ) sizeof ( long ) ) + 1 ) ) )
		* iptr = value ;  
	else {
		* iptr = absvalue + ( ( double ) ( 1L << ( 8 * ( int ) sizeof ( long ) ) - 2 ) * ( 1L << ( ( 8 * ( int ) sizeof ( double ) ) - 11 + 1 - 1 ) - ( 8 * ( int ) sizeof ( long ) ) + 1 ) ) ;  
		* iptr -= ( ( double ) ( 1L << ( 8 * ( int ) sizeof ( long ) ) - 2 ) * ( 1L << ( ( 8 * ( int ) sizeof ( double ) ) - 11 + 1 - 1 ) - ( 8 * ( int ) sizeof ( long ) ) + 1 ) ) ;  
		while ( * iptr > absvalue )  
			* iptr -= 1.0 ;  
		if ( value < 0.0 )
			* iptr = - * iptr ;
	}
	return ( value - * iptr ) ;  
}
