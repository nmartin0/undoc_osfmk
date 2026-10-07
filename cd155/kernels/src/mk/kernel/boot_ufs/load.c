/*
 * 
 * $Copyright
 * Copyright 1991, 1994, 1995 Intel Corporation
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
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
 * HISTORY
 * $Log: load.c,v $
 * Revision 1.11  1994/11/18  20:25:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/07/12  19:16:48  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.9.2.1  1994/03/11  23:06:10  andyp
 * Corrected the use of varargs to be more portable.
 *
 * Revision 1.9  1993/12/14  22:56:32  steved
 * Removed the check for 'executable' in the server and emulator
 * files when loading.
 *
 * Revision 1.8  1993/07/19  17:54:45  stans
 *    map_preloaded_program_file() can load a server using 8K VM pages where
 *    the EOT (End-Of_Text) can end up in the same VM page as the SOD
 *    (Start-Of-Data). Adjust assert() ("<" vs. "<=)" to allow for this case.
 *    Infamous boundary conditions.
 *
 * Revision 1.7  1993/06/30  22:16:23  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.6  1993/05/24  22:13:54  stans
 *    Removed (gone at last) device forwarding.
 *
 * Revision 1.5  1993/04/27  20:13:56  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:13:30  dleslie
 * First R1_0 release
 *
 * Revision 0.3  1993/04/07  17:24:26  SSD
 * changed the defaults for the BOTTOT_LOAD)_SYMBOLS so that the
 * assertfull ckernel load symbols by default.
 *
 * Revision 0.2  1993/04/05  01:56:36  andyp
 * On Paragons, added bootmagic control for optionally preserving
 * the server's and emulator's symbol tables.  If compiled without
 * asserts, symbols aren't loaded by default.  If compiled with
 * asserts, symbols are loaded.  BOOT_LOAD_SYMBOLS is the bootmagic
 * variable.
 *
 * Revision 0.1  1993/04/05  01:51:06  andyp
 * Added to RCS tree.
 *
 * Revision 2.14.5.1  92/04/30  11:44:50  bernadat
 * 	Test null pointer for boot_string in boot_load_program()
 * 	[92/04/24            bernadat]
 * 
 * 		Pass boot arguments to the server, must be a string
 * 		of space/tab separated arguments/words
 * 	[92/03/19            bernadat]
 * 
 * Revision 2.14  91/09/04  11:27:54  jsb
 * 	Fixed a printf in read_symtab_from_file (following failure of
 * 	get_symtab) by not printing unset variable ``result''.
 * 	[91/09/01  07:48:45  jsb]
 * 
 * Revision 2.13  91/08/28  11:09:49  jsb
 * 	Fixed to check vm_allocate return codes, esp in read_symtab_from_file.
 * 	[91/08/26            rpd]
 * 
 * Revision 2.12  91/07/31  17:25:29  dbg
 * 	Replace blkclr with bzero.
 * 	[91/07/26            dbg]
 * 
 * 	Moved read_symtab_from_file here and made independent of load
 * 	format.
 * 	[91/06/25            dbg]
 * 
 * 
 * Revision 2.11  91/05/14  15:23:41  mrt
 * 	Correcting copyright
 * 
 * Revision 2.10  91/03/16  14:41:37  rpd
 * 	Fixed read_emulator_symbols to not panic if there is no emulator.
 * 	[91/03/13            rpd]
 * 
 * Revision 2.9  91/02/05  17:01:45  mrt
 * 	Changed to new copyright
 * 	[91/01/28  14:55:09  mrt]
 * 
 * Revision 2.8  90/10/25  14:42:05  rwd
 * 	Modified boot_load_program and read_emulator_symbols
 * 	to allocate arrays from heap, not stack.
 * 	[90/10/23            rpd]
 * 
 * Revision 2.7  90/08/27  21:46:02  dbg
 * 	Reduce lint.
 * 	[90/08/13            dbg]
 * 
 * 	Use new error codes.  Use new file_io package.
 * 	[90/07/16            dbg]
 * 
 * Revision 2.6  90/06/02  14:45:31  rpd
 * 	Added read_emulator_symbols.
 * 	[90/05/11  17:08:19  rpd]
 * 
 * 	From jsb: to support debugging, read in the server's symbol table.
 * 	[90/04/23            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  21:31:33  rpd]
 * 
 * Revision 2.5  90/01/11  11:41:20  dbg
 * 	Use bootstrap-task print routines.
 * 	[89/12/20            dbg]
 * 
 * Revision 2.4  89/11/14  10:28:13  dbg
 * 	Restore read-only protection for text segment.
 * 	[89/11/01            dbg]
 * 
 * Revision 2.3  89/09/08  11:22:13  dbg
 * 	Use new open_file routine.  Change 'struct inode' to
 * 	'struct file' for new stand-alone IO package.
 * 	[89/08/31            dbg]
 * 
 * 26-May-89  Randall Dean (rwd) at Carnegie-Mellon University
 *	Fixed separate text/data case.  Added copyright to version
 *	generated by dbg
 *
 */
