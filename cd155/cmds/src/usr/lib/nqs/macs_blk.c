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
 *      This software is supplied under the terms of a license
 *      agreement or nondisclosure agreement with Intel Corpo-
 *      ration and may not be copied or disclosed except in
 *      accordance with the terms of that agreement.
 *
 */
/* macs_blk.c
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/macs_blk.c,v $
 *
 * DESCRIPTION:
 *
 *	The module contains a number of general subroutines for managing 
 *	 the block structure of the buddy system. 
 *
 *	Developer:
 *	----------
 *
 *	Michael Wan of San Diego Supercomputer Center.
 *
 *
 *
 */
/*
 * HISTORY
 * $Log: macs_blk.c,v $
 * Revision 1.6  1994/11/19  02:52:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/04/06  21:38:11  mwan
 * Update R1_3 to R1_2 WW15
 *
 *  Reviewer: kremenek
 *  Risk: M
 *  Benefit or PTS #: 7738,8087,8346, 8325,8599,8576,8600,8601,8088,8597,8876,8886
 *  Testing:
 *  Module(s):  macs_blk.c macs_job.c macs_lib.c macs_rootp.c macs_sched.c
 * 	     nqs_spawn.c res_msg.c smd_msg.c
 *
 * Revision 1.3  1993/07/13  17:52:13  mwan
 * T11 - fixed PTS 5022
 *
 * Revision 1.2  1992/10/09  22:24:24  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 *
 */

#ifdef SDSC
#include <stdio.h>
#include <fcntl.h>                      /* File control */
#include <pwd.h>                        /* File control */
#include <time.h>                       /* File control */
#include <signal.h>                     /* File control */
#include <errno.h>
#include "nqs.h"
#include "nqsxvars.h"
#include "buddyxvar.h"

/* External functions */

extern int init_job_req ();
extern void init_que ();
extern int release_job ();
extern int add_job ();
extern int rel_blkinjob ();
extern struct job_req *get_job ();
extern int queue_job ();
extern void prt_job ();
extern int rem_job ();
extern int rem_job_ts ();
extern struct job_req *find_job_by_name ();
extern void release_all ();
extern struct job_req *find_job ();
extern void release_all ();

int init_block ();
int rel_blk ();
int add_blk ();
int comb_blk ();
struct block *get_blk ();
int alloc_blk ();
struct block *srch_blk ();
struct block *srch_child ();
int free_to_job ();
int adj_count ();
int split_blk ();
void prt_tree ();
int bindex ();
int get_order ();
int mv_all_blk ();
int rem_blk ();
struct block *get_best_chd ();
int disp_blk ();
void free_blk_cnt ();
struct block *match_blk ();


/*** init_block ()
 *
 * initialize a block structure.
 *
 */
int init_block (blk)
struct block *blk;       /* pointer to the job_req being linked */
{
	int i;

	if (blk == NULL)
		return (-1);

	blk->free_nodes = 0;
	blk->x = 0;
	blk->y = 0;
	blk->width = 0;
	blk->height = 0;
	blk->no_combo = 0;
	blk->child_cnt = 0;
	blk->parent = NULL;
	blk->node_set = NULL;
	blk->layer_inx = 0;

	for (i = 0; i < MAX_BUD; i++) {
		blk->child[i] = NULL;
	}
	return (0);
}

/*** rel_blk
 *
 * Release the block blk. 
 *
 */

int rel_blk (blk)
struct block *blk;       /* pointer to the block being released */
{
	long free_nodes;

	if (blk == NULL)
		return (-1);

	free_nodes = blk->width * blk->height;
	if (adj_count (blk, free_nodes) < 0)  /* restore free_nodes counts */
	    printf ("I$rel_blk: adj_count problem\n");
	if (comb_blk (blk) < 0) 	/* combine buddies */
	    printf ("I$rel_blk: comb_blk problem\n");
	return (0);
}

/*** add_blk
 *
 * add a block (blk) to the queue (b_que)
 * input flag : 
 *	LAST - add to the end.
 *	SIZE - in decreasing order of size (number of nodes).
 *
 */

