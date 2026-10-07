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
 
/*++ macs_sched.c
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/macs_sched.c,v $
 *
 * DESCRIPTION:
 *
 *	The module contains a number of general subroutines for the 
 *	 Modified 2-D buddy system.
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
 * $Log: macs_sched.c,v $
 * Revision 1.10  1995/03/17  18:21:30  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 9765
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/include/nqs/buddy.h
 * 	cmds_libs/src/usr/include/nqs/buddyvar.h
 * 	cmds_libs/src/usr/lib/nqs/macs_lib.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_bsc.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_vtimer.c
 * 	cmds_libs/src/usr/lib/nqs/macs_sched.c
 * 	cmds_libs/src/usr/lib/nqs/macs_rootp.c
 *
 * Revision 1.9  1995/02/24  23:41:54  kremenek
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 5134
 *  Testing: Developer testing
 *  Module(s):      ./cmds_libs/src/usr/bin/qmgr/mgr_cmd.c
 *                 ./cmds_libs/src/usr/bin/qmgr/mgr_main.c
 *                 ./cmds_libs/src/usr/bin/qmgr/mgr_packet.c
 *                 ./cmds_libs/src/usr/bin/qmgr/qmgr.hlp
 *                 ./cmds_libs/src/usr/ccs/lib/libnqs/listq.c
 *                 ./cmds_libs/src/usr/include/nqs/nqspacket.h
 *                 ./cmds_libs/src/usr/include/nqs/nqsstruct.h
 *                 ./cmds_libs/src/usr/lib/nqs/macs_sched.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_ldconf.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_main.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_nsq.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_upq.
 *
 * Revision 1.8  1994/11/19  02:52:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/08/31  20:23:27  bradf
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.5.2.1  1994/08/05  20:37:04  kremenek
 *  Reviewer: George Kremenek SDSC
 *  Risk: Low
 *  Benefit or PTS #: 6589 9491 9950 10355
 *  Testing: EATS passed
 *  Module(s):
 *
 * Revision 1.6  1994/08/05  17:14:59  mwan
 *  Reviewer: George Kremenek
 *  Risk: Low
 *  Benefit or PTS #: 6589 9491 9950 10355
 *  Testing: EATS
 *  Module(s):
 *
 * Revision 1.4.2.1  1994/03/16  22:46:01  mwan
 * Fixed 8346 and 8325
 *
 *  Reviewer: kremenek
 *  Risk: L
 *  Benefit or PTS #: 8346 and 8325
 *  Testing:
 *  Module(s):usr/ccs/lib/libnqs/listq.c, usr/ccs/lib/libnqs/main_dsp.c
 *                    usr/lib/nqs/nqs_spawn.c, usr/lib/nqs/macs_rootp.c,
 *                    usr/lib/nqs/macs_lib.c, usr/lib/nqs/macs_sched.c
 *
 * Revision 1.3  1993/07/13  17:53:03  mwan
 * T11 - fixed PTS 5022
 *
 * Revision 1.2  1992/10/09  22:24:38  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 *
 */

#ifdef SDSC

#define MAX_WAIT_PRINT_CNT	10	/* max number of jobs in the waiting 
				 * queue to print */
#include <stdio.h>
#include <fcntl.h>
#include <pwd.h>
#include <time.h> 
#include <signal.h> 
#include <errno.h>
#include "nqs.h"
#include "nqsmail.h"
#include "nqsxvars.h"
#include "buddyvar.h"

/*
 *	External functions:
 */

extern char *malloc();                  /* Allocate dynamic memory */
extern void init_que ();
extern int read_param ();
extern void init_rootp ();
extern int screen_demo ();
extern struct job_req *get_job ();
extern int alloc_base ();
extern int alloc_ts ();
extern int alloc_blk ();
extern int add_job ();
extern int sched_job ();
extern int rel_jobreq ();
extern int release_t_job ();
extern struct job_req *match_req ();
extern int set_block_ts ();
extern int set_block ();
extern int set_rootp_attri ();
extern int alloc_set ();
extern void set_nxaccounts ();
extern void bsc_spawn ();
#ifdef SDSC
extern void bsc_spawnMode ();
#endif
extern void nqs_vtimer ();
extern void free_job_cnt ();
extern void free_blk_cnt ();
extern int mai_send ();
extern int getreq ();
extern void nqs_deque();                /* Dequeue the request */
                                        /* from the queue */
extern void nqs_disreq();               /* Dispose of request */
extern void udb_qorder();   
extern struct job_que *find_inuse_que ();
extern int rem_job ();
extern int rem_job_ts ();
extern char *timstr ();
extern int get_mode ();

extern int errno;		/* System call error number */
#ifdef SDSC
short sModeChange = 0;          /* initial value */
#endif

int macs_sched ();
void read_run_req ();
void read_que_req ();
void sched_all ();
int start_job ();
void set_time ();
int sched_job ();
int sched_t_job ();
int delete_t_job ();
void add_user ();
int chk_user ();

/* macs_sched ()
 *
 * Schedule the jobs that are queued in wait_que.
 *
 */
int macs_sched ()
{
	int i;

	if (glb.sched_flag == NO_SCHED)		/* problem in nqs_boot.c */
		return (-1);

        /* Read the param file */
 
        if (read_param () != 0) {
                printf ("I$Read_param error\n");
                fflush (stdout);
                return (-1);
        }
	
	/* set the time */

	putenv (TIME_ZONE);
	set_time ();

       if (glb.macs_flag == 1)
                return (-1);

       if(NOSCHED == 0)
        {
        /* do normal scheduling (NOSCHED set in /usr/spool/nqs/conf/sched_param) */
        glb.macs_flag = 1;            /* mark macs_sched () busy */

        printf ("I$mode =%2d, START = %5.2f I_START = %5.2f, I_END = %5.2f\n",
        glb.cur_mode, (float) glb.cur_time24 / 3600.,
        (float) glb.p_start / 3600., (float) glb.p_end / 3600.);
        fflush (stdout);

	/* set the attribute of the rootp depending on mode */

	set_rootp_attri ();

	/* read the NQS run queue */

	read_run_req ();

	/* read the NQS queued queue */

	read_que_req ();

	/* schedule all jobs in the wait queue */

	sched_all ();
        }
        else {
          /* do not schedule any jobs at all */
          /* (NOSCHED set to 1 in /usr/spool/nqs/conf/sched_param file ) */
        }

        glb.macs_flag = 0;            /* mark macs_sched ()  not busy */

#ifdef DEBUG
/*
	free_job_cnt ();
	free_blk_cnt ();
*/
	fflush (stdout);
#endif

	return (0);
}


