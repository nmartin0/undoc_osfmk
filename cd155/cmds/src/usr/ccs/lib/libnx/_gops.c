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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance
 *	with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_gops.c,v 1.6 1994/11/19 02:28:49 mtm Exp $
 *
 */

/* $Id: _gops.c,v 1.6 1994/11/19 02:28:49 mtm Exp $ */
/* ---------------- PNL-developed global operations ------------------ */

/* Copyright info:
 *
 *   This code was developed at Battelle / U.S. Department of Energy (DOE)
 *   Pacific Northwest Laboratory (PNL) in Richland, Washington, under U.S.
 *   government funding.  Neither Battelle, PNL, DOE, the U.S. government,
 *   nor any employee of those organizations makes any claim about the
 *   correctness or suitability of this code for any purpose whatsoever,
 *   nor assumes any responsibility for damages resulting from its use.
 *
 *   This code is transferred to Intel for possible modification and
 *   incorporation into the operating system on the Concurrent
 *   Supercomputing Consortium Touchstone DELTA computer at Caltech.
 *   Battelle/PNL retains all other rights regarding its use.  The code
 *   cannot be further redistributed without written permission from
 *   Battelle/PNL.
 *
 *   _gops.c 3.1 92/04/03 10:21:00
 */

/* Technical summary:
 *
 *   See file "gops.h" for configurable parameters.
 *
 *   This file plus "gops.h" will produce a complete family of global
 *   operations, including
 *
 *      gopf
 *      g{i,l}and
 *      g{d,i,s}high
 *      g{d,i,s}low
 *      g{i,l}or
 *      g{d,i,s}prod
 *      g{d,i,s}sum
 *      g{i,l}xor
 *
 *  in both C- and Fortran-callable flavors.
 *
 *  All of the combining operations are implemented in terms of a single
 *  new variant of gopf, called 'gopfb' for 'blocked'.  The calling
 *  sequence of gopfb extends that of gopf by including the length of an
 *  element:
 *
 *     gopfb (x,elen,xlen,work,fb)
 *     char x[];       // data to be combined
 *     long elen;      // length of a single element, in bytes
 *     long xlen;      // total length of x, in bytes
 *     char work[];    // scratch space, same size as x
 *     long (*fb)();   // f(x,work,blen) | blen = block length in elements
 *
 *  This extension allows gopfb to use pipelined or divide-and-conquer
 *  algorithms, which are often faster than simple one-step-at-a-time
 *  tree-based algorithms.
 *
 *  The current implementation picks the best of three algorithms
 *  based on the data volume and mesh size.  The algorithms are:
 *    1. non-pipelined binary tree that collapses down the columns
 *       and across row 0 in log(P) steps, then retraces its steps
 *       to broadcast.  This algorithm is the fastest method for small
 *       amounts of data.
 *    2. pipelined linear "snake" that sends data inward from both
 *       ends, combining it as it goes, and overlaps the subsequent
 *       broadcast with the combining of later blocks.  This algorithm
 *       is the fastest method for very large amounts of data.
 *    3. pipelined 2-D "fence" that sends data down the columns and
 *       across row 0 to combine it, and overlaps broadcast with
 *       combining.  This algorithm is the fastest for moderate
 *       amounts of data.
 *
 *  See "Modeling Node Bandwidth Limits and Their Effect on Vector
 *  Combining Algorithms", R.J.Littlefield, PNL, for discussion of the
 *  performance analysis of these algorithms.
 *
 *  Note that none of these algorithms is optimal in the sense of being the
 *  best that one can do.  In particular, I am working on a divide-and-conquer
 *  "accelerator" that can be used in conjunction with the tree and fence
 *  algorithms, promising to reduce overall execution time by a factor of
 *  1.5-2X.  Unfortunately, the accelerator will not be completed in time
 *  to meet release deadlines.
 *
 *  --Rik Littlefield, PNL
 */

#include <errno.h>
#include <nx/gops.h>
#include <stdio.h>
#include <math.h>

