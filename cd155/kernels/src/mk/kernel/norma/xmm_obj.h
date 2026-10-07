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
 * $Log: xmm_obj.h,v $
 * Revision 1.9  1994/11/18  20:56:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/07/12  19:25:17  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.7.4.1  1994/06/30  00:44:23  stans
 *   Removed trailing ';' from xmm_obj_reference(obj) macro definition.
 *
 * Revision 1.7  1993/09/28  18:06:09  andyp
 * Update for the 1.2 release.
 *
 *
 *	- Define PENDING_TRACKING option to follow data_write_completed
 *	activity through the layers of an XMM stack.
 *	- Add fields and define macros for object db_print methods.
 *	[alanl@osf.org]
 *
 * Revision 1.6  1993/07/22  02:21:36  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.5  1993/07/09  22:28:54  dleslie
 * Cleaned up garbage caused by copyright merge in comment lines
 *
 * Revision 1.4  1993/06/30  22:52:30  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:44  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:20:18  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:50:34  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_obj.h,v
 * Revision 1.2.5.2  1993/04/15  22:46:51  alanl
 * 	Paging flow control (NORMA_VM).  Added
 * 	data_write_completed method.  [sjs]
 * 	[1993/04/15  22:12:27  alanl]
 *
 * Revision 1.2  1992/11/25  01:16:46  robert
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:51:26  robert]
 * 
 * Revision 1.1  1992/11/05  21:00:30  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.4.2.4  92/06/24  18:02:55  jeffreyh
 * 		XMM Framework Cleanup:
 * 	Delete m_kobj, k_mobj fields, and add comment explaining the
 * 	m_mobj and k_kobj fields that remain.  Make reply_to a union
 * 	in xmm_reply structure.  xmm_obj_reference is now a macro here.
 * 	[92/06/24            dlb]
 * 
 * 	Deleted unused kobj_held field.
 * 	[92/06/24            sjs]
 * 
 * 	Change USE_OLD_PAGEOUT booleans to values for data_write,
 * 	data_return, and data_initialize.
 * 	[92/06/04            dlb]
 * 
 * Revision 2.4.2.3  92/03/28  10:13:10  jeffreyh
 * 	Changed data_write to data_write_return and deleted data_return method.
 * 	[92/03/20            sjs]
 * 
 * Revision 2.4.2.2  92/02/21  11:27:30  jsb
 * 	Changed mobj field in xmm_reply to kobj, and added kobj_held field.
 * 	Changed xmm_reply_allocate_send_once macro accordingly.
 * 	[92/02/16  18:23:36  jsb]
 * 
 * 	Added explicit name paramater to xmm_decl, since not all cpps
 * 	recoginize arguments inside quotes. Sigh.
 * 	[92/02/16  14:14:38  jsb]
 * 
 * 	Defined more robust xmm_decl.
 * 	[92/02/09  12:55:40  jsb]
 * 
 * Revision 2.4.2.1  92/01/21  21:54:33  jsb
 * 	Moved IKOT_XMM_* definitions to kern/ipc_kobject.h.
 * 	[92/01/21  18:21:45  jsb]
 * 
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	Added XMM_USE_MACROS conditional, off by default,
 * 	which toggles between macros and functions for invocation
 * 	routines. (Right now only the functions are provided.)
 * 	[92/01/20  17:27:35  jsb]
 * 
 * Revision 2.4  91/07/01  08:26:25  jsb
 * 	Removed malloc, free definitions.
 * 	Added xmm_decl macro.
 * 	Renamed Xobj_allocate to xmm_obj_allocate.
 * 	Added zone element to xmm_class structure.
 * 	[91/06/29  14:41:25  jsb]
 * 
 * Revision 2.3  91/06/18  20:53:00  jsb
 * 	Removed bogus include.
 * 	[91/06/18  19:06:29  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:28  jsb
 * 	First checkin.
 * 	[91/06/17  11:03:46  jsb]
 * 
 */
/* CMU_ENDHIST */
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
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	norma/xmm_obj.h
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Common definitions for xmm system.
 */

#ifndef	_NORMA_XMM_OBJ_H_
#define	_NORMA_XMM_OBJ_H_

#ifdef	KERNEL
#include <mach/std_types.h>	/* For pointer_t */
#include <mach/mach_types.h>
#include <mach/kern_return.h>
#include <mach/boolean.h>
#include <mach/vm_prot.h>
#include <mach/message.h>
#include <kern/zalloc.h>
#include <kern/assert.h>
#include <mach_kdb.h>
#else	KERNEL
#include <mach.h>
#include <xmm_hash.h>
#endif	KERNEL

typedef kern_return_t		(*kern_routine_t)();
typedef void			(*void_routine_t)();

typedef struct xmm_class	*xmm_class_t;
typedef struct xmm_obj		*xmm_obj_t;
typedef struct xmm_reply	*xmm_reply_t;

#define	XMM_CLASS_NULL		((xmm_class_t) 0)
#define	XMM_OBJ_NULL		((xmm_obj_t) 0)
#define	XMM_REPLY_NULL		((xmm_reply_t) 0)

#define	MOBJ			((struct mobj *) mobj)
#define	KOBJ			((struct kobj *) kobj)

/*
 *	Strictly for debugging:  track the progress of pageout-
 *	related messages through the XMM stack.  On the outbound
 *	side, these messages are:  memory_object_data_write,
 *	memory_object_data_return and memory_object_data_initialize.
 *	On the inbound side, only memory_object_data_write_completed
 *	matters.
 */
#define	PENDING_TRACKING	MACH_ASSERT

struct xmm_class {
	kern_routine_t	m_init;
	kern_routine_t	m_terminate;
	void_routine_t	m_deallocate;
	kern_routine_t	m_copy;
	kern_routine_t	m_data_request;
	kern_routine_t	m_data_unlock;
	kern_routine_t	m_data_write_return;
	kern_routine_t	m_lock_completed;
	kern_routine_t	m_supply_completed;
	kern_routine_t	m_change_completed;
#if	MACH_KDB
	kern_routine_t	m_db_print;
#endif

	kern_routine_t	k_data_unavailable;
	kern_routine_t	k_get_attributes;
	kern_routine_t	k_lock_request;
	kern_routine_t	k_data_error;
	kern_routine_t	k_set_ready;
	kern_routine_t	k_destroy;
	kern_routine_t	k_data_supply;
	kern_routine_t	k_release;
	kern_routine_t	k_data_write_completed;
#if	MACH_KDB
	kern_routine_t	k_db_print;
#endif

	char *		c_name;
	int		c_size;
	zone_t		c_zone;
};

#if	__STDC__

#define xmm_decl(class, name, size)				\
extern kern_return_t m_ ## class ## _init();			\
extern kern_return_t m_ ## class ## _terminate();		\
extern void	     m_ ## class ## _deallocate();		\
extern kern_return_t m_ ## class ## _copy();			\
extern kern_return_t m_ ## class ## _data_request();		\
extern kern_return_t m_ ## class ## _data_unlock();		\
extern kern_return_t m_ ## class ## _data_write_return();	\
extern kern_return_t m_ ## class ## _lock_completed();		\
extern kern_return_t m_ ## class ## _supply_completed();	\
extern kern_return_t m_ ## class ## _change_completed();	\
extern kern_return_t k_ ## class ## _db_print();		\
extern kern_return_t k_ ## class ## _data_unavailable();	\
extern kern_return_t k_ ## class ## _get_attributes();		\
extern kern_return_t k_ ## class ## _lock_request();		\
extern kern_return_t k_ ## class ## _data_error();		\
extern kern_return_t k_ ## class ## _set_ready();		\
extern kern_return_t k_ ## class ## _destroy();			\
extern kern_return_t k_ ## class ## _data_supply();		\
extern kern_return_t k_ ## class ## _release();			\
extern kern_return_t k_ ## class ## _data_write_completed();	\
extern kern_return_t k_ ## class ## _db_print();		\
								\
struct xmm_class class ## _class = {				\
	m_ ## class ## _init,					\
	m_ ## class ## _terminate,				\
	m_ ## class ## _deallocate,				\
	m_ ## class ## _copy,					\
	m_ ## class ## _data_request,				\
	m_ ## class ## _data_unlock,				\
	m_ ## class ## _data_write_return,			\
	m_ ## class ## _lock_completed,				\
	m_ ## class ## _supply_completed,			\
	m_ ## class ## _change_completed,			\
	m_ ## class ## _db_print,				\
								\
	k_ ## class ## _data_unavailable,			\
	k_ ## class ## _get_attributes,				\
	k_ ## class ## _lock_request,				\
	k_ ## class ## _data_error,				\
	k_ ## class ## _set_ready,				\
	k_ ## class ## _destroy,				\
	k_ ## class ## _data_supply,				\
	k_ ## class ## _release,				\
	k_ ## class ## _data_write_completed,			\
	k_ ## class ## _db_print,				\
								\
	name,							\
	size,							\
	ZONE_NULL,						\
}