/* read_run_req ()
 * 
 * Routine to read NQS running request and compare with the macs_sched's
 * internal structure.
 * 
 */
void read_run_req ()
{
        struct queue *queue;
        struct request *request;
        struct job_req *tj_req;
        long mode;
	int i, j;
	struct root *rootp;

	glb.num_actuser = 0;
        queue = Pribatqueset;
        while (queue != (struct queue *) 0) {
                request = queue->runset;
                while (request != (struct request *) 0) {

			/* skip job's with ncpus == 0 */

			if (request->v1.req.v2.bat.ncpus <= 0) {
                        	request = request -> next;
				continue;
			}
 
                        /* Try to match request in the done_que */
 	
                        if ((tj_req = match_req (request)) != NULL) {
				tj_req->run_flag = 1;
				add_user (tj_req);
			} else {
                                /*
                                 * Something is wrong.
                                 * There is no job_req corresponging to a
                                 * running request.
                                 */
                                printf ("I$No job_req for request=%d\n",
                                request->v1.req.orig_seqno);
                                fflush (stdout);
                        }
                        request = request -> next;
                }
                queue = queue->v1.batch.nextpriority;
        }
 
	/* release jobs with run_flag = 0 from internal struct */

        for (i = 0; i < MAX_LAYER; i++) {
          for (j = 0; j < MAX_PERIOD + 1; j++) {
 
            /* try the inuse_que first */ 
 
	    tj_req = layer[i].inuse_que[j].top;
            while (tj_req != NULL) { 
		if (tj_req->run_flag == 1) {
		    tj_req->run_flag = 0;
                    printf ("I$inuse: seqn=%6d n=%4d time=%f uid=%d",
                    tj_req->orig_seqno,
                    tj_req->req_nodes,
                    (float) tj_req->req_time / 3600.0,
		    tj_req->uid);
		    printf (" grp=%4d\n", tj_req->grp_inx);
                    fflush (stdout);
		} else {
		    printf ("I$Release job %6d - No matching NQS req\n",
                    tj_req->orig_seqno);
		    if (rel_jobreq (tj_req) < 0)
			printf ("I$read_run-req: Problem with rel_jobreq\n");
		}
                tj_req = tj_req->next;
            }
	  }
        }
}
 
/* read_que_req ()
 * 
 * Routine to read queued request and queue them in the wait_que.
 * Also, calculate glb.job_in_que.
 * 
 */
void read_que_req ()
{
        int i;
        struct queue *queue;
        struct request *request;
        struct job_req *tj_req;
        long mode;
	struct job_que *job_que;
	int sort_flag;

        glb.job_in_que = 0;
 
        /* Read all queued request */
 
        queue = Pribatqueset;
        while (queue != (struct queue *) 0) {
                if (queue->q.v1.batch.prncpus <= 0 ||
                !(queue->q.status & QUE_RUNNING)) {
                        queue = queue->v1.batch.nextpriority;
                        continue;
                }
                request = queue->queuedset;
                while (request != (struct request *) 0) {

			/* pass up jobs with ncpus <= 0 */

			if (request->v1.req.v2.bat.ncpus <= 0)
				continue;

                        if ((tj_req = get_job (&free_job_que, TOP)) == NULL) {
                                printf ("I$Out of free job_req\n");
                                fflush (stdout);
                                nqs_abort ();
                        }
			if (init_job_req (tj_req) < 0)
			    printf ("I$read_que_req: init_job_req problem\n");
                        tj_req->req_nodes = request->v1.req.v2.bat.ncpus;
			tj_req->req_time = request->v1.req.v2.bat.rcpu;
                        tj_req->prior = (float) queue->q.priority +
                        ((float) (glb.cur_time - request->v1.req.state_time) *
                        AGE_FACTOR / 3600.0);
                        tj_req->uid = request->v1.req.uid;
                        tj_req->orig_mid = request->v1.req.orig_mid;
                        tj_req->orig_seqno = request->v1.req.orig_seqno;
                        tj_req->nqs_req = request;
			strncpy (tj_req->que_name, queue->q.namev.name, 
			NAME_LEN - 1);
			tj_req->que_name[NAME_LEN - 1] = '\0';
			tj_req->grp_inx = queue->q.v1.batch.node_group;
			
			/* see which wait queue to queue this job */

			if ((request->start_time > 0) &&
			(queue->q.priority >= TSCHED_PRI)) {
				job_que = &layer[TC_LAYER].wait_que;
				sort_flag = ST_TIME;
				tj_req->start_time = request->start_time;
				tj_req->period_inx = 
				request->v1.req.v2.bat.period_inx;
			} else {
				job_que = &layer[BASE_LAYER].wait_que;
				sort_flag = PRI;
			}

                        if (add_job (tj_req, job_que, sort_flag) < 0) {
                            printf ("I$read_que_req: Bad add_job, req = %s\n",
                            request->v1.req.reqname);
                            if (add_job (tj_req, &free_job_que, LAST) < 0)
                                printf ("I$read_que_req: Bad add_job\n");
                            fflush (stdout);
                            request = request -> next;
                            continue;
                        }
                        glb.job_in_que ++;
                        request = request -> next;
                }
                queue = queue->v1.batch.nextpriority;
        }

	/* tennis court queue first */

	i = 0;
        tj_req = layer[TC_LAYER].wait_que.top;
        while (tj_req != NULL) {
	   i ++;
	   if (i <= MAX_WAIT_PRINT_CNT) {
            	printf ("I$t_w_que: seqn=%6d n=%4d time=%f pr=%5.1f uid=%d",
            	tj_req->orig_seqno,
            	tj_req->req_nodes,
            	(float) tj_req->req_time / 3600.0, tj_req->prior,
	    	tj_req->uid);
		printf (" grp=%4d\n", tj_req->grp_inx);
	   }
           tj_req = tj_req->next;
        }

	/* Then the regular queue */

	i = 0;
        tj_req = layer[BASE_LAYER].wait_que.top;
        while (tj_req != NULL) {
	   i ++;
	   if (i <= MAX_WAIT_PRINT_CNT) {
            	printf ("I$w_que: seqn=%6d n=%4d time=%f pr=%5.1f uid=%d",
            	tj_req->orig_seqno,
            	tj_req->req_nodes,
            	(float) tj_req->req_time / 3600.0, tj_req->prior,
	    	tj_req->uid);
		printf (" grp=%4d\n", tj_req->grp_inx);
	   }
           tj_req = tj_req->next;
        }
        fflush (stdout);
}

