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
 * NAME:	AFgetent
 * FUNCTION: 	Find the first entry in an Attribute File which has a given name.
 * RETURN VALUE DESCRIPTION: Returns a pointer to an Attribute structure.
 */

#include <stdio.h>
#include <string.h>
#include <AFdefs.h>

ENT_t
AFgetent( AFILE_t af, char * name )
{

	AFrewind(af);
	while (AFnxtent(af)) 
		if (strcmp(af->AF_cent.EN_name, name) == 0)
			return(&af->AF_cent);
	return(NULL);
}
