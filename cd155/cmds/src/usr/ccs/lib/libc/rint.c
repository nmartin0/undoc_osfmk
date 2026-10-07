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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libc/rint.c,v 1.3 1994/11/19 02:07:54 mtm Exp $
 *
 * 
 */

struct exception {
	int type ;
	char * name ;
	double arg1 ;
	double arg2 ;
	double retval ;
} ;
enum version { c_issue_4 , ansi_1 , strict_ansi } ;
extern double acos ( double ) ; 
extern double asin ( double ) ; 
extern double atan ( double ) ; 
extern double atan2 ( double , double ) ; 
extern double cos ( double ) ; 
extern double sin ( double ) ; 
extern double tan ( double ) ; 
extern double cosh ( double ) ; 
extern double sinh ( double ) ; 
extern double tanh ( double ) ; 
extern double exp ( double ) ; 
extern double frexp ( double , int * ) ; 
extern double ldexp ( double , int ) ; 
extern double log ( double ) ; 
extern double log10 ( double ) ; 
extern double modf ( double , double * ) ; 
extern double pow ( double , double ) ; 
extern double sqrt ( double ) ;
extern double ceil ( double ) ; 
extern double fabs ( double ) ; 
extern double floor ( double ) ; 
extern double fmod ( double , double ) ; 
extern float acosf ( float ) ; 
extern float asinf ( float ) ; 
extern float atanf ( float ) ; 
extern float atan2f ( float , float ) ; 
extern float cosf ( float ) ; 
extern float sinf ( float ) ; 
extern float tanf ( float ) ; 
extern float coshf ( float ) ; 
extern float sinhf ( float ) ; 
extern float tanhf ( float ) ; 
extern float expf ( float ) ; 
extern float logf ( float ) ; 
extern float log10f ( float ) ; 
extern float powf ( float , float ) ; 
extern float sqrtf ( float ) ;
extern float ceilf ( float ) ; 
extern float fabsf ( float ) ; 
extern float floorf ( float ) ; 
extern float fmodf ( float , float ) ; 
extern float modff ( float , float * ) ; 
typedef union _h_val {
  	unsigned long i [ 2 ] ;
	double d ;
} _h_val ;
extern const _h_val __huge_val ;
extern double erf ( double ) ; 
extern double erfc ( double ) ; 
extern double gamma ( double ) ;
extern double hypot ( double , double ) ;
extern double j0 ( double ) ; 
extern double j1 ( double ) ; 
extern double jn ( int , double ) ; 
extern double y0 ( double ) ; 
extern double y1 ( double ) ; 
extern double yn ( int , double ) ; 
extern double lgamma ( double ) ;
extern int isnan ( double ) ;
extern const enum version _lib_version ;
extern double atof ( const char * ) ; 
 
extern double scalb ( double , double ) ;
extern double logb ( double ) ;
extern double nextafter ( double , double ) ;
extern double acosh ( double ) ;
extern double asinh ( double ) ;
extern double atanh ( double ) ;
extern double cbrt ( double ) ;
extern double copysign ( double , double ) ;
extern double rint ( double ) ;
extern double remainder ( double , double ) ;
extern int unordered ( double , double ) ;
extern int finite ( double ) ;
extern int matherr ( struct exception * ) ;
extern int signgam ;
 
typedef	enum	fpclass_t {
	FP_SNAN = 0 ,	 
	FP_QNAN = 1 ,	 
	FP_NINF = 2 ,	 
	FP_PINF = 3 ,	 
	FP_NDENORM = 4 ,  
	FP_PDENORM = 5 ,  
	FP_NZERO = 6 ,	 
	FP_PZERO = 7 ,    
	FP_NNORM = 8 ,	 
	FP_PNORM = 9	 
	} fpclass_t ;
extern fpclass_t fpclass ( double ) ;	 
extern int	finite ( double ) ;
extern int	unordered ( double , double ) ;
 
 
typedef	enum	fp_rnd {
    FP_RN = 0 ,	 
    FP_RM = 1 ,   
    FP_RP = 2 ,   
    FP_RZ = 3	 
    } fp_rnd ;
extern fp_rnd   fpsetround ( fp_rnd ) ;      
extern fp_rnd   fpgetround ( void ) ;        
 
 
 
extern int fpgetmask ( void ) ;                
extern int fpsetmask ( int ) ;           
extern int fpgetsticky ( void ) ;              
extern int fpsetsticky ( int ) ;         
 
