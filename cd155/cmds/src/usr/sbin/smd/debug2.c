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
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1994  Intel Corporation.
 *
 * HISTORY
 *
 *
 */

#ifdef DUMPS
#include <stdio.h>
#include "defs.h"

/*
 *  trace_users:
 *
 * Dump USER database.
 */
void
trace_users()
{
 smd_user_t *p;

 for (p = user_db; p; p = p->next) {
	TRACE(TIMINGS, ("%d USER %x %x %x %x %d\n",
	   time(0), p, p->next, p->prev, p->service, p->fd));
 }
}

/*
 *  dump_services:
 *
 * Dump SERVICEs database (associated with the p_table).
 */
void
dump_services()
{
 smd_service_t *p;
 int  i;

 for (i = 0; i < P_HASH_SIZE + 1; i++) {
  for (p = p_table[ i ]; p; p = p->next) {
	 TRACE(TIMINGS, ("%d SERVICE %x %x %x %x %x %x %d\n",
	    time(0), p, p->next, p->prev, p->user, p->user_link_next, 
	    p->user_link_prev, i));
	 trace_qualifier(&p->q_id, "SERVICE2");
	 TRACE(TIMINGS, ("%d SERVICE3 %x %d %d %x %x %d\n",
	 time(0), p->param.flags, p->param.sig_num, p->param.sig_pid, p->param.events, 
	 p->param.alarm_val, p->param.service));
	}
  }
 }


/*
 *  trace_apps:
 *
 * Dump APPLICATION database (associated with the a_table).
 */
void
trace_apps()
{
 smd_app_t *a;
 int  i;

 for (i = 0; i < A_HASH_SIZE; i++) {
  for (a = a_table[ i ]; a; a = a->next) {
	 TRACE(TIMINGS, ("%d APPL %x %x %x %d\n", time(0), a, a->next,
	  a->prev,a->state));
	 TRACE(TIMINGS, ("%d APPL2 %d %d %d %d\n", 
	  time(0), a->data.pgid,a->data.part_id, a->data.acct_id,a->data.uid));

	 TRACE(TIMINGS, ("%d APPL3 %d %d %d %d %d %d %f\n",
	time(0), a->data.app_rollin, a->data.rollin_quantum, a->data.part_rollin, 
	a->data.app_size,  a->data.app_priority, a->data.part_size,
	a->data.global_time));

	 TRACE(TIMINGS, ("%d APPL4 %d\n", time(0), a->data.event_time));
  }
 }
}


/*
 *  trace_qualifier:
 *
 * Dump a QUALIFIER structure.
 */
trace_qualifier(qid, str)
 smd_qualifier_t *qid;
 char  *str;
{
 TRACE(TIMINGS, ("%d %s %d %d %d %d %s\n",
 time(0), str, qid->pgid, qid->part_id, qid->acct_id, qid->uid, qid->req_id));
}


#endif

