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
/* macs_job.c
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/macs_job.c,v $
 *
 * DESCRIPTION:
 *
 *	The module contains a number of general subroutines for managing 
 *	 the job_req structure of the buddy system. 
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
 * $Log: macs_job.c,v $
 * Revision 1.6  1994/11/19  02:52:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/04/06  21:38:14  mwan
 * Update R1_3 to R1_2 WW15
 *
 *  Reviewer: kremenek
 *  Risk: M
 *  Benefit or PTS #: 7738,8087,8346, 8325,8599,8576,8600,8601,8088,8597,8876,8886
 *  Testing:
 *  Module(s):  macs_blk.c macs_job.c macs_lib.c macs_rootp.c macs_sched.c
 * 	     nqs_spawn.c res_msg.c smd_msg.c
 *
 * Revision 1.4  1993/11/02  00:56:40  mwan
 * R1.2 mods
 *
 * Revision 1.3  1993/07/13  17:52:24  mwan
 * T11 - fixed PTS 5022
 *
 * Revision 1.2  1992/10/09  22:24:28  mwan
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
 
/* External function */

extern int init_block ();
extern int rel_blk ();
extern int add_blk ();
extern int comb_blk ();
extern struct block *get_blk ();
extern int alloc_blk ();
extern int alloc_blk_ts ();
extern struct block *srch_blk ();
extern struct block *srch_child ();
extern int free_to_job ();
extern int adj_count ();
extern int split_blk ();
extern void prt_tree ();
extern int bindex ();
extern int get_order ();
extern int mv_blk ();
extern int mv_all_blk ();
extern struct block *getblk_by_rootp ();
extern int rem_blk ();
extern struct block *get_best_chd ();
extern void init_rootp ();
extern int chk_nodef ();
extern int phynode_to_xy ();
extern int xy_to_phynode ();
extern struct root *alloc_rt ();
extern struct root *alloc_rt_ts ();
extern struct root *find_inuse_p ();
extern int disp_blk ();
extern int mai_send ();
extern int dist ();
extern struct block *match_blk ();

int init_job_req ();
void init_que ();
int add_job ();
int rel_blkinjob ();
struct job_req *get_job ();
void prt_job ();
int rem_job ();
int rem_job_ts ();
struct job_req *find_job_by_name ();
void release_all ();
struct job_req *find_job ();
int end_job ();
void free_job_cnt ();
struct job_req *match_req ();
int rel_jobreq ();

/*** init_que ()
 *
 * initialize the queue structure.
 *
 */
void init_que ()
{
        int i;
 
        /* initialize all queues */
 
        free_blk_que.top = NULL;
        free_blk_que.last = NULL;
        free_job_que.top = NULL;
        free_job_que.last = NULL;
           
        /* initialize the free job request queue  */
 
        for (i = 0; i < MAX_JOB; i++ ) {
                job_req[i].prev = NULL;
                job_req[i].next = NULL;
                if (init_job_req (&job_req[i]) < 0)
		    printf ("I$init_que: bad init_job_req\n");
                if (add_job (&job_req[i], &free_job_que, LAST) < 0)
		    printf ("I$init_que: problem with add_job\n");
        }
 
        /* initialize the free block queue */
 
        for (i = 0; i < MAX_BLOCK; i++ ) {
                block[i].prev = NULL;
                block[i].next = NULL;
                if (init_block (&block[i]) < 0)
		    printf ("I$init_que: problem with init_block\n");
                if (add_blk (&block[i], &free_blk_que, LAST) < 0)
		    printf ("I$init_que: problem with add_blk\n");
        }
 
        /* Initialize the wait_que */
           
	for (i = 0; i < MAX_LAYER; i++) {
        	layer[i].wait_que.req_nodes = 0;
        	layer[i].wait_que.top = NULL;
        	layer[i].wait_que.last = NULL;
	}
}

/*** init_job_req ()
 *
 * initialize a job_req structure.
 *
 */
int init_job_req (j_req)
struct job_req *j_req;       /* pointer to the job_req being linked */
{
	if (j_req == NULL) 
		return (-1);