#else	__STDC__

#define xmm_decl(class, name, size)				\
extern kern_return_t m_/**/class/**/_init();			\
extern kern_return_t m_/**/class/**/_terminate();		\
extern void	     m_/**/class/**/_deallocate();		\
extern kern_return_t m_/**/class/**/_copy();			\
extern kern_return_t m_/**/class/**/_data_request();		\
extern kern_return_t m_/**/class/**/_data_unlock();		\
extern kern_return_t m_/**/class/**/_data_write_return();	\
extern kern_return_t m_/**/class/**/_lock_completed();		\
extern kern_return_t m_/**/class/**/_supply_completed();	\
extern kern_return_t m_/**/class/**/_change_completed();	\
extern kern_return_t m_/**/class/**/_db_print();		\
extern kern_return_t k_/**/class/**/_data_unavailable();	\
extern kern_return_t k_/**/class/**/_get_attributes();		\
extern kern_return_t k_/**/class/**/_lock_request();		\
extern kern_return_t k_/**/class/**/_data_error();		\
extern kern_return_t k_/**/class/**/_set_ready();		\
extern kern_return_t k_/**/class/**/_destroy();			\
extern kern_return_t k_/**/class/**/_data_supply();		\
extern kern_return_t k_/**/class/**/_release();			\
extern kern_return_t k_/**/class/**/_data_write_completed();	\
extern kern_return_t k_/**/class/**/_db_print();		\
								\
