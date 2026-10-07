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
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
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
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * $Id: db_interface.c,v 2.22 1995/02/04 00:57:20 stans Exp $
 */
/*
 * Machine-dependent support for kernel debugger.
 * Derived from i386.
 */

#include <cpus.h>
#include <platforms.h>
#include <time_stamp.h>

#include <kern/cpu_number.h>
#include <sys/reboot.h>
#include <vm/pmap.h>

#include <i860/thread.h>
#include <i860/db_machdep.h>
#include <i860/trap.h>
#include <i860/setjmp.h>
#include <i860/pmap.h>
#include <i860/psl.h>


#include <mach/vm_param.h>
#include <vm/vm_map.h>

#include <kern/thread.h>
#include <kern/task.h>
#include <kern/queue.h>

#include <ddb/db_variables.h>
#include <ddb/db_task_thread.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>
#include <ddb/db_run.h>

#if	PARAGON860
#include <i860paragon/led.h>
#else	PARAGON860
#define	RED_ON(x)
#define	RED_OFF(x)
#define	GREEN_ON(x)
#define	GREEN_OFF(x)
#endif	PARAGON860

int		db_active = 0;
unsigned	i860_last_kdb_sp[NCPUS];
int		kdb_debug = FALSE;

struct i860_saved_state	*i860_last_saved_statep[NCPUS];
struct i860_saved_state	i860_nested_saved_state[NCPUS];

extern thread_t	db_default_thread;

#ifdef DB_NO_AOUT
#include <i860/coff.h>
#include <i860/syms.h>
#include <i860/storclass.h>

extern char *esym;

ddb_init()
{
	extern end[];
	struct ldf_hdr_t *coff_hdr;

	coff_hdr = (struct ldf_hdr_t *) intel_round_page(end);
	X_db_sym_init(coff_hdr, esym, "mach", (char *)0);
}
#endif /* DB_NO_AOUT */

/*
 * Received keyboard interrupt sequence.
 */
kdb_kbd_trap(regs)
	struct i860_saved_state *regs;
{
#if 0
	if (db_active == 0 && (boothowto & RB_KDB)) {
	    db_printf("\n\nkernel: keyboard interrupt\n");
	    kdb_trap(-1, 0, regs);
	}
#endif
	if (db_active == 0) {
		kdb_trap(-1, 0, regs);
	}
#if	NCPUS > 1
	else {
		extern int	kdb_is_slave[NCPUS];

		if ( kdb_is_slave[ cpu_number() ] )
			kdb_trap(-1, 0, regs);
	}
#endif
}

extern char *	trap_type[];
extern int	TRAP_TYPES;

/*
 *  kdb_trap - field a KDB-keyboard interrupt, TRACE or BPT trap
 */

extern jmp_buf_t	*db_recover;

#if NCPUS > 1
int	db_first_cpu = -1;
decl_simple_lock_data(, db_first_cpu_lock);
#endif