#include <mach_kdb.h>
#include <mach_assert.h>

#include <kern/kalloc.h>
#include <boot_ufs/file_io.h>
#include <boot_ufs/loader_info.h>

#include <mach/mach_traps.h>
#include <mach/mach_interface.h>
#include <mach/mach_user_internal.h>
#include <mach/mach_port_internal.h>
#include <boot_ufs/boot_printf.h>

#include <sys/varargs.h>

#if	MACH_KDB
#include <mach/mach_debug_interface.h>
#include <mach/mach_debug_internal.h>
#endif

/*
 * Loader for a.out format.
 * Loads the file into the user space of the specified task,
 * and sets up the starting thread's registers.
 */

extern char *set_regs();
extern int  ex_get_header();

boolean_t	load_protect_text = TRUE;

#if	PARAGON860

int map_preloaded_program_file(prog_base,
				user_task,
				user_thread,
				arg_size,
				arg_pos_p)
	unsigned char	*prog_base;
	mach_port_t	user_task;
	mach_port_t	user_thread;
	int		arg_size;
	char		**arg_pos_p;
{
	struct loader_info	lp;
	kern_return_t		kr;
	vm_offset_t		text_page_start,
				text_page_end,
				data_page_start,
				data_page_end,
				area_start;
	vm_size_t		area_size;

	kr = ex_get_preloaded_header(prog_base, &lp);	/* XXX */
	assert(kr == 0);

	text_page_start = trunc_page(lp.text_start);
	text_page_end   = round_page(lp.text_start + lp.text_size);
	data_page_start = trunc_page(lp.data_start);
	data_page_end   = round_page(lp.data_start + lp.data_size
					+ lp.bss_size);

	/*
	 *	XXX We don't yet support executables with one big chunk
	 *	XXX of text and data...
	 */
	assert(text_page_end <= data_page_start);

	/*
	 *	text
	 */
	area_size = (vm_size_t) (text_page_end - text_page_start);
	kr = vm_allocate(mach_task_self(), &area_start, area_size, TRUE);
	assert(kr == 0);

	kr = vm_allocate(user_task, &text_page_start, area_size, FALSE);
	assert(kr == 0);

	bcopy(prog_base + lp.text_offset, area_start, lp.text_size);

	kr = vm_write(user_task, text_page_start,
			(pointer_t) area_start, area_size);
	assert(kr == 0);

	kr = vm_deallocate(mach_task_self(), area_start, area_size);
	assert(kr == 0);

	/*
	 *	data + bss
	 */
	area_size = (vm_size_t) (data_page_end - data_page_start);
	kr = vm_allocate(mach_task_self(), &area_start, area_size, TRUE);
	assert(kr == 0);

	bzero(area_start + (lp.data_start + lp.data_size - data_page_start),
			lp.bss_size);
	bcopy(prog_base + lp.data_offset,
			area_start + (lp.data_start - data_page_start),
			lp.data_size);

	kr = vm_allocate(user_task, &data_page_start, area_size, FALSE);
	assert(kr == 0);

	kr = vm_write(user_task, data_page_start,
		(pointer_t) area_start, area_size);
	assert(kr == 0);

	kr = vm_deallocate(mach_task_self(), area_start, area_size);
	assert(kr == 0);


	/*
	 *	protect the text.
	 */
	if (load_protect_text) {
		kr = vm_protect(user_task,
			    text_page_start,
			    (vm_size_t)(trunc_page(lp.text_start+lp.text_size)
					- text_page_start),
			    FALSE,
			    VM_PROT_READ|VM_PROT_EXECUTE);
		assert(kr == 0);
	}


	/*
	 *	Set up the stack and user registers.
	 */
	*arg_pos_p = set_regs(user_task, user_thread, &lp, arg_size);

	return (KERN_SUCCESS);
}

