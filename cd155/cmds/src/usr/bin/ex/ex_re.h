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
/* @(#)$RCSfile: ex_re.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:02 $ */
/*
 * COMPONENT_NAME: (CMDEDIT) ex_re.h
 *
 * FUNCTION: none
 *
 * ORIGINS: 3, 10, 13, 26, 27
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
 * ex_re.h	1.2  com/cmd/edit/vi,3.1,9013 10/6/89 17:19:08 
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

/*
 * Regular expression definitions.
 * The regular expressions in ex are similar to those in ed,
 * with the addition of the word boundaries from Toronto ed
 * and allowing character classes to have [a-b] as in the shell.
 * The numbers for the nodes below are spaced further apart then
 * necessary because I at one time partially put in + and | (one or
 * more and alternation.)
 */
struct	regexp {
	NLchar	Expbuf[ESIZE + 2];
	short	Circfl;
	short	Nbra;
};

/*
 * There are three regular expressions here, the previous (in re),
 * the previous substitute (in subre) and the previous scanning (in scanre).
 * It would be possible to get rid of "re" by making it a stack parameter
 * to the appropriate routines.
 */
var struct	regexp re;		/* Last re */
var struct	regexp scanre;		/* Last scanning re */
var struct	regexp subre;		/* Last substitute re */

/*
 * Defining circfl and expbuf like this saves us from having to change
 * old code in the ex_re.c stuff.
 */
#define	expbuf	re.Expbuf
#define	circfl	re.Circfl
#define	nbra	re.Nbra
#define savere(a)	((a) = re)
#define resre(a)	(re = (a))

/*
 * Definitions for substitute
 */
var NLchar	*braslist[NBRA];	/* Starts of \(\)'ed text in lhs */
var NLchar	*braelist[NBRA];	/* Ends... */
var NLchar	rhsbuf[RHSSIZE];	/* Rhs of last substitute */

/*
 * Definitions of codes for the compiled re's.
 * The re algorithm is described in a paper
 * by K. Thompson in the CACM about 10 years ago
 * and is the same as in ed.
 */
#define	STAR	1

#define	CBRA	1
#define	CDOT	4
#define	CCL	8
#define	NCCL	12
#define	CDOL	16
#define	CEOFC	17
#define	CKET	18
#define	CCHR	20
#define	CBRC	24
#define	CLET	25

/*
 * Definitions of things, other than simple characters, that can appear in a
 * character class.  The internal representation is a backslash followed by
 * one of these codes.  Most of these represent ctype classes.
 */
#define CL_BADCLASS	0x00	/* Not stored; used only as an error return */
#define CL_BACKSLASH	0x01	/* Real backslash */
#define CL_RANGE	0x02	/* a-z becomes '\\' CL_RANGE 'a' 'z' */
#define CL_ALPHA	0x03
#define CL_UPPER	0x04
#define CL_LOWER	0x05
#define CL_DIGIT	0x06
#define CL_XDIGIT	0x07
#define CL_ALNUM	0x08
#define CL_SPACE	0x09
#define CL_PUNCT	0x0a
#define CL_PRINT	0x0b
#define CL_GRAPH	0x0c
#define CL_CNTRL	0x0d
#if defined(KJI)
#define CL_JALPHA	0x0e
#define CL_JDIGIT	0x0f
#define CL_JSPACE	0x10
#define CL_JPUNCT	0x11
#define CL_JPAREN	0x12
#define CL_JKANJI	0x13
#define CL_JHIRA	0x14
#define CL_JKATA	0x15
#define CL_JXDIGIT	0x16
#endif
