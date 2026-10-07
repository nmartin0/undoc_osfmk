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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: LIBCCNV watof
 *
 * FUNCTIONS: watof
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include <float.h>
#include <fp.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#define TRUE  1
#define FALSE 0

/*
 * ***********************************************************************
 *
 *      Macro m2dbls
 *
 *      Macro to multiply x.rslt[0] and x.rslt[1] by a pair of doubles
 *      from the power of ten tables.
 *      Each pair of fp doubles represents one real number. The high fp
 *      number of each pair contains the high order bits of the real number
 *      and the low fp number contains the low order bits. Thus each pair
 *      contains 106 bits of fraction. The algorithm to multiply these
 *      two numbers (e.g., x = x * y) is:
 *
 *      hi = yhi * xhi
 *      if (a != infinity)
 *         al = yhi * xhi - hi
 *         b  = yhi * xlo
 *         xlo = al + (b + ylo * xhi)
 *         xhi = hi + xlo
 *         xlo = (hi -xhi) + xlo
 *      else
 *         xhi = infinity
 *         xlo = 0.0
 *
 *    NOTE: for this macro to works hi, b, al, xhi, yhi, ylo, xlo must
 *          be doubles and py is a pointer to double.
 *
 *    Macro assumes y points to a double dimensioned array of 2 doubles
 *    where i is the second dimension. The "fixes" below for infinity
 *    have not been tested when both args being multiplied are near
 *    infinity since in atof at least one of the arguments is much
 *    smaller than infinity.
 *
 */

#if BYTE_ORDER == LITTLE_ENDIAN
#define M2DBLS(y,i)							\
{									\
	double *py;							\
	double xhi, xlo, yhi, ylo, hi, al, b;				\
									\
	py = ((double *) y+i);						\
	ylo = *py;							\
	yhi = *(py+1);							\
	xhi = *x.rslt;							\
	xlo = *(x.rslt+1);						\
	hi = yhi * xhi;							\
	if (!FINITE(hi))  {						\
		hi = DBL_MAX;						\
	}								\
	al  = yhi * xhi - hi;						\
	b      = yhi * xlo;						\
	xlo    = al+(b+(ylo * xhi));					\
	*x.rslt = hi + xlo;						\
	if(!FINITE(*x.rslt)) {						\
		*x.rslt = DBL_MAX;					\
	}								\
	*(x.rslt+1) = (hi - *x.rslt) + xlo;  				\
}
#else /* BYTE_ORDER == LITTLE_ENDIAN */	
#define M2DBLS(y,i)							\
{									\
	double *py;							\
	double xhi, xlo, yhi, ylo, hi, al, b;				\
									\
	py = ((double *) y+i);						\
	yhi = *py;							\
	ylo = *(py+1);							\
	xhi = *(x.rslt+1);						\
	xlo = *x.rslt;							\
	if ((hi = yhi * xhi) == DBL_INFINITY)  {			\
		hi = DBL_MAX;						\
	}								\
	al  = yhi * xhi - hi;						\
	b      = yhi * xlo;						\
	xlo    = al+(b+(ylo * xhi));					\
	if ((*(x.rslt+1) = hi + xlo) == DBL_INFINITY)  {		\
		*(x.rslt+1) = DBL_MAX;					\
	}								\
	*x.rslt = (hi - *(x.rslt+1)) + xlo;  				\
}
#endif /* BYTE_ORDER == LITTLE_ENDIAN */				\
/*
 *
 *  Globals variables for all routines
 *
 */

extern NLchar *_wdsto2fp();
typedef struct fp { 
	long int i[2]; 
} fp ;

