/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * boot.c,v
 * Revision 1.1.4.2  1993/02/12  17:35:34  dleslie
 * Adding new code from vendor
 *
 * Revision 1.1  1992/09/30  02:03:28  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.2.1  92/04/30  11:52:49  bernadat
 * 	Add default kernel names.
 * 	Try default name when typed name is wrong.
 * 	[92/04/29            bernadat]
 * 
 * 	Merged in MK tree.
 * 	Fixed code to use mk includes only (not sys/inode.h ....).
 * 	Do not loop on default names if user entered a bad file name.
 * 	Fix maj/unit assignments.
 * 	Pass complete typed line to kernel
 * 	Copied from main line
 * 	[92/03/19            bernadat]
 * 
 * Revision 2.2  92/04/04  11:34:37  rpd
 * 	Change date in banner.
 * 	[92/04/03  16:51:14  rvb]
 * 
 * 	Fix Intel Copyright as per B. Davies authorization.
 * 	[92/04/03            rvb]
 * 	From 2.5 version.
 * 	[92/03/30            mg32]
 * 
 */
/* CMU_ENDHIST */
/*
 * Mach Operating System
 * Copyright (c) 1992, 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */

/*
 */

/*
  Copyright 1988, 1989, 1990, 1991, 1992 
   by Intel Corporation, Santa Clara, California.

                All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Intel
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "boot.h"
#include <i386/exec.h>
#include <sys/reboot.h>

#define	KERNEL_BOOT_ADDR	0x100000	/* load at 1 Megabyte */

struct exec head;
int argv[10], esym;
char *name;
char *names[] = {
	"/mach", "/RFS/.LOCALROOT/mach",
	"/mach_kernel", "/RFS/.LOCALROOT/mach_kernel",
	"/vmunix", "/RFS/.LOCALROOT/vmunix",
	"/mach.old", "/RFS/.LOCALROOT/mach.old"
};
#define NUMNAMES	(sizeof(names)/sizeof(char *))

int currname;		/* names[] index of current name */

char namebuf[BOOT_LINE_LENGTH];	/* complete string typed at boot prompt */

boot(drive)
     int drive;
{
	int loadflags;
	part = unit = 0;
#ifdef	SD
	maj = (drive&0x80 ? 3 : 1);
#else	SD
	maj = (drive&0x80 ? 0 : 1);
#endif	SD
	printf("\n>> MACH BOOT: %d/%d k of memory  [4/4/92]\n",
	       argv[7] = memsize(0),
	       argv[8] = memsize(1));
	gateA20();
loadstart:
	if (name == names[currname]) {
		if (++currname == NUMNAMES)
			currname = 0;
	}
	name = names[currname];
	loadflags = KERNEL_BOOT_ADDR;
	getbootdev(&loadflags);
	if (openrd()) {
		printf("Can't find %s\n", name);
		goto loadstart;
	}
	if (inode.i_mode&IEXEC)
		loadflags |= RB_KDB;
	loadprog(loadflags);
	goto loadstart;
}

loadprog(howto)
	int		howto;
{
	int i;
	char *addr;
	static int (*x_entry)() = 0;

	argv[3] = 0;
	argv[4] = 0;
	read(&head, sizeof(head));
	if (head.a_info == 0407)
		poff = 32;
	else if (head.a_info == 0413 ) {
		poff = 0;
		head.a_text += sizeof(struct exec);
	} else {
		printf("Invalid format!\n");
		return;
	}

	addr = (char *) (howto & 0xffff0000);
	printf("Booting %s(%d,%c)%s\n", devs[maj], unit, 'a'+part, name);
	printf("%d", head.a_text);
	xread(addr, head.a_text);
	addr += head.a_text;
	printf("+%d", head.a_data);
	xread(addr, head.a_data);
	addr += head.a_data;
	printf("+%d", head.a_bss);

	argv[3] = (int)(addr += head.a_bss);
	pcpy(&head.a_syms, addr, sizeof(head.a_syms));
	addr += sizeof(head.a_syms);
	printf("[+%d", head.a_syms);
	xread(addr, head.a_syms);
	addr += head.a_syms;
	read(&i, sizeof(int));
	pcpy(&i, addr, sizeof(int));
	i -= sizeof(int);
	addr += sizeof(int);
	printf("+%d]", i);
	xread(addr, i);
	addr += i;
	argv[4] = ((int)(addr+sizeof(int)-1))&~(sizeof(int)-1);
	pcpy(namebuf, argv[4], BOOT_LINE_LENGTH);

	printf("\n");

	/*
	 *  We now pass the various bootstrap parameters to the loaded
	 *  image via the argument list
	 *
	 *  arg1 = boot flags
	 *  arg2 = boot device
	 *  arg3 = start of symbol table (0 if not loaded)
	 *  arg4 = end of symbol table (0 if not loaded)
	 *  arg5 = transfer address from image
	 *  arg6 = transfer address for next image pointer
	 */
	argv[1] = howto;
	switch(maj) {
	case 1:
		printf("\n\nInsert file system \n");
		getchar();
		break;
	case 4:
		maj = 3;
		unit = 0;
		break;
	}
	argv[2] = (maj << B_TYPESHIFT) |
		(unit << B_UNITSHIFT) |
		(part << B_PARTITIONSHIFT);
	argv[5] = (head.a_entry &= 0xffffff);
	argv[6] = (int) &x_entry;
	argv[0] = 8;
	startprog(head.a_entry,argv);
}

char kernel_name[BOOT_LINE_LENGTH];

getbootdev(howto)
     int *howto;
{
	char c, *ptr = namebuf; 
	int name_found = 0;

	printf("\nboot: ");
	if (gets(namebuf)) {
		while (c=*ptr) {
			while (is_space(c))
				c = *++ptr;
			if (!c)
				return;
			if (c=='-')
				while ((c = *++ptr) && !is_space(c)) {
					switch (c) {
					      case 'a':
						*howto |= RB_ASKNAME; break;
					      case 's':
						*howto |= RB_SINGLE; break;
					      case 'd':
						*howto |= RB_KDB; break;
					      case 'b':
					      case 'h':
						*howto |= RB_HALT; break;
					      default:
						/*
						 * check if digit, in this
						 * case it is the number
						 * of cpus to boot
						 */
						if (c >'0' && c <= '9')
						  *howto |= ((c-'0')<<8);
						else {
						  /* 
						   * this is not a boot arg
						   * pass it to the server
						   * and assume all the rest
						   * of the line is server
						   * arguments
						   */
						  *(ptr-1) = '-';
						  return;
						}
					}
					/*
					 * since this is a boot arg, clear
					 * it for the server, if it must
					 * be passed to the server it is
					 * done via the howto argument
					 */
					*(ptr) = ' ';   /* erase this arg */
					*(ptr-1) = ' '; /* for the server */
				}
			else if (!name_found++) {
			  	/*
				 * look for kernel:server
				 * Copy kernel part only to kernel_name
				 */
				if (*ptr != server_delimiter) {
				  	name = kernel_name;
					while ((c = *ptr) && !is_space(c) && c != server_delimiter)
						*name++ = *ptr++;
					*name = 0;	/* null terminate */
					name = kernel_name;
				}
				while ((c = *ptr) && !is_space(c))
					ptr++;
			} else
				/* must be a server argument without '-' */
				return;
		}
	}
}




