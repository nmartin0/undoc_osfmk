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
 * main.c
 *
 * loader main(), receives control from exec_with_loader(2) and crt0
 *
 * OSF/1 Release 1.0.1
 */

/* #define	HELLO_GOODBYE	1 */

#include <sys/types.h>
#include <sys/auxv.h>
#include <sys/ldr_exec.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>

#include "ldr_types.h"
#include "ldr_errno.h"
#include "ldr_auxv.h"
#include "ldr_lock.h"

#ifdef	HELLO_GOODBYE
#include <string.h>
#include <fcntl.h>
#endif

/* structure to save locking functions used by loader */

lib_lock_functions_t	ldr_lock_funcs;

/* This is the only loader lock for this process */

ldr_lock_t ldr_global_lock;

/* This process' loader context */

ldr_context_t	ldr_process_context;

/* Definition of standard loader data file */

const char *ldr_global_data_file = LDR_GLOBAL_DATA_FILE;
const char *ldr_dyn_database = LDR_DYN_DATABASE;

extern int _ldr_crt0_request;
static int *_lcrp = &_ldr_crt0_request;
int _ldr_present = 1;

#if	defined(mips) || defined(i386)
extern void _ldr_jump();
void (*_ldr_jump_func)() = _ldr_jump;
#endif

static void main_set_program_attributes(ldr_module_t);
extern void ldr_abort(void);
extern void ldr_bpt(void);
extern int errno;
static int ldr_initialized = 0;

ldr_entry_pt_t
main()
{
	ldr_entry_pt_t		entry_pt;
	ldr_module_t		module;
	int			rc;
	char *			loader_file;
	char *			file;
	int			flags;

	if (ldr_initialized++) {

		/* re-entered -- bomb out now */

		ldr_msg("loader main: loader main re-entered, exiting\n");
		ldr_abort();
	}

	ldr_auxv_init();
#ifdef	HELLO_GOODBYE
	main_hello();
#endif
	if ((rc = ldr_auxv_get_exec_loader_filename(&loader_file)) != LDR_SUCCESS) {
		ldr_msg("loader main: ldr_auxv_get_exec_loader_filename(), status: %E\n%B\n",
			rc);
		ldr_abort();
	}
	if ((rc = ldr_bootstrap(loader_file, &ldr_process_context)) != LDR_SUCCESS) {
		ldr_msg("loader main: ldr_bootstrap() failed, status: %E\n%B\n",
			rc);
		ldr_abort();
	}
	if ((rc = ldr_auxv_get_exec_filename(&file)) != LDR_SUCCESS) {
		ldr_msg("loader main: ldr_auxv_get_exec_filename() failed, status: %E\n%B\n",
			rc);
		ldr_abort();
	}
	if ((rc = ldr_context_load(ldr_process_context, file, LDR_MAIN,
				   &module)) != LDR_SUCCESS) {
		ldr_msg("loader main: load(\"%s\", LDR_MAIN) failed, status: %E\n%B\n",
			file, rc);
		ldr_abort();
	}
	main_set_program_attributes(module);
	if ((rc = ldr_context_get_entry_pt(ldr_process_context, module,
					   &entry_pt)) != LDR_SUCCESS) {
		ldr_msg("loader main: ldr_entry() failed, status: %E\n%B\n",
			rc);
		ldr_abort();
	}
#ifdef	HELLO_GOODBYE
	main_goodbye();
#endif
	if ((rc = ldr_auxv_get_exec_loader_flags(&flags)) != LDR_SUCCESS) {
		ldr_msg("loader main: ldr_auxv_get_exec_loader_flags() failed, status: %E\n%B\n",
			rc);
		ldr_abort();
	}
	if (flags & LDR_EXEC_PTRACE_F)
		ldr_bpt();
	return(entry_pt);
}

void
main_set_program_attributes(module)
	ldr_module_t		module;
{
	ldr_module_info_t	ldr_module_info, *lmip;
	ldr_region_info_t	ldr_region_info, *lrip;
	ldr_region_t		region;
	int			text_set = 0;
	int			text_start = 0;
	int			text_end = 0;
	int			data_set = 0;
	int			data_start = 0;
	int			data_end = 0;
	int			text_size, data_size;
	int			start, end;
	size_t			ret_size;
	int			rc;

	lmip = &ldr_module_info;
	if ((rc = ldr_context_inq_module(ldr_process_context, module,
					 lmip, sizeof(ldr_module_info),
					 &ret_size)) != LDR_SUCCESS) {
		ldr_msg("loader main: ldr_inq_module() failed, status: %E\n%B\n",
			rc);
		ldr_abort();
	}
	lrip = &ldr_region_info;
	for (region = 0; region < ldr_module_info.lmi_nregion; region++) {
		if ((rc = ldr_context_inq_region(ldr_process_context,
						 module, region, lrip,
						 sizeof(ldr_region_info),
						 &ret_size)) != LDR_SUCCESS) {
			ldr_msg("loader main: ldr_inq_region() failed, status: %E\n%B\n",
				rc);
			ldr_abort();
		}

		start = (int)lrip->lri_vaddr;
		end = start + (int)lrip->lri_size;

		if (lrip->lri_prot & LDR_W) {
			/* data or bss region */
			if (data_set) {
				if (start < data_start)
					data_start = start;
				if (end > data_end)
					data_end = end;
			} else {
				data_set = 1;
				data_start = start;
				data_end = end;
			}
		} else {
			/* text region */
			if (text_set) {
				if (start < text_start)
					text_start = start;
				if (end > text_end)
					text_end = end;
			} else {
				text_set = 1;
				text_start = start;
				text_end = end;
			}
		}
	}

	text_size = text_end - text_start;
	data_size = data_end - data_start;

	if (set_program_attributes(text_start, text_size, data_start,
	    data_size) == -1) {
		ldr_msg("loader main: set_program_attributes() failed with errno = %d\n",
			errno);
		ldr_abort();
	}

	/* Set the loader's idea of the current break */

	if ((rc = ldr_brk((univ_t)data_end)) != LDR_SUCCESS) {
		ldr_msg("loader main: unable to initialize break, %E\n", rc);
		ldr_abort();
	}
}

#ifdef	HELLO_GOODBYE

#define	STRING_SIZE	(10*1024)

main_hello()
{
	char hello[STRING_SIZE];
	char *loader, *file;

	hello[0] = '\0';
	if (ldr_auxv_get_exec_loader_filename(&loader) != LDR_SUCCESS)
		return;
	if (loader == (char *)-1)
		(void)strcat(hello, "loading");
	else {
		(void)strcat(hello, loader);
		(void)strcat(hello, ": loading");
	}
	if (ldr_auxv_get_exec_filename(&file) != LDR_SUCCESS)
		return;
	if (file == (char *)-1)
		(void)strcat(hello, " ...");
	else {
		(void)strcat(hello, " ");
		(void)strcat(hello, file);
		(void)strcat(hello, " ...");
	}
	ldr_msg(hello);
}

main_goodbye()
{
	char *goodbye;

	goodbye = " done\n";
	ldr_msg(goodbye);
}
#endif	/* HELLO_GOODBYE */


void
exit(int status)
{
	_exit(status);
}