/* Combining function template:
 *
 *      gopfb_cfunc(name,type,func) constructs 1) a function whose
 *      name is gopfb_${name}, operating on data of type type, using
 *      function cop_${func}, plus 2) a user-visible function whose
 *      name is g${name}, that works by calling gopfb with the appropriate
 *      name, plus 3) a fortran-callable function whose name is g${name}_ .
 *      (Notation: x${foo} means 'x' concatenated with the expansion of foo.)
 */

#define gopfb_cfunc(name,type,func) \
long tpaste2(gopfb_,name) (x,work,nelem) \
type x[], work[]; \
long nelem; \
{ \
  register int i; \
  register int ilim = nelem; \
  for (i=0; i<ilim; i++) {tpaste2(cop_,func)(x[i],work[i]);} \
} \
  \
tpaste2(USERPREFIX(),name) (x,n,work) \
type x[], work[]; \
long n; \
{ \
  tpaste2(USERPREFIX(),opfb) \
    (x,sizeof(type),n*sizeof(type),work,tpaste2(gopfb_,name)); \
} \
/*
 * Comment out part that generates Fortran-callable interface
  \
tpaste3(USERPREFIX(),name,_) (x,np,work) \
type x[], work[]; \
long *np; \
{ \
  tpaste2(USERPREFIX(),name) (x,*np,work); \
} \
*/

/* Combining functions: */

gopfb_cfunc (iand,int,and);
gopfb_cfunc (land,int,land);

gopfb_cfunc (dhigh,double,high);
gopfb_cfunc (ihigh,long,high);
gopfb_cfunc (shigh,float,high);

gopfb_cfunc (dlow,double,low);
gopfb_cfunc (ilow,long,low);
gopfb_cfunc (slow,float,low);

gopfb_cfunc (ior,long,or);
gopfb_cfunc (lor,long,lor);

gopfb_cfunc (dprod,double,prod);
gopfb_cfunc (iprod,int,prod);
gopfb_cfunc (sprod,float,prod);

gopfb_cfunc (dsum,double,sum);
gopfb_cfunc (isum,int,sum);
gopfb_cfunc (ssum,float,sum);

gopfb_cfunc (ixor,long,xor);
gopfb_cfunc (lxor,long,xor);

/* User-callable 'gopf' interface:
   
   gopf works by calling gopfb with a special "cover" combining function
   whose job is just to strip off the extra argument and then call the
   two-argument function that the user passed into gopf.  It would
   probably work to just pass the extra argument, but the structure
   used here avoids possible subtle problems later caused by e.g.
   incorrect stack manipulations.
*/

long (*gopf_argf)();    /* saves the user-supplied function pointer */
long (*gopfb_fargfb)();  /* saves user-supplied function pointer for Fortran */

gopf_cvr (x,work,nelem)    /* this is the cover function for gopf*/
char x[], work[];
long nelem;
{
  (*gopf_argf)(x,work);
}

tpaste2(USERPREFIX(),opf) (x,xlen,work,f)      /* C-callable interface */
char x[], work[];
long xlen;
long (*f)();
{ gopf_argf = f;
  tpaste2(USERPREFIX(),opfb) (x,xlen,xlen,work,gopf_cvr);
}

/*
 * Need to keep the Fortran-callable interface to _gopf_
 */

tpaste2(USERPREFIX(),opf_) (x,xlenp,work,f)
char x[], work[];
long *xlenp;
long (*f)();
{
  tpaste2(USERPREFIX(),opf) (x,*xlenp,work,f);
}

/* Fortran-callable gopfb is done using a cover function trick similar
   to that of gopf.  Here, the cover function is used to convert the
   nelem parameter from call-by-value to call-by-reference, as required
   by the user-supplied combining function written in Fortran.
*/

gopfb_fcvr (x,work,nelem) /* this is the cover function for Fortran gopfb */
char x[], work[];
long nelem;
{
  long nelemt = nelem;
  (*gopfb_fargfb) (x,work,&nelemt);
}