extern int isnanf ( float ) ;               
extern int isnand ( double ) ;
 
extern unsigned fpgetieee ( ) ;	 
extern unsigned fpsetieee ( ) ;	 
 
typedef	enum	fp_ftype {
    UNKNOWN	= 0 ,	 
    INT_DIVZ	= 1 ,	 
    INT_OVFLW	= 2 ,	 
    FP_OVFLW	= 3 ,	 
    FP_UFLW	= 4 ,	 
    FP_INXACT	= 5 ,	 
    FP_DIVZ	= 6 ,	 
    FP_INVLD	= 7 ,	 
    FP_IN_OVFLW = 8 ,	 
    FP_IN_UFLW  = 9	 
    } fp_ftype ;
extern fp_ftype _fpftype ;
 
typedef	enum	fp_type {
    FP_NULL	= 0 ,	 
    FP_L	= 1 ,	 
    FP_F	= 2 ,	 
    FP_D	= 3 ,	 
    FP_X	= 4 ,	 
    FP_U	= 5 ,	 
    }	fp_type ;
typedef struct extended {  
    unsigned long	w [ 4 ] ;
    } extended ;
typedef union fp_union {  
    long	l ;
    unsigned	u ;
    float	f ;
    double	d ;
    extended	x ;	 
    } fp_union ;
typedef struct fp_dunion {  
    fp_type	type ;
    fp_union	value ;
    } fp_dunion ;
 
typedef	enum	fp_op {	 
    FP_ADD  = 0 ,	 
    FP_SUB  = 1 ,	 
    FP_MULT = 2 ,	 
    FP_RCP  = 3 ,	 
    FP_RSQR = 4 ,	 
    FP_CONV = 6 ,	 
    FP_RNDI = 7 ,	 
    FP_CMP  = 8 ,	 
    }	fp_op ;
struct fp_fault {
    fp_op	operation ;
    fp_dunion	operand [ 2 ] ;
    fp_dunion	t_value ;
    fp_dunion	result ;
    } ;
extern struct fp_fault	* _fpfault ;
 
typedef  union {
	struct {
		unsigned  lo	: 32 ;
		unsigned  hi	: 20 ;
		unsigned  exp	: 11 ;
		unsigned  sign	: 1 ;
	} fparts ;
	struct {
		unsigned  lo	: 32 ;
		unsigned  hi	: 19 ;
		unsigned  qnan_bit	: 1 ;
		unsigned  exp	: 11 ;
		unsigned  sign	: 1 ;
	} nparts ;
	struct {
		unsigned  lo	: 32 ;
		unsigned  hi	: 32 ;
	} fwords ;
	double	d ;
} _dval ;
 
typedef  union {
	struct {
		unsigned fract	: 23 ;
		unsigned exp	: 8 ;
		unsigned sign	: 1 ;
	} fparts ;
	struct {
		unsigned fract	: 22 ;
		unsigned qnan_bit	: 1 ;
		unsigned exp	: 8 ;
		unsigned sign	: 1 ;
	} nparts ;
	unsigned long	fword ;
	float	f ;
} _fval ;
 
double
rint ( double x )
{
	double y ;
	register double z , sign = 1.0 ;
	double one = 1.0 , three = 3.0 ;
	extern fp_rnd fpgetround ( ) ;
	fp_rnd	r ;
	if ( ( ( ( _dval * ) & ( x ) ) -> fparts . exp ) == 0x7ff )
		return ( x ) ;  
	z = modf ( x , & y ) ;
	if ( x == y ) 	 
		return ( y ) ;
	one /= three ;  
 
	if ( z < 0 ) {
		z = - z ;
		sign = - sign ;
	}
	r = fpgetround ( ) ;  
	switch ( r ) {
	case FP_RN :   
		if ( z > 0.5 )
			return ( y + sign ) ;
		if ( z == 0.5 )
			if ( ( modf ( y / 2.0 , & x ) ) == 0 )
			 
				return ( y ) ;
			else return ( y + sign ) ;
		else return ( y ) ;
	case FP_RP :  
		if ( sign == 1.0 )
			return ( y + sign ) ;
		else return ( y ) ;
	case FP_RM :  
		if ( sign == 1.0 )
			return ( y ) ;
		else return ( y + sign ) ;
	case FP_RZ :  
		return ( y ) ;
	}
 
}
 
