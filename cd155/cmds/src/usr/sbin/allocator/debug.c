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
 *
 *      $Header: 
 *
 */

/* History:
 *      $Log: 
 */

#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <nx/bitmap.h>
#include <nx/defines.h>

#include "debug.h"

extern int allocation_debug, sched_debug, 
    signal_debug, compare_debug, conflict_debug;

int trace_debug = 0;

/*VARARGS1*/
void
debug_print(ident, level, fmt, a, b, c, d, e, f)
    int ident;
    int level;
    char *fmt;
{
    char dstr[256];
    int print_it = FALSE;
    int debug_level;

    int debug_alloc_level = 5;
    int debug_sched_level = 5;
    int debug_signal_level = 5;
    int debug_compare_level = 5;
    int debug_conflict_level = 5;
    int debug_other_level = 99;
    int debug_trace_level = 5;

  switch(ident)
	{
	case DEBUG_ALLOC:
	    debug_level = debug_alloc_level;
	    if (allocation_debug) {
		print_it = TRUE;
	    }
	    break;
	case DEBUG_SCHED:
	    debug_level = debug_sched_level;
	    if (sched_debug) {
		print_it = TRUE;
	    }
	    break;
	case DEBUG_SIGNAL:
	    debug_level = debug_signal_level;
	    if (signal_debug) {
		print_it = TRUE;
	    }
	    break;
	case DEBUG_COMPARE:
	    debug_level = debug_compare_level;
	    if (compare_debug) {
		print_it = TRUE;
	    }
	    break;
	case DEBUG_CONFLICT:
	    debug_level = debug_conflict_level;
	    if (conflict_debug) {
		print_it = TRUE;
	    }
	    break;
	case DEBUG_OTHER:
	    debug_level = debug_other_level;
	    print_it = TRUE;
	    break;
	case DEBUG_TRACE:
	    debug_level = debug_trace_level;
	    if (trace_debug) {
		print_it = TRUE;
	    }
	    break;
	default:
	    break;
	}

    if (print_it) {
	if (level <= debug_level) {
	    sprintf(dstr, fmt, a, b, c, d, e, f);
	    printf("%lu: %s", time(0L), dstr);
	}
    }

}

/*VARARGS1*/
void
debug_conflict(level, fmt, a, b, c, d, e, f)
    int level;
    char *fmt;
{
    (void)debug_print(DEBUG_CONFLICT, level, fmt, a,b,c,d,e,f);
}

/*VARARGS1*/
void
debug_signal(level, fmt, a, b, c, d, e, f)
    int level;
    char *fmt;
{
    (void)debug_print(DEBUG_SIGNAL, level, fmt, a,b,c,d,e,f);
}

/*VARARGS1*/
void
debug_compare(level, fmt, a, b, c, d, e, f)
    int level;
    char *fmt;
{
    (void)debug_print(DEBUG_COMPARE, level, fmt, a,b,c,d,e,f);
}
/*VARARGS1*/
void
debug_sched(level, fmt, a, b, c, d, e, f)
    int level;
    char *fmt;
{
    (void)debug_print(DEBUG_SCHED, level, fmt, a,b,c,d,e,f);
}
/*VARARGS1*/
void
debug_allocation(level, fmt, a, b, c, d, e, f)
    int level;
    char *fmt;
{
    (void)debug_print(DEBUG_ALLOC, level, fmt, a,b,c,d,e,f);
}
/*VARARGS1*/
void
debug_trace(level, fmt, a, b, c, d, e, f)
    int level;
    char *fmt;
{
    (void)debug_print(DEBUG_TRACE, level, fmt, a,b,c,d,e,f);
}

void
debug_print_bitmap(int ident, BITMAP_T *descr)
{
    COLUMN_MAP_T mask;
    int i, j, k, start_node;
/*
    switch(ident)
	{
	case DEBUG_ALLOC:
	    if (allocation_debug)
		goto print_bitmap;
	    break;
	case DEBUG_SCHED:
	    if (sched_debug)
		goto print_bitmap;
	    break;
	case DEBUG_SIGNAL:
	    if (signal_debug)
		goto print_bitmap;
	    break;
	case DEBUG_COMPARE:
	    if (compare_debug)
		goto print_bitmap;
	    break;
	case DEBUG_CONFLICT:
	    if (conflict_debug)
		goto print_bitmap;
	    break;
	case DEBUG_OTHER:
	default:
	    goto print_bitmap;
	}
    return;
*/
print_bitmap:
    printf("    +-");
    for (j = 0; j < descr->cols; j++)
	printf("---");
    printf("+\n");
    
    mask = 1;
    start_node = 0;
    k=0;
    for (i = descr->row_offset; i < (descr->rows+descr->row_offset); i++) {
	printf("%4d| ", start_node);
	for (j = 0; j < descr->cols; j++) {
	    if (mask & descr->colmap[j]) {
		/* printf("* "); */
		printf("%2d ", k++);
	    } else {
		printf(" . ");
	    }
	}
	
	printf("|\n");
	mask <<= 1;
	start_node += descr->cols;
    }
    
    printf("    +-");
    for (j = 0; j < descr->cols; j++)
	printf("---");
    printf("+\n");
}
