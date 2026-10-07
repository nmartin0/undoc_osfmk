/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
/*
 * Copyright 1993 by Intel Corporation,
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
 * Time tracing support
 *
 * HISTORY
 * $Log: time_trace.c,v $
 * Revision 1.3  1994/11/18  20:43:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  19:19:47  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.6.1  1994/02/25  22:52:37  andyp
 * Removed some lint w/ respect to the scope of external integer
 * definitions.
 *
 * Revision 1.1  1993/10/07  21:28:52  stans
 *    rkl's time tracing (profile) support.
 *
 * Revision 1.1.2.1  1993/07/30  23:53:49  stans
 *    RKL created the time trace device interface for high-res mk tracing.
 *
 * Revision 1.1  1993/07/29  20:00:13  stans
 * Initial revision
 *
 */

#include <mach_kdb.h>

#if	MACH_KDB

#include <sys/types.h>
#include <mach/mach_types.h>
#include <kern/thread.h>
#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <device/device_types.h>
#include <device/io_req.h>
#include <device/buf.h>
#include <i860paragon/dp.h>
#include <i860paragon/nic.h>
#include <i860paragon/time_trace.h>
#include <i860paragon/rpm.h>
#include <ddb/db_command.h>
#include <ddb/db_lex.h>

typedef struct {
	u_long	stuff[7];
	caddr_t	function;
} pgi_t;

#define SYNC_PTR	(double*)(RPM_BASE_VADDR + RPM_GLOBAL_TIME)
#define HIRES_PTR	(double*)(DP_ADDR + (DP_EPOCH_LO - DP_ADDR_PH))
#define	MAX_LOG_RECS	(1024*8)

log_rec_t trace_buf[ MAX_LOG_RECS ];

log_bd_t trace_bd = {
	&trace_bd,				/* next descriptor */
	trace_buf,				/* first entry */
	trace_buf,				/* current entry */
	trace_buf + (MAX_LOG_RECS - 1),		/* last pointer */
	0,					/* full flag */
	0,					/* overrun flag */
};

log_config_t trace_config = {
	(thread_t) 1,				/* thread to be traced */
	&trace_bd,				/* put buffer descriptor */
	&trace_bd,				/* take buffer descriptor */
	HIRES_PTR,				/* pointer to counter */
	20,					/* npt */
	0,					/* flags */
};


log_config_t	*log_conf = &trace_config;

u_long		trace_val_1;
u_long		trace_val_2;

/*
 *  trace_buf_init:
 *
 *	Initializes/re-initializes the trace log buffers.
 *	Note that the traced thread and flags are not changed.
 */
trace_buf_init()
{
	extern	int	paragon_node_mhz;

	log_bd_t	*log_bd;
	log_bd_t	*head_bd;

	if (log_conf) {
		if (log_conf->flags & TR_CLOCK_SYNC) {
			log_conf->npt = 100;
			log_conf->log_counter = SYNC_PTR;
		} else {
			log_conf->npt = 1000 / paragon_node_mhz;
			log_conf->log_counter = HIRES_PTR;
		}
		head_bd = log_conf->take;
		log_bd  = head_bd;

		do {
			log_bd->current = log_bd->first;
			log_bd->overrun = 0;
			log_bd->full    = 0;
			log_bd          = log_bd->next;
		} while (log_bd != head_bd);
		
		log_conf->take = log_conf->put;
	}
}

/*
 *  trace_calibrate:
 *
 *	Write calibration records.
 */
void
trace_calibrate()
{
	extern	void	_trace_in_calibrate();
	extern	void	_trace_out_calibrate();

	short_log_rec_t	*s_log;
	long_log_rec_t	*l_log;
	thread_t	old_thread;
	int		oldpri;
	u_long		tmp;

	if (log_conf) {
		oldpri = sploff();

		trace_buf_init();

		old_thread = log_conf->thread;
		log_conf->thread = 0;

		_trace_in_calibrate (0, 0, 0);
		_trace_in_calibrate (0, 0, 0);
		_trace_in_calibrate (0, 0, 0);
		_trace_out_calibrate(0, 0, 0);
		_trace_out_calibrate(0, 0, 0);
		_trace_out_calibrate(0, 0, 0);
		_trace_out_calibrate(0, 0, 0);

		log_conf->thread = old_thread;

		/*
		 *  Assume that the low order 32 bits of the counter
		 *  will not roll over.
		 */
		if (log_conf->flags & TR_EXTENDED) {
			l_log = (long_log_rec_t*)log_conf->take->first;
			tmp = l_log->time.ticks[0];
			l_log += 3;
			log_conf->trace_in_load = (l_log->time.ticks[0]-tmp)/3;

			tmp = l_log->time.ticks[0];
			l_log += 3;
			log_conf->trace_out_load = (l_log->time.ticks[0]-tmp)/3;
		} else {
			s_log = (short_log_rec_t*)log_conf->take->first;
			tmp = s_log->time.ticks[0];
			s_log += 3;
			log_conf->trace_in_load = (s_log->time.ticks[0]-tmp)/3;

			tmp = s_log->time.ticks[0];
			s_log += 3;
			log_conf->trace_out_load = (s_log->time.ticks[0]-tmp)/3;
		}
		
		splon(oldpri);
	}
}

