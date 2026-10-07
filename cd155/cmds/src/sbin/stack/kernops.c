/*
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 *
 *
 */


#include	"bootmagic.h"

#ifdef PARAGON

#include 	<mach/port.h>
#include 	<mach/error.h>
#include 	<mach/message.h>
#include 	<mach/norma_special_ports.h>
        
/*
** Routines which communicate directly with microkernel services.
**	get_boot_magic()
*/


#define	BOOTMAGIC_TRAP

#ifdef	BOOTMAGIC_TRAP	/* use BOOTMAGIC_TRAP in place of mach rpc */

int
bootmagic_trap(buffer)
    char    *buffer;
{
    asm("trap r0,r25,r0");
}

/*
 * Request bootmagic from microkernel
 */
get_boot_magic(bootmagic)
	char	*bootmagic;
{
    int 	boot_magic_len;

    boot_magic_len = bootmagic_trap(bootmagic);

    if (boot_magic_len > 0)
	return (0);
}


#else	/* use mach rpc in place of BOOTMAGIC_TRAP */

/*
 * Request bootmagic from microkernel
 */
get_boot_magic(bootmagic)
	char	*bootmagic;
{
	kern_return_t       kr;
	mach_port_t privileged_host_port;

	privileged_host_port = task_by_pid(-1);
	kr = host_get_boot_info(privileged_host_port,bootmagic);
	if (kr != KERN_SUCCESS)
	    return(-1);
	return(0);
}

#endif	/* use mach rpc in place of BOOTMAGIC_TRAP */


#endif	/* PARAGON */
