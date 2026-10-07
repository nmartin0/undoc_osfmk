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
 
/* @(#)svipc_ipc.c	2.1 16:10:13 4/20/90 SecureWare, Inc. */
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: svipc_ipc.c,v $
 * Revision 1.5  1994/11/18  20:27:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:49:19  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:49:38  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:05:41  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:20  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/12/16  13:05:00  roy
 * 	Initial revision.
 *
 * Revision 3.0  91/10/31  12:27:02  david
 * ported from 1.0.2
 * 
 * Revision 1.10  90/12/06  14:01:52  devrcs
 * 	Cleanup copyright and history log comments.
 * 	[90/11/15  11:05:06  gm]
 * 
 * Revision 1.9  90/10/07  13:19:07  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:59:29  gm]
 * 
 * Revision 1.8  90/08/24  11:18:23  devrcs
 * 	New system call interface
 * 	[90/08/17  17:38:57  nags]
 * 
 * Revision 1.7  90/08/09  13:14:46  devrcs
 * 	outdate previous revision.
 * 	[90/07/30  15:31:59  bet]
 * 
 * Revision 1.5  90/07/17  11:19:37  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:52:24  seiden]
 * 
 * Revision 1.4  90/06/22  20:06:55  devrcs
 * 	nags merge
 * 	[90/06/12  19:07:34  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:40:27  seiden]
 * 
 * Revision 1.3  90/04/27  18:52:39  devrcs
 * 	removed sysv_ipc conditional.
 * 	[90/04/18  15:59:48  bet]
 * 
 * Revision 1.2  90/02/23  00:21:58  devrcs
 * 	Removed #include <sys/dir.h>
 * 	[90/02/20  17:09:22  bet]
 * 
 * 	Initial version of common routines for System V IPC.
 * 	[90/02/16  14:40:58  bet]
 * 
 * $EndLog$
 */
/* @(#)ipc.c	6.2 */
#include <sys/secdefines.h>
#include <sys/ipc.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/signal.h>
#include <sys/proc.h>
#include <sys/user.h>
#if	SEC_BASE && !SEC_ARCH
#include <sys/security.h>
#endif	

/*
**	Common IPC routines.
*/

/*
**	Check message, semaphore, or shared memory access permissions.
**
**	This routine verifies the requested access permission for the current
**	process.  Super-user is always given permission.  Otherwise, the
**	appropriate bits are checked corresponding to owner, group, or
**	everyone.  Zero is returned on success.  On failure, u.u_error is
**	set to EACCES and one is returned.
**	The arguments must be set up as follows:
**		p - Pointer to permission structure to verify
**		mode - Desired access permissions
*/


#if	!SEC_ARCH
ipcaccess(p, mode)
register struct ipc_perm 	*p;
register u_short 		mode;
{
#if	!SEC_BASE
	if (u.u_uid == 0)
		return (0);
#endif

	if (u.u_uid != p->uid && u.u_uid != p->cuid) {
		mode >>= 3;
		if (u.u_gid != p->gid && u.u_gid != p->cgid)
			mode >>= 3;
	}

	if (mode & p->mode)
		return (0);

#if	SEC_BASE
	if (privileged(SEC_ALLOWDACACCESS, 0))
		return 0;
#endif

	return (1);
}
#endif	/* !SEC_ARCH */

/*
**	Get message, semaphore, or shared memory structure.
**
**	This routine searches for a matching key based on the given flags
**	and returns a pointer to the appropriate entry.  A structure is
**	allocated if the key doesn't exist and the flags call for it.
**	The arguments must be set up as follows:
**		key - Key to be used
**		flag - Creation flags and access modes
**		base - Base address of appropriate facility structure array
**		cnt - # of entries in facility structure array
**		size - sizeof(facility structure)
**		status - Pointer to status word: set on successful completion
**			only:	0 => existing entry found
**				1 => new entry created
**	Ipcget returns NULL with u.u_error set to an appropriate value if
**	it fails, or a pointer to the initialized entry if it succeeds.
*/

ipcget(key, flag, base, cnt, size, status, ret)
key_t           		key;
int             		flag;
register struct ipc_perm 	*base;
int             		cnt;
int 				size;
int				*status;
struct ipc_perm 		**ret;
{
	register struct ipc_perm	*a;	/* ptr to available entry */
	register int    		i;	/* loop control */

	*ret = NULL;
	if (key == IPC_PRIVATE) {
		for (i = 0; i++ < cnt;
		        base = (struct ipc_perm *) (((char *) base) + size)) {
			if (base->mode & IPC_ALLOC)
				continue;

			goto init;
		}

		return (ENOSPC);
	} 
	else {
		for (i = 0, a = NULL; i++ < cnt;
		     base = (struct ipc_perm *) (((char *) base) + size)) {

			if (base->mode & IPC_ALLOC) {

				/* this entry is allocated */
				if (base->key == key) {

					/* key matches */
					if ((flag & (IPC_CREAT | IPC_EXCL)) ==
					    (IPC_CREAT | IPC_EXCL))
						return (EEXIST);

					if ((flag & 0777) & ~base->mode)
						return (EACCES);

					*status = 0;
					*ret = base;
					return (0);
				}  
				continue;

			}

			/*
			 * entry not allocated, hold this space for a new entry 
			 */
			if (a == NULL)
				a = base;
		} /* for() */

		if (!(flag & IPC_CREAT))
			return (ENOENT);

		if (a == NULL)
			return (ENOSPC);

		base = a;
	}

init:
	*status = 1;

	base->mode = IPC_ALLOC | (flag & 0777);
	base->key = key;
	base->cuid = base->uid = u.u_uid;
	base->cgid = base->gid = u.u_gid;

	*ret = base;
	return (0);
}