#if BYTE_ORDER != LITTLE_ENDIAN
static struct fp pospow1[64][2] = {
	0x3FF00000, 0x00000000,   0x00000000, 0x00000000,      /*  10^0    */
	0x40240000, 0x00000000,   0x00000000, 0x00000000,      /*  10^1    */
	0x40590000, 0x00000000,   0x00000000, 0x00000000,      /*  10^2    */
	0x408F4000, 0x00000000,   0x00000000, 0x00000000,      /*  10^3    */
	0x40C38800, 0x00000000,   0x00000000, 0x00000000,      /*  10^4    */
	0x40F86A00, 0x00000000,   0x00000000, 0x00000000,      /*  10^5    */
	0x412E8480, 0x00000000,   0x00000000, 0x00000000,      /*  10^6    */
	0x416312D0, 0x00000000,   0x00000000, 0x00000000,      /*  10^7    */
	0x4197D784, 0x00000000,   0x00000000, 0x00000000,      /*  10^8    */
	0x41CDCD65, 0x00000000,   0x00000000, 0x00000000,      /*  10^9    */
	0x4202A05F, 0x20000000,   0x00000000, 0x00000000,      /*  10^10   */
	0x42374876, 0xe8000000,   0x00000000, 0x00000000,      /*  10^11   */
	0x426D1A94, 0xa2000000,   0x00000000, 0x00000000,      /*  10^12   */
	0x42A2309C, 0xe5400000,   0x00000000, 0x00000000,      /*  10^13   */
	0x42D6BCC4, 0x1e900000,   0x00000000, 0x00000000,      /*  10^14   */
	0x430C6BF5, 0x26340000,   0x00000000, 0x00000000,      /*  10^15   */
	0x4341C379, 0x37e08000,   0x00000000, 0x00000000,      /*  10^16   */
	0x43763457, 0x85d8a000,   0x00000000, 0x00000000,      /*  10^17   */
	0x43ABC16D, 0x674ec800,   0x00000000, 0x00000000,      /*  10^18   */
	0x43E158E4, 0x60913d00,   0x00000000, 0x00000000,      /*  10^19   */
	0x4415AF1D, 0x78b58c40,   0x00000000, 0x00000000,      /*  10^20   */
	0x444B1AE4, 0xd6e2ef50,   0x00000000, 0x00000000,      /*  10^21   */
	0x4480F0CF, 0x064dd592,   0x00000000, 0x00000000,      /*  10^22   */
	0x44B52D02, 0xc7e14af6,   0x41600000, 0x00000000,      /*  10^23   */
	0x44EA7843, 0x79d99db4,   0x41700000, 0x00000000,      /*  10^24   */
	0x45208B2A, 0x2c280290,   0x41D28000, 0x00000000,      /*  10^25   */
	0x4554ADF4, 0xb7320334,   0x42072000, 0x00000000,      /*  10^26   */
	0x4589D971, 0xe4fe8401,   0x423CE800, 0x00000000,      /*  10^27   */
	0x45C027E7, 0x2f1f1281,   0x42584400, 0x00000000,      /*  10^28   */
	0x45F431E0, 0xfae6d721,   0x429F2A80, 0x00000000,      /*  10^29   */
	0x46293E59, 0x39a08ce9,   0x42DB7A90, 0x00000000,      /*  10^30   */
	0x465F8DEF, 0x8808b024,   0x42F4B268, 0x00000000,      /*  10^31   */
	0x4693B8B5, 0xb5056e16,   0x434677C0, 0x80000000,      /*  10^32   */
	0x46C8A6E3, 0x2246c99c,   0x43682B61, 0x40000000,      /*  10^33   */
	0x46FED09B, 0xead87c03,   0x439E3639, 0x90000000,      /*  10^34   */
	0x47334261, 0x72c74d82,   0x43C5C3C7, 0xf4000000,      /*  10^35   */
	0x476812F9, 0xcf7920e2,   0x4416CD2E, 0x7c400000,      /*  10^36   */
	0x479E17B8, 0x4357691b,   0x443900F4, 0x36a00000,      /*  10^37   */
	0x47D2CED3, 0x2a16a1b1,   0x445E8262, 0x88900000,      /*  10^38   */
	0x48078287, 0xf49c4a1d,   0x44A988BE, 0xcaad0000,      /*  10^39   */
	0x483D6329, 0xf1c35ca4,   0x44E7F577, 0x3eac2000,      /*  10^40   */
	0x48725DFA, 0x371a19e6,   0x452EF96A, 0x872b9400,      /*  10^41   */
	0x48A6F578, 0xc4e0a060,   0x4556B7C5, 0x28f67900,      /*  10^42   */
	0x48DCB2D6, 0xf618c878,   0x458C65B6, 0x73341740,      /*  10^43   */
	0x4911EFC6, 0x59cf7d4b,   0x45C1BF92, 0x08008e88,      /*  10^44   */
	0x49466BB7, 0xf0435c9e,   0x45EC5EED, 0x14016454,      /*  10^45   */
	0x497C06A5, 0xec5433c6,   0x45EBB542, 0xc80deb48,      /*  10^46   */
	0x49B18427, 0xb3b4a05b,   0x46691514, 0x9bd08b31,      /*  10^47   */
	0x49E5E531, 0xa0a1c872,   0x46975A59, 0xc2c4adfd,      /*  10^48   */
	0x4A1B5E7E, 0x08ca3a8f,   0x46BA61E0, 0x66ebb2f9,      /*  10^49   */
	0x4A511B0E, 0xc57e6499,   0x47043E96, 0x2029a7ee,      /*  10^50   */
	0x4A8561D2, 0x76ddfdc0,   0x46F4E3BA, 0x83411e91,      /*  10^51   */
	0x4ABABA47, 0x14957d30,   0x472A1CA9, 0x24116636,      /*  10^52   */
	0x4AF0B46C, 0x6cdd6e3e,   0x476051E9, 0xb68adfe2,      /*  10^53   */
	0x4B24E187, 0x8814c9cd,   0x47D14666, 0x4242d97e,      /*  10^54   */
	0x4B5A19E9, 0x6a19fc40,   0x480D97FF, 0xd2d38fdd,      /*  10^55   */
	0x4B905031, 0xe2503da8,   0x48427EFF, 0xe3c439ea,      /*  10^56   */
	0x4BC4643E, 0x5ae44d12,   0x48771EBF, 0xdcb54865,      /*  10^57   */
	0x4BF97D4D, 0xf19d6057,   0x4899CCDF, 0xa7c534fc,      /*  10^58   */
	0x4C2FDCA1, 0x6e04b86d,   0x48C04017, 0x91b6823b,      /*  10^59   */
	0x4C63E9E4, 0xe4c2f344,   0x4902280E, 0xbb121165,      /*  10^60   */
	0x4C98E45E, 0x1df3b015,   0x4936B212, 0x69d695be,      /*  10^61   */
	0x4CCF1D75, 0xa5709c1a,   0x49762F4B, 0x82261d97,      /*  10^62   */
	0x4D037269, 0x87666190,   0x49B5DD8F, 0x3157d27e       /*  10^63   */
};
#else /* BYTE_ORDER != LITTLE_ENDIAN */	
static struct fp pospow1[64][2] = {
	0x00000000, 0x3FF00000,   0x00000000, 0x00000000,      /*  10^0    */
	0x00000000, 0x40240000,   0x00000000, 0x00000000,      /*  10^1    */
	0x00000000, 0x40590000,   0x00000000, 0x00000000,      /*  10^2    */
	0x00000000, 0x408F4000,   0x00000000, 0x00000000,      /*  10^3    */
	0x00000000, 0x40C38800,   0x00000000, 0x00000000,      /*  10^4    */
	0x00000000, 0x40F86A00,   0x00000000, 0x00000000,      /*  10^5    */
	0x00000000, 0x412E8480,   0x00000000, 0x00000000,      /*  10^6    */
	0x00000000, 0x416312D0,   0x00000000, 0x00000000,      /*  10^7    */
	0x00000000, 0x4197D784,   0x00000000, 0x00000000,      /*  10^8    */
	0x00000000, 0x41CDCD65,   0x00000000, 0x00000000,      /*  10^9    */
	0x20000000, 0x4202A05F,   0x00000000, 0x00000000,      /*  10^10   */
	0xe8000000, 0x42374876,   0x00000000, 0x00000000,      /*  10^11   */
	0xa2000000, 0x426D1A94,   0x00000000, 0x00000000,      /*  10^12   */
	0xe5400000, 0x42A2309C,   0x00000000, 0x00000000,      /*  10^13   */
	0x1e900000, 0x42D6BCC4,   0x00000000, 0x00000000,      /*  10^14   */
	0x26340000, 0x430C6BF5,   0x00000000, 0x00000000,      /*  10^15   */
	0x37e08000, 0x4341C379,   0x00000000, 0x00000000,      /*  10^16   */
	0x85d8a000, 0x43763457,   0x00000000, 0x00000000,      /*  10^17   */
	0x674ec800, 0x43ABC16D,   0x00000000, 0x00000000,      /*  10^18   */
	0x60913d00, 0x43E158E4,   0x00000000, 0x00000000,      /*  10^19   */
	0x78b58c40, 0x4415AF1D,   0x00000000, 0x00000000,      /*  10^20   */
	0xd6e2ef50, 0x444B1AE4,   0x00000000, 0x00000000,      /*  10^21   */
	0x064dd592, 0x4480F0CF,   0x00000000, 0x00000000,      /*  10^22   */
	0xc7e14af6, 0x44B52D02,   0x00000000, 0x41600000,      /*  10^23   */
	0x79d99db4, 0x44EA7843,   0x00000000, 0x41700000,      /*  10^24   */
	0x2c280290, 0x45208B2A,   0x00000000, 0x41D28000,      /*  10^25   */
	0xb7320334, 0x4554ADF4,   0x00000000, 0x42072000,      /*  10^26   */
	0xe4fe8401, 0x4589D971,   0x00000000, 0x423CE800,      /*  10^27   */
	0x2f1f1281, 0x45C027E7,   0x00000000, 0x42584400,      /*  10^28   */
	0xfae6d721, 0x45F431E0,   0x00000000, 0x429F2A80,      /*  10^29   */
	0x39a08ce9, 0x46293E59,   0x00000000, 0x42DB7A90,      /*  10^30   */
	0x8808b024, 0x465F8DEF,   0x00000000, 0x42F4B268,      /*  10^31   */
	0xb5056e16, 0x4693B8B5,   0x80000000, 0x434677C0,      /*  10^32   */
	0x2246c99c, 0x46C8A6E3,   0x40000000, 0x43682B61,      /*  10^33   */
	0xead87c03, 0x46FED09B,   0x90000000, 0x439E3639,      /*  10^34   */
	0x72c74d82, 0x47334261,   0xf4000000, 0x43C5C3C7,      /*  10^35   */
	0xcf7920e2, 0x476812F9,   0x7c400000, 0x4416CD2E,      /*  10^36   */
	0x4357691b, 0x479E17B8,   0x36a00000, 0x443900F4,      /*  10^37   */
	0x2a16a1b1, 0x47D2CED3,   0x88900000, 0x445E8262,      /*  10^38   */
	0xf49c4a1d, 0x48078287,   0xcaad0000, 0x44A988BE,      /*  10^39   */
	0xf1c35ca4, 0x483D6329,   0x3eac2000, 0x44E7F577,      /*  10^40   */
	0x371a19e6, 0x48725DFA,   0x872b9400, 0x452EF96A,      /*  10^41   */
	0xc4e0a060, 0x48A6F578,   0x28f67900, 0x4556B7C5,      /*  10^42   */
	0xf618c878, 0x48DCB2D6,   0x73341740, 0x458C65B6,      /*  10^43   */
	0x59cf7d4b, 0x4911EFC6,   0x08008e88, 0x45C1BF92,      /*  10^44   */
	0xf0435c9e, 0x49466BB7,   0x14016454, 0x45EC5EED,      /*  10^45   */
	0xec5433c6, 0x497C06A5,   0xc80deb48, 0x45EBB542,      /*  10^46   */
	0xb3b4a05b, 0x49B18427,   0x9bd08b31, 0x46691514,      /*  10^47   */
	0xa0a1c872, 0x49E5E531,   0xc2c4adfd, 0x46975A59,      /*  10^48   */
	0x08ca3a8f, 0x4A1B5E7E,   0x66ebb2f9, 0x46BA61E0,      /*  10^49   */
	0xc57e6499, 0x4A511B0E,   0x2029a7ee, 0x47043E96,      /*  10^50   */
	0x76ddfdc0, 0x4A8561D2,   0x83411e91, 0x46F4E3BA,      /*  10^51   */
	0x14957d30, 0x4ABABA47,   0x24116636, 0x472A1CA9,      /*  10^52   */
	0x6cdd6e3e, 0x4AF0B46C,   0xb68adfe2, 0x476051E9,      /*  10^53   */
	0x8814c9cd, 0x4B24E187,   0x4242d97e, 0x47D14666,      /*  10^54   */
	0x6a19fc40, 0x4B5A19E9,   0xd2d38fdd, 0x480D97FF,      /*  10^55   */
	0xe2503da8, 0x4B905031,   0xe3c439ea, 0x48427EFF,      /*  10^56   */
	0x5ae44d12, 0x4BC4643E,   0xdcb54865, 0x48771EBF,      /*  10^57   */
	0xf19d6057, 0x4BF97D4D,   0xa7c534fc, 0x4899CCDF,      /*  10^58   */
	0x6e04b86d, 0x4C2FDCA1,   0x91b6823b, 0x48C04017,      /*  10^59   */
	0xe4c2f344, 0x4C63E9E4,   0xbb121165, 0x4902280E,      /*  10^60   */
	0x1df3b015, 0x4C98E45E,   0x69d695be, 0x4936B212,      /*  10^61   */
	0xa5709c1a, 0x4CCF1D75,   0x82261d97, 0x49762F4B,      /*  10^62   */
	0x87666190, 0x4D037269,   0x3157d27e, 0x49B5DD8F       /*  10^63   */
};
#endif /* BYTE_ORDER != LITTLE_ENDIAN */

