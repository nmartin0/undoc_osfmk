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
 * ldr_abs.c
 * COFF absolute code loader routines
 *
 * Loader switch routines called by the object file
 * format independent loader when accessing COFF files.
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>

#include "coff_machdep.h"

#include "loader.h"
#include "ldr_types.h"
#include "ldr_sys_int.h"
#include "ldr_windows.h"
#include "ldr_malloc.h"
#include "ldr_region.h"
#include "ldr_package.h"
#include "ldr_symbol.h"
#include "ldr_switch.h"

#include "coff_types.h"
#include "ldr_errno.h"

#ifndef TRUE
#define TRUE	1
#define	FALSE	0
#endif

/* #define	DEBUG */

#ifdef	DEBUG
#define	dprintf(x)	ldr_msg x
#else
#define	dprintf(x)
#endif

#define	round(x, s)	(((unsigned)(x) + (unsigned)(s) - 1) & ~((unsigned)(s) - 1))
#define	trunc(x, s)	(((unsigned)(x)) & ~((unsigned)(s) - 1))
#define aligned(x, s)	(((unsigned)(x) & ((unsigned)(s) - 1)) == 0)

#define MAX_COFF_ABS_REGIONS	3	/* no of regions in a absolute coff file */

/*
 *	Names of the sections to be loaded from absolute code
 */

#define	TEXT_REGION		".text"
#define DATA_REGION		".data"
#define BSS_REGION		".bss"

#define TEXT_REGNO		0
#define	DATA_REGNO		1
#define	BSS_REGNO		2

/* Forward references */

static int internal_map_region(ldr_file_t fd, struct filehdr *fhdr,
			       struct aouthdr *ohdr, ldr_region_allocs *allocps,
			       int regno, ldr_region_rec *region);
static int internal_free_regions(int nregions, ldr_region_rec *regions,
				 ldr_region_allocs *allocps);
static int internal_unload_region(int regno, ldr_region_rec *region,
				  ldr_region_allocs *allocps);

static int coff_recognizer(const char *filename, ldr_file_t fd,
			       ldr_module_handle *mod);
static int coff_get_static_dep(ldr_module_handle handle, int depno,
				   char **dep);
static int coff_get_imports(ldr_module_handle handle, int *pkg_count,
				ldr_package_rec **pkgs, int *sym_count,
				ldr_symbol_rec **imports);
static int coff_abs_map_regions(ldr_module_handle handle, ldr_region_allocs
				    *allocps, int *reg_count,
				    ldr_region_rec **regions);
static int coff_get_export_pkgs(ldr_module_handle handle, int *count,
				    ldr_package_rec **packages);
static int coff_get_exports(ldr_module_handle handle, int *sym_count,
				ldr_symbol_rec **exports);
static int coff_lookup_export(ldr_module_handle handle,
				  ldr_package_rec *package,
				  ldr_symbol_rec *symbol);
static int coff_relocate(ldr_module_handle handle, int nregions,
			     ldr_region_rec regions[], int npackages,
			     ldr_package_rec import_pkgs[], int nimports,
			     ldr_symbol_rec imports[]);
static int coff_get_entry_pt(ldr_module_handle mod, ldr_entry_pt_t *entry_pt);
static int coff_run_inits(ldr_module_handle handle, entry_pt_kind kind);
static int coff_cleanup(ldr_module_handle mod);
static int coff_unload(ldr_module_handle handle, ldr_region_allocs *allocps,
			   int reg_count, ldr_region_rec *regions,
			   int ipkg_count, ldr_package_rec *import_pkgs,
			   int import_count, ldr_symbol_rec *imports,
			   int epkg_count, ldr_package_rec *export_pkgs);


/* The loader switch entry for the coff manager */

