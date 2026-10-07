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
 * $Log: xmm_split.c,v $
 * Revision 1.10  1994/11/18  20:57:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/08/31  21:25:14  mtm
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
 * Revision 1.7.2.1  1994/08/08  23:54:23  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.8  1994/08/08  19:34:13  andyp
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
 * Revision 1.7  1994/07/12  19:25:34  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.6.4.2  1994/03/11  23:02:14  andyp
 * Corrected the use of varargs to be more portable.
 *
 * Revision 1.6.4.1  1994/02/25  22:42:08  andyp
 * Removed some lint with respect to some debugging routines.
 *
 * Revision 1.6  1993/09/28  18:06:43  andyp
 * Update for the 1.2 release.
 *
 *
 *	- Count up pending split requests when printing stack.
 *	- Added {k,m}_db_print methods.
 *	[alanl@osf.org]
 *
 * Revision 1.5  1993/07/22  02:21:54  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:52:45  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:58  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:20:29  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:48:08  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_split.c,v
 * Revision 1.2.5.2  1993/04/15  22:47:44  alanl
 * 	Paging flow control (NORMA_VM).  Added support for
 * 	data_write_completed method (assume single page reply
 * 	for now and bypass this layer).  Handle lock_request
 * 	for a large range by using scatter/gather.  [sjs]
 * 	[1993/04/15  22:13:08  alanl]
 *
 * Revision 1.2  1992/11/25  01:17:21  robert
 * 	fix histry
 * 	[1992/11/09  22:30:06  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:52:38  robert]
 * 
 * Revision 0.0  92/10/15            sjs
 * 	Added support for new K_RELEASE method
 * 
 * 	Revision 1.1  1992/11/05  21:00:41  robert
 * 	Initial revision
 * 	[92/10/15            sjs]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.3.2.4  92/06/24  18:03:17  jeffreyh
 * 	Changed routines to reflect new M_ interface.
 * 	[92/06/18            sjs]
 * 
 * 	Add release logic to lock_completed.
 * 	[92/06/09            dlb]
 * 
 * Revision 2.3.2.3  92/03/28  10:13:24  jeffreyh
 * 	Changed data_write to data_write_return and deleted data_return
 * 	 method.
 * 	[92/03/20            sjs]
 * 
 * Revision 2.3.2.2  92/02/21  11:28:13  jsb
 * 	Changed reply->mobj to reply->kobj. Discard reply in lock_completed.
 * 	[92/02/16  18:24:34  jsb]
 * 
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	Changed debugging printf name and declaration.
 * 	[92/02/16  14:25:05  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	[92/02/09  14:17:57  jsb]
 * 
 * Revision 2.3.2.1  92/01/21  21:54:54  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:30:57  jsb]
 * 
 * Revision 2.3  91/07/01  08:26:34  jsb
 * 	Use zone for requests.
 * 	[91/06/29  15:34:20  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:40  jsb
 * 	First checkin.
 * 	[91/06/17  11:04:50  jsb]
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
 *	File:	norma/xmm_split.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Xmm layer to split multi-page lock_requests.
 */

#ifdef	KERNEL
#include <norma/xmm_obj.h>
#include <mach/vm_param.h>
#else	KERNEL
#include <xmm_obj.h>
#endif	KERNEL

#include <norma/xmm_dipc.h>

#define	dprintf	xmm_split_dprintf

typedef struct request *request_t;

#define	REQUEST_NULL	((request_t) 0)

struct mobj {
	struct xmm_obj	obj;
	request_t	r_head;
	request_t	r_tail;
};

struct request {
	vm_offset_t	r_offset;
	vm_size_t	r_length;
	vm_offset_t	r_start;
	vm_size_t	r_count;
	vm_size_t	r_resid;
	request_t	r_next;
	xmm_reply_t	reply;
};

#undef  KOBJ
#define KOBJ    ((struct mobj *) kobj)

#define	m_split_init			m_interpose_init
#define	m_split_terminate		m_interpose_terminate
#define	m_split_deallocate		m_interpose_deallocate
#define	m_split_copy			m_interpose_copy
#define	m_split_data_request		m_interpose_data_request
#define	m_split_data_unlock		m_interpose_data_unlock
#define	m_split_data_write_return	m_interpose_data_write_return
#define	m_split_supply_completed	m_interpose_supply_completed
#define	m_split_change_completed	m_interpose_change_completed

#define	k_split_data_unavailable	k_interpose_data_unavailable
#define	k_split_get_attributes		k_interpose_get_attributes
#define	k_split_data_error		k_interpose_data_error
#define	k_split_set_ready		k_interpose_set_ready
#define	k_split_destroy			k_interpose_destroy
#define	k_split_data_supply		k_interpose_data_supply
#define	k_split_data_write_completed	k_interpose_data_write_completed
#define	k_split_release			k_invalid_release

extern m_split_db_print();
extern k_split_db_print();

xmm_decl(split, "split", sizeof(struct mobj));

zone_t		xmm_split_request_zone;

xmm_reply_t
xmm_reply_allocate_reply(mobj, real_reply)
	xmm_obj_t mobj;
	xmm_reply_t real_reply;
{
	kern_return_t kr;
	xmm_reply_t reply;

	xmm_entry2(xmm_reply_allocate_reply, mobj, real_reply);

	if (real_reply == XMM_REPLY_NULL) {
		return XMM_REPLY_NULL;
	}
	kr = xmm_reply_allocate(mobj, (ipc_port_t) real_reply, XMM_SPLIT_REPLY,
				&reply);
	if (kr != KERN_SUCCESS) {
		panic("xmm_reply_allocate_mobj: xmm_reply_allocate: %d\n", kr);
	}
	return reply;
}

kern_return_t
xmm_split_create(old_mobj, new_mobj)
	xmm_obj_t old_mobj;
	xmm_obj_t *new_mobj;
{
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry2(xmm_split_create, old_mobj, new_mobj);

	kr = xmm_obj_allocate(&split_class, old_mobj, &mobj);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	MOBJ->r_head = REQUEST_NULL;
	MOBJ->r_tail = REQUEST_NULL;
	*new_mobj = mobj;
	return KERN_SUCCESS;
}

k_split_lock_request(kobj, offset, length, should_clean, should_flush,
		     lock_value, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	xmm_reply_t reply;
{
	request_t r;
	xmm_reply_t nreply;
	kern_return_t kr;

	xmm_entry7(k_split_lock_request,
		kobj,
		offset,
		length,
		should_clean,
		should_flush,
		lock_value,
		reply);
#ifdef	lint
	K_LOCK_REQUEST(kobj, offset, length, should_clean, should_flush,
		       lock_value, reply);
#endif	lint
	/*
	 * If we don't have to split the request,
	 * then just pass it through.
	 */
	if (length == 0) {
		return M_LOCK_COMPLETED(kobj, offset, length, reply, FALSE);
	}
	if (length <= PAGE_SIZE) {
		kr = K_LOCK_REQUEST(kobj, offset, length, should_clean,
				      should_flush, lock_value, reply);
#define	UGLY_HACK
#ifdef	UGLY_HACK
		/*
		 * HACK ALERT: this is just to get this sucker working
		 */
		k_svm_push_gather(kobj->k_kobj);
		return kr;
#endif	/* UGLY_HACK */
	}
	nreply = xmm_reply_allocate_reply(kobj, reply);
	if (reply != XMM_REPLY_NULL) {
		r = (request_t) zalloc(xmm_split_request_zone);
		r->r_offset = offset;
		r->r_length = length;
		r->r_start = (offset + PAGE_SIZE - 1) / PAGE_SIZE;
		r->r_count = (length + PAGE_SIZE - 1) / PAGE_SIZE;
		r->r_resid = r->r_count;
		r->r_next = REQUEST_NULL;
		if (KOBJ->r_head) {
			KOBJ->r_tail->r_next = r;
		} else {
			KOBJ->r_head = r;
		}
		KOBJ->r_tail = r;
		dprintf("@r start=%d count=%d\n", r->r_start, r->r_count);
	}
	while (length > PAGE_SIZE) {
		K_LOCK_REQUEST(kobj, offset, PAGE_SIZE, should_clean,
			       should_flush, lock_value,
			       nreply);
		offset += PAGE_SIZE;
		length -= PAGE_SIZE;
	}
	if (length > 0) {
		K_LOCK_REQUEST(kobj, offset, length, should_clean,
			       should_flush, lock_value,
			       nreply);
	}
#ifdef	UGLY_HACK
/*
 * HACK ALERT: this is just to get this sucker working
 */
	k_svm_push_gather(kobj->k_kobj);
#endif	UGLY_HACK
	/* XXX check return values above??? */
	return KERN_SUCCESS;
}

m_split_lock_completed(mobj, offset, length, reply, release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;	/* XXX ignored */
	xmm_reply_t reply;
	boolean_t release;
{
	vm_offset_t start;
	request_t r, *rp, r_prev = REQUEST_NULL;
	xmm_obj_t kobj;
	xmm_reply_t real_reply;

	xmm_entry5(m_split_lock_completed,
		mobj, offset, length, reply, release);

#ifdef	lint
	M_LOCK_COMPLETED(mobj, offset, length, reply, release);
#endif	lint
	/*
	 * If reply is not a private xmm_split reply, then
	 * just pass it through.
	 */
	if (reply->reply_to_type != XMM_SPLIT_REPLY) {
		return M_LOCK_COMPLETED(mobj, offset, length, reply, release);
	}

	/*
	 * Retrieve kobj and real reply from private reply,
	 * and discard private reply.
	 */
	kobj = reply->kobj;
	assert(kobj->class == &split_class);
	real_reply = reply->reply_to.reply;

	/*
	 * Look for request structure.
	 * XXX reply->reply_to really should be r, not reply.
	 * XXX that would eliminate this stupid search.
	 * XXX same thing goes for xmm_svm module.
	 */
	start = (offset + PAGE_SIZE - 1) / PAGE_SIZE;
	dprintf("#r start=%d\n", start);
	for (rp = &KOBJ->r_head; r = *rp; rp = &r->r_next) {
		dprintf(":r start=%d..%d resid=%d\n",
		       r->r_start, r->r_start + r->r_count - 1, r->r_resid);
		if (start >= r->r_start && start < r->r_start + r->r_count) {
			break;
		}
		r_prev = r;
	}
	if (r == REQUEST_NULL) {
		panic("k_split_lock_completed: lost request\n");
	}
	dprintf("#r found\n");

	/*
	 * If this is the reply that we are waiting for, then
	 * send the real unsplit reply. Otherwise, just return.
	 */
	if (--r->r_resid > 0) {
		assert(release == FALSE);
		return KERN_SUCCESS;
	}
	xmm_reply_deallocate(reply);
	if (r == KOBJ->r_tail) {
		KOBJ->r_tail = r_prev;
	}
	*rp = r->r_next;
	M_LOCK_COMPLETED(mobj, r->r_offset, r->r_length, real_reply, release);
	zfree(xmm_split_request_zone, (vm_offset_t) r);
	return KERN_SUCCESS;
}

xmm_split_init()
{
	xmm_entry0(xmm_split_init);

	xmm_split_request_zone = zinit(sizeof(struct request), 512*1024,
				       sizeof(struct request), FALSE,
				       "xmm.split.request");
}

#include <sys/varargs.h>

int xmm_split_debug = 0;

/* VARARGS */
xmm_split_dprintf(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

	if (xmm_split_debug) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		printf(fmt, &listp);
		va_end(listp);
	}
}


#if	MACH_KDB
unsigned int
xmm_split_request_count(req)
request_t	req;
{
	unsigned int	count;

	for (count = 0; req != REQUEST_NULL; ++count, req = req->r_next)
		;
	return count;
}

#define	M_SPLIT_DB_REQ_MAX	5

m_split_db_print(mobj)
xmm_obj_t	mobj;
{
	request_t	req;
	unsigned int	count, index;
	extern int	indent;

	count = xmm_split_request_count(MOBJ->r_head);
	iprintf("r_head=0x%x r_tail=0x%x request count is 0x%x\n",
		MOBJ->r_head, MOBJ->r_tail, count);
	indent += 2;
	if (count > M_SPLIT_DB_REQ_MAX)
		count = M_SPLIT_DB_REQ_MAX;
	for (index=1, req=MOBJ->r_head; count-- > 0; ++index,req=req->r_next) {
		iprintf("[%2d] r_offset=0x%x len=0x%x start=0x%x cnt=0x%x\n",
			index, req->r_offset, req->r_length, req->r_start,
			req->r_count);
		iprintf("     r_resid=0x%x reply=0x%x r_next=0x%x\n",
			req->r_resid, req->reply, req->r_next);
	}
	indent -= 2;

	return 0;
}

k_split_db_print(kobj)
xmm_obj_t	kobj;
{
	return m_split_db_print(kobj);
}
#else	/* MACH_KDB */
m_split_db_print(mobj) xmm_obj_t mobj; { }
k_split_db_print(kobj) xmm_obj_t kobj; { }
#endif	/* MACH_KDB */
