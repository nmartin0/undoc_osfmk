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
 * $Log: xmm_debug.c,v $
 * Revision 1.8  1994/11/18  20:56:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/07/12  19:25:06  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.6.8.1  1994/03/11  23:02:09  andyp
 * Corrected the use of varargs to be more portable.
 *
 * Revision 1.6  1993/07/22  02:21:18  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.5  1993/06/30  22:52:12  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:40:47  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:46:30  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:51:57  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_debug.c,v
 * Revision 1.1.5.2  1993/05/23  18:07:39  alanl
 * 	Conditionalize debugging code on MACH_KDB.
 * 	[1993/05/23  17:47:55  alanl]
 *
 * Revision 1.1  1992/09/30  02:13:53  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.3.3.5  92/06/24  18:02:25  jeffreyh
 * 	Minor changes for XMM Framework cleanup.  %M and %R are probably
 * 	broken, ditto for the sample calls.
 * 	[92/06/24            dlb]
 * 
 * 	Add debug print support for use_routine values.
 * 	[92/06/04            dlb]
 * 
 * Revision 2.3.3.4  92/03/28  10:12:39  jeffreyh
 * 	Changed data_write to data_write_return and deleted data_return.
 * 	[92/03/20            sjs]
 * 
 * Revision 2.3.3.3  92/02/21  11:25:46  jsb
 * 	No longer an xmm layer. Will eventually be used by xmm_invoke.c.
 * 	[92/02/20  15:45:23  jsb]
 * 
 * Revision 2.3.3.1  92/01/21  21:53:54  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	Use db_printf instead of printf.
 * 	[92/01/20  17:20:11  jsb]
 * 
 * 	Fixes from OSF.
 * 	[92/01/17  14:14:30  jsb]
 * 
 * Revision 2.3.1.1  92/01/15  12:14:56  jeffreyh
 * 	Return a value from the debug layer terminate routine.
 * 	It was losing it before. (sjs)
 * 
 * Revision 2.3  91/07/01  08:26:00  jsb
 * 	Collect garbage. Return valid return values.
 * 	[91/06/29  15:25:16  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:13  jsb
 * 	First checkin.
 * 	[91/06/17  11:05:55  jsb]
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
 *	File:	norma/xmm_debug.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Xmm layer providing debugging output.
 */

#ifdef	KERNEL
#include <mach_kdb.h>
#include <norma/xmm_obj.h>
#include <sys/varargs.h>
#else	KERNEL
#include <xmm_obj.h>
#include <varargs.h>
#endif	KERNEL

int xmm_debug = 0;

#if	MACH_KDB
/* VARARGS */
m_printf(va_alist)
	va_dcl
{
	va_list adx;
	char	*fmt;
	char c, buf[1024], fmt0[3], *bp;
	int i;
	xmm_obj_t mobj;
	xmm_obj_t kobj;
	xmm_reply_t reply;

	if (! xmm_debug) {
		return;
	}
	va_start(adx);
	fmt = va_arg(adx, char *);
	fmt0[0] = '%';
	fmt0[2] = '\0';
	for (;;) {
		bp = buf;
		while ((c = *fmt++) && c != '%') {
			*bp++ = c;
		}
		*bp++ = '\0';
		db_printf("%s", buf);
		if (c == '\0') {
			break;
		}
		switch (c = *fmt++) {
		case 'M':
			mobj = va_arg(adx, xmm_obj_t);
			kobj = va_arg(adx, xmm_obj_t);
			db_printf("k_%s.%x -> m_%s.%x",
				  kobj->k_kobj->class->c_name, kobj,
				  mobj->m_mobj->class->c_name, mobj);
			break;
			
		case 'K':
			kobj = va_arg(adx, xmm_obj_t);
			mobj = kobj;	/* XXX no */
			db_printf("m_%s.%x -> k_%s.%x",
				  mobj->m_mobj->class->c_name, mobj,
				  kobj->k_kobj->class->c_name, kobj);
			break;

		case 'Z':
			i = va_arg(adx, int);
			db_printf("%d", i / PAGE_SIZE);
			if (i % PAGE_SIZE) {
				db_printf(".%d", i % PAGE_SIZE);
			}
			break;

		case 'P':
			i = va_arg(adx, int);
			db_printf("%c%c%c",
				  ((i & VM_PROT_READ) ? 'r' : '-'),
				  ((i & VM_PROT_WRITE) ? 'w' : '-'),
				  ((i & VM_PROT_EXECUTE)? 'x' : '-'));
			break;

		case 'N':
			if (! va_arg(adx, int)) {
				db_printf("!");
			}
			break;

		case 'C':
			i = va_arg(adx, int);
			if (i == MEMORY_OBJECT_COPY_NONE) {
				bp = "copy_none";
			} else if (i == MEMORY_OBJECT_COPY_CALL) {
				bp = "copy_call";
			} else if (i == MEMORY_OBJECT_COPY_DELAY) {
				bp = "copy_delay";
			} else {
				bp = "copy_???";
			}
			db_printf("%s", bp);
			break;

		case 'U':
			i = va_arg(adx, int);
			if (i == XMM_USE_DATA_WRITE) {
				bp = "data_write";
			} else if (i == XMM_USE_DATA_RETURN) {
				bp = "data_return";
			} else if (i == XMM_USE_DATA_INITIALIZE) {
				bp = "data_initialize";
			} else {
				bp = "data_???";
			}
			db_printf("%s", bp);
			break;

		case 'R':
			reply = va_arg(adx, xmm_reply_t);
			/* XXX mobj = reply->mobj; ??? */
			db_printf("k_%s.%x -> r_%s.%x",
				  mobj->k_kobj->class->c_name, mobj,
				  mobj->m_mobj->class->c_name, reply);
			break;

		default:
			fmt0[1] = c;
			db_printf(fmt0, va_arg(adx, long));
			break;
		}
	}
	va_end(adx);
}
#endif	/* MACH_KDB */


#if 0
sample_calls()
{
	m_printf("m_init            (%M, 0x%x, %d)\n",
		 mobj, memory_object_name, pagesize);
	m_printf("m_terminate       (%M, 0x%x)\n",
		 mobj, kobj, memory_object_name);
	m_printf("m_copy            (%M, %Z, %Z, 0x%x)\n",
		 mobj, kobj, offset, length, new_mobj);
	m_printf("m_data_request    (%M, %Z, %Z, %P)\n",
		 mobj, kobj, offset, length, desired_access);
	m_printf("m_data_unlock     (%M, %Z, %Z, %P)\n",
		 mobj, kobj, offset, length, desired_access);
	m_printf("m_data_write_return [%U]      (%M, %Z, 0x%x, %Z)\n",
		 use_routine, mobj, kobj, offset, data, length);
	m_printf("m_lock_completed  (%R, %Z, %Z)\n",
		 reply, offset, length);
	m_printf("m_supply_completed(%R, %Z, %Z, 0x%x, %Z)\n",
		 reply, offset, length, result, error_offset);
	m_printf("m_change_completed(%R, %Nmay_cache, %C)\n",
		 reply, may_cache, copy_strategy);
	m_printf("k_data_unavailable(%K, %Z, %Z)\n",
		 kobj, offset, length);
	m_printf("k_get_attributes  (%K)\n",
		 kobj);
	m_printf("k_lock_request    (%K, %Z, %Z, %Nclean, %Nflush, %P, 0x%x)\n",
		 kobj, offset, length, should_clean, should_flush, lock_value,
		 reply);
	m_printf("k_data_error      (%K, %Z, %Z, 0x%x)\n",
		 kobj, offset, length, error_value);
	m_printf("k_set_ready       (%K, %Nready, %Nmay_cache, %C, %U, 0x%x)\n",
		 kobj, object_ready, may_cache, copy_strategy,
		 use_routine, reply);
	m_printf("k_destroy         (%K, 0x%x)\n",
		 kobj, reason);
	m_printf("k_data_supply     (%K, %Z, 0x%x, %Z, %P, %Npr, rply=0x%x)\n",
		 kobj, offset, data, length, lock_value, precious, reply);
}
#endif
