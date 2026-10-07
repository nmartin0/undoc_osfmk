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
/* ldr_known_pkg.c
 * Routines for managing known package tables (loaded, private, and global).
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <strings.h>
#include <loader.h>

#include "ldr_types.h"
#include "ldr_lock.h"
#include "ldr_hash.h"
#include "chain_hash.h"
#include "squeue.h"
#include "dqueue.h"
#include "ldr_errno.h"
#include "ldr_malloc.h"
#include "ldr_sys_int.h"

#include "ldr_region.h"
#include "ldr_package.h"
#include "ldr_symbol.h"
#include "ldr_switch.h"
#include "ldr_known_pkg.h"
#include "ldr_module.h"


/* An empty kpt list record for initialization */

static const ldr_kpt_rec initial_kpt_rec = {
	NULL,				/* next */
	NULL,				/* name */
	NULL,				/* module */
	NULL				/* package */
	};



int
ldr_kpt_hdr_init(ldr_kpt_header *kpt_hdr, ldr_heap_t heap)

/* Initialize the specified KPT header, using storage from the
 * specified heap.  Returns LDR_SUCCESS on success, negative
 * error status on error.
 */
{
	ldr_kpt			kpt;	/* the kpt itself */
	int			rc;

	kpt_hdr->lkh_magic = LKH_MAGIC;
	dq_init(&(kpt_hdr->lkh_known_modules));

	kpt_hdr->lkh_heap = heap;

	if ((rc = chain_hash_create_heap(heap, LDR_N_INSTALLS,
					 (ldr_hash_p)hash_string,
					 (ldr_hash_compare_p)strcmp,
					 (chain_hashtab_t)&kpt)) != LDR_SUCCESS)
		return(rc);

	kpt_hdr->lkh_kpt = kpt;
	return(LDR_SUCCESS);
}


int
ldr_kpt_hdr_inherit(ldr_kpt_header *kpt_hdr)

/* Try to inherit the specified KPT header.  Basically just involves
 * error checking.  Returns LDR_SUCCESS on success, negative error
 * status on error.
 */
{
	int			rc;

	/* Validate KPT header */

	if (kpt_hdr->lkh_magic != LKH_MAGIC)
		return(LDR_EVERSION);

	if ((rc = chain_hash_inherit(kpt_hdr->lkh_kpt, (ldr_hash_p)hash_string,
				     (ldr_hash_compare_p)strcmp)) != LDR_SUCCESS)
		return(rc);

	return(LDR_SUCCESS);
}


int
ldr_kpt_copy(ldr_kpt_header *from_hdr, ldr_kpt_header *to_hdr)

/* Copy all the installed modules from the source KPT header to
 * the destination KPT header.  Used in constructing the global
 * KPT from a private KPT.  The module records and KPT list are
 * copied, using the destination KPT's heap for allocation.
 * Returns LDR_SUCCESS on success, negative error status on error.
 */
{
	ldr_module_rec		*mod;
	int			rc;

	for (mod = from_hdr->lkh_forw;
	     mod != (ldr_module_rec *)(&(from_hdr->lkh_forw));
	     mod = mod->lm_forw) {

		if ((rc = ldr_install_module(to_hdr, mod)) != LDR_SUCCESS)
			return(rc);
	}
	return(LDR_SUCCESS);
}


int
ldr_install_module(ldr_kpt_header *kpt_hdr, ldr_module_rec *mod)

