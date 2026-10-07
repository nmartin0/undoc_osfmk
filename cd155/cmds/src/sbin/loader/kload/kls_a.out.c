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
/*
 * OSF/1 Release 1.0.1
 */

#include <sys/types.h>
#include <sys/param.h>
#include <string.h>
#include <loader.h>
#include <unistd.h>

#include <a.out.h>

#include "ldr_types.h"
#include "ldr_lock.h"
#include "ldr_hash.h"
#include "chain_hash.h"
#include "open_hash.h"
#include "dqueue.h"
#include "ldr_errno.h"
#include "ldr_malloc.h"

#include "ldr_region.h"
#include "ldr_package.h"
#include "ldr_symbol.h"
#include "ldr_known_pkg.h"
#include "ldr_module.h"
#include "ldr_switch.h"
#include "ldr_sys_int.h"

typedef struct handle_data {
	open_hashtab_t  export_list;
	int             region_count;
	ldr_region_rec *region_list;
	ldr_entry_pt_t  entry_pt;
} handle_data_t;

static ldr_package_rec kernel_package = {
	LDR_PACKAGE_VERSION,
	ldr_package
};
extern char *kls_default_package_name;

static int get_exports(ldr_file_t, struct exec *, open_hashtab_t *);
static int get_regions(ldr_file_t, struct exec *, int *, ldr_region_rec **);

int
kls_aout_recog(filename, fd, handle)
	const char        *filename;
	ldr_file_t         fd;
	ldr_module_handle *handle;
{
	ldr_region_rec *list;
	struct exec    *exec;
	open_hashtab_t  table;
	handle_data_t  *hdp;
	int             count, rc;

	/* read the exec header */
	if ((rc = ldr_lseek(fd, 0, SEEK_SET)) < 0)
		return(LDR_ENOEXEC);
	if ((rc = ldr_malloc(sizeof(*exec), LDR_AOUT_T,
	    (univ_t *)&exec)) != LDR_SUCCESS)
		return(rc);
	if ((rc = ldr_read(fd, (char *)exec, sizeof(*exec))) < 0)
		return(LDR_ENOEXEC);

	/* check the magic number */
	if (N_BADMAG(*exec))
		return(LDR_ENOEXEC);

	/* get/build export list */
	if ((rc = get_exports(fd, exec, &table)) != LDR_SUCCESS)
		return(LDR_ENOEXEC);

	/* get/build region list */
	if ((rc = get_regions(fd, exec, &count, &list)) != LDR_SUCCESS)
		return(LDR_ENOEXEC);

	if ((rc = ldr_malloc(sizeof(*hdp), LDR_AOUT_T,
	    (univ_t *)&hdp)) != LDR_SUCCESS)
		return(rc);

	hdp->export_list = table;
	hdp->region_count = count;
	hdp->region_list = list;
	hdp->entry_pt = (ldr_entry_pt_t)exec->a_entry;

	(void)ldr_close(fd);

	*handle = (ldr_module_handle)hdp;
	return(LDR_SUCCESS);
}

int
kls_aout_map_regions(handle, allocsp, reg_count, regions)
	ldr_module_handle handle;
	ldr_region_allocs *allocsp;
	int *reg_count;
	ldr_region_rec **regions;
{
	handle_data_t		*hdp;

	hdp = (handle_data_t *)handle;
	*reg_count = hdp->region_count;
	*regions = hdp->region_list;
	return(LDR_SUCCESS);
}

int
kls_aout_lookup_export(handle, package, symbol)
	ldr_module_handle handle;
	ldr_package_rec  *package;
	ldr_symbol_rec   *symbol;
{
	handle_data_t		*hdp;
	open_hashtab_t		table;
	ldr_symval		*sym;
	int			rc;

	if (strcmp(package->lp_name, kernel_package.lp_name))
		return(LDR_ENOPKG);

	hdp = (handle_data_t *)handle;
	table = hdp->export_list;

	if ((rc = open_hash_lookup(table, (const univ_t)symbol->ls_name,
	    (univ_t *)&sym)) != LDR_SUCCESS)
		return(rc);

	symbol->ls_value = *sym;
	return(LDR_SUCCESS);
}

int
kls_aout_get_entry_pt(handle, entry_pt)
	ldr_module_handle handle;
	ldr_entry_pt_t *entry_pt;
{
	handle_data_t		*hdp;

	hdp = (handle_data_t *)handle;
	*entry_pt = hdp->entry_pt;
	return(LDR_SUCCESS);
}

int
kls_aout_get_export_pkgs(handle, count, packages)
	ldr_module_handle handle;
	int *count;
	ldr_package_rec **packages;
{
	kernel_package.lp_name = kls_default_package_name;

	*count = 1;
	*packages = &kernel_package;

	return(LDR_SUCCESS);
}

int
kls_aout_get_exports(handle, count, exports)
	ldr_module_handle handle;
	int *count;
	ldr_symbol_rec **exports;
{
	/* This should never be needed, so don't implement it */

	return(LDR_EINVAL);
}

