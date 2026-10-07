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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/getmagic/magic.c,v 1.5 1994/11/21 16:41:12 mtm Exp $
 *
 *
 */

#include	<stdio.h>
#include	<malloc.h>
#include	"bootmagic.h"

/*
** Routines to manage internal and external
** representation of bootmagic.
*/

void 	convert_to_internal();
void 	internal_to_whitespace();
void 	in_to_ex();
char	*extract_bootenv();
int	replace_bootenv();

/*
 * buffers for entire bootmagic file
 */
char	*bootmagic;			/* newline-terminated format */
char	*in_bootmagic;			/* internal null-terminated format */

char    *whitespace_buf;	        /* buffer for whitespace .._LIST */

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
	if (whitespace)			/* get a whitespace buffer */
	{
		whitespace_buf = (char *) malloc(BOOTMAGIC_MAX);
		if (whitespace_buf == NULL)
		{
			ERR("cannot malloc internal whitespace buffer");
		}
		memset((char *)whitespace_buf, 0, BOOTMAGIC_MAX);
	}

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

	/*
	 * Converted to internal format.  If we want whitespace separated
	 * values for the .._LIST values, it gets done next.
	 */

	if (whitespace)
	{
		*whitespace_buf=0;
		(void) internal_to_whitespace(in_bootmagic, whitespace_buf);
	}
	
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
	in[size + 1] = NULL;		/* just to make sure! */
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

	if (whitespace)
	{
		p = whitespace_buf;

	}
	else
	{
		p = in_bootmagic;
	}

        for (; *p; p += strlen(p)+1) {
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

	if (whitespace)
	{
		p = whitespace_buf;

	}
	else
	{
		p = in_bootmagic;
	}

        for (; *p; p += strlen(p)+1) {
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

/*
 * internal_to_whitespace
 *
 * Take the internal format where the .._LIST values are of the form
 *  a,b,<m..n>, etc. and turn them into a full list separated by spaces.
 * Of course, this means the numbers between "m" and "n" must be created.
 */

void
internal_to_whitespace(in, wht)
	char *in, *wht;
{
	int first_number, last_number, i; /* used for temporaries */
	char *cp;			  /* used for temporary */

	while (*in != NULL)		/* done when we have this */
	{

		/*
		 * The if statement processes statements with the target
		 * token of _LIST=.  The else statement merely copies
		 * the string to the whitespace buffer.
		 */

		if (strstr(in, "_LIST=")) /* look for list token */
		{

			/*
			 * Point to the character after the equal sign, copy
			 * everything up to the equal sign to the new buffer
			 * and update the output pointer.  Just in case we have
			 * a null string, copy the character after the equal
			 * equal sign (either null or real number).
			 */
			
			cp = (char *) strstr(in, "="); /* returns -> to char */
			cp++;
			strncpy(wht, in, cp - in + 1); /* one extra */
			wht += (cp - in);
			
			/*
			 * Process numbers until the end of the line.  First
			 * convert the number to integer.  Output characters
			 * until a ',', '.' or NULL is encountered.  If it's
			 * a NULL, put it in the output buffer and to to
			 * the top of the loop for exit, otherwise, put a space
			 * character out.
			 */

			if (*cp == 0)	*wht++ = *cp;
			while (*cp != NULL)
			{
				first_number = atoi(cp);
				while ((*cp != NULL) &&
				       (*cp != ',') && (*cp != '.'))
				{
					*wht++ = *cp++;
				}
				if (*cp == NULL)
				{
					*wht++ = *cp;
					continue;
				}
				else
				{
					*wht++ = ' ';
					cp++;
				}

				/*
				 * If this was a <m..n> item, we are now
				 * pointing to the second period.  Bump to
				 * the next value and convert it to integer.
				 * fill in the numbers inbetween and fall out
				 * to output the last number.
				 */

				if (*cp == '.')
				{
					cp++;
					first_number++;	  /* already output */
					last_number = atoi(cp);

					for (;
					     first_number < last_number;
					     first_number++)
					{
						i = sprintf(wht, "%d",
							    first_number);
						wht += i;
						*wht++ = ' ';  /* space */
					}
				}
			}
		}
		else
		{
			strcpy(wht, in);         /* copy to output */
			wht += (strlen(in) + 1); /* bump output pointer */
		}
		in += (strlen(in) + 1);	         /* bump input pointer */
	}
	return;
}
