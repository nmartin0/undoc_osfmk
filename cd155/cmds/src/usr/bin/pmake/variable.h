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
 *        $Id: variable.h,v 1.2 1994/11/19 01:34:13 mtm Exp $
 *
 *    Description:
 *        Variable structure definitions.
 *
 **************************************************************************/

/* Codes in a variable definition saying where the definition came from.
   Increasing numeric values signify less-overridable definitions.  */
enum variable_origin
  {
    o_default,		/* Variable from the default set.  */
    o_env,		/* Variable from environment.  */
    o_file,		/* Variable given in a makefile.  */
#ifdef PMAKE
    o_makeconf,		/* Variable given in a makefile.  */
#endif
    o_env_override,	/* Variable from environment, if -e.  */
    o_command,		/* Variable given by user.  */
    o_override, 	/* Variable from an `override' directive.  */
    o_automatic,	/* Automatic variable -- cannot be set.  */
    o_invalid		/* Core dump time.  */
  };

/* Structure that represents one variable definition.
   Each bucket of the hash table is a chain of these,
   chained through `next'.  */

struct variable
  {
    struct variable *next;	  /* Link in the chain.  */
    char *name;			  /* Variable name.  */
    char *value;		  /* Variable value.  */
    enum variable_origin
      origin ENUM_BITFIELD (4);	  /* Variable origin.  */
    unsigned int recursive:1;	  /* Gets recursively re-evaluated.  */
    unsigned int expanding:1;	  /* Nonzero if currently being expanded.  */
#ifdef PMAKE
    unsigned int export:1;	  /* Nonzero if variable should be exported */
    unsigned int expand_export:1; /* Nonzero means expand before exporting  */
#endif
  };

/* Structure that represents a variable set.  */

struct variable_set
  {
    struct variable **table;	/* Hash table of variables.  */
    unsigned int buckets;	/* Number of hash buckets in `table'.  */
  };

/* Structure that represents a list of variable sets.  */

struct variable_set_list
  {
    struct variable_set_list *next;	/* Link in the chain.  */
    struct variable_set *set;		/* Variable set.  */
  };

extern struct variable_set_list *current_variable_set_list;


extern char *variable_buffer_output ();
extern char *initialize_variable_output ();
extern char *save_variable_output ();
extern void restore_variable_output ();

extern void push_new_variable_scope (), pop_variable_scope ();

extern int handle_function ();

extern char *variable_expand (), *allocated_variable_expand ();
extern char *variable_expand_for_file ();
extern char *allocated_variable_expand_for_file ();
extern char *expand_argument ();

extern void define_automatic_variables ();
extern void initialize_file_variables ();
extern void print_file_variables ();

extern void merge_variable_set_lists ();

extern int try_variable_definition ();

extern struct variable *lookup_variable (), *define_variable ();
extern struct variable *define_variable_for_file ();

extern int pattern_matches ();
extern char *subst_expand (), *patsubst_expand ();
#ifdef PMAKE
extern char *apply_modifier ();
extern char *direct_recurse_expand();
extern int   find_variable_reference();
#endif

extern char **target_environment ();
extern void  set_export_flags();