kdb_trap(type, code, regs)
	int	type, code;
	register struct i860_saved_state *regs;
{
	int	s;
	int	cpun = cpu_number();
	extern	kdb_enter( int	mycpu );
	extern	kdb_leave( int	mycpu );

	s = sploff();

#if NCPUS > 1
	if (db_first_cpu == -1) {
		SIMPLE_LOCK(&db_first_cpu_lock);
		if (db_first_cpu == -1) {
			db_first_cpu = cpun;
		}
		SIMPLE_UNLOCK(&db_first_cpu_lock);
	}
#endif

	switch (type) {
	    case T_BPTFLT:	/* breakpoint */
	    case T_WATCHPOINT:	/* watchpoint */
	    case T_DEBUG:	/* single_step */
	    case -1:		/* keyboard interrupt */
		break;

	    default:
		if (db_recover) {
		    i860_nested_saved_state[cpun] = *regs;
		    db_printf("Caught ");
		    if (type > TRAP_TYPES)
			db_printf("type %d", type);
		    else
			db_printf("%s", trap_type[type]);
		    db_printf(" trap, code = %x, pc = %x\n",
			      code, regs->pc);
		    db_error("");
		    /*NOTREACHED*/
		}
		kdbprinttrap(type, code);
	}
	i860_last_saved_statep[cpun] = regs;
	i860_last_kdb_sp[cpun] = (unsigned) &type;

#if	NCPUS > 1
	if ( !kdb_enter(cpun) )
		goto kdb_exit;
#endif	NCPUS > 1

	/*  Should switch to kdb`s own stack here. */

	ddb_regs = *regs;

	if (kdb_debug)
		db_printf("{%d}trap-save: r1 0x%x sp 0x%x pc 0x%x\n",
			cpun,ddb_regs.r1,ddb_regs.sp,ddb_regs.pc);

	if ((regs->psr & PSR_PU) == 0) {
	    /*
	     * Kernel mode
	     */
	}

	db_active++;

#if NCPUS > 1
 	if (cpun == db_first_cpu)
#endif
	{
		cnpollc(TRUE);
		RED_ON(RED_GETC);	/* turn on the RED LED */
	}

	db_task_trap(type, code, (regs->psr & PSR_PU) != 0);

#if NCPUS > 1
	if (cpun == db_first_cpu)
#endif
	{
		cnpollc(FALSE);
#if NCPUS > 1
		db_first_cpu = -1;
#endif
		RED_OFF(RED_GETC);	/* turn off the RED LED */
	}

	db_active--;

	/*
	 * structure assignment
	 */
	*regs = ddb_regs;

	if (kdb_debug)
		db_printf("{%d}trap-rest: r1 0x%x sp 0x%x pc 0x%x\n",
			cpun,ddb_regs.r1,ddb_regs.sp,ddb_regs.pc);
#if	NCPUS > 1
kdb_exit:
	kdb_leave(cpun);
#endif	NCPUS > 1

	splon(s);
	return (1);
}


/*
 * Print trap reason.
 */
kdbprinttrap(type, code)
	int	type, code;
{
	db_printf("kernel: ");
	if (type > TRAP_TYPES)
	    db_printf("type %d", type);
	else
	    db_printf("%s", trap_type[type]);
	db_printf(" trap, code=%x\n", code);
}


int
db_user_to_kernel_address(task, addr, kaddr, flag)
	task_t		task;
	vm_offset_t	addr;
	unsigned	*kaddr;
	int		flag;
{
	register pt_entry_t *ptp;
	
	ptp = pmap_pte(task->map->pmap, addr);
	if (ptp == PT_ENTRY_NULL || (*ptp & INTEL_PTE_VALID) == 0) {
	    if (flag) {
		db_printf("\nno memory is assigned to address %08x\n", addr);
		db_error(0);
		/* NOTREACHED */
	    }
	    return(-1);
	}
	*kaddr = (unsigned)ptetokv(*ptp) + (addr & (INTEL_PGBYTES-1));
	return(0);
}


/*
 * Read bytes from kernel address space for debugger.
 */
void
db_read_bytes(addr, size, data, task)
	vm_offset_t	addr;
	register int	size;
	register char	*data;
	task_t		task;
{
	register char	*src;
	register n;
	unsigned	kern_addr;

	src = (char *)addr;
	if (addr >= VM_MIN_KERNEL_ADDRESS || task == TASK_NULL) {
	    if (task == TASK_NULL)
		task = db_current_task();
	    while (--size >= 0) {
		if (addr < VM_MIN_KERNEL_ADDRESS && task == TASK_NULL) {
		    db_printf("\nbad address %x\n", addr);
		    db_error(0);
		    /* NOTREACHED */
		}
		if (addr++ < VM_MIN_KERNEL_ADDRESS && 
			(current_thread()->state & TH_IDLE) ) {
		    db_printf("\nWarning: current thread is idle loop\n");
		    db_printf("address %x is not kernel address\n", addr);
		    db_error(0);
		    /* NOTREACHED */
		}
		*data++ = *src++;
	    }
	    return;
	}
	while (size > 0) {
	    if (db_user_to_kernel_address(task, addr, &kern_addr, 1) < 0)
		return;
	    src = (char *) kern_addr;
	    n = intel_trunc_page(addr+INTEL_PGBYTES) - addr;
	    if (n > size)
		n = size;
	    size -= n;
	    addr += n;
	    while (--n >= 0)
		*data++ = *src++;
	}
}

