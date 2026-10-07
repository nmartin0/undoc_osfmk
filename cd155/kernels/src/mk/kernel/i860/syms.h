/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * Copyright 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: syms.h,v $
 * Revision 2.5  1994/11/18  20:39:46  mtm
 * Copyright additions/changes
 *
 * Revision 2.4  1994/07/12  19:18:47  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 2.3.8.2  1994/05/23  17:21:05  stans
 * prevent duplicate inclusion
 *
 * Revision 2.3.8.1  1994/05/23  17:02:59  stans
 * syment typedef NULL
 *
 * Revision 2.3  1993/06/30  22:31:34  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.2  1992/09/22  17:38:04  SSD
 * *** empty log message ***
 *
 * Revision 2.2  91/12/10  16:31:08  jsb
 * 	New files from Intel
 * 	[91/12/10  16:11:47  jsb]
 * 
 */ 

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)syms.h	10.1" */    /* sgs-inc:common/syms.h	1.7 */

#ifndef	_I860_SYMS_H_
#define	_I860_SYMS_H_ 1

/*		Storage Classes are defined in storclass.h  */
#ifdef MSDOS
#include "storclas.h"
#else
#include "storclass.h"
#endif


/*		Number of characters in a symbol name */
#define  SYMNMLEN	8
/*		Number of characters in a file name */
#define  FILNMLEN	14
/*		Number of array dimensions in auxiliary entry */
#define  DIMNUM		4


struct syment
{
	union
	{
		char		_n_name[SYMNMLEN];	/* old COFF version */
		struct
		{
			long	_n_zeroes;	/* new == 0 */
			long	_n_offset;	/* offset into string table */
		} _n_n;
		char		*_n_nptr[2];	/* allows for overlaying */
	} _n;
	long			n_value;	/* value of symbol */
	short			n_scnum;	/* section number */
	unsigned short	n_type;		/* type and derived type */
	char			n_sclass;	/* storage class */
	char			n_numaux;	/* number of aux. entries */
};

#define n_name		_n._n_name
#define n_nptr		_n._n_nptr[1]
#define n_zeroes	_n._n_n._n_zeroes
#define n_offset	_n._n_n._n_offset

typedef struct syment	*syment_t;

#define	SYMENT_NULL (syment_t)0
/*
   Relocatable symbols have a section number of the
   section in which they are defined.  Otherwise, section
   numbers have the following meanings:
*/
        /* undefined symbol */
#define  N_UNDEF	0
        /* value of symbol is absolute */
#define  N_ABS		-1
        /* special debugging symbol -- value of symbol is meaningless */
#define  N_DEBUG	-2
	/* indicates symbol needs transfer vector (preload) */
#define  N_TV		(unsigned short)-3

	/* indicates symbol needs transfer vector (postload) */

#define  P_TV		(unsigned short)-4

/*
   The fundamental type of a symbol packed into the low 
   4 bits of the word.
*/

#define  _EF	".ef"

#define  T_NULL     0
#define  T_ARG      1          /* function argument (only used by compiler) */
#define  T_CHAR     2          /* character */
#define  T_SHORT    3          /* short integer */
#define  T_INT      4          /* integer */
#define  T_LONG     5          /* long integer */
#define  T_FLOAT    6          /* floating point */
#define  T_DOUBLE   7          /* double word */
#define  T_STRUCT   8          /* structure  */
#define  T_UNION    9          /* union  */
#define  T_ENUM     10         /* enumeration  */
#define  T_MOE      11         /* member of enumeration */
#define  T_UCHAR    12         /* unsigned character */
#define  T_USHORT   13         /* unsigned short */
#define  T_UINT     14         /* unsigned integer */
#define  T_ULONG    15         /* unsigned long */

/*
 * derived types are:
 */

#define  DT_NON      0          /* no derived type */
#define  DT_PTR      1          /* pointer */
#define  DT_FCN      2          /* function */
#define  DT_ARY      3          /* array */

/*
 *   type packing constants
 */

#define  N_BTMASK     017
#define  N_TMASK      060
#define  N_TMASK1     0300
#define  N_TMASK2     0360
#define  N_BTSHFT     4
#define  N_TSHIFT     2

/*
 *   MACROS
 */

	/*   Basic Type of  x   */

#define  BTYPE(x)  ((x) & N_BTMASK)

	/*   Is  x  a  pointer ?   */

#define  ISPTR(x)  (((x) & N_TMASK) == (DT_PTR << N_BTSHFT))

	/*   Is  x  a  function ?  */

#define  ISFCN(x)  (((x) & N_TMASK) == (DT_FCN << N_BTSHFT))

	/*   Is  x  an  array ?   */

#define  ISARY(x)  (((x) & N_TMASK) == (DT_ARY << N_BTSHFT))

	/* Is x a structure, union, or enumeration TAG? */

#define ISTAG(x)  ((x)==C_STRTAG || (x)==C_UNTAG || (x)==C_ENTAG)

#define  INCREF(x) ((((x)&~N_BTMASK)<<N_TSHIFT)|(DT_PTR<<N_BTSHFT)|(x&N_BTMASK))

#define  DECREF(x) ((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))

/*
 *	AUXILIARY ENTRY FORMAT
 */

union auxent
{
	struct
	{
		long		x_tagndx;	/* str, un, or enum tag indx */
		union
		{
			struct
			{
				unsigned short	x_lnno;	/* declaration line number */
				unsigned short	x_size;	/* str, union, array size */
			} x_lnsz;
			long	x_fsize;	/* size of function */
		} x_misc;
		union
		{
			struct			/* if ISFCN, tag, or .bb */
			{
				long	x_lnnoptr;	/* ptr to fcn line # */
				long	x_endndx;	/* entry ndx past block end */
			} 	x_fcn;
			struct			/* if ISARY, up to 4 dimen. */
			{
				unsigned short	x_dimen[DIMNUM];
			} 	x_ary;
		}		x_fcnary;
		unsigned short  x_tvndx;		/* tv index */
	} 	x_sym;
	struct
	{
		char	x_fname[FILNMLEN];
	} 	x_file;
        struct
        {
                long    x_scnlen;          /* section length */
                unsigned short  x_nreloc;  /* number of relocation entries */
                unsigned short  x_nlinno;  /* number of line numbers */
        }       x_scn;

	struct
	{
		long		x_tvfill;	/* tv fill value */
		unsigned short	x_tvlen;	/* length of .tv */
		unsigned short	x_tvran[2];	/* tv range */
	}	x_tv;	/* info about .tv section (in auxent of symbol .tv)) */
};

#define	SYMENT	struct syment
#define	SYMESZ	18	/* sizeof(SYMENT) */

#define	AUXENT	union auxent
#define	AUXESZ	18	/* sizeof(AUXENT) */

/*	Defines for "special" symbols   */

#if VAX
#define _ETEXT	"_etext"
#define	_EDATA	"_edata"
#define	_END	"_end"
#else
#define _ETEXT	"etext"
#define _EDATA	"edata"
#define _END	"end"
#endif

#define _START	"_start"

#define _TVORIG	"_tvorig"
#define _TORIGIN	"_torigin"
#define _DORIGIN	"_dorigin"

#define _SORIGIN	"_sorigin"

#endif	/* _I860_SYMS_H_ */
