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
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: dirname.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:14 $";
#endif
/*
 * COMPONENT_NAME: (LIBCADM) Standard C Library System Admin Functions 
 *
 * FUNCTIONS: dirname 
 *
 * ORIGINS: 27 
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * dirname.c	1.7  com/lib/c/adm,3.1,8943 9/12/89 13:18:12
 */

#include <string.h>
#include <stdio.h>
/*LINTLIBRARY*/ 

char *
dirname (path)
     char *path;
{
  char        *cp;
  static char  buf[BUFSIZ];
  
  /*
  ** no slash ==> return null pointer
  */
  if (!(cp = strrchr (path, '/')))
      return cp;
  /*
  ** else, pointer to character following right-most slash
  ** I wish strcpy would terminate things so I could do this all
  ** in the return
  */
  buf [cp-path] = '\0';
  return strncpy (buf, path, (size_t)(cp - path));

}
    
  
      