int add_blk (blk, b_que, flag)
struct block *blk;	/* pointer to the block being linked */
struct blk_que *b_que;	/* pointer to the blk_que to link to */
int flag;
{
	long size;
	struct block *t_blk;

	if (blk == NULL || b_que == NULL)
		return (-1);

	switch (flag) {
	case LAST :		/* add to the end of the queue */
		if (b_que->top == NULL) {          /* first item */
			b_que->top = blk;
		} else {
			b_que->last->next = blk;
		}
		blk->next = NULL;                /* end of b_que */
		blk->prev = b_que->last;        
		b_que->last = blk;
		break;
	case SIZE :		/* in decreading order of height * width */
        	size = blk->width * blk->height;
        	if (b_que->top == NULL) {          /* first item */
                	b_que->top = blk;
                	b_que->last = blk;
                	blk->next = NULL;
                	blk->prev = NULL;
        	} else {
                	t_blk = b_que->top;
                	while (1) {
                        	if (t_blk != NULL) {
                                    if (t_blk->width * t_blk->height < size) { 
					if (t_blk == b_que->top) {  /* top */
                                                b_que->top = blk;
                                                blk->prev = NULL;
                                                blk->next = t_blk;
                                                t_blk->prev = blk;
                                        } else {    /* in the middle */
                                                blk->prev = t_blk->prev;
                                                blk->next = t_blk;
                                                blk->prev->next = blk;
                                                t_blk->prev = blk;
                                        }
                                        break;
                                    }
                        	} else {        /* at the bottom */
                                    blk->prev = b_que->last;
                                    blk->next = NULL;
                                    b_que->last->next = blk;
                                    b_que->last = blk;
                                    break;
                        	}
                        	t_blk = t_blk->next;  /* try next in chain */
                	}
        	}
		break;
	}
	return (0);
}

/*** comb_blk
 *
 * Combine the block blk and its buddies if possible. If all buddies are
 * free, remove all buddy blocks and combine them into a single block.
 *
 */

int comb_blk (blk)
struct block *blk;	/* pointer to the block being linked */
{
	int i;
	struct block *buddy_blk;
	struct block *parent_blk;

	if (blk == NULL) 
		return (-1);

	if (blk->no_combo == 1)
		return (0);

	/* all buddies are free ? */

	if ((parent_blk = blk->parent) == NULL)
		return (1);

	for (i = 0; i < parent_blk->child_cnt; i++) { 
		if ((buddy_blk = parent_blk->child[i]) == NULL) {
			printf ("I$comb_blk err: parent/child link error\n");
			return (-1);
		}
		if (buddy_blk->no_combo == 1)
			return (0);

		if (buddy_blk-> child_cnt == 0 &&
		buddy_blk->free_nodes == buddy_blk->width * buddy_blk->height) {
			continue;		/* buddy is free */
		} else {			/* buddy is not free */
			return (0);
		}
	}
	
	/* all buddies are free */

	for (i = 0; i < parent_blk->child_cnt; i++) {	/* rel all budddies */
		if (init_block (parent_blk->child[i]) < 0)
		    printf ("I$comb_blk: init_block problem\n");
		if (add_blk (parent_blk->child[i], &free_blk_que, LAST) < 0)
		    printf ("I$comb_blk: add_blk problem\n");
		parent_blk->child[i] = NULL;
	}
	parent_blk->child_cnt = 0;

	/* combine the parent block recusively */

	if (comb_blk (parent_blk) < 0) 
	    printf ("I$comb_blk: comb_blk problem\n");

	return (0);
}

/*** get_blk ()
 *
 * Get a block structure from the queue.
 * Input flag :
 *	TOP - From the top of the queue.
 *
 */
 
struct block *get_blk (b_que, flag)
struct blk_que *b_que;
int flag;
{
        struct block *temp_blk;
 
	switch (flag) {
	case TOP :
        	if (b_que->top == NULL) {
			temp_blk = NULL;
			break;
		}
        	temp_blk = b_que->top;
        	b_que->top = temp_blk->next;
        	if (temp_blk->next == NULL) {    /* no more blocks */
                	b_que->last = NULL;
        	} else {
                	temp_blk->next->prev = NULL;
        	}
		break;
	}
        return (temp_blk);
}

/*** srch_blk ()
 *
 * Search from top_blk and down for a block that is >= size.
 *
 */
 