	j_req->req_nodes = 0;
	j_req->req_time = 0;
	j_req->prior = 0;
	j_req->start_time = 0;
	j_req->jobname = 0;
	j_req->run_flag = 0;
	j_req->uid = 0;
	j_req->acct_id = 0;
	j_req->orig_mid = 0;
	j_req->orig_seqno = 0;
	j_req->part_id = 0;
	j_req->period_inx = 0;
	j_req->nqs_req = NULL;
	j_req->username[0] = '\0';
	j_req->part_name[0] = '\0';
	j_req->que_name[0] = '\0';
	j_req->blk_que.top = NULL;
	j_req->blk_que.last = NULL;
	j_req->prev = NULL;
	j_req->next = NULL;
	return (0);
}

/*** add_job
 *
 * add a request (j_req) to the the queue (queue).
 * input flag :
 *	LAST : add to the end.
 *	TIME_PRI : in decreasing order of time * prior.
 *	SIZE : in decreasing order of size.
 *	PRI :  in decreasing order of priority.
 *	ST_TIME : in increasing order of start_time.
 *
 */

int add_job (j_req, j_que, flag)
struct job_req *j_req;       /* pointer to the job_req being linked */
struct job_que *j_que;         /* pointer to the queue to link to */
int flag;
{
	float fval;
	float ftest_val;
	int val;
	int test_val;
	struct job_req *tj_req;
	int decreasing;		/* decreasing order */

	if (j_req == NULL || j_que == NULL) 
		return (-1);

	switch (flag) {

	case LAST :		/* add to the end of the queue */
	    if (j_que->top == NULL) {          /* first item */
		j_que->top = j_req;
	    } else {
		j_que->last->next = j_req;
	    }
	    j_req->next = NULL;                /* end of queue */
	    j_req->prev = j_que->last;        
	    j_que->last = j_req;
	    break;

	case SIZE :		/* in decreasing order of req_nodes */
	case ST_TIME:
	    if (flag == SIZE) {	
	        val = j_req->req_nodes;
		decreasing = 1;
	    } else if (flag == ST_TIME) {
		val = j_req->start_time;
		decreasing = 0;
	    } else {
		return (-1);
	    }

            if (j_que->top == NULL) {          /* first item */
               	j_que->top = j_req;
               	j_que->last = j_req;
               	j_req->next = NULL;
               	j_req->prev = NULL;
            } else {
               	tj_req = j_que->top;
               	while (1) {
               	    if (tj_req != NULL) {
			if (flag == SIZE)
			    test_val = tj_req->req_nodes;
	    		else if (flag == ST_TIME) 
			    test_val = tj_req->start_time;
			else
			    return (-1);

                        if (( decreasing > 0 && test_val < val) ||
			(decreasing == 0 && test_val > val)) {
                            if (tj_req == j_que->top) {  /* top */
                                j_que->top = j_req;
                                j_req->prev = NULL;
                                j_req->next = tj_req;
                                tj_req->prev = j_req;
                            } else {    /* in the middle */
                                j_req->prev = tj_req->prev;
                                j_req->next = tj_req;
                                j_req->prev->next = j_req;
                                tj_req->prev = j_req;
                            }
                            break;
                        }
                    } else {        /* at the bottom */
                         j_req->prev = j_que->last;
                         j_req->next = NULL;
                         j_que->last->next = j_req;
                         j_que->last = j_req;
                         break;
                    }
                    tj_req = tj_req->next;  /* try next in chain */
                }
       	    }
	    break;
	case TIME_PRI :		/* in decreasing order of req_time * prior */
	case PRI :		/* in decreasing order of PRI */
            if (flag == TIME_PRI) {
		fval = j_req->req_time * j_req->prior;
		decreasing = 1;
	    } else if (flag == PRI) {	
	        fval = j_req->prior;
		decreasing = 1;
	    } else {
		return (-1);
	    }

            if (j_que->top == NULL) {          /* first item */
               	j_que->top = j_req;
               	j_que->last = j_req;
               	j_req->next = NULL;
               	j_req->prev = NULL;
            } else {
               	tj_req = j_que->top;
               	while (1) {
               	    if (tj_req != NULL) {
			if (flag == TIME_PRI)
			    ftest_val = tj_req->req_time * tj_req->prior;
			else if (flag == PRI)
			    ftest_val = tj_req->prior;
			else
			    return (-1);

                        if (( decreasing > 0 && ftest_val < fval) ||
			(decreasing == 0 && ftest_val > fval)) {
                            if (tj_req == j_que->top) {  /* top */
                                j_que->top = j_req;
                                j_req->prev = NULL;
                                j_req->next = tj_req;
                                tj_req->prev = j_req;
                            } else {    /* in the middle */
                                j_req->prev = tj_req->prev;
                                j_req->next = tj_req;
                                j_req->prev->next = j_req;
                                tj_req->prev = j_req;
                            }
                            break;
                        }
                    } else {        /* at the bottom */
                         j_req->prev = j_que->last;
                         j_req->next = NULL;
                         j_que->last->next = j_req;
                         j_que->last = j_req;
                         break;
                    }
                    tj_req = tj_req->next;  /* try next in chain */
                }
       	    }
	    break;
	}
	return (0);
}


