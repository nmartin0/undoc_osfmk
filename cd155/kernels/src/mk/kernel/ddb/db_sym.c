/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: db_sym.c,v $
 * Revision 1.5  1994/11/18  20:29:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:20:53  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:19:00  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:18:43  dleslie
 * First R1_0 release
 *
 * Revision 2.10.4.1  92/02/18  18:38:53  jeffreyh
 * 	Added db_get_sym(). Simple interface to get symbol names
 * 	knowing the offset.
 * 	[91/12/20            bernadat]
 * 
 * 	Do not look for symbol names if address
 * 	is to small or to large, otherwise get
 * 	random names like INCLUDE_VERSION+??
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.10  91/10/09  16:02:30  af
 * 	 Revision 2.9.2.1  91/10/05  13:07:27  jeffreyh
 * 	 	Changed symbol table name qualification syntax from "xxx:yyy"
 * 	 	  to "xxx::yyy" to allow "file:func:line" in "yyy" part.
 * 		       "db_sym_parse_and_lookup" is also added for "yyy" part parsing.
 * 	 	Replaced db_search_symbol with db_search_task_symbol, and moved
 * 	 	  it to "db_sym.h" as a macro.
 * 	 	Added db_task_printsym, and changed db_printsym to call it.
 * 	 	Added include "db_task_thread.h".
 * 	 	Fixed infinite recursion of db_symbol_values.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.9.2.1  91/10/05  13:07:27  jeffreyh
 * 	Changed symbol table name qualification syntax from "xxx:yyy"
 * 	  to "xxx::yyy" to allow "file:func:line" in "yyy" part.
 * 	       "db_sym_parse_and_lookup" is also added for "yyy" part parsing.
 * 	Replaced db_search_symbol with db_search_task_symbol, and moved
 * 	  it to "db_sym.h" as a macro.
 * 	Added db_task_printsym, and changed db_printsym to call it.
 * 	Added include "db_task_thread.h".
 * 	Fixed infinite recursion of db_symbol_values.
 * 	[91/08/29            tak]
 * 
 * Revision 2.9  91/07/31  17:31:14  dbg
 * 	Add task pointer and space for string storage to symbol table
 * 	descriptor.
 * 	[91/07/31            dbg]
 * 
 * Revision 2.8  91/07/09  23:16:08  danner
 * 	Changed a printf.
 * 	[91/07/08            danner]
 * 
 * Revision 2.7  91/05/14  15:35:54  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/03/16  14:42:40  rpd
 * 	Changed the default db_maxoff to 4K.
 * 	[91/03/10            rpd]
 * 
 * Revision 2.5  91/02/05  17:07:07  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:19:17  mrt]
 * 
 * Revision 2.4  90/10/25  14:44:05  rwd
 * 	Changed db_printsym to print unsigned.
 * 	[90/10/19            rpd]
 * 
 * Revision 2.3  90/09/09  23:19:56  rpd
 * 	Avoid totally incorrect guesses of symbol names for small values.
 * 	[90/08/30  17:39:48  af]
 * 
 * Revision 2.2  90/08/27  21:52:18  dbg
 * 	Removed nlist.h.  Fixed some type declarations.
 * 	Qualifier character is ':'.
 * 	[90/08/20            dbg]
 * 	Modularized symtab info into a new db_symtab_t type.
 * 	Modified db_add_symbol_table  and others accordingly.
 * 	Defined db_sym_t, a new (opaque) type used to represent
 * 	symbols.  This should support all sort of future symtable
 * 	formats. Functions like db_qualify take a db_sym_t now.
 * 	New db_symbol_values() function to explode the content
 * 	of a db_sym_t.
 * 	db_search_symbol() replaces db_find_sym_and_offset(), which is
 * 	now a macro defined in our (new) header file.  This new
 * 	function accepts more restrictive searches, which are
 * 	entirely delegated to the symtab-specific code.
 * 	Accordingly, db_printsym() accepts a strategy parameter.
 * 	New db_line_at_pc() function.
 * 	Renamed misleading db_eqsym into db_eqname.
 * 	[90/08/20  10:47:06  af]
 * 
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 * Revision 2.1  90/07/26  16:43:52  dbg
 * Created.
 * 
 */
/*
 * 	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */

#include <machine/db_machdep.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>

#include <vm/vm_map.h>	/* vm_map_t */

/*
 * We import from the symbol-table dependent routines:
 */
extern db_sym_t	X_db_lookup();
extern db_sym_t	X_db_search_symbol();
extern boolean_t X_db_line_at_pc();
extern void	X_db_symbol_values();

/*
 * Multiple symbol tables
 */
#define	MAXNOSYMTABS	3	/* mach, ux, emulator */

db_symtab_t	db_symtabs[MAXNOSYMTABS] = {{0,},};
int db_nsymtab = 0;

db_symtab_t	*db_last_symtab;

db_sym_t	db_lookup();	/* forward */

/*
 * Add symbol table, with given name, to list of symbol tables.
 */
