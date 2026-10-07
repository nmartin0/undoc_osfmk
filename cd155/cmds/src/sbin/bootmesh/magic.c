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
 
#include	<malloc.h>
#include	<sys/stat.h>
#include	"bootmesh.h"

/*
** Routines to manage internal and external
** representation of bootmagic.
*/

void 	convert_to_internal();
void 	in_to_ex();
char	*extract_bootenv();
int	replace_bootenv();
char	*get_boot_service_nodelist();

/*
 * buffers for entire bootmagic file
 */
char	*bootmagic;			/* newline-terminated format */
char	*in_bootmagic;			/* internal null-terminated format */

/* configurable bootmagic values */
int	boot_mesh_x;
int	boot_mesh_y;
int	boot_first_node;
int     debug_node=(-1);
char	*boot_node_list = NULL;
char	*bootmagicfile = NULL;
char	*boot_kernel = NULL;
char	*boot_emulator = NULL;
char	*boot_server = NULL;

/* Optional bootmagic values for multiple kernels/servers.
*/
char    *boot_compute_kernel = NULL;
char    *boot_compute_server = NULL;
char    *boot_compute_nodelist = NULL;
char    *boot_io_nodelist = NULL;
char    *boot_service_nodelist = NULL;
int	has_compute_nodelist = 0;

char    *boot_sunmos_kernel = NULL;
char    *boot_sunmos_nodelist = NULL;
int	has_sunmos_nodelist = 0;

int      multiple_kernels = 0; 	/* If set, there are multiple kernels */

/*
 * Fetch and process bootmagic.
 */
