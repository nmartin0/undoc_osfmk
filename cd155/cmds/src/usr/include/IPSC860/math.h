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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/* @(#)math.h   1.25  com/inc,3.1,9013 3/13/90 11:37:37 */
/*
 * COMPONENT_NAME: (math.h) math header file
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1988, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

/*
 *      ANSI required entries in math.h
 *
 */
#ifdef _ANSI_C_SOURCE

#define HUGE_VAL        (__infinity__())       /* Infinity */
extern double   __infinity__();

#endif /*_ANSI_C_SOURCE */

/*
 *
 * The following function prototypes define functions available in the
 * XIX system but not required by the ANSI standard. They will not be
 * included in _ANSI_C_SOURCE is defined (strict ANSI conformance).
 *
 */

#ifdef _XOPEN_SOURCE

/*
 *      Useful mathmatical constants:
 *
 * M_E          - e
 * M_LOG2E      - log2(e)
 * M_LOG10E     - log10(e)
 * M_LN2        - ln(2)
 * M_PI         - pi
 * M_PI_2       - pi/2
 * M_PI_4       - pi/4
 * M_1_PI       - 1/pi
 * M_2_PI       - 2/pi
 * M_2_SQRTPI   - 2/(sqrt(pi)
 * M_SQRT2      - sqrt(2)
 * M_SQRT_2     - 1/sqrt(2)
*/

#define M_E        2.7182818284590452354E0  /*Hex  2^ 0 * 1.5bf0a8b145769 */
#define M_LOG2E    1.4426950408889633870E0  /*Hex  2^ 0 * 1.71547652B82FE */
#define M_LOG10E   4.3429448190325181667E-1 /*Hex  2^-2 * 1.BCB7B1526E50E */
#define M_LN2      6.9314718055994530942E-1 /*Hex  2^-1 * 1.62E42FEFA39EF */
#define M_LN10     2.3025850929940456840E0  /*Hex  2^ 1 * 1.26bb1bbb55516 */
#define M_PI       3.1415926535897931160E0  /*Hex  2^ 1 * 1.921FB54442D18 */
#define M_PI_2     1.5707963267948965580E0  /*Hex  2^ 0 * 1.921FB54442D18 */
#define M_PI_4     7.8539816339744827900E-1 /*Hex  2^-1 * 1.921FB54442D18 */
#define M_1_PI     3.1830988618379067154E-1 /*Hex  2^-2 * 1.45f306dc9c883 */
#define M_2_PI     6.3661977236758134308E-1 /*Hex  2^-1 * 1.45f306dc9c883 */
#define M_2_SQRTPI 1.1283791670955125739E0  /*Hex  2^ 0 * 1.20dd750429b6d */
#define M_SQRT2    1.4142135623730951455E0  /*Hex  2^ 0 * 1.6A09E667F3BCD */
#define M_SQRT1_2  7.0710678118654752440E-1 /*Hex  2^-1 * 1.6a09e667f3bcd */


#ifndef MAXFLOAT
#define MAXFLOAT            ((float)3.40282346638528860e+38)
#endif

#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE

/*
 *      Useful mathmatical constants:
 *
 * HUGE         - +infinity
 * M_2PI        - 2*pi
 *
 */
#define HUGE       HUGE_VAL
#define M_2PI      6.2831853071795862320E0  /*Hex  2^ 2 * 1.921FB54442D18 */

/* This is the nearest number to the cube root of MAXDOUBLE that   */
/*      doesn't cause the cube of it to overflow.                  */
/* In double precision hex this constant is: 554428a2 f98d728a     */
#define CUBRTHUGE      5.6438030941223618e102
#define INV_CUBRTHUGE  1.7718548704178434e-103

#endif /* _OSF_SOURCE */

#ifdef __PARAGON__

/* single precision analogues of double precision functions 
   Since they require floats to be passed as floats, only
   prototyped versions are provided.
*/
extern float acosf  (float);
extern float asinf  (float);
extern float atanf  (float);
extern float atan2f (float,
		     float);
extern float cosf   (float);
extern float sinf   (float);
extern float tanf   (float);
extern float coshf  (float);
extern float sinhf  (float);
extern float tanhf  (float);
extern float expf   (float);
extern float logf   (float);
extern float log10f (float);
extern float modff  (float,
		     float *);
extern float powf   (float,
		     float);
extern float sqrtf  (float);
extern float ceilf  (float);
extern float fabsf  (float);
extern float floorf (float);
extern float fmodf  (float,
		     float);

/* further qualify logb() */
extern double logb(double);

#ifdef __PGI
#define abs(x) __builtin_abs(x)
#define fabs(x) __builtin_fabs(x)
#define fabsf(x) __builtin_fabsf(x)
#define acos(x) __builtin_acos(x)
#define acosf(x) __builtin_acosf(x)
#define asin(x) __builtin_asin(x)
#define asinf(x) __builtin_asinf(x)
#define atan(x) __builtin_atan(x)
#define atanf(x) __builtin_atanf(x)
#define atan2(x, y) __builtin_atan2(x, y)
#define atan2f(x, y) __builtin_atan2f(x, y)
#define cos(x) __builtin_cos(x)
#define cosf(x) __builtin_cosf(x)
#define cosh(x) __builtin_cosh(x)
#define coshf(x) __builtin_coshf(x)
#define exp(x) __builtin_exp(x)
#define expf(x) __builtin_expf(x)
#define log(x) __builtin_log(x)
#define logf(x) __builtin_logf(x)
#define log10(x) __builtin_log10(x)
#define log10f(x) __builtin_log10f(x)
#define pow(x, y) __builtin_pow(x, y)
#define powf(x, y) __builtin_powf(x, y)
#define sin(x) __builtin_sin(x)
#define sinf(x) __builtin_sinf(x)
#define sinh(x) __builtin_sinh(x)
#define sinhf(x) __builtin_sinhf(x)
#define sqrt(x) __builtin_sqrt(x)
#define sqrtf(x) __builtin_sqrtf(x)
#define tan(x) __builtin_tan(x)
#define tanf(x) __builtin_tanf(x)
#define tanh(x) __builtin_tanh(x)
#define tanhf(x) __builtin_tanhf(x)
#endif  /* __PGI */

#endif /* __PARAGON__ */

#endif /* _MATH_H_ */