struct block *srch_blk (top_blk, size, tj_req, srch_type)
struct block *top_blk;
long size;
struct job_req *tj_req;
int srch_type;
{
	int i;
	struct block *tmp_blk;		/* working block */
	struct block *opt_blk;		/* working block */
	struct block *t_blk;		/* working block */
	int order;
	int srch_order[MAX_BUD];

	if (top_blk->free_nodes < size)
		return (NULL);

	opt_blk = top_blk;

	if (top_blk->child_cnt > 0) {		/* have children */

		if (get_order (srch_order, top_blk, tj_req, srch_type) < 0)
			printf ("I$srch_blk: problem with get_order\n");
		for (i = 0; i < top_blk->child_cnt; i++) {
			order = srch_order[i];
			if (top_blk->child[order] == NULL) {
				printf ("I$srch_blk err: bad child\n");
				return (NULL);
			}
			if (top_blk->child[order]->free_nodes >= size) {

			    /* do a recursive search */
				
			    if ((tmp_blk = srch_blk 
			    (top_blk->child[order], size, tj_req, srch_type)) 
			    == NULL) {
			        printf ("I$srch_blk err: bad link\n");
			        return (NULL);
			    }
			    if (tmp_blk->free_nodes == size)
				return (tmp_blk);   /* done */
			    if (tmp_blk->free_nodes < opt_blk->free_nodes) {
				opt_blk = tmp_blk;
			    } else if (tmp_blk->free_nodes == 
			    opt_blk->free_nodes) {

				/* check if opt_blk is an ancester of tmp_blk */

				t_blk = tmp_blk;
				while (t_blk != NULL) {
				    if (t_blk == opt_blk) {
					opt_blk = tmp_blk;
					break;
				    }
				    t_blk = t_blk->parent;
				}
			    }
			}
		}
	}
	return (opt_blk);
}

/*** srch_child ()
 *
 * Search the children of top_blk and return the child with the largest 
 * free_nodes.
 *
 */
 
struct block *srch_child (top_blk, tj_req, srch_type, req_nodes)
struct block *top_blk;
struct job_req *tj_req;
int srch_type;
int req_nodes;
{
	int i;
	struct block *opt_child;		/* working block */
	long opt_size;
	int order;
	int srch_order[MAX_BUD];
	int free_nodes;

	if (top_blk->child_cnt == 0)
		return (NULL);

	opt_child = NULL;
	opt_size = 0;

	if (get_order (srch_order, top_blk, tj_req, srch_type) < 0)
		printf ("I$srch_blk: problem with get_order\n");
	for (i = 0; i < top_blk->child_cnt; i++) {
		order = srch_order[i];
		/* if a blk is already allocated to the job,
		 * will just take it */

		free_nodes = top_blk->child[order]->free_nodes;
		if (tj_req->blk_que.top != NULL && free_nodes > 0)
			return (top_blk->child[order]);

		if (free_nodes == req_nodes)
			return (top_blk->child[order]);

		if (free_nodes > req_nodes) {
		    if (opt_size < req_nodes ||
		    (opt_size > req_nodes && free_nodes < opt_size)) { 
			opt_size = free_nodes;
			opt_child = top_blk->child[order];
		    }
		} else if (free_nodes > opt_size) {
			opt_size = free_nodes;
			opt_child = top_blk->child[order];
		}
	}
	return (opt_child);
}

/*** alloc_blk ()
 *
 * Allocate blocks to a job request (tj_req).  
 * The number of nodes requested is given by req_nodes.
 * Search from top_blk and down to get blocks that total tj_req->req_nodes.
 * These blocks will be linked in the tj_req->blk_que chain.
 *
 */
 
int alloc_blk (tj_req, req_nodes, top_blk)
struct job_req *tj_req;
long req_nodes;			/* working request size */
struct block *top_blk;		/* top block */
{
	struct block *tmp_blk;		/* working block */
	struct block *child_blk;
	int i;
	int srch_inx;			/* starting index to search */
	int srch_type;

	tmp_blk = top_blk;

	if (top_blk->free_nodes <= 0)
		return (-1);
	if (req_nodes > top_blk->free_nodes) {/* not enough nodes */
		return (-1);
	} else if (req_nodes == top_blk->free_nodes) {
		if (free_to_job (tj_req, top_blk) < 0)
			printf ("I$alloc_blk: problem with free_to_job\n");
		return (0);
	}

	if (top_blk->child_cnt == 0) /* split the blk if never been split */
		if (split_blk (top_blk) < 0)
			printf ("I$alloc_blk: bad split_blk\n");

	    /* find the search type */

	if (req_nodes >= top_blk->child[0]->width * 
	top_blk->child[0]->height / SZ_FACTOR)
		srch_type = LARGE;
	else
		srch_type = SMALL;
	
	/* search for a block that is >= req_nodes */

	if (tj_req->blk_que.top == NULL) {	/* find first blk */
	    if ((tmp_blk = srch_blk (tmp_blk, req_nodes, tj_req, srch_type))
	    == NULL) {
		printf ("I$alloc_blk: Inconsistent block counts\n");
		return (-1);		/* something is wrong */
	    }
	}

