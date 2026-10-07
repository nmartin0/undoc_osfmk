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
/*
 * OSF/1 Release 1.0
 */
/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1985, 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */

/*
 * NAME: 	AFopen
 * FUNCTION: 	Open an Attribute File.
 *
 * ARGUMENTS:	filename	- Attribute filename
 *		maxrecsiz	- Maximum size of a record (in bytes)
 *		maxnumatr	- Maximum number of attributes per record
 *
 * RETURN VALUE DESCRIPTION: Returns a pointer to an Attribute File structure.
 */

#include <stdio.h>
#include <sys/types.h>
#include <AFdefs.h>

static const char DEFAULT[] = "default";


AFILE_t 
AFopen( char * filename, int maxrecsiz, int maxnumatr )
{
	register AFILE_t af;
	register FILE *file;
	register int n = sizeof(struct ATTR) * (maxnumatr +1);

	/*
	 *	Open attribute file
	 */
	if ((file = fopen(filename,"r")) == NULL)
	    return(NULL);

	/*
	 *	Allocate and initialize attribute file structure
	 */
	if ((af=(AFILE_t)malloc((size_t)sizeof(struct AFILE))) == NULL)
	{
	    fclose(file);
	    return(NULL);
	}
	af->AF_iop = file;
	af->AF_maxsiz = maxrecsiz;
	af->AF_maxatr = maxnumatr;
	af->AF_dflt = (char *)DEFAULT;
	af->AF_errs = AF_OK;

	/*
	 *	Allocate and initialize current entry 
	 */
	if ((af->AF_cent.EN_cbuf=(char *)malloc((size_t)maxrecsiz +1)) == NULL)
	{
	    free((void *)af);
	    fclose(file);
	    return(NULL);
	}
	if ((af->AF_cent.EN_catr=(ATTR_t)malloc((size_t)n)) == NULL)
	{
	    free((void *)af->AF_cent.EN_cbuf);
	    free((void *)af);
	    fclose(file);
	    return(NULL);
	}
	af->AF_cent.EN_name = NULL;
	af->AF_cent.EN_natr = NULL;
	af->AF_cent.EN_catr->AT_name = NULL;
	af->AF_cent.EN_catr->AT_value = NULL;
	af->AF_cent.EN_catr->AT_nvalue = NULL;

	/*
	 *	Allocate and initialize default entry 
	 */
	if ((af->AF_dent.EN_cbuf=(char *)malloc((size_t)maxrecsiz +1)) == NULL)
	{
	    free((void *)af->AF_cent.EN_catr);
	    free((void *)af->AF_cent.EN_cbuf);
	    free((void *)af);
	    fclose(file);
	    return(NULL);
	}
	if ((af->AF_dent.EN_catr=(ATTR_t)malloc((size_t)n)) == NULL)
	{
	    free((void *)af->AF_dent.EN_cbuf);
	    free((void *)af->AF_cent.EN_catr);
	    free((void *)af->AF_cent.EN_cbuf);
	    free((void *)af);
	    fclose(file);
	    return(NULL);
	}
	af->AF_dent.EN_name = NULL;
	af->AF_dent.EN_natr = NULL;
	af->AF_dent.EN_catr->AT_name = NULL;
	af->AF_dent.EN_catr->AT_value = NULL;
	af->AF_dent.EN_catr->AT_nvalue = NULL;

	return(af);
}
