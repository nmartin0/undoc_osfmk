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
 
/*++ secgrfir.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/secgrfir.c,v $
 *
 * DESCRIPTION:
 *
 *	Given two quota limits, report whether the second is greater
 *	than the first. If it is, return 1.  If not, return 0.
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	January 24, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:48 $ $State: Exp $)
 * $Log: secgrfir.c,v $
 * Revision 1.3  1994/11/19  02:27:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:47  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:53  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:55:06  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "quolim.h"	/* for struct quotalimit and quota limit units */

/*** secgrfir
 *
 *
 *	int secgrfir():
 *
 *	Given two quota limits, return 1 if the second limit
 *	is greater than the first limit. Otherwise return 0.
 *
 *	This function was originally designed to return 1 whenever
 *	the units were unequal. Then I considered the example
 *	1.2 GB, 1 GB.  The fractional part algorithm converts
 *	the UNITS of the first to MEGABYTES! Once the 12 comparisons
 *	to handle this were in, it seemed natural to add the remaining
 *	44.  This in turn allowed grandfather clauses and other benefits.
 *
 *	It is assumed that the number of bits in an unsigned
 *	long is at least 32.  It is further assumed that first_quota
 *	and second_quota are each < 2**32.
 */
int secgrfir (first_quota, first_units, second_quota, second_units)
unsigned long first_quota;
short first_units;
unsigned long second_quota;
short second_units;
{
    if (first_units == 0 || second_units == 0) {
	return (1);
    }
    /*
     * Below, we handle the 8 pairs where the units are identical.
     */
    if (first_units == second_units) {
	 return (second_quota > first_quota);
    }
    /*
     * Below, we handle the 56 remaining possible pairs of units.
     * Where the first units are coarser than the second units,
     * we have to add 1023, 1048575, 1073741823, BYTES_PER_WORD -1,
     * (BYTES_PER_WORD * 1024) -1, (BYTES_PER_WORD * 10485476) -1,
     * or (BYTES_PER_WORD * 1073741824) -1 to handle the case
     * "1 kb 1025 b", which would otherwise appear legal.
     */
    switch (first_units) {
    case QLM_BYTES:
	switch (second_units) {
	case QLM_WORDS:
	    return (first_quota / BYTES_PER_WORD < second_quota);
	case QLM_KBYTES:
	    return (first_quota / 1024 < second_quota);
	case QLM_KWORDS:
	    return ((first_quota / 1024) / BYTES_PER_WORD < second_quota);
	case QLM_MBYTES:
	    return (first_quota / 1048576L < second_quota);
	case QLM_MWORDS:
	    return ((first_quota / 1048576L) / BYTES_PER_WORD < second_quota);
	case QLM_GBYTES:
	    return (first_quota / 1073741824L < second_quota);
	case QLM_GWORDS:
	    return ((first_quota / 1073741824L) / BYTES_PER_WORD <
		second_quota);
	}
    case QLM_WORDS:
	switch (second_units) {
	case QLM_BYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - (BYTES_PER_WORD - 1)) {
		return (first_quota <= second_quota / BYTES_PER_WORD);
	    }
	    return (first_quota <
		(second_quota + (BYTES_PER_WORD - 1)) / BYTES_PER_WORD);
	case QLM_KBYTES:
	    if ((first_quota / 1024) * BYTES_PER_WORD >= second_quota) {
		return (0);
	    }
	    if ((first_quota / 1024 == second_quota / BYTES_PER_WORD) &&
		((first_quota % 1024) * BYTES_PER_WORD >=
		(second_quota % BYTES_PER_WORD) * 1024)) {
		return (0);
	    }
	    return (1);
	case QLM_KWORDS:
	    return (first_quota / 1024 < second_quota);
	case QLM_MBYTES:
	    if ((first_quota / 1048576L) * BYTES_PER_WORD >= second_quota) {
		return (0);
	    }
	    if ((first_quota / 1048576L == second_quota / BYTES_PER_WORD) &&
		((first_quota % 1048576L) * BYTES_PER_WORD >=
		(second_quota % BYTES_PER_WORD) * 1048576L)) {
		return (0);
	    }
	    return (1);
	case QLM_MWORDS:
	    return (first_quota / 1048576L < second_quota);
	case QLM_GBYTES:
	    if ((first_quota / 1073741824L) * BYTES_PER_WORD >= second_quota) {
		return (0);
	    }
	    /*
	     * In the test below, we give up a factor of 8 in accuracy
	     * to ensure that the multiplication does not overflow.
	     */
	    if ((first_quota / 1073741824L == second_quota / BYTES_PER_WORD) &&
		(((first_quota % 1073741824L) / 8) * BYTES_PER_WORD >=
		 (second_quota % BYTES_PER_WORD) * (1073741824L / 8)
		)
	       ) {
		return (0);
	    }
	    return (1);
	case QLM_GWORDS:
	    return (first_quota / 1073741824L < second_quota);
	}
    case QLM_KBYTES:
	switch (second_units) {
	case QLM_BYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1023) {
		return (first_quota < 4194304L);
	    }
	    else {
		return (first_quota < (second_quota + 1023)/1024);
	    }
	case QLM_WORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - (1024 - 1)) {
		return ( first_quota < (4194304L * BYTES_PER_WORD));
	    }
	    if (first_quota / BYTES_PER_WORD >=
		(second_quota + (1024 - 1)) / 1024) {
		return (0);
	    }
	    if ((first_quota / BYTES_PER_WORD == second_quota / 1024) &&
		((first_quota % BYTES_PER_WORD) * 1024 >=
		(second_quota % 1024) * BYTES_PER_WORD)) {
		return (0);
	    }
	    return (1);
	case QLM_KWORDS:
	    return (first_quota / BYTES_PER_WORD < second_quota);
	case QLM_MBYTES:
	    return (first_quota / 1024 < second_quota);
	case QLM_MWORDS:
	    return ((first_quota / 1024) / BYTES_PER_WORD < second_quota);
	case QLM_GBYTES:
	    return (first_quota / 1048576L < second_quota);
	case QLM_GWORDS:
	    return ((first_quota / 1048576L) / BYTES_PER_WORD < second_quota);
	}
    case QLM_KWORDS:
	switch (second_units) {
	case QLM_BYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - ((BYTES_PER_WORD * 1024) -1)) {
		return (first_quota <= second_quota / (BYTES_PER_WORD * 1024));
	    }
	    return (first_quota <
		(second_quota + ((BYTES_PER_WORD * 1024) -1)) /
			(BYTES_PER_WORD * 1024));
	case QLM_WORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1023) {
		return (first_quota < 4194304L);
	    }
	    else {
		return (first_quota < (second_quota + 1023)/1024);
	    }
	case QLM_KBYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - (BYTES_PER_WORD -1)) {
		return (first_quota <= second_quota / BYTES_PER_WORD);
	    }
	    return (first_quota <
		(second_quota + (BYTES_PER_WORD - 1)) / BYTES_PER_WORD);
	case QLM_MBYTES:
	    if ((first_quota / 1024) * BYTES_PER_WORD >= second_quota) {
		return (0);
	    }
	    if ((first_quota / 1024 == second_quota / BYTES_PER_WORD) &&
		((first_quota % 1024) * BYTES_PER_WORD >=
		(second_quota % BYTES_PER_WORD) * 1024)) {
		return (0);
	    }
	    return (1);
	case QLM_MWORDS:
	    return (first_quota / 1024 < second_quota);
	case QLM_GBYTES:
	    if ((first_quota / 1048576L) * BYTES_PER_WORD >= second_quota) {
		return (0);
	    }
	    if ((first_quota / 1048576L == second_quota / BYTES_PER_WORD) &&
		((first_quota % 1048576L) * BYTES_PER_WORD >=
		(second_quota % BYTES_PER_WORD) * 1048576L)) {
		return (0);
	    }
	    return (1);
	case QLM_GWORDS:
	    return (first_quota / 1048576L < second_quota);
	}
    case QLM_MBYTES:
	switch (second_units) {
	case QLM_BYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1048575L) {
		return (first_quota < 4096);
	    }
	    else {
		return (first_quota < (second_quota + 1048575L)/1048576L);
	    }
	case QLM_WORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - (1048576L - 1)) {
		return ( first_quota < (4096L * BYTES_PER_WORD));
	    }
	    if (first_quota / BYTES_PER_WORD >=
		(second_quota + (1048576L - 1)) / 1048576L) {
		return (0);
	    }
	    if ((first_quota / BYTES_PER_WORD == second_quota / 1048576L) &&
		((first_quota % BYTES_PER_WORD) * 1048576L >=
		(second_quota % 1048576L) * BYTES_PER_WORD)) {
		return (0);
	    }
	    return (1);
	case QLM_KBYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1023) {
		return (first_quota < 4194304L);
	    }
	    else {
		return (first_quota < (second_quota + 1023)/1024);
	    }
	case QLM_KWORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - (1024 - 1)) {
		return ( first_quota < (4194304L * BYTES_PER_WORD));
	    }
	    if (first_quota / BYTES_PER_WORD >=
		(second_quota + (1024 - 1)) / 1024) {
		return (0);
	    }
	    if ((first_quota / BYTES_PER_WORD == second_quota / 1024) &&
		((first_quota % BYTES_PER_WORD) * 1024 >=
		(second_quota % 1024) * BYTES_PER_WORD)) {
		return (0);
	    }
	    return (1);
	case QLM_MWORDS:
	    return (first_quota / BYTES_PER_WORD < second_quota);
	case QLM_GBYTES:
	    return (first_quota / 1024 < second_quota);
	case QLM_GWORDS:
	    return ((first_quota / 1024) / BYTES_PER_WORD < second_quota);
	}
    case QLM_MWORDS:
	switch (second_units) {
	case QLM_BYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - ((BYTES_PER_WORD * 1048576L) -1)) {
		return (first_quota <=
			second_quota / (1048576L * BYTES_PER_WORD));
	    }
	    return (first_quota <
		(second_quota + ((BYTES_PER_WORD * 1048576L) -1)) /
			(BYTES_PER_WORD * 1048576L));
	case QLM_WORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1048575L) {
		return (first_quota < 4096);
	    }
	    else {
		return (first_quota < (second_quota + 1048575L)/1048576L);
	    }
	case QLM_KBYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - ((BYTES_PER_WORD * 1024) -1)) {
		return (first_quota <= second_quota / (BYTES_PER_WORD * 1024));
	    }
	    return (first_quota <
		(second_quota + ((BYTES_PER_WORD * 1024) -1)) /
			(BYTES_PER_WORD * 1024));
	case QLM_KWORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1023) {
		return (first_quota < 4194304L);
	    }
	    else {
		return (first_quota < (second_quota + 1023)/1024);
	    }
	case QLM_MBYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - (BYTES_PER_WORD -1)) {
		return (first_quota <= second_quota / BYTES_PER_WORD);
	    }
	    return (first_quota <
		(second_quota + (BYTES_PER_WORD - 1)) / BYTES_PER_WORD);
	case QLM_GBYTES:
	    if ((first_quota / 1024) * BYTES_PER_WORD >= second_quota) {
		return (0);
	    }
	    if ((first_quota / 1024 == second_quota / BYTES_PER_WORD) &&
		((first_quota % 1024) * BYTES_PER_WORD >=
		(second_quota % BYTES_PER_WORD) * 1024)) {
		return (0);
	    }
	    return (1);
	case QLM_GWORDS:
	    return (first_quota / 1024 < second_quota);
	}
