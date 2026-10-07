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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: NLcsv.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:29 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NLcsv
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 * 
 * NLcsv.c	1.4  com/lib/c/nls,3.1,8943 9/14/89 09:34:27
 */

 /* No includes */

/*
 * EXTERNAL PROCEDURES CALLED:  None
 */


/*
 * NAME: NLcsv
 *
 * FUNCTION: Return a pointer to the character set vector.
 *
 *           The character set vector is a character string
 *           which consists of byte pairs which are the begin
 *           and end points of multibyte (16-bit) characters.
 *
 * (EXECUTION ENVIRONMENT:)
 *
 *      Environment-Specific aspects, such as -
 *      Preemptable        : Yes
 *      VMM Critical Region: No
 *      Runs on Fixed Stack: No
 *      May Page Fault     : Yes
 *      May Backtrack      : Yes
 *
 * (NOTES:) Returns a pointer to the character set vector.
 *
 * (DATA STRUCTURES:) The character set vector is defined as a static
 *                    array of type char.
 *
 * RETURN VALUE DESCRIPTION: Returns a pointer to the character set vector.
 */

#ifdef KJI  /* Character Set Vector for Kanji */
static char csv[] = { 0x81, 0x9f, 0xe0, 0xfc, 0x00, 0x00 };
#else      /* Character Set Vector for RTASCII */
static char csv[] = { 0x1c, 0x1f, 0x00, 0x00 };
#endif

char *
NLcsv() {
	return(csv);
}
