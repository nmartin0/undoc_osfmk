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
 * SSD HISTORY
 * $Log: xmm_ktrace.h,v $
 * Revision 1.3  1994/11/18  20:56:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/03/29  12:44:44  fritz
 * Initial version (derived from ASVM)
 *
 *
 * END SSD HISTORY
 */

/*
 *  YAKT (see xmm_ktrace.c) definitions.
 */

#ifndef _SVM_KTRACE_H_
#define _SVM_KTRACE_H_

#include <mach_assert.h>
#include <mach_kdb.h>

#if MACH_KDB

/*
 *  Can use the high resolution timer of the node boards
 */

#if PARAGON860
#include <i860paragon/dp.h>
#endif PARAGON860

extern unsigned kt_mode;

#define kt_printf if(kt_mode) x_kt_printf

#define KT_MODE_TIME   0x1
#define KT_MODE_PARAM  0x2
#define KT_UNDEFINED   0xffffffff

#else MACH_KDB

#define kt_printf

#endif MACH_KDB

#endif _SVM_KTRACE_H_
