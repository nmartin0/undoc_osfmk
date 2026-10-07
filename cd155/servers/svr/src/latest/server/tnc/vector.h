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
 
#ifndef _TNC_VECTOR_H
#define _TNC_VECTOR_H

/*
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: vector.h,v $
 * Revision 1.6  1995/02/01  22:15:16  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:45:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:36:59  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:51:01  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:29:52  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:48  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:49:50  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:48:03  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.0  1992/06/11  16:05:20  mjl
 * Definitions for expandable vector package.
 *
 */

/*
 *  Extensible vector package
 *
 *	These vector operations are intended to be used via use-specific
 *	macroes, e.g.
 *
 *	    #define FOO_VECTOR_ALLOC(n)	vec_alloc(sizeof(struct foo), (n))
 *	    #define FOO_VECTOR_NEXT(v,s) ((struct foo *)vec_next((v),(s)))
 *	    #define FOO_VECTOR_HEADER(v) ((struct foo *)VECTOR_FIRST(v))
 *
 *	etc.
 */

#include <sys/types.h>

typedef struct vec_seg {
	struct vec_seg *vsg_next;
	union {
		char	vu_data[1];	/* start of data */
		caddr_t	vu_dataptr;	/* likewise, for vec_cast() segment */
	} vsg_u;
} vec_seg_t;
#define	vsg_data	vsg_u.vu_data
#define vsg_ptr		vsg_u.vu_dataptr

typedef struct vector {
	ulong_t		v_eltsize;	/* size in bytes of vector element */
	ulong_t		v_seglen;	/* max elements per vector segment */
	int		v_flags;	/* see below */
	vec_seg_t	*v_firstseg;	/* address of first segment */
	vec_seg_t	*v_curseg;	/* current segment */
	ulong_t		v_eltcnt;	/* total number of valid elements */
	int		v_segidx;	/* index of next elt within segment */
	int		v_vecidx;	/* index of next elt within vector */
} vector_t;

#define	V_WASCAST	0x01	/* vector created via vec_cast() */

/*
 *  Operations
 */

/* Number of elements */
#define VECTOR_LEN(v)	((v)->v_eltcnt)

/* Pointer to first element, often used as a header */
#define VECTOR_FIRST(v)		( (v)->v_flags & V_WASCAST	\
				 ? (v)->v_firstseg->vsg_ptr	\
				 : &(v)->v_firstseg->vsg_data[0] )

/* Vectors consisting of a single segment often don't have to be gathered */
#define VECTOR_IS_SIMPLE(v) \
	( (v)->v_firstseg && ! (v)->v_firstseg->vsg_next )

/* Lookahead, returns pointer to next element without altering vector state */
#define VECTOR_LOOKAHEAD(v) vec_lookahead((v), NULL, NULL)

/* Compute the size of the buffer needed for vec_gather() operations */
#define VECTOR_GATHER_SIZE(v) ((v)->v_eltsize * (v)->v_eltcnt)

/* Allocate vector and first segment */
vector_t *	vec_alloc(int elts, int eltsize);

/* Deallocate vector and all segments */
int		vec_dealloc(vector_t *);

/* Reset internal iterator to point "before" first element */
int		vec_reset(vector_t *);

/*
 *  Return pointer to next element.  If there is no next element
 *  and scan is TRUE then return NULL, else allocate a new segment
 *  and return pointer to its first element.
 */
caddr_t		vec_next(vector_t *, int scan);

/*
 *  Copy all vector segments to contiguous memory starting at `to'.
 *  Some header information, such as element size and count, is copied
 *  prior to copying the actual vector element data.
 *
 *  The total number of bytes copied is returned.
 */
int	vec_gather(vector_t *v, caddr_t to);

/*
 *  Transform a data buffer into a vector.  Assumes that the buffer
 *  begins with the header info created by vec_gather().
 */
vector_t *	vec_cast(caddr_t buf, int count, int size);

#endif	/* ! _TNC_VECTOR_H */
