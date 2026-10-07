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
 * $Log: addrconf.h,v $
 * Revision 1.4  1994/11/18  20:30:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:55:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:14:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:04  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:19:17  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:12:32  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/10/15  11:35:51  roy
 * Fix revision history comments.
 *
 * Revision 2.2  91/10/14  12:14:31  sjs
 *	Initial revision.
 * 
 * Revision 3.1  91/10/04  16:38:43  sp
 * move the loader data from on top of emulator and add emulator addresses
 * 
 * Revision 3.0  91/09/13  12:46:40  sp
 * included for mmap
 * 
 * Revision 1.3.5.3  90/12/20  14:44:27  devsrc
 * 	Merge 1.01 sandbox to osc1.0
 * 
 * Revision 1.3.5.2  90/12/13  11:12:56  kwallace
 * 	Merged changes from latest branch.
 * 	Revision 1.3.3.2  90/11/29  14:06:33  hermi
 * 		Changes for the loader and shared libs.
 * 		[90/11/29  13:51:29  hermi]
 * 	[90/12/13  10:15:44  kwallace]
 * 
 * Revision 1.3  90/10/07  13:36:44  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:29:34  gm]
 * 
 * Revision 1.2  90/07/17  11:27:18  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:16:39  kevins]
 * 
 * Revision 1.2  90/06/22  20:16:43  devrcs
 * 	Initial coding
 * 	[90/06/06  09:32:35  lwa]
 * 
 * $EndLog$
 */

/* addrconf.h -- Machine-dependent address config record initialization
 * Note that this file contains a data structure initialization.  Thus,
 * it must ONLY be included by a single C file: bsd/kern_mman.c.
 * NO ONE ELSE SHOULD INCLUDE THIS FILE!
 * Depends on <sys/types.h> and <sys/addrconf.h>
 * 
 * On the 386 architecture we put the text region starting at
 * 0x400000 as this is at a 4MB boundary.  The intent is to
 * separate text, data and bss and place each on separate 4MB
 * boundaries.  This permits the two level memory management
 * scheme of the architecture to set protection attributes
 * efficiently at the page directory level. The first 4MB
 * chunk is not used so that an attempt to dereferncing address
 * 0 will result in a trap.
 *
 * The user stack starts at ~0xa0000000 (few words used for
 * signal handling cruft) and grows downwards. The address
 * space between the user stack and the kernel (0xc0000000)
 * is used for both system V.3 shared libraries and OS/1 shared
 * libraries (presumably you don't use both together).
 */

struct addressconf addressconf[AC_N_AREAS] = {

	{ (caddr_t)0x400000, AC_UPWARD|AC_FIXED }, /* AC_TEXT */
	{ (caddr_t)0x800000, AC_UPWARD|AC_FIXED }, /* AC_DATA */
	{ (caddr_t)0xc00000, AC_UPWARD|AC_FIXED }, /* AC_BSS */
	{ (caddr_t)0xa0000000, AC_DOWNWARD|AC_FIXED }, /* AC_STACK */
	{ (caddr_t)0x90000000, AC_UPWARD|AC_FIXED }, /* AC_LDR_TEXT */
	{ (caddr_t)0x90000000, AC_UPWARD|AC_FLOAT }, /* AC_LDR_DATA */
	{ (caddr_t)0x90000000, AC_UPWARD|AC_FLOAT }, /* AC_LDR_BSS */
#ifndef	OSF1_SERVER
	{ (caddr_t)0xa0000000, AC_UPWARD|AC_FIXED }, /* AC_LDR_PRIV */
	{ (caddr_t)0xa2000000, AC_UPWARD|AC_FLOAT }, /* AC_LDR_GLB */
	{ (caddr_t)0xa4000000, AC_UPWARD|AC_FLOAT }, /* AC_LIB_PRELOAD */
#else	/* OSF1_SERVER */
	{ (caddr_t)0x70000000, AC_UPWARD|AC_FIXED }, /* AC_LDR_PRIV */
	{ (caddr_t)0x72000000, AC_UPWARD|AC_FLOAT }, /* AC_LDR_GLB */
	{ (caddr_t)0x74000000, AC_UPWARD|AC_FLOAT }, /* AC_LIB_PRELOAD */
#endif	/* OSF1_SERVER */
	{ (caddr_t)0x30000000, AC_UPWARD|AC_FLOAT }, /* AC_MMAP_TEXT */
	{ (caddr_t)0x40000000, AC_UPWARD|AC_FLOAT }, /* AC_MMAP_DATA */
	{ (caddr_t)0x40000000, AC_UPWARD|AC_FLOAT }, /* AC_MMAP_BSS */
#ifdef	OSF1_SERVER
	{ (caddr_t)0xa0000000, AC_UPWARD|AC_FIXED }, /* AC_EMUL */
	/* Place holder, maybe this is not a good adress */
	{ (caddr_t)0x80000000, AC_UPWARD|AC_FIXED }, /* AC_EMUL_MFILE */
#endif
};
