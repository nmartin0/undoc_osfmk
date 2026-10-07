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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* dqueue.c
 * This file contains the routines to enqueue and dequeue objects.
 * These routines are identical to the standard VAX C library
 * insque and remque functions, with two differences:
 *	1) this remque returns its argument as its value
 *	2) this remque guarantees that the forward and backward links of
 *	   the dequeued element point to itself.
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <loader.h>

#include "ldr_types.h"
#include "dqueue.h"


void
insque(struct dqueue_elem *elem, struct dqueue_elem *pred)
{
	elem->dq_forw = pred->dq_forw;
	elem->dq_back = pred;
	pred->dq_forw->dq_back = elem;
	pred->dq_forw = elem;
}

struct dqueue_elem *
remque(struct dqueue_elem *elem)
{
	elem->dq_forw->dq_back = elem->dq_back;
	elem->dq_back->dq_forw = elem->dq_forw;
	elem->dq_forw = elem->dq_back = elem;
	return(elem);
}
