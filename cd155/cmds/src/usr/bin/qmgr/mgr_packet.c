/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*++ mgr_packet.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qmgr/mgr_packet.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS manager program packet sending module.
 *
 *	Each of the functions defined below, returns the transaction
 *	completion codes as listed for each function.  In addition, all
 *	functions can return the transaction codes of:
 *
 *
 *		TCML_INTERNERR:	 if maximum packet size exceeded.
 *		TCML_NOESTABLSH: if unable to establish inter-
 *				 process communication.
 *		TCML_NOLOCALDAE: if the NQS daemon is not running.
 *		TCML_PROTOFAIL:	 if a protocol failure occurred.
 *
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	January 3, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1995/02/24 23:31:33 $ $State: Exp $)
 * $Log: mgr_packet.c,v $
 * Revision 1.5  1995/02/24  23:31:33  kremenek
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 5134
 *  Testing: Developer testing
 *  Module(s):	 ./cmds_libs/src/usr/bin/qmgr/mgr_cmd.c
 * 		./cmds_libs/src/usr/bin/qmgr/mgr_main.c
 * 		./cmds_libs/src/usr/bin/qmgr/mgr_packet.c
 * 		./cmds_libs/src/usr/bin/qmgr/qmgr.hlp
 * 		./cmds_libs/src/usr/ccs/lib/libnqs/listq.c
 * 		./cmds_libs/src/usr/include/nqs/nqspacket.h
 * 		./cmds_libs/src/usr/include/nqs/nqsstruct.h
 * 		./cmds_libs/src/usr/lib/nqs/macs_sched.c
 * 		./cmds_libs/src/usr/lib/nqs/nqs_ldconf.c
 * 		./cmds_libs/src/usr/lib/nqs/nqs_main.c
 * 		./cmds_libs/src/usr/lib/nqs/nqs_nsq.c
 * 		./cmds_libs/src/usr/lib/nqs/nqs_upq.
 *
 * Revision 1.4  1994/11/19  01:35:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  01:08:03  mwan
 * 1.2 mods
 *
 * Revision 1.2  1992/10/09  22:30:08  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:28:28  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:32  root
 * Version 2.0 Source
 * 
 * Revision 2.3  87/08/25  16:55:20  hender
 * uid_t gid_t now defined in /usr/netinclude/sys/types.h (4.3)
 * 
 * Revision 2.2  87/04/22  15:03:02  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)mgr_packet.c	1.3 (mgr_packet.c OSF/1 NQS2.0 GJK) 7/7/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#if	BSD43 | UTS | SGI | OSF
#include <sys/types.h>			/* Uid_t, gid_t */
#else
#if	BSD42 | SYS52 | ULTRIX | UNICOS 
#else
BAD SYSTEM TYPE
#endif
#endif
 
#ifdef SDSC
#include "transactcc.h"
#include <values.h>
#endif
 
#include NMAP				/* Mid_t (all OS's) */
					/* Uid_t and gid_t (if not BSD43) */
#include "nqspacket.h"			/* NQS local message packets */

extern long inter();			/* Send local packet */
extern void interclear();		/* Clear packet */
extern void interw32i();		/* Write packet integer datum */
extern void interw32u();		/* Write packet unsigned datum */
extern void interwstr();		/* Write packet string datum */


/*** aboque
 *
 *
 *	long aboque():
 *
 *	Send to the NQS daemon a packet which will abort all running
 *	requests in the named queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 */
long aboque (que_name, wait_time)
char *que_name;				/* Name of queue to be aborted */
int wait_time;				/* Wait time in seconds */
{
	interclear();
	interwstr (que_name);
	interw32i ((long) wait_time);
	return (inter (PKT_ABOQUE));
}


/*** addfor
 *
 *
 *	long addfor():
 *
 *	Send to the NQS daemon a packet which will add a form
 *	to the NQS forms set.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:	 if the form already exists.
 */
long addfor (form)
char *form;				/* Name of form */
{
	interclear();
	interwstr (form);
	return (inter (PKT_ADDFOR));
}


/*** addnqsman
 *
 *
 *	long addnqsman():
 *
 *	Send to the NQS daemon a packet which will add an account
 *	to the NQS manager access set.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:  if the specified account is already
 *				 an NQS manager.
 */
long addnqsman (mgr_uid, machine_id, privilege_bits)
uid_t mgr_uid;				/* User-id of manager account */
mid_t machine_id;			/* at Machine-id of the machine */
					/* for the named account */
int privilege_bits;			/* Manager privilege bits */
{
	interclear();
	interw32i ((long) mgr_uid);
	interw32i ((long) machine_id);
	interw32i ((long) privilege_bits);
	return (inter (PKT_ADDNQSMAN));
}


/*** addqcomque
 *
 *
 *	long addqcomque():
 *
 *	Send to the NQS daemon a packet which will add a queue
 *	to the queue set of a queue complex.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:	 if the specified queue already 
 *				 exists in the queue complex.
 *		TCML_INSUFFMEM:  if there was not sufficient heap
 *				 space available to the NQS daemon
 *				 to create the new destination.
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_NOSUCHCOM:	 if the named queue complex does 
 *				 not exist.
 *		TCML_WROQUETYP:	 if the specified local queue is
 *				 not a batch queue.
 */
long addqcomque (que_name, qcom_name)
char *que_name;				/* Name of queue */
char *qcom_name;			/* Name of queue complex*/
{
	interclear();
	interwstr (que_name);
	interwstr (qcom_name);
	return (inter (PKT_ADDQUECOM));
}

/*** addquedes
 *
 *
 *	long addquedes():
 *
 *	Send to the NQS daemon a packet which will add a queue
 *	to the destination set for a pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:	 if the specified destination for
 *				 the local queue already existed.
 *		TCML_INSUFFMEM:  if there was not sufficient heap
 *				 space available to the NQS daemon
 *				 to create the new destination.
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_SELREFDES:	 if the destination set for the pipe
 *				 queue would have been made self-
 *				 referential by the completion of
 *				 the transaction.
 *		TCML_WROQUETYP:	 if the specified local queue is
 *				 not a pipe queue.
 */
