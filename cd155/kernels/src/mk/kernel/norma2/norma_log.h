/*
 * 
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
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
 * HISTORY:
 * $Log: norma_log.h,v $
 * Revision 1.7  1995/02/28  00:56:32  sean
 *  Reviewer: terry
 *  Risk:		low
 *  Benefit or PTS #: 11684
 *  Testing:	sat byte (3 copy x 3 iterations), sat parallel (3 iterations)
 *  Module(s):	norma2/norma_log.h norma2/kmsg_dequeue.c
 *
 * fix:            modified norma_log.h so that __FUNC__ is defined only when
 *                 MACH_ASSERT is defined.  The assert definition of __FUNC__
 *                 is ok.  In the rare case when usage is defined outside of
 *                 the MACH_ASSERT definition, __FUNC__ was replaced by
 *                 it  string.
 *
 * Revision 1.6  1994/11/18  20:58:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/08/31  21:25:36  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.2.2.2  1994/08/08  23:55:12  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.4  1994/08/08  19:34:54  andyp
 * PTS #:	10338, 10339
 * Mandatory?:	Yes
 * Description: Don't issue proxy_lock_completed() until all expected
 * 	proxy_data_write_completed()'s have been received.
 * 	Added XMM function entry logging to the norma log ("show norma").
 * 	Added NORMA_LOG_ONLY bootmagic to log exactly one module id.
 * 	Upped the priority of the dipc_emmi_reply_threads above
 * 	that of ordinary dipc_kobj_server_threads.
 * Reviewer(s): rkl
 * Risk:	Low (compared to getting sporadic 0's or truncated files)
 * Testing:	sats, devloper tests, test cases pass.
 * Module(s):
 * 	M intel/pmap.c
 * 	M norma/xmm.c
 * 	M norma/xmm_buffer.c
 * 	M norma/xmm_copy.c
 * 	M norma/xmm_export.c
 * 	M norma/xmm_import.c
 * 	M norma/xmm_interpose.c
 * 	M norma/xmm_invalid.c
 * 	M norma/xmm_object.c
 * 	M norma/xmm_server.c
 * 	M norma/xmm_split.c
 * 	M norma/xmm_svm.c
 * 	M norma/xmm_user.c
 * 	A norma/xmm_dipc.h
 * 	M norma2/dipc_kserver.c
 * 	M norma2/norma_log.c
 * 	M norma2/norma_log.h
 *
 * Revision 1.3  1994/08/05  15:27:59  rkl
 * Revision 1.2.2.1  1994/08/05  15:26:23  rkl
 *  The MAX_LOG_ID define was off by one causing certain arrays to be incorrectly
 *  sized and dead name log event to be dropped.
 *
 *  Reviewer: andyp
 *  Risk: low
 *  Benefit or PTS #: 10512
 *  Testing: Use "show norma" debugger command
 *  Module(s): norma_log.h
 *
 * Revision 1.2  1994/07/12  21:30:24  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.20  1994/07/11  16:50:57  rkl
 *  New and improved logging.
 *
 * Revision 1.1.2.19  1994/06/09  02:47:27  stans
 * added DIPC_PORT_LOG() macro
 *
 * Revision 1.1.2.18  1994/05/07  01:35:26  rkl
 *  Extended norma logging to show thread and interrupt nesting level.
 *
 * Revision 1.1.2.17  1994/05/05  02:56:15  stans
 *   DEAD_NAME_LOG_ID
 *   dipc_port_log() prototype.
 *
 * Revision 1.1.2.16  1994/04/27  23:17:30  rkl
 *  Added support for __FUNC__ #define for logging.
 *
 * Revision 1.1.2.15  1994/04/08  15:24:10  rkl
 *  Added support for dipc_notify module.
 *
 * Revision 1.1.2.14  1994/03/30  19:54:51  andyp
 * Fixed a typo in the NORMA_ENTRY6() macro under __STDC__.
 *
 * Revision 1.1.2.13  1994/03/30  19:35:37  rkl
 *  Fixed macro fob found by __STDC__ compiler.
 *
 * Revision 1.1.2.12  1994/03/30  18:53:05  rkl
 *  Expanded the number of items that can be logged with a single entry.
 *
 * Revision 1.1.2.11  1994/03/30  00:16:25  stans
 * recv & dequeue
 *
 * Revision 1.1.2.10  1994/03/22  18:33:18  rkl
 *  Added debug logging support to dipc_server and dipc_client
 *
 * Revision 1.1.2.9  1994/03/21  21:31:12  andyp
 * Fixed a typo in the NORMA_ENTRY4() macro.
 *
 * Revision 1.1.2.8  1994/03/19  19:15:53  stans
 * kobj server log
 *
 * Revision 1.1.2.7  1994/03/19  03:04:50  rkl
 *  Changed non __STDC__ entry macros to cache function name string after
 *  first lookup.  This safes 30msec!!!! per function entry when this level
 *  is being logged.
 *
 * Revision 1.1.2.6  1994/03/18  18:53:17  stans
 * added dipic_migrate & kmsg_convert
 *
 * Revision 1.1.2.5  1994/03/18  00:38:46  rkl
 *  Added PORT_LOG_ID
 *
 * Revision 1.1.2.4  1994/03/15  23:17:11  andyp
 * Moved the MACH_ASSERT #if for compiling without asserts.
 *
 * Revision 1.1.2.3  1994/03/15  00:29:54  rkl
 *  Added ENTRY_LOG_LEVEL as a #define
 *
 * Revision 1.1.2.2  1994/03/15  00:14:39  rkl
 *  Support for buffer based norma log.
 *
 * Revision 1.1.2.1  1994/02/24  00:25:48  rkl
 *  Resolved problems (and bugs) reported by lint(1).
 *
 * Revision 1.1  1994/02/01  01:01:46  rkl
 * Initial revision
 *
 *
 */

