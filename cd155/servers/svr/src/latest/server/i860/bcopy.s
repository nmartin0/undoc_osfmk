// 
// $Copyright
// Copyright 1993, 1994 , 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: bcopy.s,v $
// Revision 1.2  1994/11/18  20:31:37  mtm
// Copyright additions/changes
//
// Revision 1.1  1993/09/21  20:00:09  cfj
// Pick up the dnoveck@osf.org i860 bcopy changes from ad1.0.5
//
 * Revision 2.2  93/06/16  13:50:18  dnoveck
 * 	Initial version.
 * 
 * 
 * $EndLog$
 */

#define _BCOPY_
#if MACH_DEBUG
#define _BCOPY_DEBUG_
#define _BCOPY_ERROR1_ _printf
#define _BCOPY_ERROR2_ _Debugger
#endif /* MACH_DEBUG */
#include <i860/i860_copy.s.h>




