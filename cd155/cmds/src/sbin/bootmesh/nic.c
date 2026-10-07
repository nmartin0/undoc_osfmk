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
 
#include	"bootmesh.h"
#include	"nic.h"

/*
**  Low level routines to communicate with Network Memory Loader
**  Boot Packet protocol over Network Interconnection Chip (NIC)
**	Handles broadcast semantics.
*/

#define FILE_BUFSIZE	0x4000

/*
 * Boot Packet Interface
 */
bootbuf_t	bootbuf;
bootbuf_t	bootbuf_zero;	/* for sending zeros over wire */
mach_port_t	host = HOST_PORT;
int		flags = NIC_NO_BROADCAST;
extern long	sequence;

int corner_node = -1;

/*
 * Reset the Boot Loader on specified node
 */
boot_restart(nodes)
	nodetype	*nodes;
{
	long	control = NIC_BOOT_RESTART;
	int	err;

	sequence = -1;
	bootbuf.data_bytes = 0;
	bootbuf.data_addr = (double *)0;

#ifdef DEBUG
DBG2("boot_restart:\n");
#endif DEBUG
	err = boot_send_list(nodes,
			flags,
			control,
			bump_seq(sequence),
			&bootbuf,
			BOOTBUF_HDR_SZ);
	if (err < 0) 
		ERR("boot_send of reset packet fails");
	return(0);
}

/*
 * Read a kernel executable, and download it to node memory.
 * Use starting address in Coff header to determine location
 * in node memory.  Clear bss pages as specified in file header,
 * and download symbol table after bss pages.
 * Return address of kernel coff header, kernel entry point,
 * and address of page following end of symbol table.
 *
 */
nic_kernel_send(nodes, fp, kernel_addr, entry_point, symtab_end)
	nodetype 	 *nodes;
	FILE 	 *fp;
	unsigned *kernel_addr;
	unsigned *entry_point;
	unsigned *symtab_end;
{
	extern u_long	vm_text_start;	/* shared with download_kernel */
	extern u_long	vm_end;
	extern int	(*start_addr)();

	char	buf[FILE_BUFSIZE];
	int	first_pkt = 1;
	int	chunk;

#ifdef DEBUG
DBG2("nic_kernel_send:\n");
#endif DEBUG

	rewind(fp);
	for (;;) {
		chunk = fread(buf, 1, sizeof(buf), fp);
		if (chunk < 0) {
			perror("fread fails on kernel file read");
			return(-1);
		}
		if (chunk == 0)
			break;

		if (download_kernel(nodes, buf, chunk, first_pkt) < 0)
			return(-1);

		first_pkt = 0;
	}

	*kernel_addr = vm_text_start;
	*symtab_end = vm_end;
	*entry_point = (u_long)start_addr;

	return(0);
}


/*
 * Read in chunks of file, and transfer chunks to node.
 * Place in node memory at "addr", rounded to next page offset;
 * return actual address and number of bytes reserved in node memory
 * for the file.
 */
nic_file_send(nodes, fp, addr, start_addr, nbytes)
	nodetype 	 *nodes;
	FILE 	 *fp;
	unsigned addr;
	unsigned *start_addr;
	int 	 *nbytes;
{
	unsigned load_ptr = intel_round_page(addr);
	long control = NIC_BOOT_LOAD_BLK;
	int chunk;
	int err;

#ifdef DEBUG
DBG2("nic_file_send: addr = %08x \n",addr);
#endif DEBUG
	*start_addr = load_ptr;
	*nbytes = 0;
	rewind(fp);

	for (;;) {
		chunk = fread(bootbuf.data, 1, sizeof(bootbuf.data),
			fp);
		if (chunk < 0) {
			perror("fread fails on file read");
			return(-1);
		}
		if (chunk == 0)
			break;


		/* pad trailing bytes and round chunk to 64-bit boundary */
		if (chunk < round(chunk,8)) {
		    char *c;
		    for (c = &bootbuf.data[chunk];
			c < &bootbuf.data[round(chunk,8)]; c++)
			*c = 0;
		    chunk = round(chunk,8);
		}

		bootbuf.data_bytes = chunk;   /* actual bytes to node memory */
		bootbuf.data_addr = (double *)load_ptr;

		err = boot_send_list(nodes,
			flags,
			control,
			bump_seq(sequence),
			&bootbuf,
			chunk+BOOTBUF_HDR_SZ);
		if (err < 0) 
			ERR("boot_send of file fails");

		load_ptr += chunk;
		*nbytes += chunk;

	}
	return(0);
}

/*
 * Download nbytes of arbitrary buffer into node memory at specified address.
 * Assumes alignment of "addr" handled by caller.
 */
nic_buf_send(nodes, buf, nbytes, addr)
	nodetype 	 *nodes;
	char 	 *buf;
	int 	 nbytes;
	unsigned addr;
{
	unsigned load_ptr = addr;
	long control = NIC_BOOT_LOAD_BLK;
	int chunk;
	int err;

#ifdef DEBUG
DBG2("nic_buf_send: addr = %08x, nbytes = %d\n",addr,nbytes);
#endif DEBUG
	while (nbytes > 0) {
		chunk = (nbytes > sizeof(bootbuf.data))
			? sizeof(bootbuf.data): nbytes;

		bcopy(buf, bootbuf.data, chunk);

		/* pad trailing bytes and round chunk to 64-bit boundary */
		if (chunk < round(chunk,8)) {
		    char *c;
		    for (c = &bootbuf.data[chunk];
			c < &bootbuf.data[round(chunk,8)]; c++)
			*c = 0;
		    chunk = round(chunk,8);
#ifdef DEBUG
DBG2("nic_buf_send: padding to %d bytes\n",chunk);
#endif DEBUG
		}

		bootbuf.data_bytes = chunk;   /* actual bytes to node memory */
		bootbuf.data_addr = (double *)load_ptr;

		err = boot_send_list(nodes,
			flags,
			control,
			bump_seq(sequence),
			&bootbuf,
			chunk+BOOTBUF_HDR_SZ);
		if (err < 0) 
			ERR("boot_send of buffer fails");

		nbytes -= chunk;
		buf += chunk;
		load_ptr += chunk;

	}
	return(0);
}

