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
 * This source file was created by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: sthread.h,v $
 * Revision 1.7  1994/11/18  20:49:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  18:44:22  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:06:10  cfj
 * Adding new code from vendor
 *
 * Revision 1.1.1.1  1993/05/03  17:53:46  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/05/06  19:33:05  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2.8.2  1993/03/29  22:47:22  nandy
 * Changes from loverso to fix the OIP intr problem.
 *  Revision 2.11  93/03/24  15:38:44  loverso
 *  Add OIP_LOCK_TRY(); remove unused flags; add macros to test if
 *  oip is in use or forwarded; replaced use of lookup and forall_remove
 *  with new lookup_func and forall_func. (loverso)
 *
 * Revision 2.11  93/03/24  15:38:44  loverso
 * 	Add OIP_LOCK_TRY(); remove unused flags; add macros to test if
 * 	oip is in use or forwarded; replaced use of lookup and forall_remove
 * 	with new lookup_func and forall_func. (loverso)
 * 
 * Revision 2.10  93/03/05  17:46:37  loverso
 * 	Rename silly macros to use OIP_ naming.  Remove unneeded FORWARD
 * 	bit.  The oip_intr_all now uses the alternate form of FORALL/UNLOCK
 * 	to avoid a deadlock with oip_deregister.  (loverso)
 * 
 * Revision 2.13  93/05/16  20:59:28  loverso
 * 	Pass intr & exiting indications via uthread area. (hack)
 * 
 * Revision 2.12  93/04/08  11:43:16  loverso
 * 	Fix notice.
 *
 * Revision 2.11  93/03/24  15:38:44  loverso
 * 	Add OIP_LOCK_TRY(); remove unused flags; add macros to test if
 * 	oip is in use or forwarded; replaced use of lookup and forall_remove
 * 	with new lookup_func and forall_func. (loverso)
 * 
 * Revision 2.10  93/03/05  17:46:37  loverso
 * 	Rename silly macros to use OIP_ naming.  Remove unneeded FORWARD
 * 	bit.  The oip_intr_all now uses the alternate form of FORALL/UNLOCK
 * 	to avoid a deadlock with oip_deregister.  (loverso)
 * 
 * Revision 2.9  92/05/01  10:04:23  rabii
 * 	Add "distingishing value" to REMOVE_PORT macro.(loverso)
 * 
 * Revision 2.8  92/03/20  11:34:33  pjg
 * 	Remove oip_credsport
 * 
 * Revision 2.7  92/03/15  14:32:52  roy
 * 	<<<log message for ./server/uxkern/sthread.h>>>
 * 
 * Revision 2.6  92/03/03  13:51:30  pjg
 * 	Changed hash functions to mpportid_* (loverso).
 * 
 * Revision 2.5  92/03/01  18:36:29  pjg
 * 	Facelift from "struct servermsg" to "struct server_oip"..
 * 
 * Revision 2.4  92/02/11  21:56:59  pjg
 * 	Revision 2.3.1.1  92/01/30  11:13:05  loverso
 * 	    Allow the servermsg to be a null placeholder.
 * 	    Delete SM_POST_INTERRUPT as this is now expanded in-line.
 * 
 * Revision 2.3  92/01/17  17:26:50  roy
 * 	Interruptible system call support (loverso).
 * 
 * Revision 2.2  92/01/05  20:14:32  roy
 * 	1991/10/14  20:56:33  noemi
 * 	Initial revision
 * 
 */

#ifdef	OSF1_ADFS
#ifndef _UXKERN_STHREAD_H_
#define _UXKERN_STHREAD_H_
#ifdef	_KERNEL
#include <sys/unix_defs.h>
#endif

/*
 * Server thread specific data structures.
 */

typedef unsigned long   transaction_id_t;

/*
 * Structure recording operations in process, primarily for fileservers.
 * Records each fsvr thread by identifying it with a credsport and
 * transaction id.  This structure is part of the uthread structure.
 * Primarily used for forwarding notification of interrupted system calls.
 */