const struct loader_switch_entry coff_switch_entry = {
	LSW_VERSION,
	LSF_MUSTOPEN,
	coff_recognizer,
	coff_get_static_dep,
	coff_get_imports,
	coff_abs_map_regions,
	coff_get_export_pkgs,
	coff_get_exports,
	coff_lookup_export,
	coff_relocate,
	coff_get_entry_pt,
	coff_run_inits,
	coff_cleanup,
	coff_unload,
};


int
ldr_coff_entry(ldr_context_t ctxt)

/* The manager entry point is called with a pointer to a loader context.
 * It is responsible for linking its switch entry into the context's
 * switch (by calling ldr_switch_ins_tail()).  This procedure
 * allows dynamically-loaded auxiliary managers to be initialized in
 * the same way as statically-linked managers.
 */
{
	return(ldr_switch_ins_tail(ctxt, (ldr_switch_t)&coff_switch_entry));
}


/*
 * The recognizer routine checks to see whether the specified file
 * (opened for atleast O_READ|O_EXEC access mode on the file descriptor fd)
 * is of an object file format supported by this format-dependent 
 * manager.  It returns LDR_SUCCESS on success or a negative loader error
 * status on failure.  On success, the format-dependent manager's handle
 * is left in the handle variable.  Also, after a sucessful recognition,
 * the open file descriptor is the responsiblity of the format-dependent
 * manager; it is never used again by the format-independent manager.
 */

static int 
coff_recognizer(const char *filename, ldr_file_t fd, ldr_module_handle *mod)
{
	coff_module_handle_t module; 
	int rc;			/* return code */
	ldr_window_t *wp;	/* window pointer for file header */
	ldr_window_t *owp;	/* window pointer for optional header */
	struct filehdr *hdr;	/* COFF file header */
	struct aouthdr *ohdr;	/* COFF optional header */

	/* window the header of file */
	wp = ldr_init_window(fd);
	hdr = (struct filehdr *)
		ldr_file_window(0, sizeof(struct filehdr), wp);
	if (hdr == NULL) {
		dprintf(("coff_recognizer: failure to window file header\n"));
		return LDR_ENOEXEC;
	}

	/* check its magic number, then look for optional header */
	if (COFF_MAGICOK(hdr)) {
		rc = ldr_malloc(sizeof(struct coff_module_handle), LDR_COFF_T,
			       (univ_t *)&module);
		if (rc != 0) {
			dprintf(("coff_recognizer: alloc module handle failed\n"));
			ldr_unwindow(wp);
			return rc;
		}

		/* if optional header exists, window it */
		if (hdr->f_opthdr > 0) {
			owp = ldr_init_window(fd);
			ohdr = (struct aouthdr *)
				ldr_file_window(sizeof(struct filehdr),
						hdr->f_opthdr, owp);
			if (ohdr == NULL) {
				dprintf(("coff_recognizer: failure to window optional header\n"));

				/* cleanup */
				ldr_unwindow(wp);
				rc = ldr_free(module);	/* should check rc */
				return LDR_ENOEXEC;
			}

			module->wpo = owp;
			module->ohd = (univ_t) ohdr;
			module->entry_pt = (ldr_entry_pt_t)COFF_ENTRYPT(hdr, ohdr);
		}			/* no optional header */
		else {
			module->ohd = (univ_t) NULL;
			module->entry_pt = NULL;
		}
			
		module->fd = fd;	
		module->wp = wp;
		module->fhd = (univ_t) hdr;
	}
	else {
		ldr_unwindow(wp);
		return LDR_ENOEXEC;
	}

	*mod = (univ_t)module;

	dprintf(("coff_recognizer: headers windowed: fh 0x%x fhs %d oh 0x%x ohs %d \n",
		 module->fhd, sizeof(struct filehdr), module->ohd, 
		 sizeof(struct aouthdr)));

	return LDR_SUCCESS;
}


