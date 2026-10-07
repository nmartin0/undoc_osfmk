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
 * node.c
 *
 * ASCII to binary (and vice versa) node list conversion routines.
 */
#include "reset_bnl.h"

/*
 * node_number_to_node_string(count, list, string)
 *
 * Make an ascii node list from an array of longs.  Optimize by looking
 * for consecutive node numbers.
 */
int
node_number_to_node_string(count, list, string)
long    count;
long    *list;
char    *string;
{
	long    anchor;
	long    prev;
	int     range = 0;
	int	at_least_one;
 
	anchor = *list++;
	while ((count > 0) && (anchor == BAD_NODE)) {
		anchor = *list++;
		count--;
	}
	at_least_one = count;

	prev = anchor;
	while (--count > 0) {
		if (*list == BAD_NODE) {
			list++;
		}
		else {
			if (*list == (prev + 1)) {
				range = 1;
				prev = *list++;
			} else 	{
				if (range)
					sprintf(string, "%d..%d,", anchor,prev);
				else
					sprintf(string, "%d,", anchor);
 
				string += strlen(string);
				anchor = prev = *list++;
				range = 0;
			}
		}
	}
 
	if (range)
		sprintf(string, "%d..%d", anchor, prev);
	else if ((anchor != BAD_NODE) && at_least_one) 
		sprintf(string, "%d", anchor);
}
 