struct xmm_class class/**/_class = {				\
	m_/**/class/**/_init,					\
	m_/**/class/**/_terminate,				\
	m_/**/class/**/_deallocate,				\
	m_/**/class/**/_copy,					\
	m_/**/class/**/_data_request,				\
	m_/**/class/**/_data_unlock,				\
	m_/**/class/**/_data_write_return,			\
	m_/**/class/**/_lock_completed,				\
	m_/**/class/**/_supply_completed,			\
	m_/**/class/**/_change_completed,			\
	m_/**/class/**/_db_print,				\
								\
	k_/**/class/**/_data_unavailable,			\
	k_/**/class/**/_get_attributes,				\
	k_/**/class/**/_lock_request,				\
	k_/**/class/**/_data_error,				\
	k_/**/class/**/_set_ready,				\
	k_/**/class/**/_destroy,				\
	k_/**/class/**/_data_supply,				\
	k_/**/class/**/_release,				\
	k_/**/class/**/_data_write_completed,			\
	k_/**/class/**/_db_print,				\
								\
	name,							\
	size,							\
	ZONE_NULL,						\
}

#endif	__STDC__

/*
 *	The xmm_obj structure is the basis of the entire XMM system.
 *
 *	The m_mobj link is used for M_ invocations moving from the
 *	manager towards the kernel.  It points to the next xmm_obj
 *	in the direction of the kernel.
 *
 *	The k_kobj link is used for K_ invocations moving from the
 *	kernel towards the manager.  It points to the next xmm_obj
 *	in the direction of the manager.
 *
 *	These two links always form a doubly linked list.  This means:
 *	(1) if obj->m_mobj is not null, then obj == obj->m_mobj->k_kobj
 *	(2) if obj->k_kobj is not null, then obj == obj->k_kobj->m_mobj
 *
 */

struct xmm_obj {
	int		refcount;
	xmm_class_t	class;
	xmm_obj_t	m_mobj;
	xmm_obj_t	k_kobj;
};

struct xmm_reply {
	union {
		ipc_port_t	port;
		xmm_obj_t	mobj;
		xmm_reply_t	reply;
	} reply_to;
	mach_msg_type_name_t
	    		reply_to_type;
	xmm_obj_t	kobj;
	ipc_port_t	reply_proxy;
};