struct server_oip {
	unsigned int		oip_flag;
	/* oip_transid should be a transaction_id_t, but compiler complains! */
	int			oip_transid;	/* transaction id for syscall */
	mach_port_t		oip_forwport;	/* port message forwarded to */
#ifdef	_KERNEL
	udecl_simple_lock_data(,oip_lock)
#endif
};

#ifdef	_KERNEL
#define	OIP_LOCK(oipp)		usimple_lock(&(oipp)->oip_lock)
#define	OIP_LOCK_TRY(oipp)	usimple_lock_try(&(oipp)->oip_lock)
#define	OIP_UNLOCK(oipp)	usimple_unlock(&(oipp)->oip_lock)
#define	OIP_LOCK_INIT(oipp)	usimple_lock_init(&(oipp)->oip_lock)
#define	OIP_LOCK_HOLDER(oipp)	SLOCK_HOLDER(&(oipp)->oip_lock)
#endif

/*
 * Values for server_oip flag field
 */

#define	OIP_IDLE	0		/* no oip or not interruptible */
#define	OIP_INTERRUPT	0x80000000	/* interrupt processed */

#define OIP_INUSE(oipp)		((oipp)->oip_transid != 0)
#define OIP_FORWARDED(oipp)	((oipp)->oip_forwport != MACH_PORT_NULL)

/*
 * Macros to manipulate the server_oip_hash_table.
 */

/* Number of server port hash table entries */
#define SERVER_OIP_HASHSZ	127

#define	OIP_ENTER(port, transid, ptr)					\
	mpportid_hash_enter(server_oip_hash_table, port, transid, (char *)ptr)
#define	OIP_REMOVE(port, transid, ptr)					\
	mpportid_hash_remove(server_oip_hash_table, port, transid, (char *)ptr)

#define	OIP_FORALL(port, func)						\
	mpportid_hash_foreach_func(server_oip_hash_table, port, func)
#define	OIP_INTR(port, transid, func)					\
	mpportid_hash_lookup_func(server_oip_hash_table, port, transid, func)

/*
 * This is used to pass args from across the OIP_FORALL() and OIP_INTR
 * macros down to do_oip_intr().  I didn't want to use more space in uthread_t.
 * (hack, hack)
 */
#define uu_oip_intr	uu_spare[7]
#define uu_oip_exiting	uu_spare[6]

/*
 * Macros to manipulate op-in-progress structures
 *
 * These are normally used as follows:
 *
 *	if (error == EREMOTE) {
 *		struct server_oip *oipp = &u.uu_oip;
 *		struct nameidata *ndp = &u.u_nd;
 *
 *	 	OIP_SET_FORW(oipp, ndp->ni_forwport);
 *		error = fsvr_xxxx(ndp->ni_forwport, creds_port, transid,
 *				root_port,
 *				ndp->ni_ptr, strlen(ndp->ni_ptr)+1, ...);
 *		OIP_END_FORW(oipp);
 *	}
 *
 * Note that in remote_xxx() routines, the above might use
 *	oipp->oip_transid
 * for "transid".
 *
 * XXX:	There is a race condition if an interrupt is delivered just before
 *	we call fsvr_xxxx().  It is possible for the forwarded intr_delivery
 *	to get there first, and the interrupt can get lost.  Retry logic in
 *	sbsd_issig_psig() will catch this, but this will need to be revisited
 *	for efficiency.
 */

#define	OIP_SET_FORW(oipp, port)			\
MACRO_BEGIN						\
	if (OIP_INUSE((oipp))) {			\
		OIP_LOCK((oipp));			\
		(oipp)->oip_forwport = (port);		\
		OIP_UNLOCK((oipp));			\
	}						\
MACRO_END

#define	OIP_END_FORW(oipp)				\
MACRO_BEGIN						\
	if (OIP_INUSE((oipp))) {			\
		OIP_LOCK((oipp));			\
		(oipp)->oip_forwport = MACH_PORT_NULL;	\
		OIP_UNLOCK((oipp));			\
	}						\
MACRO_END

#endif 	/* _UXKERN_STHREAD_H_ */
#endif	/* OSF1_ADFS */
