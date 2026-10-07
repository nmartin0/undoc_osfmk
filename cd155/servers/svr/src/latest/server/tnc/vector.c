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
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: vector.c,v $
 * Revision 1.8  1995/02/01  22:14:48  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.7  1994/11/18  20:45:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  18:36:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:50:58  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:29:42  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:46  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:10:21  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:07:17  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.3  1993/01/15  02:03:44  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 3.3  1992/12/10  17:33:01  mjl
 * Use new debug macros.
 *
 * Revision 3.2  92/08/08  01:49:48  jdh
 * fixed bug -- missing initialization (mjl fixed) -- jdh
 * 
 * Revision 3.1  92/06/26  18:02:52  mjl
 * Fix NULL pointer problem in vec_dealloc().
 * 
 * Revision 3.0  92/06/11  16:05:05  mjl
 * Expandable vector package.
 * 
 */

/*
 *  Extensible vector package
 */

#include <sys/types.h>
#include <net/net_globals.h>
#include <net/net_malloc.h>
#include <tnc/un_debug.h>
#include <tnc/vector.h>

#define IS_CAST_SEGMENT(v,s) \
	( (v)->v_firstseg == (s) && (v)->v_flags & V_WASCAST )


/*
 *  Allocate vector and first segment.  If the number of elements
 *  per segment is unspecified, pick a reasonable value based on
 *  the element size and the size of the memory area that the
 *  underlying memory allocator prefers.
 */
vector_t *
vec_alloc(
	int elts,
	int eltsize)
{
	vector_t *vec;
	vec_seg_t *seg;

	/*
	 *  If elements-per-segment was not specified, pick a number
	 *  based on a heap allocation of 256 bytes.
	 */
	if ( elts == 0 ) {
	    elts = (256 - (sizeof(vector_t) + sizeof(vec_seg_t *))) / eltsize;
	    UNDEBUG(U_VECTOR, ("vec_alloc: chose %d %d-byte elts per seg\n",
			       elts, eltsize));
	    if ( elts == 0 )
		elts = 8;
	}

	NET_MALLOC(vec,
		   vector_t *,
		   sizeof(vector_t) + (elts * eltsize) + sizeof(vec_seg_t *),
		   M_TEMP,
		   M_WAITOK);

	seg = (vec_seg_t *)((caddr_t)vec + sizeof(vector_t));
	seg->vsg_next = NULL;

	vec->v_eltsize	= eltsize;
	vec->v_seglen	= elts;
	vec->v_firstseg	= seg;
	vec->v_eltcnt	= 0;
	vec->v_flags	= 0;
	vec->v_curseg	= seg;
	vec->v_segidx	= 0;
	vec->v_vecidx	= 0;

	return vec;
}

/*
 *  Deallocate vector and all segments.
 *
 *  Because the vector's first segment header is always allocated in
 *  the same memory allocator call as the vector_t header itself, we
 *  can start freeing the segments starting at the second segment, not
 *  the first.  A side effect of this is that we don't have to treat
 *  V_WASCAST vectors as a special case.
 */
int
vec_dealloc(vector_t *v)
{
	register vec_seg_t *free, *the_rest;

	ASSERT(v && v->v_firstseg);

	free = v->v_firstseg->vsg_next;
	if ( free )
		the_rest = free->vsg_next;

	while ( free ) {
		NET_FREE(free, M_TEMP);
		if (free = the_rest)
			the_rest = free->vsg_next;
	}

	NET_FREE(v, M_TEMP);	
	return 0;
}


/*
 *  Given an array of elements in a buffer, return a vector_t pointer
 *  that allows the array to be accessed with these routines.
 *
 *  NB deallocating the resulting vector_t does not deallocate the
 *  original buffer from which it was cast.  Also, if the vector is
 *  extended then each new segment will have `count' elements as
 *  established here.
 */
vector_t *
vec_cast(
	caddr_t	buf,		/* start of buffer to cast */
	int	count,		/* number of vector elements in buffer */
	int	size)		/* size of a vector element */
{
	vector_t *vec;
	vec_seg_t *seg;

	NET_MALLOC(vec,
		   vector_t *,
		   sizeof(vector_t) + sizeof(vec_seg_t),
		   M_TEMP,
		   M_WAITOK);

	seg = (vec_seg_t *)((caddr_t)vec + sizeof(vector_t));
	seg->vsg_next = NULL;

	vec->v_eltsize	= size;
	vec->v_seglen	= count;
	vec->v_eltcnt	= count;
	vec->v_firstseg	= seg;
	vec->v_curseg	= seg;
	vec->v_flags	= V_WASCAST;
	seg->vsg_ptr	= buf;
	vec->v_segidx	= 0;
	vec->v_vecidx	= 0;

	return vec;
}


