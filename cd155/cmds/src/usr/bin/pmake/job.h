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
 *        $Id: job.h,v 1.2 1994/11/19 01:33:52 mtm Exp $
 *
 *    Description:
 *        Child process structure definition.
 *
 **************************************************************************/

/* Structure describing a running or dead child process.  */

struct child
  {
    struct child *next;		 /* Link in the chain.  */

    struct file *file;		 /* File being remade.  */

    char **environment;		 /* Environment for commands.  */

    char **command_lines;	 /* Array of variable-expanded cmd lines.  */
    unsigned int command_line;	 /* Index into above.  */
    char *command_ptr;		 /* Ptr into command_lines[command_line].  */

    int pid;			 /* Child process's ID number.  */
    unsigned int remote:1;	 /* Nonzero if executing remotely.  */

    unsigned int noerror:1;	 /* Nonzero if commands contained a `-'.  */

    unsigned int good_stdin:1;	 /* Nonzero if this child has a good stdin.  */
    unsigned int deleted:1;	 /* Nonzero if targets have been deleted.  */
#ifdef PMAKE
    long processor;              /* Processor on which child is running */
#endif
  };

extern struct child *children;

extern void new_job ();
extern void wait_for_children ();
extern void push_signals_blocked_p (), pop_signals_blocked_p ();

extern char **construct_command_argv ();
extern void child_execute_job ();
extern void exec_command ();

extern unsigned int job_slots_used;


#ifdef PMAKE

extern char *partition;

extern int               init_parallel();
extern void              free_processor();
extern long              get_processor();

#define PMAKE_PTYPE      1

#endif


