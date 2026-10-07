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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 *
 * relocate.c
 *
 * machine dependent relocation for a Mach-O file
 * for the AT386 (intel 386).
 *
 * OSF/1 Release 1.0.1
 */

#include <sys/types.h>
#include <sys/param.h>
#include <loader.h>
#include <math.h>
#include <strings.h>

#include "ldr_types.h"
#include "ldr_sys_int.h"
#include "ldr_windows.h"
#include "ldr_malloc.h"
#include "ldr_region.h"
#include "ldr_package.h"
#include "ldr_symbol.h"
#include "ldr_errno.h"
#include "ldr_symval_std.h"
#include "ldr_hash.h"

#include <mach_o_header.h>
#include <mach_o_format.h>

#include "open_hash.h"
#include "mo_ldr.h"
#include "relocate.h"


/* #define DEBUG	1 */

#ifdef DEBUG
#define	dprintf(x)	ldr_msg x
#else
#define	dprintf(x)
#endif /* DEBUG */

/*
 *	MACROS FOR INTEL 386 RELOCATION
 */

#define RELOC_TARGET_SIZE(r)	(((r)->ri_size_type & R_SIZESP) >> 12)
#define RELOC_EXTERN_P(r)	((r)->ri_flags & RI_SYMBOL_F)
#define RELOC_PCREL_P(r)	((r)->ri_size_type & 0x10)

/*
 * process a single relocation.  Intel 386 specific machine dependent code.
 * Patch the relocation target address with the relocation value.
 */
int
patch_reloc_addr(reloc_info_t *reloc_p, univ_t vaddr, univ_t mapaddr,
		 mo_long_t relocation)
{
	unsigned int mask;		/* relocation mask */
	char *loc;			/* location to relocate = target */
	int bytes;			/* 1, 2 or 4 byte relocation */
	int rc;				/* return code/status */


	loc = (char *) mapaddr;

	if (RELOC_PCREL_P(reloc_p))
		relocation -= (mo_long_t)vaddr;

	bytes = 1 << RELOC_TARGET_SIZE(reloc_p);

	/* mask for relocation */
	mask = 1 << (32 - 1);
	mask |= mask - 1;

	relocation &= mask;

	dprintf(("patch_reloc_addr: vaddr : 0x%x mapaddr : 0x%x bytes : %d mask : 0x%x relocation : 0x%x\n",
		 vaddr, mapaddr, bytes, mask, relocation));

	rc = LDR_SUCCESS;

	switch (RELOC_TARGET_SIZE(reloc_p))
	{
	      case 0:
		if (!RELOC_EXTERN_P(reloc_p))
			relocation += get_num(loc, bytes);
		if (RELOC_PCREL_P(reloc_p))
			relocation -= 1;
		*(char *) loc &= ~mask;
		*(char *) loc |= relocation;
		break;
	  
	      case 1:
		if (!RELOC_EXTERN_P(reloc_p))
			relocation += get_num(loc, bytes);
		if (RELOC_PCREL_P(reloc_p))
			relocation -= 2;
		*(short *) loc &= ~mask;
		*(short *) loc |= relocation;
		break;
	  
	      case 2:
		relocation += get_num(loc, bytes);
		if (RELOC_PCREL_P(reloc_p))
			relocation -= 4;
		*(long *) loc &= ~mask;
		*(long *) loc |= relocation;
		break;
	  
	      default:
		dprintf(("unknown relocation target size\n"));
		rc = LDR_ENOEXEC;
	} /* switch */

	return rc;
}

int get_num(buf, n)
     char *buf;
     int n;
{
  int val = 0;
  buf += (n - 1);
  for (; n > 0; n--)
    {
      val = val * 256 + (*buf-- & 0xff);
    }
  return val;
}