/* Map the regions of the object file into the process' address space.
 * The callee allocates the list of mapped regions and its contents,
 * and will be responsible for freeing it.  The callee must fill in
 * these fields of the region descriptor record for each region mapped:
 *   - structure version number
 *   - region name (may be NULL if no region name is known)
 *   - region protection
 *   - the address it is to ultimately live at in the destination process'
 *     address space (vaddr)
 *   - the address it is currently mapped at in this process (mapaddr)
 *   - region size
 * 
 * absp is pointer to procedure to call to allocate (or at least get best
 * guess for) vaddr and mapaddr for an absolute region; relp for a
 * relocatable region.  Returns the number of regions in the region
 * list in *reg_count.  Return LDR_SUCCESS on success or negative error
 * status on error.
 */

static int 
coff_abs_map_regions(ldr_module_handle handle, ldr_region_allocs *allocps,
		     int *reg_count, ldr_region_rec **regions)
{
	int rc;				/* return code */
	struct filehdr *fhdr;		/* file header */
	struct aouthdr *ohdr;		/* optional header */
	ldr_region_rec *reg_list;	/* regions list */
	int nregions;			/* temp for region count */
	int regno;			/* temp for region count */
	long bss_start, bss_end;	/* start and end of bss */
	coff_module_handle_t module 	/* switching types across interface */
		= (coff_module_handle_t)handle;


	/* check for valid module */
	if (module == NULL) {
		return LDR_EINVAL;
	}

	/*
	 * get size of segement and address to map segment at
	 * from the optional header.
	 */
	fhdr = (struct filehdr *) module->fhd;
	ohdr = (struct aouthdr *) module->ohd;
	
	/* We only handle demand-paged executables here */

	if (ohdr->magic != ZMAGIC)
		return(LDR_ENOEXEC);

	/* get the region count; it will either be 2 or 3 depending on whether
	 * there is a BSS region */

	bss_start = COFF_BSSSTART(fhdr, ohdr);
	bss_end = COFF_BSSEND(fhdr, ohdr);

	/* if no bss, no of regions is one less than usual */
	nregions = ((bss_end <= bss_start) ? (MAX_COFF_ABS_REGIONS -1) :
		    MAX_COFF_ABS_REGIONS);

	dprintf(("coff_abs_map_regions: no of regions = %d\n", nregions));

	/* Allocate the region list */

	if ((rc = ldr_regions_create(nregions, LDR_REGION_VERSION, &reg_list)) != LDR_SUCCESS) {

		dprintf(("coff_abs_map_regions: region create error %d\n", rc));
		return(rc);
	}

	/* Now map the regions */

	for (regno = 0; regno < nregions; regno++) {

		if ((rc = internal_map_region(module->fd, fhdr, ohdr, allocps,
					      regno, &reg_list[regno])) != LDR_SUCCESS) {

			(void)internal_free_regions(regno - 1, reg_list, allocps);
			return(rc);
		}
	}

	*reg_count = nregions;
	*regions = reg_list;

	return(LDR_SUCCESS);
}


static int 
internal_map_region(ldr_file_t fd, struct filehdr *fhdr, struct aouthdr *ohdr,
		    ldr_region_allocs *allocps, int regno, ldr_region_rec *region)

