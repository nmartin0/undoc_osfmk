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
 
/*
 * Dummy routines to resolve problems with adding nx_port.o to libc_r.a. 
 * A reentrant version of nx_port.o is added to libc_r.a to make handler
 * thread a pthread instead of mach thread. In case -lnx is not used in
 * the link process, then nx_port object complains of unresolved symbols.
 * These dummy symbols are added here for the pthread applications which
 * do not need to link in with libnx.a.
 */

#include <mcmsg/mcmsg_xmsg.h>
#include <mcmsg/mcmsg_nx.h>

unsigned long *_nx_perfmon_msginfo;
nxreq_t *nxreq;

_free_mid(){;}
_fulfill_nx_xmsg(){;}
_isend_forward(){;}
_msgignore(){;}
_msgmerge(){;}
_msgwait(){;}
_probe_nxreq(){;}
masktrap_spin_lock(){;}
masktrap_spin_unlock(){;}
mcmsg_masktrap(){;}
mcmsg_nx_send_continue(){;}
mcmsg_nxport_setup(){;}
myptype(){;}
nx_spin_lock(){;}
nx_spin_unlock(){;}
tree_recv(){;}
