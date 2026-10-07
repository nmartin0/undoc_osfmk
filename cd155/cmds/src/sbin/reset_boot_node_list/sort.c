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
 * sort.c
 *
 * Sort and search routines.
 */
#include "reset_bnl.h"

/*
 * compare(l1, l2)
 *
 * Comparison function for qsort() to use.
 *
 */
int
compare(l1, l2)
long	*l1, *l2;
{
	if (*l1 == *l2)
		return 0;
	else if (*l1 < *l2)
		return -1;
	else
		return 1;
}


/*
 * sort(count, list)
 *
 * Sort list of longs in ascending order.
 */
int
sort(count, list)
int	count;		/* Number of elements in list */
long	*list;		/* List of longs to sort */
{
	qsort(list, count, sizeof(long), compare);
} 


/*
 * search(count, list, n))
 *
 * Search for n in list of longs. Return TRUE if n is found, FALSE otherwise.
 */
int
search(count, list, n)
int	count;		/* Number of elements is list */
long	*list;		/* List of longs to search */
long	n;		/* Number to search for */
{
	return (int) bsearch(&n, list, count, sizeof(long), compare);
}