/* sched_all ()
 *
 * Schedule the jobs that are queued in wait_que.
 * Jobs are ordered in decreasing order of priority in the wait_que.
 */
void sched_all ()
{
	int i, j, tmp_inx;
	struct job_req *tj_req;
	struct job_que tmp_job_que;


	tmp_job_que.top = tmp_job_que.last = NULL;

	for (i = 0; i < glb.num_set; i++) {
	    tmp_inx = glb.set_inx[i];
	    for (j = 0; j < MAX_LAYER; j++) {
		node_set[tmp_inx].blocked[j] = 0;
	    }
	}
	/* schedule the tennis court job first */

        while ((tj_req = get_job (&layer[TC_LAYER].wait_que, TOP)) != NULL) {

            for (j = 0; j < glb.num_group; j++) {
            	if (tj_req->grp_inx == glb.grp_inx[j])
                    break;
            }
            if (j >= glb.num_group) {
            	printf ("I$sched_all: group %d has not been configured\n", 
		tj_req->grp_inx);
                continue;
            }

	    if ((i = sched_t_job (tj_req)) == 0) {  /* successful */
		if (start_job (tj_req) < 0) {
		    printf ("I$Sched_all: problem with start_job\n");
		    if (delete_t_job (tj_req) < 0)
		        printf ("I$Sched_all: problem with delete_t_job\n");
		}
	    } else if (i == 1) {	/* requeue the job */
		add_job (tj_req, &free_job_que, LAST);
		continue;
	    } else {			/* delete the NQS job */
	        if (delete_t_job (tj_req) < 0)
	            printf ("I$Sched_all: problem with delete_t_job\n");
	    }
	}

        while ((tj_req = get_job (&layer[BASE_LAYER].wait_que, TOP)) != NULL) {
            for (j = 0; j < glb.num_group; j++) {
                if (tj_req->grp_inx == glb.grp_inx[j])
                    break;
            }
            if (j >= glb.num_group) {
                printf ("I$sched_all: group %d has not been configured\n",
                tj_req->grp_inx);
		add_job (tj_req, &free_job_que, LAST);
                continue;
            }

	    if (CHK_RUNLIMIT > 0) {
		if (chk_runlimit (tj_req) < 0) {  /* NQS run_limit exceeded */
		    add_job (tj_req, &free_job_que, LAST);
		    continue;
		}
	    }

	    /* check if overrun the soft user_limit */

	    if (chk_user (tj_req, CHK_SOFT_ULIM) != 0) {
		add_job (tj_req, &tmp_job_que, LAST);
		continue;
	    }

	    if (sched_job (tj_req) == 0) {  /* successful */
		    if (start_job (tj_req) < 0) {
		        printf ("I$Sched_all: problem with start_job\n");
		        if (rel_jobreq (tj_req) < 0)
		            printf ("I$Sched_all: problem with rel_jobreq\n");
		    } else {
			add_user (tj_req);
		    }
	    } else {
		    if (set_block (tj_req) < 0)
		        printf ("I$Sched_all: problem with set_block\n");
		    if (set_block_ts (tj_req) < 0)
		        printf ("I$Sched_all: problem with set_block_ts\n");
		    add_job (tj_req, &free_job_que, LAST);
	    }
	}

	/* now check the hard user_limit */

	while ((tj_req = get_job (&tmp_job_que, TOP)) != NULL) {

	    /* check if overrun the soft user_limit */

	    if (chk_user (tj_req, CHK_HARD_ULIM) != 0) {
		add_job (tj_req, &free_job_que, LAST);
		continue;
	    }

	    if (sched_job (tj_req) == 0) {  /* successful */
		    if (start_job (tj_req) < 0) {
		        printf ("I$Sched_all: problem with start_job\n");
		        if (rel_jobreq (tj_req) < 0)
		            printf ("I$Sched_all: problem with rel_jobreq\n");
		    } else {
			add_user (tj_req);
		    }
	    } else {
		    add_job (tj_req, &free_job_que, LAST);
	    }
	}

	printf ("I$Done scheduling\n");
	fflush (stdout);
}

/* chk_runlimit ()
 *
 * check the NQS global, complex and queue run_limits.
 * return :
 *	0 => OK.
 *	-1 => Not OK.
 */
int chk_runlimit (tj_req)
struct job_req *tj_req;
{
	int i;
        struct queue *queue;
        struct request *request;
	register struct qcomplex *qcomplex;     /* Queue complex walking */

	if (tj_req == NULL)
		return (-1);

	if ((request = tj_req->nqs_req) == NULL) {
		printf ("I$chk_runlimit: No request in job_req\n");
		return (-1);
	} 

	if ((queue = request->queue) == NULL) {
		printf ("I$chk_runlimit: No queue in request\n");
		return (-1);
	} 