/*
 *  trace_in:
 *
 *	Called on entry to a traced function.
 */
trace_in(ret_addr, pgi_mem, caller, args)
caddr_t	ret_addr;
pgi_t	*pgi_mem;
caddr_t	caller;
u_long	*args;
{
	log_bd_t		*log_bd;
	log_rec_t		*log;
	int			oldpri;

	/*
	 *  See if driver is open
	 */
	if (log_conf) {
		oldpri = sploff();

		/*
		 *  See if this is a thread of interest.
		 */
		if ((log_conf->thread == current_thread()) ||
		    (log_conf->thread == 0)) {

			/*
			 *  Find the current log record.
			 */
			log_bd = log_conf->put;
			log = log_bd->current;

			/*
			 *  Log the basics
			 */
			log->s_rec.function    = pgi_mem->function;
			log->s_rec.thread      = current_thread();
			log->s_rec.time.dticks = *log_conf->log_counter;

			/*
			 *  If this is an extended log, record extra stuff.
			 */
			if (log_conf->flags & TR_EXTENDED) {
				log->l_rec.caller       = caller;
				log->l_rec.vars.args[0] = args[0];
				log->l_rec.vars.args[1] = args[1];
				log->l_rec.vars.args[2] = args[2];
				log++;
			} else
				log = (log_rec_t*)((short_log_rec_t*)log + 1);

			/*
			 *  Update the current log record pointer and see if
			 *  it's time to use the next log buffer.
			 */
			log_bd->current = log;

			if (log >= log_bd->last) {
				log_bd->full = 1;
				log_bd->current = log_bd->last;
				log_bd = log_bd->next;
				log_conf->put = log_bd;

				/*
				 *  Check for overrun.
				 */
				if (log_bd->full)
					log_bd->overrun = 1;
				if (log_conf->flags & TR_OVERWRITE)
					log_bd->current = log_bd->first;

				/*
				 *  Let the driver know buffer is full.
				notify_driver();
				 */
			}
		}
		splon(oldpri);
	}
}

/*
 *  trace_out:
 *
 *	Called on exit from a traced function.
 */
trace_out(ret_addr, return_val, caller)
caddr_t	ret_addr;
int	return_val;
caddr_t	caller;
{
	log_bd_t		*log_bd;
	log_rec_t		*log;
	int			oldpri;

	/*
	 *  See if driver is open
	 */
	if (log_conf) {
		oldpri = sploff();

		/*
		 *  See if this is a thread of interest.
		 */
		if ((log_conf->thread == current_thread()) ||
		    (log_conf->thread == 0)) {

			/*
			 *  Find the current log entry.
			 */
			log_bd = log_conf->put;
			log = log_bd->current;

			/*
			 *  Log the basics.  Function is marked odd
			 *  to tell entries from exits in log buffer.
			 */
			log->s_rec.function    = (caddr_t)((u_long)ret_addr|1);
			log->s_rec.thread      = current_thread();
			log->s_rec.time.dticks = *log_conf->log_counter;

			/*
			 *  If this is an extended log, record extra stuff.
			 */
			if (log_conf->flags & TR_EXTENDED) {
				log->l_rec.caller            = caller;
				log->l_rec.vars.vals.ret_val = return_val;
				log->l_rec.vars.vals.val_1   = trace_val_1;
				log->l_rec.vars.vals.val_2   = trace_val_2;
				log++;
			} else
				log = (log_rec_t*)((short_log_rec_t*)log + 1);

			/*
			 *  Update the current log record pointer and see if
			 *  it's time to use the next log buffer.
			 */
			log_bd->current = log;

			if (log >= log_bd->last) {
				log_bd->full = 1;
				log_bd->current = log_bd->last;
				log_bd = log_bd->next;
				log_conf->put = log_bd;

				/*
				 *  Check for overrun.
				 */
				if (log_bd->full)
					log_bd->overrun = 1;
				if (log_conf->flags & TR_OVERWRITE)
					log_bd->current = log_bd->first;

				/*
				 *  Let the driver know buffer is full.
				notify_driver();
				 */
			}
		}
		splon(oldpri);
	}
}

