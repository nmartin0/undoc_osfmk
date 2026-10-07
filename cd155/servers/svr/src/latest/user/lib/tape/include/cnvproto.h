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
 *	cnvproto.h 11.1 94/03/22 16:49:08
 */
static char	cnvprotoh_ver[] = "@(#) sourcefile cnvproto.h 11.1 94/03/22 16:49:08";

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

/************************************************
 *  cnvproto.h
 *
 *    Function Prototypes for data conversion
 ************************************************/


/* EBCDIC to ASCII conversions */

char    etoa(char ebcdic);
int     strnetoa(char *ebcdic_p, char *ascii_p, int length) ;
int     stretoa(char *ebcdic_p, char *ascii_p, int length) ;
char   *set_etoa_trans(char *new_table_p) ;


/* ASCII to EBCDIC conversions */

char    atoe(char ascii);
int     strnatoe(char *ascii_p, char *ebcdic_p, int length) ;
int     stratoe(char *ascii_p, char *ebcdic_p) ;
char   *set_atoe_trans(char *new_table_p) ;


/* IBM ZONED conversions */

int     itoz(int intel_int, char zone[], int zone_size) ;
int     eitoz(esize_t eint, unsigned char zone[], int zone_size);
int     ztoi(char *zoned, int *result, int length) ;
int     ztoei(unsigned char *zoned, esize_t *result, int length);
int     ztoa(unsigned char *zoned, char *asc_buf, int length);
int     ztoa_rtn(unsigned char *zoned, char *asc_buf, int length, int null_term);


/* IBM PACKED conversions */

int     itop(int intel_int, unsigned char packed[], int packed_length);
int     eitop(esize_t eint, unsigned char packed[], int packed_length);
int     ptoi(unsigned char *packed, int *result, int length);
int     ptoei(unsigned char *packed, esize_t *result, int length);
int     ptoa(unsigned char *packed, char *asc_buf, int length);
int     ptoa_rtn(unsigned char *packed, char *asc_buf, int length, int null_term);


/* intel to IBM integer conversions */

bit16   itoI_16(bit16 intel_int16);
bit32   itoI_32(bit32 intel_int32);
void    itoIn_16(bit16 intel_int16[], bit16 ibm_int16[], int length);
void    itoIn_32(bit32 intel_int32[], bit32 ibm_int32[], int length);


/* IBM to intel integer conversions */

bit16   Itoi_16(bit16 ibm_int16);
bit32   Itoi_32(bit32 ibm_int32);
void    Itoin_16(bit16 ibm_int16[], bit16 intel_int16[], int length);
void    Itoin_32(bit32 ibm_int32[], bit32 intel_int32[],int length);


/* IEEE (intel) to IBM floating point conversions */

int     istoIs(float *i_float, float *ibm_float, int length);
int     idtoIs(double *i_double, float *ibm_float, int length);
int     idtoId(double *i_double, double *ibm_double, int length);


/* IBM to IEEE (intel) floating point conversions */

int     Istois(float *ibm_float, float *i_float, int length);
void    Istoid(float *ibm_float, double *i_double, int length);
int     Idtoid(double *ibm_double, double *i_double, int length);


/* MISC conversion utilities */

void    uitoa(unsigned int uintel_int, char ascii[]);


