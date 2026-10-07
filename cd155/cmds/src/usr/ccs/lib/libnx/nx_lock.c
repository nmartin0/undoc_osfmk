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
 *                        INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *        Copyright 1993  Intel Corporation.
 *
 */


#include <stdio.h>
#include <errno.h>
#include <mcmsg/mcmsg_appl.h>
#include <mcmsg/mcmsg_intf.h>
#include <mcmsg/mcmsg_xmsg.h>
#include <nx/mcmsg_lib.h>
#include <mcmsg/mcmsg_nx.h>
#include <nx/assert.h>
#include <nx/debugxmsg.h>
#include "nx.h"
#include <sys/types.h>
#include <dbglib.h>
#include <sys/table.h>
#include <mach/port.h>
#include <mach/thread_switch.h>
#include <mach/mach_traps.h>


volatile int _nx_lock = 0;
volatile int _masktrap_lock = 0;

/*
 * nx_spin_lock - loosely stolen from mach cthread code.
 * If locked, depress thread's priority for up to 10ms.) -gjr
 */
typedef int nx_spin_lock_t;

/* initialize the nx lock semiphore */

nx_spin_lock_init()
{
        _nx_lock = 0;
}

/* wait for the nx lock semiphore to be free */

nx_spin_lock()
{
	while (!_nx_spin_lock(&_nx_lock)) {
		(void) thread_switch(MACH_PORT_NULL, SWITCH_OPTION_DEPRESS, 10);
	}
}

/* wait for the nx masktrap lock semiphore to be free */

masktrap_spin_lock()
{
	while (!_nx_spin_lock(&_masktrap_lock)) {
		(void) thread_switch(MACH_PORT_NULL, SWITCH_OPTION_DEPRESS, 10);
	}
}

/* free the nx lock semiphore */

nx_spin_unlock()
{
	_nx_spin_unlock(&_nx_lock);
}

/* free the nx masktrap lock semiphore */

masktrap_spin_unlock()
{
	_nx_spin_unlock(&_masktrap_lock);
}

/* low level routine to check and set semiphore 
   this should to be an atomic operation */
 
_nx_spin_lock(p)
int *p;
{
	asm("	nop");
	asm("	lock");
	asm("	ld.l 0(r16),r17");
	asm("	mov 1,r18");
	asm("	unlock");
	asm("	st.l r18,0(r16)");
	asm("	xor 1,r17,r16");
}

/* low level clear semiphore */

_nx_spin_unlock(p)
int *p;
{
	asm("	st.l r0,0(r16)");
}
