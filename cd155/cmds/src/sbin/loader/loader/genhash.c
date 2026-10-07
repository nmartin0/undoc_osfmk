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
 * gen_hash.c
 *
 * Generate open hash table for loader bootstrapping
 * Generates an open hash table for the bootstrapping of
 * the standalone loader, from a list of exported packages
 * and symbols.
 *
 * Syntax:
 *  gen_hash [-s] [-p <pkg_rec_name>] [-t <table_name>] [-o <output file> exports_file
 *
 *  -s			make all structures static
 *  -p			name loader package rec array
 *  -t			name hash table
 *
 * Format of exports file:
 * # comment
 * -export pkg:sym,sym,sym,
 * sym,sym
 * -export_function pkg:sym,sym,sym
 * -export_data pkg:sym,sym,sym
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include <ldr_main_types.h>

#include "ldr_types.h"
#include "ldr_malloc.h"
#include "ldr_package.h"
#include "ldr_hash.h"
#include "open_hash.h"
#include "open_hash_pvt.h"


/* Shouldn't be here, but there is no include file for them... */

extern int getopt();
extern int optind, opterr;
extern char *optarg;

char	*symbol_kinds[] = {
	"ldr_sym_unknown",
	"ldr_sym_function",
	"ldr_sym_data"
	};

char	*program = NULL;
char	*exports_file = NULL;
char	*output_file = NULL;
char	*do_static = "";
char	*pkg_name = "loader_packages";
char	*table_name = "loader_exports";

struct pkg {
	struct pkg *next;
	char *name;
};

struct pkg_list {
	struct pkg *head;
	struct pkg *tail;
	int count;
};

struct sym {
	struct sym *next;
	char *name;
	int pkgno;
	ldr_symbol_kind_t kind;
};


int
main(argc, argv)
int argc;
char **argv;
{
	struct pkg_list	pkgs;
	open_hashtab_t	exports;

	if (parse_options(argc, argv) != 0) {
		usage(argc, argv);
		exit(1);
	}

	if (read_exports(exports_file, &pkgs, &exports) != 0)
		exit(1);

	if (write_output(&pkgs, exports, output_file) != 0)
		exit(1);

	exit(0);
}


int
usage(argc, argv)
int argc;
char **argv;
{
	fprintf(stderr, "usage: %s [-s] [-p <pkg_rec_name>] [-t <table_name>] [-o <output file> exports_file\n",
		argv[0]);
}


int
parse_options(argc, argv)
int argc;
char **argv;
{
	int optc;

	/* Initialize */

	program = argv[0];

	while ((optc = getopt(argc, argv, "sp:t:o:")) != EOF) {
		switch(optc) {

		      case 's':		/* static */
			do_static = "static ";
			break;

		      case 'p':		/* pkg rec name */
			pkg_name = optarg;
			break;

		      case 'o':		/* output file */
			output_file = optarg;
			break;

		      case 't':		/* table name */
			table_name = optarg;
			break;

		      default:
			return(-1);
		}
	}

	if (optind < argc)
		exports_file = argv[optind];

	return(0);
}


int
read_exports(name, pkgs, exports)
char *name;
struct pkg_list *pkgs;
open_hashtab_t *exports;

/* Read the specified exports file.  Construct packages and export
 * hash tables.
 */
{
	FILE		*fd;
	char		buf[BUFSIZ];
	char		pkg_buf[BUFSIZ];
	char		sym_buf[BUFSIZ];
	char		*tok, *next_tok;
	int		cur_pkg;
	int		sym_count;
	struct pkg_list	pkg_list;
	struct sym	*symbols;
	struct sym	*sym;
	univ_t		elem;
	ldr_symbol_kind_t kind;
	open_hashtab_t	tab;
	int		rc;
#ifdef	PREFIX_UNDERSCORE
	char		_sym_name[1024];
#endif

	if (name == NULL)
		fd = stdin;
	else if ((fd = fopen((char *)name, "r")) == NULL) {
		perror("opening exports file");
		return(-1);
	}

	cur_pkg = 0;
	pkg_list.count = 0;
	pkg_list.head = pkg_list.tail = NULL;
	sym_count = 0;
	symbols = NULL;

	while (fgets(buf, sizeof(buf), fd) != NULL) {

		if (buf[0] == '#')
			continue;

		if (sscanf(buf, "-export_function %[^:]:%s\n", pkg_buf, sym_buf) == 2) {
			kind = ldr_sym_function;
			add_pkg(&cur_pkg, &pkg_list, pkg_buf);
		} else if (sscanf(buf, "-export_data %[^:]:%s\n", pkg_buf, sym_buf) == 2) {
			kind = ldr_sym_data;
			add_pkg(&cur_pkg, &pkg_list, pkg_buf);
		} else if (sscanf(buf, "-export %[^:]:%s\n", pkg_buf, sym_buf) == 2) {
			kind = ldr_sym_unknown;
			add_pkg(&cur_pkg, &pkg_list, pkg_buf);
		} else
			strcpy(sym_buf, buf);

		for (tok = next_tok = sym_buf; tok != NULL; ) {

			tok = strtok(next_tok, ",\n");
			if (tok != NULL && *tok != '\0') {
				sym_count++;
				add_sym(&symbols, tok, cur_pkg, kind);
			}
			next_tok = NULL;
		}
	}

	if (name != NULL)
		(void)fclose(fd);

	if ((rc = open_hash_create(sym_count, (ldr_hash_p)hash_string, 
				   (ldr_hash_compare_p)strcmp, 0, &tab)) < 0) {
		fprintf(stderr, "open_hash_create failed %d\n", rc);
		return(rc);
	}

	for (sym = symbols; sym != NULL; sym = sym->next) {

		elem = (univ_t)sym;
#ifdef	PREFIX_UNDERSCORE
		_sym_name[0] = '_';
		strcpy(&_sym_name[1], sym->name);
		rc = open_hash_insert(tab, (const univ_t)_sym_name, elem);
#else
		rc = open_hash_insert(tab, (const univ_t)sym->name, elem);
#endif
		if (rc < 0) {
			fprintf(stderr, "open_hash_insert %s failed %d\n", sym->name, rc);
			return(rc);
		}
	}

	*pkgs = pkg_list;
	*exports = tab;

	return(0);
}


add_pkg(cur_pkg, pkg_list, name)
int *cur_pkg;
struct pkg_list *pkg_list;
char *name;

/* Add a package structure with the specified name to the package list
 * if it's not already there.  Return the package number of the packge
 * in *cur_pkg.
 */
{
	struct	pkg	*pkgp;
	int		i;

	for (i = 0, pkgp = pkg_list->head; pkgp != NULL; pkgp = pkgp->next, i++) {
		if (strcmp(pkgp->name, name) == 0) {
			*cur_pkg = i;
			return;
		}
	}

	if ((pkgp = (struct pkg *)malloc(sizeof(struct pkg))) == NULL) {
		perror("unable to alloc pkg list");
		exit(1);
	}

	pkgp->name = (char *)malloc(strlen(name)+1);
	strcpy(pkgp->name, name);
	pkgp->next = NULL;
	if (pkg_list->tail == NULL) {
		pkg_list->head = pkg_list->tail = pkgp;
	} else {
		pkg_list->tail->next = pkgp;
		pkg_list->tail = pkgp;
	}
	*cur_pkg = pkg_list->count;
	pkg_list->count++;
}


add_sym(sym_list, name, cur_pkg, kind)
struct sym **sym_list;
char *name;
int cur_pkg;
ldr_symbol_kind_t kind;

/* Add a symbol structure for the specified symbol name, package number, and
 * kind to the symbol list.
 */
{
	struct	sym *symp;

	if ((symp = (struct sym *)malloc(sizeof(struct sym))) == NULL) {
		perror("unable to alloc sym list");
		exit(1);
	}

	symp->name = (char *)malloc(strlen(name)+1);
	strcpy(symp->name, name);
	symp->kind = kind;
	symp->pkgno = cur_pkg;
	symp->next = *sym_list;
	*sym_list = symp;
}


write_output(pkgs, exports, output_file)
struct pkg_list *pkgs;
open_hashtab_t exports;
char *output_file;

/* Write the output file in the proper format.
 */
{
	FILE		*fd;
	int_hashtab_t	*tab = (int_hashtab_t *)exports;
	int		pkg_count;
	int		ent;
	int		sym_count;
	struct pkg	*pkgp;
	struct sym	*symp;

	for (pkg_count = 0, pkgp = pkgs->head; pkgp != NULL; pkg_count++, pkgp = pkgp->next)
		;

	if (output_file == NULL)
		fd = stdout;
	else if ((fd = fopen(output_file, "w")) == NULL) {
		perror("opening output file");
		exit(1);
	}

	fprintf(fd, "\
/* WARNING -- this file has been mechanically generated!\n\
 * DO NOT EDIT!\n\
 * Generated by %s from exports file %s\n\
 */\n\
\n\
", program, exports_file);

	fprintf(fd, "\
#define OPEN_HASH_MAXELEM %d\n\
\n\
", tab->h_header.hh_maxelem);

	fprintf(fd, "\
#include \"ldr_package.h\"\n\
#include \"ldr_hash.h\"\n\
#include \"open_hash.h\"\n\
#include \"open_hash_pvt.h\"\n\
\n\
struct export_symbol {\n\
	char		*se_name;\n\
	int		se_pkgno;\n\
	ldr_symval	se_value;\n\
};\n\
\n\
");

#ifdef	PREFIX_UNDERSCORE
	fprintf(fd, "\
#define	make_export_symbol(n, p, k) { \\\n\
	\"_\"#n, \\\n\
	p, \\\n\
	ldr_symval_init_abs(k, (univ_t)&n) \\\n\
	}\n\
\n\
");
#else	/* PREFIX_UNDERSCORE */
		fprintf(fd, "\
#define	make_export_symbol(n, p, k) { \\\n\
	#n, \\\n\
	p, \\\n\
	ldr_symval_init_abs(k, (univ_t)&n) \\\n\
	}\n\
\n\
");
#endif	/* PREFIX_UNDERSCORE */

	fprintf(fd, "\
%sldr_package_rec %s[] = {\n\
", do_static, pkg_name);

	for (pkgp = pkgs->head; pkgp != NULL; pkgp = pkgp->next) {

		fprintf(fd, "\
	{\n\
		LDR_PACKAGE_VERSION,\n\
		ldr_package,\n\
		\"%s\",\n\
	},\n\
", pkgp->name);
	}

	fprintf(fd, "\
};\n\
\n\
#define %s_count (sizeof(%s) / sizeof(ldr_package_rec))\n\
\n\
", pkg_name, pkg_name);

	for (ent = 0, sym_count = 0; ent < tab->h_header.hh_maxelem; ent++) {

		if ((symp = (struct sym *)tab->h_entries[ent].value) == NULL)
			continue;

		sym_count++;
		fprintf(fd, "\
%sstruct export_symbol __%s = make_export_symbol(%s, %d, %s);\n\
", do_static, symp->name, symp->name, symp->pkgno, symbol_kinds[(int)symp->kind]);
	}

	fprintf(fd, "\
\n\
%sint_hashtab_t %s = {\n\
	{\n\
		OPEN_HASH_MAXELEM,\n\
		%d,\n\
		(ldr_hash_p)hash_string,\n\
		(ldr_hash_compare_p)strcmp,\n\
		0,\n\
	},\n\
\n\
	{\n\
", do_static, table_name, sym_count);

	for (ent = 0, sym_count = 0; ent < tab->h_header.hh_maxelem; ent++) {

		if ((symp = (struct sym *)tab->h_entries[ent].value) == NULL) {

			fprintf(fd, "\
	{ NULL, NULL },\n\
");
		} else {

#ifdef	PREFIX_UNDERSCORE
			fprintf(fd, "\
	{ \"_%s\", &__%s },\n\
", symp->name, symp->name);
#else
				fprintf(fd, "\
	{ \"%s\", &__%s },\n\
", symp->name, symp->name);
#endif
		}
	}

	fprintf(fd, "\
	}\n\
};\n\
");

	if (output_file != NULL)
		fclose(fd);

	return(0);
}


ldr_heap_t	ldr_process_heap = NULL;

int
ldr_heap_malloc(heap, nbytes, type, ptr)
ldr_heap_t heap;
size_t nbytes;
ldr_malloc_t type;
univ_t *ptr;

{
	univ_t	res;

	if ((res = (univ_t)malloc(nbytes)) == NULL)
		return(-1);

	*ptr = res;
	return(0);
}


int
ldr_heap_free (heap, ptr)
ldr_heap_t heap;
univ_t ptr;
{
	return(free(ptr));
}