	while (1) {

		if (tmp_blk->free_nodes == req_nodes) {	/* done */
			if (free_to_job (tj_req, tmp_blk) < 0)
			    printf ("I$alloc_blk: bad free_to_job\n");
			break;
		}

		if (tmp_blk->child_cnt > 0) {		/* have children */
			if ((child_blk = srch_child 
			(tmp_blk, tj_req, srch_type, req_nodes)) == NULL) {
				printf ("I$alloc_blk err: No more children\n");
				return (-1);
			}
			if (child_blk->free_nodes >= req_nodes) {
			    tmp_blk = child_blk;
			    continue;
			}
			req_nodes -= child_blk->free_nodes;
			if (free_to_job (tj_req, child_blk) < 0)
			    printf ("I$alloc_blk: bad free_to_job\n");
			continue;
		} else {
			if (split_blk (tmp_blk) < 0)	/* split the block */
				printf ("I$alloc_blk: bad alloc_blk\n");
		}
	}
	return (0);
}
		

/*** free_to_job ()
 *
 * Queue all free blocks start at top_blk and down to tj_req->blk_que.
 *
 */
 
int free_to_job (tj_req, top_blk)
struct job_req *tj_req;
struct block *top_blk;
{
	struct block *tmp_blk;		/* working block */
	int i;
	long size;

	if (tj_req == NULL || top_blk == NULL)
		return (-1);

	if (top_blk->child_cnt == 0) {		/* no descendant */
		if (add_blk (top_blk, &tj_req->blk_que, SIZE) < 0)
		    printf ("free_to_job: add_blk problem\n");
		size = top_blk->free_nodes;
		if (adj_count (top_blk, -size) < 0) 
	    		printf ("I$free_to_job: adj_count problem\n");
	} else {
		for (i = 0; i < top_blk->child_cnt; i++) {

			/* queue the children recursively */

			if ((tmp_blk = top_blk->child[i]) == NULL) {
				printf ("I$fee_to_job err: Bad child\n");
				return (-1);
			}
			if (tmp_blk->free_nodes > 0) 
				if (free_to_job (tj_req, tmp_blk) < 0)
				    printf ("I$fee_to_job: bad recursive \n");
		}
	}
	return (0);
}
		
/*** adj_count ()
 *
 * adjust the free_nodes of bot_blk and up by the size value.
 *
 */
 
int adj_count (bot_blk, size)
struct block *bot_blk;
long size;
{
	struct block *tmp_blk;		/* working block */

	if (bot_blk == NULL)
		return (-1);

	tmp_blk = bot_blk;
	while (tmp_blk != NULL) {
		tmp_blk->free_nodes += size;
		tmp_blk = tmp_blk->parent;
	}
	return (0);
}

/*** split_blk ()
 *
 * Split the top_blk using the modified 2-D buddy system. 
 *
 */
 
int split_blk (top_blk)
struct block *top_blk;
{
	struct block *tmp_blk;		/* working block */
	long h2;			/* the largest power of 2 height */
	long w2;			/* the largest power of 2 width */
	long x[MAX_BUD];		/* x-cord of buddies */
	long y[MAX_BUD];		/* y-cord of buddies */
	long w[MAX_BUD];		/* width of buddy */
	long h[MAX_BUD];		/* height of buddy */
	int buddy_cnt;			/* number of buddy */
	int i;

	if (top_blk->height == 1 && top_blk->width == 1) {   /* can't split */
		top_blk->child_cnt = 0;
		return (0);
	}

	h2 = power2 (top_blk->height);
	w2 = power2 (top_blk->width);

