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

/* currently, only supports one symbols section and one strings section */

#include <sys/types.h>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <mach_o_header.h>
#include <mach_o_format.h>

#include <nlist.h>

static int  file_open(char *);
static int  file_read(int, int, void *, int);
static void file_close(int);
static int  alloc(int, void **);

int
macho_nlist(filename, nl)
	char         *filename;
	struct nlist *nl;
{
	char                    buffer[MO_SIZEOF_RAW_HDR];
	char                   *cmds = (char *)0;
	char                   *cp;
	load_cmd_map_command_t *map;
	mo_header_t             moh;
	symbols_command_t      *syp;
	strings_command_t      *stp;
	ldc_header_t           *hp;
	int                     fd = -1;
	char                   *strings = (char *)0;
	symbol_info_t          *symbols = (symbol_info_t *)0;
	int                     i, symbols_count;
	symbol_info_t          *sip;
	struct nlist           *nlp;
	int                     unfound = 0;

	if ((fd = file_open(filename)) == -1)
		goto error;

	if (file_read(fd, 0, (void *)buffer, MO_SIZEOF_RAW_HDR))
		goto error;

	if (decode_mach_o_hdr((void *)buffer, MO_SIZEOF_RAW_HDR,
	    MOH_HEADER_VERSION, &moh))
		goto error;

	if (moh.moh_magic != MOH_MAGIC)
		goto error;

	if (alloc(moh.moh_sizeofcmds, (void **)&cmds))
		goto error;

	if (file_read(fd, moh.moh_first_cmd_off, (void *)cmds,
	    moh.moh_sizeofcmds))
		goto error;

#define	XLATE(offset) \
	(((char *)cmds) + (((int)(offset)) - ((int)moh.moh_first_cmd_off)))

	map = (load_cmd_map_command_t *)XLATE(moh.moh_load_map_cmd_off);

	for (i = 0; i < map->lcm_nentries; i++) {
		hp = (ldc_header_t *)XLATE(map->lcm_map[i]);
		if (hp->ldci_cmd_type == LDC_SYMBOLS) {
			if (symbols)
				continue;
			syp = (symbols_command_t *)hp;
			if (syp->symc_kind != SYMC_DEFINED_SYMBOLS)
				continue;
			if (alloc(syp->ldc_section_len, (void **)&symbols))
				goto error;
			if (file_read(fd, syp->ldc_section_off, (void *)symbols,
			    syp->ldc_section_len))
				goto error;
			symbols_count = syp->symc_nentries;
		} else if (hp->ldci_cmd_type == LDC_STRINGS) {
			if (strings)
				continue;
			stp = (strings_command_t *)hp;
			if (alloc(stp->ldc_section_len, (void **)&strings))
				goto error;
			if (file_read(fd, stp->ldc_section_off, (void *)strings,
			    stp->ldc_section_len))
				goto error;
		}
	}

	if (!symbols)
		goto error;

	if (!strings)
		goto error;

	for (nlp = nl; nlp->n_name; nlp++) {
		sip = symbols;
		for (i = 0; i < symbols_count; i++) {
			if ((!strcmp(nlp->n_name, (strings + sip->si_symbol_name)))
			  && ((sip->si_flags & SI_ABSOLUTE_VALUE_F)
			    || (sip->si_flags & SI_LITERAL_F))) {
				nlp->n_value = (unsigned long)sip->si_abs_val;
				nlp->n_type = -1;
				break;
			}
			sip++;
		}
		if (!nlp->n_type)
			unfound++;
	}

	free(strings);
	free(symbols);
	free(cmds);
	file_close(fd);
	return(unfound);

error:
	if (strings)
		free(strings);
	if (symbols)
		free(symbols);
	if (cmds)
		free(cmds);
	if (fd != -1)
		file_close(fd);
	return(-1);
}

static int
file_open(filename)
	char *filename;
{
	int fd;

	if ((fd = open(filename, O_RDONLY)) == -1)
		return(-1);
	return(fd);
}

static int
file_read(fd, offset, buffer, size)
	void *buffer;
{
	int count;

	if (lseek(fd, offset, SEEK_SET) == -1)
		return(-1);
	if ((count = read(fd, buffer, size)) == -1)
		return(-1);
	if (count != size)
		return(-1);
	return(0);
}

static void
file_close(fd)
{
	(void)close(fd);
}

static int
alloc(size, pp)
	void **pp;
{
	void *mem;

	if (mem = malloc(size)) {
		*pp = mem;
		return(0);
	}
	return(-1);
}
