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
 
/* Copyright (C) 1991 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

GNU Make is distributed "AS IS" in the hope that it will be useful,
but WITHOUT ANY WARRANTY of any kind; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Make; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/***************************************************************************
 *
 *    Copyright (c) 1988-1992 Intel Corporation
 *
 *    This file is part of pmake, an extension of GNU Make.
 *    Pmake is distributed by the Intel Corporation under the
 *    terms of the GNU General Public License.
 *
 *    Title:
 *        $Id: fnmatch.h,v 1.2 1994/11/19 01:34:26 mtm Exp $
 *
 *    Description:
 *        Declarations for file name pattern matching.
 *
 *************************************************************************/

#ifndef	_FNMATCH_H

#define	_FNMATCH_H	1

/* Bits set in the FLAGS argument to `fnmatch'.  */
#define	FNM_PATHNAME	(1 << 0)/* No wildcard can ever match `/'.  */
#define	FNM_NOESCAPE	(1 << 1)/* Backslashes don't quote special chars.  */
#define	FNM_PERIOD	(1 << 2)/* Leading `.' is matched only explicitly.  */
#define	__FNM_FLAGS	(FNM_PATHNAME|FNM_NOESCAPE|FNM_PERIOD)

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#define	FNM_NOMATCH	1

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
extern int fnmatch();

#endif /* fnmatch.h */
