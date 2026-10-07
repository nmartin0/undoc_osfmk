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
 * $Id: dipc_notify.c,v 1.3 1994/11/18 20:57:58 mtm Exp $
 * 
 * HISTORY:
 *
 */

#include <ipc/ipc_port.h>
#include <norma2/dipc_notify.h>
#include <norma2/norma_log.h>

/*
 *  Name:	dipc_no_more_senders()
 *
 *  Implicit Inputs:
 *	Our caller has cleared (IP_NULL) 'ip_nsrequest' field.
 *
 *  Inputs:	Pointer to the proxy
 *
 *  Side Effects: 
 *		proxy may be destroyed.
 *
 *  Returns:		void
 *
 *  MP and locking
 *  consideration:	Describe locking and MP considerations.
 *
 *  Description:
 *	The send-right count has dropped to zero and someone noticed a
 *	no-more senders notification request. Here we mop up the send-rights
 *	accounting and attempt to remove the proxy.
 */

void
dipc_no_more_senders(proxy)
	ipc_port_t	proxy;
{
	notify_entry1(dipc_no_more_senders, proxy);

	assert(DIPC_IS_PROXY(proxy));
	assert(proxy->ip_nsrequest == IP_NULL);/* caller cleared ip_nsrequest */
	assert(proxy->ip_srights == 0);
	assert(proxy->ip_sorights > 0);

	dipc_port_log(__FUNC__,"recv'ed NMS",proxy);

	/*
	 * consume the send-once right and clobber the ns request.
	 */
	ip_lock(proxy);
	proxy->ip_sorights--;
	ip_release(proxy);
	/* our caller has already cleared ip_nsrequest */
	proxy->ip_nsrequest = IP_NULL;
	ip_unlock(proxy);

	/*
	 *  The proxy might be free to destroy.
	 */
	dipc_port_remove_try(proxy);
}
