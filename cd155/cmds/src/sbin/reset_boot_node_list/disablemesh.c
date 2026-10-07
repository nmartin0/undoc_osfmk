/*
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */
/*
 * disablemesh
 *
 * HISTORY:
 * $Log: disablemesh.c,v $
 * Revision 1.4.4.1  1995/06/11  18:42:22  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.4  1994/11/21  16:47:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/01/18  15:22:38  cfj
 * Merge R1_2 bug fix.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.2.2.1  1994/01/18  15:21:10  cfj
 * Use ROOT_FS_NODE instead of BOOT_FIRST_NODE.
 *
 *  Reviewer:shala
 *  Risk:L
 *  Benefit or PTS #:7830
 *  Testing:Got DEBUG_NODE working.
 *  Module(s):cmds_libs/src/sbin/reset_boot_node_list/disablemesh.c
 *
 * Revision 1.2  1993/09/16  17:10:59  stans
 *    Added Intel/SSD copyright notice and CVS 'Log' history.
 *
 */


#include <stdio.h>
#include <malloc.h>
#include "reset_bnl.h"
#include "bootmagic.h"

char	*extract_bootenv();
char	*progname;
int	whitespace = 0;			/* for whitespace in .._LIST values */
int	debug = 0;

/*
 * Global used by extract_bootenv() in magic.c
 */
char    *in_bootmagic;                  /* internal null-terminated format */

main(argc, argv, envp)
int	argc;
char	**argv;
char	**envp;
{
	char	*bootmagic_string;	/* External format bootmagic string */
	char	*boot_node_string;	/* Boot node list string */
	char	*bad_node_string;	/* Bad node list string */
	long	*boot_node_list;	/* List of boot nodes */
	int	boot_node_count;	/* Number of boot nodes */
	char	*p;
	int	i;

	progname = argv[0];

	/*
	 * Get boot magic from kernel and convert to internal representation
	 */
	bootmagic_string = (char *) malloc(BOOTMAGIC_MAX);
	if (bootmagic_string == NULL) {
		ERR("Cannot malloc external bootmagic string");
	}
	in_bootmagic = (char *) malloc(BOOTMAGIC_MAX);
	if (in_bootmagic == NULL) {
		ERR("Cannot malloc internal bootmagic string");
	}
	if (get_boot_magic(bootmagic_string) < 0) {
		ERR("Cannot get bootmagic from kernel");
	}
	convert_to_internal(bootmagic_string, in_bootmagic);

	if (p = extract_bootenv("ROOT_FS_NODE")) {
		boot_node_string = p;
	}
	else {
		ERR("No ROOT_FS_NODE in bootmagic");
	}

	boot_node_list = (long *) malloc(sizeof(long));
	if (boot_node_list == NULL) {
		ERR("Cannot malloc list of boot nodes");
	}

        /*
         * Initialize boot_node_list from boot_node_string
         */
        boot_node_count = parse_nodestring(boot_node_string, boot_node_list);
        if (boot_node_count <= 0) {
                ERR("Error parsing BOOT_NODE_LIST");
        }

	strcpy(boot_node_string, "BOOT_NODE_LIST=");
	node_number_to_node_string(boot_node_count, boot_node_list,
	                           boot_node_string + strlen(boot_node_string));
	
	reset_boot_node_list(boot_node_string,
	                     strlen(boot_node_string),
	                     0); /* globaly reset all nodes */
	exit(0);
}
	