/*
 *  Copy all the vector's segments to a contiguous buffer.
 *  Returns the total number of bytes copied.
 *  The vector itself is not modified.
 *  No vector length information is copied to the buffer.
 */
int
vec_gather(
	vector_t *v,
	caddr_t  to)
{
	vec_seg_t	*seg;
	caddr_t		saved_to = to;
	int		n, len, howmany = v->v_eltcnt;

	/*
	 *  To really have a complete set of vector_t operations,
	 *  we ought to allow for V_WASCAST vectors here---but
	 *  in practice we'll never call vec_gather() on such a
	 *  vector.
	 */
	ASSERT((v->v_flags & V_WASCAST) == 0);

	for ( seg = v->v_firstseg; seg; seg = seg->vsg_next ) {
		n = ( howmany > v->v_seglen ? v->v_seglen : howmany );
		howmany -= n;
		len = n * v->v_eltsize;
		bcopy(&seg->vsg_data[0], to, len);
		to += len;
	}
	return (int)(to - saved_to);
}


/*
 *  Reset internal iterator to point "before" first element
 */
int
vec_reset(vector_t *v)
{
	v->v_curseg = v->v_firstseg;
	v->v_segidx = 0;
	v->v_vecidx = 0;
}


#define ALLOC	0
#define	SCAN	1
#define PEEK	2

static caddr_t
vec_common(
	vector_t *v,
	int	 opcode)
{
	int	si = v->v_segidx;
	int	vi = v->v_vecidx;
	int	ec = v->v_eltcnt;
	vec_seg_t *seg = v->v_curseg;
	int	segsize;
	caddr_t	rv, segbase;

	ASSERT(opcode == ALLOC || opcode == SCAN || opcode == PEEK);
	ASSERT(!(opcode == ALLOC && IS_CAST_SEGMENT(v,seg)));

	/*
	 *  Case 1:  Positioned within a segment.
	 */
	if ( si < v->v_seglen ) {
		/* Segbase[si] is the element in question. */
		segbase = (IS_CAST_SEGMENT(v,seg)
			   ? seg->vsg_ptr
			   : &seg->vsg_data[0]);
		switch ( opcode ) {
		case ALLOC:
			rv = segbase + (si * v->v_eltsize);
			v->v_segidx++;
			v->v_eltcnt++;
			break;
		case PEEK:
			rv = (vi < ec) ? segbase + (si * v->v_eltsize) : NULL;
			break;
		case SCAN:
			rv = (vi < ec) ? segbase + (si * v->v_eltsize) : NULL;
			v->v_segidx++;
			v->v_vecidx++;
			break;
		}
		return rv;
	}

	/*
	 *  Case 2:  At end-of-segment, but not end-of-vector
	 */
	if ( seg->vsg_next ) {
		ASSERT(opcode != ALLOC);
		seg = seg->vsg_next;
		rv = &seg->vsg_data[0];
		if ( opcode == SCAN ) {
			v->v_segidx = 1;	/* index of *next* element */
			v->v_vecidx++;
			v->v_curseg = seg;
		}
		return rv;
	}

	/*
	 *  Case 3:  At end-of-vector
	 */
	if ( opcode == SCAN || opcode == PEEK )
		return NULL;

	/* ...else allocate a new segment. */
	segsize = (v->v_seglen * v->v_eltsize) + sizeof(vec_seg_t *);
	NET_MALLOC(seg->vsg_next, vec_seg_t *, segsize, M_TEMP, M_WAITOK);
	v->v_curseg = seg->vsg_next;
	v->v_curseg->vsg_next = NULL;

	v->v_segidx = 1;	/* index of *next* element */
	v->v_eltcnt++;

	return &v->v_curseg->vsg_data[0];
}


/*ARGSUSED*/
caddr_t
vec_lookahead(
	vector_t	*v,		/* IN  - vector to examine */
	int		*idxp,		/* OUT - index of next element */
	vec_seg_t	**segp)		/* OUT - segment address of next elt */
{
	ASSERT(idxp == NULL && segp == NULL);	/* prevent obsolete usage */
	return vec_common(v, PEEK);
}

caddr_t
vec_next(
	vector_t *v,
	int	 scan)
{
	return vec_common(v, (scan ? SCAN : ALLOC ));
}
