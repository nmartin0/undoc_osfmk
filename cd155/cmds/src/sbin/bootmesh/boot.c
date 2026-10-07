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
 
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "bootmesh.h"
#include "nic.h"

nodetype  	*meshlist;		/* array of maxnodes nodes */
int		maxnodes;
				/* how many times we called boot_nodes() */
static int	boot_nodes_iteration=0;
				/* whether to broadcast kernel start command */
static int	broadcast_kernel_goto = 1;

FILE*	kfp;				/* file containing kernel */
FILE* 	sfp;				/* file containing server */
FILE* 	efp;				/* file containing emulator */

kernel_args_t 	mkargs;			/* zeroed args for meshnode's kernel */

static	char	kernel_checksum[1000];
#define	CHECKSUM	"BOOT_MK_TEXT_CHECK"	/* Bootmagic checksum name */
static	char *checksum = CHECKSUM;
static	char *phase_name = "";
extern	char *extract_bootenv();
extern	int  has_compute_nodelist;
extern	int  has_sunmos_nodelist;
extern	int     control_flags;

int compute_node_list();

/*
 *  Driver to boot all nodes in mesh:
 *	Resets Network Memory Loader on each node.
 *  	Downloads kernel, server and emulator files to each node.
 *	Forces each node to branch to kernel entry point and begin execution.
 *  Accesses nodes serially unless broadcast is enabled.
 */
