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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/macros.h,v 1.4 1994/11/19 03:04:18 mtm Exp $
 *
 */

/* History:
 *	$Log: macros.h,v $
 * Revision 1.4  1994/11/19  03:04:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/06/13  22:47:20  sdh
 * Changed debug messages to go through debug print routine.
 *
 *  Reviewer: mag
 *  Risk: low
 *  Benefit or PTS #:
 *  Testing: EATS
 *  Module(s):
 *         cmds_libs/src/usr/sbin/allocator/tiles.c
 *         cmds_libs/src/usr/sbin/allocator/allocator.c
 *         cmds_libs/src/usr/sbin/allocator/allocutils.c
 *         cmds_libs/src/usr/sbin/allocator/conflict.c
 *         cmds_libs/src/usr/sbin/allocator/init_appl.c
 *         cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
 *         cmds_libs/src/usr/sbin/allocator/mkpart_rpc.c
 *         cmds_libs/src/usr/sbin/allocator/rmpart_rpc.c
 *         cmds_libs/src/usr/sbin/allocator/schedule.c
 *         cmds_libs/src/usr/sbin/allocator/server_loop.c
 *         cmds_libs/src/usr/sbin/allocator/smd.c
 *         cmds_libs/src/usr/sbin/allocator/tiles.c
 *         cmds_libs/src/usr/sbin/allocator/macros.h
 *
 * Revision 1.2  1993/11/17  02:54:16  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.1  1993/05/26  00:28:45  carbajal
 * New prototype files
 *
*/

/*
 * Macros to lookup, insert, and remove partitions and applications from their
 * respective hash tables.
 */
#define HASH_LOOKUP_PART(k)	\
		((PART_T *) hash_tbl_lookup(part_tbl, (unsigned long)k))
#define HASH_LOOKUP_APPL(k)	\
		((APPL_T *) hash_tbl_lookup(appl_tbl, (unsigned long)k))
#define HASH_INSERT_APPL(k, p) \
		hash_tbl_insert(appl_tbl, (unsigned long) k, (void *) p)
#define HASH_INSERT_PART(k, p) \
		hash_tbl_insert(part_tbl, (unsigned long) k, (void *) p)
#define HASH_REMOVE_APPL(k)	hash_tbl_delete(appl_tbl, (unsigned long) k)
#define HASH_REMOVE_PART(k)	hash_tbl_delete(part_tbl, (unsigned long) k)

/*
 * Macros to allocate application, layer, and partition structures.
 */
#define	ALLOC_APPL()	((APPL_T *) MALLOC(sizeof(APPL_T)))
#define	ALLOC_LAYER()	((LAYER_T *) MALLOC(sizeof(LAYER_T)))
#define	ALLOC_PART()	((PART_T *) MALLOC(sizeof(PART_T)))

/*
 * Macro to insert layer in sorted list of layers.
 */
#define INSERT_LAYER(hd, lyr)                                           \
                {                                                       \
                        LAYER_T *p, *p_last;                            \
                                                                        \
                        p_last = (LAYER_T *) 0;                         \
                        p = (hd);                                       \
                        while ((p != (LAYER_T *) 0) &&                  \
                                (p->num_free < (lyr)->num_free))        \
                        {                                               \
                                p_last = p;                             \
                                p = p->next;                            \
                        }                                               \
                        if (p_last == (LAYER_T *) 0) {                  \
                                INSERT_FIRST((hd), (lyr));              \
                        }                                               \
                        else {                                          \
                                INSERT_AFTER((lyr), p_last);            \
                        }                                               \
                }

/* Macros to validate the active layer. Validation involves making sure the
 * active layer appears in the list of scheduling layers. If it does not
 * the active layer will be set to null
*/
#define VALIDATE_ACTIVE_LAYER(part)								\
	{											\
		LAYER_T	*l;									\
		int	valid;									\
		if ((part)->active_lyr != (LAYER_T *)0){					\
			valid = 0;								\
			debug_sched(5,"VALIDATE_ACTIVE_LAYER on inode %d\n",(part)->inode);	\
			for(l = (part)->child_sched_lyr; l != (LAYER_T *)0; l = l->next) 	\
				if (l == (part)->active_lyr){					\
					valid = 1;						\
					break;							\
				}								\
			if (!valid)								\
				(part)->active_lyr = (LAYER_T *)0;				\
		}										\
	}						

#ifndef MAX
#define MAX(a,b)        (((a)>(b))?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b)        (((a)<(b))?(a):(b))
#endif

/*
 * Convert dclock() to milliseconds.
 */
double  dclock();
#define GET_TIME_IN_MILLISECONDS()      ((dclock() * 1000.0) + 0.5)
 
/*
 * Macro to increment time rolled in without overflow.
 */
#define INCR_TIME(interval, time)       (time = MAX(time, time + interval))
 
#define LOCKED_TOO_LONG         3*60000 /* 360 seconds in ms */
 
typedef struct alloc_lock_type {
        short           locked;
        unsigned long   last;
} locktype;
 
#define INIT_LOCK(l)    {l.locked=FALSE;l.last=0;}
#define SET_LOCK(l)     {l.locked=TRUE;l.last=GET_TIME_IN_MILLISECONDS();}