#if BYTE_ORDER != LITTLE_ENDIAN
static struct fp pospow2[5][2] = {
	0x3FF00000, 0x00000000,   0x00000000, 0x00000000,      /*  10^0    */
	0x4D384F03, 0xe93ff9f4,   0x49EB54F2, 0xfdadc71e,      /*  10^64   */
	0x5A827748, 0xf9301d31,   0x57337F19, 0xbccdb0db,      /*  10^128  */
	0x67CC0E1E, 0xf1a724ea,   0x6475AA16, 0xef894fd2,      /*  10^192  */
	0x75154FDD, 0x7f73bf3b,   0x71CA3776, 0xee406e64       /*  10^256  */
};
#else /* BYTE_ORDER != LITTLE_ENDIAN */	
static struct fp pospow2[5][2] = {
	0x00000000, 0x3FF00000,   0x00000000, 0x00000000,      /*  10^0    */
	0xe93ff9f4, 0x4D384F03,   0xfdadc71e, 0x49EB54F2,      /*  10^64   */
	0xf9301d31, 0x5A827748,   0xbccdb0db, 0x57337F19,      /*  10^128  */
	0xf1a724ea, 0x67CC0E1E,   0xef894fd2, 0x6475AA16,      /*  10^192  */
	0x7f73bf3b, 0x75154FDD,   0xee406e64, 0x71CA3776       /*  10^256  */
};
#endif /* BYTE_ORDER != LITTLE_ENDIAN */


