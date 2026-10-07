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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: dump.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:54:56 $";
#endif
/*
 * COMPONENT_NAME: CMDAOUT (dump command)
 *
 * FUNCTIONS: Mdump, dump_alloc, error, fread_OK, fseek_OK,
 *	hex_dump, output, pr_name, process_file, title, usage
 *
 * ORIGINS: 27, 3
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * dump.c	1.13  com/cmd/aout/dump,3.1,9013 2/20/90 15:52:09";
 */

#include <stdio.h>
#include <ctype.h>

#include <errno.h>
#include "dump_defs.h"
#include <locale.h>

nl_catd catd;

extern void exit();
extern void *malloc();
extern char *strchr();

extern void dump_rose();
/* extern void dump_archive(); */

/*
 * Local macros for representing the MACH-O magic number.
 */
#include <mach_o_header.h>
#define MOH_1 ((MOH_MAGIC >> 24) & 0xff)
#define MOH_2 ((MOH_MAGIC >> 16) & 0xff)

struct obj_format {
	char		*f_name;
	unsigned char	f_magic1;
	unsigned char	f_magic2;
	void		(*f_func)();
} formats[] = {

/* Note the problems for little-endian/big-endian co-existance this structure provides */
/* This is structured for little-endian. The macros can be reworked for big-endian. */

	{ "OSF/ROSE",	MOH_1,	MOH_2,	dump_rose	},
/* 	{ "ARCHIVE",	'!',	'<',	dump_archive	}, */
	{ NULL,		'\0',	'\0',	(void (*)()) 0	}
};

/* Flags that can be modified by command line arguments */

/* Object file independent flags */

int
	Head_sect = 0,		/* -L */
	Obj_sect = 0,		/* -d */
	Print_header = 1,	/* -p */
	Reloc = 0,		/* -r */
	Str_table = 0,		/* -s */
	Sym_table = 0,		/* -t */
	Symbolic = 0,		/* -v */
	File_header = 0,	/* -h */
	Entry = 0,		/* -e */
	Underline = 0,		/* no longer used */
        Prefix_names = 0,	/* -o */
        Ac_component_names = 0; /* -O */

/* MACH-O specific flags */
/* Note that -A sets all flags */

int	Export_syms = 0,	/* -E */
	Import_syms = 0,	/* -I */
	Debug_syms = 0,		/* -S */
	Defined_syms = 0,	/* -D */
        All_cmds = 0,		/* -F */
	Info_cmd = 0,		/* -i */
	Load_map = 0,		/* -M */
        Pkg_names = 0;		/* -P */

/* Internal state */

char	*Current_file,		/* current object file being processed */
	*Member_name = NULL;	/* name of archive member being processed */
int	Obj_flag;
long	File_origin;
char	*Command_name = NULL;

main(argc, argv)
	int argc;
	char *argv[];
{
	register char	*p;
	register int	c;
	FILE		*filep;
	extern char	*optarg;
	extern int	optind;

	catd = catopen(MF_ODUMP, 0);

	(void)setlocale(LC_ALL, "");

/* The following code for processing -t range limits is not used. */
	/* modify the command line arguments so getopt() can be used.
	 *	change all '+' flags to '-' and change letter to upper case
	 *	change -t to -+ if numeric option follows.
	 */
/*
 *	for (c = 1; c < argc; ++c)
 *	{
 *		p = argv[c];
 *
 *		if (p[0] == '-' && p[1] == 't')
 *		{
 *			if (isdigit((int)p[3]) ||
 *			(p[3] == '\0' && isdigit((int)*argv[c+1])))
 *				p[1] = '+';
 *		}
 *		else if (p[0] == '+')
 *		{
 *			p[1] = toupper((int)p[1]);
 *			p[0] = '-';
 *		}
 *	}
 */
	Command_name = argv[0];

	/* let getopt() parse the flags */
	while ((c = getopt(argc, argv, "ADEFGILMO:PSdehoprstv")) != EOF)
	{
		switch (c)
		{
		  case 'A':	/* set all object file options and verbose */
			++Str_table; ++Entry; ++File_header; ++Head_sect; 
			++Info_cmd; ++Load_map; ++Reloc; ++Obj_sect;
			++Sym_table; ++Symbolic;
			break;

		  case 'd':	/* dump object file section contents */
			++Obj_sect;
			break;

		  case 'D':	/* dump all the defined symbols */
			++Defined_syms;
			break;

		  case 'e':	/* dump the entry information */
			++Entry;
			break;

		  case 'E':	/* dump the exported (defined) symbols */
			++Export_syms;
			break;

		  case 'F':	/* dump ALL the load commands */
			++All_cmds;
			error("-F is not implemented yet.\n");
			break;

		  case 'G': 	/* dump the information section */
			++Info_cmd;
			break;

		  case 'h':	/* dump the file header */
			++File_header;
			break;

		  case 'I':	/* dump the import symbols */
			++Import_syms;
			break;

		  case 'L':
			++Head_sect;
			break;

		  case 'M':	/* dump load command map */
			++Load_map;
			break;

		  case 'o':	/* prefix each line with object name */
			++Prefix_names;
			error("-o is not implemented yet.\n");
			break;

		  case 'O':	/* specifies desired archive component names */
			++Ac_component_names;
			error("-O is not implemented yet.\n");
			/* pick up next arg (name list) */
			break;

		  case 'p':	/* do not print headers */
			Print_header = 0;
			break;

		  case 'P':	/* dump the package names */
			++Pkg_names;
			break;

		  case 'r':	/* dump relocation information */
			++Reloc;
			break;

		  case 's':	/* dump the string table */
			++Str_table;
			break;

		  case 'S':	/* dump the debug symbols */
			++Debug_syms;
			break;

		  case 't':	/* dump symbol table entries */
			++Sym_table;
			break;

		  case 'v':	/* dump in symbolic form (not numeric) */
			++Symbolic;
			break;

		  default:	/* modified flags will have erroneous message */
			usage();
			/*NOTREACHED*/
		}
	}

	if (optind >= argc)
		usage();

	Obj_flag = Str_table | Reloc | Obj_sect | Sym_table 
			| Head_sect | Debug_syms | Import_syms | Export_syms
			| File_header | Info_cmd | Entry | Load_map
			| Defined_syms | Pkg_names;
	if (!Obj_flag) {
		(void) fprintf(stderr, MSGSTR(ODUMP_ONE_MSG, ODUMP_ONE));
		usage();
	}

	/* Obj_flag was used along with archive-related options to make
	   sure that at least one "real" option was specified. 
	   ODUMP_ONE_MSG was used for the error message. */

	for (; optind < argc; ++optind)
	{
		Current_file = argv[optind];

		if ((filep = fopen(Current_file, "r")) == NULL)
		{
			error(MSGSTR(CANT_OPEN_MSG, CANT_OPEN));
			continue;
		}

		process_file(filep);
		(void) fclose(filep);
	}

	exit(0);
	/*NOTREACHED*/
}

