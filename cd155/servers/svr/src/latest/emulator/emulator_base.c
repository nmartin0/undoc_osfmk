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
 * HISTORY
 * $Log: emulator_base.c,v $
 * Revision 1.7  1994/11/18  20:23:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  17:31:11  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.3  1993/07/01  18:23:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  20:14:56  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:17:37  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:17:45  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 2.6  1993/02/22  13:34:09  yazz
 * Increase the maximum size of the emulator, for Paragon.
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:00:26  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.3  1993/01/22  17:27:24  terry
 * changed starting address of data to 0x50000
 *
 * Revision 1.2  1992/11/30  22:08:36  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:15:46  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 4.1  1992/11/03  23:57:30  cfj
 * Bump major revision number.
 *
 * Revision 2.7  1992/10/14  17:24:21  cfj
 * NX integration.
 *
 * Revision 2.6  1992/10/10  03:30:01  cfj
 * Bumped the start of emulator data. Will have to bump it again
 * for the NX changes.
 *
 * Revision 2.5  1992/04/05  16:41:16  pjg
 * 	Make starting of data go from EMULATOR_BASE+(148*1024) to
 * 	EMULATOR_BASE+(160*1024) to make room for TNC (rabii)
 * 
 * Revision 2.4  92/02/11  18:48:38  pjg
 * 	Fixed size for i860 (rabii).
 * 
 * Revision 2.3  91/11/14  15:33:11  rabii
 * 	Added i860 support
 * 
 * Revision 2.2  91/08/30  16:40:26  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.0  91/01/17  12:05:12  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.3  90/05/21  13:45:57  dbg
 * 	Return 0 for success.
 * 	[90/03/14            dbg]
 * 
 * Revision 2.2  89/11/29  15:26:49  af
 * 	Added mips, RCS-ed.
 * 	[89/11/16            af]
 * 
 */
#include <machine/vmparam.h>
#include <sys/exec.h>

main()
{
#ifdef sun
	printf("%x\n", EMULATOR_BASE + sizeof (struct exec));
#else	/* sun */
#ifdef mips
	printf("%x -D %x\n", EMULATOR_BASE, EMULATOR_BASE + (1024*1024));
#else	/* mips */
#ifdef	I860
        printf("0x%x -d 0x%x\n", EMULATOR_BASE, EMULATOR_BASE + (384*1024));
#else	/* I860 */
	printf("%x\n", EMULATOR_BASE);
#endif	/* I860 */
#endif	/* mips */
#endif	/* sun */
	return (0);
}