tpaste2(USERPREFIX(),opfb_) (x,elenp,xlenp,work,fb) /* Fortran gopfb */
char x[], work[];
long *elenp;
long *xlenp;
long (*fb)();
{ gopfb_fargfb = fb;
  tpaste2(USERPREFIX(),opfb) (x,*elenp,*xlenp,work,gopfb_fcvr);
}

/* ---------- Mapping routines:

   The following routines implement two kinds of mappings:
     1. node numbers (as used by the communications calls) to and from
        grid row/column.  This mapping should reflect the physical
        topology of the machine.
     2. grid row/column to and from position within a ring that has
        been carefully embedded in the grid so as to avoid link
        conflicts if the grid --> physical mapping does.

*/

/* gopfb_nd2rc -- map from comm node number to (row,column) */

gopfb_nd2rc (node,h,w,rp,cp)
int node;     /* communications node number */
int h;        /* grid height */
int w;        /* grid width */
int *rp;      /* row number returned */
int *cp;      /* column number returned */
{
  int r, c;
  r = node / w;
  c = node % w;
  *rp = r;
  *cp = c;
  return;
}

/* gopfb_rc2nd -- map from (row,column) to comm node number */

gopfb_rc2nd (h,w,r,c)
int h;       /* grid height */
int w;       /* grid width */
int r;       /* row number */
int c;       /* column number */
{
  int node;
  node = r*w + c;
  return (node);
}

/* gopfb_rc2rp -- map from (row,column) to ring logical position */

long gopfb_rc2rp (h,w,r,c)
int h;        /* height of the mesh */
int w;        /* width of the mesh */
int r;        /* row number (from 0) */
int c;        /* column number (from 0) */
{ int lpos;      /* logical position in ring */
  if      (w == 1)     lpos = r;
  else if (r == 0)     lpos = c;
  else if (c == 0)     lpos = h*w - r;
  else if (r % 2 == 0) lpos = c + (w-1)*r;
  else                 lpos = (w-1)*(r+1) + 1 - c;
  return lpos;
}

/* gopfb_rp2rc -- map from ring logical position to (row,column) */

gopfb_rp2rc (lpos,h,w,rp,cp)
int lpos;     /* logical position in ring */
int h;        /* height of mesh */
int w;        /* width of mesh */
int *rp;      /* row number returned */
int *cp;      /* column number returned */
{
  int r, c;   /* row and column temporaries */

  if (w == 1) {r=lpos; c=0; }
  else if (lpos < w) { r=0; c = lpos;}
  else if (lpos <= h*(w-1)) {
    r = (lpos-1)/(w-1);
    if (r%2 == 0) { c = lpos - r*(w-1); }
    else          { c = (r+1)*(w-1) - lpos + 1; }
  }
  else {c = 0; r = h*w - lpos; }
  *rp = r;
  *cp = c;
}

/* iPSC/860 substitute for __mypart():

   For testing in an iPSC/860 environment, compile with -DCUBE.
*/

#ifdef CUBE
__mypart (rp,cp)   /* substitute to use in iPSC/860 environment */
long *rp, *cp;
{ int r, c, nnodes;

  nnodes = numnodes ();
  r = 1;
  c = 1;
  while (r*c < nnodes) {
    c *= 2;
    if (r*c < nnodes) r *= 2;
  }

  /* Fix it up if rows * columns not equal numnodes.  */
  if (r*c > nnodes) {
    r = nnodes;
    c = 1;

    /* make it as square as possible */
    while ((r > c) && ((r & 1) == 0)) {
        r /= 2;
        c *= 2;
    }
  }

  *rp = r;
  *cp = c;
}
#endif

/* ---------- gopfb itself:
*/


double gopfb_fence_cost ();    /* cost functions defined later */
double gopfb_snake_cost ();
double gopfb_tree_cost ();

tpaste2(USERPREFIX(),opfb) (x,elen,xlen,work,fb)
char x[];       /* data to be combined */
long elen;      /* length of a single element, in bytes */
long xlen;      /* total length of x, in bytes */
char work[];    /* scratch space, same size as x */
long (*fb)();    /* f(x,work,blen) | blen = block length in elements */

