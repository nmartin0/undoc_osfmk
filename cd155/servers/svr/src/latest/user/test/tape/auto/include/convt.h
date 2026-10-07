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
 *	convt.h 11.1 94/03/22 16:48:54
 */
static char	convth_ver[] = "@(#) sourcefile convt.h 11.1 94/03/22 16:48:54";

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

/* CONVT.H:
 *
 * This include file contains data type definitions
 * for the data conversion routines.
 *
 */

struct s_size {

  long slow;
  long shigh;
};

typedef struct s_size esize_t;

/* proto-types for the extended integer math routines */

esize_t 	_eadd(esize_t a, esize_t b);
esize_t 	_eadd1(esize_t a, long b);
esize_t   _esub(esize_t a, esize_t b);
esize_t   _esub1(esize_t a, long b);
esize_t   _emul(esize_t a, long b);
long      _ediv(esize_t a, long b);
long      _emod(esize_t a, long b);
long      _ecmp(esize_t a, esize_t b);
long      _etos(esize_t a, char *s);
