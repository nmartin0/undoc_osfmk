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
/* testsq1.c
 *
 * OSF/1 Release 1.0
 */

#include	<stdio.h>

#include	"squeue.h"

#define	NELT	20

struct	sq_elem	array[NELT];		/* the queue elements */
struct	squeue1	hdr;			/* the queue header */
char *functs = "functions are: <ah>|<at>|<aa>|<rh>|<rt>|<ra>|<d>| <elt> [<prev>]\n";
main (argc, argv)
int	argc;
char	**argv;
{
	char	func[80];
	int	elt, prev;
	struct	sq_elem	*e;
	
	sq1_init(&hdr);
	for (elt = 0; elt < NELT; elt++)
		sq1_init(&array[elt]);
	printf ("%s", functs);
	for (;;) {
		scanf ("%s", func);
		if (strcmp (func, "?") == 0) {
			printf ("%s", functs);
			continue;
		} else if (strcmp (func, "q") == 0) {
			exit (0);
		}
		scanf ("%d", &elt);
		if (elt < 0 || elt >= NELT) {
			printf ("no such element\n");
			continue;
		}
		if (strcmp (func, "ah") == 0) {
			sq1_ins_head(&hdr, &array[elt]);
		} else if (strcmp (func, "at") == 0) {
			sq1_ins_tail(&hdr, &array[elt]);
		} else if (strcmp (func, "aa") == 0) {
			scanf ("%d", &prev);
			sq1_ins_after(&array[elt], &array[prev]);
		} else if (strcmp (func, "d") == 0) {
			sq1_rem_elem(&hdr, &array[elt]);
		} else if (strcmp (func, "ra") == 0) {
			scanf ("%d", &prev);
			sq1_rem_after(&array[elt], &array[prev]);
		} else if (strcmp (func, "rh") == 0) {
			e = sq1_rem_head(&hdr);
			if (e != NULL)
				printf("Removed %d\n", (e - array));
			else
				printf("Queue empty\n");
		} else if (strcmp (func, "rt") == 0) {
			e = sq1_rem_tail(&hdr);
			if (e != NULL)
				printf("Removed %d\n", (e - array));
			else
				printf("Queue empty\n");
		} else {
			printf ("no such function\n");
			continue;
		}
		show_que(&hdr);
	}
}


show_que(q)
register struct sq_elem	*q;
{
	register struct sq_elem	*ent;
	register int	len;
	
	for (ent = q->sq_forw, len = 0; ent != NULL;
	     ent = ent->sq_forw, len++) {
		if (len > NELT) {
			printf("looping, break...\n");
			return;
		}
		printf ("\telement %3d", (ent - array));
		if (ent->sq_forw != NULL)
			printf("  forw %3d\n", (ent->sq_forw - array));
		else
			printf("\n");
	}
}