	if (h2 == top_blk->height && w2 == top_blk->width) {
		if (h2 == w2) {
			x[0] = top_blk->x;
			y[0] = top_blk->y;
			x[1] = top_blk->x + w2 / 2;
			y[1] = top_blk->y;
			x[2] = top_blk->x + w2 / 2;
			y[2] = top_blk->y + h2 / 2;
			x[3] = top_blk->x;
			y[3] = top_blk->y + h2 / 2;
			w[0] = w[1] = w[2] = w[3] = w2 / 2;
			h[0] = h[1] = h[2] = h[3] = h2 / 2;
			buddy_cnt = 4;
		} else if (h2 > w2) {
			x[0] = top_blk->x;
			y[0] = top_blk->y;
			x[1] = top_blk->x;
			y[1] = top_blk->y + h2 / 2;
			w[0] = w[1] = w2;
			h[0] = h[1] = h2 / 2;
			buddy_cnt = 2;
			
		} else {
			x[0] = top_blk->x;
			y[0] = top_blk->y;
			x[1] = top_blk->x + w2 / 2;
			y[1] = top_blk->y;
			w[0] = w[1] = w2 / 2;
			h[0] = h[1] = h2;
			buddy_cnt = 2;
		}
	} else if ( h2 == top_blk->height && w2 != top_blk->width) {
		x[0] = top_blk->x;
		y[0] = top_blk->y;
		x[1] = top_blk->x + w2;
		y[1] = top_blk->y;
		w[0] =  w2 ;
		w[1] = top_blk->width - w2;
		h[0] = h[1] = h2;
		buddy_cnt = 2;
	} else if ( h2 != top_blk->height && w2 == top_blk->width) {
		x[0] = top_blk->x;
		y[0] = top_blk->y;
		x[1] = top_blk->x;
		y[1] = top_blk->y + h2;
		w[0] = w[1] = w2;
		h[0] = h2;
		h[1] = top_blk->height - h2;
		buddy_cnt = 2;
	} else {
		x[0] = top_blk->x;
		y[0] = top_blk->y;
		x[1] = top_blk->x + w2;
		y[1] = top_blk->y;
		x[2] = top_blk->x + w2;
		y[2] = top_blk->y + h2;
		x[3] = top_blk->x;
		y[3] = top_blk->y + h2;
		w[0] = w[3] = w2;
		h[0] = h [1] = h2;
		w[1] = w[2] = top_blk->width - w2;
		h[2] = h[3] = top_blk->height - h2;
		buddy_cnt = 4;
	}

	/* now link the blocks */

	top_blk->child_cnt = buddy_cnt;

	for (i = 0; i < buddy_cnt; i++) {
		if ((tmp_blk = get_blk (&free_blk_que, TOP)) == NULL) {
			printf ("I$split_blk: Out of free blocks\n");
			return (-1);
		}
		if (init_block (tmp_blk) < 0)
		    printf ("I$split_blk: init_block problem\n");
		tmp_blk->next = NULL;
		tmp_blk->prev = NULL;
		tmp_blk->x = x[i];
		tmp_blk->y = y[i];
		tmp_blk->width = w[i];
		tmp_blk->height = h[i];
		tmp_blk->parent = top_blk;
		if (top_blk->free_nodes > 0)
			tmp_blk->free_nodes = w[i] * h[i];
		tmp_blk->node_set = top_blk->node_set;
		tmp_blk->layer_inx = top_blk->layer_inx;
		top_blk->child[i] = tmp_blk;
	}
	return (0);
}

/*** prt_tree ()
 *
 * Print the 2-D buddy tree starting at top_blk.  
 *
 */
 
void prt_tree (top_blk)
struct block *top_blk;
{
	int i;

	/* print the parent */

	printf ("\nblock index: %d     free_nodes: %d\n\n", 
	bindex (top_blk), top_blk->free_nodes);