long addquedes (local_queue, dest_queue, dest_mid)
char *local_queue;			/* Name of local queue */
char *dest_queue;			/* Name of destination queue */
mid_t dest_mid;				/* Machine-id of destination */
{
	interclear();
	interwstr (local_queue);
	interwstr (dest_queue);
	interw32i ((long) dest_mid);
	return (inter (PKT_ADDQUEDES));
}


/*** addquedev
 *
 *
 *	long addquedev():
 *
 *	Send to the NQS daemon a packet which will add a device
 *	to the device set for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:	 if the mapping already exists.
 *		TCML_INSUFFMEM:	 if there was not sufficient memory
 *				 available to the NQS daemon to
 *				 establish the mapping.
 *		TCML_NOSUCHDEV:	 if the named device does not
 *				 exist.
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_WROQUETYP:	 if the named queue is not a
 *				 device queue.
 */
long addquedev (queue_name, device_name)
char *queue_name;			/* Name of queue */
char *device_name;			/* Name of device */
{
	interclear();
	interwstr (queue_name);
	interwstr (device_name);
	return (inter (PKT_ADDQUEDEV));
}

 
/*** addquegid
 *
 *
 *	long addquegid():
 *
 *	Send to the NQS daemon a packet which will add a gid
 *	to the access set for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADACC:	 if the group already has access
 *		TCML_INSUFFMEM:	 if there was not sufficient memory
 *				 available to the NQS daemon to
 *				 establish the access.
 *		TCML_NOSUCHGRP:	 if there is no such group on this
 *				 machine
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_UNRESTR:	 if access to this queue is unrestricted.
 */
long addquegid (queue_name, gid)
char *queue_name;			/* Name of queue */
gid_t gid;				/* Group id */
{
	interclear();
	interwstr (queue_name);
	interw32i ((long) gid);
	return (inter (PKT_ADDQUEGID));
}

 
/*** addqueuid
 *
 *
 *	long addqueuid():
 *
 *	Send to the NQS daemon a packet which will add a uid
 *	to the access set for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADACC:	 if the user already has access
 *		TCML_INSUFFMEM:	 if there was not sufficient memory
 *				 available to the NQS daemon to
 *				 establish the mapping.
 *		TCML_NOSUCHACC:	 if there is no such account on this
 *				 machine
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_UNRESTR:	 if access to this queue is unrestricted.
 */
long addqueuid (queue_name, uid)
char *queue_name;			/* Name of queue */
uid_t uid;				/* User id */
{
	interclear();
	interwstr (queue_name);
	interw32i ((long) uid);
	return (inter (PKT_ADDQUEUID));
}

 
/*** crebatque
 *
 *
 *	long crebatque():
 *
 *	Send to the NQS daemon a packet which will create
 *	a batch queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:	 if the named queue already exists.
 *		TCML_INSUFFMEM:	 if the NQS daemon did not have
 *				 sufficient memory to create the
 *				 new queue.
 */
long crebatque (que_name, que_priority, que_runlimit, que_pipeonly)
char *que_name;				/* Name of queue to be created */
int que_priority;			/* Queue priority [0..32767] */
int que_runlimit;			/* Queue run-limit */
int que_pipeonly;			/* Boolean pipeonly attr */
{
	interclear();
	interwstr (que_name);
	interw32i ((long) que_priority);
	interw32i ((long) que_runlimit);
	interw32i ((long) que_pipeonly);
	return (inter (PKT_CREBATQUE));
}


/*** crecom
 *
 *
 *	long crecom():
 *
 *	Send to the NQS daemon a packet which will create a queue complex.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:	 if the named queue complex already exists.
 *		TCML_INSUFFMEM:	 if the NQS daemon did not have
 *				 sufficient memory to create the
 *				 new queue.
 */
long crecom (qcom_name)
char *qcom_name;			/* Name of complex to be created */
{
	interclear();
	interwstr (qcom_name);
	return (inter (PKT_CRECOM));
}



/*** credev
 *
 *
 *	long credev():
 *
 *	Send to the NQS daemon a packet which will create a device.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:	 if the named device already exists.
 *		TCML_INSUFFMEM:	 if the NQS daemon did not have
 *				 sufficient memory to create the
 *				 new device.
 *		TCML_TOOMANDEV:	 if too many devices would exist,
 *				 if this device were successfully
 *				 created.
 */
long credev (dev_name, dev_forms, dev_fullname, dev_server)
char *dev_name;				/* Name of device to be created */
char *dev_forms;			/* Name of forms to be loaded */
					/* in the device */
char *dev_fullname;			/* Full pathname of device */
char *dev_server;			/* Server for device */
{
	interclear();
	interwstr (dev_name);
	interwstr (dev_forms);
	interwstr (dev_fullname);
	interwstr (dev_server);
	return (inter (PKT_CREDEV));
}


/*** credevque
 *
 *
 *	long credevque():
 *
 *	Send to the NQS daemon a packet which will create a device queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:	 if the named queue already exists.
 *		TCML_INSUFFMEM:	 if the NQS daemon did not have
 *				 sufficient memory to create the
 *				 new queue.
 */
long credevque (que_name, que_priority, que_pipeonly)
char *que_name;				/* Name of queue to be created */
int que_priority;			/* Queue priority [0..32767] */
int que_pipeonly;			/* Boolean pipeonly attr */
{
	interclear();
	interwstr (que_name);
	interw32i ((long) que_priority);
	interw32i ((long) que_pipeonly);
	return (inter (PKT_CREDEVQUE));
}


/*** crepipque
 *
 *
 *	long crepipque():
 *
 *	Send to the NQS daemon a packet which will create a pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:	 if the named queue already exists.
 *		TCML_INSUFFMEM:	 if the NQS daemon did not have
 *				 sufficient memory to create the
 *				 new queue.
 */
