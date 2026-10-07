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
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 *
 *	@(#)regexp.h	1.1 (Berkeley) 9/29/87
 */
/***************************************************************************
 *
 *    Copyright (c) 1992 Intel Corporation
 *
 *    This file is part of pmake, an extension of GNU Make.
 *    Pmake is distributed by the Intel Corporation under the
 *    terms of the GNU General Public License.
 *
 *    Title:
 *        $Id: regexp.h,v 1.2 1994/11/19 01:34:00 mtm Exp $
 *
 *    Description:
 *        Regular expression definitions.
 *
 *    History: 
 * $Log: regexp.h,v $
 * Revision 1.2  1994/11/19  01:34:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/04  22:04:59  raya
 * Initial checkin of pmake sources to the commands and libraries
 * tree.
 *
 *
 **************************************************************************/
#define NSUBEXP  10
typedef struct regexp {
	char *startp[NSUBEXP];
	char *endp[NSUBEXP];
	char regstart;		/* Internal use only. */
	char reganch;		/* Internal use only. */
	char *regmust;		/* Internal use only. */
	int regmlen;		/* Internal use only. */
	char program[1];	/* Unwarranted chumminess with compiler. */
} regexp;

extern regexp *regcomp();
extern int regexec();
extern void regsub();
extern void regerror();
