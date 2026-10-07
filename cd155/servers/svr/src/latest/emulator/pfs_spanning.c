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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	Copyright 1992 Intel Corporation.
 *
 *  This module implements the spanning tree algorithm for broadcast
 *  as described in the paper by Payne, Barnett and Robt van de Geijn
 *  of December 4th titled "Optimal Broadcasting in Mesh-Connected
 *  Architectures".
 *
 * HISTORY
 * $Log: pfs_spanning.c,v $
 * Revision 1.3  1994/11/18  20:24:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/03/29  18:33:02  rlg
 * Merged the changes from 1.1.8.1 in the R1.2 branch into R1.3.
 *
 * 
 */
#ifdef PFS
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/std_types.h>
#include <mach/mach_types.h>
#include <sys/estat.h>
#include <sys/errno.h>
#include "emul.h"
#include "fdt.h"
#include "pfs_iomode.h"
/*
 * Forward references:
 */
static unsigned int bintogray();
static unsigned int graytobin();
static int  power_of_two();
static void testrecv();
static void toggle();
static void tree_init();
static int  tree_recv();
static int  tree_send();



/***************************************************************************
 *	
 *	Calling Sequence:
 *	      pfs_getspanning()
 *	
 *	Description:
 *		The pfs_getspanning function is used to return the
 *		list of destination nodes when using a spanning tree
 *		method of data exchange.  This function is used by 
 *		the M_GLOBAL PFS I/O mode to establish the data 
 *		communication channels.
 *	
 *	Parameters:
 *
 *		node:		My logical node number, (sending node.).
 *
 *		nnodes:		Number of nodes in the application.
 *		 
 *		final_list:	Array used to hold the destination
 *				node numbers. 	
 *
 *		final_cnt:	Number of node numbers in final_list.
 *	
 *	Returns:
 *		Nothing.
 *		
 */
void
pfs_getspanning(node, nnodes, final_list, final_cnt)
int node;
int nnodes;
int final_list[];
int *final_cnt;
{
int 		cnt, list[PFS_MAXSPAN];
int 		i;
long 		pt;
unsigned int 	gval;
int 		uphalf;
int 		pow;	
int 		rem;


	*final_cnt = 0;
	tree_init(0, nnodes, &pt, &gval, &uphalf, &pow, &rem);
	cnt = tree_send(list,nnodes,gval,pt,uphalf,pow);
	if( node == 0) {
		*final_cnt = cnt;
		for (i=0; i<cnt; i++) {
			final_list[i] = list[i];
		}
	} else {
		for (i=0; i<cnt; i++) {
			testrecv(0, list[i], final_list,
				 final_cnt, nnodes, node);
		}
	}
}


/***************************************************************************
 *	
 *	Calling Sequence:
 *	      testrecv()
 *	
 *	Description:
 *		This function is used to determine the destination
 *		node numbers to send data to using the spanning tree
 *		communicaton algorithm.
 *	
 *	Parameters:
 *
 *		src:		Source node.		
 *
 *		newme:		My node.
 *
 *		final_list:	Array of destination nodes.
 *
 *		final_cnt:	Number of destination nodes.		
 *
 *		nnodes:		Number of nodes in the application.
 *
 *	Returns:
 *	      Nothing
 *	
 */
static void
testrecv(src, newme, final_list, final_cnt, nnodes, node)
int src; 
int newme;
int final_list[];
int *final_cnt;
int nnodes;
{
	int cnt, list[PFS_MAXSPAN];
	int i;
	int pt, gval;
	int uphalf;
	int pow,rem;

	tree_init(newme, nnodes, &pt, &gval, &uphalf, &pow, &rem);
	cnt = tree_recv (src, list, nnodes, pt, gval, uphalf, pow);
	if (cnt > 0) {
		if (node == newme) {
			*final_cnt = cnt;
			for (i=0; i<cnt; i++) {
				final_list[i] = list[i];
			}
			cnt = 0;
		}
	}
	for (i=0; i<cnt; i++) {
		testrecv(newme, list[i], final_list,
			 final_cnt, nnodes, node);
	}
}

	

/***************************************************************************
 *	
 *	Calling Sequence:
 *	      tree_init()
 *	
 *	Description:
 *	      Initialize variables used for spanning tree broadcast.
 *	
 *	Parameters:
 *		me:	My logical node number.
 *
 *		num:	Number of nodes in the application.
 *
 *		pt:	Power of two number of nodes.
 *
 *		gval:	Gray code number.
 *
 *		uphalf: Flag indicating lower or upper half.
 *
 *		pow:	Number of nodes power of 2?	
 *
 *		rem:	Remaining nodes.
 *
 *	
 *	Returns:
 *	      None
 *	
 */
static void
tree_init(me, num, pt, gval, uphalf, pow, rem)
int me; 
int num;
int *pt;
int *gval;
int *uphalf;
int *pow;
int *rem;

{
	*pt = power_of_two(num, pow, rem);
	*gval = bintogray( me );
	*uphalf = 0;
	if ( me >= (1<<*pow) ) *uphalf = 1;
}


