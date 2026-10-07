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
 * NAME:	AFnxtatr 
 * FUNCTION: 	Get the next attribute for Attribute File entry
 * RETURN VALUE DESCRIPTION: Returns a pointer to an Attribute structure.
 */

#include <stdio.h>
#include <string.h>
#include <AFdefs.h>

ATTR_t
AFnxtatr( ENT_t entry )
{
	if (entry == NULL || entry->EN_catr == NULL)
		return(NULL);

	if (entry->EN_natr == NULL )
		entry->EN_natr = entry->EN_catr;
	else if ( entry->EN_natr->AT_name != NULL )
		entry->EN_natr++;

	if ( entry->EN_natr->AT_name == NULL )
		return(NULL);
	entry->EN_natr->AT_nvalue = NULL;
	return(entry->EN_natr);
}