#define xmm_obj_reference(obj)	(obj)->refcount++

kern_return_t	m_interpose_init();
kern_return_t	m_interpose_terminate();
kern_return_t	m_interpose_copy();
kern_return_t	m_interpose_data_request();
kern_return_t	m_interpose_data_unlock();
kern_return_t	m_interpose_data_write_return();
kern_return_t	m_interpose_lock_completed();
kern_return_t	m_interpose_supply_completed();
kern_return_t	m_interpose_change_completed();
#if	MACH_KDB
kern_return_t	m_interpose_db_print();
#endif
kern_return_t	k_interpose_data_unavailable();
kern_return_t	k_interpose_get_attributes();
kern_return_t	k_interpose_lock_request();
kern_return_t	k_interpose_data_error();
kern_return_t	k_interpose_set_ready();
kern_return_t	k_interpose_destroy();
kern_return_t	k_interpose_data_supply();
kern_return_t	k_interpose_release();
kern_return_t	k_interpose_data_write_completed();
#if	MACH_KDB
kern_return_t	k_interpose_db_print();
#endif

kern_return_t	m_invalid_init();
kern_return_t	m_invalid_terminate();
kern_return_t	m_invalid_copy();
kern_return_t	m_invalid_data_request();
kern_return_t	m_invalid_data_unlock();
kern_return_t	m_invalid_data_write_return();
kern_return_t	m_invalid_lock_completed();
kern_return_t	m_invalid_supply_completed();
kern_return_t	m_invalid_change_completed();
#if	MACH_KDB
kern_return_t	m_invalid_db_print();
#endif
kern_return_t	k_invalid_data_unavailable();
kern_return_t	k_invalid_get_attributes();
kern_return_t	k_invalid_lock_request();
kern_return_t	k_invalid_data_error();
kern_return_t	k_invalid_set_ready();
kern_return_t	k_invalid_destroy();
kern_return_t	k_invalid_data_supply();
kern_return_t	k_invalid_release();
kern_return_t	k_invalid_data_write_completed();
#if	MACH_KDB
kern_return_t	k_invalid_db_print();
#endif

kern_return_t	xmm_obj_allocate();
kern_return_t	xmm_reply_allocate();
xmm_reply_t	convert_port_to_reply();

extern ipc_port_t remote_host_priv();

/*
 * Central repository of magic reply_to_types uses for non-port reply_tos.
 */
#define	XMM_SVM_REPLY		(MACH_MSG_TYPE_LAST + 1)
#define	XMM_SPLIT_REPLY		(MACH_MSG_TYPE_LAST + 2)
#define	XMM_BUFFER_REPLY	(MACH_MSG_TYPE_LAST + 3)

#define	xmm_reply_allocate_send_once(kobj, reply_to, replyp)\
xmm_reply_allocate((kobj), (reply_to), MACH_MSG_TYPE_PORT_SEND_ONCE, (replyp))

/*
 * More meaningful parameter constants for memory_object calls.
 */
#define	OBJECT_READY_FALSE	FALSE
#define	MAY_CACHE_FALSE		FALSE
#define	PRECIOUS_FALSE		FALSE

#define	OBJECT_READY_TRUE	TRUE
#define	MAY_CACHE_TRUE		TRUE
#define	PRECIOUS_TRUE		TRUE

#define XMM_USE_DATA_WRITE	0
#define XMM_USE_DATA_RETURN	1
#define XMM_USE_DATA_INITIALIZE	2


#ifndef	KERNEL
char	*malloc();
int	free();

/*
 * XXX should find or define vm_page_shift
 */
#define	atop(addr)	((addr) / vm_page_size)
#define	ptoa(page)	((page) * vm_page_size)
#endif	KERNEL

#define	XMM_USE_MACROS	0

#ifdef	lint
#undef	XMM_USE_MACROS
#define	XMM_USE_MACROS	0
#endif

#if	XMM_USE_MACROS

@@@_need_to_rewrite_macros_before_this_option_can_be_enabled_@@@;

#else	XMM_USE_MACROS

#endif	XMM_USE_MACROS

#endif	_NORMA_XMM_OBJ_H_