get_magic(bootmagicfile)
	char *bootmagicfile;
{
	FILE *f;
	char *p;
	int n, rv;

#ifdef DEBUG
DBG("Entering get_magic: bootmagicfile %s\n", bootmagicfile);
#endif DEBUG

	/* get buffer space */
	bootmagic = (char *)malloc(BOOTMAGIC_MAX);
	if (bootmagic == NULL)
		ERR("cannot malloc external bootmagic buffer");
	in_bootmagic = (char *)malloc(BOOTMAGIC_MAX);
	if (in_bootmagic == NULL)
		ERR("cannot malloc internal bootmagic buffer");

	/* get bootmagic */
	if (bootmagicfile) {

		/* open file, get newline-terminated strings */
		f = fopen(bootmagicfile, "r");
		if (f == NULL) {
			perror("bootmagic file");
			ERR("cannot open bootmagic file");
		}
		n = fread((char *)bootmagic, 1,
			(int)BOOTMAGIC_MAX, f);
		if (n <= 0) {
			perror("bootmagic file");
			ERR("cannot read bootmagic file");
		}
		if (n == BOOTMAGIC_MAX) {
			sprintf(err,
			"bytes in %s may be exceeding internal max %d\n",
				bootmagicfile,BOOTMAGIC_MAX); 
			WARN(err);
		}
		fclose(f);
#ifdef DEBUG
DBG("get_magic: read in %d bytes of %s\n",n,bootmagicfile);
#endif DEBUG
		
	} else {
#ifdef DEBUG
DBG("get_magic: getting bootmagic from kernel\n");
#endif DEBUG
		/* fetch via system call */
		if (get_boot_magic(bootmagic) < 0)
			ERR("cannot get bootmagic from kernel");
	}
#ifdef DEBUG
if (debug>1) print_bootenv(bootmagic);
#endif DEBUG
	convert_to_internal(bootmagic,in_bootmagic);
#ifdef DEBUG
DBG("get_magic: converted to internal format \n");
if (debug>1) print_bootenv(in_bootmagic);
#endif DEBUG

	/* find nodelist, mesh dimensions in bootenv */
	if (p = extract_bootenv("BOOT_MESH_X")) 
		boot_mesh_x = atoi(p);
	else 
		ERR("no boot_mesh_x");

	if (p = extract_bootenv("BOOT_MESH_Y"))
		boot_mesh_y = atoi(p);
	else
		ERR("no boot_mesh_y");

	if (p = extract_bootenv("BOOT_FIRST_NODE"))
		boot_first_node = atoi(p);
	else
		ERR("no boot_first_node");

	if (p = extract_bootenv("BOOT_NODE_LIST"))
		boot_node_list = p;

	/* find pathnames to system executables */

	if (boot_emulator == NULL)
	    if (p = extract_bootenv("BOOT_EMULATOR_NAME"))
		    boot_emulator = p;
	    else
		    ERR("no boot_emulator_name");
	
	if (boot_server == NULL)
	    if (p = extract_bootenv("BOOT_STARTUP_NAME"))
		    boot_server = p;
	    else
		    ERR("no boot_startup_name");

	if (boot_kernel == NULL)
	    if (p = extract_bootenv("BOOT_KERNEL_NAME"))
		    boot_kernel = p;
	    else
		    ERR("no boot_kernel_name");

	/* In case of separate kernels/servers on compute nodes,
	   bootmagic strings tell the node lists for each partition, and
	   names of files to boot.  If these are present, they override
	   BOOT_NODE_LIST.  In that case, multiple_kernels is set,
	   and boot_multiple_kernels() will be called.

	   These names are required:
		BOOT_KERNEL_NAME
		BOOT_STARTUP_NAME
		BOOT_EMULATOR_NAME

	   These are optional, but if any are present, then they must all be:
		BOOT_COMPUTE_NODE_LIST
		BOOT_IO_NODE_LIST
		BOOT_COMPUTE_STARTUP_NAME
	
	   This is optional, defaults to BOOT_KERNEL_NAME:
		BOOT_COMPUTE_KERNEL_NAME
	*/
	

	/* The two nodelists are:
		BOOT_COMPUTE_NODE_LIST = compute nodes
		BOOT_IO_NODE_LIST = everything else, including boot
		  node, I/O nodes, etc.

	   In case the user specifies a node to boot, we don't 
	   boot multiple nodes.
	*/
	if ((arg_node == -1) &&
			(p = extract_bootenv("BOOT_COMPUTE_NODE_LIST"))) {
		boot_compute_nodelist = p;
		multiple_kernels++;
		if (*boot_compute_nodelist != (char)NULL)
			has_compute_nodelist = 1;

		if (p = extract_bootenv("BOOT_IO_NODE_LIST")) 
			boot_io_nodelist = p;
		else
			ERR("no boot_io_nodelist");
	
		if (boot_compute_server == NULL)
		    if (p = extract_bootenv("BOOT_COMPUTE_STARTUP_NAME"))
			    boot_compute_server = p;
		    else
			    ERR("no boot_compute_startup_name");
	
		if (boot_compute_kernel == NULL)
		    if (p = extract_bootenv("BOOT_COMPUTE_KERNEL_NAME"))
			    boot_compute_kernel = p;
		    else
			    boot_compute_kernel = boot_kernel;

		boot_service_nodelist =  get_boot_service_nodelist();

		if (!one_detect_off) {
		    rv = can_boot_one();
		    if (rv < 0)
			return rv;
		    if (rv)
			multiple_kernels--;
		}
	}

	if ((arg_node == -1) &&
			(p = extract_bootenv("BOOT_ALT_NODE_LIST"))) {
		boot_sunmos_nodelist = p;
		multiple_kernels++;
		if (*boot_sunmos_nodelist != (char)NULL)
			has_sunmos_nodelist = 1;

		if (boot_sunmos_kernel == NULL)
		    if (p = extract_bootenv("BOOT_ALT_KERNEL_NAME"))
			    boot_sunmos_kernel = p;
		    else
			    ERR("no boot_sunmos_kernel_name");
	}

	if (p = extract_bootenv("BOOT_MK_VERBOSE")) 
		if (atoi(p) == 1)
			progress++;

	VERBOSE("Bootmagic variables:\n");
	VERBOSE("\tmesh_x     = %d\n",boot_mesh_x);
	VERBOSE("\tmesh_y     = %d\n",boot_mesh_y);
	VERBOSE("\tfirst_node = %d\n",boot_first_node);
	VERBOSE("\tnode_list  = %s\n",boot_node_list);
	VERBOSE("\tkernel     = %s\n",boot_kernel);
	VERBOSE("\tserver     = %s\n",boot_server);
	VERBOSE("\temulator   = %s\n",boot_emulator);
	VERBOSE("\tcompute_node_list  = %s\n",boot_compute_nodelist);
	VERBOSE("\tsunmos_node_list  = %s\n",boot_sunmos_nodelist);
	VERBOSE("\tio_node_list  = %s\n",boot_io_nodelist);
	VERBOSE("\tcompute_kernel     = %s\n",boot_compute_kernel);
	VERBOSE("\tcompute_server     = %s\n",boot_compute_server);
	VERBOSE("\tsunmos_kernel     = %s\n",boot_sunmos_kernel);


	/* We've pointed a bunch of global pointers at strings within
	   the internal bootmagic.  Now we'll make a duplicate copy.
	   The first copy will not be touched, so all those strings will
	   be valid when we refer to them later.  The new copy, to
	   which the pointer 'in_bootmagic' points, will get chewed on
	   by boot_nodes(), which modifies the bootmagic for each node,
	   before downloading the bootmagic to the node.  Each node gets
	   its own node number and the current time.
	*/

	in_bootmagic = (char *)malloc(BOOTMAGIC_MAX);
	if (in_bootmagic == NULL)
		ERR("cannot malloc second internal bootmagic buffer");
	convert_to_internal(bootmagic,in_bootmagic);

	/*
         * Look for the bootmagic string 'DEBUG_NODE'. If it's set, then
         * assume this system is being setup as a debugging system.
         */
        if ((p = extract_bootenv("DEBUG_NODE")) == NULL) return(0);
        debug_node = atoi(p);
        verbose=1;              /* Since we're debugging... */
        VERBOSE("\n-------- System being setup for Debugging ------\n");
        VERBOSE("Debug Node = %d\n",debug_node);
	return(0);
}


