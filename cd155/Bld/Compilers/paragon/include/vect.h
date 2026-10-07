/*
 *	Copyright 1992, The Portland Group, Incorporated.
 *	All rights reserved.
 *
 *	THE PORTLAND GROUP, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with The Portland Group and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

/*  naming convention:
 *     <prec>_<func>
 *
 *  <prec>  =  dv  -  double precision
 *             sv  -  single precision
 *
 *  <func>  =  name of function (e.g., sin, asin, etc.);
 *             Fortran-callable versions have a trailing _
 */
void dv_acos(int , double *, int , double *, int );
void dv_acos_(int *, double *, int *, double *, int *);
void dv_asin(int , double *, int , double *, int );
void dv_asin_(int *, double *, int *, double *, int *);
void dv_atan(int , double *, int , double *, int );
void dv_atan_(int *, double *, int *, double *, int *);
void dv_atan2(int , double *, int , double *, int , double *, int );
void dv_atan2_(int *, double *, int *, double *, int *, double *, int *);
void dv_cos(int , double *, int , double *, int );
void dv_cos_(int *, double *, int *, double *, int *);
void dv_div(int , double *, int , double *, int , double *, int );
void dv_div_(int *, double *, int *, double *, int *, double *, int *);
void dv_exp(int , double *, int , double *, int );
void dv_exp_(int *, double *, int *, double *, int *);
void dv_log(int , double *, int , double *, int );
void dv_log_(int *, double *, int *, double *, int *);
void dv_log10(int , double *, int , double *, int );
void dv_log10_(int *, double *, int *, double *, int *);
void dv_pow(int , double *, int , double *, int , double *, int );
void dv_pow_(int *, double *, int *, double *, int *, double *, int *);
void dv_recp(int , double , double *, int , double *, int );
void dv_recp_(int *, double *, double *, int *, double *, int *);
void dv_rsqrt(int , double *, int , double *, int );
void dv_rsqrt_(int *, double *, int *, double *, int *);
void dv_sin(int , double *, int , double *, int );
void dv_sin_(int *, double *, int *, double *, int *);
void dv_sqrt(int , double *, int , double *, int );
void dv_sqrt_(int *, double *, int *, double *, int *);
void dv_tan(int , double *, int , double *, int );
void dv_tan_(int *, double *, int *, double *, int *);
void sv_acos(int , float *, int , float *, int );
void sv_acos_(int *, float *, int *, float *, int *);
void sv_asin(int , float *, int , float *, int );
void sv_asin_(int *, float *, int *, float *, int *);
void sv_atan(int , float *, int , float *, int );
void sv_atan_(int *, float *, int *, float *, int *);
void sv_atan2(int , float *, int , float *, int , float *, int );
void sv_atan2_(int *, float *, int *, float *, int *, float *, int *);
void sv_cos(int , float *, int , float *, int );
void sv_cos_(int *, float *, int *, float *, int *);
void sv_div(int , float *, int , float *, int , float *, int );
void sv_div_(int *, float *, int *, float *, int *, float *, int *);
void sv_exp(int , float *, int , float *, int );
void sv_exp_(int *, float *, int *, float *, int *);
void sv_log(int , float *, int , float *, int );
void sv_log_(int *, float *, int *, float *, int *);
void sv_log10(int , float *, int , float *, int );
void sv_log10_(int *, float *, int *, float *, int *);
void sv_pow(int , float *, int , float *, int , float *, int );
void sv_pow_(int *, float *, int *, float *, int *, float *, int *);
void sv_recp(int , float , float *, int , float *, int );
void sv_recp_(int *, float *, float *, int *, float *, int *);
void sv_rsqrt(int , float *, int , float *, int );
void sv_rsqrt_(int *, float *, int *, float *, int *);
void sv_sin(int , float *, int , float *, int );
void sv_sin_(int *, float *, int *, float *, int *);
void sv_sqrt(int , float *, int , float *, int );
void sv_sqrt_(int *, float *, int *, float *, int *);
void sv_tan(int , float *, int , float *, int );
void sv_tan_(int *, float *, int *, float *, int *);
