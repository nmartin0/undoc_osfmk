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
/*
 * OSF/1 Release 1.0
 * 
 */

/* 
 * tpfl_desc.h
 * Contains typedefs for the descriptors that connect type and flag
 * values with their corresponding strings.
 * This version was written for odump to use with rose_names.h, but
 * the structures are general-purpose.
 */

#ifndef	array_count
#define	array_count(array)	(sizeof(array)/sizeof(array[0]))
#endif

typedef struct type_desc {
	char *string;
	int   type;
} type_desc_t;

typedef struct flags_desc {
	char *string;
	long   flag;
} flags_desc_t;

extern void
type_name_print __ ((int, type_desc_t *, int));

extern void
type_name_get __ ((int, type_desc_t *, int, char *));  /* last arg is 50 char buf */

extern void
flag_names_print __ ((long, long, flags_desc_t *, int));

extern void
flag_names_get __ ((long, long, flags_desc_t *, int, char *)); /* last arg is 80 char buf */



