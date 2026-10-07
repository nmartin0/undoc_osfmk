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
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: db_coff_sym.c,v $
 * Revision 2.7  1994/11/18  20:38:37  mtm
 * Copyright additions/changes
 *
 * Revision 2.6  1994/01/29  00:08:24  lenb
 *  Reviewer: mp3sw
 *  Risk: low
 *  Benefit or PTS #: show static symbols on ddb output
 *  Testing:
 *  Module(s):
 *
 * Revision 2.5  1993/06/30  22:30:25  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.4  1993/04/27  20:28:22  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 2.3.6.1  1993/04/22  18:28:40  dleslie
 * First R1_0 release
 *
 * Revision 2.4  1993/04/05  02:20:15  andyp
 * Fix for symbol names that are exactly 8 characters long.
 *
 * Revision 2.3  1992/09/22  17:12:42  SSD
 * *** empty log message ***
 *
 * Revision 2.2.2.1  92/04/08  15:43:26  jeffreyh
 * 	Print out static symbols if possible
 * 	[92/04/08            andyp]
 * 
 * Revision 2.2  91/12/10  16:30:24  jsb
 * 	New files from Intel
 * 	[91/12/10  16:11:24  jsb]
 * 
 */
/*
 * i860-specific routines for loading coff symbols.
 */

#include <machine/db_machdep.h>
#include<mach/boolean.h>
#include <ddb/db_sym.h>

#include <vm/vm_kern.h>
#include <i860/thread.h>
#include <i860/coff.h>
#include <i860/syms.h>
#include <i860/storclass.h>


#ifdef DB_NO_AOUT

extern db_symtab_t db_symtabs[];
extern int db_nsymtab;


long
db_long(char_p)
char *char_p;
{
long ret_value;

  bcopy(char_p, &ret_value, sizeof(ret_value));
  return ret_value;
}

db_sym_t
X_db_lookup(stab, symstr)
	db_symtab_t	*stab;
	char *		symstr;
{
struct syment *symbol_entry;
long   num_symbols;
int    ii;
char  *sym_string;
char  *symtbl_ptr;
char  sbuf[12];


  num_symbols = ((struct filehdr *)stab->start)->f_nsyms;
  symtbl_ptr = (char *)stab->start + sizeof(ldf_hdr_t);


  /* Search for the symbol */
  for(ii=0;ii<num_symbols;ii++) {
      symbol_entry = (struct syment *)(symtbl_ptr + (ii * SYMESZ));
      if (db_long((char *)&symbol_entry->n_zeroes) != 0) {
	/*
	 * symbol is contained in n_name[], if sym is < 8 chars then n_name[]
	 * is null padded. Problem arrises when the symbol is 8 chars. Here
	 * we copy the symbol into a buffer which has been cleared thus we
	 * have a null terminated string.
	 */
	  register int j;
	  register char *cp;

	  bzero(sbuf,12);
	  for(cp=sbuf,j=0; j < 8; j++,cp++) {
		if ( (*cp = symbol_entry->n_name[j]) == '\0' )
			break;
	  }
	  sym_string = sbuf;
      } else {
	/*
	 * n_offset is an offset into the COFF string table.
	 */
	  sym_string = (symtbl_ptr + (num_symbols * SYMESZ)) + 
	      db_long((char *)&symbol_entry->n_offset);
      }

      if (db_eqname(sym_string, symstr, '_')) {
	  return ((db_sym_t)symbol_entry);
      }
  }
  return ((db_sym_t)0);
}