/*
 * Write bytes to kernel address space for debugger.
 */
void
db_write_bytes(addr, size, data, task)
	vm_offset_t	addr;
	register int	size;
	register char	*data;
	task_t		task;
{
	register char	*dst;

	register pt_entry_t *ptep0 = 0;
	pt_entry_t	oldmap0 = 0;
	vm_offset_t	addr1;
	register pt_entry_t *ptep1 = 0;
	pt_entry_t	oldmap1 = 0;
	extern char	etext;
	void		db_write_bytes_user_space();

	if ((addr < VM_MIN_KERNEL_ADDRESS) ^
	    ((addr + size) <= VM_MIN_KERNEL_ADDRESS)) {
	    db_error("\ncannot write data into mixed space\n");
	    /* NOTREACHED */
	}
	if (addr < VM_MIN_KERNEL_ADDRESS) {
	    if (task) {
		db_write_bytes_user_space(addr, size, data, task);
		return;
	    } else if (db_current_task() == TASK_NULL) {
		db_printf("\nbad address %x\n", addr);
		db_error(0);
		/* NOTREACHED */
	    }
	}

	if (addr >= VM_MIN_KERNEL_ADDRESS &&
	    addr <= (vm_offset_t)&etext)
	{
	    ptep0 = pmap_pte(kernel_pmap, addr);
	    oldmap0 = *ptep0;
	    *ptep0 |= INTEL_PTE_WRITE;

	    addr1 = i860_trunc_page(addr + size - 1);
	    if (i860_trunc_page(addr) != addr1) {
		/* data crosses a page boundary */

		ptep1 = pmap_pte(kernel_pmap, addr1);
		oldmap1 = *ptep1;
		*ptep1 |= INTEL_PTE_WRITE;
	    }
	    flush_tlb();
	}

	dst = (char *)addr;

	while (--size >= 0)
	    *dst++ = *data++;

	if (ptep0) {
	    *ptep0 = oldmap0;
	    if (ptep1) {
		*ptep1 = oldmap1;
	    }
	    flush_tlb();
	}
	flush();
}

void
db_write_bytes_user_space(addr, size, data, task)
	vm_offset_t	addr;
	register int	size;
	register char	*data;
	task_t		task;
{
	register char	*dst;
	register	n;
	unsigned	kern_addr;

	while (size > 0) {
	    if (db_user_to_kernel_address(task, addr, &kern_addr, 1) < 0)
		return;
	    dst = (char *)kern_addr;
	    n = intel_trunc_page(addr+INTEL_PGBYTES) - addr;
	    if (n > size)
		n = size;
	    size -= n;
	    addr += n;
	    while (--n >= 0)
		*dst++ = *data++;
	}
	flush();
}

boolean_t
db_check_access(addr, size, task)
	vm_offset_t	addr;
	register int	size;
	task_t		task;
{
	register	n;
	unsigned	kern_addr;

	if (addr >= VM_MIN_KERNEL_ADDRESS) {
	    if (kernel_task == TASK_NULL)
	        return(TRUE);
	    task = kernel_task;
	} else if (task == TASK_NULL) {
	    if (current_thread() == THREAD_NULL)
		return(FALSE);
	    task = current_thread()->task;
	}
	while (size > 0) {
	    if (db_user_to_kernel_address(task, addr, &kern_addr, 0) < 0)
		return(FALSE);
	    n = intel_trunc_page(addr+INTEL_PGBYTES) - addr;
	    if (n > size)
		n = size;
	    size -= n;
	    addr += n;
	}
	return(TRUE);
}