/* This routine implements three algorithms for global combining.
   See "Modeling Node Bandwidth Limits and Their Effect on Vector
   Combining Algorithms", R.J.Littlefield, Battelle / D.O.E.
   Pacific Northwest Laboratory, Richland, WA, for a discussion
   of the algorithms and cost models.
*/

{
  float treecost, fencecost, snakecost, mincost;
  long h, w;        /* mesh size */
  long sopt;        /* optimum block size for pipelined schemes */
  double d1, d2, d3; /* for timing */

  if (xlen < 0) {
	errno = EQLEN;
	return -1;
  }
  __mypart (&h,&w);

  /* For really short stuff don't mess around with considering 
     alternate algorithms -- just go with the tree.
  */

  fencecost = 1.e30;
  snakecost = 1.e30;
  if (xlen < GOP_ALG_THRESHOLD) treecost = 0;
  else
  { treecost = gopfb_tree_cost (xlen,elen,w,h);
    fencecost = gopfb_fence_cost (xlen,elen,w,h,&sopt);
    snakecost = gopfb_snake_cost (xlen,elen,w,h,&sopt);
  }

#ifdef DEBUG
  if (mynode() == 0)
  { printf ("treecost = %f\n",treecost);
    printf ("fencecost, sopt = %f %d\n",fencecost,sopt);
    printf ("snakecost, sopt = %f %d\n",snakecost,sopt);
  }
#endif

  gopfb_treecost = treecost;
  gopfb_fencecost = fencecost;
  gopfb_snakecost = snakecost;

  mincost = treecost;
  if (fencecost < mincost) mincost = fencecost;
  if (snakecost < mincost) mincost = snakecost;

/* left-over timing stuff
  gsync ();
  d1 = dclock ();
*/

  if (treecost == mincost) {
    /* if (mynode() == 0) printf ("using tree\n"); */
    gopfb_algused = 0;
    gopfb_tree (x,xlen,work,fb,elen,w,h);
  }

  else if (fencecost == mincost) {
    /* if (mynode() == 0) printf ("using fence, sopt = %d\n",sopt); */
    gopfb_algused = 1;
    gopfb_sopt = sopt;
    gopfb_fence (x,xlen,work,fb,elen,w,h,sopt);
  }

  else if (snakecost == mincost) {
    /* if (mynode() == 0) printf ("using snake, sopt = %d\n",sopt); */
    gopfb_algused = 2;
    gopfb_sopt = sopt;
    gopfb_snake (x,xlen,work,fb,elen,w,h,sopt);
  }

  else {
    fprintf (stderr,"gopvf: can't happen\n");
  }

/* leftover timing stuff...
  gsync ();
  d2 = dclock ();
  gsync ();
  d3 = dclock ();
  if (mynode() == 0) printf ("actual time = %f\n",(d2-d1)-(d3-d2));
*/
  return 0;
}