#endif	PARAGON860


int load_program_file(fp, user_task, user_thread, arg_size, arg_pos_p)
	register struct file *	fp;
	mach_port_t		user_task;
	mach_port_t		user_thread;
	int			arg_size;
	char *			*arg_pos_p;	/* out */
{
	struct loader_info	lp;
	kern_return_t		result;
	vm_offset_t		text_page_start,
				text_page_end,
				data_page_start,
				data_page_end,
				area_start;
	vm_size_t		resid;

	/*
	 * It was a real pain in the backside to remember to set the 'x' bit
	 * in the filemode of the server/emulator. So, we don't check this
	 * anymore.
	 */
#if 0
	if ((fp->i_mode & IFMT) != IFREG ||
	    (fp->i_mode & (IEXEC|IEXEC>>3|IEXEC>>6)) == 0)
		return (EX_NOT_EXECUTABLE);
#else
	if ((fp->i_mode & IFMT) != IFREG)
		return (EX_NOT_EXECUTABLE);
#endif

	/*
	 * Read in the header and get the file pointers.
	 */
	result = ex_get_header(fp, &lp);
	if (result)
		return (result);

	/*
	 * Allocate space.
	 */
	text_page_start = trunc_page(lp.text_start);
	text_page_end   = round_page(lp.text_start + lp.text_size);
	data_page_start = trunc_page(lp.data_start);
	data_page_end   = round_page(lp.data_start + lp.data_size
					+ lp.bss_size);

	if (text_page_end >= data_page_start) {
	    /*
	     * One contiguous area for text and data.
	     */
	    result = vm_allocate(mach_task_self(),
				&area_start,
				(vm_size_t)(data_page_end - text_page_start),
				TRUE);
	    if (result)
		return (result);

	    result = read_file(fp,
				lp.text_offset,
				area_start + (lp.text_start - text_page_start),
				lp.text_size,
				&resid);
	    if (result)
		return (result);
	    if (resid)
		return (EX_NOT_EXECUTABLE);

	    result = read_file(fp,
				lp.data_offset,
				area_start + (lp.data_start - text_page_start),
				lp.data_size,
				&resid);
	    if (result)
		return (result);
	    if (resid)
		return (EX_NOT_EXECUTABLE);

	    bzero((char *)(area_start +
				(lp.data_start + lp.data_size
					 - text_page_start)),
				lp.bss_size);

	    result = vm_allocate(user_task,
				&text_page_start,
				(vm_size_t)(data_page_end - text_page_start),
				FALSE);
	    if (result)
		return (result);

	    result = vm_write(user_task,
				text_page_start,
				(pointer_t)area_start,
				(vm_size_t)(data_page_end - text_page_start));
	    if (result)
		return (result);

	    result = vm_deallocate(mach_task_self(),
				area_start,
				(vm_size_t)(data_page_end - text_page_start));
	    if (result)
		return (result);

	}
	else {
	    /*
	     * Separated text and data areas.
	     */
	    /*
	     * First read in the text
	     */
	    result = vm_allocate(mach_task_self(),
				&area_start,
				(vm_size_t)(text_page_end - text_page_start),
				TRUE);
	    if (result)
		return (result);

	    result = read_file(fp,
				lp.text_offset,
				area_start + (lp.text_start - text_page_start),
				lp.text_size,
				&resid);
	    if (result)
		return (result);
	    if (resid)
		return (EX_NOT_EXECUTABLE);

	    result = vm_allocate(user_task,
				&text_page_start,
				(vm_size_t)(text_page_end - text_page_start),
				FALSE);
	    if (result)
		return (result);

	    result = vm_write(user_task,
				text_page_start,
				(pointer_t)area_start,
				(vm_size_t)(text_page_end - text_page_start));
	    if (result)
		return (result);

	    result = vm_deallocate(mach_task_self(),
				area_start,
				(vm_size_t)(text_page_end - text_page_start));
	    if (result)
		return (result);

	    /*
	     * Then the data
	     */
	    result = vm_allocate(mach_task_self(),
				&area_start,
				(vm_size_t)(data_page_end - data_page_start),
				TRUE);
	    if (result)
		return (result);

	    result = read_file(fp,
				lp.data_offset,
				area_start + (lp.data_start - data_page_start),
				lp.data_size,
				&resid);
	    if (result)
		return (result);
	    if (resid)
		return (EX_NOT_EXECUTABLE);

	    bzero((char *)(area_start + 
				(lp.data_start + lp.data_size
					- data_page_start)),
		   lp.bss_size);

	    result = vm_allocate(user_task,
				&data_page_start,
				(vm_size_t)(data_page_end - data_page_start),
				FALSE);
	    if (result)
		return (result);

	    result = vm_write(user_task,
				data_page_start,
				(pointer_t)area_start,
				(vm_size_t)(data_page_end - data_page_start));
	    if (result)
		return (result);

	    result = vm_deallocate(mach_task_self(),
				area_start,
				(vm_size_t)(data_page_end - data_page_start));
	    if (result)
		return (result);

	}
	/*
	 * Protect the text.
	 */
	if (load_protect_text) {
	    result = vm_protect(user_task,
			    text_page_start,
			    (vm_size_t)(trunc_page(lp.text_start+lp.text_size)
					- text_page_start),
			    FALSE,
			    VM_PROT_READ|VM_PROT_EXECUTE);
	    if (result)
		return (result);
	}

	/*
	 * Set up the stack and user registers.
	 */
	*arg_pos_p = set_regs(user_task, user_thread, &lp, arg_size);

	return (KERN_SUCCESS);
}