static int
get_exports(fd, exec, tablep)
	ldr_file_t      fd;
	struct exec    *exec;
	open_hashtab_t *tablep;
{
	open_hashtab_t table;
	struct nlist  *symbols, *symbols_end, *syp;
	char          *strings, *name;
	size_t         size;
	int            export_count, offset, rc, i;
	ldr_symval    *export_list, *svp;

	/* check for no symbols */
	if (!exec->a_syms) {
		*tablep = (open_hashtab_t *)0;
		return(LDR_SUCCESS);
	}

	/* read strings */
	offset = N_STROFF(*exec);
	if ((rc = ldr_lseek(fd, offset, SEEK_SET)) < 0)
		return(rc);
	if ((rc = ldr_read(fd, (char *)&size, sizeof(size))) < 0)
		return(rc);
	if (!size) {
		*tablep = (open_hashtab_t *)0;
		return(LDR_SUCCESS);
	}
	if ((rc = ldr_lseek(fd, offset, SEEK_SET)) < 0)
		return(rc);
	if ((rc = ldr_malloc(size, LDR_AOUT_T,
	    (univ_t *)&strings)) != LDR_SUCCESS)
		return(rc);
	if ((rc = ldr_read(fd, (char *)strings, size)) < 0)
		return(rc);

	/* read symbols */
	offset = N_SYMOFF(*exec);
	if ((rc = ldr_lseek(fd, offset, SEEK_SET)) < 0)
		return(rc);
	if ((rc = ldr_malloc(exec->a_syms, LDR_AOUT_T,
	    (univ_t *)&symbols)) != LDR_SUCCESS)
		return(rc);
	if ((rc = ldr_read(fd, (char *)symbols, size)) < 0)
		return(rc);

	/* get end of symbol table */
#define	add(a, b) \
	(((unsigned int)(a)) + ((unsigned int)(b)))
	symbols_end = (struct nlist *)add(symbols, exec->a_syms);

	/* count up exports */
	export_count = 0;
	for (syp = symbols; syp < symbols_end; syp++)
		if (syp->n_type & N_EXT)
			export_count++;

	/* build export list */
	size = export_count * sizeof(*export_list);
	if ((rc = ldr_malloc(size, LDR_AOUT_T,
	    (univ_t *)&export_list)) != LDR_SUCCESS)
		return(rc);

	if ((rc = open_hash_create(export_count, (ldr_hash_p)hash_string,
	    (ldr_hash_compare_p)strcmp, (open_hash_flags_t)0, &table)) != LDR_SUCCESS)
		return(rc);

	svp = export_list;
	for (syp = symbols; syp < symbols_end; syp++) {
		if (syp->n_type & N_EXT) {

			svp->ls_tag = ldr_sym_abs;

			if (syp->n_type & N_TEXT) {
				svp->ls_kind = ldr_sym_function;
			} else if ((syp->n_type & N_DATA)
			    || (syp->n_type & N_BSS)) {
				svp->ls_kind = ldr_sym_data;
			} else {
				svp->ls_kind = ldr_sym_unknown;
			}

			svp->ls_abs = (univ_t)syp->n_value;

			name = strings + syp->n_un.n_strx;

#if	i860
			/* ANSI C doesn't allow &(cast)svp, OK to (cast)&svp
			 * see loader/include/open_hash.h, open_hash_insert()
			 */
			if ( (rc=open_hash_insert(table,
			    (const univ_t)name,
			    svp)) != LDR_SUCCESS)
				return(rc);
#else
			if ( (rc=open_hash_insert(table,
			    (const univ_t)name,
			    (univ_t)svp)) != LDR_SUCCESS)
				return(rc);
#endif

			svp++;
		}
	}

	*tablep = table;
	return(LDR_SUCCESS);
}

static int
get_regions(fd, exec, countp, listp)
	ldr_file_t       fd;
	struct exec     *exec;
	int             *countp;
	ldr_region_rec **listp;
{
	ldr_region_rec *region_list, *r;
	int             rc, region_count;
	int             text_start, data_start, bss_start;
	int             text_size, data_size, bss_size;

	/* compute addresses of regions */
#ifdef	i386
	/*
	 * This should be in one or more machine dependent files.
	 * Rather than going to the trouble to create them now,
	 * simply create them when and if the a.out KLS 
	 * format dependent manager is needed on some platfrom
	 * other than the 386.
	 */

#define KERNEL_TEXT_START    0xc0100000		/* XXX */

	text_start = KERNEL_TEXT_START;
	text_size = exec->a_text;

	data_start = text_start + roundup(text_size, CLBYTES);
	data_size = exec->a_data;

	bss_start = data_start + data_size;
	bss_size = exec->a_bss;
#endif

	/* count up regions */
	region_count = 0;
	if (text_size)
		region_count++;
	if (data_size)
		region_count++;
	if (bss_size)
		region_count++;

	/* allocate region list */
	if ((rc = ldr_regions_create(region_count, LDR_REGION_VERSION,
				     &region_list)) != LDR_SUCCESS)
		return(rc);
	r = region_list;

	/* fill in region list */
	if (text_size) {
		r->lr_version = LDR_REGION_VERSION;
		r->lr_name = "text";
		r->lr_prot = LDR_R|LDR_X;
		r->lr_vaddr = (univ_t)text_start;
		r->lr_mapaddr = (univ_t)-1;
		r->lr_size = (size_t)text_size;
		r->lr_flags = LRF_LOADED;
		r++;
	}
	if (data_size) {
		r->lr_version = LDR_REGION_VERSION;
		r->lr_name = "data";
		r->lr_prot = LDR_R|LDR_W|LDR_X;
		r->lr_vaddr = (univ_t)data_start;
		r->lr_mapaddr = (univ_t)-1;
		r->lr_size = (size_t)data_size;
		r->lr_flags = LRF_LOADED;
		r++;
	}
	if (bss_size) {
		r->lr_version = LDR_REGION_VERSION;
		r->lr_name = "bss";
		r->lr_prot = LDR_R|LDR_W|LDR_X;
		r->lr_vaddr = (univ_t)bss_start;
		r->lr_mapaddr = (univ_t)-1;
		r->lr_size = (size_t)bss_size;
		r->lr_flags = LRF_LOADED;
		r++;
	}

	/* return region list and count */
	*countp = region_count;
	*listp = region_list;
	return(LDR_SUCCESS);
}