#ifndef	_NORMA_LOG_H_
#define	_NORMA_LOG_H_

#include <mach_assert.h>

/*
 *  Buffered log record structure.
 */
typedef	struct log_entry {
	char	*str;			/* format string */
	unsigned long	a;		/* val 1 */
	unsigned long	b;		/* val 2 */
	unsigned long	c;		/* val 3 */
	unsigned long	d;		/* val 4 */
	unsigned long	e;		/* val 5 */
	unsigned long	f;		/* val 6 */
	unsigned long	g;		/* val 7 */
	unsigned long	h;		/* val 8 */
	unsigned long	thread;		/* thread */
	unsigned long	time_stamp;	/* time stamp */
	unsigned short	intr;		/* interrupt nesting level */
	unsigned short	cpu;		/* CPU logging entry */
} log_entry_t;

typedef unsigned long _log_level_t;

/*
 *  Module ID's.
 *
 *  An ID can not be > 31
 */
#define	MQUEUE_LOG_ID		0
#define	KMSG_PARSE_LOG_ID	1
#define	UID_LOG_ID		2
#define	TRANSIT_LOG_ID		3
#define	TX_ENGINE_LOG_ID	4
#define	PORT_LOG_ID		5
#define	MIGRATE_LOG_ID		6
#define	KMSG_CONVERT_LOG_ID	7
#define	KSERVER_LOG_ID		8
#define	SERVER_LOG_ID		9
#define	CLIENT_LOG_ID		10
#define	KMSG_DEQUEUE_LOG_ID	11
#define	RX_ENGINE_LOG_ID	12
#define	NOTIFY_LOG_ID		13
#define DEAD_NAME_LOG_ID	14
#define	XMM_LOG_ID		15

#define	MAX_LOG_ID		16

/*
 *  Reserved log levels
 */
#define	ENTRY_LOG_LEVEL		31

/*
 *  The macros
 */
extern	void		(*norma_log)();
extern	char*		norma_log_name();
extern	_log_level_t	dipc_log_modules[];

#define bit(x)			((_log_level_t)((_log_level_t)(1) << x))
#define	INTERESTING(lev)	(_mod_level_ & bit(lev))

#if	MACH_ASSERT

#define NORMA_LOG1(lev, str)						\
	if(INTERESTING(lev))						\
		(*norma_log)(str)
#define NORMA_LOG2(lev, str, a)						\
	if(INTERESTING(lev))						\
		(*norma_log)(str, a)
#define NORMA_LOG3(lev, str, a, b)					\
	if(INTERESTING(lev))						\
		(*norma_log)(str, a, b)
#define NORMA_LOG4(lev, str, a, b, c)					\
	if(INTERESTING(lev))						\
		(*norma_log)(str, a, b, c)
#define NORMA_LOG5(lev, str, a, b, c, d)				\
	if(INTERESTING(lev))						\
		(*norma_log)(str, a, b, c, d)
