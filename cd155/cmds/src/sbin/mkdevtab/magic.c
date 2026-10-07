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
 
#include	<stdio.h>
#include	<malloc.h>
#include	"bootmagic.h"

extern int debug;

/*
** Routines to manage internal and external
** representation of bootmagic.
*/

void 	convert_to_internal();
void 	in_to_ex();
char	*extract_bootenv();
int	replace_bootenv();

/*
 * buffers for entire bootmagic file
 */
char	*bootmagic;			/* newline-terminated format */
char	*in_bootmagic;			/* internal null-terminated format */

/* configurable bootmagic values */
int	boot_mesh_x;
int	boot_mesh_y;
int	boot_first_node;
char	*boot_node_list = NULL;
char	*bootmagicfile = NULL;
char	*boot_kernel = NULL;
char	*boot_emulator = NULL;
char	*boot_server = NULL;

/*
 * Fetch and process bootmagic.
 */
get_magic(bootmagicfile)
	char *bootmagicfile;
{
	FILE *f;
	char *p;
	int n;

#ifdef DEBUG
if (debug) printf("Entering get_magic: bootmagicfile %s\n", bootmagicfile);
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
			printf("WARNING: bytes in %s may be exceeding internal max %d\n",
				bootmagicfile,BOOTMAGIC_MAX); 
		}
		fclose(f);
#ifdef DEBUG
if (debug) printf("get_magic: read in %d bytes of %s\n",n,bootmagicfile);
#endif DEBUG
		
	} else {
#ifdef DEBUG
if (debug) printf("get_magic: getting bootmagic from kernel\n");
#endif DEBUG
		/* fetch via system call */
		if (get_boot_magic(bootmagic) < 0)
			ERR("cannot get bootmagic from kernel");
	}
#ifdef DEBUG
if (debug==2) print_bootenv(bootmagic);
#endif DEBUG
	convert_to_internal(bootmagic,in_bootmagic);
#ifdef DEBUG
if (debug) printf("get_magic: converted to internal format \n");
if (debug==2) print_bootenv(in_bootmagic);
#endif DEBUG

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
if(debug) printf("convert_to_internal: size = %d\n",size);
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
if(debug) printf("convert_to_external: \n");
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
if (debug) printf("extract_bootenv: name = %s \n",name);
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
if (debug) printf("replace_bootenv: %s\n", new);
if (debug==2) print_bootenv(in_bootmagic);
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
if (debug) printf("replace_bootenv: new internal env...\n");
if (debug==2) print_bootenv(in_bootmagic);
#endif
        return 0;
}

print_bootenv(s)
	char * s;
{
	char *p;
        for (p = s; *p; p += strlen(p)+1) 
		printf("%s\n",p);
}
