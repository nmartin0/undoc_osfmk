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
 * OSF/1 Release 1.0.1
 */
/* @(#)$RCSfile: a.out.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:41:14 $ */

#ifndef _A_OUT_H_
#define _A_OUT_H_

#ifdef  _COFF
#include <coff.a.out.h>
#else
#include <bsd.a.out.h>
#endif

#endif /* _A_OUT_H_ */
