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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991  Intel Corporation.
 */

/*
 * $Id: rklib.c,v 1.42 1994/11/19 02:32:30 mtm Exp $
 *
 * RK interface library
 *
 * HISTORY
 * $Log: rklib.c,v $
 * Revision 1.42  1994/11/19  02:32:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.41  1994/07/16  00:26:32  joel
 *  Reviewer: regnier
 *  Risk: low
 *  Benefit or PTS #: 7214
 *  Testing: irxnode, message EATS
 *  Module(s): nx_initve.c rklib.c mcmsg_appl.h
 *
 * Revision 1.40  1994/06/01  16:52:35  terry
 * This fix removes the touching of all BSS pages.  The touching of the pages
 * was to fix bug 6942, 6878 and 7878 and is no longer needed as the underlining
 * problem was fixed by fitz.
 *
 *  Reviewer: regnier, joel, tnt
 *  Risk: low
 *  Benefit or PTS #: 9244
 *  Testing: ran previous failing testcases on watson also ran sats on kepler
 *  Module(s): ../usr/ccs/lib/libnx/rklib.c
 *
 * Revision 1.39  1994/01/27  20:49:49  joel
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: 7878
 *  Testing: EATS
 *  Module(s): rklib.c
 *
 * Revision 1.38  1994/01/20  18:43:43  joel
 *  Reviewer:
 *  Risk: low
 *  Benefit or PTS #: 7878
 *  Testing: ran test case, PASSED
 *  Module(s): rklib.c
 *
 * Revision 1.37  1993/12/03  00:16:16  tnt
 *  Reviewer: none
 *  Risk:     Low
 *  Benefit or PTS #: Fixed a potential problem with two threads manipulating
 *                    the xmsg buffer queues.
 *  Testing:  Message Passing EATs, irxnode, TAMU
 *  Module(s): rklib.c
 *
 * Added an nx spin lock around the _xprovide() call in the
 * _set_provide_threshold() routine.  Callers to _xprovide() must have
 * grabbed the nx lock to prevent multiple threads from manipulating the
 * xmsg buffer queues.
 *
 * Revision 1.36  1993/12/01  19:55:33  joel
 *  Reviewer: Terry Prickett
 *  Risk: medium
 *  Benefit or PTS #: 6942 & 6878   NAS parallel benchmarks deadlocked the
 * 	kernel when more than 170 processes all tried to page in the same
 * 	page at the same time.  Rather than fix the MACH VM/XMM code we
 * 	are working around the bug by touching all .bss at load time with
 * 	access skewed so no nodes are touching the same page at the same
 * 	time. Also will get rid of the "first time through the code is
 * 	always slower" phenomena.
 *  Testing: NAS APPSP and APPBP benchmarks
 *  Module(s): rklib.c
 *
 * Revision 1.35  1993/11/18  19:25:30  dleslie
 *  Reviewer: shala
 *  Risk: low
 *  Benefit or PTS #: get nx and mcmsg headers out of export tree, not obj
 * 	tree.  This allows users to build without having an obj tree fully
 * 	populated with headers
 *  Testing: built libnx
 *  Module(s):
 *     Makefile _gcol.c _gcolx.c _gops.c _gsync.c _load.c allocUser.c
 *     allocsys.c allocsys_.c autoinit.c bitmap.c bitmap2.c create.c
 *     nodeparser.c nx_initve.c nx_load.c nx_load_.c nx_loadve.c
 *     nx_loadve_.c nx_lock.c nx_part_ops.c nx_part_ops_.c nx_port.c
 *     nx_pri.c nxlib.c parsepart.c parsesched.c partlock.c
 *     partprint.c partutils.c rkassert.c rklib.c rkmem.c utils.c
 *     writepart.c
 *
 *
 * VS:    writepart.c
 *
 * Revision 1.34  1993/10/12  19:48:05  shala
 * Remove _mynode() to define it in _mynode.c file in common_nx_c directory.
 *
 * Revision 1.33  1993/09/29  17:26:33  shala
 * Removed _numnodes(). It is now in common_nx_c/_numnodes.c to be shared
 * with libc. This is done to eliminate the need to link with libnx.a by default.
 *
 * Revision 1.32  1993/09/20  17:54:13  shala
 * Removed _myptype() routine and mcsmg_ptype declaration. They are now
 * defined in _myptype.c in usr/ccs/lib/common_nx_c directory. This is
 * needed to resolve problem with linking libnx.a by default. The linker
 * in R1.2 will be modified to exclude linking libnx.a by default.
 *
 * Revision 1.31  1993/09/09  22:02:52  andrews
 * Added support for set_provide_threshold() call.
 *
 * Revision 1.30  1993/07/29  21:35:11  tnt
 * Added nx_spin_lock code around critical sections.  Fixed problem with
 * allocation of xmsg buffer area which set mcmsg_buffer pointing to an address
 * ahead of the allocated buffer space.
 *
 * Revision 1.29  1993/07/06  23:58:28  andrews
 * Added xmsg buffer coalescing.
 *
 * Revision 1.28  1993/06/22  22:09:01  prp
 * Move all buffered message matching to the kernel.
 *
 * Revision 1.27  1993/06/16  22:16:03  andrews
 * Fixed problem with local sends in _t_xsend() when mcmsg_incoming_head ==
 * mcmsg_incoming_middle.
 *
 * Revision 1.26  1993/06/14  23:08:07  shala
 * Use new names for ptype and mynode variables which matches the
 * name used in the interface. Fixed by joel.
 *
 * Revision 1.25  1993/06/11  23:15:37  shala
 * Added additional debug code to _x_free(). Fixed by andrews.
 *
 * Revision 1.24  1993/06/10  16:26:36  andrews
 * Added split xmsg buffer support.
 *
 * Revision 1.23  1993/05/23  00:52:41  prp
 * Message processor interface, moved NX calls, flick fix, library cleanup.
 *
 * Revision 1.22  1993/04/02  23:50:13  top
 * corrected some provide problems in _xprovide().
 *
 * Revision 1.21  1993/03/23  19:45:49  hays
 * Modified nx_port.c, nxlib.c, and rklib.c for performance monitoring.
 * the macros are in the kernel header file mcmsg_xmsg.h
 *
 * Revision 1.20  1993/03/07  00:45:08  regnier
 * Make provide more aggressive at feeding the kernel with buffer space.
 * Generally, provide pieces as small as memory_export/4.
 *
 * Revision 1.19  1992/12/16  17:53:50  leonard
 * Added the function (and support for) _xavail() to rkmem: this function returns
 * the size of the largest available contiguous block of memory.
 *
 * The function _xprovide(), in rklib, now uses _xavail() to determine how
 * much memory to allocate if the first call to _xmalloc() fails.
 *
 * These modifications fix bug 2757.
 *
 * Revision 1.18  1992/10/31  17:50:42  joel
 *  Fixed PTS 2722 and 3151.  Now mcmsg_init returns -2 if it cannot wire
 * down the user memory without leaving too few pages for the system to continue
 * to operate.
 *
 * Revision 1.17  1992/09/03  07:55:07  shala
 * Renamed rk interface instead of disabling them, since they are used internaly.
 *
 * Revision 1.16  92/09/02  07:44:47  shala
 * Disabled rk interface routines until they get fixed
 * 
 * Revision 1.15  92/09/01  16:50:55  joel
 * Fix _myptype,_mynode,_myhost,_numnodes,_infocount,_infonode,_infotype, & 
 * _infoptype to return -1 if application has never called nx_initve ( or
 * autoinit()).
 * 
 * Revision 1.14  92/08/20  14:36:47  regnier
 * Change from preallocated mcmsg_buffer to dynamically allocated,
 * variable sized mcmsg_buffer. This implements the -mbf application
 * switch.
 * 
 * Revision 1.13  92/06/17  17:35:43  prp
 * Bug 2682, 2nd try
 * 
 * Revision 1.12  92/06/09  21:20:47  stans
 * added ID & ident strings
 * 
 */