db_sym_t
X_db_search_symbol(symtab, off, strategy, diffp)
	db_symtab_t *	symtab;
	register
	db_addr_t	off;
	db_strategy_t	strategy;
	db_expr_t	*diffp;		/* in/out */
{
	unsigned int	diff = *diffp;
	struct syment	*symp = 0;
	struct syment	*sp;
	char            *sp_raw;
	long   num_symbols;
	int    ii;

	sp_raw = (char *) symtab->start + sizeof(ldf_hdr_t);
	num_symbols = ((struct filehdr *)symtab->start)->f_nsyms;


	for (ii = 0; ii < num_symbols; ii++) {
	    sp = (struct syment *)(sp_raw + (ii * SYMESZ));

	    /* if syment has aux entries, skip it -- and them too */
	    if (sp->n_numaux != 0) {
		ii += sp->n_numaux;
		continue;
	    }

	    /* ignore pesky little symbols like .text, .S01234 etc. */
	    if (*sp->n_name == '.')
		continue;

	    if (off >= db_long((char *)&sp->n_value)) {
		if (off - db_long((char *)&sp->n_value) < diff) {
		    diff = off - db_long((char *)&sp->n_value);
		    symp = sp;
		    if (diff == 0)
			break;
		}
		else if (off - db_long((char *)&sp->n_value) == diff) {
		    if (symp == 0)
			symp = sp;
		    else if (symp->n_sclass != C_EXT &&
				sp->n_sclass == C_EXT)
			symp = sp;	/* pick the external symbol */
		}
	    }
	}
	if (symp == 0) {
	    *diffp = off;
	}
	else {
	    *diffp = diff;
	}
	return ((db_sym_t)symp);
}

/*
 * Return the name and value for a symbol.
 */
void
X_db_symbol_values(sym, namep, valuep)
	db_sym_t	sym;
	char		**namep;
	db_expr_t	*valuep;
{
    struct syment *symbol_entry;
    long           num_symbols;
    int            ii, len;
    char          *symtbl_ptr, *src, *dst;
    static char	   symname[SYMNMLEN + 1];

    for(ii=0;ii<db_nsymtab;ii++) {
	if (sym >= db_symtabs[ii].start && sym <= db_symtabs[ii].end) {
	    break;
	}
    }

    if (ii >= db_nsymtab) {
	*namep="<unknown symbol>";
	*valuep = 0;
	return;
    }

    num_symbols = ((struct filehdr *)db_symtabs[ii].start)->f_nsyms;

    symbol_entry = (struct syment *)sym;
    if (namep) {
	if (db_long((char *)&symbol_entry->n_zeroes) != 0) {
	    /*
	     * if the symbol name is exactly 8 characters long,
	     * it isn't null terminated...so copy it into the
	     * static buffer for all cases <= 8
	     */

	    symname[SYMNMLEN] = 0;
	    src = &symbol_entry->n_name[0];
	    dst = &symname[0];
	    for (len = 0; len < 8; len++)
		if ((*dst++ = *src++) == 0)
			break;
	    if (len < 8)
		*namep = &symbol_entry->n_name[0];
	    else
		*namep = &symname[0];
	} else {

	    symtbl_ptr = (char *)db_symtabs[ii].start + sizeof(ldf_hdr_t);
	    *namep = (char *)(symtbl_ptr + (num_symbols * SYMESZ)) + 
		db_long((char *)&symbol_entry->n_offset);
	}
    }

    if (valuep) {
	*valuep = db_long((char *)&symbol_entry->n_value);
    }
}


/*
 * Find filename and lineno within, given the current pc.
 */
boolean_t
X_db_line_at_pc(symtab, sym, filenamep, linep, pc)
	db_symtab_t	*symtab;
	db_sym_t	sym;		/* unused for a.out */
	char		**filenamep;
	int		*linep;
	db_expr_t	pc;
{
return FALSE;
}



boolean_t
X_db_sym_init(sym_start, sym_end, name, task_addr)
    char *      sym_start;      /* Start of the symbol table */
    char *      sym_end;        /* End of the symbol table */
    char *	name;
    char *	task_addr;	/* use for this task only */
{
struct filehdr *coff_hdr;

    coff_hdr = (struct filehdr *)sym_start;
    if (coff_hdr->f_nsyms > 0) {
	db_printf("[ preserving %d bytes of %s symbol table ]\n",
		(sym_end - sym_start), name);

	return db_add_symbol_table(sym_start,
				sym_end,
				name,
				coff_hdr,  /* Use the private field to store num syms */
				task_addr);
    }
    return FALSE;
}
#endif /* DB_NO_AOUT */
