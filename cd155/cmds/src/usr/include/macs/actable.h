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
 
/* In what order you want the data be sorted, summarized and reported
 * The valid keys for all the data items are grouped into 3 levels for
 * the purpose of sorting:
 *    account level:
 *       a       account name
 *       l       login name
 *    job level:
 *       q       queue type
 *    usage level:
 *       p       average job-partition size
 *       c       total cpu-hours
 *       i       total idle cpu-hours
 *       u       total under-used cpu-hours
 *       t       total charge
 * Only one key from each group can be used at a time to specify a
 * desired sorting sequence for an accounting report and an account
 * level key must be immediately following an usage level key.
 * If the SORT_ORDER is defined to be "clq", then the report will
 * show the biggest cpu-time consumer in the period first; within
 * an user is sorted by account; within an user account is sorted
 * by queue type.
 * If no sorting sequence is specified, the report will be sorted
 * by account (sorted by site, within a site is sorted by project and
 * within a project is sorted by group), then within a project is
 * sorted by login name, within a login name is sorted by queue type
 * ... and so on
 * SORT_KEY_NAME correspond to keys in DEFAULT_SORT
 */
 
#define SORT_LEVELS     3
#define SORT_KEYS       8

/* default sort order is also the column order of jrec output */
#define DEFAULT_SORT    "alqpciut"

/* index for SORT_KEY_GRP */
#define ACCT_GROUP	0
#define QUEUE_GROUP	1
#define USAGE_GROUP	2

/* number of keys in each group */
#define N_ACCT_KEYS	2
#define N_QUEUE_KEYS	1
#define N_USAGE_KEYS	5

/* queue types */
#define INTERACTIVE	0
#define BATCH		1
#define DEDICATE	2

struct actable {
	int maxnodes;
	int n_bins;
	int *bins;
	int n_actlevel;
	int *actlevel;
	char **actlabel;
	int n_batchq;
	char **batchq;
	int n_dedicateq;
	char **dedicateq;
};

extern struct actable *get_actab();