	if (queue->q.v1.batch.runlimit > queue->q.runcount &&
	Maxgblbatlimit > Gblbatcount) {
		for (i = MAX_COMPLXSPERQ; --i >= 0;) {
			qcomplex = queue->v1.batch.qcomplex[i];
			if (qcomplex == (struct qcomplex *)0) 
				continue;
			if (qcomplex->runlimit <= qcomplex->runcount)
				return (-1);
		}
	} else {
		return (-1);
	}
	return (0);
}

/* start_job ()
 *
 * subroutine to start a job running.
 * 
 */
int start_job (job_req)
struct job_req *job_req;
{
        long result;
 
        if (job_req == NULL)
                return (-1);
 
        glb.job_in_que--;
        disp_blk (job_req);
        nqs_spawn (job_req->nqs_req, (struct device *) 0);
 
        return (0);
}

/*** set_time ()
 *
 * Subroutine to set the time for the scheduler.
 *
 */
 
void set_time ()
{
        int i, j;
        struct tm *tm;                  /* tm struct */
        long alarm_time;
 
        /* check current local time and adjust glb.cur_time24 */
 
        glb.cur_time = time (0);
        printf ("I$\n");
        printf ("I$Time: %s\n", timstr (glb.cur_time));
        tm = localtime (&glb.cur_time);
        i = tm->tm_wday;                /* day of the week */
        glb.cur_time24 = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;
        if (PRIME_START[i] > PRIME_END[i]) {
                if (glb.cur_time24 <= PRIME_END[i]) {
                        glb.p_end = PRIME_END[i];
                        i -= 1;         /* yesterday */
                        if (i < 0)
                                i = DAYS_PER_W - 1;
                        glb.p_start = PRIME_START[i] - SEC_P_DAY;
                } else {
                    glb.p_start = PRIME_START[i];
		    glb.p_end = 0;
		    for (j = 0; j < DAYS_PER_W; j++) {
                        i += 1;         /* next day */
                        if (i >= DAYS_PER_W)
                                i = 0;
			glb.p_end += SEC_P_DAY;
			if (PRIME_START[i] == PRIME_END[i]) {
                                continue;
                        } else {
                                glb.p_end += PRIME_END[i];
                                break;
                        }
		    }
                }
        } else {
                if (glb.cur_time24 >= PRIME_END[i]) {
		    glb.p_start = glb.p_end = 0;
       		    for (j = 0; j < DAYS_PER_W; j++) { 
                        i += 1;         /* next day */
                        if (i >= DAYS_PER_W)
                                i = 0;
			glb.p_start += SEC_P_DAY;
			glb.p_end += SEC_P_DAY;
			if (PRIME_START[i] == PRIME_END[i]) {
				continue;
			} else {
                        	glb.p_start += PRIME_START[i];
                        	glb.p_end += PRIME_END[i];
				break;
			}
		    }
                } else {
		    glb.p_start = glb.p_end = 0;
                    for (j = 0; j < DAYS_PER_W; j++) {
                        if (PRIME_START[i] == PRIME_END[i]) {
				glb.p_start += SEC_P_DAY;
				glb.p_end += SEC_P_DAY;
                        	i += 1;         /* next day */
                        	if (i >= DAYS_PER_W)
                                	i = 0;
                                continue;
                        } else {
                                glb.p_start += PRIME_START[i];
                                glb.p_end += PRIME_END[i];
				break;
                        }
		    }
                }
        }
        if (glb.p_end < glb.p_start)
                glb.p_end = glb.p_start;
 
        /* set the vtimer to run pack2d ().  Schedule every 15 minutes
         * or at glb.p_end or glb.p_start, whichever comes first. */

 
        if (glb.p_end - glb.cur_time24 > 900) {
	  /* End of prime time is more than 15 minutes away;
	     set alarm for 15 minutes from now. */
	  alarm_time = glb.cur_time + 900;
	  nqs_vtimer(&alarm_time, bsc_spawn);
        } else {
	  /* End of prime time is less than 15 minutes away;
	     set alarm for end of prime time.  Also invoke 
	     nprime_script at that time. */
	  alarm_time = glb.cur_time - glb.cur_time24 + glb.p_end;
	  if (alarm_time > glb.cur_time) {
	    nqs_vtimer(&alarm_time, bsc_spawnMode);
	  }
        }
        if (glb.p_start - glb.cur_time24 < 900) {
	  /* Start of prime time is less than 15 minutes away */
	  if (alarm_time > (glb.cur_time - glb.cur_time24 + glb.p_start)) {
	    /* Alarm is currently set for after start of prime time; 
	       reset it to start of prime time.  Also invoke 
	       prime_script at that time. */
	    alarm_time = glb.cur_time - glb.cur_time24 + glb.p_start;
	    if (alarm_time > glb.cur_time) {
	      nqs_vtimer(&alarm_time, bsc_spawnMode);
	    }
	  }
        }
        glb.cur_mode = get_mode (glb.cur_time24);
}
 
/* sched_job ()
 *
 * Schedule the job. 
 *
 */

int sched_job (tj_req)
struct job_req *tj_req;
{
	int period_inx;		/* major, minor or combo */
	int grp_inx;

	if (tj_req == NULL)
		return (-1);

        grp_inx = tj_req->grp_inx;

        if (init_grp (grp_inx) < 0) {
            printf ("I$sched_job: Problem with init_grp for node_group %d\n",
            grp_inx);
            return (-1);
        }

	if ((period_inx = alloc_base (tj_req)) >= 0) {
		if (alloc_set (tj_req, BASE_LAYER, period_inx, 
		tj_req->req_nodes) < 0) {
			printf ("I$Sched_job: alloc_set problem.\n");
			return (-1);
		}
		tj_req->period_inx = period_inx;
		add_job (tj_req, &layer[BASE_LAYER].inuse_que[period_inx],SIZE);
	} else if (TIMESHARE > 0) {  	/* no timesharing */
		if ((period_inx = alloc_ts (tj_req)) >= 0) {
		    if (alloc_set (tj_req, TS_LAYER, period_inx,
		    tj_req->req_nodes) < 0) {
			return (-1);
		    }
		    tj_req->period_inx = period_inx;
		    add_job (tj_req, &layer[TS_LAYER].inuse_que[period_inx], 
		    SIZE);
		} else {
		    return (-1);
		}
	} else {
	    return (-1);
	}
	return (0);
}

