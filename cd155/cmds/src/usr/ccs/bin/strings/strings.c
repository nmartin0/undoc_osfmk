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
#ifndef lint
#ifndef _NOIDENT
static char rcsid[] = "@(#) $RCSfile: strings.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:55:38 $";
#endif
#endif
/* static char sccsid[] = "@(#)strings.c	1.11  com/cmd/scan,3.1,9013 3/6/90 21:29:55"; */
/*
 * COMPONENT_NAME: (CMDSCAN) strings
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989, 1990
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

#include <stdio.h>
#include <locale.h>
#include <ctype.h>
#include <sys/file.h>
#if defined(NLS) || defined(KJI)
#include <NLchar.h>
#else
typedef char NLchar;
#endif

#include <ldfcn.h>

#ifdef MSG
#include "strings_msg.h" 
#define MSGSTR(n,s) NLgetamsg(MF_STRINGS, MS_STRINGS, n, s) 
#else
#define MSGSTR(n,s) s
#endif

/*
 * strings
 */


static void find(long cnt);
static int dirt(int c);
static void ScanRegion( ld_region * );

static char	*infile = "Standard input";
static int	oflg;
static int	asdata;
static long	offset;
static int	minlength = 4;

main(int argc, char *argv[])
{
#if defined(NLS) || defined(KJI)
	setlocale( LC_ALL, "" );
#endif
	argc--, argv++;
	while (argc > 0 && argv[0][0] == '-') {
		register int i;
		if (argv[0][1] == 0)
			asdata++;
		else for (i = 1; argv[0][i] != 0; i++) switch (argv[0][i]) {

		/*
		 * Option to preceed each buffer print by its offset in the
		 * file; in octal.
		 */
		case 'o':
			oflg++;
			break;

		/*
		 * Option to search the entire file, not just the data
		 * section for printable characters.
		 */
		case 'a':
			asdata++;
			break;

		/*
		 * Any other options, other than a number will cause
		 * execution to halt.  A number option (e.g. -2) will
		 * cause the minimum length to be that number.
		 */
		default:
			if (!isdigit((int)argv[0][i])) {
				fprintf(stderr, MSGSTR(USAGE, "Usage: strings [ -a ] [ -o ] [ -# ] [ file ... ]\n")); /*MSG*/
				exit(1);
			}
			minlength = argv[0][i] - '0';
			for (i++; isdigit((int)argv[0][i]); i++)
				minlength = minlength * 10 + argv[0][i] - '0';
			i--;
			break;
		}
		argc--, argv++;
	}
	do {
	  	raw_mo_header_t	header;
		ldfile	*ld;

		if (argc > 0) {
			if (freopen(argv[0], "r", stdin) == NULL) {
				perror(argv[0]);
				exit(1);
			}
			infile = argv[0];
			argc--, argv++;
		}
		fseek(stdin, 0, SEEK_SET);
		if (asdata ||
		    fread((void *)&header, sizeof(header), 1, stdin) != 1 ||
		    (header.rmoh_magic != OUR_MOH_MAGIC)) {
			fseek(stdin, 0, SEEK_SET);
			find((long) 100000000L);
			continue;
		}

		fseek( stdin, 0, SEEK_SET );

		ld = ldAlloc();
		ld->ld_macho_fd = stdin;
		ld->ld_filename = infile;
		ld->ld_is_open_flag = 1;

		if (!ldInit(ld))
		  fprintf(stderr, MSGSTR(NOT_MACHO, "strings: file %s is a damaged object file\n"), infile);

		/*
		 * Now, look at the initialized data regions (there are three of them) for
		 * strings
		 */
		ScanRegion( &ld->ld_region_list[REG_DATA_T] );
#ifdef REG_RDATA_T
		ScanRegion( &ld->ld_region_list[REG_RDATA_T] );
#endif
#ifdef REG_SDATA_T
		ScanRegion( &ld->ld_region_list[REG_SDATA_T] );
#endif
		ldClose(ld);
		ldFree(ld);
	} while (argc > 0);

	exit(0);
}

