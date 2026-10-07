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
 
/* buddyxvar.h
 *
 * The buddy allocation system's global variable defination file.
 */
/*
 * HISTORY
 * $Log: buddyxvar.h,v $
 * Revision 1.4  1994/11/19  02:43:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:05:49  mwan
 * R1.2 mods.
 *
 * Revision 1.2  1993/07/13  18:18:56  mwan
 * T11 - fixed PTS 5012
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 *
 */

#include "buddy.h"

/* Block structure */

extern struct block block[MAX_BLOCK];		/* block structure */
extern struct blk_que free_blk_que;		/* free block queue */

/* Job Request structure */

extern struct job_req job_req[MAX_JOB];	/* job request structure */
extern struct job_que free_job_que;		/* free job request queue */
extern struct job_que timesh_que;

/* Layer and root part structure */

extern struct layer layer[MAX_LAYER];
extern struct root_part rpart[MAX_RPART];
extern struct node_set node_set[MAX_SET];
extern struct node_grp node_grp[MAX_GROUP];

/* global variables */

extern struct glb glb;

/* default search order */

extern int def_order[MAX_BUD];
extern char key_wd[N_KEYWD][NAME_LEN];

/* sched_param input */

extern struct inp inp;