#if BYTE_ORDER != LITTLE_ENDIAN
static struct fp negpow1[64][2] = {
	0x3FF00000, 0x00000000,    0x00000000, 0x00000000,   /* 10^-0   */
	0x3FB99999, 0x99999999,    0x3C633333, 0x33333333,   /* 10^-1   */
	0x3F847AE1, 0x47ae147a,    0x3C3C28F5, 0xc28f5c29,   /* 10^-2   */
	0x3F50624D, 0xd2f1a9fb,    0x3C0CED91, 0x6872b021,   /* 10^-3   */
	0x3F1A36E2, 0xeb1c432c,    0x3BC4AF4F, 0x0d844d01,   /* 10^-4   */
	0x3EE4F8B5, 0x88e368f0,    0x3B908C3F, 0x3e0370ce,   /* 10^-5   */
	0x3EB0C6F7, 0xa0b5ed8d,    0x3B4B5A63, 0xf9a49c2c,   /* 10^-6   */
	0x3E7AD7F2, 0x9abcaf48,    0x3B15E1E9, 0x9483b023,   /* 10^-7   */
	0x3E45798E, 0xe2308c39,    0x3AFBF3F7, 0x0834acdb,   /* 10^-8   */
	0x3E112E0B, 0xe826d694,    0x3AC65CC5, 0xa02a23e2,   /* 10^-9   */
	0x3DDB7CDF, 0xd9d7bdba,    0x3A86FAD5, 0xcd10396a,   /* 10^-10  */
	0x3DA5FD7F, 0xe1796495,    0x3A47F7BC, 0x7b4d28aa,   /* 10^-11  */
	0x3D719799, 0x812dea11,    0x39F97F27, 0xf0f6e886,   /* 10^-12  */
	0x3D3C25C2, 0x68497681,    0x39E84CA1, 0x9697c81b,   /* 10^-13  */
	0x3D06849B, 0x86a12b9b,    0x394EA709, 0x09833de7,   /* 10^-14  */
	0x3CD203AF, 0x9ee75615,    0x3983643E, 0x74dc0530,   /* 10^-15  */
	0x3C9CD2B2, 0x97d889bc,    0x3925B4C2, 0xebe68799,   /* 10^-16  */
	0x3C670EF5, 0x4646d496,    0x39112426, 0xfbfae7eb,   /* 10^-17  */
	0x3C32725D, 0xd1d243ab,    0x38E41CEB, 0xfcc8b989,   /* 10^-18  */
	0x3BFD83C9, 0x4fb6d2ac,    0x388A52B3, 0x1e9e3d07,   /* 10^-19  */
	0x3BC79CA1, 0x0c924223,    0x38675447, 0xa5d8e536,   /* 10^-20  */
	0x3B92E3B4, 0x0a0e9b4f,    0x383F769F, 0xb7e0b75e,   /* 10^-21  */
	0x3B5E3920, 0x10175ee5,    0x3802C54C, 0x931a2c4b,   /* 10^-22  */
	0x3B282DB3, 0x4012b251,    0x37C13BAD, 0xb829e079,   /* 10^-23  */
	0x3AF357C2, 0x99a88ea7,    0x379A9624, 0x9354b394,   /* 10^-24  */
	0x3ABEF2D0, 0xf5da7dd8,    0x376544EA, 0x0f76f610,   /* 10^-25  */
	0x3A88C240, 0xc4aecb13,    0x37376A54, 0xd92bf80d,   /* 10^-26  */
	0x3A53CE9A, 0x36f23c0f,    0x370921DD, 0x7a89933d,   /* 10^-27  */
	0x3A1FB0F6, 0xbe506019,    0x36B06C5E, 0x54eb70c4,   /* 10^-28  */
	0x39E95A5E, 0xfea6b347,    0x3689F04B, 0x7722c09d,   /* 10^-29  */
	0x39B4484B, 0xfeebc29f,    0x3660C684, 0x960de6a5,   /* 10^-30  */
	0x398039D6, 0x6589687f,    0x3633D203, 0xab3e521e,   /* 10^-31  */
	0x3949F623, 0xd5a8a732,    0x35F2E99F, 0x7863b696,   /* 10^-32  */
	0x3914C4E9, 0x77ba1f5b,    0x35C587B2, 0xc6b62bab,   /* 10^-33  */
	0x38E09D87, 0x92fb4c49,    0x3585A5EA, 0xd789df78,   /* 10^-34  */
	0x38AA95A5, 0xb7f87a0e,    0x355E1E55, 0x793b192d,   /* 10^-35  */
	0x38754484, 0x932d2e72,    0x351696EF, 0x285e8eaf,   /* 10^-36  */
	0x3841039D, 0x428a8b8e,    0x34F5D5F9, 0x435905df,   /* 10^-37  */
	0x380B38FB, 0x9daa78e4,    0x34A2ACB7, 0x3de9ac65,   /* 10^-38  */
	0x37D5C72F, 0xb1552d83,    0x347BBD5F, 0x64baf050,   /* 10^-39  */
	0x37A16C26, 0x2777579c,    0x34463119, 0x1d6259da,   /* 10^-40  */
	0x376BE03D, 0x0bf225c6,    0x341E8DAD, 0xb11b7b15,   /* 10^-41  */
	0x37364CFD, 0xa3281e38,    0x33E87157, 0xc0e2c8dd,   /* 10^-42  */
	0x3701D731, 0x4f534b60,    0x33B38DDF, 0xcd823a4b,   /* 10^-43  */
	0x36CC8B82, 0x18854567,    0x33682C65, 0xc4d3edbc,   /* 10^-44  */
	0x3696D601, 0xad376ab9,    0x331A27AC, 0x0f72f8c0,   /* 10^-45  */
	0x366244CE, 0x242c5560,    0x331C372A, 0xce584c13,   /* 10^-46  */
	0x362D3AE3, 0x6d13bbce,    0x32BAFAAB, 0x8f01e6e1,   /* 10^-47  */
	0x35F7624F, 0x8a762fd8,    0x32859556, 0x0c018581,   /* 10^-48  */
	0x35C2B50C, 0x6ec4f313,    0x32656EEF, 0x38009bcd,   /* 10^-49  */
	0x358DEE7A, 0x4ad4b81e,    0x323DF258, 0xf99a163e,   /* 10^-50  */
	0x3557F1FB, 0x6f10934b,    0x320E5B7A, 0x614811cb,   /* 10^-51  */
	0x352327FC, 0x58da0f6f,    0x31DEAF95, 0x1aa00e3c,   /* 10^-52  */
	0x34EEA660, 0x8e29b24c,    0x31977F54, 0xf7667d2d,   /* 10^-53  */
	0x34B8851A, 0x0b548ea3,    0x316932AA, 0x5f8530f1,   /* 10^-54  */
	0x34839DAE, 0x6f76d883,    0x30EEAAA3, 0x26eb4b43,   /* 10^-55  */
	0x344F62B0, 0xb257c0d1,    0x30F4BBBB, 0x5b8bc3c3,   /* 10^-56  */
	0x34191BC0, 0x8eac9a41,    0x30B45F92, 0x2c12d2d2,   /* 10^-57  */
	0x33E41633, 0xa556e1cd,    0x309B596D, 0xab3ababa,   /* 10^-58  */
	0x33B011C2, 0xeaabe7d7,    0x306C478A, 0xef622efc,   /* 10^-59  */
	0x3379B604, 0xaaaca626,    0x300B6379, 0x2f412cb0,   /* 10^-60  */
	0x3344919D, 0x5556eb51,    0x2FF8AD7E, 0xa30d08f0,   /* 10^-61  */
	0x3310747D, 0xdddf22a7,    0x2FCA2465, 0x4f3da0c0,   /* 10^-62  */
	0x32DA53FC, 0x9631d10c,    0x2F803A3B, 0xb1fc3467    /* 10^-63  */
};
#else /* BYTE_ORDER != LITTLE_ENDIAN */	
static struct fp negpow1[64][2] = {
	0x00000000, 0x3FF00000,    0x00000000, 0x00000000,   /* 10^-0   */
	0x99999999, 0x3FB99999,    0x33333333, 0x3C633333,   /* 10^-1   */
	0x47ae147a, 0x3F847AE1,    0xc28f5c29, 0x3C3C28F5,   /* 10^-2   */
	0xd2f1a9fb, 0x3F50624D,    0x6872b021, 0x3C0CED91,   /* 10^-3   */
	0xeb1c432c, 0x3F1A36E2,    0x0d844d01, 0x3BC4AF4F,   /* 10^-4   */
	0x88e368f0, 0x3EE4F8B5,    0x3e0370ce, 0x3B908C3F,   /* 10^-5   */
	0xa0b5ed8d, 0x3EB0C6F7,    0xf9a49c2c, 0x3B4B5A63,   /* 10^-6   */
	0x9abcaf48, 0x3E7AD7F2,    0x9483b023, 0x3B15E1E9,   /* 10^-7   */
	0xe2308c39, 0x3E45798E,    0x0834acdb, 0x3AFBF3F7,   /* 10^-8   */
	0xe826d694, 0x3E112E0B,    0xa02a23e2, 0x3AC65CC5,   /* 10^-9   */
	0xd9d7bdba, 0x3DDB7CDF,    0xcd10396a, 0x3A86FAD5,   /* 10^-10  */
	0xe1796495, 0x3DA5FD7F,    0x7b4d28aa, 0x3A47F7BC,   /* 10^-11  */
	0x812dea11, 0x3D719799,    0xf0f6e886, 0x39F97F27,   /* 10^-12  */
	0x68497681, 0x3D3C25C2,    0x9697c81b, 0x39E84CA1,   /* 10^-13  */
	0x86a12b9b, 0x3D06849B,    0x09833de7, 0x394EA709,   /* 10^-14  */
	0x9ee75615, 0x3CD203AF,    0x74dc0530, 0x3983643E,   /* 10^-15  */
	0x97d889bc, 0x3C9CD2B2,    0xebe68799, 0x3925B4C2,   /* 10^-16  */
	0x4646d496, 0x3C670EF5,    0xfbfae7eb, 0x39112426,   /* 10^-17  */
	0xd1d243ab, 0x3C32725D,    0xfcc8b989, 0x38E41CEB,   /* 10^-18  */
	0x4fb6d2ac, 0x3BFD83C9,    0x1e9e3d07, 0x388A52B3,   /* 10^-19  */
	0x0c924223, 0x3BC79CA1,    0xa5d8e536, 0x38675447,   /* 10^-20  */
	0x0a0e9b4f, 0x3B92E3B4,    0xb7e0b75e, 0x383F769F,   /* 10^-21  */
	0x10175ee5, 0x3B5E3920,    0x931a2c4b, 0x3802C54C,   /* 10^-22  */
	0x4012b251, 0x3B282DB3,    0xb829e079, 0x37C13BAD,   /* 10^-23  */
	0x99a88ea7, 0x3AF357C2,    0x9354b394, 0x379A9624,   /* 10^-24  */
	0xf5da7dd8, 0x3ABEF2D0,    0x0f76f610, 0x376544EA,   /* 10^-25  */
	0xc4aecb13, 0x3A88C240,    0xd92bf80d, 0x37376A54,   /* 10^-26  */
	0x36f23c0f, 0x3A53CE9A,    0x7a89933d, 0x370921DD,   /* 10^-27  */
	0xbe506019, 0x3A1FB0F6,    0x54eb70c4, 0x36B06C5E,   /* 10^-28  */
	0xfea6b347, 0x39E95A5E,    0x7722c09d, 0x3689F04B,   /* 10^-29  */
	0xfeebc29f, 0x39B4484B,    0x960de6a5, 0x3660C684,   /* 10^-30  */
	0x6589687f, 0x398039D6,    0xab3e521e, 0x3633D203,   /* 10^-31  */
	0xd5a8a732, 0x3949F623,    0x7863b696, 0x35F2E99F,   /* 10^-32  */
	0x77ba1f5b, 0x3914C4E9,    0xc6b62bab, 0x35C587B2,   /* 10^-33  */
	0x92fb4c49, 0x38E09D87,    0xd789df78, 0x3585A5EA,   /* 10^-34  */
	0xb7f87a0e, 0x38AA95A5,    0x793b192d, 0x355E1E55,   /* 10^-35  */
	0x932d2e72, 0x38754484,    0x285e8eaf, 0x351696EF,   /* 10^-36  */
	0x428a8b8e, 0x3841039D,    0x435905df, 0x34F5D5F9,   /* 10^-37  */
	0x9daa78e4, 0x380B38FB,    0x3de9ac65, 0x34A2ACB7,   /* 10^-38  */
	0xb1552d83, 0x37D5C72F,    0x64baf050, 0x347BBD5F,   /* 10^-39  */
	0x2777579c, 0x37A16C26,    0x1d6259da, 0x34463119,   /* 10^-40  */
	0x0bf225c6, 0x376BE03D,    0xb11b7b15, 0x341E8DAD,   /* 10^-41  */
	0xa3281e38, 0x37364CFD,    0xc0e2c8dd, 0x33E87157,   /* 10^-42  */
	0x4f534b60, 0x3701D731,    0xcd823a4b, 0x33B38DDF,   /* 10^-43  */
	0x18854567, 0x36CC8B82,    0xc4d3edbc, 0x33682C65,   /* 10^-44  */
	0xad376ab9, 0x3696D601,    0x0f72f8c0, 0x331A27AC,   /* 10^-45  */
	0x242c5560, 0x366244CE,    0xce584c13, 0x331C372A,   /* 10^-46  */
	0x6d13bbce, 0x362D3AE3,    0x8f01e6e1, 0x32BAFAAB,   /* 10^-47  */
	0x8a762fd8, 0x35F7624F,    0x0c018581, 0x32859556,   /* 10^-48  */
	0x6ec4f313, 0x35C2B50C,    0x38009bcd, 0x32656EEF,   /* 10^-49  */
	0x4ad4b81e, 0x358DEE7A,    0xf99a163e, 0x323DF258,   /* 10^-50  */
	0x6f10934b, 0x3557F1FB,    0x614811cb, 0x320E5B7A,   /* 10^-51  */
	0x58da0f6f, 0x352327FC,    0x1aa00e3c, 0x31DEAF95,   /* 10^-52  */
	0x8e29b24c, 0x34EEA660,    0xf7667d2d, 0x31977F54,   /* 10^-53  */
	0x0b548ea3, 0x34B8851A,    0x5f8530f1, 0x316932AA,   /* 10^-54  */
	0x6f76d883, 0x34839DAE,    0x26eb4b43, 0x30EEAAA3,   /* 10^-55  */
	0xb257c0d1, 0x344F62B0,    0x5b8bc3c3, 0x30F4BBBB,   /* 10^-56  */
	0x8eac9a41, 0x34191BC0,    0x2c12d2d2, 0x30B45F92,   /* 10^-57  */
	0xa556e1cd, 0x33E41633,    0xab3ababa, 0x309B596D,   /* 10^-58  */
	0xeaabe7d7, 0x33B011C2,    0xef622efc, 0x306C478A,   /* 10^-59  */
	0xaaaca626, 0x3379B604,    0x2f412cb0, 0x300B6379,   /* 10^-60  */
	0x5556eb51, 0x3344919D,    0xa30d08f0, 0x2FF8AD7E,   /* 10^-61  */
	0xdddf22a7, 0x3310747D,    0x4f3da0c0, 0x2FCA2465,   /* 10^-62  */
	0x9631d10c, 0x32DA53FC,    0xb1fc3467, 0x2F803A3B    /* 10^-63  */
};
#endif /* BYTE_ORDER != LITTLE_ENDIAN */

