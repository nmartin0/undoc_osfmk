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
**
** Global bootmesh variables
**
*/

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*
 * generic control variables
 */
extern	char 	*progname;
extern	int	verbose;
extern	int	one_detect_off;
extern	int	broadcast_enabled;
extern	int 	debug;
extern	int 	warn;
extern	int	arg_node;
extern	int	arg_timeout;
extern	int	poll_all;
extern	int 	progress;
#ifdef SLOWDOWN
#define 	SLOWDOWN_SECS	15
extern	int	slowdown;
extern	int	slowdown_secs;
#endif SLOWDOWN

/* Bootmagic indicates whether there is more than one kernel */
extern int	multiple_kernels;

/* Load states */
#define LD_TEXT_DATA 1
#define LD_SYMS 2
#define LD_FINISHED 3


/* control stages of downloading across mesh */
#define	SKIP_RESET	0x1
#define	SKIP_KERNEL	0x2
#define	SKIP_SERVER	0x4
#define	SKIP_EMULATOR	0x8
#define	SKIP_MAGIC	0x10
#define	SKIP_GOTO	0x20
#define	SKIP_POLLING	0x40
extern 	int	control_flags;
#define control(x)	(control_flags & (x))

int 	skip_packet_send;

/*
 * buffers for bootmagic file
 */
#define	BOOTMAGIC_MAX	8192
extern	char 	*bootmagicfile;
extern	char 	*bootmagic;		/* external newline-terminated format */
extern	char 	*in_bootmagic;		/* internal null-terminated format */

/*
 * bootmagic values of interest while mesh booting
 */
extern	int	boot_mesh_x;
extern	int	boot_mesh_y;
extern	int	boot_first_node;
extern  int     debug_node;

extern	char 	*boot_node_list;
extern	char 	*boot_server;
extern	char 	*boot_emulator;
extern	char 	*boot_kernel;

/* For booting multiple kernels */
extern	char 	*boot_compute_kernel;
extern	char 	*boot_io_kernel;
extern	char 	*boot_sunmos_kernel;
extern	char 	*boot_compute_server;
extern	char 	*boot_io_server;
extern	char 	*boot_compute_nodelist;
extern	char 	*boot_io_nodelist;
extern	char 	*boot_sunmos_nodelist;
extern  char    *boot_service_nodelist;

/***  Note: These values are hard coded.  If the path of the .partinfo
 *    file changes, or if its internal configuration changes, then
 *    bootmesh will not be able to find which nodes are the service nodes
 */
#define PARTINFO_FILE_NAME      "/etc/nx/service/.partinfo"
/* This is the line of the .partinfo file that gives the list of
 *service nodes
 */
#define SERVICE_LIST_LINE       11      



/*
 * argument list for Mach microkernel
 */
struct kernelargs {
    unsigned 	symtab_end;
    unsigned	rd_addr;
    int		rd_sz;
    unsigned	bm_addr;
    unsigned	svr_addr;
    int		svr_sz;
    unsigned	emul_addr;
    int		emul_sz;
};
typedef struct kernelargs kernel_args_t;


/*
 * node descriptor
 */
struct node {				/* describe a mesh node */
	int	n_id;
        int     debug_node;             /* Is this the debug node? */
	int	n_operational;
	int	n_acked;
	struct node *n_next;		/* nic code needs a linked list */
	char*	n_attrs;		/* node attributes */
};
typedef	struct node nodetype;
extern	nodetype	*meshlist;
extern  int		maxnodes;


/*
 * miscellaneous macros
 */
#define my_physical_node()	_myphysnode()
#define round(x,y)		((((u_int)(x)) + (y) - 1) & ~((y)-1))

#define INTEL_PGBYTES   4096    /* bytes per page */
#define intel_trunc_page(x)     (((u_int)(x)) & ~(INTEL_PGBYTES-1))
#define intel_round_page(x)     ((((u_int)(x)) + INTEL_PGBYTES - 1) & \
					~(INTEL_PGBYTES-1))

char err[1024];			/* error message buffer */
#define ERR(str) \
	{ \
	fprintf(stderr,"%s: Error: %s\n",progname,str); \
	return(-1); \
	}

#define WARN(str) \
	{ \
	if (warn) \
	fprintf(stderr,"%s: Warning: %s\n",progname,str); \
	}

#define DBG 		if (debug) printf
#define DBG2 		if (debug > 1) printf


#define	ATTR_FILE	"/etc/nx/nodeinfo"

