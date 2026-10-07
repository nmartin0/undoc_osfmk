/*
 * 
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
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
 * $Id: norma_log.c,v 1.6 1994/11/18 20:58:45 mtm Exp $
 *
 * HISTORY:
 */

#include <mach_assert.h>
#include <mach/boolean.h>
#include <ipc/ipc_port.h>
#include <i860paragon/dp.h>
#include <i860paragon/nic.h>
#include <i860paragon/rpm.h>
#include <norma2/norma_log.h>
#include <machine/db_machdep.h>
#include <kern/cpu_number.h>

#if	MACH_ASSERT

#define	MAX_NORMA_LOG_ENTRIES	max_entries

log_entry_t	*norma_log_buffer;

boolean_t dipc_reference_logging;	/* IPC reference logging boolean */
boolean_t log_all_refs;			/* IPC reference logging boolean */

#define SYNC_PTR	((double*)(RPM_BASE_VADDR + RPM_GLOBAL_TIME))
#define HIRES_PTR	((double*)(DP_ADDR + (DP_EPOCH_LO - DP_ADDR_PH)))

static	unsigned long	MAX_NORMA_LOG_ENTRIES;
static	int		norma_log_index = 0;
static	int		backup_norma_log_index = 0;
static	int		wrap_count = 0;
static	int		backup_wrap_count = 0;
static	unsigned long	logging_overhead = 0;
static	unsigned long	backup_logging_overhead = 0;
static	volatile double	*tics_ptr = SYNC_PTR;	/* try RPM counter first */
static	boolean_t	use_rpm = TRUE;

_log_level_t	dipc_log_modules[MAX_LOG_ID];

typedef	union {
	unsigned long	tics[2];
	double		dticks;
} ticks_t;

void
norma_log_init()
{
	ticks_t		rpm1, rpm2;
	unsigned long	delta;
	int		i, only;
	unsigned long	level;
	
	/*
	 *  See if they want us to log ip_reference changes.
	 */
	dipc_reference_logging = getbootint("DIPC_REF_LOGGING", 0);
	log_all_refs = getbootint("VERBOSE_REF_LOGGING", 0);

	/*
	 *  Determine the size of the log buffer and allocate it.
	 */
	MAX_NORMA_LOG_ENTRIES  = getbootint("NORMA_LOG_ENTRIES", 2048);
	norma_log_buffer = (log_entry_t*) kalloc(MAX_NORMA_LOG_ENTRIES *
							sizeof(log_entry_t));

	/*
	 *  Set logging level - do all levels except entries by default
	 */
	level = getbootint("NORMA_LOG_LEVEL", (bit(0) | bit(1) | bit(2)));

	/*
	 *  See if they want function entries logged.  If so, add it to
	 *  log_level.
	 */
	if (getbootint("NORMA_LOG_FUNC_ENTRY", 0))
		level |= bit(ENTRY_LOG_LEVEL);

	/*
	 *  Squelch all modules except a specific module id.
	 */
	only = getbootint("NORMA_LOG_ONLY", -1);

	/*
	 *  Modify all module logging level values as set by default or
	 *  though bootmagic.
	 */
	for (i = 0; i < MAX_LOG_ID; i++) {
		if (only == -1) {
			dipc_log_modules[i] = level;
		} else {
			dipc_log_modules[i] = (i == only) ? level : 0;
		}
	}

	/*
	 *  See if the RPM seems to be running
	 */
	tics_ptr = SYNC_PTR;
	use_rpm  = TRUE;

	rpm1.dticks = *tics_ptr;
	delay(500);
	rpm2.dticks = *tics_ptr;

	delta = rpm2.tics[0] - rpm1.tics[0];
	if (delta < 4850 || delta > 5150) {
		use_rpm = FALSE;
		tics_ptr = HIRES_PTR;
	}
}


static	boolean_t invert = FALSE;

boolean_t
substr(p1, p2, count)
	char	*p1;
	char	*p2;
	int	count;
{
	int	hits;
	char	*curr;
	boolean_t	t = TRUE;
	boolean_t	f = FALSE;

	if (invert == TRUE) {
		t = FALSE;
		f = TRUE;
	}
	invert = FALSE;

	curr = p1;
	hits = 0;

	while (*p2) {
		if (*curr++ == *p2++) {
			if (++hits == count)
				return t;
		} else {
			curr = p1;
			hits = 0;
		}
	}
	return f;
}

