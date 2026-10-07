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
/*
 * OSF/1 Release 1.0
 */
/*LINTLIBRARY*/
#include <stdio.h>
#include <a.out.h>

extern long lseek();
extern int open(), read(), close(), strcmp();
extern char *calloc(), *malloc();
extern void free();

#if 0
/*
 * HACK ALERT !!
 * Since the mips compiler doesn't prepend a '_' to the symbols, we
 * need to fake out the symbols passed in to "pretend" that this is
 * true. What is done it to increment the symbol name in the namelist
 * arg past the initial underscore before the compare
 * is made. I know this is gross and ugly, but it is easier than
 * changing all of the commands that use this interface.
 */

#define checksym(l, v, n) \
    { register struct nlist *p = l; \
        for (; p->n_name && p->n_name[0]; p++) \
        { \
                if (!p->n_type && \
                    strcmp(&p->n_name[1], (n)) == 0) \
                { \
                        nlist_syms--; \
                        p->n_value = (v); \
                        p->n_type = 1; \
                        break; \
                } \
        } \
    }
#endif

int
nlist(fname, list)
	const char *fname;
	struct nlist list[];
{
	struct filehdr buf;
	int     bufsiz=sizeof(buf);
        SYMENT	*symtab;
        FILHDR	*fhdr;
	long    nlist_syms = 0, f_idx, s_idx;
        char    *ext_strings, *local_strings;
	register struct nlist *p;
	long	sa;
	int	fd;
        
#if 0
        /*
	 * init name list
	 */
	for ( p = list;
	    p->name && p->n_name;	/* cpp bug in the line p-> */
	    nlist_syms++,p++) /* n_name can be ptr */
	{
		p->n_type = 0;
		p->n_value = 0L;
	}
        
	/* open file */
	if ((fd = open(fname, 0)) < 0)
		return(-1);

        /* read filehdr */
        (void) read(fd, (char *)&buf, bufsiz);
	if (!ISCOFF(buf.f_magic))
	{
		(void) close(fd);
		return (-1);
	}
	sa = buf.f_symptr;

        /* read symbol table header */
	(void) lseek(fd, sa, 0);
        (void) read(fd, (char *)&symhdr, cbHDRR);
        if (symhdr.magic != magicSym)
        {
                (void) close(fd);
                return (-1);
        }

        /* read external strings table */
        if (symhdr.issExtMax)
        {
                if (!(ext_strings = malloc(symhdr.issExtMax)))
                {
                        (void) close(fd);
                        return (-1);
                }
                (void) lseek(fd, symhdr.cbSsExtOffset, 0);
                (void) read(fd, ext_strings, symhdr.issExtMax);
        
                /* read external symbol table section */
                if (!(exttab = (EXTR *)calloc(symhdr.iextMax, cbEXTR)))
                {
                        (void) close(fd);
                        return (-1);
                }
                (void) lseek(fd, symhdr.cbExtOffset, 0);
                (void) read(fd, (char *)exttab, (symhdr.iextMax * cbEXTR));
                
                /* check each external symbol */
                for (s_idx = 0; s_idx < symhdr.iextMax; s_idx++)
                {
                        switch(exttab[s_idx].asym.st)
                        {
                        case stNil:
                        case stLabel:
                        case stBlock:
                        case stEnd:
                        case stMember:
                        case stFile:
                        case stTypedef:
                        case stLocal:
                        case stParam:
                                break;
                        default:
                                checksym(list, exttab[s_idx].asym.value, (ext_strings + exttab[s_idx].asym.iss));
                        }
                }

                /* free external symbol table */
                free(exttab);
                free(ext_strings);

                /* if all symbols are found at this point return */
                if (nlist_syms == 0)
                {
                        (void) close(fd);
                        return (0);
                }
        }

        /* read local strings table */
        if (symhdr.issMax)
        {
                if (!(local_strings = malloc(symhdr.issMax)))
                {
                        (void) close(fd);
                        return (-1);
                }
                (void) lseek(fd, symhdr.cbSsOffset, 0);
                (void) read(fd, local_strings, symhdr.issMax);
        
                /* read file table section */
                if (!(fhdr = (FILHDR *)calloc(symhdr.ifdMax, cbFDR)))
                {
                        (void) close(fd);
                        return (-1);
                }
                (void) lseek(fd, symhdr.cbFdOffset, 0);
                (void) read(fd, (char *)fhdr, (symhdr.ifdMax * cbFDR));
                
                /* Now we have to loop for each file looking for symbols */
                for (f_idx = 0; f_idx < symhdr.ifdMax; f_idx++)
                {
                        /* read symbol table for file */
                        if (!(symtab = (SYMENT *)calloc(fhdr[f_idx].csym, cbSYMR)))
                        {
                                (void) close(fd);
                                return (-1);
                        }
                        (void) lseek(fd, (symhdr.cbSymOffset + (fhdr[f_idx].isymBase * cbSYMR)), 0);
                        (void) read(fd, (char *)symtab, (fhdr[f_idx].csym * cbSYMR));

                        /* check each file entry for symbols */
                        for (s_idx = 0; s_idx < fhdr[f_idx].csym; s_idx++)
                        {
                                switch(symtab[s_idx].st)
                                {
                                case stNil:
                                case stLabel:
                                case stBlock:
                                case stEnd:
                                case stMember:
                                case stFile:
                                case stTypedef:
                                case stLocal:
                                case stParam:
                                        break;
                                default:
                                        checksym(list, symtab[s_idx].value, (local_strings + fhdr[f_idx].issBase + symtab[s_idx].iss));
                                }
                        }
                        /* free symbol table */
                        (void) free (symtab);
                }
                /* free file header and local strings table */
                (void) free (fhdr);
                (void) free (local_strings);
        }
#endif
        /* close the file */
        (void) close(fd);
	return (nlist_syms);
}
