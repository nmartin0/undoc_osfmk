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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: sem.h,v $
 * Revision 1.5  1994/11/18  20:41:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/09/10  23:21:20  jlitvin
 * Add missing semun union to header file.  It's mentioned in the man
 * page for semctl, but never defined.  This fixes PTS #5280.
 *
 * Revision 1.3  1993/07/14  18:27:13  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:36:26  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:36  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:16  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  93/06/02  09:55:22  yazz
 * For Sys V IPC under TNC change the pid field from short to pid_t.
 * 
 * Revision 2.2  91/08/31  14:11:21  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:44:51  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:53:48  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:44:36  gm]
 * 
 * Revision 1.6  90/08/09  13:29:00  devrcs
 * 	Removed  extern u_int sem_mark;
 * 	[90/07/31  10:48:14  bet]
 * 
 * Revision 1.5  90/07/27  09:07:44  devrcs
 * 	Added external declarations.
 * 	Define kernel structures for _KERNEL only.
 * 	[90/07/23  12:50:15  bet]
 * 
 * Revision 1.4  90/06/22  20:53:54  devrcs
 * 	Changed type of sempid to short, as per SVID.
 * 	[90/06/06  13:05:17  ers]
 * 
 * Revision 1.3  90/04/27  19:26:59  devrcs
 * 	Integrated with AIX header.
 * 	[90/04/20  10:52:57  bet]
 * 
 * Revision 1.2  90/02/23  00:33:04  devrcs
 * 	Initial version of System V IPC semaphore header file.
 * 	[90/02/20  11:25:04  bet]
 * 
 * $EndLog$
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)sem.h	1.13  com/inc/sys,3.1,8943 9/7/89 08:17:11 */
#ifndef _SYS_SEM_H_
#define _SYS_SEM_H_

#include <standards.h>
#include <sys/ipc.h>

#ifdef _XOPEN_SOURCE


/*
 *	Semaphore Operation Flags.
 */

#define	SEM_UNDO	010000	/* set up adjust on exit entry */

/*
 *	Semctl Command Definitions.
 */

#define	GETNCNT	3	/* get semncnt */
#define	GETPID	4	/* get sempid */
#define	GETVAL	5	/* get semval */
#define	GETALL	6	/* get all semval's */
#define	GETZCNT	7	/* get semzcnt */
#define	SETVAL	8	/* set semval */
#define	SETALL	9	/* set all semval's */

/*
 *	There is one semaphore id data structure for each set of semaphores
 *		in the system.
 */

struct semid_ds {
	struct ipc_perm	sem_perm;	/* operation permission struct	*/
	struct sem	*sem_base;	/* ptr to first semaphore in set*/
	u_short		sem_nsems;	/* # of semaphores in set	*/
	time_t		sem_otime;	/* last semop time		*/
	time_t		sem_ctime;	/* last change time		*/
};

/*
 *	There is one semaphore structure for each semaphore in the system.
 */

struct sem {
	u_short		semval;		/* semaphore text map address	*/
	pid_t		sempid;		/* pid of last operation	*/
	u_short		semncnt;	/* # awaiting semval > cval	*/
	u_short		semzcnt;	/* # awaiting semval = 0	*/
};

/*
 *	User semaphore template for semop system calls.
 */

struct sembuf {
	u_short		sem_num;	/* semaphore #			*/
	short		sem_op;		/* semaphore operation		*/
	short		sem_flg;	/* operation flags		*/
};

/*
 *	User semaphore template for semctl system calls.
 */

union semun {
	int		val;		/* semaphore value		*/
	struct semid_ds	*buf;		/* *structure of semid_ds	*/
	u_short		*array;		/* *array of semval values	*/
};

#ifndef _KERNEL
#ifdef _NO_PROTO
extern int semctl();
extern int semget();
extern int semop();
#else
extern int semget(key_t, int, int);
extern int semop(int, struct sembuf *, u_int);
extern int semctl(int, int, int, ...);
#endif /* _NO_PROTO */
#endif /* _KERNEL   */

#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE
/*
 * 	Implementation Constants. 
 */

#define	PSEMN	(PZERO + 3)	/* sleep priority waiting for greater value */
#define	PSEMZ	(PZERO + 2)	/* sleep priority waiting for zero */

/*
 * 	Permission Definitions. 
 */
#define	SEM_A	IPC_W	/* alter permission */
#define	SEM_R	IPC_R	/* read permission */

/*
 * 	There is one undo structure per process in the system. 
 *      each sem_undo struct has: 
 *       - a pointer to the next active undo struct,       
 * 	 - the number of active entries, 
 *       - and an array of undo struct entries. 
 */

#ifdef _KERNEL
struct sem_undo {
	struct sem_undo 	*un_nextp;	/* ptr to next active undo structure */
	short           	un_cnt;		/* # of active entries */
	struct undo_entry 	*un_entp;	/* ptr to list of undo entries */
};

struct undo_entry {
	struct undo_entry	*uent_nextp;	/* pointer to next undo structure */
	short           	uent_aoe;	/* adjust on exit values */
	short           	uent_num;	/* semaphore # */
	int             	uent_id;	/* semid */
};

/*
 *	Semaphore information structure. 
 */
struct seminfo {
	int	semmni,		/* # of semaphore identifiers */
		semmsl,		/* max # of semaphores per id */
		semopm,		/* max # of operations per semop call */
		semume,		/* max # of undo entries per process */
		semvmx,		/* semaphore maximum value */
		semaem;		/* adjust on exit max value */
};

#endif /* _KERNEL */

extern struct semid_ds sema[];
extern struct seminfo seminfo;

#endif /* _OSF_SOURCE */

#endif /* _SYS_SEM_H_ */