/*
 * Function: ScanRegion
 *
 * Description:	Position the file-pointer to the specified region types and call "find"
 *		for each sub-region.
 */

static void
ScanRegion( ld_region *region ) {

  while ( region->ld_reg_cmd ) {		/* Is there a "cmd" entry */
    region_command_t *section = region->ld_reg_cmd;		/* Equivalent to "COFF" section */

    if (section->ldc_header.ldci_section_len) {			/* Non-zero length section? */
      fseek(stdin, (long) section->ldc_header.ldci_section_off, SEEK_SET);
      find (section->ldc_header.ldci_section_len);
    }

    region = region->ld_next;			/* Advance to next subregion */
    if (!region) break;				/* There aren't anymore! */
  }
}

/*
 * Function: find
 *
 * Description: Find and print all valid characters from the file that was
 *              opened above.
 */

static void
find(long cnt)
{
	register int cc;
#if defined(NLS) || defined(KJI)
	NLchar c;
	static NLchar buf[BUFSIZ+1];
	register NLchar *cp;
	char unlc[4];			/* Make buffer big enough for ANY reasonable encoding */
#else
	register int c;
	static char buf[BUFSIZ];
	register char *cp;
#endif

	cp = buf, cc = 0;
	for (; cnt != 0; cnt--) {
		/*
		 * Get the character and check to see if it is valid.
		 */
#if defined(NLS) || defined(KJI)
		c = getwc(stdin);
		(void) NCencode(&c,&unlc);
		if (unlc[0] == '\n' || !(NCisNLchar(c)) ||
			(!(NCisprint(c)) && (unlc[0] != '\f')) || cnt == 0)
#else
		c = getc(stdin);
		if (c == '\n' || dirt(c) || cnt == 0)
#endif
		{
		/*
		 * If the character read is "dirt" or a terminating character,
		 * then, if buf contains more than minlength characters
		 * print the buffer with newline termination, and reset the
		 * buffer.
		 */
			if (cp > buf && cp[-1] == '\n')
				--cp;
			*cp++ = 0;
			if (cp > &buf[minlength]) {
				if (oflg)
					printf("%7o ", ftell(stdin) - cc - 1);
#if defined(NLS) || defined(KJI)
				NLprintf("%S\n", buf);
#else
				printf("%s\n", buf);
#endif
			}
			cp = buf, cc = 0;
		} else {
			/*
			 * As long as the current line being read is less
			 * than BUFSIZ - sizeof(NLchar) (to allow space for multi-byte
			 * characters) then add characters to the buffer.
			 *
			 * If the last valid character of a file falls on a
			 * buffer boundary, then the next character will be
			 * EOF, and the above "dirt" condition will occur.
			 */
			if (cp < &buf[BUFSIZ - sizeof(NLchar)]) {
				*cp++ = c;
				cc++;
			} else {
				/*
				 * If the line read is too long, then
				 * print it out as if a terminating
				 * character were found.
				 */
				*cp++ = c;
				*cp++ = 0;
				if (oflg)
					printf("%7o ", ftell(stdin) - cc - 1);
#if defined(NLS) || defined(KJI)
				NLprintf("%S", buf);
#else
				printf("%s", buf);
#endif
				cp = buf, cc = 0;
			}
		}
		/*
		 * If an error condition occurs when reading the input
		 * stream, or we reach end of file, end current search.
		 */
		if (ferror(stdin) || feof(stdin))
			break;
	}
}

/*
 * Function: dirt
 *
 * Description: Static function to see if normal (Non-NLS) characters
 *              are valid for placement into the print buffer.
 */

static int
dirt(int c)
{

	switch (c) {

	case '\f':
		return (0);

	case 0177:
		return (1);

	default:
		return (!isprint(c));
	}
}