#define	isspace(c)	((c) == ' ' || (c) == '\t')


int boot_load_program(va_alist)
	va_dcl
{
	va_list			argv_ptr;
	char *			arg_ptr;

	mach_port_t		master_host_port;
	mach_port_t		master_device_port;
	mach_port_t		user_task;
	mach_port_t		user_thread;
	mach_port_t		dummy_port;
	char			*rootname;
	char			*boot_string;
	char			*dummy_char;

	int			arg_len;
	int			arg_count;
	int			boot_arg_count;
	char *			arg_pos;
	unsigned int		arg_item_len;

	kern_return_t		result;
	struct file		file;
	char *			file_name;

	char *			namebuf;
#if	PARAGON860
	boolean_t	server_preloaded;
	unsigned char	*server_base;
	unsigned long	server_size;
#endif	PARAGON860

	extern char *	strbuild();

	/*
	 * Get the target file name
	 */

	va_start(argv_ptr);
	master_host_port   = va_arg(argv_ptr, mach_port_t);
	master_device_port = va_arg(argv_ptr, mach_port_t);
	user_task	   = va_arg(argv_ptr, mach_port_t);
	user_thread        = va_arg(argv_ptr, mach_port_t);
	rootname           = va_arg(argv_ptr, char *);
	boot_string        = va_arg(argv_ptr, char *);
	file_name	   = va_arg(argv_ptr, char *);
	va_end(argv_ptr);

	/*
	 * Build file name: "/dev/$rootname/$filename"
	 */
	namebuf = (char *) kalloc(MAXPATHLEN+1);
	(void) strbuild(namebuf, "/dev/",
				 rootname,
				 "/",
				 file_name,
				 (char *)0);

	/*
	 * Open the file
	 */
	bzero((char *)&file, sizeof(file));

#if	PARAGON860
	/*
	 *	XXX don't open the files if they have
	 *	XXX been preloaded.
	 */
	server_preloaded = preloaded_program_present("server",
				&server_base, &server_size);
	if (server_preloaded == FALSE) {
		result = open_file(master_device_port, namebuf, &file);
		if (result != 0) {
			panic("boot_load_program: unable to open server file");
		}
	}
#else	PARAGON860
	result = open_file(master_device_port, namebuf, &file);
	if (result != 0) {
	    panic("boot_load_program: unable to open server file");
	}
#endif	PARAGON860

	/*
	 * Calculate the size of the boot argument list.
	*
	 * first process the arguments passed at boot prompt.
	 * It is a list of arguments/words separated by spaces/tabs.
	 * This list is ignored if null.
	 * While counting them, null terminate them
	 */

	boot_arg_count = 0;
	arg_ptr = boot_string;
	arg_len = 0;

	if (arg_ptr) while (*arg_ptr) {
	  	char *temp_ptr;
		while (*arg_ptr && isspace(*arg_ptr))
			*arg_ptr++ = '\0';
		if (*(temp_ptr = arg_ptr))
			boot_arg_count++;
		while (*arg_ptr && !isspace(*arg_ptr))
			arg_ptr++;
	    	arg_len += strlen(temp_ptr) + 1;	/* space for '\0' */
	}
		
	
	/*
	 * Calculate the size of the argument list.
         *
	 * Now process the var arg list passed as arguments
	 */

	va_start(argv_ptr);
	dummy_port = va_arg(argv_ptr, mach_port_t);	/* master_host_port */
	dummy_port = va_arg(argv_ptr, mach_port_t);	/* master_device_port */
	dummy_port = va_arg(argv_ptr, mach_port_t);	/* user_task */
	dummy_port = va_arg(argv_ptr, mach_port_t);	/* user_thread */
	dummy_char = va_arg(argv_ptr, char *);		/* rootname */
	dummy_char = va_arg(argv_ptr, char *);		/* boot_string */
	arg_count = boot_arg_count;			
	for (;;) {
	    arg_ptr = va_arg(argv_ptr, char *);
	    if (arg_ptr == (char *)0)
		break;
	    arg_count++;
	    arg_len += strlen(arg_ptr) + 1;	/* space for '\0' */
	}
	va_end(argv_ptr);

	/*
	 * Add space for:
	 *    arg_count
	 *    pointers to arguments
	 *    trailing 0 pointer
	 *    dummy 0 pointer to environment variables
	 *    and align to integer boundary
	 */
	arg_len += sizeof(int) + (2 + arg_count) * sizeof(char *);
	arg_len = (arg_len + (sizeof(int) - 1)) & ~(sizeof(int)-1);

#if	PARAGON860
	if (server_preloaded) {
		result = map_preloaded_program_file(server_base,
				user_task, user_thread,
				arg_len, &arg_pos);
		if (result) {
			panic("map_preloaded_program_file %d", result);
		}
#if	MACH_KDB
		map_symtab_from_preloaded_file(server_base, server_size,
			master_host_port, user_task, "unix");
#endif	MACH_KDB
	} else {
		/*
		 * Load the file
		 */
		result = load_program_file(&file, user_task, user_thread,
					   arg_len, &arg_pos);
		if (result)
			panic("load_program_file %d", result);

#if	MACH_KDB
		/*
		 * Read symbols from file
		 */
		read_symtab_from_file(&file, master_host_port, user_task, "unix");
#endif	MACH_KDB
	}
#else	PARAGON860
	/*
	 * Load the file
	 */
	result = load_program_file(&file, user_task, user_thread, arg_len,
				   &arg_pos);
	if (result)
	    panic("load_program_file %d", result);

#if	MACH_KDB
	/*
	 * Read symbols from file
	 */
	read_symtab_from_file(&file, master_host_port, user_task, "unix");
#endif	MACH_KDB
#endif	PARAGON860

	/*
	 * Copy out the arguments.
	 */
	{
	    vm_offset_t	u_arg_start;
				/* user start of argument list block */
	    vm_offset_t	k_arg_start;
				/* kernel start of argument list block */
	    vm_offset_t u_arg_page_start;
				/* user start of args, page-aligned */
	    vm_size_t	arg_page_size;
				/* page_aligned size of args */
	    vm_offset_t	k_arg_page_start;
				/* kernel start of args, page-aligned */

	    register
	    char **	k_ap;	/* kernel arglist address */
	    char *	u_cp;	/* user argument string address */
	    register
	    char *	k_cp;	/* kernel argument string address */
	    register
	    int		i;

	    /*
	     * Get address of argument list in user space
	     */
	    u_arg_start = (vm_offset_t)arg_pos;

	    /*
	     * Round to page boundaries, and allocate kernel copy
	     */
	    u_arg_page_start = trunc_page(u_arg_start);
	    arg_page_size = (vm_size_t)(round_page(u_arg_start + arg_len)
					- u_arg_page_start);

	    result = vm_allocate(mach_task_self(),
				 &k_arg_page_start,
				 (vm_size_t)arg_page_size,
				 TRUE);
	    if (result)
		panic("boot_load_program: arg size");

	    /*
	     * Set up addresses corresponding to user pointers
	     * in the kernel block
	     */
	    k_arg_start = k_arg_page_start + (u_arg_start - u_arg_page_start);

	    k_ap = (char **)k_arg_start;

	    /*
	     * Start the strings after the arg-count and pointers
	     */
	    u_cp = (char *)u_arg_start + arg_count * sizeof(char *)
					+ 2 * sizeof(char *)
					+ sizeof(int);
	    k_cp = (char *)k_arg_start + arg_count * sizeof(char *)
					+ 2 * sizeof(char *)
					+ sizeof(int);

	    /*
	     * first the argument count
	     */
	    *k_ap++ = (char *)arg_count;

	    /*
	     * Then the strings and string pointers for each argument
	     */

#define pass_arg()							\
	arg_item_len = strlen(arg_ptr) + 1; /* include trailing 0 */	\
		/* set string pointer */				\
	*k_ap++ = u_cp;							\
		/* copy string */					\
	bcopy(arg_ptr, k_cp, arg_item_len);				\
	k_cp += arg_item_len;						\
	u_cp += arg_item_len;

	va_start(argv_ptr);
	dummy_port = va_arg(argv_ptr, mach_port_t);	/* master_host_port */
	dummy_port = va_arg(argv_ptr, mach_port_t);	/* master_device_port */
	dummy_port = va_arg(argv_ptr, mach_port_t);	/* user_task */
	dummy_port = va_arg(argv_ptr, mach_port_t);	/* user_thread */
	dummy_char = va_arg(argv_ptr, char *);		/* rootname */
	dummy_char = va_arg(argv_ptr, char *);		/* boot_string */

	arg_ptr = va_arg(argv_ptr, char *);	/* server name */
	pass_arg();
	arg_count--;

	arg_ptr = boot_string;			/* boot arguments */
	for (i = 0; i < boot_arg_count; i++) {
		while(!*arg_ptr)
			arg_ptr++;
		pass_arg();
		while(*arg_ptr)
			arg_ptr++;
	}
	arg_count -= boot_arg_count;

	for (i=0; i < arg_count; i++) {		/* flags and root dev */
		arg_ptr = va_arg(argv_ptr, char *);
		pass_arg();
	}
		

	va_end(argv_ptr);

	    /*
	     * last, the trailing 0 argument and a null environment pointer.
	     */
	    *k_ap++ = (char *)0;
	    *k_ap   = (char *)0;

	    /*
	     * Now write all of this to user space.
	     */
	    (void) vm_write(user_task,
			    u_arg_page_start,
			    k_arg_page_start,
			    arg_page_size);

	    (void) vm_deallocate(mach_task_self(),
				 k_arg_page_start,
				 arg_page_size);
	}

#if	PARAGON860
	if (server_preloaded) {
		preloaded_program_release("server");
	}
#endif	PARAGON860

	kfree((vm_offset_t)namebuf, MAXPATHLEN+1);
	return (0);
}