/*** rel_blkinjob
 *
 * Release the job j_req. Go through the blk_que and release all blocks
 * assigned to the job.
 *
 */

int rel_blkinjob (j_req)
struct job_req *j_req;       /* pointer to the job_req being linked */
{
	struct block *t_blk;

	if (j_req == NULL)
		return (-1);

	/* Release all blocks assigned to the job */
 
	while ((t_blk = get_blk (&j_req->blk_que, TOP)) != NULL) {
		if (rel_blk (t_blk) < 0)
		    printf ("I$rel_blkinjob: problem with rel_blk\n");
	}
	return (0);
}

/*** get_job ()
 *
 * Get a job structure from the queue.
 * Input flag :
 *	TOP - From the top of the queue.
 *
 */
 
struct job_req *get_job (j_que, flag)
struct job_que *j_que;
int flag;
{
        struct job_req *temp_job;
 
	switch (flag) {
	case TOP :
        	if (j_que->top == NULL) {
			temp_job = NULL;
			break;
		}
        	temp_job = j_que->top;
        	j_que->top = temp_job->next;
        	if (temp_job->next == NULL) {    /* no more job */
                	j_que->last = NULL;
        	} else {
                	temp_job->next->prev = NULL;
        	}
		break;
	}
        return (temp_job);
}

/*** prt_job ()
 *
 * Print all jobs in the inuse_que.  
 *
 */
 
void prt_job ()
{
	struct job_req *tj_req;
	struct block *t_blk;
	int i, j;

        for (i = 0; i < MAX_LAYER; i++) {
            for (j = 0; j < MAX_PERIOD + 1; j++) {
	    	printf (" Layer[%d], Period[j]\n\n", i, j);
	    	tj_req = layer[i].inuse_que[j].top;
	    	while (tj_req != NULL) {
		    printf ("\nRequest nodes: %d\n\n", tj_req->req_nodes);
		    t_blk = tj_req->blk_que.top;
		    while (t_blk != NULL) {
			printf ("bindex: %d, nodes: %d\n",
			bindex (t_blk), 
			t_blk->width * t_blk->height);
			t_blk = t_blk->next;
		    }
		    tj_req = tj_req->next;
		}
	    }
	}
}

/*** rem_job
 *
 * Remove a request (j_req) from a queue (queue)
 *
 */
 
int rem_job (j_req, queue)
struct job_req *j_req;      /* pointer to the job_req being removed */
struct job_que *queue;     /* pointer to the queue from which job_req */
                            /* is removed */
 
{
	if (j_req == NULL || queue == NULL)
		return (-1);

        if (j_req->prev) {                /* not top of the queue */
                j_req->prev->next = j_req->next;
        } else {                          /* top of the queue */
                queue->top = j_req->next;
                if (queue->top)           /* at least 1 element in queue */
                        queue->top->prev = NULL;
        }
        if (j_req->next)                  /* net the last element */
                j_req->next->prev = j_req->prev;
        else
                queue->last = j_req->prev;
 
        j_req->next = j_req->prev = NULL;
	
	return (0);
}

/*** find_job_by_name ()
 *
 * find a job :
 * flag :
 *	SIZE - find a job that is <= req_nodes.
 *
 */
 
struct job_req *find_job_by_name (jobname)
int jobname;
{
	int i, j;
        struct job_req *tj_req;


        for (i = 0; i < MAX_LAYER; i++) {
            for (j = 0; j < MAX_PERIOD + 1; j++) {

        	tj_req = layer[i].inuse_que[j].top;
        	while (tj_req != NULL) {
                	if (tj_req->jobname == jobname)
                        	return (tj_req);
                	tj_req = tj_req->next;
        	}
	    }
	}
        return (NULL);
}

