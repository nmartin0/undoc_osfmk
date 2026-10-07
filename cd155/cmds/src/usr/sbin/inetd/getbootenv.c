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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1993  Intel Corporation.
 *
 */
/*
 * HISTORY
 * $Log: getbootenv.c,v $
 * Revision 1.4  1994/11/19  03:07:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/08/31  20:23:57  bradf
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.1.4.1  1994/08/10  18:25:40  flb
 * Reviewer: shane
 * Risk: low
 * Benefit or PTS #:9976
 * Testing: System Boot with large MAGIC.MASTER
 * Module(s):getmagic/kernops.c,bootmesh/kernops.c,bootmesh/bootmesh.h,cbs/kernops.c,mkdevtab/kernops.c,mkdevtab/bootmagic.h,inetd/getbootenv.c,load_level/src/osf1_dep.c,ufs_fsck/admagic.c
 *
 * Revision 1.1  1994/02/07  15:32:09  stefan
 * If inetd was (re)started on a node other than ROOT_FS_NODE, it had a
 * wrong idea about which node is ROOT_FS_NODE. This could result in
 * inetd scheduling network jobs onto ROOT_FS_NODE instead of skipping
 * ROOT_FS_NODE.
 * This fix uses the boot magic strings and table(TBL_PHYSNODEINFO) for
 * finding out the correct node number for ROOT_FS_NODE.
 *
 *  Reviewer: shala
 *  Risk: low to medium
 *  Benefit or PTS #: 8083
 *  Testing: developer testing
 *  Module(s): cmds_libs/src/usr/sbin/inetd/next_node.c
 *             cmds_libs/src/usr/sbin/inetd/Makefile
 *             cmds_libs/src/usr/sbin/inetd/getbootenv.c
 *
 */

#include	<stdio.h>
#include	<malloc.h>
#include 	<mach/port.h>
#include 	<mach/error.h>
        

#define	BOOTMAGIC_MAX	(1024 * 16)

/*
 * buffers for entire bootmagic string
 */
static char	*bootmagic = NULL;	/* internal null-terminated format */



#define	BOOTMAGIC_TRAP

#ifdef	BOOTMAGIC_TRAP	/* use BOOTMAGIC_TRAP in place of mach rpc */

int
bootmagic_trap(buffer)
    char    *buffer;
{
    asm("trap r0,r25,r0");
}


#endif	/* BOOTMAGIC_TRAP */


char * getbootenv(name)
char	*name;
{
	char		*tmp_bootmagic;
	kern_return_t	kr;
	mach_port_t	privileged_host_port;
        int		len;
        char		*src;
        char		*dest;
        char		*str;
	int		i;


	if ( bootmagic == NULL ) {
		/*
		 * Not initialized yet.
		 */
		bootmagic = (char *) malloc(BOOTMAGIC_MAX);
		if ( bootmagic == NULL ) {
			/*
			 * Oops - out of memory.
			 */
			return (NULL);
		}

		tmp_bootmagic = (char *) malloc(BOOTMAGIC_MAX);
		if ( tmp_bootmagic == NULL ) {
			/*
			 * Oops - out of memory.
			 */
			return (NULL);
		}

		/*
		 * Get the boot magics from the kernel.
		 */

#ifdef	BOOTMAGIC_TRAP	/* use BOOTMAGIC_TRAP in place of mach rpc */

		if (bootmagic_trap(tmp_bootmagic) <= 0) {
			/*
			 * Oops - didn't get boot magics from kernel.
			 */
	    		return (NULL);
		}
#else	/* use mach rpc in place of BOOTMAGIC_TRAP */

		privileged_host_port = task_by_pid(-1);
		kr = host_get_boot_info(privileged_host_port, tmp_bootmagic);
		if ( kr != KERN_SUCCESS ) {
			/*
			 * Oops - didn't get boot magics from kernel.
			 */
	    		return (NULL);
		}
#endif	/* use mach rpc in place of BOOTMAGIC_TRAP */

		/*
		 * Convert to null terminated format.
		 */
		len = strlen(tmp_bootmagic);

		/*
		 * Copy source to bootmagicination, converting
		 * newlines to NULLs.
		 */
		src = tmp_bootmagic;
		dest = bootmagic;
		for ( i = 0; i < len; i++ ) {
			*dest++ = *src++;
			if ( *(src - 1) == '\n' ) {
				*(dest - 1) = NULL;
			}
		}

		/*
		 * Add terminating NULL.
		 */
		*dest = NULL;

		/*
		 * Free temporary buffer.
		 */
		free(tmp_bootmagic);
	}

	/*
	 * Search for the boot magic we have been asked for.
	 */
	len = strlen(name);
	for ( str = bootmagic; *str; str += strlen(str) + 1 ) {
		if ( strncmp(str, name, len) == 0 && str[len] == '=' ) {
			return (&str[len + 1]);
		}
	}

	/*
	 * Not found.
	 */
	return (NULL);
}
