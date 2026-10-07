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
 * interp_tpfl.c
 * Routines to print or return the names of types or flags.  The names and
 * their corresponding values are passed in as an array.
 */
 
#include <standards.h>
#include "tpfl_desc.h"
#include <string.h>


void
flag_names_print(flags, mask, array, count)
        long flags;
        long mask;
	flags_desc_t *array;
        int count;
{
	int printed = 0;
	int first = 1;
	int i;

	for (i = 0; i < count; i++)
		if (array[i].flag == flags) {
			printf("%s", array[i].string);
			return;
		}

	for (i = 0; i < count; i++)
		if (flags & array[i].flag) {
			if (first)
				first = 0;
			else
				printf(",");
			if (!printed)
				printed = 1;
			printf("%s", array[i].string);
		}
	if ((!printed) || ((flags & mask) != 0)) {
		if (first == 0) printf(",%#x", (flags & mask));
		else printf("%#x", (flags & mask));
	}
}

void
flag_names_get(flags, mask, array, count, name)
        long flags;
        long mask;
	flags_desc_t *array;
        int count;
	char *name;			/* an 80-char buffer */
{
	int printed = 0;
	int first = 1;
	int i;
	char *null_string = "";
	char flag_buf[20];
	char *hex_flags = flag_buf;

	for (i = 0; i < count; i++)
		if (array[i].flag == flags) {
			strcpy(name, array[i].string);
			return;
		}

	strcpy(name, null_string);
	for (i = 0; i < count; i++)
		if (flags & array[i].flag) {
			if (first)
				first = 0;
			else
				strcat(name, ",");
			if (!printed)
				printed = 1;
			strcat(name, array[i].string);
		}
	if ((!printed) || ((flags & mask) != 0)) {
		if (first == 0) strcat(name, ",");
		sprintf(hex_flags, "%#x", (flags & mask));
		strcat(name, hex_flags);
	}
}


void
type_name_print (type, array, count)
        int type;
        type_desc_t *array;
        int count;
{
	int i;

	for (i = 0; i < count; i++)
		if (array[i].type == type) {
			printf("%s", array[i].string);
			return;
		}
	printf("%d", type);
}


void
type_name_get (type, array, count, name)
        int type;
        type_desc_t *array;
        int count;
        char *name;
{
	int i;

	for (i = 0; i < count; i++)
		if (array[i].type == type) {
			strcpy(name, array[i].string);
			return;
		}
	sprintf(name, "%d", type);
}