boolean_t
db_phys_eq(task1, addr1, task2, addr2)
	task_t		task1;
	vm_offset_t	addr1;
	task_t		task2;
	vm_offset_t	addr2;
{
	unsigned	kern_addr1, kern_addr2;

	if (addr1 >= VM_MIN_KERNEL_ADDRESS || addr2 >= VM_MIN_KERNEL_ADDRESS)
	    return(FALSE);
	if ((addr1 & (INTEL_PGBYTES-1)) != (addr2 & (INTEL_PGBYTES-1)))
	    return(FALSE);
	if (task1 == TASK_NULL) {
	    if (current_thread() == THREAD_NULL)
		return(FALSE);
	    task1 = current_thread()->task;
	}
	if (db_user_to_kernel_address(task1, addr1, &kern_addr1, 0) < 0
		|| db_user_to_kernel_address(task2, addr2, &kern_addr2, 0) < 0)
	    return(FALSE);
	return(kern_addr1 == kern_addr2);
}

#define DB_USER_STACK_ADDR	(VM_MIN_KERNEL_ADDRESS)
#define DB_NAME_SEARCH_LIMIT	(DB_USER_STACK_ADDR-(INTEL_PGBYTES*3))

static int
db_search_null(task, svaddr, evaddr, skaddr, flag)
	task_t		task;
	unsigned	*svaddr;
	unsigned	evaddr;
	unsigned	*skaddr;
	int		flag;
{
	register unsigned vaddr;
	register unsigned *kaddr;

	kaddr = (unsigned *)*skaddr;
	for (vaddr = *svaddr; vaddr > evaddr; vaddr -= sizeof(unsigned)) {
	    if (vaddr % INTEL_PGBYTES == 0) {
		vaddr -= sizeof(unsigned);
		if (db_user_to_kernel_address(task, vaddr, skaddr, 0) < 0)
		    return(-1);
		kaddr = (unsigned *)*skaddr;
	    } else {
		vaddr -= sizeof(unsigned);
		kaddr--;
	    }
	    if ((*kaddr == 0) ^ (flag  == 0)) {
		*svaddr = vaddr;
		*skaddr = (unsigned)kaddr;
		return(0);
	    }
	}
	return(-1);
}

void
db_task_name(task)
	task_t		task;
{
	register char *p;
	register n;
	unsigned vaddr, kaddr;

	vaddr = DB_USER_STACK_ADDR;
	kaddr = 0;

	/*
	 * skip nulls at the end
	 */
	if (db_search_null(task, &vaddr, DB_NAME_SEARCH_LIMIT, &kaddr, 0) < 0) {
	    db_printf(DB_NULL_TASK_NAME);
	    return;
	}
	/*
	 * search start of args
	 */
	if (db_search_null(task, &vaddr, DB_NAME_SEARCH_LIMIT, &kaddr, 1) < 0) {
	    db_printf(DB_NULL_TASK_NAME);
	    return;
	}

	n = DB_TASK_NAME_LEN-1;
	p = (char *)kaddr + sizeof(unsigned);
	for (vaddr += sizeof(int); vaddr < DB_USER_STACK_ADDR && n > 0; 
							vaddr++, p++, n--) {
	    if (vaddr % INTEL_PGBYTES == 0) {
		(void)db_user_to_kernel_address(task, vaddr, &kaddr, 0);
		p = (char*)kaddr;
	    }
	    db_printf("%c", (*p < ' ' || *p > '~')? ' ': *p);
	}
	while (n-- >= 0)	/* compare with >= 0 for one more space */
	    db_printf(" ");
}


/*
 * Branching calculations for single step
 */
unsigned getreg_val(r, regs)
	int		r;
	db_regs_t	*regs;
{
	return ((unsigned *)(&regs->r0))[r];
}

int db_branch_delay = 0;