	printf ("I$Children count : %d\n\n", top_blk->child_cnt);
	if (top_blk->child_cnt == 2) {
	    printf ("bindex: %6d          %6d\n", 
	    bindex (top_blk->child[0]),
	    bindex (top_blk->child[1]));
	    printf ("fnodes: %6d          %6d\n", 
	    top_blk->child[0]->free_nodes,
	    top_blk->child[1]->free_nodes);
	    printf ("     x: %6d          %6d\n", 
	    top_blk->child[0]->x,
	    top_blk->child[1]->x);
	    printf ("     y: %6d          %6d\n", 
	    top_blk->child[0]->y,
	    top_blk->child[1]->y);
	    printf ("     w: %6d          %6d\n", 
	    top_blk->child[0]->width,
	    top_blk->child[1]->width);
	    printf ("     h: %6d          %6d\n", 
	    top_blk->child[0]->height,
	    top_blk->child[1]->height);
	    printf ("nchild: %6d          %6d\n", 
	    top_blk->child[0]->child_cnt,
	    top_blk->child[1]->child_cnt);
	    printf ("child0: %6d          %6d\n", 
	    bindex (top_blk->child[0]->child[0]),
	    bindex (top_blk->child[1]->child[0]));
	    printf ("child1: %6d          %6d\n", 
	    bindex (top_blk->child[0]->child[1]),
	    bindex (top_blk->child[1]->child[1]));
	    printf ("child2: %6d          %6d\n", 
	    bindex (top_blk->child[0]->child[2]),
	    bindex (top_blk->child[1]->child[2]));
	    printf ("child3: %6d          %6d\n", 
	    bindex (top_blk->child[0]->child[3]),
	    bindex (top_blk->child[1]->child[3]));
	    printf ("parent: %6d          %6d\n", 
	    bindex (top_blk->child[0]->parent),
	    bindex (top_blk->child[1]->parent));
	} else if (top_blk->child_cnt == 4) {
	    printf ("bindex: %6d          %6d           %6d           %6d\n", 
	    bindex (top_blk->child[0]),
	    bindex (top_blk->child[1]),
	    bindex (top_blk->child[2]),
	    bindex (top_blk->child[3]));
	    printf ("fnodes: %6d          %6d           %6d           %6d\n", 
	    top_blk->child[0]->free_nodes,
	    top_blk->child[1]->free_nodes,
	    top_blk->child[2]->free_nodes,
	    top_blk->child[3]->free_nodes);
	    printf ("     x: %6d          %6d           %6d           %6d\n", 
	    top_blk->child[0]->x,
	    top_blk->child[1]->x,
	    top_blk->child[2]->x,
	    top_blk->child[3]->x);
	    printf ("     y: %6d          %6d           %6d           %6d\n", 
	    top_blk->child[0]->y,
	    top_blk->child[1]->y,
	    top_blk->child[2]->y,
	    top_blk->child[3]->y);
	    printf ("     w: %6d          %6d           %6d           %6d\n", 
	    top_blk->child[0]->width,
	    top_blk->child[1]->width,
	    top_blk->child[2]->width,
	    top_blk->child[3]->width);
	    printf ("     h: %6d          %6d           %6d           %6d\n", 
	    top_blk->child[0]->height,
	    top_blk->child[1]->height,
	    top_blk->child[2]->height,
	    top_blk->child[3]->height);
	    printf ("nchild: %6d          %6d           %6d           %6d\n", 
	    top_blk->child[0]->child_cnt,
	    top_blk->child[1]->child_cnt,
	    top_blk->child[2]->child_cnt,
	    top_blk->child[3]->child_cnt);
	    printf ("child0: %6d          %6d           %6d           %6d\n", 
	    bindex (top_blk->child[0]->child[0]),
	    bindex (top_blk->child[1]->child[0]),
	    bindex (top_blk->child[2]->child[0]),
	    bindex (top_blk->child[3]->child[0]));
	    printf ("child1: %6d          %6d           %6d           %6d\n", 
	    bindex (top_blk->child[0]->child[1]),
	    bindex (top_blk->child[1]->child[1]),
	    bindex (top_blk->child[2]->child[1]),
	    bindex (top_blk->child[3]->child[1]));
	    printf ("child2: %6d          %6d           %6d           %6d\n", 
	    bindex (top_blk->child[0]->child[2]),
	    bindex (top_blk->child[1]->child[2]),
	    bindex (top_blk->child[2]->child[2]),
	    bindex (top_blk->child[3]->child[2]));
	    printf ("child3: %6d          %6d           %6d           %6d\n", 
	    bindex (top_blk->child[0]->child[3]),
	    bindex (top_blk->child[1]->child[3]),
	    bindex (top_blk->child[2]->child[3]),
	    bindex (top_blk->child[3]->child[3]));
	    printf ("parent: %6d          %6d           %6d           %6d\n", 
	    bindex (top_blk->child[0]->parent),
	    bindex (top_blk->child[1]->parent),
	    bindex (top_blk->child[2]->parent),
	    bindex (top_blk->child[3]->parent));
	} else {
		return;
	}

	for (i = 0; i < top_blk->child_cnt; i++) {
		if (top_blk->child[i]->child_cnt > 0)
			prt_tree (top_blk->child[i]);
	}
	return;
}

/*** bindex ()
 *
 * given a block pointer, return the index into the block array..  
 *
 */
 
int bindex (blk)
struct block *blk;
{
	if (blk == NULL) 
		return (-1);

	return (((int) blk - (int) &block[0]) / sizeof (struct block));
}

/*** get_order ()
 *
 * Get the search order.
 *
 */
 
