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
/* tload1.c
 * Just call the loader
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <stdio.h>
#include <loader.h>

#include "ldr_types.h"
#include "ldr_lock.h"

void *ldr_process_context;
extern int ldr_bootstrap(char *, void **);

/* structure to save locking functions used by loader */

lib_lock_functions_t	ldr_lock_funcs;

/* This is the only loader lock for this process */

ldr_lock_t ldr_global_lock;

const char *ldr_global_data_file = "/tmp/ldr_global.dat";
const char *ldr_dyn_database = "/tmp/ldr_dyn_mgr.conf";


main(argc, argv)

int argc;
char **argv;
{
	int	i;
	ldr_module_t	mod;
	ldr_entry_pt_t	entry;
	int	rc;

	if (argc < 2) {
usage:
		fprintf(stderr, "usage: %s file ...\n", argv[0]);
		exit(1);
	}

	if ((rc = ldr_bootstrap(argv[0], &ldr_process_context)) < 0) {
		fprintf(stderr, "ldr_bootstrap failed %d\n", rc);
		exit(1);
	}

	for (i = 1; i < argc; i++) {

		mod = load(argv[i], 0);
		if (mod == 0) {
			perror("load failed");
			fprintf(stderr, "file %s\n", argv[i]);
		} else {
			printf("load succeeded, file %s, module %d\n", argv[i], mod);
		}
		entry = ldr_entry(mod);
		if (entry == NULL) {
			perror("ldr_entry failed");
			fprintf(stderr, "file %s\n", argv[i]);
		} else {
			printf("ldr_entry for file %s is %x\n", argv[i], entry);
		}
	}

	exit(0);
}

int kls_client_ipc_connect_to_server() { return(-1); }
int kls_client_ipc_disconnect_from_server() { return(-1); }
int kls_client_load() { return(-1); }
int kls_client_unload() { return(-1); }
int kls_client_entry() { return(-1); }
int kls_client_lookup() { return(-1); }
int kls_client_lookup_package() { return(-1); }
int kls_client_next_module() { return(-1); }
int kls_client_inq_module() { return(-1); }
int kls_client_inq_region() { return(-1); }