unsigned
branch_taken(inst, pc, getreg_val, regs)
	unsigned long	inst;
	unsigned long	pc;
	unsigned 	(*getreg_val)();
	db_regs_t	*regs;
{
	long		t;

	switch (inst & 0xfc000000) {

	case 0x40000000:	/* bri */
	case 0x4c000000:	/* calli */
		db_branch_delay = 1;
		return getreg_val((inst >> 11) & 0x1f, regs);


	case 0x50000000:	/* btne, src1 is reg */
	case 0x54000000:	/* btne, src1 is immediate */
	case 0x58000000:	/* bte (src1 is reg) */
	case 0x5c000000:	/* bte (src1 is immediate) */
		db_branch_delay = 0;
		t = ((inst >> 5) & 0xf800) | (inst & 0x07ff);
		if (t & 0x8000)
			t |= 0xffff0000; /* sign extend */
		return pc + 4*t + 4;

	case 0xb4000000:	/* bla */
		db_branch_delay = 1;
		t = ((inst >> 5) & 0xf800) | (inst & 0x07ff);
		if (t & 0x8000)
			t |= 0xffff0000; /* sign extend */
		return pc + 4*t + 4;

	case 0x68000000:	/* br */
	case 0x6c000000:	/* call */
	case 0x74000000:	/* bc.t */
	case 0x7c000000:	/* bnc.t */
		db_branch_delay = 1;
		t = inst & 0x03ffffff;
		if (t & 0x02000000)
			t |= 0xfc000000; /* sign extend */
		return pc + 4*t + 4;

	case 0x70000000:	/* bc */
	case 0x78000000:	/* bnc */
		db_branch_delay = 0;
		t = inst & 0x03ffffff;
		if (t & 0x02000000)
			t |= 0xfc000000; /* sign extend */
		return pc + 4*t + 4;
	}
	return pc;
}

next_instr_address(pc, br)
	unsigned	pc;
	int		br;
{

	if (br && db_branch_delay)
		return pc+8;

	return pc+4;
}


isa_bri(inst)
	unsigned long	inst;
{
	return ((inst & 0xfc000000) == 0x40000000);
}


isa_rei(inst)
	unsigned long	inst;
{
	return isa_bri(inst);
}


isa_ret(inst)
	unsigned long	inst;
{
	return isa_bri(inst);
}


isa_calli(inst)
	unsigned long	inst;
{
	return ((inst & 0xfc00001f) == 0x4c000002);
}


isa_call(inst)
	unsigned long	inst;
{
	return ((inst & 0xfc000000) == 0x6c000000) || isa_calli(inst);
}


isa_bte(inst)
	unsigned long	inst;
{
	unsigned long	op;

	op = (inst & 0xfc000000);
	return	(op == 0x58000000) ||	/* bte, src1 is reg */
		(op == 0x5c000000);	/* bte, src1 is immediate */
}


isa_btne(inst)
	unsigned long	inst;
{
	unsigned long	op;

	op = (inst & 0xfc000000);
	return	(op == 0x50000000) ||	/* btne, src1 is reg */
		(op == 0x54000000);	/* btne, src1 is immediate */
}


isa_bla(inst)
	unsigned long	inst;
{
	return ((inst & 0xfc000000) == 0xb4000000);
}


isa_br(inst)
	unsigned long	inst;
{
	return ((inst & 0xfc000000) == 0x68000000);
}


isa_bc(inst)
	unsigned long	inst;
{
	return ((inst & 0xfc000000) == 0x70000000);
}


isa_bct(inst)
	unsigned long	inst;
{
	return ((inst & 0xfc000000) == 0x74000000);
}


isa_bnc(inst)
	unsigned long	inst;
{
	return ((inst & 0xfc000000) == 0x78000000);
}


isa_bnct(inst)
	unsigned long	inst;
{
	return ((inst & 0xfc000000) == 0x7c000000);
}

isa_branch(inst)
	unsigned long	inst;
{
	return	isa_bri(inst)	||
		isa_bte(inst)	||
		isa_btne(inst)	||
		isa_bla(inst)	||
		isa_br(inst)	||
		isa_bc(inst)	||
		isa_bct(inst)	||
		isa_bnc(inst)	||
		isa_bnct(inst);
}


inst_unconditional_flow_transfer(inst)
	unsigned long	inst;
{
	return	isa_bri(inst)	||
		isa_call(inst)	||
		isa_calli(inst)	||
		isa_br(inst);
}