long crepipque (que_name, que_priority, que_runlimit, que_pipeonly, que_server)
char *que_name;				/* Name of queue to be created */
int que_priority;			/* Queue priority [0..32767] */
int que_runlimit;			/* Queue run-limit */
int que_pipeonly;			/* Boolean pipeonly attr */
char *que_server;			/* Server for queue */
{
	interclear();
	interwstr (que_name);
	interw32i ((long) que_priority);
	interw32i ((long) que_runlimit);
	interw32i ((long) que_pipeonly);
	interwstr (que_server);
	return (inter (PKT_CREPIPQUE));
} 


/*** delcom
 *
 *
 *	long delcom():
 *
 *	Send to the NQS daemon a packet which will delete a complex.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHCOM	 if no such complex exists;
 */
long delcom (qcom_name)
char *qcom_name;				/* complex name */
{
	interclear();
	interwstr (qcom_name);
	return (inter (PKT_DELCOM));
}


/*** deldev
 *
 *
 *	long deldev():
 *
 *	Send to the NQS daemon a packet which will delete a device.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_DEVACTIVE:	 if the named device is active.
 *		TCML_DEVENABLE:	 if the named device is enabled.
 *		TCML_NOSUCHDEV:	 if the named device does not
 *				 exist.
 */
long deldev (dev_name)
char *dev_name;				/* device name */
{
	interclear();
	interwstr (dev_name);
	return (inter (PKT_DELDEV));
}


/*** delfor
 *
 *
 *	long delfor():
 *
 *	Send to the NQS daemon a packet which will delete a form
 *	from the NQS forms set.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHFORM: if the named form does not
 *				 exist.
 */
long delfor (form)
char *form;				/* Name of form to delete */
{
	interclear();
	interwstr (form);
	return (inter (PKT_DELFOR));
}


/*** delnqsman
 *
 *
 *	long delnqsman():
 *
 *	Send to the NQS daemon a packet which will delete an account
 *	from the NQS manager access set.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHMAN:	 if the named manager account did
 *				 not exist.
 *		TCML_ROOTINDEL:	 if an attempt was made to delete
 *				 the system "root" account from the
 *				 NQS manager set.
 */
long delnqsman (mgr_uid, machine_id, privilege_bits)
uid_t mgr_uid;				/* User-id of manager */
mid_t machine_id;			/* at this machine */
int privilege_bits;			/* Manager privilege bits */
{
	interclear();
	interw32i ((long) mgr_uid);
	interw32i ((long) machine_id);
	interw32i ((long) privilege_bits);
	return (inter (PKT_DELNQSMAN));
}


/*** delque
 *
 *
 *	long delque():
 *
 *	Send to the NQS daemon a packet which will delete a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_QUEENABLE:	 if the specified queue is enabled.
 *		TCML_QUEHASREQ:	 if the specified queue has reqs
 *				 in it.
 */
long delque (que_name)
char *que_name;				/* Name of queue to be deleted */
{
	interclear();
	interwstr (que_name);
	return (inter (PKT_DELQUE));
}


/*** delquedes
 *
 *
 *	long delquedes():
 *
 *	Send to the NQS daemon a packet which will delete a
 *	destination from destination set for the specified pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHDES:   if the specified destination does
 *				 not exist for the local queue.
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_WROQUETYP:	 if the type of the local queue is
 *				 not a pipe queue.
 */
long delquedes (local_queue, dest_queue, dest_mid)
char *local_queue;			/* Name of local queue */
char *dest_queue;			/* Name of destination queue */
mid_t dest_mid;				/* Destination machine-id */
{
	interclear();
	interwstr (local_queue);
	interwstr (dest_queue);
	interw32i ((long) dest_mid);
	return (inter (PKT_DELQUEDES));
}


/*** delquedev
 *
 *
 *	long delquedev():
 *
 *	Send to the NQS daemon a packet which will delete a device
 *	from the device set for the specified queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHMAP:	 if the specified queue-to-device
 *				 mapping does not exist.
 *		TCML_NOSUCHQUE:	 if the specified queue does not
 *				 exist.
 *		TCML_WROQUETYP:	 if the specified queue is not a
 *				 device queue.
 */
long delquedev (queue_name, device_name)
char *queue_name;			/* Name of queue */
char *device_name;			/* Name of device */
{
	interclear();
	interwstr (queue_name);
	interwstr (device_name);
	return (inter (PKT_DELQUEDEV));
}


/*** delquegid
 *
 *
 *	long delquegid():
 *
 *	Send to the NQS daemon a packet which will delete a gid
 *	from the access set for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOACCNOW:	 if the group does not have access now
 *		TCML_NOSUCHGRP:	 if there is no such group on this
 *				 machine
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_UNRESTR:	 if access to this queue is unrestricted.
 */
long delquegid (queue_name, gid)
char *queue_name;			/* Name of queue */
gid_t gid;				/* Group id */
{
	interclear();
	interwstr (queue_name);
	interw32i ((long) gid);
	return (inter (PKT_DELQUEGID));
}

 
/*** delqueuid
 *
 *
 *	long delqueuid():
 *
 *	Send to the NQS daemon a packet which will delete a uid
 *	from the access set for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOACCNOW:	 if the user does not have access now
 *		TCML_NOSUCHACC:	 if there is no such account on this
 *				 machine
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_UNRESTR:	 if access to this queue is unrestricted.
 */
long delqueuid (queue_name, uid)
char *queue_name;			/* Name of queue */
uid_t uid;				/* User id */
{
	interclear();
	interwstr (queue_name);
	interw32i ((long) uid);
	return (inter (PKT_DELQUEUID));
}

 
/*** disdev
 *
 *
 *	long disdev():
 *
 *	Send to the NQS daemon a packet which will disable a device.,
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHDEV:	 if the named device does not
 *				 exist.
 */
long disdev (device_name)
char *device_name;			/* Name of the device */
{
	interclear();
	interwstr (device_name);
	return (inter (PKT_DISDEV));
}


/*** disque
 *
 *
 *	long disque():
 *
 *	Send to the NQS daemon a packet which will disable a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 */