#if BYTE_ORDER != LITTLE_ENDIAN
static struct fp negpow2[6][2] = {
	0x3FF00000, 0x00000000,    0x00000000, 0x00000000,   /* 10^-0    */
	0x32A50FFD, 0x44f4a73d,    0x2F3A53F2, 0x398d747b,   /* 10^-64   */
	0x255BBA08, 0xcf8c979c,    0x220282B1, 0xf2cfdb41,   /* 10^-128  */
	0x18123FF0, 0x6eea8479,    0x14C019ED, 0x8c1a8d19,   /* 10^-192  */
	0x0AC80628, 0x64ac6f43,    0x07539FA9, 0x11155ff0    /* 10^-256  */
};
#else /* BYTE_ORDER != LITTLE_ENDIAN */	
static struct fp negpow2[6][2] = {
	0x00000000, 0x3FF00000,    0x00000000, 0x00000000,   /* 10^-0    */
	0x44f4a73d, 0x32A50FFD,    0x398d747b, 0x2F3A53F2,   /* 10^-64   */
	0xcf8c979c, 0x255BBA08,    0xf2cfdb41, 0x220282B1,   /* 10^-128  */
	0x6eea8479, 0x18123FF0,    0x8c1a8d19, 0x14C019ED,   /* 10^-192  */
	0x64ac6f43, 0x0AC80628,    0x11155ff0, 0x07539FA9    /* 10^-256  */
};
#endif /* BYTE_ORDER != LITTLE_ENDIAN */