/* Create a copy of the specified module record, and install the copy
 * in the specified known package table.  First, scan the list to make
 * sure it's not already there.  Then copy the module record (copying
 * only the relevant fields) into space allocated from the heap in the
 * kpt header.  Link the copied module record onto the installed module list
 * in the kpt header. Returns LDR_SUCCESS on success, negative error
 * status on error.
 */
{
	ldr_module_rec		*ins_mod;
	ldr_package_rec		*pkgs;
	ldr_kpt_rec		*kpt_list;
	int			npkgs;
	int			rc;

	if ((rc = ldr_lookup_installed_module(kpt_hdr, mod->lm_name,
					      &ins_mod)) == LDR_SUCCESS)
		return(LDR_EEXIST);

	/* Get module's export packages, from manager if necessary */

	if (mod->lm_export_pkg_count != 0) {
		npkgs = mod->lm_export_pkg_count;
		pkgs = mod->lm_export_pkgs;
	} else {
		if ((rc = LSW_GET_EXPORT_PKGS(mod, &npkgs, &pkgs)) != LDR_SUCCESS)
			return(rc);

		/* Must leave module record in a consistent state for
		 * later cleanup.
		 */

		lm_set_kpt_list(mod, npkgs, pkgs, NULL);
	}

	/* Copy the module record (including its package record) to the heap,
	 * and flag it as "installed" (as opposed to "loaded").
	 */

	if ((rc = ldr_module_copy(kpt_hdr->lkh_heap, mod, &ins_mod)) != LDR_SUCCESS)
		return(rc);
	lm_flag_installed(ins_mod);

	/* Now create kpt list for the module, from the right heap */

	if ((rc = ldr_kpt_list_create(kpt_hdr->lkh_heap, npkgs, &kpt_list)) != LDR_SUCCESS) {
		ldr_module_free_copy(kpt_hdr->lkh_heap, ins_mod);
		return(rc);
	}
	lm_set_kpt_list(ins_mod, ins_mod->lm_export_pkg_count,
			ins_mod->lm_export_pkgs, kpt_list);

	/* Insert module's packages into the kpt */

	if ((rc = ldr_kpt_insert(kpt_hdr->lkh_kpt, ins_mod)) != LDR_SUCCESS) {
		(void)ldr_module_free_copy(kpt_hdr->lkh_heap, ins_mod);
		return(rc);
	}

	/* Finally, link onto list of known modules */

	dq_ins_tail(&(kpt_hdr->lkh_known_modules), &(ins_mod->lm_list));
	lm_flag_onlist(ins_mod);

	return(LDR_SUCCESS);
}


int
ldr_remove_module(ldr_kpt_header *kpt_hdr, ldr_module_rec *mod)

/* Remove the specified module record from the known module list of
 * the specified KPT, and free it into the KPT's heap.  Module is
 * assumed to be linked to the KPT's known module list.  Returns
 * LDR_SUCCESS on success, negative error status on error.
 */
{
	ldr_module_free_copy(kpt_hdr->lkh_heap, mod);
	return(LDR_SUCCESS);
}


int
ldr_lookup_installed_module(ldr_kpt_header *kpt_hdr, const char *name,
			    ldr_module_rec **mod)

/* Look up the specified module by name in the list of modules installed
 * in the specified known package table.  If found, return the module
 * record copy describing the module.  Returns LDR_SUCCESS on success,
 * negative error status on failure.
 */
{
	ldr_module_rec		*tmp;

	for (tmp = kpt_hdr->lkh_forw;
	     tmp != (ldr_module_rec *)(&(kpt_hdr->lkh_forw));
	     tmp = tmp->lm_forw) {

		if (strcmp(name, tmp->lm_name) == 0) {
			*mod = tmp;
			return(LDR_SUCCESS);
		}
	}

	return(LDR_ENOMODULE);
}


int
ldr_kpt_list_create(ldr_heap_t heap, int count, ldr_kpt_rec **kpts)

/* Create a known package table list large enough to hold count records,
 * initialize it, and return in in *kpts.  Allocate space for the
 * kpt list from the specified heap.  Returns LDR_SUCCESS on success, or
 * negative error status on error.
 */
{
	ldr_kpt_rec 	*kpt_list;
	int		pkgno;
	int		rc;

	if (count == 0) {
		*kpts = NULL;
		return(LDR_SUCCESS);
	}

	if ((rc = ldr_heap_malloc(heap, count * sizeof(ldr_kpt_rec),
				  LDR_KPT_REC_T, (univ_t *)&kpt_list)) != LDR_SUCCESS)
		return(rc);

	for (pkgno = 0; pkgno < count; pkgno++)
		kpt_list[pkgno] = initial_kpt_rec;

	*kpts = kpt_list;
	return(LDR_SUCCESS);
}


