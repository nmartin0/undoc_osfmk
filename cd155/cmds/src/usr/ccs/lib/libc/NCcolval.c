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
static char rcsid[] = "@(#)$RCSfile: NCcolval.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:01:57 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCcolval
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
 * NCcolval.c	1.5  com/lib/c/nls,3.1,9013 2/28/90 10:26:55
 */

#include <sys/types.h>
#include <NLctype.h>

/*LINTLIBRARY*/
/*
 *
 *  Find the (not necessarily unique) collation value of a character.
 *
 *  Elements in NLcoldesc contain four fields: 
 *	cd_stroff	the offset from the start of the NLcoldesc array 
 *			of a collating string,
 *	cd_repoff	the offset from the start of the NLcoldesc array
 *			of a replacement string
 *	cd_cval		a collate value
 *	cd_cuniq	a coluniq value
 *
 *  The fields are interpreted as follows:
 *      cd_stroff  cd_repoff  cd_cval   cd_cuniq
 *      ---------  ---------  -------   --------
 *       "from"      "to"       0         0        n-to-n mapping
 *       "from"        0       coll      uniq      multi-char coll symbol
 *         0           0       coll      uniq      default for char
 *     
 *  IF the "cd_stroff" field is non-zero and the cd_cval is zero,
 *  then the entry defines a many-to-many replacement mapping.
 *  NCcolval is called recursively to determine the collation value of
 *  the first character in the replacement string.
 *  
 *  If the "repoff" field in zero, and "stroff" is non-zero, then 
 *  the entry defines a multi-character collating element (e.g. Spanish
 *  'ch'); input pointer is set past element, and collation value
 *  is returned.
 *  
 *  If "cd_stroff" and "cd_repoff" are zero, then the entry defines the
 *  collating (and coluniq) value for the character itself; and collation
 *  value is returned.
 */

/*  Find collating value for character chr.  If NCcollate returns a negative
 *  value, go through the collating descriptors until find the one for the
 *  individual character and use that (not necessarily unique) collation 
 *  value.  For 1-n collation, use the collation value of the first 
 *  character in the replacement string.
 */
int
NCcolval (wchar_t chr)
{
	register coldesc_t *desc;
	register coldesc_t *coldescp = _locp->lc_coltbl->lc_coldesc;
	register wchar_t *stringsp   = _locp->lc_coltbl->lc_strings;
	register int rval;

	rval = NCcollate (chr);
	if (rval < 0) {
		rval = -rval - 1;
		for (desc = &coldescp[rval]; ; ++desc) {
			rval = desc->cd_cval;
			if (desc->cd_stroff == 0)
				break;
			if (desc->cd_cval == 0) {
				chr = *(stringsp + desc->cd_repoff);
				rval = NCcolval(chr);
				break;
			}
		}
	}
	return (rval);
}