int get_order (srch_order, top_blk, tj_req, srch_type)
int srch_order[MAX_BUD];
struct block *top_blk;
struct job_req *tj_req;
int srch_type;
{
	int i, j;
	int order;
	int distance[MAX_BUD];
	struct block *alloc_blk;
	long tmp;

	if (top_blk == NULL) 
		return (-1);
	
	if (tj_req == NULL) 
		return (-1);

	if (top_blk->child_cnt <= 0)
		return (-1);


	/* Check if there is already at least one block allocated to
	 * the request.
	 */

	alloc_blk = tj_req->blk_que.top;
	if (alloc_blk == NULL) {	/* Nothing */
		if (srch_type == LARGE)	{	/* in sequence */
		    for (i = 0; i < top_blk->child_cnt; i++) { 
			if (def_order[i] > top_blk->child_cnt - 1) 
			    srch_order[i] = top_blk->child_cnt - 1;
			else
			    srch_order[i] = def_order[i];
		    }
		} else {	/* reverse of search order */
		    for (i = 0; i < top_blk->child_cnt; i++) { 
			order = def_order[top_blk->child_cnt - 1 - i];
			if (order > top_blk->child_cnt - 1)
			    srch_order[i] = top_blk->child_cnt - 1;
			else
			    srch_order[i] = order;
		    }
		}
	} else {	/* sort by distance to the first allocated blk */

		for (i = 0; i < top_blk->child_cnt; i++) {
		    distance[i] = dist (alloc_blk, top_blk->child[i]);
		    srch_order[i] = i;
		}

		/* Sort by increasing order of distance using a simple
		 * bubble search.
		 */

		for (i = 1; i < top_blk->child_cnt; ++i) {
		    for (j = top_blk->child_cnt - 1; j >= i; --j) {
			if (distance[j - 1] > distance[j]) {
			    tmp = distance[j - 1];
			    distance[j - 1] = distance[j];
			    distance[j] = tmp;
			    tmp = srch_order[j - 1];
			    srch_order[j - 1] = srch_order[j];
			    srch_order[j] = tmp;
			}
		    }
		}
	}
	return (0);
}

/*** dist ()
 *
 * calculate the distance between 2 blooks.
 *
 */
 
int dist (blk1, blk2)
struct block *blk1;
struct block *blk2;
{
	long min_x1;
	long min_y1;
	long max_x1;
	long max_y1;
	long min_x2;
	long min_y2;
	long max_x2;
	long max_y2;
	long tot_dist;
	long tmp_x;
	long tmp_y;

	if (blk1 == NULL || blk2 == NULL) 
		return (9999999);

	min_x1 = blk1->x;
	min_y1 = blk1->y;
	max_x1 = min_x1 + blk1->width - 1;
	max_y1 = min_y1 + blk1->height - 1;
	min_x2 = blk2->x;
	min_y2 = blk2->y;
	max_x2 = min_x2 + blk2->width - 1;
	max_y2 = min_y2 + blk2->height - 1;

	tot_dist = 0;
	tmp_x = min_x1 - min_x2;
	tmp_y = min_y1 - min_y2;
	tot_dist += tmp_x * tmp_x + tmp_y * tmp_y;
	tmp_x = max_x1 - max_x2;
	tmp_y = min_y1 - min_y2;
	tot_dist += tmp_x * tmp_x + tmp_y * tmp_y;
	tmp_x = max_x1 - max_x2;
	tmp_y = max_y1 - max_y2;
	tot_dist += tmp_x * tmp_x + tmp_y * tmp_y;
	tmp_x = min_x1 - min_x2;
	tmp_y = max_y1 - max_y2;
	tot_dist += tmp_x * tmp_x + tmp_y * tmp_y;

	return (tot_dist);
}

	
/*** mv_all_blk ()
 *
 * Move all blocks from from_job to to_job.
 *
 */
 
int mv_all_blk (from_job, to_job)
struct job_req *from_job;
struct job_req *to_job;
{
	struct block *tmp_blk;

	if (from_job == NULL || to_job == NULL)
		return (-1);

	while ((tmp_blk = get_blk (&from_job->blk_que, TOP)) != NULL) 
		if (add_blk (tmp_blk, &to_job->blk_que, SIZE) < 0)
			printf ("I$mv_all_blk: add_blk problem\n"); 
	return (0);
}

/*** rem_blk
 *
 * Remove a block from a block queue
 *
 */

int rem_blk (blk, b_que)
struct block *blk;          /* pointer to the blk being removed */
struct blk_que *b_que;     /* pointer to the b_que from which blk is removed */
{
	if (b_que == NULL || blk == NULL)
		return (-1);

        if (blk->prev) {                /* not top of the b_que */
                blk->prev->next = blk->next;
        } else {                          /* top of the b_que */
                b_que->top = blk->next;
                if (b_que->top)           /* at least 1 element in b_que */
                        b_que->top->prev = NULL;
        }
        if (blk->next)                  /* net the last element */
                blk->next->prev = blk->prev;
        else
                b_que->last = blk->prev;
 
        blk->next = blk->prev = NULL;
	return (0);
}

/*** get_best_chd (top_blk, req_size)
 *
 * Get the smallest child block that is >= neq_nodes.
 *
 */
