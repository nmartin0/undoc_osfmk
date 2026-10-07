/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: stddef.h,v $
 * Revision 1.3  1994/11/18  21:10:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:06  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:56  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:18:40  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:08  nandy
 * Initial revision
 *
 * Revision 1.4  92/04/13  14:39:23  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.4  90/10/07  20:36:53  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:59:31  gm]
 * 
 * Revision 1.3  90/04/27  23:07:19  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/17  12:36:43  tom]
 * 
 * Revision 1.2  90/03/13  21:23:31  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:37:16  tom]
 * 
 * $EndLog$
 */
/* @(#)stddef.h 1.9  com/inc,3.1,9013 1/14/90 16:46:17 */
/*
 * COMPONENT_NAME: stddef.h
 *                                                                    
 * ORIGIN: IBM
 *
 * Copyright International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   

#ifndef _STDDEF_H_ 
#define _STDDEF_H_ 
#include <standards.h>

/*
 *
 *      The ANSI standard requires that certain values be in stddef.h.
 *      It also requires that if _ANSI_C_SOURCE is defined then ONLY these
 *      values are present. This header includes all the ANSI required entries.
 *
 */

#ifdef _ANSI_C_SOURCE

/*
 *      The following definitions are included in <sys/types.h>.  They
 *      are included in <stddef.h> to comply with ANSI.
 */

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef int             ptrdiff_t;
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long   size_t;
#endif

#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned short  wchar_t;
#endif

#ifndef NULL
#define NULL    0
#endif


#define offsetof(s_name, s_member) (size_t)&(((s_name *)0)->s_member)

#endif /* _ANSI_C_SOURCE */
#endif /* _STDDEF_H_ */
