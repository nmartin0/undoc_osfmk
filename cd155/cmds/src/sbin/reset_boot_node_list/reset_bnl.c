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
 * reset_bnl - reset Boot Node List
 *
 *	Instruct the server to reset it's version of BOOT_NODE_LIST bootmagic.
 *	Nodes which failed to boot are removed from the list. The server will
 *	broadcast the new BNL to all servers.
 *
 * HISTORY:
 * $Log: reset_bnl.c,v $
 * Revision 1.6.4.1  1995/06/11  18:42:25  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.6  1994/11/21  16:47:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/10/26  18:29:17  stans
 *    Cleanup code: be less verbose about printing the entire revised bootnode
 *    list. It's sufficient to list just the nodes which did NOT boot.  Identify
 *    the pgm in printf()'s so the operator can understand which pgm is
 *    outputting console messages.
 *
 * Revision 1.4  1993/09/16  17:10:49  stans
 *    Added Intel/SSD copyright notice and CVS 'Log' history.
 *
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
	char	*disked_node_string;	/* Disked node list string */
	int	x_size, y_size;		/* X and Y size of mesh */
	int	max_nodes;		/* Maximum number of nodes in mesh */
	long	*boot_node_list;	/* List of boot nodes */
	int	boot_node_count;	/* Number of boot nodes */
	long	*bad_node_list;		/* List of bad nodes */
	int	bad_node_count;		/* Number of bad nodes */
	long	*disked_node_list;	/* List of nodes with disks */
	int	disked_node_count;	/* Number of nodes with disks */
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

	/*
	 * Get X and Y size of cube so we know how many nodes to allow
	 * for.
	 */
	if (p = extract_bootenv("BOOT_MESH_X")) {
		x_size = atoi(p);
	}
	else  {
		ERR("No BOOT_MESH_X in bootmagic");
	}

	if (p = extract_bootenv("BOOT_MESH_Y")) {
		y_size = atoi(p);
	}
	else {
		ERR("No BOOT_MESH_Y in bootmagic");
	}

	/*
	 * Get boot node list from bootmagic
	 */
	if (p = extract_bootenv("BOOT_NODE_LIST")) {
		boot_node_string = p;
	}
	else {
		ERR("No BOOT_NODE_LIST in bootmagic");
	}

	/*
	 * Allocate space for list of boot node numbers
	 */
	max_nodes = x_size * y_size;
	boot_node_list = (long *) malloc(max_nodes * sizeof(long));
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
	 
	/*
	 * Allocate space for list of disked nodes
	 */
	disked_node_list = (long *) malloc(boot_node_count * sizeof(long));
	if (disked_node_list == NULL) {
		ERR("Cannot malloc list of disked nodes");
	}

	/*
	 * Get list of nodes with disks from bootmagic
	 */
	if (p = extract_bootenv("BOOT_DISK_NODE_LIST")) {
		disked_node_string = p;
		/*
		 * Initialize disked_node_list from disked_node_string
		 */
		disked_node_count = parse_nodestring(disked_node_string,
		                                     disked_node_list); 
	}
	else {
		disked_node_count = 0;
	}
	 
	/*
	 * Allocate space for list of bad nodes
	 */
	bad_node_list = (long *) malloc(boot_node_count * sizeof(long));
	if (bad_node_list == NULL) {
		ERR("Cannot malloc list of bad nodes");
	}

	/*
	 * Initialize bad node list from bad nodes file
	 */
	bad_node_count = get_bad_nodes(boot_node_count, bad_node_list);

	/*
	 * Sort bad node list so that we can do a binary search in it
	 */
	sort(bad_node_count, bad_node_list);

	/*
	 * Mark as bad any nodes in boot node list that appear in bad node
	 * list
	 */
	for (i = 0; i < boot_node_count; i++) {
		if (search(bad_node_count, bad_node_list, boot_node_list[i])) {
			boot_node_list[i] = BAD_NODE;
		}
	}

	/*
	 * Check to see if any of the bad nodes have disks
	 */
	for (i = 0; i < disked_node_count; i++) {
		if (search(bad_node_count,bad_node_list,disked_node_list[i])) {
			printf("%s: Node %d with a filesystem on it has failed, can not recover.\n",
			       progname,disked_node_list[i]);
			exit(1);
		}
	}

	/*
	 * Build new boot node string from boot node list. Put
	 * BOOT_NODE_LIST= at beginning of string.
	 */
	strcpy(boot_node_string, "BOOT_NODE_LIST=");
	node_number_to_node_string(boot_node_count, boot_node_list,
	                           boot_node_string + strlen(boot_node_string));
	
	if (bad_node_count != 0) {
		register int	lineCount;
		/*
		 * Call server to reset the BOOT_NODE_LIST bootmagic string
		 */
		printf("%s: The following nodes failed to boot:\n  ",progname);
		for (i = 0,lineCount=0; i < bad_node_count - 1; i++) {
			printf(" %4d,", bad_node_list[i]);
			if ( ++lineCount >= 12 ) {
				printf("\n  ");
				lineCount=0;
			}
		}
		printf(" %4d\n", bad_node_list[i]);

#ifdef DEBUG /* be a little less verbose */
		printf("Calling server to reset %s\n",
		       boot_node_string);
#endif
		/*
		 * inform the server and micro-kernel about the new
		 * boot-node list.
		 */
		reset_boot_node_list(boot_node_string,
		                     strlen(boot_node_string),
		                     0); /* globaly reset all nodes */
	}

	exit(0);
}
	