/*** find_job ()
 *
 * find a job :
 * flag :
 *	SIZE - find a job that is <= req_nodes.
 *
 */
 
struct job_req *find_job (queue, req_nodes, flag)
struct job_que *queue;     /* pointer to the queue from which job_req */
long req_nodes;
int flag;
{
        struct job_req *tj_req;
 
        tj_req = queue->top;
	switch (flag) {
	case SIZE :
        	while (tj_req != NULL) {
			if (tj_req->req_nodes <= req_nodes)
                        	break;
                	tj_req = tj_req->next;
        	}
		break;
	}
        return (tj_req);
}


/*** release_all ()
 *
 * Release all jobs.
 *
 */
 
void release_all ()
{
	int i, j;
        struct job_req *tj_req;
 
	for (i = 0; i < MAX_LAYER; i++) {
	    for (j = 0; j < MAX_PERIOD + 1; j++) {
        	while ((tj_req = layer[i].inuse_que[j].top) != NULL) {
                        if (rel_jobreq (tj_req) < 0)
			    printf ("I$rlease_all: bad rel_jobreq\n");
        	}
	    }
	}
}


/*** match_req ()
 *
 * Subroutine to match the input NQS request struct with job_req queued in 
 * the timesh_que or the inuse_que. If there is a match, return the pointer 
 * of the matching job_req struct.
 *
 */

struct job_req *match_req (request)
struct request *request;               /* request to search */
{
        struct job_req *j_req;
	struct job_que *queue;      /* queue to search */
	int i, j;

	for (i = 0; i < MAX_LAYER; i++) {
	    for (j = 0; j < MAX_PERIOD + 1; j++) {

	    	/* try the inuse_que first */

            	j_req = layer[i].inuse_que[j].top;
            	while (j_req != NULL) {
                    if (j_req->nqs_req == request &&
                    j_req->uid == request->v1.req.uid &&
                    j_req->orig_mid == request->v1.req.orig_mid &&
                    j_req->orig_seqno == request->v1.req.orig_seqno)
        	    	return (j_req);
                    j_req = j_req->next;
	        }
	    }
        }
        return (NULL);
}

/*** end_job ()
 *
 * Subroutine to match the input NQS request struct with job_req queued in 
 * the timesh_que or the inuse_que. If there is a match, return the pointer 
 * of the matching job_req struct.
 *
 */

int end_job (request)
struct request *request;               /* request to search */

{
        struct job_req *j_req;

	if (request == NULL)
		return (-1);

	if ((j_req = match_req (request)) == NULL) {
		printf ("I$end_job: no match for request\n");
		return (-1);
	} 

	if (rel_jobreq (j_req) < 0)
	    printf ("I$end_job: Problem with rel_jobreq\n");
	return (0);
}

/*** free_job_cnt ()
 *
 * Count the number of job_req in the free_job_que. 
 *
 */
 
void free_job_cnt ()
{
        struct job_req *j_req;
        int job_cnt; 
 
        job_cnt = 0; 
        j_req = free_job_que.top; 
        while (j_req != NULL) { 
                job_cnt ++; 
                j_req = j_req->next;
        } 
        printf ("I$Number of free job struct = %d\n", job_cnt); 
} 

/*** rel_jobreq
 *
 * release a job in the inuse_que.
 *
 */
 
int rel_jobreq (j_req)
struct job_req *j_req;      /* pointer to the job_req being removed */
{
	struct job_req *tj_req;
	long major_nodes;
	long minor_nodes;
	long tmp_nodes;
	int i;
	int layer_inx, period_inx;
	struct block *top_blk;
 
	if (j_req == NULL)
		return (-1);
	top_blk = j_req->blk_que.top;
	if (top_blk == NULL) {
	    printf ("I$rel_jobreq: Job %d has no block.\n", j_req->orig_seqno);
	    return (-1);
	}
	layer_inx = top_blk->layer_inx;
        if (layer_inx < 0 || layer_inx >= MAX_LAYER) { 
	    printf ("I$rel_jobreq: Problem with from_layer input \n");
            return (-1);
        }

	period_inx = j_req->period_inx;
        if (period_inx < 0 || period_inx > MAX_PERIOD) {
            printf ("I$rel_jobreq: Problem with period_inx input \n");
            return (-1);
        }