isa_load(inst)
	unsigned long	inst;
{
	switch (inst & 0xfc000000) {
	case 0x00000000:	/* ld.x */
	case 0x04000000:	/* ld.x */
	case 0x10000000:	/* ld.x */
	case 0x14000000:	/* ld.x */
	case 0x20000000:	/* fld.x */
	case 0x24000000:	/* fld.x */
	case 0x30000000:	/* ld.c */
		return 1 == 1;
	}
	return 1 == 0;
}


isa_store(inst)
	unsigned long	inst;
{
	switch (inst & 0xfc000000) {
	case 0x0c000000:	/* st.x */
	case 0x1c000000:	/* st.x */
	case 0x28000000:	/* fst.x */
	case 0x2c000000:	/* fst.x */
	case 0x38000000:	/* st.c */
	case 0x3c000000:	/* pst.d */
		return 1 == 1;
	}
	return 1 == 0;
}

db_print_thread_pcb(th)
	thread_t	th;
{
	struct i860_kernel_state	*ks;
	struct i860_exception_link	*el;
	struct i860_saved_state		*ss;

	db_printf("k  ");
	if (th->kernel_stack) {
		ks = STACK_I860KS(th->kernel_stack);
		el = STACK_I860EL(th->kernel_stack);
		ss = el->eframe;
		db_printf("ks=%x ss=%x sp=%x fp=%x r1=",
			ks,
			ss,
			ks->ks_sp,
			ks->ks_fp);
		db_printsym((db_addr_t)ks->ks_r1, DB_STGY_ANY);
		db_printf("\n");
	} else {
		db_printf("[no kernel stack]\n");
		if (th->pcb == 0) {
			ss = 0;
			db_printf("[no pcb]\n");
		} else {
			ss = USER_REGS(th);
		}
	}
	if (ss != 0) {
		db_printf("u  ss=%x psr=%x sp=%x fp=%x dirbase=%x pc=",
			ss,
			ss->psr,
			ss->sp,
			ss->fp,
			ss->dirbase);
		db_printsym((db_addr_t)ss->pc, DB_STGY_ANY);
		db_printf("\n");
		db_printf("current_thread(): 0x%x\n", current_thread());
	}
}

#if	NCPUS > 1
/*
 * Code used to synchronize kdb among all cpus, one active at a time, switch
 * from on to another using kdb_on! #cpu or cpu #cpu
 */

decl_simple_lock_data(, kdb_lock);	/* kdb lock			*/

volatile int	kdb_cpu = -1;		/* current cpu controlling kdb	*/

int		kdb_is_slave[NCPUS];
int		kdb_active[NCPUS];

/*
 * Called when entering kdb:
 * Takes kdb lock. If if we were called remotly (slave state) we just
 * wait for kdb_cpu to be equal to cpu_number(). Otherwise enter kdb if
 * not active on another cpu
 */

kdb_enter( int	mycpu )
{
	int		master = 0;
	extern		lock_kdb(int mycpu);
	extern		unlock_kdb(int mycpu);

	kdb_active[mycpu]++;
	lock_kdb(mycpu);

	if ( ! kdb_is_slave[mycpu] ) {
		/*
		 * at this jucture I own the kdb_lock. Other processor(s) are
		 * spinning in lock_kdb() on kdb_cpu and when that changes
		 * they will spin on the kdb_lock.
		 */
		master = 1;	/* in control.... */
	}

	if (kdb_debug)
		db_printf("{%d}kdb_enter: master %d, kdb_cpu %d, run mode %d\n",
				mycpu, master, kdb_cpu, db_run_mode);

	if (kdb_cpu == -1 && master) {
		remote_kdb();	/* notify/stop other cpus */
		kdb_cpu = mycpu;
		return(1);
	}
	else if (kdb_cpu == mycpu)
		return(1);
	else
		return(0);
}

kdb_leave( int	mycpu )
{
	if ( kdb_is_slave[mycpu] ) {
		kdb_is_slave[mycpu]--;
	}
	else if (db_run_mode == STEP_CONTINUE)
		kdb_cpu = -1;

	if (kdb_debug)
		db_printf("{%d}kdb_leave: kdb_cpu %d, run_mode %d\n",
				mycpu, kdb_cpu, db_run_mode);

	unlock_kdb(mycpu);
	kdb_active[mycpu]--;
}

