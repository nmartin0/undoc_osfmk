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
 * $Log: mbstr.h,v $
 * Revision 1.3  1994/11/18  21:10:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:15:53  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:58:43  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:14  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:19:49  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:37:23  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:31:20  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:51:10  gm]
 * 
 * Revision 1.2  90/03/13  21:22:48  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:23:40  tom]
 * 
 * $EndLog$
 */
/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)mbstr.h	1.3  com/inc,3.1,8943 7/11/89 18:19:06 */
/*
 * NAME: mbstr.h
 */                                                                   

#ifndef _MBSTR_H_
#define _MBSTR_H_

#include <sys/types.h>

#define MBMAXLEN	2

#ifdef   _NO_PROTO
extern size_t mbslen();
extern char *mbscat();
extern char *mbsncat();
extern int mbscmp();
extern int mbsncmp();
extern char *mbscpy();
extern char *mbsncpy();
extern char *mbschr();
extern char *mbsrchr();
extern char *mbspbrk();
extern int mbstoint();
#else  /*_NO_PROTO */
extern size_t mbslen(const char *);
extern char *mbscat(char *, char *);
extern char *mbsncat(char *, const char *, size_t);
extern int mbscmp(char *, char *);
extern int mbsncmp(char *, char *, size_t);
extern char *mbscpy(char *, char *);
extern char *mbsncpy(char *, char *, size_t);
extern char *mbschr(const char *, const int);
extern char *mbsrchr(char *, int);
extern char *mbspbrk(char *, char *);
extern int mbstoint(char *);
#endif /*_NO_PROTO */

#endif  /* _MBSTR_H_ */