read_emulator_symbols(master_host_port, master_device_port, rootname, file_name)
	mach_port_t	master_host_port;
	mach_port_t	master_device_port;
	char		rootname[];
	char		file_name[];
{
	kern_return_t	result;
	struct file	file;
	char *		namebuf;
	extern char *	strbuild();
#if	PARAGON860
	boolean_t	emulator_preloaded;
	unsigned char	*emulator_base;
	unsigned long	emulator_size;
#endif	PARAGON860

#if	MACH_KDB
	/*
	 * Build file name: "/dev/$rootname/$filename"
	 */
	namebuf = (char *) kalloc(MAXPATHLEN+1);
	(void) strbuild(namebuf, "/dev/",
				 rootname,
				 "/",
				 file_name,
				 (char *)0);

	/*
	 * Open the file
	 */
	bzero((char *)&file, sizeof(file));

#if	PARAGON860
	/*
	 *	XXX don't open the files if they have
	 *	XXX been preloaded.
	 */
	emulator_preloaded = preloaded_program_present("emulator",
				&emulator_base, &emulator_size);
	if (emulator_preloaded == FALSE) {
		result = open_file(master_device_port, namebuf, &file);
		if (result != 0) {
			boot_printf("[ no emulator symbols ]");
			return;
		}
		read_symtab_from_file(&file, master_host_port,
				MACH_PORT_NULL,		/* in all tasks */
				"emulator");
	} else {
		map_symtab_from_preloaded_file(emulator_base, emulator_size,
			master_host_port, MACH_PORT_NULL, "emulator");
		preloaded_program_release("emulator");	/* XXX */
	}
#else	PARAGON860
	result = open_file(master_device_port, namebuf, &file);
	if (result != 0) {
	    boot_printf("[ no emulator symbols ]");
	    return;
	}

	/*
	 * Read symbols from file
	 */
	read_symtab_from_file(&file, master_host_port,
			MACH_PORT_NULL,		/* in all tasks */
			"emulator");
#endif	PARAGON860

	kfree((vm_offset_t)namebuf, MAXPATHLEN+1);

#endif	MACH_KDB
}