long disque (queue_name)
char *queue_name;			/* Name of the queue */
{
	interclear();
	interwstr (queue_name);
	return (inter (PKT_DISQUE));
}


/*** enadev
 *
 *
 *	long enadev():
 *
 *	Send to the NQS daemon a packet which will enable a device.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHDEV:	 if the named device does not
 *				 exist.
 */
long enadev (device_name)
char *device_name;			/* Name of the device */
{
	interclear();
	interwstr (device_name);
	return (inter (PKT_ENADEV));
}


/*** enaque
 *
 *
 *	long enaque():
 *
 *	Send to the NQS daemon a packet which will enable a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 */
long enaque (queue_name)
char *queue_name;			/* Name of the queue */
{
	interclear();
	interwstr (queue_name);
	return (inter (PKT_ENAQUE));
}


/*** locdae
 *
 *
 *	long locdae():
 *
 *	Send to the NQS daemon a packet which will lock
 *	the NQS daemon in memory making the daemon immune
 *	to swapping activity.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_PLOCKFAIL:	 if the daemon call to plock()
 *				 failed.
 */
long locdae()
{
	interclear();
	return (inter (PKT_LOCDAE));
}


/*** modreq
 *
 *
 *	long modreq():
 *
 *	Send to the NQS daemon a packet which will modify
 *	the parameters of a request.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_NOSUCHREQ:	if request does not exist;
 *		TCML_NOSUCHQUO:	if this limit is meaningless 
 *				for this operating system;
 *		TCML_FIXBYNQS:	if successful, but NQS had to set the
 *				limit to a value other than requested;
 */
long modreq (orig_uid, req_seqno, req_mid, limit_type, param_value1,
 		param_value2, param_infinite)
uid_t orig_uid;			/* Originator's user-id */
int req_seqno;			/* Request sequence number */
mid_t req_mid;			/* Request machine id */
int limit_type;			/* Limit type */
long param_value1;		/* First part of parameter value */
long param_value2;		/* Second part of parameter value */
short param_infinite;		/* Infinite paramater value indicator */
{
	interclear();
	interw32i ((long) orig_uid);
	interw32i ((long) req_seqno);
	interw32i ((long) req_mid);
	interw32i ((long) limit_type);
	interw32i (param_value1);
	interw32i (param_value2);
	interw32i ((long) param_infinite);
	return (inter (PKT_MODREQ));
}


/*** movque
 *
 *
 *	long movque():
 *
 *	Send to the NQS daemon a packet which will move a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if a named queue does not
 *				 exist.
 */
long movque (src_qname,des_qname)
char *src_qname;				/* Name of source queue */
char *des_qname;				/* Name of destn queue */
{
	interclear();
	interwstr (src_qname);
	interwstr (des_qname);
	return (inter (PKT_MOVQUE));
}


/*** nqsshutdn
 *
 *
 *	long nqsshutdn():
 *
 *	Send to the NQS daemon a packet which will shutdown the NQS daemon.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_SHUTDNERR:  if unsuccessful;
 */
long nqsshutdn (wait_time)
int wait_time;				/* NQS process grace-period time */
					/* in seconds */
{
	interclear();
	interw32i ((long) wait_time);
	return (inter (PKT_SHUTDOWN));
}


/*** purque
 *
 *
 *	long purque():
 *
 *	Send to the NQS daemon a packet which will purge a specified queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 */
long purque (que_name)
char *que_name;				/* Name of queue to be purged */
{
	interclear();
	interwstr (que_name);
	return (inter (PKT_PURQUE));
}


/*** remqcomque
 *
 *
 *	long remqcomque():
 *
 *	Send to the NQS daemon a packet which will remove a queue
 *	from the queue set of a queue complex.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHCOM:	 if the named queue complex does
 *				 not exist.	
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist in the queue complex.
 */
long remqcomque (que_name,qcom_name)
char *que_name;				/* Name of queue to be removed */
char *qcom_name;			/* Name of queue complex */
{
	interclear();
	interwstr (que_name);
	interwstr (qcom_name);
	return (inter (PKT_REMQUECOM));
}


/*** setbatpri
 *
 *
 *	long setbatpri():
 *
 *	Send to the NQS daemon a packet which will set a default
 *	intra-queue batch request priority.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setbatpri (newpri)
int newpri;				/* New intra-queue priority level */
{
	interclear();
	interw32i ((long) newpri);
	return (inter (PKT_SETDEFBATPR));
}


/*** setbatque
 *
 *
 *	long setbatque():
 *
 *	Send to the NQS daemon a packet which will set
 *	a default batch queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 */
long setbatque (que_name)
char *que_name;				/* Name of default batch queue */
{
	interclear();
	interwstr (que_name);
	return (inter (PKT_SETDEFBATQU));
}


/*** setcomrun
 *
 *
 *	long setcomrun():
 *
 *	Send to the NQS daemon a packet which will set a queue complex
 *	run-limit.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHCOM:	 if the named complex does not
 *				 exist.
 */
long setcomrun (qcom_name, run_limit)
char *qcom_name;			/* Name of queue complex */
int run_limit;				/* Maximum number of reqs allowed */
					/* to run in the named complex at */
					/* any given time */
{
	interclear();
	interwstr (qcom_name);
	interw32i ((long) run_limit);
	return (inter (PKT_SETCOMLIM));
}


/*** setdeblev
 *
 *
 *	long setdeblev():
 *
 *	Send to the NQS daemon a packet which will set the debug level.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setdeblev (debug_level)
int debug_level;			/* NQS new debug level */
{
	interclear();
	interw32i ((long) debug_level);
	return (inter (PKT_SETDEB));
}


/*** setdestim
 *
 *
 *	long setdestim():
 *
 *	Send to the NQS daemon a packet which will set the default
 *	number of seconds that can elapse during which time a pipe
 *	queue destination can be unreachable, before being marked
 *	as completely failed.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setdestim (retrydelta)
long retrydelta;		/* Default destination retry time tolerance */
{
	interclear();
	interw32i (retrydelta);
	return (inter (PKT_SETDEFDESTI));
}


