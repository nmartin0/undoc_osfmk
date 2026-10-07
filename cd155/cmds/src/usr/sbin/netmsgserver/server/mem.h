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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * mem.h 
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/mem.h,v 1.2 1994/11/19 03:11:26 mtm Exp $ 
 *
 */

/*
 * Definitions for the memory management module. 
 */


#ifndef	_MEM_
#define	_MEM_

#include <pthread.h>
#include "debug.h"
#include "ls_defs.h"
#include <mach/boolean.h>
#include <mach/error.h>
#include <mach_init.h>

/**** defined in mach/std_types.h
#ifdef OSF
typedef	char * pointer_t;
#endif
***/
#define	MEM_DEBUG_LO	debug.mem
#define	MEM_DEBUG_HI	(debug.mem & 0x2)


#if	GOOD_MALLOC
#else	GOOD_MALLOC
/*
 * Object bucket information.
 */
typedef struct mem_objbucket {
	int			num;		/* number of free objects */
	int		freehead;	/* next free object */
	struct mem_objbucket	*next;		/* bucket chain */
	struct mem_objbucket	*prev;		/* bucket chain */
	boolean_t		avail;		/* TRUE if on allocation chain */
	int		info;		/* back pointer to head */
	struct mem_objbucket	*list_next;	/* chain of all buckets */
	struct mem_objbucket	*list_prev;	/* chain of all buckets */
} mem_objbucket_t, *mem_objbucket_ptr_t;
#endif	GOOD_MALLOC

/*
 * Record for object allocation chain.
 */
typedef struct mem_objrec {
#if	GOOD_MALLOC
#else	GOOD_MALLOC
	pthread_mutex_t	lock;		/* lock for allocation */
#endif	GOOD_MALLOC
	char		*name;		/* name of zone */
	unsigned int	obj_size;	/* size of one object */
#if	GOOD_MALLOC
#else	GOOD_MALLOC
	boolean_t	aligned;	/* objects are page-aligned */
	int		reuse_num;	/* threshold for reuse of a bucket */
	int		full_num;	/* max number of objects per bucket */
	unsigned int	bucket_size;	/* size of one bucket */
	unsigned int	mask;		/* mask to locate bucket info */
	int		cur_buckets;	/* current number of buckets */
	int		max_buckets;	/* hwm for number of buckets */
	mem_objbucket_t	*first;		/* head of free buckets chain */
	mem_objbucket_t	*last;		/* tail of free buckets chain */
	mem_objbucket_t	*list_first;	/* chain of all buckets */
	mem_objbucket_t	*list_last;	/* chain of all buckets */
	struct mem_objrec	*mem_chain;	/* link in chain of objects */
#endif	GOOD_MALLOC
} mem_objrec_t, *mem_objrec_ptr_t;

/*
 * Macros for object allocation.
 */

extern mem_objrec_t	MEM_NNREC;

#if	GOOD_MALLOC
#define MEM_ALLOCOBJ(ret,type,ot) { ret = (type) malloc(ot.obj_size); }
#else	GOOD_MALLOC
#define	MEM_ALLOCOBJ(ret,type,ot) {						\
	int	_fh;							\
										\
	pthread_mutex_lock(&(ot).lock);							\
	if ((ot).first) {							\
		DEBUG5(MEM_DEBUG_HI,3,1110,&(ot),(ot).first,(ot).first->num,	\
					(ot).first->freehead,			\
					*(int *)(ot).first->freehead);	\
		(ot).first->num--;						\
		_fh = (ot).first->freehead;					\
		if (((ot).first->freehead = *(int *)_fh) == NULL) {	\
			(ot).first->avail = FALSE;				\
			(ot).first = (ot).first->next;				\
			if ((ot).first == NULL) {				\
				(ot).last = NULL;				\
				DEBUG0(MEM_DEBUG_HI,3,1111);			\
			} else {						\
				(ot).first->prev = NULL;			\
				DEBUG3(MEM_DEBUG_HI,3,1112,(ot).first,		\
					(ot).last,(ot).first->freehead);	\
			}							\
		}								\
	} else {								\
		_fh = mem_allocobj_proc(&(ot));					\
	}									\
	pthread_mutex_unlock(&(ot).lock);						\
	INCSTAT(mem_allocobjs);							\
	DEBUG3(MEM_DEBUG_LO, 3, 1113,_fh,(ot).first,(ot).cur_buckets);		\
	DEBUG_STRING(MEM_DEBUG_LO, 3, 1114, (ot).name);				\
	ret = (type) _fh;							\
}
#endif	GOOD_MALLOC