/* Map the specified region of the object module with the specified headers,
 * using the specified allocation procedures.  Return LDR_SUCCESS on success
 * or negative error status on error.
 */
{
	int rc;				/* return code */
	long region_start;		/* map region to this address */
	long region_end;		/* end address of this region */
	size_t rsize;			/* size of this region */
	long data_start;		/* for special case checks */
	long data_end;			/* for special case checks */
	int prot;			/* protection attributes for region */
	char rname[8];			/* region name */
	int found;			/* true/false flag */
	int i;				/* loop variable */
	size_t pagesize;		/* VM page size */
	off_t off, noff;		/* offset into file to be mapped, new offset  */
	ldr_prot_t rprot;		/* region protection */
	univ_t baseaddr;		/* base address for mapping */
	univ_t mapaddr;			/* address it ended up mapped at */
	int mapflags;			/* flags for ldr_mmap */

	dprintf(("coff_abs_map_region: regno = %d\n",regno));
	switch(regno) {
		case TEXT_REGNO : 
			region_start = COFF_TXTSTART(fhdr, ohdr);
			region_end = COFF_TXTEND(fhdr, ohdr);

			/* Special case: if text overlaps part of data, map that
			 * piece of the text region as part of the data region
			 * instead.  Sigh...
			 */

			data_start = COFF_DATASTART(fhdr, ohdr);
			if (region_end > data_start)
				region_end = data_start;

			strcpy(rname, TEXT_REGION);
			prot = LDR_PROT_READ | LDR_PROT_EXEC;
			off = COFF_TXTOFF(fhdr, ohdr);
			rprot = LDR_R|LDR_X;
			mapflags = LDR_MAP_FILE;
			break;
		case DATA_REGNO :
			region_start = COFF_DATASTART(fhdr, ohdr);
			region_end = COFF_DATAEND(fhdr, ohdr);
			strcpy(rname, DATA_REGION);
			prot = LDR_PROT_READ | LDR_PROT_WRITE | LDR_PROT_EXEC;
			off = COFF_DATAOFF(fhdr, ohdr);
			rprot = LDR_R|LDR_W|LDR_X;
			mapflags = LDR_MAP_FILE;
			break;
		case BSS_REGNO :
			region_start = COFF_BSSSTART(fhdr, ohdr);
			region_end = COFF_BSSEND(fhdr, ohdr);

			/* Special case: if BSS overlaps part of data, map that
			 * piece of BSS as part of the data region instead.
			 * Sigh...
			 */

			data_end = COFF_DATAEND(fhdr, ohdr);
			if (data_end > region_start)
				region_start = data_end;

			strcpy(rname, BSS_REGION);   /* section named for consistency */
			prot = LDR_PROT_READ | LDR_PROT_WRITE | LDR_PROT_EXEC;
			off = 0;
			rprot = LDR_R|LDR_W|LDR_X;
			mapflags = LDR_MAP_ANON;
			fd = LDR_FILE_NONE;
			break;
		default:
			dprintf(("coff_abs_map_region: invalid region no %d in switch\n",regno));
			return LDR_EINVAL;
	}

	rsize = region_end - region_start;

	/* Special-case for old binaries where file offset is not page-aligned; tell
	 * mmap to bypass alignment restrictions.
	 */

	pagesize = ldr_getpagesize();	/* virtual memory page size */
	if (!aligned(off, pagesize))
		mapflags |= LDR_MAP_UNALIGNED;

	/* Now call the absolute allocator procedure to get the map address */

	if ((rc = (*(allocps->lra_abs_alloc))((univ_t)region_start, rsize,
					      rprot, &baseaddr))
	    != LDR_SUCCESS)
		return(rc);
	if (baseaddr == (univ_t)region_start)
		mapflags |= (LDR_MAP_FIXED | LDR_MAP_PRIVATE);
	else
		mapflags |= LDR_MAP_PRIVATE;

	/* bss section is mapped anon */
	if (regno == BSS_REGNO) {
		dprintf(("coff_abs_map_region: mapping %s at 0x%x anon size 0x%x\n",
			rname, region_start, rsize));
	}
	else {	/* map in text or data segment */
		dprintf(("coff_abs_map_region: mapping %s at start = 0x%x rsize = %d prot = 0x%x, fd = %d, off = %d\n",
			 rname, region_start, rsize, prot, fd, off));
	}

	/* Don't even try to map a zero-length region.  We'll still list it in
	 * the region list (it's too late to do anything else at this point),
	 * but we can't actually map it.
	 */

	if (rsize != 0) {

		rc = ldr_mmap(baseaddr, rsize, prot, mapflags, fd, off, &mapaddr);
		if (rc != 0) {
			dprintf(("coff_abs_map_region: failure to map %s segment rc = %d\n", rname, rc));
			return rc;
		}
	}

		
	/* Region mapped successfully; return all the information in the
	 * region record.
	 */

	region->lr_name = ldr_strdup(rname);
	region->lr_vaddr = (univ_t) region_start;
	region->lr_mapaddr = mapaddr;
	region->lr_size = rsize;
	region->lr_prot = prot;

	dprintf(("coff_abs_map_region: sucessfully mapped %s region\n",rname));
	return LDR_SUCCESS;
}