/*
 * Download nbytes of zeros into node memory at specified address.
 * Assumes alignment of "addr" handled by caller.
 */
nic_zero_send(nodes, nbytes, addr)
	nodetype 	 *nodes;
	int 	 nbytes;
	unsigned addr;
{
	unsigned load_ptr = addr;
	long control = NIC_BOOT_LOAD_BLK;
	int chunk;
	int err;

#ifdef DEBUG
DBG2("nic_zero_send: addr = %08x, nbytes = %d\n",addr,nbytes);
#endif DEBUG
	while (nbytes > 0) {
		chunk = (nbytes > sizeof(bootbuf_zero.data))
			? sizeof(bootbuf_zero.data): nbytes;

		/* pad trailing bytes and round chunk to 64-bit boundary */
		chunk = round(chunk,8);

		bootbuf_zero.data_bytes = chunk;   /* actual bytes to node memory */
		bootbuf_zero.data_addr = (double *)load_ptr;

		err = boot_send_list(nodes,
			flags,
			control,
			bump_seq(sequence),
			&bootbuf_zero,
			chunk+BOOTBUF_HDR_SZ);
		if (err < 0) 
			ERR("boot_send of buffer fails");

		nbytes -= chunk;
		load_ptr += chunk;
	}
	return(0);
}

/*
 * Request NIC Memory Loader to branch to "addr", passing up to
 * MAX_GOTO_ARGS arguments to kernel on node.
 */
nic_goto(nodes, addr, args)
	nodetype		*nodes;
	unsigned 	addr;
	kernel_args_t	*args;
{
	long control = NIC_BOOT_GO_ADDR;
	int err;
#ifdef DEBUG
DBG2("nic_goto: addr = %08x\n",addr);
#endif DEBUG

	if (sizeof(kernel_args_t) > MAX_GOTO_ARG_SZ)
		return(-1);

	bootbuf.data_bytes = sizeof(kernel_args_t);
	bootbuf.data_addr = (double *)addr;
	bcopy((char *)args, bootbuf.data, sizeof(kernel_args_t));

	err = boot_send_list(nodes,
		flags,
		control,
		bump_seq(sequence),
		&bootbuf,
		sizeof(kernel_args_t)+BOOTBUF_HDR_SZ);
	if (err < 0) 
		ERR("boot_send of goto packet fails");
	return(0);
}


/*
 * Send boot message to all operational nodes on list.
 */
#ifdef todo
/* implement broadcast from arbitrary node */
#endif todo
boot_send_list(nodes, flags, control, seq, buf, sz)
	nodetype *nodes;
	int flags;
	int control;
	int seq;
	char *buf;
	long sz;
{
	nodetype *np;
	int err = 0;
	nodetype *lastone;

	if (skip_packet_send)
		return(0);
	if (broadcast_enabled) {
#ifdef DEBUG
DBG2("boot_send_list: broadcasting to corner_node %d\n", corner_node);
#endif DEBUG
		if (corner_node < 0)
			return(-1);
		flags |= NIC_BROADCAST;
		err = _boot_send(host,
			corner_node,
			flags,
			control,
			seq,
			(long *)buf,
			sz);
		return(err);
	}

	for (np = nodes; np != NULL; np = np->n_next) {
		if (!np->n_operational)
			continue;
#ifndef TEST
#ifdef DEBUG
DBG2("boot_send_list: sending to node %d\n",np->n_id);
#endif DEBUG
#endif TEST
		err = _boot_send(host,
			np->n_id,
			flags,
			control,
			seq,
			(long *)buf,
			sz);
		if (err < 0)
			break;
	}
	return(err);
}

/*
 * For now, use extremely simplistic algorithm.
 * Allow broadcast only if bootnode is in a corner.
 * Identify broadcast region by node in corner opposite bootnode.
 * Ignore non-operational nodes in the mesh; assume their NICs
 * can pass messages through.
 */
setup_broadcast()
{
	if (!broadcast_enabled)
		return(0);

	/* See if the boot node is in the Upper left corner */
	if (boot_first_node == 0)
		corner_node = boot_mesh_x * boot_mesh_y - 1;

	/* See if the boot node is in the Upper Right Corner */
	else if (boot_first_node == (boot_mesh_x - 1))	
		corner_node = boot_mesh_x * boot_mesh_y - boot_mesh_x;

	/* See if its in the Lower Right Corner */
	else if (boot_first_node == (boot_mesh_x * boot_mesh_y - 1))
		corner_node = 0;

	/* See if its in the Lower Left Corner */
	else if (boot_first_node ==
		(boot_mesh_x * boot_mesh_y - boot_mesh_x))
		    corner_node = boot_mesh_x - 1;

	/* The boot node is not in a corner of the physical mesh. 
	   Assume the system was constructed so that the boot node
	   is in the upper right hand corner of the mesh. Make the
	   node in the lower left the corner node.
	*/
	else {
		corner_node = boot_mesh_x * boot_mesh_y - boot_mesh_x;
		
	}
#ifdef OBSOLETE
	/* The boot node is not in one of the corners */
	else {
		/* can't use broadcast yet */
		VERBOSE("Bootnode not in corner, no broadcast\n");
		broadcast_enabled = 0;
	}
#endif
	return(0);
}
