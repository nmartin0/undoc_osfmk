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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */

/** Copyright (c) 1990  Mentat Inc.
 ** tlistate.h 1.1, last change 4/14/90
 **/

#ifndef	_TLISTATE_
#define	_TLISTATE_

#include <tli/iostate.h>

struct tli_st {
	int	tlis_state;		/* state */
	int	tlis_servtype;		/* server type */
	int	tlis_flags;		/* BLC sez: now we're real! */
	long    tlis_etsdu_size;        /* Transport service data unit size */
        long    tlis_tsdu_size;         /* Transport service data unit size */
        long    tlis_tidu_size;         /* Transport individual data unit size */

};

/* Flags! */
#define	TLIS_DATA_STOPPED	1
#define	TLIS_EXDATA_STOPPED	2

#ifdef _NO_PROTO
extern  int     iostate_sw();
#else
extern  int     iostate_sw(int fd, struct tli_st *tlis, int cmd, int flags);
#endif

#endif	/*_TLISTATE_*/