/*** setdeswai
 *
 *
 *	long setdeswai():
 *
 *	Send to the NQS daemon a packet which will set
 *	the default destination wait time in seconds between
 *	failed destination connection attempts.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setdeswai (destwait)
long destwait;		/* Default time to wait between failed destination */
{			/* connections */
	interclear();
	interw32i (destwait);
	return (inter (PKT_SETDEFDESWA));
}


/*** setdevfor
 *
 *
 *	long setdevfor():
 *
 *	Send to the NQS daemon a packet which will set
 *	the device forms for a device.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHDEV:	 if the named device does not
 *				 exist.
 *		TCML_NOSUCHFORM: if the specified form is not
 *				 defined in the local NQS forms
 *				 set.
 */
long setdevfor (dev_name, forms_name)
char *dev_name;				/* Name of device */
char *forms_name;			/* New forms name */
{
	interclear();
	interwstr (dev_name);
	interwstr (forms_name);
	return (inter (PKT_SETDEVFOR));
}


/*** setdevpri
 *
 *
 *	long setdevpri():
 *
 *	Send to the NQS daemon a packet which will set
 *	the default intra-queue device request priority.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setdevpri (newpri)
int newpri;				/* New intra-queue priority level */
{
	interclear();
	interw32i ((long) newpri);
	return (inter (PKT_SETDEFDEVPR));
}


/*** setdevser
 *
 *
 *	long setdevser():
 *
 *	Send to the NQS daemon a packet which will set
 *	the device server for a device.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHDEV:	 if the named device does not
 *				 exist.
 */
long setdevser (dev_name, server_name)
char *dev_name;				/* Name of device */
char *server_name;			/* New server name */
{
	interclear();
	interwstr (dev_name);
	interwstr (server_name);
	return (inter (PKT_SETDEVSER));
}


/*** setfor
 *
 *
 *	long setfor():
 *
 *	Send to the NQS daemon a packet which will set
 *	the NQS forms set to the single specified form.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setfor (form)
char *form;				/* Single form in forms set */
{
	interclear();
	interwstr (form);
	return (inter (PKT_SETFOR));
}


/*** setgblbat
 *
 *
 *	long setgblbat():
 *
 *	Send to the NQS daemon a packet which will set the global
 *	batch-limit.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *				 exist.
 */
long setgblbat ( bat_limit)
int bat_limit;				/* Maximum number of batch requests */
					/* allowed to run simultaneously */
{
	interclear();
	interw32i ((long) bat_limit);
	return (inter (PKT_SETBATLIM));
}


#ifdef SDSC

/*** setsoftulim
 *
 *
 *	long setsoftulim():
 *
 *	Send to the NQS daemon a packet which will set the soft
 *	user-limit.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *				 exist.
 */
long setsoftulim ( user_limit)
int user_limit;				/* Maximum number of nodes */
					/* allowed to run simultaneously */
{
	interclear();
	interw32i ((long) user_limit);
	return (inter (PKT_SETSOFTULIM));
}

/*** sethardulim
 *
 *
 *	long sethardulim():
 *
 *	Send to the NQS daemon a packet which will set the hard
 *	user-limit.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *				 exist.
 */
long sethardulim ( user_limit)
int user_limit;				/* Maximum number of nodes */
					/* allowed to run simultaneously */
{
	interclear();
	interw32i ((long) user_limit);
	return (inter (PKT_SETHARDULIM));
}

#endif

/*** setlife
 *
 *
 *	long setlife():
 *
 *	Send to the NQS daemon a packet which will set
 *	the pipe queue request lifetime.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setlife (lifetime)
long lifetime;				/* Pipe queue req lifetime */
{
	interclear();
	interw32i (lifetime);
	return (inter (PKT_SETLIFE));
}


/*** setlogfil
 *
 *
 *	long setlogfil():
 *
 *	Send to the NQS daemon a packet which will instruct
 *	the NQS daemon to use a different file in the recording
 *	of errors and messages.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_LOGFILERR:	 if the NQS daemon was unable to
 *				 open or create the specified
 *				 logfile.
 */
long setlogfil (logfile_name)
char *logfile_name;			/* Name of new logfile */
{
	interclear();
	interwstr (logfile_name);
	return (inter (PKT_SETLOGFIL));
}


/*** setmaxcop
 *
 *
 *	long setmaxcop():
 *
 *	Send to the NQS daemon a packet which will set a maximum
 *	on the number of print copies.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setmaxcop (max_copies)
long max_copies;				/* New max. copy limit */
{
	interclear();
	interw32i (max_copies);
	return (inter (PKT_SETMAXCOP));
}


/*** setmaxopr
 *
 *
 *	long setmaxopr():
 *
 *	Send to the NQS daemon a packet which will set a maximum
 *	failed device open retry count.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setmaxopr (maxretries)
long maxretries;			/* Max-failed device open retry count */
{
	interclear();
	interw32i (maxretries);
	return (inter (PKT_SETMAXOPERE));
}


/*** setmaxpsz
 *
 *
 *	long setmaxpsz():
 *
 *	Send to the NQS daemon a packet which will set a maximum
 *	on print file size.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setmaxpsz (maxprintsize)
long maxprintsize;		/* Max-print file size */
{
	interclear();
	interw32i (maxprintsize);
	return (inter (PKT_SETMAXPRISI));
}


/*** setndfbat
 *
 *
 *	long setndfbat():
 *
 *	Send to the NQS daemon a packet which will cause there to be
 *	no default batch queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setndfbat ()
{
	interclear();
	return (inter (PKT_SETNDFBATQU));
}


/*** setndffor
 *
 *
 *	long setndffor():
 *
 *	Send to the NQS daemon a packet which will cause there to be
 *	no default print form.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setndffor ()
{
	interclear();
	return (inter (PKT_SETNDFPRIFO));
}


/*** setndfpri
 *
 *
 *	long setndfpri():
 *
 *	Send to the NQS daemon a packet which will cause there to be
 *	no default print queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setndfpri ()
{
	interclear();
	return (inter (PKT_SETNDFPRIQU));
}


/*** setnetcli
 *
 *
 *	long setnetcli():
 *
 *	Send to the NQS local daemon a packet which will set
 *	the NQS network client.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setnetcli (network_client_name)
char *network_client_name;		/* Pathname, args of network client */
{
	interclear();
	interwstr (network_client_name);
	return (inter (PKT_SETNETCLI));
}