char _rklib_c_id[]="$Id: rklib.c,v 1.42 1994/11/19 02:32:30 mtm Exp $";

#include <stdio.h>
#include <errno.h>
#include <mcmsg/mcmsg_appl.h>
#include <mcmsg/mcmsg_intf.h>
#include <mcmsg/mcmsg_xmsg.h>
#define MCMSG_LIB_EXT
#include <nx/mcmsg_lib.h>
#include <nx/assert.h>
#include <nx/debugxmsg.h>


#ifdef VERIFY_XMSG
#define xmsg_verify(xmsg) _xmsg_verify(xmsg)
#else
#define xmsg_verify(xmsg)
#endif

long		msginfo[8] = {-1,-1,-1,-1};
xmsg_t * _xmalloc();
xmsg_t * _xmalloc_largest();


extern end;
extern edata;
extern unsigned long mcmsg_free_bytes;

unsigned long data_lock_end;
unsigned long first_stack_locked;

#ifdef PREALLOCATE
xmsg_t mcmsg_preallocated_buffer[1048576/sizeof(xmsg_t) + 1];
#endif PREALLOCATE

_xinit(nodeinfo)
	nodeinfo_t	*nodeinfo;
{
	unsigned long	stacktop;
	unsigned long	buffer_size;
	unsigned long	provide_size;
	xmsg_t		*xm;
	unsigned long	t;
	int st;

	assert((sizeof(xmsg_t) & (sizeof(xmsg_t) - 1)) == 0); /* power of 2 */

	mcmsg_mynode = nodeinfo->mynode;
	mcmsg_numnodes = nodeinfo->numnodes;
	mcmsg_app = nodeinfo->applinfo.app;
	mcmsg_pid = getpid();
	mcmsg_pkt_size = nodeinfo->applinfo.pkt_size;
	mcmsg_memory_each = nodeinfo->applinfo.memory_each;
	mcmsg_send_threshold = nodeinfo->applinfo.send_threshold;
	mcmsg_send_count = nodeinfo->applinfo.send_count;
	mcmsg_give_threshold = nodeinfo->applinfo.give_threshold;
	mcmsg_process_lock = nodeinfo->applinfo.process_lock;
	buffer_size = nodeinfo->applinfo.memory_buffer;
	provide_size = nodeinfo->applinfo.memory_buffer;

	_xminit();
	if (_nx_lib_init() == -1) {
		return -1;
	}

	if (provide_size > buffer_size) {
		errno = EQPARAM;
		return -1;
	}
	mcmsg_buffer_size = (buffer_size + sizeof(xmsg_t) - 1) &
				~(sizeof(xmsg_t) - 1);
	mcmsg_provide_threshold = PROVIDE_THRESHOLD;

#if PREALLOCATE
	if (sizeof(mcmsg_preallocated_buffer) - sizeof(xmsg_t) <
	    mcmsg_buffer_size) {
		errno = EQPARAM;
		return -1;
	}
	mcmsg_buffer = (xmsg_t *)
		( ( ((unsigned long)mcmsg_preallocated_buffer) + 0x1F) & ~0x1F);
#else PREALLOCATE
	mcmsg_buffer = (xmsg_t *)malloc(mcmsg_buffer_size + sizeof(xmsg_t));
	mcmsg_buffer = (xmsg_t *)(((unsigned long)mcmsg_buffer + sizeof(xmsg_t)) &
	                          ~(sizeof(xmsg_t) - 1));
	if (mcmsg_buffer == 0) {
		return -1;
	}
#endif PREALLOCATE
	/* touch begin and end, then wire */
	t = mcmsg_buffer[0].size +
	    mcmsg_buffer[mcmsg_buffer_size/sizeof(xmsg_t) - 1].size;
	if ((st = mcmsg_wire(mcmsg_buffer, mcmsg_buffer_size)) == -1) {
		errno = EACCES;
		return -1;
	}
	if (st == -2) {
		errno = ENOMEM;
		return -1;
	}

	if (mcmsg_process_lock) {
		data_lock_end = (unsigned long)&end;
		if ((st = mcmsg_wire(&edata,
			       data_lock_end - (unsigned long)&edata))
					== -1) {
			errno = EACCES;
			return -1;
		}
		if (st == -2) {
			errno = ENOMEM;
			return -1;
		}
		first_stack_locked = (unsigned long)&stacktop;
		stacktop = first_stack_locked | 0x000fffff;
		if ((st = mcmsg_wire(first_stack_locked,
			       stacktop - first_stack_locked + 1)) == -1) {
			errno = EACCES;
			return -1;
		}
		if (st == -2) {
			errno = ENOMEM;
			return -1;
		}
	}


	mcmsg_buffer_end = (xmsg_t *)
			((unsigned long)mcmsg_buffer + mcmsg_buffer_size);

	for (xm = mcmsg_buffer; xm < mcmsg_buffer_end; t += (xm++)->size);

	mcmsg_buffer[0].chain_number = 0;
	mcmsg_buffer[0].state = XMSG_TRASH;
	mcmsg_buffer[0].size = buffer_size - sizeof(xmsg_t);
	mcmsg_buffer[0].length = t;
	mcmsg_buffer[0].link = 0;
	mcmsg_buffer[0].backlink = 0;
	mcmsg_buffer[0].prev_adjacent = 0;
	mcmsg_buffer[0].next_adjacent = 0;
	mcmsg_provide_bytes = 0;
	mcmsg_outgoing_tail = 0;
	_xfree(mcmsg_buffer);
	_xprovide();

	return 0;
}

