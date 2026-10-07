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
 *
 * NAME:	AFnxtval
 * FUNCTION:	Get the next attribute value for current attribute.
 * RETURN VALUE DESCRIPTION: Returns a pointer the next string value in the
 *		attribute list.
 */

#include <stdio.h>
#include <string.h>
#include <AFdefs.h>

char *
AFnxtval( ATTR_t attr )
{
	if (attr == NULL || attr->AT_value == NULL)
		return(NULL);

	if (attr->AT_nvalue == NULL) {
		attr->AT_nvalue = attr->AT_value;
		return(attr->AT_nvalue);
	}

	for (;*attr->AT_nvalue != '\0'; attr->AT_nvalue++)
	;

	if ( *(attr->AT_nvalue+1) == '\0' )
		return(NULL);

	attr->AT_nvalue++;
	return(attr->AT_nvalue);
}