/*** setnetdae
 *
 *
 *	long setnetdae():
 *
 *	Send to the NQS local daemon a packet which will set
 *	the NQS network daemon.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setnetdae (network_daemon_name)
char *network_daemon_name;		/* Pathname, args of network daemon */
{
	interclear();
	interwstr (network_daemon_name);
	return (inter (PKT_SETNETDAE));
}


/*** setnetser
 *
 *
 *	long setnetser():
 *
 *	Send to the NQS local daemon a packet which will set
 *	the NQS network server.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setnetser (network_server_name)
char *network_server_name;		/* Pathname, args of network server */
{
	interclear();
	interwstr (network_server_name);
	return (inter (PKT_SETNETSER));
}


/*** setnnedae
 *
 *
 *	long setnnedae():
 *
 *	Send to the NQS daemon a packet which will cause there to be
 *	no network daemon.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setnnedae ()
{
	interclear();
	return (inter (PKT_SETNONETDAE));
}


/*** setnoponl
 *
 *
 *	long setnoponl():
 *
 *	Send to the NQS daemon a packet which will clear
 *	the pipeonly entry attribute for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 */
long setnoponl (queue_name)
char *queue_name;			/* Name of the queue */
{
	interclear();
	interwstr (queue_name);
	return (inter (PKT_SETNOPIPONL));
}


/*** setnoqueacc
 *
 *
 *	long setnoqueacc ():
 *
 *	Send to the NQS daemon a packet which will set the
 *	access set for a queue to none.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 */
long setnoqueacc (queue_name)
char *queue_name;			/* Name of queue */
{
	interclear();
	interwstr (queue_name);
	return (inter (PKT_SETNOQUEACC));
}

 
/*** setnqsmai
 *
 *
 *	long setnqsmai():
 *
 *	Send to the NQS daemon a packet which will set
 *	the NQS mail account to the specified account user-id.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHACC:	 if the specified account does
 *				 not exist.
 */
long setnqsmai (nqs_mail_account)
uid_t nqs_mail_account;			/* New NQS mail account user-id */
{
	interclear();
	interw32i ((long) nqs_mail_account);
	return (inter (PKT_SETNQSMAI));
}


/*** setnqsman
 *
 *
 *	long setnqsman():
 *
 *	Send to the NQS daemon a packet which will set the
 *	NQS manager access set to the single account
 *	of root, with all privileges.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setnqsman ()
{
	interclear();
	return (inter (PKT_SETNQSMAN));
}


/*** setopewai
 *
 *
 *	long setopewai():
 *
 *	Send to the NQS daemon a packet which will set the open
 *	wait time in seconds between failed device open attempts.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setopewai (openwait)
long openwait;			/* Time to wait between failed device opens */
{
	interclear();
	interw32i (openwait);
	return (inter (PKT_SETOPEWAI));
}


/*** setpipcli
 *
 *
 *	long setpipcli():
 *
 *	Send to the NQS daemon a packet which will set
 *	the pipe client for a pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_WROQUETYP:	 if the named queue is not a pipe queue.
 */
long setpipcli (que_name, client_name)
char *que_name;				/* Name of queue */
char *client_name;			/* Name of client */
{
	interclear();
	interwstr (que_name);
	interwstr (client_name);
	return (inter (PKT_SETPIPCLI));
}


/*** setpiponl
 *
 *
 *	long setpiponl():
 *
 *	Send to the NQS daemon a packet which will set
 *	the pipeonly entry attribute for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 */
long setpiponl (queue_name)
char *queue_name;			/* Name of the queue */
{
	interclear();
	interwstr (queue_name);
	return (inter (PKT_SETPIPONL));
}


/*** setppcore
 *
 *
 *	long setppcore():
 *
 *	Send to the NQS daemon a packet which will set
 *	the per process core file size limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setppcore (queue_name, new_quota, new_units, infinite)
char *queue_name;
unsigned long new_quota;
short new_units;			/* QLM_??? */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_quota);
	interw32i ((long) new_units);
	interw32i ((long) infinite);
	return (inter (PKT_SETPPCORE));
}

#ifdef SDSC
/*** setquequed
 *
 *
 *      long setquequed():
 *
 *      Send to the NQS daemon a packet which will set a queue
 *      (queued+run) per user limit.
 *
 *      Returns:
 *              TCML_COMPLETE:   if successful;
 *              TCML_FIXBYNQS:   if sucessful, but NQS had to set the
 *                               limit to a value other than that
 *                               requested;
 *              TCML_GRANFATHER: if successful, but a queued request
 *                               will be given a grandfather clause.
 *              TCML_NOSUCHQUE:  if the named queue does not exist.
 *              TCML_NOSUCHQUO:  if this limit is meaningless
 *                               for this operating system.
 *              TCML_WROQUETYP:  if this limit cannot be set for
 *                               this queue type.
 */
long setquequed (queue_name, new_quota, new_units, infinite)
char *queue_name;
unsigned long new_quota;
short new_units;                        /* QLM_??? */
short infinite;                         /* Non-zero if infinite */
{
        interclear();
        interwstr (queue_name);
        interw32u (new_quota);
        interw32i ((long) new_units);
        interw32i ((long) infinite);
        if ((short)MAXSHORT <  new_quota ||
           new_quota < 1)
           if (infinite != 1) return(TCML_NOSUCHQUO);
        return (inter (PKT_SETQUEQDLIM));
}
#endif
 

/*** setppcput
 *
 *
 *	long setppcput():
 *
 *	Send to the NQS daemon a packet which will set
 *	the per process cpu time limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setppcput (queue_name, new_seconds, new_ms, infinite)
char *queue_name;			/* Name of the queue */
unsigned long new_seconds;		/* Integer part */
short new_ms;				/* Milliseconds */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_seconds);
	interw32i ((long) new_ms);
	interw32i ((long) infinite);
	return (inter (PKT_SETPPCPUT));
}