/***************************************************************************
 *	
 *	Calling Sequence:
 *		  count = tree_send(node_list);
 *	
 *	Description:
 *		  Calculate node list for originator of broadcast
 *		  using spanning tree algorithm.
 *	
 *	Parameters:
 *		  int node_list[]		logical node list
 *		  int num			Number of nodes.
 *		  int gval			Gray code value.
 *		  int pt			Power of two flag.
 *		  int uphalf			Upper half flag.
 *		  int pow			What power of two.	
 *	
 *	Returns:
 *		  int count			number of nodes in node_listp
 *	
 */
static int
tree_send(node_list, num, gval, pt, uphalf, pow)
	int	node_list[];
	int	num;
	int	gval;
	int	pt;
	int     uphalf;
	int	pow;
{
	long  i;
	int node_count;

	unsigned int gdestproc;
	unsigned int destproc;

	node_count = 0;

	if ( pt == 1 ) {		/* is a power of 2 */

		if ( pow > 0 ) {
			for (i=pow-1; i>=0; i--) {
				toggle( gval, i, &gdestproc );
				node_list[node_count++] = graytobin( gdestproc );
			}
		}

	} else {			/* not a power of 2 */


		if ( uphalf == 1 ) {	 /* upper half node */

			toggle( gval, pow, &gdestproc );
			node_list[node_count++] = graytobin( gdestproc );

		} else {			 /* lower half node */

			if ( pow > 0 ) {
				for (i=pow-1; i>=0; i--) {
					toggle( gval, i, &gdestproc );
					node_list[node_count++] = graytobin( gdestproc );
				}
			}

			toggle( gval, pow, &gdestproc );
			destproc = graytobin( gdestproc );
			if ( destproc < num ) {
				node_list[node_count++] = destproc;
			}
		}
	} 
	return node_count;
}



/***************************************************************************
 *	
 *	Calling Sequence:
 *		  node_count = tree_recv(from_node, node_list);
 *	
 *	Description:
 *		  Calculate node list for receiver of broadcast message
 *		  using spanning tree algorithm.
 *	
 *	Parameters:
 *        int from_node			the node the msg came from
 *	  int node_list[]		list of logical nodes to forward msg
 *	
 *	Returns:
 *	  int node_count		number of nodes in node_list
 *	
 */

static int
tree_recv(from_node, node_list, num, pt, gval, uphalf, pow)
	int 	from_node;
	int	node_list[];
	int	num;
	int	pt;
	int	gval;
	int	uphalf;
	int	pow;
{
	int node_count;

	int	i;
	int	destproc;
	unsigned int gsval, tmask, bitset, gdestproc;

	node_count = 0;

	if ( !uphalf ) {			 /* lower half node */

		gsval = bintogray( from_node );
		tmask = gval^gsval;
		if (tmask != 0) {
			bitset = 0;
			while (tmask != 1) {
				tmask = tmask >> 1;
				bitset++;
			}
			for (i=bitset-1; i>=0; i--) {
				toggle( gval, i, &gdestproc );
				node_list[node_count++] = graytobin( gdestproc );
			}
		}

		if ( pt != 1 ) {			/* not a power of 2 */
			toggle( gval, pow, &gdestproc );
			destproc = graytobin( gdestproc );
			if ((destproc < num)  && (destproc != from_node))
				node_list[node_count++] = destproc;
		}
	} 
	return node_count;
}



/*
 * Utility functions.
 */

static int
power_of_two(num, ppow, prem)
	long num;
	int *ppow;
	int *prem;
{
	unsigned size;

	*ppow = 0;
	size = 1;

	while ( size < num ) {
		size = size << 1;
		(*ppow)++;
	}
 
	if ( size == num ) {
		*prem = 0;
		return 1;
	}

	(*ppow)--;
	*prem = num - (size>>1);

	if ( *prem > 0 )
		 return 0;

	return -1;
}

static void
toggle( inbits, bitt, poutbits )
	unsigned int inbits;
	int bitt;
	unsigned int *poutbits;
{
	unsigned int bmask;

	bmask = 1 << bitt;
	if ( (inbits & bmask) >> bitt ) {
		*poutbits = inbits & ~bmask;
	} else {
		*poutbits = inbits | bmask;
	}
}

static unsigned int
bintogray(n)
unsigned int n;
{
  int temp;

  temp = n<<1;
  return((temp^n)>>1);

}

static unsigned int mask[16] = { 0x0001, 0x0002, 0x0004, 0x0008,
                                 0x0010, 0x0020, 0x0040, 0x0080,
                                 0x0100, 0x0200, 0x0400, 0x0800,
                                 0x1000, 0x2000, 0x4000, 0x8000 };

static unsigned int
graytobin(g)
unsigned int g;
{
  int i, n;

  n = mask[15] & g;
  for (i=14; i>=0; --i)
    n += (mask[i] & g) ^ ((mask[i+1] & n)>>1);
  return(n);
}
#endif
