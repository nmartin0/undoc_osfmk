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
 *	Copyright 1991, The Portland Group, Incorporated.
 *	All rights reserved.
 *
 *	THE PORTLAND GROUP, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with The Portland Group and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

/* assign.h - declare structure representing a value found during
   list-directed/namelist read.  This value is stored by __fio_assign. */

typedef struct atag {
    int		dtype;	/* TY_INT, TY_LOG, TY_DBLE, TY_DCMPLX, TY_(N)CHAR */
    union {		/* value: depends on dtype */
	int	i;		/* TY_INT, TY_LOG */
	double	d;		/* TY_DBLE */
	struct {		/* TY_CHAR, TY_NCHAR */
	    int		len;	/* length of string */
	    char	*str;	/* ptr to its characters */
	} c;
	struct atag *cmplx;	/* TY_DCMPLX: ptr to 2 element TKNVAL, */
				/* [0] - real, [1] - imag, both are TY_DBLE */
    } val;
} AVAL;