/*
 * lock KDB.
 *	processor(s) which are not the master will spin here until released
 *	by the master KDB processor.
 *
 * implicit outputs:
 *	Master KDB processor exits this routine holding the kdb_lock.
 */
int		in_lock_kdb[NCPUS];
static volatile int	kdb_lock_owned = 0;

lock_kdb( int my_cpu )
{
	register	i;

	in_lock_kdb[my_cpu]++;
	GREEN_OFF(GREEN_KDB);
	for(;;) {
		/*
		 * slave processors spin here until released.
		 */
		if (kdb_cpu != -1 && kdb_cpu != my_cpu) {
			continue;
		}

		/* kdb master owns this lock */
		for (;;) {
			if (!kdb_lock_owned) {
				SIMPLE_LOCK(&kdb_lock);
				if (!kdb_lock_owned) {
					++kdb_lock_owned;
					SIMPLE_UNLOCK(&kdb_lock);
					break;
				}
				SIMPLE_UNLOCK(&kdb_lock);
			}
		}

		if (kdb_cpu == -1 || kdb_cpu == my_cpu)
			break;

		kdb_lock_owned = 0;
	} 
	in_lock_kdb[my_cpu]--;
	GREEN_ON(GREEN_KDB);
}

#if	TIME_STAMP
extern unsigned old_time_stamp;
#endif	TIME_STAMP

unlock_kdb( int cpun )
{
	kdb_lock_owned = 0;
#if	TIME_STAMP
	old_time_stamp = 0;
#endif	TIME_STAMP
	GREEN_OFF(GREEN_KDB);
}


#ifdef	__STDC__
#define KDB_SAVE(type, name) extern type name; type name##_save = name
#define KDB_RESTORE(name) name = name##_save
#else	__STDC__
#define KDB_SAVE(type, name) extern type name; type name/**/_save = name
#define KDB_RESTORE(name) name = name/**/_save
#endif	__STDC__

#define KDB_SAVE_CTXT() \
	KDB_SAVE(int, db_run_mode); \
	KDB_SAVE(boolean_t, db_sstep_print); \
	KDB_SAVE(int, db_loop_count); \
	KDB_SAVE(int, db_call_depth); \
	KDB_SAVE(int, db_inst_count); \
	KDB_SAVE(int, db_last_inst_count); \
	KDB_SAVE(int, db_load_count); \
	KDB_SAVE(int, db_store_count); \
	KDB_SAVE(boolean_t, db_cmd_loop_done); \
	KDB_SAVE(jmp_buf_t *, db_recover); \
	KDB_SAVE(db_addr_t, db_dot); \
	KDB_SAVE(db_addr_t, db_last_addr); \
	KDB_SAVE(db_addr_t, db_prev); \
	KDB_SAVE(db_addr_t, db_next); \
	KDB_SAVE(db_regs_t, ddb_regs); 

#define KDB_RESTORE_CTXT() \
	KDB_RESTORE(db_run_mode); \
	KDB_RESTORE(db_sstep_print); \
	KDB_RESTORE(db_loop_count); \
	KDB_RESTORE(db_call_depth); \
	KDB_RESTORE(db_inst_count); \
	KDB_RESTORE(db_last_inst_count); \
	KDB_RESTORE(db_load_count); \
	KDB_RESTORE(db_store_count); \
	KDB_RESTORE(db_cmd_loop_done); \
	KDB_RESTORE(db_recover); \
	KDB_RESTORE(db_dot); \
	KDB_RESTORE(db_last_addr); \
	KDB_RESTORE(db_prev); \
	KDB_RESTORE(db_next); \
	KDB_RESTORE(ddb_regs); 

/*
 * switch to another cpu
 */