#if	GOOD_MALLOC
#define	MEM_DEALLOCOBJ(ptr,ot) free(ptr)
#else	GOOD_MALLOC
#define	MEM_DEALLOCOBJ(ptr,ot) {						\
	mem_objbucket_ptr_t	_ob;						\
										\
	pthread_mutex_lock(&(ot).lock);							\
	_ob = (mem_objbucket_ptr_t) (((unsigned int) (ptr)) & (ot).mask);	\
	DEBUG6(MEM_DEBUG_HI, 3, 1115, ptr, _ob, _ob->num, (ot).cur_buckets,	\
						(ot).first,_ob->freehead);	\
	if (_ob->info != (int)&ot)					\
		panic("MEM_DEALLOCOBJ: wrong object");				\
	*(int *) (ptr) = _ob->freehead;					\
	_ob->freehead = (int) (ptr);					\
	_ob->num++;								\
	if (									\
				((_ob->num == (ot).reuse_num)			\
			&&							\
				!(_ob->avail))					\
		||								\
				((_ob->num == (ot).full_num)			\
			&&							\
				(_ob != (ot).first))				\
		) {								\
		mem_deallocobj_proc(&(ot),_ob);					\
	}									\
	pthread_mutex_unlock(&(ot).lock);						\
	INCSTAT(mem_deallocobjs);						\
	DEBUG5(MEM_DEBUG_LO,3,1116,ptr,_ob,_ob->num,(ot).first,_ob->freehead);	\
	DEBUG_STRING(MEM_DEBUG_LO, 3, 1114, (ot).name);				\
}
#endif	GOOD_MALLOC

#define	MEM_ALLOC(ptr,type,_size,aligned) {					\
	if (_size <= 0) {							\
		ERROR((msg,"MEM_ALLOC: illegal size: %d", _size));		\
		panic("MEM_ALLOC with negative size");				\
	}									\
										\
	ptr = (type) malloc(_size);					\
	if (ptr == NULL) {						\
		ERROR((msg,"MEM_ALLOC.malloc returned NULL, size=%d",	\
			_size));						\
		panic("MEM_ALLOC.malloc");				\
	}								\
	INCSTAT(mem_allocs);							\
	DEBUG2(MEM_DEBUG_LO, 3, 1117, _size, (int)ptr);			\
}


#define	MEM_DEALLOC(ptr,_size) {							\
	DEBUG2(MEM_DEBUG_HI, 3, 1118, _size, (int)ptr);			\
										\
	if (_size < 0) {							\
		ERROR((msg,"MEM_ALLOC: illegal size: %d, ptr=0x%x",	\
							_size,ptr));	\
		panic("MEM_DEALLOC with negative size");		\
	}								\
	if (_size > 0) {							\
		free((char *)ptr);					\
	}								\
	INCSTAT(mem_deallocs);							\
	DEBUG2(MEM_DEBUG_LO, 3, 1118, _size, (int)ptr);			\
}


/*
 * To use the object allocator for objects of type foo_t:
 *
 * Declarations:
 *
 *	extern mem_objrec_t	MEM_FOO;
 *
 * Initialization:
 *
 *	PUBLIC mem_objrec_t	MEM_FOO;
 *	mem_initobj(&MEM_FOO,"foo",sizeof(foo_t),
 *			aligned,full_num,reuse_num);
 *
 */


/*
 * External procedures.
 */
extern boolean_t mem_init();
/*
*/

extern int mem_clean();
/*
*/

extern void mem_initobj();
/*
mem_objrec_ptr_t	or;
char			*name;
unsigned int		obj_size;
boolean_t		aligned;
int			full_num;
int			reuse_num;
*/

extern char * mem_allocobj_proc();
/*
mem_objrec_ptr_t	or;
*/

extern void mem_deallocobj_proc();
/*
mem_objrec_ptr_t	or;
mem_objbucket_ptr_t	ob;
*/

#endif	_MEM_