#include <machine/db_machdep.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>

/*
 *  find_sym:
 *
 *	Lookup a symbol name.
 */
static db_expr_t
find_sym(sym_addr, name, offset)
caddr_t	sym_addr;
char	**name;
int	*offset;
{
	extern	db_sym_t db_search_task_symbol();
	extern	void	 db_symbol_values();

	db_expr_t	value;
	db_sym_t	cursym;

	cursym = db_search_task_symbol(	(db_addr_t)sym_addr,
					DB_STGY_PROC,
					offset,
					TASK_NULL);

	db_symbol_values(cursym, name, &value);
	return (value);
}

/*
 *  print_extended:
 *
 *	Print extended log records.
 */
static
print_extended(log_bd)
log_bd_t	*log_bd;
{
	long_log_rec_t	*log;
	char		*function;
	int		f_offset;
	char		*caller;
	int		c_offset;
	u_long		delta, last;
	u_long		in_load, out_load, dual_load;
	int		in_rec;

	log = (long_log_rec_t*)log_bd->first;
	last = log->time.ticks[0];
	in_rec = 1;
	in_load = log_conf->trace_in_load;
	out_load = log_conf->trace_out_load;
	dual_load = (in_load / 2) + (out_load / 2);

	while (log <= (long_log_rec_t*)log_bd->current) {

		(void)find_sym((u_long)log->function & ~1, &function,&f_offset);
		(void)find_sym(log->caller,                &caller,  &c_offset);

		/*
		 *  Check for exit record.
		 */
		delta = log->time.ticks[0] - last;
		if ((u_long)log->function & 1) {
			if (delta) delta -= (in_rec ? dual_load : out_load);
			if ((long)delta < 0) delta = 0;
			delta *= log_conf->npt;
			delta = ((delta + 500) / 1000);
			db_printf("<- %8x %8x %d %s %x %x %x %s+%x\n",
				log->thread,
				log->time.ticks[0],
				delta,
				function,
				log->vars.args[0],
				log->vars.args[1],
				log->vars.args[2],
				caller, c_offset);

			last = log->time.ticks[0];
			in_rec = 0;
		/*
		 *  Else do entry record.
		 */
		} else {
			if (delta) delta -= (in_rec ? in_load : dual_load);
			if ((long)delta < 0) delta = 0;
			delta *= log_conf->npt;
			delta = ((delta + 500) / 1000);
			db_printf("-> %8x %8x %d  %s",
				log->thread,
				log->time.ticks[0],
				delta,
				function);
			if (f_offset)
				db_printf("+%x", f_offset);
			db_printf("(%x,%x,%x) %s+%x\n",
				log->vars.args[0],
				log->vars.args[1],
				log->vars.args[2],
				caller, c_offset);

			last = log->time.ticks[0];
			in_rec = 1;
		}
		log++;
	}
}

/*
 *  print_abbreviated:
 *
 *	Print abbreviated log records.
 */
static
print_abbreviated(log_bd)
log_bd_t	*log_bd;
{
	short_log_rec_t	*log;
	char		*function;
	u_long		delta, last;
	u_long		in_load, out_load, dual_load;
	int		offset;
	int		in_rec;
	int	i;

	log = (short_log_rec_t*)log_bd->first;
	last = log->time.ticks[0];
	in_rec = 1;
	in_load = log_conf->trace_in_load;
	out_load = log_conf->trace_out_load;
	dual_load = (in_load / 2) + (out_load / 2);

	for (i = 0; log != (short_log_rec_t*)log_bd->current; i++) {

		/*
		 *  Lookup the symbol name.
		 */
		(void)find_sym((u_long)log->function & ~1, &function, &offset);

		/*
		 *  Check for exit record.
		 */
		delta = log->time.ticks[0] - last;
		if ((u_long)log->function & 1) {
			if (delta) delta -= (in_rec ? dual_load : out_load);
			if ((long)delta < 0) delta = 0;
			delta *= log_conf->npt;
			delta = ((delta + 500) / 1000);
			db_printf("%4d <- %8x %8x %d  %s\n",
				i,
				log->thread,
				log->time.ticks[0],
				delta,
				function);

			last = log->time.ticks[0];
			in_rec = 0;
		/*
		 *  Else do entry record.
		 */
		} else {
			if (delta) delta -= (in_rec ? in_load : dual_load);
			if ((long)delta < 0) delta = 0;
			delta *= log_conf->npt;
			delta = ((delta + 500) / 1000);
			db_printf("%4d -> %8x %8x %d  %s",
				i,
				log->thread,
				log->time.ticks[0],
				delta,
				function);
			if (offset)
				db_printf("+%x\n", offset);
			else
				db_printf("\n");

			last = log->time.ticks[0];
			in_rec = 1;
		}
		log++;
	}
}

