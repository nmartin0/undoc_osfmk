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
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/smd/debug.c,v 1.3 1994/11/19 03:16:41 mtm Exp $
 *
 * HISTORY
 * $Log: debug.c,v $
 * Revision 1.3  1994/11/19  03:16:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/07/18  19:55:45  carbajal
 * changed printing of global time from %d to %f
 *
 * Revision 1.1  1992/10/05  23:30:25  rkl
 * Initial revision
 *
 *
 */

#ifdef DUMPS
#include <stdio.h>
#include "defs.h"

/*
 *  dump_users:
 *
 *	Dump all smd_user_t structures.
 */
void
dump_users()
{
	smd_user_t	*p;

	fprintf(stderr, "User list\n");
	for (p = user_db; p; p = p->next) {
		dump_user(p, "  ");
		fprintf(stderr, "\n");
	}
}

/*
 *  dump_parts:
 *
 *	Dump all smd_service_t structures associated with the p_table.
 */
void
dump_parts()
{
	smd_service_t	*p;
	int		i;

	for (i = 0; i < P_HASH_SIZE + 1; i++) {
		if (p_table[ i ])
			fprintf(stderr, "Table index %d:\n", i);
		for (p = p_table[ i ]; p; p = p->next) {
			dump_service(p, "  ");
			fprintf(stderr, "\n");
		}
	}
}

/*
 *  dump_apps:
 *
 *	Dump all smd_app_t structures associated with the a_table.
 */
void
dump_apps()
{
	smd_app_t	*a;
	int		i;

	for (i = 0; i < A_HASH_SIZE; i++) {
		if (a_table[ i ])
			fprintf(stderr, "Table index %d:\n", i);
		for (a = a_table[ i ]; a; a = a->next) {
			dump_app(a, "  ");
			fprintf(stderr, "\n");
		}
	}
}

/*
 *  dump_user:
 *
 *	Dump a single smd_user_t structure and it's associate services.
 */
dump_user(user, str)
	smd_user_t *user;
	char	   *str;
{
	smd_service_t	*p;

	fprintf(stderr, "%sUser(%x): next = %x prev = %x fd = %d\n",
			str, user, user->next, user->prev, user->fd);
	
	for (p = user->service; p; p = p->user_link_next) {
		dump_service(p, str);
		fprintf(stderr, "\n");
	}
}

/*
 *  dump_service:
 *
 *	Dump a single smd_service_t structure.
 */
dump_service(serv, str)
	smd_service_t	*serv;
	char		*str;
{
	fprintf(stderr,
"%sService(%x): next = %x prev = %x user = %x user_next = %x user_prev = %x\n",
				str, serv, serv->next, serv->prev, serv->user,
				serv->user_link_next, serv->user_link_prev);
	dump_qualifier(&serv->q_id, str);
	dump_params(&serv->param, str);
}

/*
 *  dump_qualifier:
 *
 *	Dump a smd_qualifier structure.
 */
dump_qualifier(qid, str)
	smd_qualifier_t	*qid;
	char		*str;
{
	fprintf(stderr,
"%sQualifier(%x): pgid = %d part_id = %d acct_id = %d, uid = %d req_id = %s\n",
	str, qid, qid->pgid, qid->part_id, qid->acct_id, qid->uid, qid->req_id);
}

/*
 *  dump_params:
 *
 *	Dump a smd_params structure.
 */
dump_params(p, str)
	params_t	*p;
	char		*str;
{
	fprintf(stderr,
"%sParams(%x): flags = %x sig_num = %d sig_pid = %d events = %x alarm_val = %d\n",
	str, p, p->flags, p->sig_num, p->sig_pid, p->events, p->alarm_val);
}

/*
 *  dump_app:
 *
 *	Dump a single smd_app_t structure.
 */
dump_app(app, str)
	smd_app_t *app;
	char	  *str;
{
	fprintf(stderr,
"%sApp(%x): next = %x prev = %x pgid = %d acct_id = %d uid = %d\n",
  str, app,app->next,app->prev,app->data.pgid,app->data.acct_id,app->data.uid);

	fprintf(stderr,
"%s         : app_size = %d app_rollin = %d app_priority = %d\n",
	str, app->data.app_size, app->data.app_rollin, app->data.app_priority);

	fprintf(stderr,
"%s         : part_id = %d part_size = %d rollin_quantum = %d\n",
	str, app->data.part_id, app->data.part_size, app->data.rollin_quantum);

	fprintf(stderr,
"%s         : part_rollin = %d global_time = %f event_time = %d\n",
	str,app->data.part_rollin, app->data.global_time, app->data.event_time);
}

#endif