/*
 * Calling graph:
 *
 * user -> xmalloc -+----------------------------+
 *                  +-------------------+        |
 *                                      |        |
 *                                      v        |
 * user -> xfree  --+--------------> _xfree      |
 *                  +                            v
 *                  +-> _xprovide ----------> _xmalloc
 */

_t_xfree(xm)
	xmsg_t	*xm;
{
	xmsg_verify(xm - 1);
	_xfree(xm - 1);
	_xprovide();
}

xmsg_t *
_t_xmalloc(size)
	unsigned long	size;
{
	xmsg_t	*xm;

	nx_spin_lock();
	_xgarbage();
	xm = _xmalloc((size + sizeof(xmsg_t) - 1) & ~(sizeof(xmsg_t) - 1));
	if (xm != 0) {
		xm->length = size;
		nx_spin_unlock();
		return xm + 1;
	}
	errno = EQMEM;
	nx_spin_unlock();
	return 0;
}

_t_xlength(xm)
	xmsg_t	*xm;
{

	return (xm - 1)->length;
}

_myhost()
{

	return mcmsg_numnodes;
}

_infotype()
{

	return	msginfo_type;
}

_infocount()
{

	return	msginfo_count;
}

_infonode()
{

	return	msginfo_node;
}

_infoptype()
{

	return	msginfo_ptype;
}

