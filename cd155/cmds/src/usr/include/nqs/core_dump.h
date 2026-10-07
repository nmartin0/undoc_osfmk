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
 
/*++ core_dump.h - Network Queueing System
 *
 */
/*
 * HISTORY
 * $Log: core_dump.h,v $
 * Revision 1.2  1994/11/19  02:43:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 *
 */


#if !defined SCCS
static char     sccs_id[] = "@(#)core_dump.h	1.3 (core_dump.h OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif


/*
 *      Handy defines for reducing lint errors.
 */

void CoreDump();
extern int errno;

#define VOID    (void)
#define FPRINTF VOID fprintf
/* #define PANIC   CoreDump(module_name,__LINE__,errno)  */
#define PANIC JUNK_for_now_abort()
#define PANIC_2(errmsg)   Panic_2(module_name,__LINE__,Error,errmsg)
#define PRINTF  VOID printf
#define SPRINTF VOID sprintf