#if	MACH_KDB

#define	HEADER_MAX	(64*sizeof(int))

#if	PARAGON860
map_symtab_from_preloaded_file(prog_base, prog_size, host_port, task, symtab_name)
	unsigned char	*prog_base;
	unsigned long	prog_size;
	mach_port_t	host_port;
	mach_port_t	task;
	char *		symtab_name;
{
	vm_offset_t	sym_off;
	vm_size_t	sym_size;
	kern_return_t	result;
	char		header[HEADER_MAX];
	vm_size_t	header_size;
	vm_size_t	table_size;
	vm_offset_t	symtab;
	boolean_t	size_prefix = FALSE;
	boolean_t	cc;
	extern boolean_t	get_preload_symtab();

	/*
	 *	Only load symbols if asked to do so.
	 */
#if	MACH_ASSERT
	if (getbootint("BOOT_LOAD_SYMBOLS", 1) == 0) {
		boot_printf("[ not mapping symbols for %s ]\n", symtab_name);
		return;
	}
#else	MACH_ASSERT
	if (getbootint("BOOT_LOAD_SYMBOLS", 0) == 0) {
		boot_printf("[ not mapping symbols for %s ]\n", symtab_name);
		return;
	}
#endif	MACH_ASSERT

	header_size = HEADER_MAX;
	cc = get_preload_symtab(prog_base, prog_size, &sym_off, &sym_size, &header[0], &header_size);
	if (!cc) {
		boot_printf("[ no valid symbol table header present for %s ]\n",
			symtab_name);
		return;
	}

	/*
	 * Allocate space for the symbol table, preceding it
	 * by the optional header if there is one.
	 */
	table_size = sym_size + header_size;
	if (table_size == 0)
		return;	/* no symbols */

	result = vm_allocate(mach_task_self(), &symtab, table_size, TRUE);
	if (result) {
		boot_printf("[ error %d allocating space for %s symbol table ]\n",
			result, symtab_name);
		return;
	}

	bcopy(header, (char *)symtab, header_size);
	bcopy(prog_base + sym_off, symtab + header_size, sym_size);

	(void) host_load_symbol_table(host_port,
			task, symtab_name, symtab, table_size);
	(void) vm_deallocate(mach_task_self(), symtab, table_size);
}
#endif	PARAGON860