_infopid()
{

	return	msginfo_ptype;
}

_set_provide_threshold(provide_threshold)
unsigned long	provide_threshold;
{
	provide_threshold &= ~((2 * sizeof(xmsg_t)) - 1);
	if (provide_threshold > mcmsg_buffer_size) {
		provide_threshold = mcmsg_buffer_size;
	}

	if (provide_threshold < mcmsg_provide_threshold) {
		mcmsg_provide_threshold = provide_threshold;
		nx_spin_lock();
		_xprovide();
		nx_spin_unlock();
	} else {
		mcmsg_provide_threshold = provide_threshold;
	}
	return 0;
}

_xprovide()
{
	int	st;
	xmsg_t	*xm;
	int can_provide;

	assert(mcmsg_provide_bytes <= mcmsg_buffer_size);

	can_provide = mcmsg_buffer_size - mcmsg_provide_bytes;
	if (((int)can_provide) <
	    ((int)mcmsg_provide_threshold)) {
		return;
	}

	xm =  _xmalloc_largest();
	if (xm == 0) {
		return;
	}

	/*
	 * Init the buffer for kernel use.
	 */

	xm->link = 0;
	xm->backlink = 0;
	xmsg_verify(xm);

	/*
	 * Increment amount we believe is provided to the kernel.
	 */
	mcmsg_provide_bytes += xm->size + sizeof(xmsg_t);
	debugxmsg((xm->length = mcmsg_provide_bytes, xm), "provide");

	/*
	 * Make the provide kernel call.
	 */
	st = mcmsg_provide(xm);
	if (st < 0) {
		switch (st) {

		case -1:
			errno = EQNOACT;
			break;

		case -2:
			errno = EQPCCODE;
			break;

		case -3:
			errno = EQPCNODE;
			break;

		case -4:
			errno = EQLEN;
			break;
		}
		assert(st == 0);
	}

	assert(mcmsg_provide_bytes > 0);
	assert(mcmsg_provide_bytes <= mcmsg_buffer_size);
}

_xgarbage()
{
	xmsg_t	*xm;
	xmsg_t  *xxm;
	unsigned long	t1;

	xmsg_verify(mcmsg_outgoing_head);

	assert( (t1 = TIMEOUT) != 0 );
	while ( (xm = mcmsg_outgoing_head) != 0 &&
		xm->state == XMSG_TRASH ) {

		xxm = xm;
		while (xxm->chain_number != 0) {
			xxm = xxm->link;
		}
		mcmsg_outgoing_head = xxm->link;
		if (mcmsg_outgoing_head == 0) {
			mcmsg_outgoing_tail = 0;
		}
		_xfree(xm);
		assert(t1-- > 0);
	}
	xmsg_verify(mcmsg_outgoing_head);
}

_led(s)
	long	s;
{

	return;
}