        if (rem_job (j_req, &layer[layer_inx].inuse_que[period_inx]) < 0)
	    printf ("I$rel_jobreq: problem with rem_job\n");
        if (rel_blkinjob (j_req) < 0)
	    printf ("I$rel_jobreq: problem with rel_blkinjob\n");

	/* can we move jobs from the timeshare layer to the
	 * base layer ? 
	 */
	if ( layer_inx == BASE_LAYER && TIMESHARE != 0) {
	    /* try to move jobs in timeshare layer to the base
	     * layer.
	     */
	    for (i = 0; i < MAX_PERIOD + 1; i++) {
            	tj_req = layer[TS_LAYER].inuse_que[i].top;
            	while (tj_req != NULL) {
	            if (mv_job (tj_req, TS_LAYER, BASE_LAYER) < 0)
		    	printf ("I$rel_jobreq: problem with rel_jobreq\n");
                    tj_req = tj_req->next;
		}
	    }
        }

        if (init_job_req (j_req) < 0)
		printf ("I$rel_jobreq: bad init_job_req\n");
        if (add_job (j_req, &free_job_que, LAST) < 0)
		printf ("I$rel_jobreq: problem with add_job\n");
	return (0);
}

/*** mv_job
 *
 * try to move a job from from_rootp, to to_rootp.
 *
 */
 
int mv_job (j_req, from_layer, to_layer)
struct job_req *j_req;      /* pointer to the job_req being removed */
int from_layer; 
int to_layer; 
{
	struct blk_que b_que;      /* pointer to the tmp blk_que to hold blk */
	struct block *blk, *t_blk, *root_blk;
	int mv_error;
	int period_inx;

	if (j_req == NULL) {
	    printf ("I$mv_job: Problem with j_req input \n");
	    return (-1);
	}

	if (from_layer < 0 || from_layer >= MAX_LAYER) {
	    printf ("I$mv_job: Problem with from_layer input \n");
	    return (-1);
	}

	if (to_layer < 0 || to_layer >= MAX_LAYER) {
	    printf ("I$mv_job: Problem with to_layer input \n");
	    return (-1);
	}

	period_inx = j_req->period_inx;
	if (period_inx < 0 || period_inx > MAX_PERIOD) {
	    printf ("I$mv_job: Problem with period_inx input \n");
	    return (-1);
	}
	b_que.top = b_que.last = NULL;
	mv_error = 0;

	/* try to find each blk in to_layer */

	blk = j_req->blk_que.top;
	while (blk != NULL) {
	    root_blk = blk->node_set->root_blk[to_layer];
	    if (root_blk->node_set->blocked[to_layer] > 0) {  
		mv_error = 1; 	/* to_layer is blocked */
		break;
	    }	    
	    if ((t_blk = match_blk (blk, root_blk)) != NULL) {
		add_blk (t_blk, &b_que, SIZE);
		if (adj_count (t_blk, -1 * (t_blk->width * t_blk->height)) 
		< 0) {
		    printf ("I$mv_job: adj_count problem\n");
		    mv_error = -1;
		    break;
		}
	    } else {
		mv_error = 1;
		break;
	    }
	    blk = blk->next;
	}

	if (mv_error == 0) {		/* can be moved */
	    if (rem_job (j_req, &layer[from_layer].inuse_que[period_inx]) < 0) {
                printf ("I$mv_job: problem with rem_job\n");
		mv_error = -1;
	    } else {
		if (rel_blkinjob (j_req) < 0) {
                    printf ("I$mv_job: problem with rel_blkinjob\n");
		    mv_error = -1;
		} else {
		    /* link in the new blk */
		    j_req->blk_que.top = b_que.top;
		    j_req->blk_que.last = b_que.last;
		    b_que.top = b_que.last = NULL;
		    add_job (j_req, &layer[to_layer].inuse_que[period_inx], 
		    SIZE);
		}
	    }
	}

	/* release blk queueed in b_que */

	blk = b_que.top;
	while (blk != NULL) {
	    if (rel_blk (blk) < 0) {
		printf ("I$mv_job: problem with rel_blk\n");
		mv_error = -1;
	        break;
	    }
	    blk = blk->next;
	}
	return (mv_error);
}
#endif