read_symtab_from_file(fp, host_port, task, symtab_name)
	struct file	*fp;
	mach_port_t	host_port;
	mach_port_t	task;
	char *		symtab_name;
{
	/*
	 * The symbol table is read from the file.
	 * Machine-dependent code can specify an
	 * optional header to be prefixed to the
	 * symbol table, containing information
	 * that cannot directly be read from the file.
	 */

	vm_size_t	resid;
	kern_return_t	result;
	vm_offset_t	sym_off;
	vm_size_t	sym_size;
	char		header[HEADER_MAX];
	vm_size_t	header_size;
	vm_size_t	table_size;
	vm_offset_t	symtab;
	boolean_t	size_prefix = FALSE;

#if	PARAGON860
	/*
	 *	Only load symbols if asked to do so.
	 */
#if	MACH_ASSERT
	if (getbootint("BOOT_LOAD_SYMBOLS", 1) == 0) {
		boot_printf("[ not loading symbols for %s ]\n", symtab_name);
		return;
	}
#else	MACH_ASSERT
	if (getbootint("BOOT_LOAD_SYMBOLS", 0) == 0) {
		boot_printf("[ not loading symbols for %s ]\n", symtab_name);
		return;
	}
#endif	MACH_ASSERT
#endif	PARAGON860

	/*
	 * Read the header from the file to find the symbols.
	 */
	header_size = HEADER_MAX;
	if (!get_symtab(fp, &sym_off, &sym_size, &header[0], &header_size)) {
	    boot_printf("[ no valid symbol table header present for %s ]\n",
			symtab_name);
	    return;
	}

	/*
	 * Allocate space for the symbol table, preceding it
	 * by the optional header if there is one.
	 */
	table_size = sym_size + header_size;
	if (table_size == 0)
	    return;	/* no symbols */

	result= vm_allocate(mach_task_self(),
			    &symtab,
			    table_size,
			    TRUE);
	if (result) {
	    boot_printf("[ error %d allocating space for %s symbol table ]\n",
			result, symtab_name);
	    return;
	}

	/*
	 * Copy the header to the allocated area, then
	 * read in the symbol table.
	 */
	bcopy(header, (char *)symtab, header_size);

	result = read_file(fp, sym_off,
			symtab + header_size, sym_size, &resid);
	if (result || resid) {
	    boot_printf("[ no valid symbol table present for %s ]\n",
		symtab_name);
	}
	else {
	    result = host_load_symbol_table(host_port,
			task, symtab_name, symtab, table_size);
	}
	(void) vm_deallocate(mach_task_self(), symtab, table_size);
}

#endif	MACH_KDB