int
ldr_kpt_lookup(ldr_kpt kpt, ldr_package_rec *pkg, ldr_kpt_rec **kpte)

/* Look up the specified package by name in the specified known package
 * table, and return the kpt record corresponding to it in
 * *kpt_rec.  Returns LDR_SUCCESS on success, or negative error status on
 * failure (including LDR_ENOSYM if the lookup fails).
 */
{
	if (chain_hash_lookup((chain_hashtab_t)kpt,
			      (const univ_t)pkg->lp_name,
			      (chain_hash_elem **)kpte) != LDR_SUCCESS)
		return(LDR_ENOPKG);
}


int
ldr_kpt_list_free(ldr_heap_t heap, int count, ldr_kpt_rec *kpts)

/* Free a list of kpt records containing count records.  Free the
 * storage into the specified heap.  Returns LDR_SUCCESS on success,
 * negative error status on error.
 */
{
	if (count == 0)
		return(LDR_SUCCESS);
	return(ldr_heap_free(heap, kpts));
}


int
ldr_kpt_insert(ldr_kpt kpt, ldr_module_rec *mod)

/* Insert all the packages in the specified module's export package
 * table list into the specified known package table.
 * Note that package names must be unique.  Return LDR_SUCCESS on
 * success or negative error status on error, including LDR_EDUPPKG
 * on duplicate package name.
 */
{
	int			pkgno;	/* package number */
	ldr_kpt_rec		*kpte;	/* temp for loop */
	chain_hash_elem		*elem;	/* temp for chain hash insert */
	int			rc;

	for_all_kptes(mod, pkgno, kpte) {

		/* Set up kpt rec to point to module and package record */

		kpte->lkp_module = mod;
		kpte->lkp_package = &mod->lm_export_pkgs[pkgno];
		kpte->lkp_name = mod->lm_export_pkgs[pkgno].lp_name;

		/* We want to insert this package only if another package with the
		 * same name isn't already present.  So, use chain_hash_search
		 * with the (INSERT|LOOKUP) option; if the key is already present,
		 * it will return the other package record in the specified element.
		 */

		elem = (chain_hash_elem *)kpte;
		if ((rc = chain_hash_search((chain_hashtab_t)kpt,
					    kpte->lkp_name, &elem,
					    (LDR_HASH_INSERT|LDR_HASH_LOOKUP))) != LDR_SUCCESS)
			return(rc);
		if (elem != (chain_hash_elem *)kpte) { /* duplicate; error */

			/* Null out this lpt list record so cleanup won't
			 * attempt to remove it from the hash table.
			 */

			ldr_log("ldr_kpt_insert: duplicate package name %s, module %s\n",
				kpte->lkp_name, mod->lm_name);
			*kpte = initial_kpt_rec;
			return(LDR_EDUPPKG);
		}
	}
	return(LDR_SUCCESS);
}


int
ldr_kpt_list_remove(ldr_kpt kpt, ldr_module_rec *mod)

/* Remove the specified module's kpt list from the specified known
 * package table.  Walk the kpt list, unhashing each record
 * from the kpt.  Returns LDR_SUCCESS on success, or negative error status
 * on error.
 */
{
	ldr_kpt_rec	*kpte;
	int		pkgno;
	int		rc;

	for_all_kptes(mod, pkgno, kpte) {

		if (kpte->lkp_name != NULL) {
			(void)chain_hash_delete(kpt, kpte->lkp_name);
			kpte->lkp_name = NULL;
		}
	}
	return(LDR_SUCCESS);
}


