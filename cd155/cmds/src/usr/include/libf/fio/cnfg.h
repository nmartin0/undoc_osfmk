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

/* cnfg.h - declare structure which may alter the configuration */


/*  declare external variables/arrays used by Fortran I/O:  */

extern int	__PC_DOS;

typedef struct {
    char       *default_name; /* sprintf string for default file name */
    int         true_mask;    /* 1 => odd is true, -1 => nonzero is true */
} FIO_CNFG;

extern FIO_CNFG  __fio_cnfg_;	/* ending '_' so it's accessible by the
				 * fortran programmer */

extern void      __fio_scratch_name();
