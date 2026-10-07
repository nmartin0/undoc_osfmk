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
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/reset_boot_node_list/parsenode.c,v 1.2 1994/11/21 16:47:37 mtm Exp $
 *
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

/*
 * Recursive descent parser for number lists of the form
 *
 *	123,456,789..1000
 */

extern int	debug;

static char	*p;
static int	*r;

parse_nodestring(nodestring, nodelist)
	char	*nodestring;
	int	*nodelist;
{
	int	i;

	r = nodelist;
	p = nodestring;
	while ((i = parse_node_element()) == 1);
	if (i == -1) {
		return -1;
	}
	return r - nodelist;
}

parse_node_element()
{

	if (parse_node_range() == -1) {
		return -1;
	}
	if (*p == ',') {
		p++;
		return 1;
	}
	return 0;
}

parse_node_range()
{
	int	m, n;

	if ((m = parse_node_number()) == -1) {
		return -1;
	}
	if (*p == '.') {
		p++;
		if (*p++ != '.') {
			return -1;
		}
		if ((n = parse_node_number()) == -1) {
			return -1;
		}
		while (m <= n) {
			*r++ = m++;
		}
	} else {
		*r++ = m;
	}
	return 0;
}


parse_node_number()
{
	int	v;

	v = 0;
	if (!(*p >= '0' && *p <= '9')) {
		return -1;
	}
	while (*p >= '0' && *p <= '9') {
		v = 10*v + *p++ - '0';
	}
	return v;
	return 0;
}