/* sched_t_job ()
 *
 * Schedule the tennis court job. 
 * return value:
 * -1 - remove the NQS job
 *  1 - Cannot schedule the job now. Requeue it.
 *  0 - success
 *
 */

int sched_t_job (tj_req)
struct job_req *tj_req;
{
	struct root *rootp;
	long major_nodes;
	long minor_nodes;
	int period_inx, grp_inx;
	int free_nodes[MAX_PERIOD + 1];

	if (tj_req == NULL)
		return (-1);

        grp_inx = tj_req->grp_inx;

        if (init_grp (grp_inx) < 0) {
            printf ("I$sched_t_job: Problem with init_grp for node_group %d\n",
            grp_inx);
            return (-1);
        }

	free_nodes[MAJOR] = node_grp[grp_inx].free_nodes[TC_LAYER][MAJOR];
	free_nodes[MINOR] = node_grp[grp_inx].free_nodes[TC_LAYER][MINOR];
	free_nodes[COMBO] = free_nodes[MAJOR] + free_nodes[MINOR];

	if ((major_nodes = tj_req->nqs_req->v1.req.v2.bat.major_nodes) < 0) {
            printf ("I$sched_t_job: major_nodes out of range for %d\n",
            tj_req->orig_seqno);
            return (-1);
        }
	
	if ((minor_nodes = tj_req->req_nodes - major_nodes) < 0) {
            printf ("I$sched_t_job: minor_nodes out of range for %d\n",
            tj_req->orig_seqno);
            return (-1);
        }

	period_inx = tj_req->period_inx;

        if (period_inx > MAX_PERIOD || period_inx < 0) {
            printf ("I$sched_t_job: period_inx out of range for %d\n",
            tj_req->orig_seqno);
            return (-1);
        }

	if ((period_inx == MAJOR && minor_nodes != 0) ||
	(period_inx == MINOR && major_nodes != 0)) {
            printf ("I$sched_t_job: minor_nodes/major_nodes mismatch for %d\n",
            tj_req->orig_seqno);
            return (-1);
        }

	if (tj_req->req_nodes > free_nodes[period_inx]) {
	    printf ("I$sched_t_job: not sufficient free_nodes for %d\n",
	    tj_req->orig_seqno);
	    return (1);
	}

	if (tj_req->start_time + tj_req->req_time < glb.cur_time) {
	    printf ("I$sched_t_job: end_time is less than cur_time for %d\n",
	    tj_req->orig_seqno);
	    return (-1);
	}

	if (glb.cur_mode == PRIME_M) {
	    if (period_inx != MAJOR || minor_nodes != 0) {
                printf ("I$sched_t_job:Try to run MINOR in Prime time for %d\n",
            	tj_req->orig_seqno);
           	 return (-1);
            }
	} else {		/* non-prime */
	    if (period_inx != MAJOR) {
	    	/* overrun to prime ? Must run in MAJOR*/
	    	if (tj_req->start_time + tj_req->req_time >
	    	glb.p_start + glb.cur_time - glb.cur_time24) {
                    printf ("I$sched_t_job:Non-prime job overrun to Prime % d\n"
                    , tj_req->orig_seqno);
                    return (-1);
		}
		if (major_nodes > free_nodes[MAJOR] ||
		minor_nodes > free_nodes[MINOR]) {
            	    printf ("I$sched_t_job: not sufficient free_nodes for %d\n",
            	    tj_req->orig_seqno);
            	    return (1);
        	}
	    }
	}

	/* things checked out. Allocate block to the job */
	if (period_inx != COMBO) {
	    if (alloc_set (tj_req, TC_LAYER, period_inx, 
	    tj_req->req_nodes) < 0) {
	        printf ("I$Sched_t_job:Problem with alloc_set\n");
	        return (-1);
	    }
	} else {
	    if (minor_nodes > 0) {	/* do the MINOR first */
	        if (alloc_blk (tj_req, TC_LAYER, MINOR, minor_nodes) < 0) {
	            printf ("I$Sched_t_job:Problem with alloc_blk 1\n");
	            return (-1);
	        }
	    }
	    if (major_nodes > 0) {	/* do the MAJOR */
	        if (alloc_blk (tj_req, TC_LAYER, MAJOR, major_nodes) < 0) {
	            printf ("I$Sched_t_job:Problem with alloc_blk 2\n");
	            return (-1);
	        }
	    }
	}
	add_job (tj_req, &layer[TC_LAYER].inuse_que[period_inx], SIZE);
	return (0);
}

/*** delete_t_job
 *
 * release a tennis court job.
 *
 */

int delete_t_job (j_req)
struct job_req *j_req;      /* pointer to the job_req being removed */
{
        struct rawreq rawreq;
        int fd;                         /* File-descriptor */
        struct request *req;
	struct job_que *inuse_que;


        if (j_req == NULL)
                return (-1);

        if ((req = j_req->nqs_req) == NULL)
                return (-1);
 
        /* now send the user a mail message */
 
        if ((fd = getreq ((long) req->v1.req.orig_seqno,
        req->v1.req.orig_mid, &rawreq)) == -1) {
            printf ("I$delete_t_job: Unable to getreq\n");
        } else {
            mai_send (&rawreq, (struct requestlog *) 0, MSX_BADTSCHED);
        }
        close (fd);
 
        nqs_deque (j_req->nqs_req);  /* Rem the req queue */
        nqs_disreq (j_req->nqs_req, 1);
        udb_qorder (j_req->nqs_req->queue);
 
        if ((inuse_que = find_inuse_que (j_req)) != NULL) {
            if (rem_job (j_req, inuse_que) < 0)
                printf ("I$delete_t_job: problem with rem_job\n");
        }
        if (init_job_req (j_req) < 0)
                printf ("I$delete_t_job: bad init_job_req\n");
        if (add_job (j_req, &free_job_que, LAST) < 0)
                printf ("I$delete_t_job: problem with add_job\n");
        return (0);
}

