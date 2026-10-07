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
 * $Log: spanning_tree.c,v $
 * Revision 1.4  1994/11/18  20:44:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/06/02  22:29:59  chrisp
 * In dpvpop_reap(), perform PVPOP_RMV_PGRP_LIST() for zombie child
 * only if child's pgrp leader is not its parent; return child pgid.
 * In dvpop_wait(), analyze pgid returned for zombie child and call
 * PVPOP_RMV_PGRP_LIST() if this is the parent pid.
 *
 * Support added for waitmulti() - viz: elder reporting and reap multi
 * operation (refer to rtask_cli_vproc.c).
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): dpvproc.h dvp_pvpops.c dvp_vpops.c pvp.ops pvps.ops rtask.h
 * 	    rtask_cli_vproc.c rtask_server.c rtask_svr_vproc.c
 * 	    spanning_tree.c tnc_async.defs tnc_server_side.c
 * 	    tnc_types.defs tnc_types.h tnc_types_gen.c
 *
 * Revision 1.2  1993/10/27  15:30:58  cfj
 * Add forward declaration for minimum().
 *
 * Revision 1.1  1993/10/21  23:35:13  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 3.0  93/09/16  08:15:58  chrisp
 * Routines provided by Intel to implement a spanning tree algorithm.
 * 
 * 
 */
#include <sys/types.h>

#define	minimum(a,b) ( (a>b) ? b : a )
#define	maximum(a,b) ( (a>b) ? a : b )
  
static unsigned int mask[16] = { 0x0001, 0x0002, 0x0004, 0x0008,
                                 0x0010, 0x0020, 0x0040, 0x0080,
                                 0x0100, 0x0200, 0x0400, 0x0800,
                                 0x1000, 0x2000, 0x4000, 0x8000 };

int
ginv(  g )
unsigned g;
{
  int i, n;

  n = mask[15] & g;
  for (i=14; i>=0; --i)
    n += (mask[i] & g) ^ ((mask[i+1] & n)>>1);
  return(n);
}

int
gray( n )
unsigned n;
{
  int temp;

  temp = n<<1;
  return((temp^n)>>1);

}

void
tnc_spanning_tree_info(nnodes, me, rbase, size)
long  nnodes;
long  me;
long  *rbase;
long  *size;
{
	int   i;
	long  pow;
	long  half;
	long  me_in_uphalf;
	long  root_in_uphalf;
	long  root = 0;
	long  base = 0;

	while (me != root) {

		for (i = 1, pow = 0; i < nnodes; i <<= 1, pow++) 
		    ;

		half = 1 << (pow - 1);
		me_in_uphalf = (me >= half);
		root_in_uphalf = (root >= half);

		if (me_in_uphalf && root_in_uphalf) {
		     base += half;
		     root -= half;
		     me -= half;
		     nnodes -= half;
		} else if (me_in_uphalf) {
		     base += half;
		     root = minimum(ginv(gray(root) | half), nnodes - 1) - half;
		     me -= half;
		     nnodes -= half;
		} else if (root_in_uphalf) {
		     root = ginv(gray(root) & ~half);
		     nnodes = half;
		} else {
		     nnodes = half;
		}
	}
	*size = nnodes;
	*rbase = base;
	return;
}

int
get_spanning_tree( nnodes, me, down, ndown )
long  nnodes;
long  me;
long *down;
long *ndown;
{
    long base = 0;
    long root = 0;
    long up = -1;

    return( _getNeighborNodes( nnodes, base, root, me, &up, down, ndown ) );
}

int
_getNeighborNodes( nnodes, base, root, me, up, down, ndown )
long  nnodes;
long  base;
long  root;
long  me;
long *up;
long *down;
long *ndown;
{
    int   i;
    long  pow;
    long  half;
    long  me_in_uphalf;
    long  root_in_uphalf;

    if ( me == root ) {
        return( _getDownstreamNodes( nnodes, root, base, down, ndown ) );
    }

    for ( i = 1, pow = 0; i < nnodes; i <<= 1, pow++ ) 
        ;

    half = 1 << (pow - 1);
    me_in_uphalf = ( me >= half );
    root_in_uphalf = ( root >= half );

    *up = root + base;

    if ( me_in_uphalf && root_in_uphalf ) {
         base += half;
         root -= half;
         me -= half;
         nnodes -= half;
    } else if ( me_in_uphalf ) {
         base += half;
         root = minimum( ginv( gray( root ) | half ), nnodes - 1 ) - half;
         me -= half;
         nnodes -= half;
    } else if ( root_in_uphalf ) {
         root = ginv( gray( root ) & ~half );
         nnodes = half;
    } else {
         nnodes = half;
    }

    return( _getNeighborNodes( nnodes, base, root, me, up, down, ndown ) );
}


int 
_getDownstreamNodes( nnodes, root, base, down, ndown )
long   nnodes;
long   root;
long   base;
long  *down;
long  *ndown;
{
    long  i = 0;
    long  bit;
    long  groot;
    long  node;
     
    if ( nnodes <= 1 ) {
	*ndown = 0;
        return 0;
    }

    for ( bit = 1, i = 0; bit < nnodes; bit <<= 1, i++ ) 
        ;

    if (i > *ndown)
	return(-1);

    groot = gray( root );
    for ( i = 0, bit >>= 1; bit > 0; bit >>= 1 ) {
        node = minimum( ginv( groot ^ bit ), nnodes - 1 );
        if ( node != root ) {
            down[i++] = node + base;
        }
    }

    *ndown = i;

    return 0;
}