boolean_t
db_add_symbol_table(start, end, name, ref, map_pointer)
	char *start;
	char *end;
	char *name;
	char *ref;
	char *map_pointer;
{
	register db_symtab_t *st;
	extern vm_map_t kernel_map;

	if (db_nsymtab >= MAXNOSYMTABS)
	    return (FALSE);

	st = &db_symtabs[db_nsymtab];
	st->start = start;
	st->end = end;
	st->private = ref;
	st->map_pointer = (map_pointer == (char *)kernel_map)? 0: map_pointer;
	strcpy(st->name, name);

	db_nsymtab++;

	return (TRUE);
}

/*
 *  db_qualify("vm_map", "ux") returns "ux::vm_map".
 *
 *  Note: return value points to static data whose content is
 *  overwritten by each call... but in practice this seems okay.
 */
static char *
db_qualify(sym, symtabname)
	db_sym_t	sym;
	register char	*symtabname;
{
	char		*symname;
	static char     tmp[256];
	register char	*s;

	X_db_symbol_values(sym, &symname, 0);
	s = tmp;
	while (*s++ = *symtabname++) {
	}
	s[-1] = ':';
	*s++ = ':';
	while (*s++ = *symname++) {
	}
	return tmp;
}


boolean_t
db_eqname(src, dst, c)
	char *src;
	char *dst;
	char c;
{
	if (!strcmp(src, dst))
	    return (TRUE);
	if (src[0] == c)
	    return (!strcmp(src+1,dst));
	return (FALSE);
}

boolean_t
db_value_of_name(name, valuep)
	char		*name;
	db_expr_t	*valuep;
{
	db_sym_t	sym;

	sym = db_lookup(name);
	if (sym == DB_SYM_NULL)
	    return (FALSE);
	db_symbol_values(sym, &name, valuep);
	return (TRUE);
}


/*
 * Lookup a symbol.
 * If the symbol has a qualifier (e.g., ux::vm_map),
 * then only the specified symbol table will be searched;
 * otherwise, all symbol tables will be searched.
 */
db_sym_t
db_lookup(symstr)
	char *symstr;
{
	db_sym_t sp;
	register int i;
	int symtab_start = 0;
	int symtab_end = db_nsymtab;
	register char *cp;

	/*
	 * Look for, remove, and remember any symbol table specifier.
	 */
	for (cp = symstr; *cp; cp++) {
		if (*cp == ':' && cp[1] == ':') {
			*cp = '\0';
			for (i = 0; i < db_nsymtab; i++) {
				if (! strcmp(symstr, db_symtabs[i].name)) {
					symtab_start = i;
					symtab_end = i + 1;
					break;
				}
			}
			*cp = ':';
			if (i == db_nsymtab)
				db_error("Invalid symbol table name\n");
			symstr = cp+2;
		}
	}

	/*
	 * Look in the specified set of symbol tables.
	 * Return on first match.
	 */
	for (i = symtab_start; i < symtab_end; i++) {
		if (sp = X_db_lookup(&db_symtabs[i], symstr)) {
			db_last_symtab = &db_symtabs[i];
			return sp;
		}
	}
	return 0;
}

/*
 * Common utility routine to parse a symbol string into a file
 * name, a symbol name and line number.
 * This routine is called from X_db_lookup if the object dependent
 * handler supports qualified search with a file name or a line number.
 * It parses the symbol string, and call an object dependent routine
 * with parsed file name, symbol name and line number.
 */ 
db_sym_t
db_sym_parse_and_lookup(func, symtab, symstr)
	db_sym_t	(*func)();
	db_symtab_t	*symtab;
	char		*symstr;
{
	register 	char *p;
	register 	n;
	int	 	n_name;
	int	 	line_number;
	char	 	*file_name = 0;
	char	 	*sym_name = 0;
	char		*component[3];
	db_sym_t 	found = DB_SYM_NULL;

	/*
	 * disassemble the symbol into components:
	 *	[file_name:]symbol[:line_nubmer]
	 */
	component[0] = symstr;
	component[1] = component[2] = 0;
	for (p = symstr, n = 1; *p; p++) {
		if (*p == ':') {
			if (n >= 3)
				break;
			*p = 0;
			component[n++] = p+1;
		}
	}
	if (*p != 0)
		goto out;
	line_number = 0;
	n_name = n;
	p = component[n-1];
	if (*p >= '0' && *p <= '9') {
		if (n == 1)
			goto out;
		for (line_number = 0; *p; p++) {
			if (*p < '0' || *p > '9')
				goto out;
			line_number = line_number*10 + *p - '0';
		}
		n_name--;
	} else if (n >= 3)
		goto out;
	if (n_name == 1) {
		for (p = component[0]; *p && *p != '.'; p++);
		if (*p == '.') {
			file_name = component[0];
			sym_name = 0;
		} else {
			file_name = 0;
			sym_name = component[0];
		}
	} else {
		file_name = component[0];
		sym_name = component[1];
	}
	found = func(symtab, file_name, sym_name, line_number);
	
out:
	while (--n >= 1)
		component[n][-1] = ':';
	return(found);
}

/*
 * Does this symbol name appear in more than one symbol table?
 * Used by db_symbol_values to decide whether to qualify a symbol.
 */
