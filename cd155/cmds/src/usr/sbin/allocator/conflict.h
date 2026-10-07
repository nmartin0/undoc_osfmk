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
 
extern void init_conflict_list(conflict_list_t *conflicts);
extern void build_conflict_list(CONSUMER_T *object,PART_T *part);
extern conflict_list_t * get_conflict_list(CONSUMER_T *object);
extern void insert_conflict(CONSUMER_T *object,CONSUMER_T *c);