/*
 * dump_alloc()	- allocate space while checking return value
 */
char *
dump_alloc(size)
	unsigned int size;
{
	register char *ptr = malloc(size);

	if (ptr == NULL)
	{
		error(MSGSTR(NO_MEM_MSG, NO_MEM));
		exit(1);
	}

	return(ptr);
}

/*
 * error()	- print message with program name and current filename
 */
error(message)
	char *message;
{
	fflush(stdout);

	if (Member_name && *Member_name)
		(void) fprintf(stderr, "%s: %s[%s]: ",
				Command_name, Current_file, Member_name);
	else
		(void) fprintf(stderr, "%s: %s: ", Command_name, Current_file);

	if (errno != 0)
	{
		errno = 0;
	}
	(void) fprintf(stderr, "%s", message);
	fflush(stderr);
}

fread_OK(ptr, nitems, stream)
	char *ptr;
	size_t nitems;
	FILE *stream;
{
	if (fread(ptr, (size_t)1, nitems, stream) == nitems)
		return(1);

	error(MSGSTR(READ_ERR_MSG, READ_ERR));
	return(0);
}

fseek_OK(stream, offset)
	FILE *stream;
	long offset;
{
	if (fseek(stream, offset, 0) == 0)
		return(1);

	error(MSGSTR(SEEK_ERR_MSG, SEEK_ERR));
	return(0);
}


hex_dump(prompt, fp, offset, length)
	char *prompt;
	FILE *fp;
	long offset, length;
{
	register int	c,
			i;
	register long	l,
			o;
	int		inc = 32,
			j;

	output(prompt);

	if (!fseek_OK(fp, File_origin + offset))
		return;

	for (o = 0, l = length; l > 0; l -= inc, o += inc)
	{
		(void) printf("%5lx: ", o);
		j = (l > inc) ? inc : l;

		for (i = 0; i < j; ++i)
		{
			if ((c = getc(fp)) == EOF)
			{
				error(MSGSTR(READ_ERR_MSG, READ_ERR));
				return;
			}

			(void) printf("%02X", c & 0xFF);

			if ((i % 4) == 3)
				(void) putchar(' ');
		}

		(void) putchar('\n');
	}
}

/*
 * output()	- output a string possibly underlining it
 */
output(string)
	char *string;
{
	register char *p;

	if (Underline)
	{
		for (p = string; *p; ++p)
			(void) printf("_\b%c", *p);
	}
	else
		(void) printf(string);

	(void) printf(":\n");
}

/*
 * pr_name()	- print filename
 */
pr_name()
{
	char	buf[256];

	(void) putchar('\n');
	if (Member_name)
		(void) sprintf(buf, "%s[%s]", Current_file, Member_name);
	else
		(void) sprintf(buf, "%s", Current_file);

	output(buf);
}

process_file(fp)
	FILE *fp;
{
	register struct obj_format *p;
	int	magic1,
		magic2;

	File_origin = ftell(fp);

	if ((magic1 = getc(fp)) == EOF || (magic2 = getc(fp)) == EOF)
	{
		error(MSGSTR(NOT_AN_OBJ_MSG, NOT_AN_OBJ));
		return;
	}

	(void) fseek(fp, (long)-2, (int)1);

	for (p = formats; p->f_name != NULL; ++p)
	{
		if (magic1 == p->f_magic1 && magic2 == p->f_magic2)
			break;
	}

	if (p->f_name == NULL)
	{
		error(MSGSTR(NOT_AN_OBJ_MSG, NOT_AN_OBJ));
		return;
	}

	/* fix to print member name for archives */

	printf("\n\tOBJECT FILE:  %s\n", Current_file);
	printf("\t______________________________________\n");
	printf("\t--------------------------------------\n");

	(p->f_func)(fp);
}

title(string)
	char *string;
{
	(void) printf("\n\t\t\t***%s***\n", string);
}

/*
 * usage()	- show how this program was intended to be used
 */
usage()
{
	(void) fprintf(stderr, MSGSTR(USAGE_MSG, USAGE));
	exit(1);
}