/*
 * NAME: watof
 *                                                                    
 * FUNCTION: convert an ascii string to a float/double
 *                                                                    
 * NOTES:
 *
 *  RIOS Specific Versions of:
 *
 *  watof --   Ascii string to double conversion
 *  wstrtod -- Ascii string to double conversion
 *
 *
 *   The following descriptions are extracted from the proposed ANSI C
 *   standard.
 *
 * ***********************************************************************
 *
 *   double wstrtod(const NLchar *nptr, char ** endptr)
 *
 *   "The strtod function converts the initial portion of the
 *   string pointed to by nptr to double representation. First it
 *   decomposes the input string into three parts: an initial,
 *   possibly empty, sequence of white-space characters (as specified
 *   by the isspace function), a subject sequence resembling a floating
 *   point constant; and a final string of one unrecognized character,
 *   including the terminating null character of the input string. Then
 *   it attempts to convert the subject sequence to a floating-point
 *   number, and returns the result.
 *
 *   The expected form of the subject sequence is an optional plus or
 *   minus sign, then a sequence of digits optionally containing a decimal
 *   point character, then an optional exponent part ...
 *
 *   ... if neither an exponent part nor a decimal point character
 *   appears, a decimal point is assumed to follow the last digit in the
 *   string. ... A pointer to the final string is stored in the object
 *   pointed to by endptr, provided that endptr is not a null pointer.
 *   ....
 *
 *   If the subject sequence is empty or does not have the expected form,
 *   no conversion is performed; the value of nptr is stored in the object
 *   pointed to by endptr, provided that endptr is not a null pointer."
 *
 *   "The strtod function returns the converted value, if any. If no
 *   conversion could be performed, zero is returned. If the correct
 *   value would cause overflow, plus or minus HUGE_VAL is returned.
 *   (according to the sign of the value), and the value of the macro
 *   ERANGE is stored in errno. If the correct value would cause underflow,
 *   zero is returned and the value of the macro ERANGE is stored in errno."
 *
 *   On RIOS HUGE_VAL is infinity; +0.0 is returned if no conversion
 *   is possible; and a properly signed 0.0 is returned on underflow.
 *
 *   The RIOS version will also recognize the strings: "NaNQ", "NaNS", and
 *   "infinity" and convert them to the proper IEEE value. If NaNQ or NaNS is
 *   preceeded by a sign, it is ignored. If 2infinity is preceeded by a sign
 *   then a properly signed infinity is generated.
 *
 * ***********************************************************************
 *
 *   double watof(const NLchar *nptr)
 *
 *   "The atof function converts the initial portion of the string pointed
 *   to by nptr to double representation. Except for the behavior on error
 *   it is equivalent to:
 *
 *              wstrtod(nptr, (NLchar **)NULL) "
 *
 * ***********************************************************************
 *
 * RETURNS: a double value
 *
 */