/*
 *  time_trace_print:
 *
 *	Print contents of trace buffer.
 */
void
time_trace_print()
{
	log_bd_t		*log_bd;
	log_bd_t		*head_bd;

	/*
	 *  See if we have started tracing.
	 */
	if (!log_conf) {
		db_printf("Time tracing has not been started\n");
		return;
	}

	/*
	 *  Print the trace configuration.
	 */
	db_printf(
	"Traced thread = %x npt = %d in-load = %d out-load = %d, node clocks%s in sync\n",
			log_conf->thread,
			log_conf->npt,
			log_conf->trace_in_load,
			log_conf->trace_out_load,
			log_conf->flags & TR_CLOCK_SYNC   ? ""     : " not");

	/*
	 *  Let's start printing them.
	 */
	head_bd = log_conf->take;
	log_bd  = head_bd;

	do {
		if (log_bd->overrun)
			db_printf(
		"******* trace buffer overrun - log records have been lost\n");

		if (log_conf->flags & TR_EXTENDED)
			print_extended(log_bd);
		else
			print_abbreviated(log_bd);

		log_bd = log_bd->next;

	} while (log_bd != head_bd);
}

/*************************************
 *  Device driver interface routines.
 *************************************/

/*
 *	Open the time trace device.
 */
ttrace_open(dev, flag, ior)
	dev_t		dev;
	int		flag;
	io_req_t	ior;
{
	return D_SUCCESS;
}


/*
 *	Close the time trace device.
 */
ttrace_close(dev, flag)
	dev_t	dev;
	int	flag;
{
	return D_SUCCESS;
}


/*
 *	Read/Write the trace device data.
 *
 */
#define	TTRACE_MAX_READ	(sizeof(trace_config)	+ \
			 sizeof(trace_bd)	+ \
			 sizeof(trace_buf))

ttrace_strategy(ior)
	io_req_t	ior;
{
	int	spl;
	char	*data;

	if (ior->io_op & IO_READ) {

		if (ior->io_count == 0) {
			iodone(ior);
			return;
		}

		if (ior->io_count > TTRACE_MAX_READ) {
			ior->io_op |= IO_ERROR;
			ior->io_error = D_INVALID_SIZE;
			iodone(ior);
			return;
		}

		data = ior->io_data;
		spl = sploff();

		bcopy(&trace_config, data, sizeof(trace_config));
		data += sizeof(trace_config);
		bcopy(&trace_bd, data, sizeof(trace_bd));
		data += sizeof(trace_bd);
		bcopy(trace_buf, data, sizeof(trace_buf));

		splon(spl);
		ior->io_residual = TTRACE_MAX_READ - ior->io_count;
		iodone(ior);
		return;
	}

	/*
	 *  We don't do write
	 */
	ior->io_op |= IO_ERROR;
	ior->io_error = D_IO_ERROR;
	iodone(ior);
}

ttrace_min(ior)
	io_req_t	ior;
{
}

/*
 *	Read the time trace data.
 */
ttrace_read(dev, ior)
	dev_t		dev;
	io_req_t	ior;
{
	if (dev != ior->io_unit)
		ior->io_unit = dev;
	return block_io(ttrace_strategy, ttrace_min, ior);
}

/*
 *	Write to the RPM counters (not really)
 */
ttrace_write(dev, ior)
	dev_t		dev;
	io_req_t	ior;
{
	if (dev != ior->io_unit)
		ior->io_unit = dev;
	return block_io(ttrace_strategy, ttrace_min, ior);
}


/*
 *	Map in the time trace data.
 */