boolean_t db_qualify_ambiguous_names = FALSE;

boolean_t
db_symbol_is_ambiguous(sym)
	db_sym_t	sym;
{
	char		*sym_name;
	register int	i;
	register
	boolean_t	found_once = FALSE;

	if (!db_qualify_ambiguous_names)
		return FALSE;

	X_db_symbol_values(sym, &sym_name, 0);
	for (i = 0; i < db_nsymtab; i++) {
		if (X_db_lookup(&db_symtabs[i], sym_name)) {
			if (found_once)
				return TRUE;
			found_once = TRUE;
		}
	}
	return FALSE;
}

/*
 * Find the closest symbol to val, and return its name
 * and the difference between val and the symbol found.
 */
db_sym_t
db_search_task_symbol(val, strategy, offp, task)
	register db_addr_t	val;
	db_strategy_t		strategy;
	db_expr_t		*offp;
	task_t			task;
{
	register
	unsigned int	diff;
	unsigned int	newdiff;
	register int	i;
	db_symtab_t	*sp;
	db_sym_t	ret = DB_SYM_NULL, sym;
	vm_map_t	map_for_val;

	if (task == TASK_NULL)
	    task = db_current_task();
	map_for_val = (task == TASK_NULL)? VM_MAP_NULL: task->map;
	newdiff = diff = ~0;
	db_last_symtab = 0;
	for (sp = &db_symtabs[0], i = 0;
	     i < db_nsymtab;
	     sp++, i++) {
	    if ((vm_map_t)sp->map_pointer == VM_MAP_NULL ||
		(vm_map_t)sp->map_pointer == map_for_val) {
		sym = X_db_search_symbol(sp, val, strategy, &newdiff);
		if (newdiff < diff) {
		    db_last_symtab = sp;
		    diff = newdiff;
		    ret = sym;
		}
	    }
	}
	*offp = diff;
	return ret;
}

/*
 * Return name and value of a symbol
 */
void
db_symbol_values(sym, namep, valuep)
	db_sym_t	sym;
	char		**namep;
	db_expr_t	*valuep;
{
	db_expr_t	value;

	if (sym == DB_SYM_NULL) {
		*namep = 0;
		return;
	}

	X_db_symbol_values(sym, namep, &value);

	if (db_symbol_is_ambiguous(sym))
		*namep = db_qualify(sym, db_last_symtab->name);
	if (valuep)
		*valuep = value;
}


/*
 * Print a the closest symbol to value
 *
 * After matching the symbol according to the given strategy
 * we print it in the name+offset format, provided the symbol's
 * value is close enough (eg smaller than db_maxoff).
 * We also attempt to print [filename:linenum] when applicable
 * (eg for procedure names).
 *
 * If we could not find a reasonable name+offset representation,
 * then we just print the value in hex.  Small values might get
 * bogus symbol associations, e.g. 3 might get some absolute
 * value like _INCLUDE_VERSION or something, therefore we do
 * not accept symbols whose value is zero (and use plain hex).
 */

unsigned int	db_maxoff = 0x4000;
extern		char end;
unsigned int	db_maxval = (int)&end;
unsigned int	db_minval = 0x1000;

void
db_task_printsym(off, strategy, task)
	db_expr_t	off;
	db_strategy_t	strategy;
	task_t		task;
{
	db_expr_t	d;
	char 		*filename;
	char		*name;
	db_expr_t	value;
	int 		linenum;
	db_sym_t	cursym;

	if (off >= db_maxval || off < db_minval) {
		db_printf("%#n", off);
		return;
	}
	cursym = db_search_task_symbol(off, strategy, &d, task);

	db_symbol_values(cursym, &name, &value);
	if (name == 0 || d >= db_maxoff || value == 0) {
		db_printf("%#n", off);
		return;
	}
	db_printf("%s", name);
	if (d)
		db_printf("+0x%x", d);
	if (strategy == DB_STGY_PROC) {
		if (db_line_at_pc(cursym, &filename, &linenum, off)) {
			db_printf(" [%s", filename);
			if (linenum > 0)
				db_printf(":%d", linenum);
			db_printf("]");
		}
	}
}

/*
 * Return symbol name for a given offset and
 * change the offset to be relative to this symbol.
 * Very usefull for xpr, when you want to log offsets
 * in a user friendly way.
 */

char null_sym[] = "";

char *db_get_sym(off)
db_expr_t	*off;
{
	db_sym_t	cursym;
	db_expr_t	value;
	char		*name;
	db_expr_t	d;

	cursym = db_search_symbol(*off, DB_STGY_ANY, &d);
	db_symbol_values(cursym, &name, &value);
	if (name) 
		*off = d;
	else
		name = null_sym;
	return(name);
}

void
db_printsym(off, strategy)
	db_expr_t	off;
	db_strategy_t	strategy;
{
	db_task_printsym(off, strategy, TASK_NULL);
}

boolean_t
db_line_at_pc( sym, filename, linenum, pc)
{
	return X_db_line_at_pc( db_last_symtab, sym, filename, linenum, pc);
}