/*** setppdata
 *
 *
 *	long setppdata():
 *
 *	Send to the NQS daemon a packet which will set
 *	the per process data segment size limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setppdata (queue_name, new_quota, new_units, infinite)
char *queue_name;
unsigned long new_quota;
short new_units;			/* QLM_??? */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_quota);
	interw32i ((long) new_units);
	interw32i ((long) infinite);
	return (inter (PKT_SETPPDATA));
}


/*** setppmem
 *
 *
 *	long setppmem():
 *
 *	Send to the NQS daemon a packet which will set
 *	the per process memory size limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setppmem (queue_name, new_quota, new_units, infinite)
char *queue_name;
unsigned long new_quota;
short new_units;			/* QLM_??? */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_quota);
	interw32i ((long) new_units);
	interw32i ((long) infinite);
	return (inter (PKT_SETPPMEM));
}


/*** setppnice
 *
 *
 *	long setppnice():
 *
 *	Send to the NQS daemon a packet which will set
 *	the nice value for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system;
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setppnice (queue_name, new_nice)
char *queue_name;
int new_nice;
{
	interclear();
	interwstr (queue_name);
	interw32i ((long) new_nice);
	return (inter (PKT_SETPPNICE));
}


/*** setpppfile
 *
 *
 *	long setpppfile():
 *
 *	Send to the NQS daemon a packet which will set
 *	the per process permanent file size limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setpppfile (queue_name, new_quota, new_units, infinite)
char *queue_name;
unsigned long new_quota;
short new_units;			/* QLM_??? */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_quota);
	interw32i ((long) new_units);
	interw32i ((long) infinite);
	return (inter (PKT_SETPPPFILE));
}


/*** setppstack
 *
 *
 *	long setppstack():
 *
 *	Send to the NQS daemon a packet which will set
 *	the per process stack size limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setppstack (queue_name, new_quota, new_units, infinite)
char *queue_name;
unsigned long new_quota;
short new_units;			/* QLM_??? */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_quota);
	interw32i ((long) new_units);
	interw32i ((long) infinite);
	return (inter (PKT_SETPPSTACK));
}


/*** setpptfile
 *
 *
 *	long setpptfile():
 *
 *	Send to the NQS daemon a packet which will set
 *	the per process temporary file size limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setpptfile (queue_name, new_quota, new_units, infinite)
char *queue_name;
unsigned long new_quota;
short new_units;			/* QLM_??? */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_quota);
	interw32i ((long) new_units);
	interw32i ((long) infinite);
	return (inter (PKT_SETPPTFILE));
}


/*** setppwork
 *
 *
 *	long setppwork():
 *
 *	Send to the NQS daemon a packet which will set
 *	the working set size limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setppwork (queue_name, new_quota, new_units, infinite)
char *queue_name;
unsigned long new_quota;
short new_units;			/* QLM_??? */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_quota);
	interw32i ((long) new_units);
	interw32i ((long) infinite);
	return (inter (PKT_SETPPWORK));
}


/*** setprcput
 *
 *
 *	long setprcput():
 *
 *	Send to the NQS daemon a packet which will set
 *	the per request cpu time limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setprcput (queue_name, new_seconds, new_ms, infinite)
char *queue_name;			/* Name of the queue */
unsigned long new_seconds;		/* Integer part */
short new_ms;				/* Milliseconds */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_seconds);
	interw32i ((long) new_ms);
	interw32i ((long) infinite);
	return (inter (PKT_SETPRCPUT));
}


/*** setprifor
 *
 *
 *	long setprifor():
 *
 *	Send to the NQS daemon a packet which will set
 *	the default print forms.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHFORM: if the specified form is not
 *				 defined in the forms database.
 */
long setprifor (formsname)
char *formsname;			/* Default forms name */
{
	interclear();
	interwstr (formsname);
	return (inter (PKT_SETDEFPRIFO));
}


/*** setprique
 *
 *
 *	long setprique():
 *
 *	Send to the NQS daemon a packet which will set
 *	the default print queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 */
long setprique (que_name)
char *que_name;				/* Name of default print queue */
{
	interclear();
	interwstr (que_name);
	return (inter (PKT_SETDEFPRIQU));
}


/*** setprmem
 *
 *
 *	long setprmem():
 *
 *	Send to the NQS daemon a packet which will set
 *	the per request memory size limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setprmem (queue_name, new_quota, new_units, infinite)
char *queue_name;
unsigned long new_quota;
short new_units;			/* QLM_??? */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_quota);
	interw32i ((long) new_units);
	interw32i ((long) infinite);
	return (inter (PKT_SETPRMEM));
}
#ifdef SDSC
/*** setprcpus
 *
 *
 *	long setprncpus():
 *
 *	Send to the NQS daemon a packet which will set
 *	the ncpus value for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system;
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setprncpus (queue_name, new_ncpus)
char *queue_name;
int new_ncpus;
{
	interclear();
	interwstr (queue_name);
	interw32i ((long) new_ncpus);
	return (inter (PKT_SETPRNCPUS));
}

/*** setnodegrp
 *
 *
 *	long setnodegrp():
 *
 *	Send to the NQS daemon a packet which will set
 *	the nodegrp value for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system;
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setnodegrp (queue_name, new_nodegrp)
char *queue_name;
int new_nodegrp;
{
	interclear();
	interwstr (queue_name);
	interw32i ((long) new_nodegrp);
	return (inter (PKT_SETNODEGRP));
}
#endif


/*** setprpfile
 *
 *
 *	long setprpfile():
 *
 *	Send to the NQS daemon a packet which will set
 *	the per request permanent file size limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setprpfile (queue_name, new_quota, new_units, infinite)
char *queue_name;
unsigned long new_quota;
short new_units;			/* QLM_??? */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_quota);
	interw32i ((long) new_units);
	interw32i ((long) infinite);
	return (inter (PKT_SETPRPFILE));
}


