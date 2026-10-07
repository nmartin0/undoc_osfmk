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
typedef long  wchar_t;
#endif

#ifndef NULL
#define NULL    0
#endif


#define offsetof(s_name, s_member) (size_t)&(((s_name *)0)->s_member)

#endif /* _ANSI_C_SOURCE */
#endif /* _STDDEF_H_ */
