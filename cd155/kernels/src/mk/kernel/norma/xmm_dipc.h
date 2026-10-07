/*
 *
 * $Copyright
 * Copyright 1994 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */

/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1994 Intel Corporation.
 *
 *	$Id: xmm_dipc.h,v 1.2 1994/11/18 20:56:36 mtm Exp $
 */

#ifndef	_XMM_DIPC_H_
#define	_XMM_DIPC_H_

#if	NORMA2
#include <norma2/norma_log.h>

#define xmm_log1	NORMA_LOG1
#define xmm_log2	NORMA_LOG2
#define xmm_log3	NORMA_LOG3
#define xmm_log4	NORMA_LOG4
#define xmm_log5	NORMA_LOG5
#define xmm_log6	NORMA_LOG6

#define xmm_entry0(func) \
	NORMA_ENTRY0(XMM_LOG_ID, func)
#define xmm_entry1(func, a)      \
	NORMA_ENTRY1(XMM_LOG_ID, func, a)
#define xmm_entry2(func, a, b) \
	NORMA_ENTRY2(XMM_LOG_ID, func, a, b)
#define xmm_entry3(func, a, b, c) \
	NORMA_ENTRY3(XMM_LOG_ID, func, a, b, c)
#define xmm_entry4(func, a, b, c, d) \
	NORMA_ENTRY4(XMM_LOG_ID, func, a, b, c, d)
#define xmm_entry5(func, a, b, c, d, e) \
	NORMA_ENTRY5(XMM_LOG_ID, func, a, b, c, d, e)
#define xmm_entry6(func, a, b, c, d, e, f) \
	NORMA_ENTRY6(XMM_LOG_ID, func, a, b, c, d, e, f)
#define xmm_entry7(func, a, b, c, d, e, f, g) \
	NORMA_ENTRY7(XMM_LOG_ID, func, a, b, c, d, e, f, g)
#define xmm_entry8(func, a, b, c, d, e, f, g, h) \
	NORMA_ENTRY8(XMM_LOG_ID, func, a, b, c, d, e, f, g, h)

#else	/* NORMA2 */

#define xmm_entry0(func)
#define xmm_entry1(func, a)
#define xmm_entry2(func, a, b)
#define xmm_entry3(func, a, b, c)
#define xmm_entry4(func, a, b, c, d)
#define xmm_entry5(func, a, b, c, d, e)
#define xmm_entry6(func, a, b, c, d, e, f)
#define xmm_entry7(func, a, b, c, d, e, f, g)
#define xmm_entry8(func, a, b, c, d, e, f, g, h)

#endif	/* NORMA2 */
#endif	/* _XMM_DIPC_H_ */