/*** get_time ()
 *
 * Subroutine to set the time for the scheduler.
 * input : cur_time - time since epoch time in sec
 * output :
 * 	p_start - prime start since the start of the day in sec
 *	p_end - prime end since the start of the day in sec
 *	cur_time24 - curent time since the start of the day
 *	mode
 * 
 *
 *
 */
 
void get_time (cur_time, p_start, p_end, cur_time24, mode)
time_t cur_time;
long *p_start;
long *p_end;
long *cur_time24;
long *mode;
{
        int i, j;
        struct tm *tm;                  /* tm struct */
 
        /* check current local time and adjust glb.cur_time24 */
 
        tm = localtime (&cur_time);
        i = tm->tm_wday;                /* day of the week */
        *cur_time24 = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;
        if (PRIME_START[i] > PRIME_END[i]) {
                if (*cur_time24 <= PRIME_END[i]) {
                        *p_end = PRIME_END[i];
                        i -= 1;         /* yesterday */
                        if (i < 0)
                                i = DAYS_PER_W - 1;
                        *p_start = PRIME_START[i] - SEC_P_DAY;
                } else {
                    *p_start = PRIME_START[i];
                    *p_end = 0;
                    for (j = 0; j < DAYS_PER_W; j++) {
                        i += 1;         /* next day */
                        if (i >= DAYS_PER_W)
                                i = 0;
                        *p_end += SEC_P_DAY;
                        if (PRIME_START[i] == PRIME_END[i]) {
                                continue;
                        } else {
                                *p_end += PRIME_END[i];
                                break;
                        }
                    }
                }
        } else {
                if (glb.cur_time24 >= PRIME_END[i]) {
                    *p_start = *p_end = 0;
                    for (j = 0; j < DAYS_PER_W; j++) {
                        i += 1;         /* next day */
                        if (i >= DAYS_PER_W)
                                i = 0;
                        *p_start += SEC_P_DAY;
                        *p_end += SEC_P_DAY;
                        if (PRIME_START[i] == PRIME_END[i]) {
                                continue;
                        } else {
                                *p_start += PRIME_START[i];
                                *p_end += PRIME_END[i];
                                break;
                        }
                    }
                } else {
                    *p_start = *p_end = 0;
                    for (j = 0; j < DAYS_PER_W; j++) {
                        if (PRIME_START[i] == PRIME_END[i]) {
                                *p_start += SEC_P_DAY;
                                *p_end += SEC_P_DAY;
                                i += 1;         /* next day */
                                if (i >= DAYS_PER_W)
                                        i = 0;
                                continue;
                        } else {
                                *p_start += PRIME_START[i];
                                *p_end += PRIME_END[i];
                                break;
                        }
                    }
                }
        }
        if (*p_end < *p_start)
                *p_end = *p_start;

        if (*cur_time24 >= *p_start && *cur_time24 < *p_end) {
                *mode = PRIME_M;
        } else {
                *mode = NPRIME_M;
        }
}

/*** chk_tsched ()
 *
 * Subroutine to check whether tennis court scheduling is possible.
 * input : 
 *	start_time - start time (sec from epoch time.
 *	req_time - request time.
 *	req_nodes - number of nodes requested.
 *	
 * return value:
 *	-1 - tsched not allowed.
 *	>= 0 - the period_inx assigned to the job.
 *
 */
 
int chk_tsched (start_time, req_time, req_nodes, major_nodes, grp_inx)
time_t start_time;
int req_time;
long req_nodes;
long *major_nodes;
int grp_inx;
{
        int slot_cnt, i, j;
	long end_time;
        struct tm *tm;                  /* tm struct */
	int temp_time;
	struct schedule *tsched;
	int start_slot, end_slot;
        register struct queue *queue;
        register struct request *req;
	int period_inx;
	struct job_req *j_req;
	int tmp_inx;
 
        /* check if start_time is at TSCHED_TSLOT interval */
 
        tm = localtime (&start_time);
        temp_time = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

	if (temp_time / TSCHED_TSLOT * TSCHED_TSLOT != temp_time) {
#ifdef DEBUG
	    printf ("I$chk_tsched: start_time not at TSCHED_SLOT interval\n");
#endif
	    return (-1);
	}

	/* req_time too long ? */

	if ((slot_cnt = req_time / TSCHED_TSLOT) > MAX_SLOT_CNT) {
#ifdef DEBUG
	    printf ("I$chk_tsched: req_time too long\n");
#endif
	    return (-1);
	}
	
	if (slot_cnt * TSCHED_TSLOT != req_time) {
#ifdef DEBUG
	    printf ("I$chk_tsched: req_time not at TSCHED_SLOT interval\n");
#endif
	    return (-1);
	}

#ifdef DEBUG
	printf ("I$start_time=%d, req_time=%d, req_nodes=%d, slot_cnt=%d\n",
	start_time, req_time, req_nodes, slot_cnt);
	fflush (stdout);
#endif
	tsched = (struct schedule *) 
	malloc (sizeof (struct schedule) * slot_cnt);
	if (tsched == (struct schedule *) 0) {
#ifdef DEBUG
	    printf ("I$chk_tsched: malloc problem, errno=%d \n", errno);
#endif
	    return (-1);
	}

	/* Fill in the mode entry on tsched */

	if (init_tsched (tsched, slot_cnt, start_time, req_time, grp_inx) < 0) {
	    printf ("I$chk_tsched: problem with filling init_tsched\n");
	    free ((char *) tsched);  
	    return (-1);
	}

	end_time = start_time + req_time;
	for (i = 0; i < MAX_PERIOD + 1; i ++) {
	    j_req = layer[TC_LAYER].inuse_que[i].top;
	    while (j_req != NULL) {
	      if (grp_inx == j_req->grp_inx) {
		if (fill_nodes (tsched, start_time, end_time, 
		j_req->start_time, j_req->start_time + j_req->req_time, 
		j_req->req_nodes, j_req->nqs_req->v1.req.v2.bat.major_nodes) 
		< 0) {
	    	    free ((char *) tsched);  
#ifdef DEBUG
	    	    printf ("I$chk_tsched: fill_nodes problem \n");
#endif
	    	    return (-1);
		}
	      }
	      j_req = j_req->next;
	    }
	}

	/* check those tsched req that are waiting */

        queue = Nonnet_queueset;
	while (queue != (struct queue *) 0) {
	    req = queue->waitset;
	    while (req != (struct request *)0) {
		period_inx = req->v1.req.v2.bat.period_inx;
		if (req->start_time > 0 && 
		req->queue->q.priority >= TSCHED_PRI &&
		period_inx >= 0 && period_inx < MAX_PERIOD + 1) {
	          if (grp_inx == req->queue->q.v1.batch.node_group) {
		    if (fill_nodes (tsched, start_time, end_time, 
		    req->start_time, 
		    req->start_time + req->v1.req.v2.bat.rcpu,
		    req->v1.req.v2.bat.ncpus, 
		    req->v1.req.v2.bat.major_nodes) < 0) {
			free ((char *) tsched);  
#ifdef DEBUG
	    	        printf ("I$chk_tsched: fill_nodes problem \n");
#endif
			return (-1);
		    }
		  }
		}
		req = req->next;
	    }
	    queue = queue->next;
	}
#ifdef DEBUG
	printf ("I$TSCHED  mode  MAJOR  MINOR COMBO\n");
	for (i = 0; i < slot_cnt; i++) {
	    printf ("I$        %4d  %5d  %5d  %5d\n", 
	    tsched[i].mode, tsched[i].node_cnt[MAJOR], 
            tsched[i].node_cnt[MINOR], tsched[i].node_cnt[COMBO]);
	}
	fflush (stdout);
#endif

	/* Now see if we can schedule this job. */ 

	if (chk_sched (tsched, slot_cnt, req_nodes, major_nodes, MINOR) == 0)
            period_inx = MINOR;
        else if (chk_sched (tsched, slot_cnt, req_nodes, major_nodes, COMBO)
	== 0)
            period_inx = COMBO;
        else if (chk_sched (tsched, slot_cnt, req_nodes, major_nodes, MAJOR) 
	== 0)
            period_inx = MAJOR;
        else
            period_inx = -1;

	free ((char *) tsched);  
	return (period_inx);
}