/* Return the address of the entry point of the specified module, if
 * any, in *entry_pt.  Return LDR_SUCCESS on success or negative
 * error status on error.
 */

int 
coff_get_entry_pt(ldr_module_handle mod, ldr_entry_pt_t *entry_pt)
{
	coff_module_handle_t module = (coff_module_handle_t)mod;
	struct filehdr *hdr;	/* file header */
	struct aouthdr *ohdr;	/* optional aout header */

	/* check valid module */
	if (module == NULL) {
		return LDR_EINVAL;
	}
	
	if (module->entry_pt == NULL)
		return LDR_ENOMAIN;

	*entry_pt = module->entry_pt;

	dprintf(("coff_get_entry_pt: ok entry pt = 0x%x\n", *entry_pt));

	return LDR_SUCCESS;
}

/* Complete the loading of the specified module, clean up open files,
 * temporary data structures, etc.  Return LDR_SUCCESS on success or
 * negative error status on error.
 */

static int 
coff_cleanup(ldr_module_handle mod)
{
	coff_module_handle_t module = (coff_module_handle_t)mod;
	int rc;

	/* check for valid module handle */
	if (module == NULL) {
		return LDR_EINVAL;
	}

	/* unwindow file header */
	if (module->fhd != NULL) {
		ldr_unwindow(module->wp);
		module->fhd = NULL;
	}

	/* if optional aout header exists, unwindow it */
	if (module->ohd != NULL)  {
		ldr_unwindow(module->wpo);
		module->ohd = NULL;
	}

	if ((rc = ldr_close(module->fd)) != 0) {
		return rc;
	}

	module->fd = LDR_FILE_NONE;	/* mark module handle file closed */

	/* throw away any other temporary data structures */

	dprintf(("coff_cleanup: cleanup done\n"));

	return LDR_SUCCESS;
}


static int
internal_free_regions(int nregions, ldr_region_rec *regions,
		      ldr_region_allocs *allocps)

/* Unmap the first nregions regions described by the specified list of
 * region records.  Don't give up if one unmap fails, but continue on
 * and unmap as many of the regions as possible.  Then free the
 * list of region records.  Note that nregions may be less than the
 * size of the region list, if we're being called as a result of a
 * map_regions error.
 */
{
	int		regno;
	int		rc, rrc;

	rc = LDR_SUCCESS;

	for (regno = 0; regno < nregions; regno++) {
		rrc = internal_unload_region(regno, &regions[regno], allocps);
		if (rc == LDR_SUCCESS) rc = rrc;
		if (regions[regno].lr_name != NULL)
			(void)ldr_free(regions[regno].lr_name);
	}

	rrc = ldr_regions_free(nregions, regions);
	if (rc == LDR_SUCCESS) rc = rrc;
	return(rc);
}


static int 
internal_unload_region(int regno, ldr_region_rec *region, ldr_region_allocs
		       *allocps)

/* Unload the specified region of the specified object module. Return 
 * LDR_SUCCESS on success or negative error status on error.
 */
{
	int rc;

	/* unmap region (but not if it's zero-length) */
	if (region->lr_size != 0) {
		if ((rc = ldr_munmap(region->lr_mapaddr, region->lr_size)) != 0) {
			dprintf(("coff_unload_region: failure to ldr_munmap region\n"));
			return rc;
		}
	}

	dprintf(("coff_unload_region: ok ldr_munmap done\n"));

	/* deallocate space on return */

	return((*(allocps->lra_dealloc))(region->lr_vaddr, region->lr_mapaddr,
					 region->lr_size));
}