double gopfb_fence_cost (xlen,le,w,h,soptp)
long xlen;         /* total data length, in bytes */
long le;           /* length of a data element, in bytes */
long w;            /* width of mesh */
long h;            /* height of mesh */
long *soptp;       /* returned as optimal block length, 
                      in elements (bytes/le) */
{
  int n;           /* number of elements to combine */
  double beta;     /* xfer cost per element */
  double tpred;    /* predicted execution time */
  int sopt;        /* optimum block length */
  int nb;          /* number of blocks */

  /* Unless the mesh is at least 2x3, the cost model is wrong (too small),
     but the algorithm is inappropriate anyway.  Returning a big number
     will keep the algorithm from being selected.
  */

  if (h < 2 || w < 3) {
    return (1.e30);
  }

  /* Otherwise, the cost model is OK.  First, compute an optimal
     block size under continuity assumptions.
  */

  n = xlen/le;
  beta = XferPerByte * le;
  sopt = gop_sqrt(6*n*alpha/
                  (h*(c2+2*beta*f2) + w*(c3+2*beta*f3) -3*c3 +
                  beta*(2*f1 -2*f2 -4*f3 +2*f4 -3*f6)));

  /* Adjust nominal optimum block size to account for endcases and to
     incorporate heuristics about the Delta's communications protocol.
     The intent is to use full packets whenever that can be done
     without increasing the block size, and to always keep the blocks
     small enough that each block can be transferred without having to
     pay for an explicit acknowledgement (i.e., without using up the
     source processor's quota of packet buffers on the receiver).
  */

  if (sopt > n) sopt = n;
  if (sopt > GOPFB_PACKETSIZE/le) 
    sopt = ((sopt*le)/GOPFB_PACKETSIZE)*(GOPFB_PACKETSIZE/le);
  if (sopt > MAX_GOPFB_BLOCKSIZE/le) sopt = MAX_GOPFB_BLOCKSIZE/le;
  if (sopt < 1) sopt = 1;
  
  nb = (n+sopt-1)/sopt;

  tpred = 
        1   * (1*alpha + f1 * beta * sopt + c2 * sopt) +
      (h-1) * (2*alpha + f2 * beta * sopt + c2 * sopt) +
      (w-2) * (3*alpha + f3 * beta * sopt + c3 * sopt) +
        1   * (4*alpha + f4 * beta * sopt + c3 * sopt) +
     (nb-3) * (6*alpha + f6 * beta * sopt + c3 * sopt) +
        1   * (4*alpha + f4 * beta * sopt + c3 * sopt) +
      (w-2) * (3*alpha + f3 * beta * sopt        ) +
      (h-1) * (2*alpha + f2 * beta * sopt        ) +
        1   * (1*alpha + f1 * beta * sopt        );

  *soptp = sopt;
  return (tpred);
}

gopfb_fence (x,xlen,work,fb,le,w,h,b)
char x[];            /* data to be combined, length [xlen] bytes */
char work[];         /* work array, length [b*le] bytes */
long xlen;           /* length of x in bytes */
long (*fb)();        /* combining function */
long le;             /* length of an element in bytes */
long w;              /* width of the node array */
long h;              /* height of the node array */
long b;              /* communication block size, in elements (bytes/le) */
{
  int pipedelay;          /* number of csends before first returning block
                             is crecv'd */
  int cb, rb;             /* combining and returning block numbers */
  int cbsize, rbsize;     /* block sizes, in elements */
  int pid;                /* process id */
  int nb;                 /* total number of blocks needed */
  int ioff;               /* byte offset into x */
  int me;                 /* this node number */
  int myrow, mycol;       /* this logical row and column within grid */
  int above, below, left, right;     /* node numbers for comm partners */
  int n;                  /* number of elements to be combined */

  n = xlen / le;

    /* number of blocks = ceiling(n/b) */
  nb = (n+b-1)/b;

    /* get this node's grid coordinates */
  pid = myptype ();
  me = mynode ();
  gopfb_nd2rc (me,h,w,&myrow,&mycol);

    /* get node numbers of communication partners */
  above = below = right = left = -1;
  if (myrow != h-1) above = gopfb_rc2nd (h,w,myrow+1,mycol);
  if (myrow != 0)   below = gopfb_rc2nd (h,w,myrow-1,mycol);
  if (mycol != w-1) right = gopfb_rc2nd (h,w,myrow,mycol+1);
  if (mycol != 0)   left  = gopfb_rc2nd (h,w,myrow,mycol-1);

    /* number of csends before first returning block is crecv'd */
  pipedelay = 2*(myrow+mycol)+1;

  cb = 0;          /* combining block number */
  rb = -pipedelay; /* returning block number */

  while (rb < nb)
  {
     if (cb < nb) 
     {  cbsize = n - cb*b;
	if (cbsize > b) cbsize = b;
        gopfb_pxlen = cbsize*le;
	ioff = cb*b*le;

	if (myrow != h-1)
	{   crecv (DOWNTYPE,work,gopfb_pxlen);
	    fb(x+ioff,work,cbsize);
	}
	if (myrow == 0 && mycol < w-1)
	{  crecv (LEFTTYPE,work,gopfb_pxlen);
	   fb(x+ioff,work,cbsize);
	}
	if (myrow > 0)
	{  csend (DOWNTYPE,x+ioff,gopfb_pxlen,below,pid);
	}
	else if (mycol > 0)
	{  csend (LEFTTYPE,x+ioff,gopfb_pxlen,left,pid);
	}
	cb = cb + 1;
    }

    if (rb >= 0) 
    {  rbsize = n - rb*b;
       if (rbsize > b) rbsize = b;
       ioff = rb*b*le;

       if (myrow > 0)
       {  crecv (UPTYPE,x+ioff,rbsize*le);
       }
       else if (mycol > 0)
       {  crecv (RIGHTTYPE,x+ioff,rbsize*le);
       }
       if (myrow != h-1)
       {  csend (UPTYPE,x+ioff,rbsize*le,above,pid);
       }
       if (myrow == 0 && mycol < w-1)
       {  csend (RIGHTTYPE,x+ioff,rbsize*le,right,pid);
       }
    }
    rb = rb + 1;

  }

  return;
}