/* fill_nodes
 * 
 * Routine that decrease the node_cnt of the tsched schedule if a job's
 * j_start_time - j_end_time overlaps with the start_time and end_time
 * of the tsched.
 *
 */
int fill_nodes (tsched, start_time, end_time, j_start_time, j_end_time, 
req_nodes, major_nodes, grp_inx, top_blk)
struct schedule *tsched;
time_t start_time;
time_t end_time;
time_t j_start_time;
time_t j_end_time;
long req_nodes;
long major_nodes;
{
        int i, j;
	int start_slot, end_slot;
	long minor_nodes;
 
	/* req_time too long ? */

	minor_nodes = req_nodes - major_nodes;
	if ((end_time - start_time) / TSCHED_TSLOT > MAX_SLOT_CNT) {
	    printf ("I$fill_nodes: tsched time too long\n");
	    return (-1);
	}

	if (j_start_time < start_time)
	    start_slot = start_time;
	else if (j_start_time > end_time) 
	    start_slot = end_time;
	else
	    start_slot = j_start_time;
	
	if (j_end_time < start_time)
	    end_slot = start_time;
	else if (j_end_time > end_time) 
	    end_slot = end_time;
	else
	    end_slot = j_end_time;

	if (end_slot > start_slot) {
	    start_slot = (start_slot - start_time) / TSCHED_TSLOT;
	    end_slot = (end_slot - start_time) /TSCHED_TSLOT - 1;

	    for (i = start_slot; i <= end_slot; i++) {
		    tsched[i].node_cnt[COMBO] -= req_nodes;
		    tsched[i].node_cnt[MAJOR] -= major_nodes;
		    tsched[i].node_cnt[MINOR] -= minor_nodes;
	    }
	}
	return (0);
}

/* chk_sched ()
 *
 * routine check if a request with req_nodes number of nodes will 
 * fit in the tsched.
 *
 * return :
 * 	0 -> yes
 *	-1 -> no
 *
 */
int chk_sched (tsched, slot_cnt, req_nodes, major_nodes, period_inx)
struct schedule *tsched;
int slot_cnt;
int req_nodes;
long *major_nodes;
{
	int i;
	int min_minor;

	*major_nodes = 0;
	min_minor = -1;
	
	if (period_inx == COMBO) {
            for (i = 0; i < slot_cnt; i++) {
                if (tsched[i].node_cnt[period_inx] < req_nodes) 
                    return (-1);
		if (min_minor == -1) {		/* first time */
		    if (req_nodes <= tsched[i].node_cnt[MINOR]) {
			min_minor = req_nodes;
		    } else {
			min_minor = tsched[i].node_cnt[MINOR];
		    }
		} else if (tsched[i].node_cnt[MINOR] < min_minor) {
		    min_minor = tsched[i].node_cnt[MINOR];
		}
            }
	    *major_nodes = req_nodes - min_minor;

	    /* go back one more time to make sure we have the major_nodes */

            for (i = 0; i < slot_cnt; i++) {
                if (tsched[i].node_cnt[MAJOR] < *major_nodes) {
                    return (-1);
		}
	    }

	} else {
            for (i = 0; i < slot_cnt; i++) {
                if (tsched[i].node_cnt[period_inx] < req_nodes) {
                    return (-1);
		}
	    }
	    if (period_inx == MAJOR)
		*major_nodes = req_nodes;
	    else 
		*major_nodes = 0;
	}
	return (0);
}

/* init_tsched
 * 
 * Routine that fills in the mode and node_cnt of the tsched schedule.
 * The node_cnt is the number of free nodes available. The node_cnt
 * for the COMBO partition are set to zero initially.
 */