vm_offset_t
ttrace_mmap(dev, off, prot)
	dev_t		dev;
	vm_offset_t	off;
	vm_prot_t	prot;
{
	/*
	if (minor(dev) != 0)
		return -1;
	return i860_btop(RPM_BASE_PADDR);
	*/
	return -1;
}

ttrace_devinfo(number, code, data)
	int	number, code, *data;
{
	return D_INVALID_OPERATION;
}

/*
 *  ttrace_getstat:
 *
 *	Get trace driver status.
 */
ttrace_getstat(dev, flavor, status, count)
	dev_t           dev;
	int             flavor;
	dev_status_t    status;
	u_int           *count;
{
	if (log_conf == 0)
		return (D_INVALID_OPERATION);

	switch (flavor) {
		case TR_FLAGS:
			*status = (int)log_conf->flags;
			*count = 4;
			break;
		case TR_GET_RD_SIZE:
			*status = TTRACE_MAX_READ;
			*count = 4;
			break;
		default:
			return (D_INVALID_OPERATION);
	}
	return (D_SUCCESS);
}

/*
 *  ttrace_setstat:
 *
 *	Set trace driver status.
 */
ttrace_setstat(dev, flavor, status, count)
	dev_t           dev;
	int             flavor;
	dev_status_t    status;
	u_int           count;
{
	extern	int	paragon_node_mhz;

	if (log_conf == 0)
		return (D_INVALID_OPERATION);

	switch (flavor) {
		case TR_FLAGS:
			log_conf->flags = *((u_long*)status);
			if (log_conf->flags & TR_CLOCK_SYNC) {
				log_conf->log_counter = SYNC_PTR;
				log_conf->npt = 100;
			} else {
				log_conf->npt = 1000 / paragon_node_mhz;
				log_conf->log_counter = HIRES_PTR;
			}
			break;
		case TR_RESET:
			trace_calibrate();
			break;
		case TR_SET_THREAD:
			ttrace_set_thread( *((thread_t*)status) );
			break;
		case TR_START:
			ttrace_on();
			break;
		case TR_STOP:
			ttrace_off();
			break;
		default:
			return (D_INVALID_OPERATION);
	}
	return (D_SUCCESS);
}

/*
 *	user-mode trap & kdb interface routines
 */

/*
 * start time tracing
 *	machine tt start | on {all,thread-ID}
 */
ttrace_on()
{
	if (log_conf == 0)
		return;

	log_conf->thread = (thread_t)((u_long)log_conf->thread & ~1);
}

ttrace_set_thread(th)
	thread_t	th;
{
	if (log_conf == 0)
		return;

	log_conf->thread = th;
}

/*
 * stop time tracing
 *	machine tt stop | off
 */
ttrace_off()
{
	if (log_conf == 0)
		return;

	log_conf->thread = (thread_t)((u_long)log_conf->thread | 1);
}

db_tt_start()
{
	int		t, ok=1;
	char		*name = 0;
	extern char	db_tok_string[];

	switch( (t = db_read_token()) ) {
	  case tIDENT:	/* all */
		name = db_tok_string;
		if (strcmp(name,"all") == 0)
			ttrace_set_thread( 0 );
		break;

	  case tNUMBER:	/* a thread ID */
		ttrace_set_thread( db_tok_number );
		break;

	  case tEOL:	/* no args imples all threads */
		ttrace_set_thread( 0 );
		break;

	  default:
		ok = 0;
		break;
	}
	if ( ok )
		ttrace_on();
}

char	*tt_help="start/on stop/off cal/calibrate/clear show/print";

db_tt_help()
{
	db_printf("Valid machine tt commands { %s }\n",tt_help);
}

struct db_command db_time_trace_cmds[] = {
	{ "help",	db_tt_help,		CS_OWN,	0 },
	{ "start",	db_tt_start,		CS_OWN,	0 },
	{ "on",		db_tt_start,		CS_OWN,	0 },
	{ "stop",	ttrace_off,		CS_OWN,	0 },
	{ "off",	ttrace_off,		CS_OWN,	0 },
	{ "calibrate",	trace_calibrate,	CS_OWN,	0 },
	{ "cal",	trace_calibrate,	CS_OWN,	0 },
	{ "clear",	trace_calibrate,	CS_OWN,	0 },
	{ "show",	time_trace_print,	CS_OWN,	0 },
	{ "print",	time_trace_print,	CS_OWN,	0 },
        { (char *)0 }
};

#endif	/* MACH_KDB */
