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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: vs_soops.c,v $
 * Revision 1.10  1995/02/01  23:30:42  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.9  1994/11/18  20:52:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1993/09/25  00:41:00  cfj
 * Merge R1.1 bug fix.
 *
 * Revision 1.7.6.1  1993/09/25  00:39:08  cfj
 * Change INCREMENT/DECREMNT_VS_REFCNT to INCREMENT/DECREMENT_VSNET_REFCNT.
 *
 * Revision 1.7  1993/07/14  18:49:35  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.5  1993/07/01  21:14:40  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:34:22  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 3.11  1992/12/11  15:02:17  mjl
 * No more VSOP_VSOCK{CREATE,DESTROY}() virtual socket operations.  Fixed
 * misspelled (sp?) extern declarations for soreserve(), sogetaddr().
 *
 * Revision 3.10  92/10/27  17:56:09  bhk
 * Removed usrreq and sosleep operations. We now use the generic so... versions
 * of these operations
 * 
 * Revision 3.9  92/08/17  13:45:34  mjl
 * Got rid of extraneous arg to VSOP_SLEEP().
 * 
 * Revision 3.8  92/08/08  01:57:05  jdh
 *  
 * added a default sosleep() to go with a VSOP_SLEEP that has an extra
 * parameter
 * XXX don't need extra field anymore -- remove this -- jdh
 * 
 * Revision 3.7  92/06/16  18:47:54  bhk
 * replaced pure wrapper routines with actual so???? call
 * 
 * Revision 3.6  92/05/06  16:15:20  bhk
 * added default sleep virtual socket operation
 * 
 * Revision 3.5  92/04/22  17:32:07  bhk
 * Added support for getsockname call
 * 
 * Revision 3.4  92/04/14  14:31:48  roman
 * Increment vsocket reference count to avoid the "Bad Ref Count so" message.
 * 
 * Revision 3.3  92/04/06  11:05:39  mjl
 * Add VSOP_RELOCATE() virtual socket op.  Got rid of function pointer argument
 * to DECREMENT_VS_REFCNT(); not needed since VSOP_CLOSE() only called on last
 * close anyway.
 * 
 * Revision 3.2  92/03/20  17:08:41  bhk
 * moved vsocket.h to vsocket directory
 * 
 * Revision 3.1  92/03/18  18:06:18  mjl
 * VSOP_CREATE() now takes single socket pointer arg.
 * Added no-op for VSOP_NOTIFY().
 * 
 * Revision 3.0  92/03/04  14:56:52  bhk
 * Genesis	bhk
 * 
 *
 * Genisis 12/5/91 bhk
 *
 */

#include "sys/param.h"
#include "sys/types.h"
#include "sys/mbuf.h"
#include "sys/uio.h"
#include "sys/socket.h"
#include "vsocket/vsocket.h"
#include "sys/socketvar.h"
#include "sys/protosw.h"
#include "sys/errno.h"


extern int vs_socreate();
extern int vs_soclose();
extern int sosend();
extern int soreceive();
extern int vs_sodequeue();
extern int solisten();
extern int soconnect();
extern int soconnect2();
extern int sobind();
extern int soshutdown();
extern int sosetopt();
extern int sogetopt();
extern int vs_noop();
extern int vs_badop();
extern int sogetaddr();
extern int sosleep();
extern int soreserve();
extern int sogetaddr();


struct vsocket_ops default_vsops = {
	vs_socreate,
	vs_soclose,
	sosend,
	soreceive,
	vs_sodequeue,
	solisten,
	soconnect,
	soconnect2,
	sobind,
	soshutdown,
	sosetopt,
	sogetopt,
	vs_noop,		/* VSOP_NOTIFY */
	vs_noop,		/* VSOP_RELOCATE */
	sogetaddr,
	sosleep,
	vs_badop,		/* VSOP_VSOCKCONNECT */
	soreserve
};


int 
vs_socreate(vs)
struct socket *vs;	/* run socket creation hook using this socket */
{

	SOCKET_LOCK(vs);
	INCREMENT_VSNET_REFCNT(vs,"vs_socreate:");
	SOCKET_UNLOCK(vs);
	return 0;
}

int
vs_soclose(vs)
struct socket *vs;		/* vsocket to close */
{
	int error;
	
	DECREMENT_VSNET_REFCNT(vs,"vs_soclose:");
}
		



int
vs_sodequeue(head, so, nam, compat_43)
struct socket *head; 
struct socket **so;
struct mbuf **nam;
int compat_43;
{
	int error;
	
	if(!(error = sodequeue(head,so,nam,compat_43))) {
		SOCKET_LOCK(*so);
		INCREMENT_VSNET_REFCNT(*so,"vs_dequeue:");
		(*so)->vs_ops = head->vs_ops;
		SOCKET_UNLOCK(*so);
	}
	return(error);
}





/*
 *  The remaining vsocket operations have no regular socket
 *  equivalents, and are no-ops for non-TNC systems.
 */

int
vs_noop()
{
	return(NULL);
}

int
vs_badop()
{
	return EINVAL;
}