int init_tsched (tsched, slot_cnt, start_time, req_time, grp_inx)
struct schedule *tsched;
int slot_cnt;
time_t start_time;
int req_time;
int grp_inx;
{
        int i, j;
	time_t cur_time;
	long p_start; 
	long p_end; 
	long cur_time24; 
	long mode;
	int slot_used, cur_slot;
 
	if (tsched == NULL)
	    return (-1);

	/* req_time too long ? */

	if (slot_cnt > MAX_SLOT_CNT) {
	    printf ("I$init_tsched: Too many tsched time slot\n");
	    return (-1);
	}

	if (slot_cnt * TSCHED_TSLOT != req_time) {
	    printf ("I$init_tsched: Mismatch in slot_cnt and req_time\n");
	    return (-1);
	}

	/* Now fill in the slot */

	cur_time = start_time;
	cur_slot = 0;
	while (cur_slot < slot_cnt) {
		get_time (cur_time, &p_start, &p_end, &cur_time24, &mode);
		if (mode == PRIME_M) {
		    slot_used = (p_end - cur_time24) / TSCHED_TSLOT;
		    if (slot_used <= 0)
			slot_used = 1;	/* so it won't get stuck */
		    if (cur_slot + slot_used > slot_cnt)
			slot_used = slot_cnt - cur_slot;
		    for (i = cur_slot; i < cur_slot + slot_used; i++) {
			tsched[i].mode = mode;
                        tsched[i].node_cnt[MAJOR] = 
			node_grp[grp_inx].size[MAJOR];
                        tsched[i].node_cnt[MINOR] = 0;
			tsched[i].node_cnt[COMBO] = 0;
		    }
		} else {
		    slot_used = (cur_time24 - p_end) / TSCHED_TSLOT;
		    if (slot_used <= 0)
			slot_used = 1;	/* so it won't get stuck */
		    if (cur_slot + slot_used > slot_cnt)
			slot_used = slot_cnt - cur_slot;
		    for (i = cur_slot; i < cur_slot + slot_used; i++) {
			tsched[i].mode = mode;
                        tsched[i].node_cnt[MAJOR] = 
			node_grp[grp_inx].size[MAJOR];
                        tsched[i].node_cnt[MINOR] = 
			node_grp[grp_inx].size[MINOR];
			tsched[i].node_cnt[COMBO] = 
			node_grp[grp_inx].size[MAJOR] + 
			node_grp[grp_inx].size[MINOR];
		    }
		}
		cur_slot += slot_used;
		cur_time += slot_used * TSCHED_TSLOT;
	}
	return (0);
}

/* add_user
 *
 * sum up the number of nodes used by a user.
 *
 *
 */
void add_user (j_req)
struct job_req *j_req;
{
	int i;
	int found_user;

	if (j_req == NULL)
		return;

	found_user = 0;
	for (i = 0; i < glb.num_actuser; i ++) {
	    if (j_req->uid == glb.actuser[i].uid) {
		glb.actuser[i].node_used += j_req->req_nodes;
		found_user = 1;
		break;
	    }
	}

	if (found_user == 0) {
	    glb.actuser[glb.num_actuser].uid = j_req->uid;
	    glb.actuser[glb.num_actuser].node_used = j_req->req_nodes;
	    glb.num_actuser ++;
	}
}

/* chk_user
 *
 * check if a user has exceeded the soft/hard user limit..
 *
 *
 */
int chk_user (j_req, ulim_flag)
struct job_req *j_req;
int ulim_flag;		/* soft or hard ulim */
{
	int i;
	int found_user;
	int node_used;

	if (j_req == NULL)
		return (-1);

	if (ulim_flag != CHK_SOFT_ULIM && ulim_flag != CHK_HARD_ULIM) {
	    printf ("I$chk_user: Incorrect ulim_flag value\n");
	    return (-1);
	}

	if (ulim_flag == CHK_SOFT_ULIM && Maxsoftulimit <= 0)
	    return (0);
 
	if (ulim_flag == CHK_HARD_ULIM && Maxhardulimit <= 0)
	    return (0);
 
	found_user = 0;
	for (i = 0; i < glb.num_actuser; i ++) {
	    if (j_req->uid == glb.actuser[i].uid) {
		node_used = glb.actuser[i].node_used + j_req->req_nodes;
		found_user = 1;
		break;
	    }
	}

	if (found_user == 0) {
	    if (ulim_flag == CHK_SOFT_ULIM) 	/* softulimit can be exceeded */
		return (0); 			/* if first time. */
	    node_used = j_req->req_nodes;
	}

	if (ulim_flag == CHK_SOFT_ULIM && node_used > Maxsoftulimit) {
	    return (-1);
	} else if (ulim_flag == CHK_HARD_ULIM && node_used > Maxhardulimit) {
	    return (-1);
	} else {
	    return (0);
	}
}

#endif

#ifdef SDSC
 
/* iQueuedByUid ()
 *
 * Routine to read all running and queued requests for given queue and user, 
 * calculate the total number of running and queued jobs for given queue 
 * and user. This function  serves as a toll for enforcing
 * max number of running and queued jobs per queue and user for "qsub" command
 *
 */
 
int iQueuedByUid(sLocQueName, iLocUid)
char *sLocQueName;
uid_t iLocUid;
{
        struct queue *queue;
        struct request *request;
        int iRunningAndQueued = 0;
 
        queue = Pribatqueset;
        while (queue != (struct queue *) 0) {
                if (strcmp(sLocQueName, queue->q.namev.name) == 0){
                  /* count all running requests */
                  request = queue->runset;
                  while (request != (struct request *) 0) {
                        if (request->v1.req.uid == iLocUid)
                                iRunningAndQueued++;
                        request = request -> next;
                      }
 
                  /* count all queued requests */
                  request = queue->queuedset;
                  while (request != (struct request *) 0) {
                        if (request->v1.req.uid == iLocUid)
                                iRunningAndQueued++;
                        request = request -> next;
                      }
                }
                queue = queue->v1.batch.nextpriority;
        }
        return(iRunningAndQueued);
}
#endif