/*------------------------------------------------------------------------*/

double gopfb_snake_cost (xlen,le,w,h,soptp)
long xlen;         /* total data length, in bytes */
long le;           /* length of a data element, in bytes */
long w;            /* width of mesh */
long h;            /* height of mesh */
long *soptp;       /* returned as optimal block length, 
                      in elements (bytes/le) */
{
  int n;           /* number of elements to combine */
  double beta;     /* xfer cost per element */
  double tpred;    /* predicted execution time */
  int sopt;        /* optimum block length */
  int nb;          /* number of blocks */

  n = xlen/le;
  beta = XferPerByte * le;

  /* The following equation may not be strictly correct -- it was obtained
     by blindly substituting (h*w)/2-1 for P-2 in the cost equation
     derived for the SHPCC paper.  Oh well, it ought to be close,
     and these optima are pretty flat.
  /*

  sopt = gop_sqrt(4*n*alpha/(((h*w)/2-1)*(c2+2*beta*f2)+2*beta*f3-3*beta*f4)),

/*
  if (mynode() == 0)
  {  printf ("n,alpha,beta,h,w,sopt = %d, %f, %f, %d, %d, %d\n",
              n,alpha,beta,h,w,sopt);
  }
*/

  /* Adjust nominal optimum block size to account for endcases and
     to incorporate heuristics about the Delta's comm protocol.  The
     intent is to use full packets whenever that can be done without
     increasing the block size, and to always keep the blocks small
     enough that each block can be transferred without having to
     pay for an explicit acknowledgement (i.e., without using up
     the source processor's quota of packet buffers on the receiver).
  */

  if (sopt > n) sopt = n;
  if (sopt > GOPFB_PACKETSIZE/le) 
    sopt = ((sopt*le)/GOPFB_PACKETSIZE)*(GOPFB_PACKETSIZE/le);
  if (sopt > MAX_GOPFB_BLOCKSIZE/le) sopt = MAX_GOPFB_BLOCKSIZE/le;
  if (sopt < 1) sopt = 1;
  
  nb = (n+sopt-1)/sopt;

  tpred = 
           1   * (1*alpha + f1 * beta * sopt + c2 * sopt) +
  ((h*w)/2+1) * (2*alpha + f2 * beta * sopt + c2 * sopt) +
           1   * (3*alpha + f3 * beta * sopt + c2 * sopt) +
        (nb-3) * (4*alpha + f4 * beta * sopt + c2 * sopt) +
           1   * (3*alpha + f3 * beta * sopt + c2 * sopt) +
  ((h*w)/2+1) * (2*alpha + f2 * beta * sopt         ) +
           1   * (1*alpha + f1 * beta * sopt         );

  *soptp = sopt;
  return (tpred);
}