case QLM_GBYTES:
	switch (second_units) {
	case QLM_BYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1073741823L) {
		return (first_quota < 4);
	    }
	    else {
		return (first_quota < (second_quota + 1073741823L)/1073741824L);
	    }
	case QLM_WORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - (1073741824L - 1)) {
		return ( first_quota < (4L * BYTES_PER_WORD));
	    }
	    if (first_quota / BYTES_PER_WORD >=
		(second_quota + (1073741824L - 1)) / 1073741824L) {
		return (0);
	    }
	    /*
	     * In the test below, we give up a factor of 8 in accuracy
	     * to ensure that the multiplication does not overflow.
	     */
	    if ((first_quota / BYTES_PER_WORD == second_quota / 1073741824L) &&
		((first_quota % BYTES_PER_WORD) * (1073741824L / 8)  >=
		((second_quota % 1073741824L) / 8) * BYTES_PER_WORD)) {
		return (0);
	    }
	    return (1);
	case QLM_KBYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1048575L) {
		return (first_quota < 4096);
	    }
	    else {
		return (first_quota < (second_quota + 1048575L)/1048576L);
	    }
	case QLM_KWORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - (1048576L - 1)) {
		return ( first_quota < (4096L * BYTES_PER_WORD));
	    }
	    if (first_quota / BYTES_PER_WORD >=
		(second_quota + (1048576L - 1)) / 1048576L) {
		return (0);
	    }
	    if ((first_quota / BYTES_PER_WORD == second_quota / 1048576L) &&
		((first_quota % BYTES_PER_WORD) * 1048576L >=
		(second_quota % 1048576L) * BYTES_PER_WORD)) {
		return (0);
	    }
	    return (1);
	case QLM_MBYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1023) {
		return (first_quota < 4194304L);
	    }
	    else {
		return (first_quota < (second_quota + 1023)/1024);
	    }
	case QLM_MWORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - (1024 - 1)) {
		return ( first_quota < (4194304L * BYTES_PER_WORD));
	    }
	    if (first_quota / BYTES_PER_WORD >=
		(second_quota + (1024 - 1)) / 1024) {
		return (0);
	    }
	    if ((first_quota / BYTES_PER_WORD == second_quota / 1024) &&
		((first_quota % BYTES_PER_WORD) * 1024 >=
		(second_quota % 1024) * BYTES_PER_WORD)) {
		return (0);
	    }
	    return (1);
	case QLM_GWORDS:
	    return (first_quota / BYTES_PER_WORD < second_quota);
	}
    case QLM_GWORDS:
	switch (second_units) {
	case QLM_BYTES:
	    if (BYTES_PER_WORD >= 4) {
		/*
		 * Recall that second_quota is < 2**32.
		 */
		return (first_quota == 0 && second_quota > 0);
	    }
	    else {
		/* Before adding, check for overflow. */
		if (second_quota >
		    4294967295L - ((BYTES_PER_WORD * 1073741824L) -1)) {
		    return (first_quota <=
			second_quota / (1073741824L * BYTES_PER_WORD));
		}
		return (first_quota <
		    (second_quota + ((BYTES_PER_WORD * 1073741824L) -1)) /
		    (BYTES_PER_WORD * 1073741824L));
	    }
	case QLM_WORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1073741823L) {
		return (first_quota < 4);
	    }
	    else {
		return (first_quota < (second_quota + 1073741823L)/1073741824L);
	    }
	case QLM_KBYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - ((BYTES_PER_WORD * 1048576L) -1)) {
		return (first_quota <=
			second_quota / (1048576L * BYTES_PER_WORD));
	    }
	    return (first_quota <
		(second_quota + ((BYTES_PER_WORD * 1048576L) -1)) /
		(BYTES_PER_WORD * 1048576L));
	case QLM_KWORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1048575L) {
		return (first_quota < 4096);
	    }
	    else {
		return (first_quota < (second_quota + 1048575L)/1048576L);
	    }
	case QLM_MBYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - ((BYTES_PER_WORD * 1024) -1)) {
		return (first_quota <= second_quota / (BYTES_PER_WORD * 1024));
	    }
	    return (first_quota <
		(second_quota + ((BYTES_PER_WORD * 1024) -1)) /
		(BYTES_PER_WORD * 1024));
	case QLM_MWORDS:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - 1023) {
		return (first_quota < 4194304L);
	    }
	    else {
		return (first_quota < (second_quota + 1023)/1024);
	    }
	case QLM_GBYTES:
	    /* Before adding, check for overflow. */
	    if (second_quota > 4294967295L - (BYTES_PER_WORD -1)) {
		return (first_quota <= second_quota / BYTES_PER_WORD);
	    }
	    return (first_quota <
		(second_quota + (BYTES_PER_WORD - 1)) / BYTES_PER_WORD);
	}
    }
}				/* end secgrfir () */
