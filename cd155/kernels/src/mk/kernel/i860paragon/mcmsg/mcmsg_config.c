/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
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
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_config.c,v 1.9 1994/11/18 20:43:52 mtm Exp $
 */

/*
 * mcmsg_config.c
 *
 * Message module configuration
 */


/*
 * mcmsg_config.h has macros for every module.
 * Here we use it to make external declarations for the init procedures.
 */

#define INIT_DEFINE(m,p) extern int p();
#include <i860paragon/mcmsg/mcmsg_config.h>

extern int mcmsg_init_null();

/*
 * mcmsg_config.h has macros for every module.
 * Here we use it to fill in the mcmsg_init_switch array with pointers to
 * the init procedures.
 */


#define INIT_DEFINE(m,p)	p,
#define	INIT_NULL(m)		mcmsg_init_null,

int (*mcmsg_init_switch[])() = {
#include <i860paragon/mcmsg/mcmsg_config.h>
};


/*
 * mcmsg_config.h has macros for every module.
 * Here we use it to make external declarations for the inquiry procedures.
 */

#define INQ_DEFINE(m,p) extern int p();
#include <i860paragon/mcmsg/mcmsg_config.h>

extern int mcmsg_inq_null();

/*
 * mcmsg_config.h has macros for every module.
 * Here we use it to fill in the mcmsg_inq_switch array with pointers to
 * the inq procedures.
 */


#define INQ_DEFINE(m,p)		p,
#define	INQ_NULL(m)		mcmsg_inq_null,

int (*mcmsg_inq_switch[])() = {
#include <i860paragon/mcmsg/mcmsg_config.h>
};


/*
 * mcmsg_config.h has macros for every interface call.
 * Here we use it to make external declarations for the interface procedures.
 */

#define POST_DEFINE(m,n,p,a,s,u) extern int p();
#include <i860paragon/mcmsg/mcmsg_config.h>

extern int mcmsg_post_sw_err();

/*
 * mcmsg_config.h has macros for every interface call.
 * Here we use it to fill in the mcmsg_post_switch array with pointers to
 * the interface procedures.
 */


#define POST_DEFINE(m,n,p,a,s,u)	p,
#define	POST_NULL(m)			mcmsg_post_sw_err,

int (*mcmsg_post_switch[])() = {
#include <i860paragon/mcmsg/mcmsg_config.h>
};


/*
 * mcmsg_config.h has macros for every interface call.
 * Here we use it to fill in the mcmsg_user_switch array with pointers to
 * the user accessible interface procedures.
 */


#define POST_DEFINE(m,n,p,a,s,u)	(u == USER)? p : mcmsg_post_sw_err,
#define	POST_NULL(m)			mcmsg_post_sw_err,

int (*mcmsg_user_switch[])() = {
#include <i860paragon/mcmsg/mcmsg_config.h>
};


/*
 * mcmsg_config.h has macros for every packet type.
 * Here we use it to make external declarations for the packet routines.
 */

#define PACKET_DEFINE(n, name, sname, send, recv) \
extern int send(); \
extern int recv();

#define PACKET_NULL(x)
#define PACKET_FILL(x)
#define PACKET_END(x)

#include <i860paragon/mcmsg/mcmsg_config.h>

extern int mcmsg_send_sw_null();
extern int mcmsg_send_sw_err();
extern int mcmsg_recv_sw_err();

/*
 * mcmsg_config.h has macros for every packet type.
 * Here we use it to fill in the mcmsg_send_switch array with pointers to
 * the packet send routines.
 */

#define PACKET_DEFINE(n, name, sname, send, recv) send,
#define PACKET_NULL(x)	mcmsg_send_sw_null,
#define PACKET_FILL(x)	mcmsg_send_sw_err,
#define PACKET_END(x)


int (*mcmsg_send_switch[])() = {

#include <i860paragon/mcmsg/mcmsg_config.h>

};


/*
 * mcmsg_config.h has macros for every packet type.
 * Here we use it to fill in the mcmsg_recv_switch array with pointers to
 * the packet recv routines.
 */

#define PACKET_DEFINE(n, name, sname, send, recv) recv,
#define PACKET_NULL(x)	mcmsg_recv_sw_err,
#define PACKET_FILL(x)	mcmsg_recv_sw_err,
#define PACKET_END(x)


int (*mcmsg_recv_switch[])() = {

#include <i860paragon/mcmsg/mcmsg_config.h>

};

#define	MSG_TRACE	1
#if	MSG_TRACE

/*
 * mcmsg_config.h has macros for every call.
 * Here we use it to define call names, argument counts, and module id.
 */

#define POST_DEFINE(m,n,p,a,i,u)	n,
#define POST_NULL(m)			"**ng**",
char *post_name_list[] = {
#include <i860paragon/mcmsg/mcmsg_config.h>
};

#define POST_DEFINE(m,n,p,a,i,u)	a,
#define POST_NULL(m)			0,
int post_args_list[] = {
#include <i860paragon/mcmsg/mcmsg_config.h>
};

#define POST_DEFINE(m,n,p,a,i,u)	i,
#define POST_NULL(m)			0,
int post_module_list[] = {
#include <i860paragon/mcmsg/mcmsg_config.h>
};

/*
 * mcmsg_config.h has macros for every packet type.
 * Here we use it to fill in the packet names.
 */

#define PACKET_DEFINE(n, name, sendname, send, recv) name,
#define PACKET_NULL(x)	"**null**",
#define PACKET_FILL(x)	"***ng***",
#define PACKET_END(x)

char	*control_name[] = {
#include "mcmsg_config.h"
};


/*
 * mcmsg_config.h has macros for every packet type.
 * Here we use it to fill in the send method names.
 */

#define PACKET_DEFINE(n, name, sendname, send, recv) sendname,
#define PACKET_NULL(x)	"MCTRL_NULL",
#define PACKET_FILL(x)	"***ng***",
#define PACKET_END(x)

char *method_name_list[] = {

#include "mcmsg_config.h"
	"**space**",
	"SELMETH_APP",
	"SELMETH_PID_TASK",
	"SELMETH_PID_SEL",
	"SELMETH_PID",
	"SELMETH_PTYPE",
	"SELMETH_TASK",
	"SELMETH_SEQ",
	"SELMETH_XMSG",
	"SELMETH_RECV_TYPESEL",
	"SELMETH_RECV_NODESEL",
	"SELMETH_RECV_ANY",
	"SELMETH_RECV_SRC",
	"SELMETH_RECV_TYPESET",
	"SELMETH_RECV_TYPE",
	"SELMETH_RECV_TYPESRC",
	"SELMETH_RECV_XMSG",
	"SELMETH_TASK_PTYPE",
	"SELMETH_NODE_PTYPE",
};

#endif MSG_TRACE