static	char pattern_buf[ 256 ];

boolean_t
pattern_match(master, pattern)
	char	*master;
	char	*pattern;
{
	char		*start;
	int		count;

	start = master;
	count = 0;
	for (;;) {
		switch (*master) {
			case   0:
			case ' ':
				return (substr(start, pattern, count));

			case '|':
			case '.':
				if (substr(start, pattern, count))
					return TRUE;
				start = master + 1;
				count = 0;
				break;
			case ':':
			case '&':
				if (!substr(start, pattern, count))
					return FALSE;
				start = master + 1;
				count = 0;
				break;
			case '!':
				invert = TRUE;
				start = master + 1;
				count = 0;
				break;
			default:
				count++;
				break;
		}
		master++;
	}
}

char	*norma_log_help_str = "\
\n\
show norma_log/[r]|[tdecruh:] [entry[,count]]\n\
\n\
r - reset log buffer.  Must be only option specified\n\
t - show tick value at time of log entry\n\
T - show thread making long entry\n\
d - show time delta between displayed entries\n\
e - do not show log entry number\n\
c - show CPU that made log entry\n\
u - undo reset\n\
h - this display\n\
: - begin string pattern matching specification\n\
    Pattern strings can be separated by '.' for OR\n\
                                        ':' for AND\n\
                                       '\\!' for NOT operators.\n";

#define	TOTAL_LOG_ENTRIES \
	(norma_log_index + (MAX_NORMA_LOG_ENTRIES * wrap_count))

#define	ADDR_IN_RANGE(addr) \
	((addr < TOTAL_LOG_ENTRIES) && \
	 (wrap_count ?	(addr > (TOTAL_LOG_ENTRIES - MAX_NORMA_LOG_ENTRIES)) : \
			(addr >= 0)))