#ifdef KJI

#if STRTOD

double wstrtod(const NLchar *nptr, NLchar **endptr)

#else

/* Must be ATOF */
double watof (const NLchar *nptr)

#endif

{
	NLchar ch;                      /* Current character                    */
	int saverm;                   /* Caller's round mode.                 */
	int exponent;                 /* Decimal exp. returned from _dsto2fp   */
	int class;                    /* Result class type returned from " "  */
	unsigned int  i;              /* temp array index          */
	union { 
		double rslt[2];       /* result: hi and low parts  */
		long int i[4];
	} x;
	double sign = 1.0;             /* +1.0 = '+', -1.0 = '-' ; assume +   */
	NLchar *valid_addr;              /* a valid address for endptr to point */
	double dblrslt;
	float  fltrslt;

#if STRTOD
	/* If endptr points to a null then point it to a valid address so */
	/* we don't have to worry about it later.                         */
	if ( endptr == (NLchar **)0) endptr = &valid_addr;
#else
	NLchar **endptr = &valid_addr;
#endif

	saverm = write_rnd(FP_RND_RN);        /* set round mode=RN,save mode */

	while (NCisspace(ch = *nptr) ) nptr++;  /* skip leading whitespace */

        ch = isascii(ch)? ch: _jistoa(ch);
	switch (ch) {                     /* Look for + or - sign            */
	case '-': 
		sign = -sign;        /* set sign to -1.0, fall thru     */
	case '+': 
		nptr++;              /* bump nptr to skip + or - sign   */
	}

   /*
    * _dsto2fp reads in the rest of the string and creates two RIOS double
    * fp values to represent the base string. It also returns an
    * exponent (base 10) to be used to scale the base string.
    * The return value is a pointer to the character that ended the
    * scan ( i.e., the character after the string if everything went
    * okay.) If the first character was not valid then it returns nptr.
    */

	*endptr = _wdsto2fp( nptr, x.rslt, &exponent, &class);

	/* Look for all the special numbers _dsto2fp could have returned */

	switch (class) {
	case FP_PLUS_ZERO:
	case FP_PLUS_INF:
		write_rnd(saverm);
		return(sign * x.rslt[0]);
	case FP_QNAN:
	case FP_SNAN:
		write_rnd(saverm);
		return(copysign (x.rslt[0], sign));
	}


  /*
   *  Multiply the pair of fp numbers returned by _dsto2fp by the power
   *  of 10 (returned in exponent). If  -63 < exponent < 63 then
   *  only one multiply is needed. If exponent > 63 or < -63 and
   *  > -320 then only 2 multiplies will be needed. Three multiplies
   *  are only needed when the exponent < -320.
   *
   *  We don't get here unless the pair of fp numbers is finite and
   *  non-zero.  If the result of the multiplies is infinity
   *  then an overflow occurred and we should set errno = ERANGE
   *  if strtod or strtof. If the result of the multiplies is a zero
   *  then underflow occurred and we should also set errno = ERANGE for
   *  strtod or strtof.
   *
   *  NOTE: The multiplies below can cause the overflow bit to be set
   *        even tho the final result will not overflow. Only a very
   *        miniscule number of values will have this problem and
   *        only in the directed rounding modes.
   *        The fix for this would be to save the fp status before the
   *        multiplies and then restore it immediately afterwords.
   *        Unfortunately this takes a lot of cycles for something that
   *        almost never occurs and when it does the extraneous flag
   *        error is pretty harmless. However, the save and restore
   *        probably should be inserted if we get the C built in's to
   *        save & restore the FPSCR because then the speed penalty would
   *        be minimal. --- jb 12/22/88
   */

	if (exponent >= 0) {  /* positive exponent */
		if (exponent > 319)
			exponent = 319;
		i = ((exponent&0x3f) << 1);
		M2DBLS(pospow1,i);
		if (i = ((exponent&0x3c0) >> 5))   /* Don't mult. if zero */
		{ 
			M2DBLS(pospow2,i); 
		}
	}
	else {  /* negative exponent */
		if (exponent < -383) 
			exponent = 383; 
		else 
			exponent = -exponent;
		i = ((exponent&0x3f) << 1);
		M2DBLS(negpow1,i);
		if (i = ((exponent&0x3c0) >> 5)) {  /* Don't mult if zero  */
			if ( i > 8 ) {              /* if exp < -256       */
				M2DBLS(negpow2,8);  /* mult by -256 first  */
				i -= 8;             /* then by what's left */
			}
			M2DBLS(negpow2,i);
		}
	}

	/*  Round to double.                                                 */
	/*  Add low part to high part (usually this only affects the low     */
	/*  order bits of the high part) using the caller's round mode.      */

	/* Restore caller's round mode for last operation  */
	write_rnd(saverm); 	/* restore caller's round mode. */
	dblrslt = sign*x.rslt[0] + sign*x.rslt[1];
	if ((dblrslt == 0.0) || !FINITE(dblrslt))
		errno = ERANGE;
	if (dblrslt != dblrslt) {
		errno = ERANGE;
		dblrslt = copysign (HUGE_VAL, dblrslt);
	}
	return (dblrslt);


}

#else /* the non-KJI stub */

#if STRTOD
double wstrtod(const NLchar *nptr, NLchar **endptr)

#else

double watof(NLchar *nptr)

#endif

{
}

#endif /* KJI */
