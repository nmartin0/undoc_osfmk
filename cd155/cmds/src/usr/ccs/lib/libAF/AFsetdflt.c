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
 * NAME: 	AFsetdflt
 * FUNCTION: 	Sets the default entry name for an Attribute File.
 *
 * ARGUMENTS:	af		- Attribute file
 *		dflt		- pointer to char string (NULL is valid)
 *
 * RETURN VALUE DESCRIPTION: Returns a pointer to an Attribute File structure.
 */

#include <stdio.h>
#include <sys/types.h>
#include <AFdefs.h>

void
AFsetdflt( AFILE_t af, char * dflt )
{
	if (af != NULL)
		af->AF_dflt = dflt;
	return;
}
