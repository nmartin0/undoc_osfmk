/*
 * 
 * $Copyright
 * Copyright 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#if	ASMP
#include <mach_kdb.h>		/* MACH_KDB */
#include <kern/cpu_number.h>	/* master_cpu */
#include <i860paragon/baton.h>	/* struct baton */
#include <i860/db_machdep.h>	/* db_expr_t */
#include <ddb/db_output.h>	/* db_printf */

/*
 * kernel_baton definition
 * master_cpu is initial owner.
 */
struct baton kernel_baton = {master_cpu, 0};

#if	MACH_KDB
/*
 * ddb> machine baton
 */
/*ARGSUSED*/
void
db_show_baton(addr, have_addr, count, modif)
	db_expr_t	addr;
	int		have_addr;
	db_expr_t	count;
	char		*modif;
{
	db_printf("baton: %s cpu %d, level %d\n",
		kernel_baton.cpu == BATON_FREE ? "FREE" : "OWNED by",
		kernel_baton.cpu,
		kernel_baton.level);
}
#endif	/* MACH_KDB */

#endif	/* ASMP */
