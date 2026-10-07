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
 
/* Copyright (C) 1988, 1989 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

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
 *        $Id: dep.h,v 1.2 1994/11/19 01:33:44 mtm Exp $
 *
 *    Description:
 *        Dependency structure definitions.
 *
 **************************************************************************/

/* Structure representing one dependency of a file.
   Each struct file's `deps' points to a chain of these,
   chained through the `next'.

   Note that the first three words of this match a struct nameseq.  */

struct dep
  {
    struct dep *next;
    char *name;
#ifdef PMAKE
    char *origname;
#endif
    struct file *file;
    int changed;
  };


/* Structure used in chains of names, for parsing and globbing */

struct nameseq
  {
    struct nameseq *next;
    char *name;
  };


extern struct nameseq *multi_glob (), *parse_file_seq ();


#ifndef	iAPX286
#define dep_name(d) ((d)->name == 0 ? (d)->file->name : (d)->name)
#else
/* Buggy compiler can't hack this.  */
extern char *dep_name ();
#endif

#ifdef PMAKE
extern char *old_dep_name ();
#endif