/*** setprtfile
 *
 *
 *	long setprtfile():
 *
 *	Send to the NQS daemon a packet which will set
 *	the per request temporary file size limit for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_FIXBYNQS:	 if sucessful, but NQS had to set the
 *				 limit to a value other than that
 *				 requested;
 *		TCML_GRANFATHER: if successful, but a queued request
 *				 will be given a grandfather clause.
 *		TCML_NOSUCHQUE:	 if the named queue does not exist.
 *		TCML_NOSUCHQUO:	 if this limit is meaningless
 *				 for this operating system.
 *		TCML_WROQUETYP:	 if this limit cannot be set for
 *				 this queue type.
 */
long setprtfile (queue_name, new_quota, new_units, infinite)
char *queue_name;
unsigned long new_quota;
short new_units;			/* QLM_??? */
short infinite;				/* Non-zero if infinite */
{
	interclear();
	interwstr (queue_name);
	interw32u (new_quota);
	interw32i ((long) new_units);
	interw32i ((long) infinite);
	return (inter (PKT_SETPRTFILE));
}


/*** setquedes
 *
 *
 *	long setquedes():
 *
 *	Send to the NQS daemon a packet which will set
 *	the queue to destination set for a pipe queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:	 if the specified destination for
 *				 the local queue already existed.
 *		TCML_INSUFFMEM:  if there was not sufficient heap
 *				 space available to the NQS daemon
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_SELREFDES:	 if the destination set for the pipe
 *				 queue would have been made self-
 *				 referential by the completion of
 *				 the transaction.
 *		TCML_WROQUETYP:	 if the specified local queue is
 *				 not a pipe queue.
 */
long setquedes (local_queue, dest_queue, dest_mid)
char *local_queue;			/* Name of local queue */
char *dest_queue;			/* Name of destination queue */
mid_t dest_mid;				/* Machine-id of destination */
{
	interclear();
	interwstr (local_queue);
	interwstr (dest_queue);
	interw32i ((long) dest_mid);
	return (inter (PKT_SETQUEDES));
}


/*** setquedev
 *
 *
 *	long setquedev():
 *
 *	Send to the NQS daemon a packet which will set
 *	the device set for a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_ALREADEXI:	 if the mapping already exists.
 *		TCML_INSUFFMEM:	 if there was not sufficient memory
 *				 available to the NQS daemon to
 *				 establish the mapping.
 *		TCML_NOSUCHDEV:	 if the named device does not
 *				 exist.
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_WROQUETYP:	 if the named queue is not a
 *				 device queue.
 */
long setquedev (queue_name, device_name)
char *queue_name;			/* Name of queue */
char *device_name;			/* Name of device */
{
	interclear();
	interwstr (queue_name);
	interwstr (device_name);
	return (inter (PKT_SETQUEDEV));
}

 
/*** setquepri
 *
 *
 *	long setquepri():
 *
 *	Send to the NQS daemon a packet which will set a queue priority.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 */
long setquepri (que_name, new_priority)
char *que_name;				/* Name of queue */
int new_priority;			/* New priority */
{
	interclear();
	interwstr (que_name);
	interw32i ((long) new_priority);
	return (inter (PKT_SETQUEPRI));
}


/*** setquerun
 *
 *
 *	long setquerun():
 *
 *	Send to the NQS daemon a packet which will set a queue run-limit.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 *		TCML_WROQUETYP:	 if the named queue is a device-
 *				 queue.
 */
long setquerun (que_name, run_limit)
char *que_name;				/* Name of queue */
int run_limit;				/* Maximum number of reqs allowed */
					/* to run in the named queue at */
					/* any given time */
{
	interclear();
	interwstr (que_name);
	interw32i ((long) run_limit);
	return (inter (PKT_SETQUERUN));
}
 

/*** setshsfix
 *
 *
 *	long setshsfix():
 *
 *	Send to the NQS daemon a packet which will set
 *	a fixed shell strategy.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setshsfix (shell_name)
char *shell_name;		/* Name of shell */
{
	interclear();
	interwstr (shell_name);
	return (inter (PKT_SETSHSFIX));
}


/*** setshsfre
 *
 *
 *	long setshsfre():
 *
 *	Send to the NQS daemon a packet which will set
 *	a free shell strategy.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setshsfre()
{
	interclear();
	return (inter (PKT_SETSHSFRE));
}


/*** setshslog
 *
 *
 *	long setshslog():
 *
 *	Send to the NQS daemon a packet which will set
 *	a login shell strategy.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 */
long setshslog()
{
	interclear();
	return (inter (PKT_SETSHSLOG));
}


/*** setunrqueacc
 *
 *
 *	long setunrqueacc():
 *
 *	Send to the NQS daemon a packet which will set the
 *	access set for a queue to unrestricted.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 */
long setunrqueacc (queue_name)
char *queue_name;			/* Name of queue */
{
	interclear();
	interwstr (queue_name);
	return (inter (PKT_SETUNRQUEAC));
}

 
/*** staque
 *
 *
 *	long staque():
 *
 *	Send to the NQS daemon a packet which will start a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 */
long staque (que_name)
char *que_name;				/* Name of queue to be started */
{
	interclear();
	interwstr (que_name);
	return (inter (PKT_STAQUE));
}


/*** stoque
 *
 *
 *	long stoque():
 *
 *	Send to the NQS daemon a packet which will stop a queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHQUE:	 if the named queue does not
 *				 exist.
 */
long stoque (que_name)
char *que_name;				/* Name of queue to be stopped */
{
	interclear();
	interwstr (que_name);
	return (inter (PKT_STOQUE));
}


/*** unldae
 *
 *
 *	long unldae():
 *
 *	Send to the NQS daemon a packet which will unlock
 *	the NQS daemon from memory making it subject to swapping.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_PLOCKFAIL:	 if the daemon call to plock()
 *				 failed.
 */
long unldae ()
{
	interclear();
	return (inter (PKT_UNLDAE));
}