kdb_on(int cpu)
{
	int	cpun=cpu_number();
	KDB_SAVE_CTXT();

	if ( cpu == cpun ) {
		db_printf("already on cpu %d\n",cpu);
		return 1;
	}

	if (cpu < 0 || cpu >= NCPUS || !kdb_active[cpu]) {
		db_printf("invalid cpu # '%d' or not an active cpu.\n",cpu);
		return 1;
	}

	if (kdb_is_slave[cpu])		/* new KDB cpu is no longer a slave */
		kdb_is_slave[cpu]--;

	kdb_is_slave[cpu_number()]++;	/* old KDB cpu becomes a slave */

	kdb_cpu = cpu;	/* set a new KDB cpu */

	unlock_kdb(cpun);	/* new KDB cpu will now exit from kdb_lock() */

	lock_kdb(cpun);		/* old KDB cpu will hang in kdb_lock() */

	KDB_RESTORE_CTXT();

	if (kdb_cpu == -1) /* someone continued */
		db_continue_cmd(0, 0, 0, &"");

	return 0;
}

#endif	NCPUS > 1

/*
 * assuming the 'pc' is at the start of a function then output the function name
 * and the arguments (1st five) in registers [r16...r20].
 *
 * Format for "machine_arg_names[]":
 *
 *	1st entry is the name of the "pc" register.
 *	2..MAX are names of where the arguments are stored.
 */

char	*machine_arg_names[] =
	{	"pc",
		"r16",
		"r17",
		"r18",
		"r19",
		"r20",
		"r21",
		"r22",
		"r23",
		"r24",
		"r25",
		"r26",
		"r27",
		"r28",
		"r29",
		(char *)0
	};

/* ARGSUSED */
void
db_show_machine_args(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char		*modif;
{
	register db_variable_t	regp;
	db_expr_t		value;
	char			*name;
	task_t			task = TASK_NULL;
	char			**arg;
	int			argc;
	struct db_var_aux_param	aux_param;

	extern db_variable_t	db_find_reg_name();

	aux_param.modif = modif;
	aux_param.thread = THREAD_NULL;

	if (db_option(modif, 't')) {
	    if (have_addr) {
		if (!db_check_thread_address_valid((thread_t)addr))
		    return;
		aux_param.thread = (thread_t)addr;
	    } else
	        aux_param.thread = db_default_thread;
	    if (aux_param.thread != THREAD_NULL)
		task = aux_param.thread->task;
	}

	/*
	 * make sure we have valid task and thread pointers.
	 */
	if ( aux_param.thread == THREAD_NULL ) {
		if ((aux_param.thread = db_default_thread) == THREAD_NULL) {
			if ((aux_param.thread=current_thread()) == THREAD_NULL){
				db_error("no current_thread()?\n");
				/*NOTREACHED*/
			}
        	}
	}
	if ( task == TASK_NULL )
		task = aux_param.thread->task;

	/*
	 * output the name of the function we are in (aka 'pc').
	 */
	if ( (regp = db_find_reg_name( *machine_arg_names )) == DB_VAR_NULL ) {
		db_printf(" unknown pc name '%s'?\n", *arg);
		return;
	}

	aux_param.level = regp->max_level;
	aux_param.suffix[0] = regp->low;

        db_read_write_variable(regp, &value, DB_VAR_GET, &aux_param);

        db_task_printsym( value, DB_STGY_PROC, task );
	db_printf("( ");

	/*
	 * output function arg values derrived from the register set, actuals
	 * spilled into memory are not handled. "r29" points to arg memory.
	 */
	if ( (argc = db_count_func_args( task, value )) > 14 )
		argc = 14;

	for( arg = &machine_arg_names[1]; (*arg && (argc > 0)); arg++,argc-- ) {
	    int	i;

	    if ( (regp = db_find_reg_name( *arg )) == DB_VAR_NULL ) {
		db_printf(" unknown argument name '%s'?\n", *arg);
		continue;
	    }

	    if (regp->max_level > 1) {
		db_printf("bad multi-suffixed register %s\n", regp->name);
		continue;
	    }

	    if ( arg != &machine_arg_names[1] )
	   	db_printf(", ");

	    aux_param.level = regp->max_level;

	    for (i = regp->low; i <= regp->high; i++) {
		aux_param.suffix[0] = i;
	        db_read_write_variable(regp, &value, DB_VAR_GET, &aux_param);
		db_printf("0x%x", value);
	    }
	}
	db_printf(" )\n");
}
