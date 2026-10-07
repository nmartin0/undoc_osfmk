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
static char sccsid[] = "@(#)strdata.c	1.3  com/cmd/ksh/shlib,3.1,9013 9/10/89 06:39:34";
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 * data for string evaluator library
 */

#include	"streval.h"

const struct Optable optable[] =
	/* opcode	precedence,assignment  opname */
{
	{DEFAULT,	MAXPREC|NOASSIGN,	0177, 0 },
	{DONE,		0|NOASSIGN,		0 , 0 },
	{NEQ,		9|NOASSIGN,		'!', '=' },
	{NOT,		MAXPREC|NOASSIGN,	'!', 0 },
	{MOD,		13|NOFLOAT,		'%', 0 },
	{ANDAND,	5|NOASSIGN|SEQPOINT,	'&', '&' },
	{AND,		8|NOFLOAT,		'&', 0 },
	{LPAREN,	MAXPREC|NOASSIGN|SEQPOINT,'(', 0 },
	{RPAREN,	1|NOASSIGN,		')', 0 },
	{TIMES,		13,			'*', 0 },
#ifdef future
	{PLUSPLUS,	14|NOASSIGN|NOFLOAT|SEQPOINT, '+', '+'},
#endif
	{PLUS,		12,			'+', 0 },
#ifdef future
	{MINUSMINUS,	14|NOASSIGN|NOFLOAT|SEQPOINT, '-', '-'},
#endif
	{MINUS,		12,			'-', 0 },
	{DIVIDE,	13,			'/', 0 },
#ifdef future
	{COLON,		2|NOASSIGN,		':', 0 },
#endif
	{LSHIFT,	11|NOFLOAT,		'<', '<' },
	{LE,		10|NOASSIGN,		'<', '=' },
	{LT,		10|NOASSIGN,		'<', 0 },
	{EQ,		9|NOASSIGN,		'=', '=' },
	{ASSIGNMENT,	2|RASSOC,		'=', 0 },
	{RSHIFT,	11|NOFLOAT,		'>', '>' },
	{GE,		10|NOASSIGN,		'>', '=' },
	{GT,		10|NOASSIGN,		'>', 0 },
#ifdef future
	{QCOLON,	3|NOASSIGN|SEQPOINT,	'?', ':' },
	{QUEST,		3|NOASSIGN|SEQPOINT|RASSOC,	'?', 0 },
#endif
	{XOR,		7|NOFLOAT,		'^', 0 },
	{OROR,		5|NOASSIGN|SEQPOINT,	'|', '|' },
	{OR,		6|NOFLOAT,		'|', 0 }
};

#ifdef MSG
/*
 *   NOTE: These messages must also be externalized, and represented
 *         in the catalog and in ksh_msg.h. 
 */
#endif
#ifndef KSHELL
    const char e_number[]	= "bad number";
#endif /* KSHELL */
const char e_moretokens[]	= "more tokens expected";
const char e_paren[]		= "unbalanced parenthesis";
const char e_badcolon[]		= "invalid use of :";
const char e_divzero[]		= "divide by zero";
const char e_synbad[]		= "syntax error";
const char e_notlvalue[]	= "assignment requires lvalue";
const char e_recursive[]	= "recursion too deep";
#ifdef future
    const char e_questcolon[]	= ": expected for ? operator";
#endif
#ifdef FLOAT
    const char e_incompatible[]= "operands have incompatible types";
#endif /* FLOAT */

const char e_hdigits[] = "00112233445566778899aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ"; /*NOTX*/