/* Unload the specified module.  Unmap all regions, clean up open files,
 * temporary data structures, etc.  This should include deallocating 
 * the module's export list.  Once this routine is complete, the 
 * ldr_module_handle is no longer usable.  Return LDR_SUCCESS on success
 * or negative error status on error. NOTE: this routine is also used
 * to abort the loading of a module when a load error has occurred.
 */

static int 
coff_unload(ldr_module_handle handle, ldr_region_allocs *allocps,
	    int reg_count, ldr_region_rec *regions,
	    int ipkg_count, ldr_package_rec *import_pkgs,
	    int import_count, ldr_symbol_rec *imports,
	    int epkg_count, ldr_package_rec *export_pkgs)
{
	coff_module_handle_t module = (coff_module_handle_t)handle;
	int rc, rrc;

	/* check for valid module handle */
	if (module == NULL) {
		return LDR_EINVAL;
	}

	rc = LDR_SUCCESS;

	/* Unmap all regions */

	rrc = internal_free_regions(reg_count, regions, allocps);
	if (rc == LDR_SUCCESS) rc = rrc;

	/* deallocate module's import and export packages and lists */
	
	/* unwindow file header */
	if (module->fhd != NULL)
		ldr_unwindow(module->wp);

	/* if optional aout header exists, unwindow it */
	if (module->ohd != NULL) 
		ldr_unwindow(module->wpo);

	/* close file if not previously closed */
	if (module->fd != LDR_FILE_NONE) {
		if ((rrc = ldr_close(module->fd)) != 0) {
			dprintf(("coff_finish_unload: error closing file\n"));
		}
		if (rc == LDR_SUCCESS) rc = rrc;
		module->fd = LDR_FILE_NONE;	/* mark file closed */
	}

	(void)ldr_free(module);	/* invalid to use module handle after this */

	dprintf(("coff_finish_unload: ok handle disposed\n"));

	return rc;
}

/* Stub routines */

static int 
coff_get_static_dep(ldr_module_handle handle, int depno, char **dep)
{
	return(LDR_EAGAIN);
}

int coff_get_imports(ldr_module_handle handle, int *pkg_count,
		     ldr_package_rec **pkgs, int *sym_count,
		     ldr_symbol_rec **imports)
{
	dprintf(("coff_get_imports returning 0\n"));
	*pkg_count = *sym_count = 0;
	*pkgs = NULL;
	*imports = NULL;
	return(LDR_SUCCESS);
}

static int 
coff_get_export_pkgs(ldr_module_handle handle, int *count,
		     ldr_package_rec **packages)
{
	dprintf(("coff_get_export_pkgs returning 0\n"));
	*count = 0;
	*packages = NULL;
	return(LDR_SUCCESS);
}

int coff_get_exports(ldr_module_handle handle,int *sym_count,
		     ldr_symbol_rec **exports)
{
	dprintf(("coff_get_exports returning 0\n"));
	*sym_count = 0;
	*exports = NULL;
	return(LDR_SUCCESS);
}

static int 
coff_lookup_export(ldr_module_handle handle, ldr_package_rec *package,
		   ldr_symbol_rec *symbol)
{
	dprintf(("coff_lookup_export called on %s!\n", symbol->li_name));
	return(LDR_ENOSYM);
}

static int 
coff_relocate(ldr_module_handle handle, int nregions, ldr_region_rec regions[],
	      int npackages, ldr_package_rec packages[],
	      int nimports, ldr_symbol_rec imports[])
{
	dprintf(("coff_relocate called\n"));
	return(LDR_SUCCESS);
}

static int 
coff_run_inits(ldr_module_handle handle, entry_pt_kind kind)
{
	return(LDR_SUCCESS);
}
