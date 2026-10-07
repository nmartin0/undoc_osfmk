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
 */
/* @(#)$RCSfile: string.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:48:32 $ */
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	string.h	3.7 (Berkeley) 6/29/88
 */

#define STR_DEBUG

char *str_cpy();
char *str_ncpy();
char *str_cat();
char *str_itoa();

#define str_cmp(a, b)	strcmp(a, b)

#ifdef STR_DEBUG
struct string {
	struct string *s_forw;
	struct string *s_back;
	char s_data[1];
};

struct string str_head;

#define str_offset ((unsigned)str_head.s_data - (unsigned)&str_head)
#define str_stos(s) ((struct string *)((unsigned)(s) - str_offset))

char *str_alloc();
int str_free();
#else
#define str_free(s)	free(s)
#define str_alloc(s)	malloc(s)
#endif