gopfb_snake (x,xlen,work,fb,le,w,h,b)
char x[];            /* data to be combined, length [xlen] bytes */
char work[];         /* work array, length [b*le] bytes */
long xlen;           /* length of x in bytes */
long (*fb)();        /* combining function */
long le;             /* length of an element in bytes */
long w;              /* width of the node array */
long h;              /* height of the node array */
long b;              /* communication block size, in elements (bytes/le) */
{
  int pipedelay;          /* number of csends before first returning block
                             is crecv'd */
  int cb, rb;             /* combining and returning block numbers */
  int cbsize, rbsize;     /* block sizes, in elements */
  int pid;                /* process id */
  int nb;                 /* total number of blocks needed */
  int ioff;               /* byte offset into x */
  int me;                 /* this node number */
  int myrow, mycol;       /* this logical row and column within grid */
  int left, right;        /* node numbers for comm partners */
  int n;                  /* number of elements to be combined */
  int lcenter;            /* logical node number in the center of the snake */
  int lpos;               /* logical position of this node within the snake */
  int r, c;               /* temporaries -- row and column within the grid */

  n = xlen / le;
  lcenter = (w*h)/2;

    /* number of blocks = ceiling(n/b) */
  nb = (n+b-1)/b;

    /* get this node's grid coordinates */
  pid = myptype ();
  me = mynode ();
  gopfb_nd2rc (me,h,w,&myrow,&mycol);
  lpos = gopfb_rc2rp (h,w,myrow,mycol);

    /* get node numbers of communication partners */
  right = left = -1;
  if (lpos > 0) {
    gopfb_rp2rc (lpos-1,h,w,&r,&c);
    left = gopfb_rc2nd (h,w,r,c);
  }
  if (lpos < h*w-1) {
    gopfb_rp2rc (lpos+1,h,w,&r,&c);
    right = gopfb_rc2nd (h,w,r,c);
  }

    /* number of csends before first returning block is crecv'd */
  pipedelay = 2*(lpos-lcenter);
  if (pipedelay < 0) pipedelay = -pipedelay;
  pipedelay = pipedelay + 2;

  cb = 0;          /* combining block number */
  rb = -pipedelay; /* returning block number */

  while (rb < nb)
  {
     if (cb < nb) 
     {  cbsize = n - cb*b;
	if (cbsize > b) cbsize = b;
        gopfb_pxlen = cbsize*le;
	ioff = cb*b*le;

	if (lpos >= lcenter && lpos < h*w-1)
	{   crecv (LEFTTYPE,work,gopfb_pxlen);
	    fb(x+ioff,work,cbsize);
	}
	if (lpos <= lcenter && lpos > 0)
	{   crecv (RIGHTTYPE,work,gopfb_pxlen);
	    fb(x+ioff,work,cbsize);
	}

	if (lpos > lcenter)
	{  csend (LEFTTYPE,x+ioff,gopfb_pxlen,left,pid);
	}
	else if (lpos < lcenter)
	{  csend (RIGHTTYPE,x+ioff,gopfb_pxlen,right,pid);
	}
	cb = cb + 1;
    }

    if (rb >= 0) 
    {  rbsize = n - rb*b;
       if (rbsize > b) rbsize = b;
       ioff = rb*b*le;

       if (lpos < lcenter)
       {  crecv (LEFTTYPE,x+ioff,rbsize*le);
       }
       else if (lpos > lcenter)
       {  crecv (RIGHTTYPE,x+ioff,rbsize*le);
       }
       if (lpos <= lcenter && lpos > 0)
       {  csend (LEFTTYPE,x+ioff,rbsize*le,left,pid);
       }
       if (lpos >= lcenter && lpos < h*w-1)
       {  csend (RIGHTTYPE,x+ioff,rbsize*le,right,pid);
       }
    }
    rb = rb + 1;

  }

  return;
}

/* ------------------------------------------------------------------------ */

double gopfb_tree_cost (xlen,le,w,h)
long xlen;         /* total data length, in bytes */
long le;           /* length of a data element, in bytes */
long w;            /* width of mesh */
long h;            /* height of mesh */
{
  int n;           /* number of elements to combine */
  double beta;     /* xfer cost per element */
  double tpred;    /* predicted execution time */
  int logp;        /* log_2 (numnodes) */
  int p2;           /* 2^logp */

  n = xlen/le;
  beta = XferPerByte * le;

  logp = 0; p2 = 1;
  while (p2 < h*w) { logp += 1; p2 *= 2;}

/*
  if (mynode() == 0)
  {  printf ("n,alpha,beta,h,w,logp = %d, %f, %f, %d, %d, %d\n",
              n,alpha,beta,h,w,logp);
   }
*/
  
  tpred = 2 * logp * (alpha + n*beta + c2*beta);
  return (tpred);
}


