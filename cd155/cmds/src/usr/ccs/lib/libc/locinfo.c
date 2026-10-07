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
static char rcsid[] = "@(#)$RCSfile: locinfo.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:55 $";
#endif

/*
 * When adding a locale to libc, the following steps must be performed:
 * 
 * 1)  The collating table (ctab) information for the locale is transferred 
 *     into binary form using the ctab utility.
 * 2)  The locale is added to the libc Makefile.  This causes the environment
 *     information for the locale and the binary form of the ctab information
 *     to be used by the libloc utility to create a C-language source module.
 *     The libloc utility will take as an argument the name of the locale.
 *     This "name" is traditionally one or two upper case characters (such
 *     as DA for Danish).  If name is, for example, DA, then 'DA' (without
 *     the quotes) should be added to the libc Makefile, the binary form of
 *     the ctab information for this locale should reside in /etc/nls/DA,
 *     and the environment information for the locale should reside in 
 *     /etc/nls/DA.en.  The source module produced by the libloc utility
 *     will be compiled and archived into the library.
 * 3)  The locale's name (DA in the example above) is placed in the _locnms
 *     array below, surrounded by double quotes and followed by a comma. 
 *     The array must be NULL terminated.
 * 4)  The locale's name is added to the _locadr array below IN THE SAME 
 *     POSITION AS IT WAS ADDED TO _locnms!  i.e. if DA were made the 
 *     second element of _locnms, then '&DA,' (without the quotes) must 
 *     be the second element of _locadr.
 * 5)  The library is rebuilt.
 */

#include <NLctype.h>

char *_locnms[] = {
	"C",
	NULL
};

extern loc_t C;

loc_t *_locadr[] = {
	&C,
	NULL
};