/*
 * Generate internal bootmagic format:
 * copy bytes from ex to in, converting newlines to nulls
 */
void
convert_to_internal(ex, in)
	char *ex, *in;
{
	int i;
	int size = strlen(ex);
#ifdef DEBUG
DBG("convert_to_internal: size = %d\n",size);
#endif DEBUG
	for (i = 0; i < size; i++) {
		*in++ = *ex++;
		if (*(ex-1) == '\n')
			*(in-1) = NULL;
	}
	in[size] = NULL;
}

/*
 * Generate external bootmagic format from internal format:
 * copy in to ex, converting nulls to newlines;
 * terminate ex with null to create a single string
 */
void
convert_to_external(in, ex)
	char *in, *ex;
{
#ifdef DEBUG
DBG("convert_to_external: \n");
#endif DEBUG
        while (*in) {
                strcpy(ex, in);
		in += strlen(in)+1;
		ex += strlen(ex);
                *ex++ = '\n';
        }
	*ex = *in;
}

/*
 * Obtain the value of a given parameter in the
 * internal bootmagic strings. 
 */
char *
extract_bootenv(name)
        char    *name;
{
        int     n;
        char    *p;

#ifdef DEBUG
DBG("extract_bootenv: name = %s \n",name);
#endif DEBUG
        for (n = 0, p = name; *p && (*p != '='); p++, n++);
        for (p = in_bootmagic; *p; p += strlen(p)+1) {
	    if (strncmp(p, name, n) == 0 &&
		p[n] == '=') {
		    return &p[n+1];
	    }
        }
        return NULL;
}

/*
 * Add or replace a parameter in the internal bootmagic strings.
 * The form of _new_ is "BOOT_PARAMETER=value".
 */
replace_bootenv(new)
        char    *new;
{
        int     n;
        char    *p;
        char    *q;

#ifdef DEBUG
DBG("replace_bootenv: %s\n", new);
if (debug>1) print_bootenv(in_bootmagic);
#endif
        for (n = 0, p = new; *p && *p != '='; p++, n++);
        if (*p != '=')
                return -1;
        for (p = in_bootmagic; *p; p += strlen(p)+1) {
                if (strncmp(p, new, n) == 0 &&
                    p[n] == '=') {
                        for (q = p + strlen(p)+1; *q; ) {
                                *p++ = *q++;
                                if (*q == 0) {
                                        *p++ = *q++;
                                }
                        }
                        break;
                }
        }
        q = new;
        while (*q) {
                *p++ = *q++;
        }
        *p = 0;
	if (*(++p))
		*p = 0;
#ifdef DEBUG
DBG("replace_bootenv: new internal env...\n");
if (debug>1) print_bootenv(in_bootmagic);
#endif
        return 0;
}

