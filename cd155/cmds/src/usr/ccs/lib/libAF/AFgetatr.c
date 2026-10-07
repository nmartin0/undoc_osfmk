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
 *
 * NAME:	AFgetstr
 * FUNCTION:	Get the attribute value matching name.
 * RETURN VALUE DESCRIPTION: Returns a pointer to the value of name.
 */

#include <stdio.h>
#include <string.h>
#include <AFdefs.h>

ATTR_t
AFgetatr( ENT_t entry, char * name )
{
	if (entry == NULL || name == NULL)
		return(NULL);

	if ((entry->EN_natr=entry->EN_catr) == NULL)		/* Null list */
		return(NULL);

	for ( ;entry->EN_natr->AT_name != NULL; entry->EN_natr++) {
	    
	    if (strcmp(entry->EN_natr->AT_name, name) == 0) {
		    entry->EN_natr->AT_nvalue = NULL;
		    return(entry->EN_natr);
	    }

	}
	return(NULL);
}