#define NORMA_LOG6(lev, str, a, b, c, d, e)				\
	if(INTERESTING(lev))						\
		(*norma_log)(str, a, b, c, d, e)
#define NORMA_LOG7(lev, str, a, b, c, d, e, f)				\
	if(INTERESTING(lev))						\
		(*norma_log)(str, a, b, c, d, e, f)
#define NORMA_LOG8(lev, str, a, b, c, d, e, f, g)			\
	if(INTERESTING(lev))						\
		(*norma_log)(str, a, b, c, d, e, f, g)
#define NORMA_LOG9(lev, str, a, b, c, d, e, f, g, h)			\
	if(INTERESTING(lev))						\
		(*norma_log)(str, a, b, c, d, e, f, g, h)

#define	__FUNC__	_func_name_

#define	NORMA_LOG_PREAMBLE(mod_id, func)				\
	_log_level_t	 _mod_level_ = dipc_log_modules[mod_id];	\
	static char	*_func_name_ = 0;				\
	if (_func_name_ == 0)						\
		_func_name_ = norma_log_name((void(*)())func)

#if	__STDC__

#define NORMA_ENTRY0(mod_id, func)					\
	NORMA_LOG_PREAMBLE(mod_id, func);				\
	NORMA_LOG1(ENTRY_LOG_LEVEL,					\
		#func"()\n")
#define NORMA_ENTRY1(mod_id, func, a)					\
	NORMA_LOG_PREAMBLE(mod_id, func);				\
	NORMA_LOG2(ENTRY_LOG_LEVEL,					\
		#func"("#a"=%x)\n", a)
#define NORMA_ENTRY2(mod_id, func, a, b)				\
	NORMA_LOG_PREAMBLE(mod_id, func);				\
	NORMA_LOG3(ENTRY_LOG_LEVEL,					\
		#func"("#a"=%x "#b"=%x)\n",				\
				a, b)
#define NORMA_ENTRY3(mod_id, func, a, b, c)				\
	NORMA_LOG_PREAMBLE(mod_id, func);				\
	NORMA_LOG4(ENTRY_LOG_LEVEL,					\
		#func"("#a"=%x "#b"=%x "#c"=%x)\n",			\
				a, b, c)
#define NORMA_ENTRY4(mod_id, func, a, b, c, d)				\
	NORMA_LOG_PREAMBLE(mod_id, func);				\
	NORMA_LOG5(ENTRY_LOG_LEVEL,					\
		#func"("#a"=%x "#b"=%x "#c"=%x "#d"=%x)\n",		\
				a, b, c, d)
#define NORMA_ENTRY5(mod_id, func, a, b, c, d, e)			\
	NORMA_LOG_PREAMBLE(mod_id, func);				\
	NORMA_LOG6(ENTRY_LOG_LEVEL,					\
		#func"("#a"=%x "#b"=%x "#c"=%x "#d"=%x "#e"=%x)\n",	\
				a, b, c, d, e)
#define NORMA_ENTRY6(mod_id, func, a, b, c, d, e, f)			\
	NORMA_LOG_PREAMBLE(mod_id, func);				\
	NORMA_LOG7(ENTRY_LOG_LEVEL,					\
	#func"("#a"=%x "#b"=%x "#c"=%x "#d"=%x "#e"=%x "#f"=%x)\n",	\
				a, b, c, d, e, f)
#define NORMA_ENTRY7(mod_id, func, a, b, c, d, e, f, g)			\
	NORMA_LOG_PREAMBLE(mod_id, func);				\
	NORMA_LOG8(ENTRY_LOG_LEVEL,					\
#func"("#a"=%x "#b"=%x "#c"=%x "#d"=%x "#e"=%x "#f"=%x "#g"=%x)\n",	\
				a, b, c, d, e, f, g)
#define NORMA_ENTRY8(mod_id, func, a, b, c, d, e, f, g, h)		\
	NORMA_LOG_PREAMBLE(mod_id, func);				\
	NORMA_LOG9(ENTRY_LOG_LEVEL,					\
#func"("#a"=%x "#b"=%x "#c"=%x "#d"=%x "#e"=%x "#f"=%x "#g"=%x "#h"=%x)\n",\
				a, b, c, d, e, f, g, h)

#else	/* __STDC__ */

#define	_no_stdc_hack_ \
	if (INTERESTING(ENTRY_LOG_LEVEL)) (*norma_log)

#define NORMA_ENTRY0(mod_id, func)				\
	NORMA_LOG_PREAMBLE(mod_id, func);			\
	NORMA_LOG2(ENTRY_LOG_LEVEL,				\
		"%s()\n",					\
		_func_name_)
#define NORMA_ENTRY1(mod_id, func, a)				\
	NORMA_LOG_PREAMBLE(mod_id, func);			\
	NORMA_LOG3(ENTRY_LOG_LEVEL,				\
		"%s( %x )\n",					\
		_func_name_, a)
#define NORMA_ENTRY2(mod_id, func, a, b)			\
	NORMA_LOG_PREAMBLE(mod_id, func);			\
	NORMA_LOG4(ENTRY_LOG_LEVEL,				\
		"%s( %x %x )\n",				\
		_func_name_, a, b)
#define NORMA_ENTRY3(mod_id, func, a, b, c)			\
	NORMA_LOG_PREAMBLE(mod_id, func);			\
	NORMA_LOG5(ENTRY_LOG_LEVEL,				\
		"%s( %x %x %x )\n",				\
		_func_name_, a, b, c)
#define NORMA_ENTRY4(mod_id, func, a, b, c, d)			\
	NORMA_LOG_PREAMBLE(mod_id, func);			\
	NORMA_LOG6(ENTRY_LOG_LEVEL,				\
		"%s( %x %x %x %x )\n",				\
		_func_name_, a, b, c, d)
#define NORMA_ENTRY5(mod_id, func, a, b, c, d, e)		\
	NORMA_LOG_PREAMBLE(mod_id, func);			\
	NORMA_LOG7(ENTRY_LOG_LEVEL,				\
		"%s( %x %x %x %x %x )\n",			\
		_func_name_, a, b, c, d, e)
#define NORMA_ENTRY6(mod_id, func, a, b, c, d, e, f)		\
	NORMA_LOG_PREAMBLE(mod_id, func);			\
	NORMA_LOG8(ENTRY_LOG_LEVEL,				\
		"%s( %x %x %x %x %x %x )\n",			\
		_func_name_, a, b, c, d, e, f)
#define NORMA_ENTRY7(mod_id, func, a, b, c, d, e, f, g)		\
	NORMA_LOG_PREAMBLE(mod_id, func);			\
	NORMA_LOG9(ENTRY_LOG_LEVEL,				\
		"%s( %x %x %x %x %x %x %x )\n",			\
		_func_name_, a, b, c, d, e, f, g)
/** NORMA_ENTRY8() not available in non-__STD__ environments **/
#endif	/* __STD__ */

extern void dipc_port_log( char *, char *, ipc_port_t );

#define DIPC_PORT_LOG(msg,port) dipc_port_log(__FUNC__, msg, port)

#else	/* MACH_ASSERT */


#define NORMA_LOG1(lev, str)
#define NORMA_LOG2(lev, str, a)
#define NORMA_LOG3(lev, str, a, b)
#define NORMA_LOG4(lev, str, a, b, c)
#define NORMA_LOG5(lev, str, a, b, c, d)
#define NORMA_LOG6(lev, str, a, b, c, d, e)
#define NORMA_LOG7(lev, str, a, b, c, d, e, f)
#define NORMA_LOG8(lev, str, a, b, c, d, e, f, g)
#define NORMA_LOG9(lev, str, a, b, c, d, e, f, g, h)

#define NORMA_ENTRY0(mod_id, func)
#define NORMA_ENTRY1(mod_id, func, a)
#define NORMA_ENTRY2(mod_id, func, a, b)
#define NORMA_ENTRY3(mod_id, func, a, b, c)
#define NORMA_ENTRY4(mod_id, func, a, b, c, d)
#define NORMA_ENTRY5(mod_id, func, a, b, c, d, e)
#define NORMA_ENTRY6(mod_id, func, a, b, c, d, e, f)
#define NORMA_ENTRY7(mod_id, func, a, b, c, d, e, f, g)
#define NORMA_ENTRY8(mod_id, func, a, b, c, d, e, f, g, h)

#define dipc_port_log(a,b,c)
#define DIPC_PORT_LOG(msg,port)

#endif  /* MACH_ASSERT */

#endif	/* _NORMA_LOG_H_ */
