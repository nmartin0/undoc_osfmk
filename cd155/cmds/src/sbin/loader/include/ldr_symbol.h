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
/* ldr_symbol.h
 * Definitions for loader import symbol table
 *
 * This routine defines the common symbol table format, used for
 * both the import symbol table and the export symbol table.
 * The symbol table lists all the imported or exported symbols for a
 * module, the package each symbol comes from, and the symbol's
 * value.
 *
 * The import symbol table is built by the format-dependent
 * manager, with all symbols unresolved.  The symbol values are
 * filled in by the format-independent symbol resolution and
 * symbol value computation routines.  The entire import symbol
 * table is then passed to the format-dependent manager for its
 * use during relocation.
 *
 * The export symbol table is also built by the format-dependent
 * manager.  It is normally only used when pre-loading a library.
 *
 * This file depends on: ldr_machdep.h ldr_package.h
 * OSF/1 Release 1.0
 */

#ifndef _H_LDR_SYMBOL
#define _H_LDR_SYMBOL


#define	LDR_SYMBOL_VERSION	1	/* current structure version */

typedef struct ldr_symbol_rec {
	int		ls_version;	/* version number of structure */
	char		*ls_name;	/* symbol name */
	int		ls_packageno;	/* package number symbol belongs to */
	univ_t		ls_module;	/* module exporting symbol */
	ldr_symval	ls_value;	/* symbol's value */
	char		ls_reserved[40]; /* reserved */
} ldr_symbol_rec;


/* Create a list of symbol records large enough to hold
 * count records, initialize it, and return it in *syms.  Returns
 * LDR_SUCCESS on success, negative error status on error.
 */

extern int ldr_symbols_create __((int count, int version, ldr_symbol_rec **retval));

/* Free a list of symbol packages containing count records.  Returns
 * LDR_SUCCESS on success, negative error status on error.
 */

extern int ldr_symbols_free __((int count, ldr_symbol_rec *val));

#endif /* _H_LDR_SYMBOL */
