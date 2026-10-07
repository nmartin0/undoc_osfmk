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
 
/* Makeconf support for GNU Make. 
Copyright (C) 1988, 1989, 1991 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
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
 *        $Id: nx.c,v 1.4 1994/11/19 01:33:57 mtm Exp $
 *
 *    Description:
 *        Routines to support NX-style parallel execution on
 *	  Paragon system.
 *
 **************************************************************************/

#include "make.h"
#include "job.h"
#include "variable.h"

#ifdef PMAKE
#ifdef PARAGON

#include <nx.h>
#include <allocsys.h>
#include <sys/types.h>
#include <limits.h>

static long *processor;		/* Stores #jobs running on each processor */

char *partition = 0;		/* Partition name  */
long  nnodes = 0;       	/* Number of nodes in partition */


/***************************************************************************
 *
 *    Calling Sequence:
 *        init_parallel (partition, size)
 *
 *    Description:
 *        Initialize make as a parallel application on the Paragon
 *	  system and set up processor list for managing parallel
 *	  execution.
 *
 *    Parameters:
 *        partition:	Name of partition in which to run child 
 *	  		processes.
 *	  jobslots:	Number of processors to use.  Defaults 
 *			to numnodes. 
 *	  origin:       Origin of jobslots value.
 *
 *    Returns:
 *        0 if sucessful; -1 otherwise.
 *
 **************************************************************************/

int
init_parallel (part, jobslots, origin)
  char         *part;
  unsigned int *jobslots;
  unsigned int  origin;
{
  int               i;
  int               argc;
  char             *argv[2];
  struct variable  *v;
  nx_part_info_t    partinfo;

  argc = 1;
  argv[0] = "pmake";
  argv[1] = 0;

  v = lookup_variable("NX_PARALLEL", 11);

  if (v && (*v->value != '\0') && (v->origin == o_env)) {
    /*
     *  Already a parallel application.
     */
    partition = savestring(v->value, strlen(v->value));
    setptype(PMAKE_PTYPE);
    nnodes = numnodes();
  } else {
    /*
     *  Determine partition name.
     */
    if ( strcmp(part, nx_dflt_part) == 0 ) {
      v = lookup_variable(nx_dflt_part, strlen(nx_dflt_part));
      if (v && v->value && *v->value)
        part = v->value;
      else
        part=".compute";
    } else {
      if ( strcmp(part, ".service") == 0 ) {
        message("-P switch not needed in .service partition (ignored)");
        return 0;
      }
    }
    partition = savestring(part, strlen(part));

    /*
     *  Set partition size.
     */
    if ( nx_get_partition_attributes( partition, &partinfo ) == -1 ) 
      fatal("Cannot find partition %s\n", partition);
    nnodes = partinfo.nodes;
    if ( ( origin != so_default ) && ( *jobslots <= nnodes ) ) 
      nnodes = *jobslots;
    else
      nnodes = 0;

#ifdef TEST
    printf("nx_initve(%s, %d, NULL, ...) returns ", partition, nnodes);
#endif
    nnodes = nx_initve(partition, nnodes, NULL, &argc, argv);
#ifdef TEST
    printf("%d\n", nnodes);
#endif

    if (nnodes <= 0)
      fatal("Cannot run in partition %s (nx_initve() failed)\n", partition);

    define_variable("NX_PARALLEL", 11, partition, o_env, 0, 1);
  }

  /*
   * Initialize processor array
   */
  processor = (long *) gmalloc(nnodes * sizeof(long));
  for (i=0; i<nnodes; i++) 
    processor[i] = 0;

  /*
   * Reset job_slots to numnodes() if user did not specify.
   */
  if (origin == so_default) 
    *jobslots = nnodes;

  return 0;
}



/***************************************************************************
 *
 *    Calling Sequence:
 *        get_processor ()
 *
 *    Description:
 *        Find the least busy processor. 
 *
 *    Parameters:
 *        none.
 *
 *    Returns:
 *        Processor number. 
 *
 **************************************************************************/

long
get_processor ()
{
  int    i;
  int    min = INT_MAX;
  long   minproc;

  for (i=0; i<nnodes; i++) {
    if (processor[i] < min) {
      min = processor[i];
      minproc = i;
    }
    if (min == 0)
      break;
  }

  processor[minproc]++;

  return (minproc);
}


/***************************************************************************
 *
 *    Calling Sequence:
 *        free_processor (p)
 *
 *    Description:
 *        Decrement the number of jobs running on a processor.
 *
 *    Parameters:
 *        p:	Processor affected.
 *
 *    Returns:
 *        void.
 *
 **************************************************************************/

void
free_processor (p)
  long p;
{
  if (processor[p] > 0)
    processor[p]--;
}


#endif PARAGON
#endif PMAKE

