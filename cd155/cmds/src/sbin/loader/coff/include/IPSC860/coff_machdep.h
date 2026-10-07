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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* coff_machdep.h
 * Machine-dependent definitions for IPSC860 COFF files
 *
 * This file contains the definitions for the machine-dependent macros
 * used by the COFF format-dependent manager.
 * This file depends on <filehdr.h>, <aouthdr.h>, <a.out.h>
 *
 * OSF/1 Release 1.0.1
 */

#include <a.out.h>

/* The COFF_MAGICOK macro returns true iff this is a valid COFF file
 * for this machine type.  Argument is pointer to the file header
 * of the file.
 */

#define COFF_MAGICOK(fhdr) ((fhdr)->f_magic == I860MAGIC)

/* These macros return the file offset, base virtual address, and
 * size in bytes of each region in the COFF file.  They are machine-
 * dependent because different manufacturers represent this information
 * differently in the COFF file.  On PMAX, we can simply
 * use the standard N_TXTOFF macro.  Arguments are pointers to the file
 * header and a.out header of the file.
 */

#define COFF_SECTALIGN	4096
#define COFF_TXTOFF(fhdr, ohdr)	((ohdr)->magic == ZMAGIC ? 0 : \
				 N_TXTOFF((*fhdr), (*ohdr)))
#define COFF_TXTSTART(fhdr, ohdr) (trunc((ohdr)->text_start, ldr_getpagesize()))
#define COFF_TXTEND(fhdr, ohdr) (round((ohdr)->text_start + (ohdr)->tsize, \
				       ldr_getpagesize()))

#define COFF_DATAOFF(fhdr, ohdr) (trunc((COFF_TXTOFF((fhdr), (ohdr)) + (ohdr)->tsize), \
					ldr_getpagesize()))
#define COFF_DATASTART(fhdr, ohdr) (trunc((ohdr)->data_start, ldr_getpagesize()))
#define COFF_DATAEND(fhdr, ohdr) (round((ohdr)->data_start + (ohdr)->dsize, \
					ldr_getpagesize()))

#define COFF_BSSSTART(fhdr, ohdr) (COFF_DATAEND((fhdr), (ohdr)))
#define COFF_BSSEND(fhdr, ohdr) (round(COFF_DATASTART((fhdr), (ohdr)) + \
				       (ohdr)->dsize + (ohdr)->bsize, \
				       ldr_getpagesize()))
				
				
/* COFF_ENTRYPT returns the entry point address from the COFF module */

#define COFF_ENTRYPT(fhdr, ohdr) ((ohdr)->entry)

/* Must define magic numbers here -- they're not defined elsewhere */

#define OMAGIC	0407		/* old impure format */
#define NMAGIC	0410		/* read-only text */
#define ZMAGIC	0413		/* demand load format */
