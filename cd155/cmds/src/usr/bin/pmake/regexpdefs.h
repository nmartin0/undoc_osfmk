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
 
/***************************************************************************
 *
 *    Copyright (c) 1992 Intel Corporation
 *
 *    This file is part of pmake, an extension of GNU Make.
 *    Pmake is distributed by the Intel Corporation under the
 *    terms of the GNU General Public License.
 *
 *    Title:
 *        $Id: regexpdefs.h,v 1.2 1994/11/19 01:34:01 mtm Exp $
 *
 *    Description:
 *        Regular expression definitions.
 *
 *    History: 
 *        $Log: regexpdefs.h,v $
 * Revision 1.2  1994/11/19  01:34:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/04  22:05:01  raya
 * Initial checkin of pmake sources to the commands and libraries
 * tree.
 *
 *
 **************************************************************************/
#ifndef	_REGEXPDEFS_H_
#define	_REGEXPDEFS_H_

/*
 * defined macros required by OSF regexp.h
 *	see man regexp
 */
#ifndef INIT
#define INIT  register char *_AChar=instring;
#define GETC() (*_AChar++)
#define UNGETC(c) (--_AChar)
#define PEEKC() (*_AChar)
#define RETURN(a) return (a)
#define ERROR(a) regerror(a)
#endif

/*
 * BSD style regexp() from Mach 2.5 "/usr/include/regexp.h"
 */

/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 *
 *	@(#)regexp.h	1.1 (Berkeley) 9/29/87
 */
#define NSUBEXP  10

#endif	/* _REGEXPDEFS_H_ */