gopfb_tree (x,xlen,work,fb,le,w,h)
char x[];            /* data to be combined, length [xlen] bytes */
char work[];         /* work array, length [b*le] bytes */
long xlen;           /* length of x in bytes */
long (*fb)();        /* combining function */
long le;             /* length of an element in bytes */
long w;              /* width of the node array */
long h;              /* height of the node array */
{
  int pid;                /* process id */
  int me;                 /* this node number */
  int myrow, mycol;       /* this logical row and column within grid */
  int partner;            /* node number of current communication partner */
  int n;                  /* number of elements to be combined */
  int r, c;               /* temporaries -- row and column within the grid */
  int dr, dc;             /* delta row and column */
  int mod;                /* temporary */
  int sender;             /* node number that sent message */

  n = xlen / le;
  gopfb_pxlen = xlen;

    /* get this node's grid coordinates */
  pid = myptype ();
  me = mynode ();
  gopfb_nd2rc (me,h,w,&myrow,&mycol);

    /* Reduce columns */

  for (dr = 1; dr < h; dr *= 2)
  { mod = myrow % (2*dr);
    if (mod == 0 & myrow+dr < h)  /* this node receives */
    { partner = gopfb_rc2nd (h,w,myrow+dr,mycol);
      crecv (INTYPE+partner,work,xlen);
      if ((sender = infonode()) != partner)
      {  printf ("gopfb_tree, incorrect nodes: %d != %d\n",sender,partner);
      }
      fb(x,work,n);
    }
    else if (mod == dr) /* this node sends */
    { partner = gopfb_rc2nd (h,w,myrow-dr,mycol);
      csend (INTYPE+me,x,xlen,partner,pid);
    }
  }

    /* Reduce row 0 */

  if (myrow == 0)
  { for (dc = 1; dc < w; dc *= 2)
    { mod = mycol % (2*dc);
      if (mod == 0 & mycol+dc < w)  /* this node receives */
      { partner = gopfb_rc2nd (h,w,myrow,mycol+dc);
	crecv (INTYPE+partner,work,xlen);
        if ((sender = infonode()) != partner)
        { printf ("gopfb_tree, incorrect nodes: %d != %d\n",sender,partner);
        }
	fb(x,work,n);
      }
      else if (mod == dc) /* this node sends */
      { partner = gopfb_rc2nd (h,w,myrow,mycol-dc);
	csend (INTYPE+me,x,xlen,partner,pid);
      }
    }
  }

    /* Broadcast row 0 */

  if (myrow == 0)
  { while (dc > 0)
    { mod = mycol % (2*dc);
      if (mod == 0 & mycol+dc < w)  /* this node sends */
      { partner = gopfb_rc2nd (h,w,myrow,mycol+dc);
	csend (OUTTYPE,x,xlen,partner,pid);
      }
      else if (mod == dc) /* this node receives */
      { partner = gopfb_rc2nd (h,w,myrow,mycol-dc);
	crecv (OUTTYPE,x,xlen);
      }
      dc /= 2;
    }
  }

  /* Broadcast across columns */

  while (dr > 0)
  { mod = myrow % (2*dr);
    if (mod == 0 & myrow+dr < h)  /* this node sends */
    { partner = gopfb_rc2nd (h,w,myrow+dr,mycol);
      csend (OUTTYPE,x,xlen,partner,pid);
    }
    else if (mod == dr) /* this node receives */
    { partner = gopfb_rc2nd (h,w,myrow-dr,mycol);
      crecv (OUTTYPE,x,xlen);
    }
    dr /= 2;
  }

  return;
}

/* ---------- Global sync.

   This routine implements a binary tree global sync that causes
   no more than 2 messages to be queued at any node.  This minimizes
   possible slowdown due to increasing message queue length when
   many processors are blocked at the gsync.

*/