boot_nodes()
{
	kernel_args_t 	tmpargs;	/* temps for skip control */
	unsigned 	kernel_addr;	/* beginning of kernel coff in memory */
	unsigned 	start_addr;	/* kernel entry point */
	nodetype	*np, *npp;
	char 		s[1024];
	int		size;
	int 		bcast;
	int 		common_length, offset;

#ifdef DEBUG
DBG("boot_nodes\n");
#endif DEBUG
	boot_nodes_iteration++;	/* keep track of how many times called */

	if (open_executables() < 0)
		ERR("bad file specifications");

	/* hardwired kernel arguments */
	mkargs.rd_addr = mkargs.rd_sz = 0;

	/*
	 * for each mesh node:
	 *	reset boot loader 
	 * 	read and send each file; return addr and size of file sent
	 *	send incore bootmagic
	 * 	prepare microkernel arguments 
	 *	start microkernel execution 
	 */

	/* Restart the boot loader on the nodes.  If we are not broadcasting,
  	   then call boot_restart() each time boot_nodes() is called because
	   it restarts only the nodes in the current node list.  If
	   we are broadcasting, then call boot_restart() only once.
	*/
	skip_packet_send = control(SKIP_RESET);
	if ((boot_nodes_iteration == 1) || (!broadcast_enabled))
		if (boot_restart(meshlist) < 0)
			ERR("cannot reset bootloader");

	/*
	 * download kernel, creating a runtime memory image
	 * using info in file header; return starting 
	 * address and address at end of symbol table;
	 */
	skip_packet_send = control(SKIP_KERNEL);
	if (!skip_packet_send) {
	    PROGRESS("Downloading%s:   kernel  ", phase_name);
	    VERBOSE("%s\n", boot_kernel);
	}
	if (nic_kernel_send(meshlist,
		kfp,
		&kernel_addr,
		&start_addr,
		&tmpargs.symtab_end) < 0)
			ERR("cannot download kernel");

	/*
	 * put server file at end of kernel symtab;
	 * return actual address (rounded to page boundary as
	 * necessary) and size of file in bytes;
	 * note that no relocation of Coff sections is done;
	 */
	skip_packet_send = control(SKIP_SERVER);
	if (!skip_packet_send) {
	    VERBOSE("Downloading%s:   ", phase_name);
	    PROGRESS("server  ");
	    VERBOSE("%s\n", boot_server);
	}
	if (nic_file_send(meshlist,
		sfp,
		tmpargs.symtab_end,
		&tmpargs.svr_addr,
		&tmpargs.svr_sz) < 0)
			ERR("cannot download server");

	/*
	 * put emulator file at end of server data;
	 * return actual address (rounded to page boundary as
	 * necessary) and size of file in bytes;
	 * note that no relocation of Coff sections is done
	 */
	skip_packet_send = control(SKIP_EMULATOR);
	if (!skip_packet_send) {
	    VERBOSE("Downloading%s: ", phase_name);
	    PROGRESS("emulator  ");
	    VERBOSE("%s\n", boot_emulator);
	}

	if (nic_file_send(meshlist,
		efp,
		(tmpargs.svr_addr+tmpargs.svr_sz),
		&tmpargs.emul_addr,
		&tmpargs.emul_sz) < 0)
			ERR("cannot download emulator");

	/*
	 * put bootmagic in hardwired memory location
	 * in front of kernel, page aligned;
	 * because each node has a unique physical node number,
	 * download each bootmagic serially (disable broadcast)
	 */

	skip_packet_send = control(SKIP_MAGIC);
	if (!skip_packet_send) {
	    VERBOSE("Downloading%s: ", phase_name);
	    PROGRESS("bootmagic  ");
	    VERBOSE("\n");
	}


	/* determine common size */
	/* note: The Caller must do: delete_bootenv("BOOT_MY_NODE")
	 *       prior to calling boot_nodes()
	 */

	bcast = broadcast_enabled;

	sprintf(s, "BOOT_TIME=%u",time(NULL));
	if (replace_bootenv(s) < 0)
		ERR("cannot reset BOOT_TIME in bootmagic");

	convert_to_external(in_bootmagic,bootmagic);

	common_length = strlen(bootmagic);

	/* set node id and convert to external format */


	sprintf(s, "BOOT_MY_NODE=%04d", 9999);
	if (replace_bootenv(s) < 0)
		ERR("cannot reset BOOT_MY_NODE in bootmagic");

	convert_to_external(in_bootmagic,bootmagic);

	size = strlen(bootmagic)+1;

#ifdef DEBUG
	VERBOSE("common bootmagic length = %d  bootmagic size = %d\n", common_length, size);
#endif DEBUG

#ifdef BUMPER
	/*
	 * if we've been broadcasting the non-bootmagic stuff,
	 * always send bootmagic as even multiples of 16 bytes;
	 * no need to clear bytes, since bootmagic is null-terminated
	 */
	if (bcast) {
		if (size % 16)
			size = round(size,16);
		if ((size / 16) % 2) 
			size += 16;
		if (size > BOOTMAGIC_MAX)
		    ERR("buffer overflow trying to round bootmagic");
	}
#endif BUMPER
		
	tmpargs.bm_addr = intel_trunc_page(kernel_addr - size);

#ifdef DEBUG
	DBG("bootmagic: size %% 64 = %d  size - common_length = %d\n", (size % 64), (size - common_length));
#endif DEBUG

	if (bcast && size > 1024 && debug < 2) {
		/* use combination of broadcast and non-bcast */

		/* determine size of piece to broadcast */
		offset = (common_length / 64) * 64;

		if (!skip_packet_send) 
			VERBOSE("Downloading common bootmagic data (%d bytes) via broadcast.\n", offset);

#ifdef DEBUG
		DBG("&bootmagic = 0x%08x\nDBG offset = %d\nDBG tmpargs.bm_addr = 0x%08x\n", bootmagic, offset, tmpargs.bm_addr);
#endif DEBUG

		if (nic_buf_send(meshlist,
			bootmagic,
			offset,
			tmpargs.bm_addr) < 0)
				ERR("cannot download bootmagic");

	    if (!skip_packet_send) 
		VERBOSE("Downloading individual bootmagic data (%d bytes) to node:\n", size - offset);
	}
	else {	/* use non-broadcast only */
	    offset = 0;
	    if (!skip_packet_send) {
		VERBOSE("Downloading bootmagic (%d bytes) to node:\n", size);
	    }		    
	}

#ifdef DEBUG
	DBG("&bootmagic + offset = 0x%08x\nDBG size - offset = %d\n", tmpargs.bm_addr + offset = 0x%08x\n", bootmagic + offset, size - offset, tmpargs.bm_addr + offset);
#endif DEBUG

	bcast = broadcast_enabled;
	broadcast_enabled = 0;

	for (np = meshlist; np != NULL; np = np->n_next) {

		if (!np->n_operational)
			continue;

		/* set node id and convert to external format */
		sprintf(s, "BOOT_MY_NODE=%04d", np->n_id);
		if (replace_bootenv(s) < 0)
			ERR("cannot reset BOOT_MY_NODE in bootmagic");

		convert_to_external(in_bootmagic,bootmagic);

	
/*************  We don't want to see hundreds of bootmagic strings
#ifdef DEBUG
DBG("boot_nodes: external env... size %d\n",strlen(bootmagic));
DBG(bootmagic);
#endif
*********************/
		size = strlen(bootmagic)+1; /* send terminator */
#ifdef BUMPER
		/*
		 * if we've been broadcasting the non-bootmagic stuff,
		 * always send bootmagic as even multiples of 16 bytes;
		 * no need to clear bytes, since bootmagic is null-terminated
		 */
		if (bcast) {
			if (size % 16)
				size = round(size,16);
			if ((size / 16) % 2) 
				size += 16;
			if (size > BOOTMAGIC_MAX)
			    ERR("buffer overflow trying to round bootmagic");
		}
#endif BUMPER
		
		if (!skip_packet_send) 
		    if (debug)
			VERBOSE("Downloading bootmagic to node %d... %s\n", np->n_id, s);
		    else
			VERBOSE("%6d  ", np->n_id);

		/* treat it like a list of length one */
		npp = np->n_next;
		np->n_next = NULL;
		if (nic_buf_send(np,
			bootmagic + offset,
			size - offset,
			tmpargs.bm_addr + offset) < 0)
				ERR("cannot download bootmagic");
		np->n_next = npp;
	}
	if (!skip_packet_send && !debug) 
		VERBOSE("\n");

	broadcast_enabled = bcast;


	if (!skip_packet_send) {
	    PROGRESS("done.\n");
	}

	/* 
	 * initialize mkargs associated with unskipped steps so that
	 * the microkernel notices when the associated data is
	 * not in memory by finding zeros
	 */
	if (!control(SKIP_KERNEL)) {
		mkargs.symtab_end = tmpargs.symtab_end;;
	}
	if (!control(SKIP_SERVER)) {
		mkargs.svr_addr = tmpargs.svr_addr;
		mkargs.svr_sz = tmpargs.svr_sz;
	}
	if (!control(SKIP_EMULATOR)) {
		mkargs.emul_addr = tmpargs.emul_addr;
		mkargs.emul_sz = tmpargs.emul_sz;
	}
	if (!control(SKIP_MAGIC)) {
		mkargs.bm_addr = tmpargs.bm_addr;
	}

	/*
	 * branch into kernel, passing in prepared args
	 */
	skip_packet_send = control(SKIP_GOTO);
	if (!skip_packet_send) {
		PROGRESS("Startup    %s:   send_start ", phase_name);
		VERBOSE("\n");
	}

#ifdef SLOWDOWN
	/*
	 * Slowdown rate at which servers initialize and flodd
	 * mesh with ipc.  Serialize commands to start kernel
	 * execution and pause between each command.
	 */
	if (slowdown) {
#ifdef DEBUG
DBG("boot_nodes: slowdown %d secs between gotos\n",slowdown_secs);
#endif DEBUG
		bcast = broadcast_enabled;
		broadcast_enabled = 0;

		for (np = meshlist; np != NULL; np = np->n_next) {
			if (!np->n_operational)
				continue;
			if (!skip_packet_send)
				VERBOSE("Starting node %d...\n",np->n_id);
			npp = np->n_next;
			np->n_next = NULL;
			if (nic_goto(np, start_addr, &mkargs) < 0)
			    	ERR("cannot start kernel execution");
			usleep(slowdown_secs);
			np->n_next = npp;
		}
		broadcast_enabled = bcast;
	}
	else
#endif SLOWDOWN

	{
		bcast = broadcast_enabled;
		broadcast_enabled &= broadcast_kernel_goto;	
	
		if (nic_goto(meshlist, start_addr, &mkargs) < 0)
		    ERR("cannot start kernel execution");
	
		broadcast_enabled = bcast;	
	}


	VERBOSE("Kernel start addr = 0x%08x\n",start_addr);
	VERBOSE("Kernel Arguments:\n");
	VERBOSE("\tsymtab_end      = 0x%08x\n", mkargs.symtab_end);
	VERBOSE("\tramdisk addr    = 0x%08x\n",mkargs.rd_addr);
	VERBOSE("\tramdisk size    = 0x%08x (%04d)\n",
		mkargs.rd_sz, mkargs.rd_sz);
	VERBOSE("\tbootmagic addr  = 0x%08x\n",mkargs.bm_addr);
	VERBOSE("\tserver addr     = 0x%08x\n",mkargs.svr_addr);
	VERBOSE("\tserver size     = 0x%08x (%04d)\n",
		mkargs.svr_sz, mkargs.svr_sz);
	VERBOSE("\temulator addr   = 0x%08x\n",mkargs.emul_addr);
	VERBOSE("\temulator size   = 0x%08x (%04d)\n",
		mkargs.emul_sz, mkargs.emul_sz);
	
	return(0);
}

/*
 * determine all meshnodes in root partition
 */
compute_node_list(list)
char *list;
{
	struct node *np;
	int i;

#ifdef DEBUG
DBG("compute_node_list\n");
#endif DEBUG
        if (debug_node == (-1))
	{
		/* validate mesh configuration info */
		if (boot_first_node != my_physical_node())  {
#ifdef DEBUG
DBG("compute_node_list: first node %d != my node %d\n",
		boot_first_node,my_physical_node());
#endif DEBUG
			return(-1);
		}
	}

	/* override root partition computation */
	if (arg_node != -1) {
#ifdef DEBUG
DBG("booting single node %d\n",arg_node);
#endif DEBUG
#ifndef TEST
		if (boot_first_node == arg_node) 
			ERR("cannot meshboot first node");
#endif TEST
		np = (nodetype *)calloc(1, sizeof(nodetype));
		if (np == NULL) {
			sprintf(err,"calloc fails\n");
			ERR(err);
		}
		np->n_id = arg_node;
		np->n_operational = 1;	/* patch in here */
		np->n_next = (struct node *)NULL;
		np->n_attrs = (char*)0;
		meshlist = np;
		maxnodes = 1;
		return(0);
	}
	
	maxnodes = boot_mesh_x * boot_mesh_y;
	if (maxnodes <= 0)
		return(-1);

	meshlist = (nodetype *)calloc(maxnodes, sizeof(nodetype));
	if (meshlist == NULL) {
		sprintf(err,"calloc fails\n");
		ERR(err);
	}	

	for (i = 0; i < maxnodes; i++) {
		meshlist[i].n_id = i;
		meshlist[i].n_next = &meshlist[i+1];
		meshlist[i].n_attrs = (char*)0;
	}
	meshlist[maxnodes-1].n_next = NULL;
	setup_broadcast();
	return(operational_node_list(meshlist, list));
}

/*
 * use boot_node_list to determine which nodes in mesh are operational
 */
operational_node_list(meshlist,oplist)
	nodetype *meshlist;	 	/* list of all nodes except bootnode */
	char	*oplist;		/* list of operational nodes */
{
	char	*list, *dot, *tok;
	int	startnode, endnode, i; 

#ifdef DEBUG
DBG("convert_node_list: oplist %s\n",oplist);
#endif DEBUG

	list = strdup(oplist);
	tok = strtok(list,",");
	while (tok) {
#ifdef DEBUG
DBG("checking token %s\n",tok);
#endif DEBUG
		/* check for .. */
		if (dot = strstr(tok,"..")) {
			*dot = NULL;
			startnode = atoi(tok);
			endnode = atoi(dot+2);
			if ((startnode > endnode) || (endnode > (maxnodes-1))) {
				sprintf(err,
					"out of bounds %d..%d in nodelist\n",
					startnode,endnode);
				ERR(err);
			}
#ifdef DEBUG
DBG("start range %d, end range %d\n",startnode,endnode);
#endif DEBUG
			for (i = startnode; i <= endnode; i++)
				meshlist[i].n_operational = 1;
		} else if (strstr(tok,".")) {
			sprintf(err,"error in %s\n",tok);
			ERR(err);
		} else {
#ifdef DEBUG
DBG("node %d\n",atoi(tok));
#endif DEBUG
			meshlist[atoi(tok)].n_operational = 1;
		}

		/* get next token */
		tok = strtok(NULL,",");
	}
	/* skip operations to bootnode */
	meshlist[boot_first_node].n_operational = 0;
#ifdef DEBUG
	DBG("Operational nodes are...\n");
	for (i = 0; i < maxnodes; i++)
		if (meshlist[i].n_operational) DBG("%d\n",i);

#endif DEBUG
	return(0);
}


/* 
 * open/validate system files to be downloaded to meshnodes
 */
open_executables()
{
#ifdef DEBUG
DBG("open_executables\n");
DBG("boot_kernel\t%s\n", boot_kernel);
DBG("boot_server\t%s\n", boot_server);
DBG("boot_emulator\t%s\n", boot_emulator);
#endif DEBUG

	kfp = fopen(boot_kernel, "r");
	if (kfp == NULL) {
		perror("kernel file");
		return(-1);
	}
	sfp = fopen(boot_server, "r");
	if (debug_node == (-1))
        {
		if (sfp == NULL) {
			perror("server file");
			return(-1);
		}
	}
	efp = fopen(boot_emulator, "r");
	if (debug_node == (-1))
        {
		if (efp == NULL) {
			perror("emulator file");
			return(-1);
		}
	}
	return(0);
}

/* Are there any operational nodes in a meshlist?
 */
int
any_nodes(list)
nodetype *list;
{
	int	i;

	for (i = 0; i < maxnodes; i++) {
		if (list[i].n_operational){
			return (1);		/* Found one */
		}
	}
	return (0);			/* There are none */
}


/* Boot separate kernels on compute nodes, service and I/O nodes,
 * and ALT nodes
*/
int
boot_multiple_kernels()
{
	nodetype	*service_meshlist, *io_meshlist, *compute_meshlist,
			*sunmos_meshlist;
	int		i, j;
	char		*p;
	FILE*		fd;

	/*************** Some day we may need the kernel checksum
	p = extract_bootenv(checksum);
	strcpy(kernel_checksum, checksum);
	strcat(kernel_checksum, "=");	
	strcat(kernel_checksum, p);	
	************************************************************/

	/* Delete checksum from bootmagic.  In case of multiple
	   kernels, we know the checksum won't match.  A more thorough
	   approach would be to send a correct checksum to each node
	   for its corresponding kernel.  
	*/
	delete_bootenv(checksum);	/* get rid of it */

	/* Make mesh lists for the various node lists. 
	   A side effect of compute_node_list is that meshlist
	   points to the thing we want.
	*/
	if (compute_node_list(boot_compute_nodelist) < 0)
		ERR("boot_compute_nodelist is no good");
	compute_meshlist = meshlist;

	/* If there are service nodes, add them to the list of I/O nodes
	*/
	if (compute_node_list(boot_service_nodelist) < 0)
		ERR("boot_service_nodelist is no good");
	service_meshlist = meshlist;

	if (compute_node_list(boot_io_nodelist) < 0)
		ERR("boot_io_nodelist is no good");
	for (i = 0; i < maxnodes; i++) {
		if (service_meshlist[i].n_operational)
			meshlist[i].n_operational = 1;
	}
	io_meshlist = meshlist;

	/* Subtract service nodes from compute_meshlist */
	for (i = 0; i < maxnodes; i++) {
		if (service_meshlist[i].n_operational)
			compute_meshlist[i].n_operational = 0;
	}

	if (has_sunmos_nodelist)
	{
		if (compute_node_list(boot_sunmos_nodelist) < 0)
			ERR("boot_sunmos_nodelist is no good");
		sunmos_meshlist = meshlist;
	}

	/* Boot the I/O nodes and service nodes */

	/* If there will be subsequent downloads, then don't broadcast
	   the 'goto'
	*/
	if (has_compute_nodelist || has_sunmos_nodelist)
		broadcast_kernel_goto = 0;
	else
		broadcast_kernel_goto = 1;
	meshlist = io_meshlist;
	if (any_nodes(meshlist)) {
		phase_name = " Service & I/O";
		if (boot_nodes() < 0)
			ERR("cannot boot io nodes");
		if (!control(SKIP_POLLING)) {
		    PROGRESS("svr_wait ");
		}
		if (poll_nodes(io_meshlist) < 0)
			ERR("Cannot handshake with io nodes");
	
		fclose(kfp);
		fclose(sfp);
		fclose(efp);
	}


	/* If there are compute nodes, boot them*/
	if (has_compute_nodelist && any_nodes(compute_meshlist))
	{
		delete_bootenv("BOOT_MY_NODE");
		if (has_sunmos_nodelist)
			broadcast_kernel_goto = 0;
		else
			broadcast_kernel_goto = 1;

		/* Boot the compute nodes */
		boot_kernel = boot_compute_kernel;
		boot_server = boot_compute_server;
		meshlist = compute_meshlist;
		phase_name = " Compute";
		if (boot_nodes() < 0)
			ERR("cannot boot compute nodes");
		if (!control(SKIP_POLLING)) {
		    PROGRESS("svr_wait ");
		}
		if (poll_nodes(compute_meshlist) < 0)
			ERR("Cannot handshake with compute nodes");

		fclose(kfp);
		fclose(sfp);
		fclose(efp);
	}

	/* If there are sunmos nodes, boot them*/
	if (has_sunmos_nodelist)
	{
		delete_bootenv("BOOT_MY_NODE");
		broadcast_kernel_goto = 1;
		boot_kernel = boot_sunmos_kernel;
		meshlist = sunmos_meshlist;
		phase_name = " Guest OS";
		control_flags |= SKIP_SERVER;
		control_flags |= SKIP_EMULATOR;
		control_flags |= SKIP_POLLING;
		if (boot_nodes() < 0)
			ERR("cannot boot sunmos nodes");
		if (!control(SKIP_GOTO))
		    PROGRESS("startup complete.\n");
	}

	/* Now accumulate the attribute strings and write to the file */

	io_meshlist[my_physical_node()].n_attrs = (char*) malloc(1024);

	nx_create_node_attributes(task_by_pid(-1),
	  io_meshlist[my_physical_node()].n_attrs, 1024);
	strcat(io_meshlist[my_physical_node()].n_attrs, ",bootnode");

	fd = fopen(ATTR_FILE, "w");
	if (!fd)
		ERR("Cannot open attributes files");

	for (i=0; i<maxnodes; i++) {
		nodetype*	cp = compute_meshlist+i;
		nodetype*	ip = io_meshlist+i;
		nodetype*	sp = sunmos_meshlist+i;
		char*		attr;

		if (has_compute_nodelist && cp->n_attrs)
			attr = cp->n_attrs;
		else if (ip && ip->n_attrs)
			attr = ip->n_attrs;
		else if (has_sunmos_nodelist && sp->n_operational)
			attr = "guest_os";
		else
			attr = "";
		
		fprintf(fd, "%s\n", attr);
#ifdef DEBUG
		VERBOSE("node %4d attribute: \"%s\"\n", i, attr);
#endif DEBUG
	}

	fclose(fd);


	return(0);
}