void
show_norma_log(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char		*modif;
{
	log_entry_t	*p;
	boolean_t	show_time = FALSE;
	boolean_t	show_thread = FALSE;
	boolean_t	show_delta = FALSE;
	boolean_t	show_event = TRUE;
	boolean_t	show_cpu = FALSE;
	boolean_t	show_int_level = FALSE;
	boolean_t	mark_matches = FALSE;
	char		*str = 0;
	int		start;
	int		first_entry;
	unsigned long	old_time;
	unsigned long	overhead;

	/*
	 *  Resets must have the 'r' and only the 'r' option.
	 */
	if (*modif == 'r' && *(modif+1) == 0) {
		db_printf("Norma log reset\n");
		backup_norma_log_index = norma_log_index;
		backup_wrap_count = wrap_count;
		backup_logging_overhead = logging_overhead;
		norma_log_index = 0;
		wrap_count = 0;
		logging_overhead = 0;
		return;
	}

	/*
	 *  Go through the options.
	 */
	while (*modif && str == 0) {
		switch (*modif) {
			case 't': show_time = TRUE; break;
			case 'T': show_thread = TRUE; break;
			case 'd': show_delta = TRUE; break;
			case 'e': show_event = FALSE; break;
			case 'c': show_cpu = TRUE; break;
			case 'i': show_int_level = TRUE; break;
			case 'm': mark_matches = TRUE; break;
			case ':': str = modif+1; break;
			case 'h': db_printf(norma_log_help_str); break;
			case 'u':
				norma_log_index = backup_norma_log_index;
				wrap_count = backup_wrap_count;
				logging_overhead = backup_logging_overhead;
				db_printf("Norma log entries reset\n");
				break;
			default:
				db_printf("Bad option - %c ignored\n", *modif);
		}
		modif++;
	}

	overhead = (logging_overhead / TOTAL_LOG_ENTRIES) / (use_rpm ? 1 : 5);

	db_printf(
      "Total entries logged = %d : log buffer depth = %d : overhead = %d.%d\n",
				TOTAL_LOG_ENTRIES, MAX_NORMA_LOG_ENTRIES,
				overhead / 10, overhead % 10);

	if (TOTAL_LOG_ENTRIES == 0)
		return;

	/*
	 *  Determine the starting point
	 *
	 *  See if address is within range.
	 */
	if (have_addr && ADDR_IN_RANGE(addr)) {
		start = (addr % MAX_NORMA_LOG_ENTRIES);
	} else {
		start = norma_log_index - 1;
		addr = start + (wrap_count * MAX_NORMA_LOG_ENTRIES);
	}

	if (start < 0) {
		if (wrap_count)
			start += MAX_NORMA_LOG_ENTRIES;
		else
			start = 0;
	}

	/*
	 *  Determine counts.
	 */
	if (count == -1 || count > MAX_NORMA_LOG_ENTRIES)
		count = MAX_NORMA_LOG_ENTRIES;

	first_entry = 0;
	if (wrap_count)
		first_entry = TOTAL_LOG_ENTRIES - MAX_NORMA_LOG_ENTRIES;

	if ((addr - count) < first_entry)
		count = (addr - first_entry) + 1;

	/*
	 *  Here we go...
	 */
	old_time = 0;
	p = &norma_log_buffer[ start ];
	while (count) {
		boolean_t match;

		/*
		 *  Check for buffer wrap.
		 */
		if ((unsigned long)p < (unsigned long)norma_log_buffer)
			p = &norma_log_buffer[ MAX_NORMA_LOG_ENTRIES - 1 ];

		/*
		 *  Construct the printable string in a buffer for
		 *  easier pattern matching.
		 */
#define	APPEND(buf)	(buf + strlen(buf))
		pattern_buf[0] = 0;

		sprintf(APPEND(pattern_buf), "* ");

		if (show_time)
			sprintf(APPEND(pattern_buf), "%u ", p->time_stamp); 

		if (show_delta)
			sprintf(APPEND(pattern_buf), "(%6u) ",
					old_time ? use_rpm ?
						(old_time - p->time_stamp) / 10:
						(old_time - p->time_stamp) / 50:
						 0 );
		if (show_event)
			sprintf(APPEND(pattern_buf), "[%d] ", addr);

		if (show_cpu)
			sprintf(APPEND(pattern_buf), "{%d} ", p->cpu);

		if (show_int_level)
			sprintf(APPEND(pattern_buf), ">%d< ", p->intr);

		if (show_thread)
			sprintf(APPEND(pattern_buf), "<%x> ", p->thread);

		sprintf(APPEND(pattern_buf), p->str, p->a, p->b, p->c, p->d,
					     p->e, p->f, p->g, p->h);

		if (str == 0 || (match = pattern_match(str, pattern_buf)) ||
								mark_matches) {
			if (!match)
				pattern_buf[0] = ' ';
			if (!mark_matches)
				db_printf(pattern_buf+2);
			else
				db_printf(pattern_buf);
			old_time = p->time_stamp;
		}

		count--;
		addr--;
		p--;
	}
#undef	APPEND
}

void
norma_buffered_trace_log(str, a, b, c, d, e, f, g, h)
	char *str;
	int  a, b, c, d, e, f, g, h;
{
	extern	int     interrupt_nesting_level[NCPUS];

	log_entry_t	*p;
	ticks_t		tics_union;
	ticks_t		tics_overhead_out;
	int		old;

	tics_union.dticks = *tics_ptr;

	old = sploff();

	p = &norma_log_buffer[ norma_log_index ];
	p->str = str;
	p->a = a;
	p->b = b;
	p->c = c;
	p->d = d;
	p->e = e;
	p->f = f;
	p->g = g;
	p->h = h;
	p->thread = (unsigned long)(current_thread());
	p->time_stamp = tics_union.tics[0];
	p->intr = interrupt_nesting_level[cpu_number()];
	p->cpu = cpu_number();

	if (++norma_log_index == MAX_NORMA_LOG_ENTRIES) {
		norma_log_index = 0;
		wrap_count++;
	}

	splon(old);

	tics_overhead_out.dticks = *tics_ptr;
	if (tics_union.tics[1] == tics_overhead_out.tics[1])
		logging_overhead += tics_overhead_out.tics[0] -
				    tics_union.tics[0];
}

void	(*norma_log)() = norma_buffered_trace_log;

#include <machine/db_machdep.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>

char*
norma_log_name(func)
	void	(*func)();
{
	extern	db_sym_t	db_search_task_symbol();
	extern	void		db_symbol_values();

	char		*name;
	int		offset;
	db_expr_t	value;
	db_sym_t	cursym;
	
	cursym = db_search_task_symbol( (db_addr_t)func, DB_STGY_PROC,
					&offset, TASK_NULL);
	db_symbol_values(cursym, &name, &value);
	if (*name == '_')
		name++;
	return (offset ? "?????" : name);
}

#endif	/* MACH_ASSERT */
