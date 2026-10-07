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
 * HISTORY
 * $Log: slock2.c,v $
 * Revision 1.7  1994/11/18  20:32:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  18:00:00  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.5  1993/07/08  22:02:22  nandy
 * Corrected the last change.
 *
 * Revision 1.4  1993/07/08  17:49:23  nandy
 * simple_lock() now calls _simple_lock_solid() instead of _simple_lock() to give
 * the cthreads a chance to get scheduled.
 *
 * Revision 1.1.1.3  1993/07/01  19:18:55  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:10:39  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:09  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:22:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:24  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1992/05/12  16:18:02  loverso
 * 	Renamed from slock.c
 *
 * Revision 2.2  92/03/15  14:49:10  roy
 * 	Initial revision (cfg@intel).
 * 
 * $EndLog$
 */

#include <kern/lock.h>

#define SIMPLE_LOCK_DEBUG(l) \
        l->slthread = (char *) current_thread(); \
        inc_slock(l);

#define SIMPLE_UNLOCK_DEBUG(l) \
        l->slthread = (char *) -1; \
        dec_slock(l);

void simple_lock_init(l)
	simple_lock_t	l;
{
#if	MACH_LDEBUG
	l->slthread = (char *) -1;
        l->slck_addr = (int) -1;
        l->sunlck_addr = (int) -1;
#endif
        _simple_lock_init(&l->lock_data);
}

void simple_lock(l)
	simple_lock_t l;
{
	if (!spin_try_lock(&l->lock_data))
		_simple_lock_solid(&l->lock_data);
#if	MACH_LDEBUG
        SIMPLE_LOCK_DEBUG(l);
#endif
}

boolean_t simple_lock_try(l)
	simple_lock_t l;
{
#if	MACH_LDEBUG
        register int state = _simple_lock_try(&l->lock_data);
        if (state)
                SIMPLE_LOCK_DEBUG(l);
        return(state);
#else
        return(_simple_lock_try(&l->lock_data));
#endif
}

void simple_unlock(l)
	simple_lock_t l;
{
#if	MACH_LDEBUG
        SIMPLE_UNLOCK_DEBUG(l);
#endif
        _simple_unlock(&l->lock_data);
}