struct block *get_best_chd (top_blk, req_size)
struct block *top_blk;
int req_size;
{
	int i;
	struct block *best_blk;
	struct block *child_blk;
	long best_sz;
	long sz;
	long order;

	best_blk = NULL;
	best_sz = 0;
	for (i = 0; i < top_blk->child_cnt; i++) {
	    	order = def_order[i];
		child_blk = top_blk->child[order];
		sz = child_blk->height * child_blk->width;
		if (sz >= req_size) {
			if (best_blk == NULL || sz < best_sz) {
				best_blk = child_blk;
				best_sz = sz;
			}
		}
	}
	return (best_blk);
}

/*** disp_blk ()
 *
 * Display all blocks assigned to a job.
 *
 */
 
int disp_blk (j_req)
struct job_req *j_req;
{
	struct job_req *ts_req;
	struct block *t_blk;

	if (j_req == NULL)
		return (-1);

	if ((j_req->nqs_req->start_time > 0) &&
        (j_req->nqs_req->queue->q.priority >= TSCHED_PRI)) {
	    printf ("I$Scheduling TSCHED_req: %d\n", j_req->orig_seqno);
	} else {
	    printf ("I$Scheduling request: %d\n", j_req->orig_seqno);
	}

	/* go through its own blk_que */

	t_blk = j_req->blk_que.top;
	while (t_blk != NULL) {
	    printf ("I$x: %5d    y: %5d    w: %5d    h: %5d\n", 
	    t_blk->x, t_blk->y, t_blk->width, t_blk->height); 
	    t_blk = t_blk->next;
	}
	return (0);
}

/*** free_blk_cnt ()
 *
 * Count the number of blocks in the free_blk_que.
 *
 */
 
void free_blk_cnt ()
{
	struct block *blk;
	int blk_cnt;

	blk_cnt = 0;
	blk = free_blk_que.top;
	while (blk != NULL) {
		blk_cnt ++;
		blk = blk->next;
	}
	printf ("I$Number of free block struct = %d\n", blk_cnt);
}

/*** match_blk ()
 *
 * get a block in root_blk that will match (x, y, height and width) the blk
 * specification. Return a NULL if no match.
 *
 */
 
struct block *match_blk (blk, root_blk)
struct block *blk;
struct block *root_blk;
{
        struct block *tmp_blk;
	int size, i;
        int max_x, min_x, max_y, min_y;
        int max_x1, min_x1, max_y1, min_y1;

	
	if (blk == NULL) {
            printf ("I$match_blk: Problem with blk input \n");
            return (NULL);
        }

	if (root_blk == NULL) {
            printf ("I$match_blk: Problem with root_blk input \n");
            return (NULL);
        }

        tmp_blk = root_blk;

	size = blk->height * blk->width;
        min_x = blk->x;
        min_y = blk->y;
        max_x = min_x + blk->width - 1;
        max_y = min_y + blk->height - 1;

	while (1) {
	    if (tmp_blk->free_nodes < size) {
		comb_blk (tmp_blk);
		return (NULL);
	    }
            min_x1 = tmp_blk->x;
            min_y1 = tmp_blk->y;
            max_x1 = min_x1 + tmp_blk->width - 1;
            max_y1 = min_y1 + tmp_blk->height - 1;
	
	    /* see if blk is in range ? */

	    if (min_x < min_x1 || max_x > max_x1 || min_y < min_y1 
	    || max_y > max_y1) {	/* out of range */
		comb_blk (tmp_blk);
		return (NULL);
	    }

	    if (min_x == min_x1 && max_x == max_x1 && min_y == min_y1
	    && max_y == max_y1 && tmp_blk->free_nodes == size) {	

	    	/* This is what we want */

		return (tmp_blk);

	    }

            if (tmp_blk->child_cnt > 0) {           /* have children */
        	for (i = 0; i < tmp_blk->child_cnt; i++) {
            	    min_x1 = tmp_blk->child[i]->x;
            	    min_y1 = tmp_blk->child[i]->y;
            	    max_x1 = min_x1 + tmp_blk->child[i]->width - 1;
            	    max_y1 = min_y1 + tmp_blk->child[i]->height - 1;
	
	    	    /* see if blk is in range ? */

	    	    if (min_x < min_x1 || max_x > max_x1 || min_y < min_y1 
	    	    || max_y > max_y1) {	/* out of range */
			continue;
		    } else { 			/* in range */
			break;
		    }
		}
		if (i >= tmp_blk->child_cnt) {	/* out of range */
		    comb_blk (tmp_blk);
		    return (NULL);
		} else {
		    tmp_blk = tmp_blk->child[i];
		}
            } else {
                if (split_blk (tmp_blk) < 0) {    /* split the block */
                        printf ("I$match_blk: bad split_blk\n");
		    	comb_blk (tmp_blk);
		    	return (NULL);
		}
            }
	}
}
#endif