/* Get the list of service nodes from the .partinfo file, if the file exists
*/
char *
get_boot_service_nodelist()
{
	char	*buf;
	int	i;
	FILE	*fp;


	fp =  fopen(PARTINFO_FILE_NAME, "r");
	if (fp == NULL)
	{
#ifdef DEBUG
DBG("get_boot_service_nodelist: can't open .partinfo file");
#endif DEBUG
		return (char *)NULL;
	}

	buf  = (char *)malloc(BOOTMAGIC_MAX);

	/* Read lines of the file until the line we want */
	for(i=0; i<SERVICE_LIST_LINE; i++)
	{
		fgets(buf, BOOTMAGIC_MAX, fp);
		if (*buf == '\0')
		{
#ifdef DEBUG
DBG("get_boot_service_nodelist: hit end of .partinfo file");
#endif DEBUG
			free ((void *)buf);
			fclose(fp);
			return (char *)NULL;
		}
	}

	/* It would be nice to do some error checking, to see if
	   what we have here is really a node list */
	fclose(fp);
#ifdef DEBUG
DBG("get_boot_service_nodelist: returning %s", buf);
#endif DEBUG
	return buf;
}


/*
 * Delete a string from the bootmagic.  To actually delete it is overkill,
 * we'll just deface the name so the kernel won't recognize it.
 */
int
delete_bootenv(name)
        char    *name;
{
        int      n;
        char    *p;

#ifdef DEBUG
DBG("delete_bootenv: name = %s \n",name);
#endif DEBUG
        for (n = 0, p = name; *p && (*p != '='); p++, n++);
        for (p = in_bootmagic; *p; p += strlen(p)+1) {
	    if (strncmp(p, name, n) == 0 &&
		p[n] == '=') {
			*p = '?';
	    }
        }
        return NULL;
}

#ifdef DEBUG
print_bootenv(s)
	char * s;
{
	char *p;
        for (p = s; *p; p += strlen(p)+1) 
		printf("%s\n",p);
}
#endif DEBUG


int
can_boot_one()
{
    struct stat	boot_stat, compute_stat;

    /*
     * For Kernel and Server
     *
     * if a boot_compute_FILE is specified
     *    and boot_FILE != boot_compute_FILE
     * then
     *	  if the boot_FILE's inode != boot_compute_FILE's inode
     *	      we can't boot just one kernel & server
     *    endif
     * endif
     */
    if (boot_compute_kernel != NULL	
	&& strcmp(boot_kernel, boot_compute_kernel) != 0) {

	/* test for files having different inodes */
	if (stat(boot_kernel, &boot_stat) < 0) {
	    fprintf(stderr,"%s: Error: Can't stat \"%s\"\n",
			progname, boot_kernel);
	    return -1;
	}
	if (stat(boot_compute_kernel, &compute_stat) < 0) {
	    fprintf(stderr,"%s: Error: Can't stat \"%s\"\n",
			progname, boot_compute_kernel);
	    return -1;
	}
	if (boot_stat.st_ino != compute_stat.st_ino) {
	    VERBOSE("%s is not the same inode as %s\n", boot_kernel, boot_compute_kernel);
	    return 0;
	}
    }

    if (boot_compute_server != NULL	
	&& strcmp(boot_server, boot_compute_server) != 0) {
	/* test for files having different inodes */
	if (stat(boot_server, &boot_stat) < 0) {
	    fprintf(stderr,"%s: Error: Can't stat \"%s\"\n",
			progname, boot_server);
	    return -1;
	}
	if (stat(boot_compute_server, &compute_stat) < 0) {
	    fprintf(stderr,"%s: Error: Can't stat \"%s\"\n",
			progname, boot_compute_server);
	    return -1;
	}
	if (boot_stat.st_ino != compute_stat.st_ino) {
	    VERBOSE("%s is not the same inode as %s\n", boot_server, boot_compute_server);
	    return 0;
	}
    }

    VERBOSE("Compute and Service & I/O Kernels and Servers are identical\n");
    return 1;
}

