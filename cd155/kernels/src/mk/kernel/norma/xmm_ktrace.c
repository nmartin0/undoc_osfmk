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
 * SSD HISTORY
 * $Log: xmm_ktrace.c,v $
 * Revision 1.3  1994/11/18  20:56:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/03/29  12:46:22  fritz
 * Initial version (derived from sz's ASVM)
 *
 *
 * END SSD HISTORY
 */
/*
 *  YAKT - Yet Another Kernel Tracer
 *
 *  YAKT supports a kt_printf function that acts just like a normal
 *  printf but writes its arguments into a trace record instead
 *  of evaluating them. When >> p_svm_trace << is called from the
 *  kernel debugger, the postponed printfs will be done.
 */

#include <norma/xmm_ktrace.h>

#if MACH_KDB

#include <platforms.h>
#include <sys/varargs.h>
#include <kern/assert.h>
#include <machine/db_machdep.h>

#if PARAGON860

#include <i860paragon/rpm.h>
#include <machine/cpu_number.h>
#include <kern/lock.h>

#if 0
#ifdef simple_lock
#undef simple_lock_init
#undef simple_lock
#undef simple_unlock
#endif simple_lock
#endif

#endif PARAGON860

#define TRACE_BUFFER_ELEMS 32768 

#define printf kdbprintf

/*
 *  No argument type ever passed to tprintf() must be bigger then
 *  trace_entry_t.
 *  I assume this is guaranteed be making trace_entry_t a pointer
 *  If you really want to print floating points, make it a double.
 */
typedef char *trace_entry_t;

unsigned kt_mode = KT_UNDEFINED;


#if PARAGON860
extern int paragon_node_mhz;
struct rpm *kt_rpm;
#endif PARAGON860

simple_lock_data_t svm_trace_lock;
trace_entry_t *svm_trace_buff;
trace_entry_t *svm_trace_start, *svm_trace_actual, *svm_trace_top;

#define ENTER_TRACE(value, type) \
    { \
        trace_entry_t *next_pos = svm_trace_top + 1; \
        if(next_pos >= svm_trace_buff+TRACE_BUFFER_ELEMS) { \
            next_pos = svm_trace_buff; \
        } \
 \
        if(next_pos == svm_trace_start) { \
            /* \
             *  Move readout start pointer as we will overwrite \
             *  the next entry. \
             */ \
             svm_trace_start += *((unsigned*)svm_trace_start); \
            if(svm_trace_start >= svm_trace_buff+TRACE_BUFFER_ELEMS) { \
                svm_trace_start -= TRACE_BUFFER_ELEMS; \
            } \
        } \
 \
        *((type *)svm_trace_top) = (value); \
 \
        *((unsigned*) svm_trace_actual) += 1; \
        svm_trace_top = next_pos; \
    }

svm_trace_init()
    {
    if(kt_mode) {
        svm_trace_buff = (trace_entry_t *) kalloc(
            TRACE_BUFFER_ELEMS*sizeof(trace_entry_t));
    } else {
        svm_trace_buff = (trace_entry_t *) 0;
    }
    svm_trace_start = svm_trace_top = svm_trace_actual = svm_trace_buff;

    simple_lock_init(&svm_trace_lock);
#if PARAGON860
    if (kt_mode & KT_MODE_TIME) {
        /*
         *  Check if we can use the global RPM time base
         */
        if (rpm_probe())
            kt_rpm = (struct rpm *) RPM_BASE_VADDR;
        else
            kt_rpm = (struct rpm *) 0;
    }
#endif PARAGON860
}

/*
 *  This trace printf accepts parameters like a normal printf
 *  but writes them into the trace buffer instead of evaluating them.
 */
void x_kt_printf(fmt, va_alist)
    char *fmt;
    va_dcl
{
    va_list listp;
    int s;

    assert(kt_mode);

    va_start(listp);

    if (kt_mode == KT_UNDEFINED) {
        kt_mode = getbootint("BOOT_XMM_TRACE", 0);
        if (kt_mode == 0)
            return;
        svm_trace_init();
    }

#if PARAGON860
    /*
     *  Protect compute cpu's and message passing processor.
     */
    s = sploff();
#endif PARAGON860
    simple_lock(&svm_trace_lock);

    ENTER_TRACE(0, unsigned);

#if PARAGON860
    if (kt_mode & KT_MODE_TIME) {
        if (kt_rpm) {
            ENTER_TRACE(*((unsigned *)&kt_rpm->rpm_time), unsigned);
        } else {
            ENTER_TRACE(inl(DP_EPOCH_LO), unsigned);
        }
    }
#endif

    ENTER_TRACE(fmt, char *);

    if (kt_mode & KT_MODE_PARAM)
        for(; *fmt != '\0'; fmt++) {
            if(*fmt == '%') {
                boolean_t is_long = FALSE;
                while(1) {
                    fmt++;
                    if(*fmt == 'l') {
                        is_long = TRUE;
                        continue;
                    }
                    if(*fmt == 's') {
                        char *value = va_arg(listp, char *);
                        ENTER_TRACE(value, char*);
                        break;
                    }
                    if (*fmt > '0' && *fmt <= '9')
                        continue;
                    if((*fmt > 'a' && *fmt <= 'z') || (*fmt > 'A' && *fmt <= 'Z')) {
                        if(is_long) {
                            long value = va_arg(listp, long);
                            ENTER_TRACE(value, long int);
                        } else {
                            int value = va_arg(listp, int);
                            ENTER_TRACE(value, int);
                        }
                        break;
                    }
                    panic("kt_printf: Illegal format string");
                }
                fmt++;
            }
        }

    ENTER_TRACE(*((unsigned*) svm_trace_actual) + 1, unsigned);
    svm_trace_actual = svm_trace_top;

    simple_unlock(&svm_trace_lock);

#if PARAGON860
    /*
     *  Unprotect compute cpu's or message passing processor.
     */
    splon(s);
#endif PARAGON860

    va_end(listp);
}

#define TT_NONE 0
#define TT_INT 1
#define TT_LONG 2
#define TT_STRING 3

#define GET_TRACE(value, type) \
    { \
        value = *((type *)record); \
        if(++record >= svm_trace_buff+TRACE_BUFFER_ELEMS) { \
            record = svm_trace_buff; \
        } \
    }

/*
 *  Print a trace entry using the debugger printf function.
 */
void
p_svm_trace_record(record)
    trace_entry_t *record;
{
    char s[256];
    unsigned size;
    char *fmt;
    unsigned trace_type;
    unsigned i;
#if PARAGON860
    unsigned int time_stamp;
#endif

    GET_TRACE(size, unsigned);

#if PARAGON860
    if (kt_mode & KT_MODE_TIME) {
        GET_TRACE(time_stamp, unsigned);
        if (!kt_rpm)
            time_stamp = time_stamp / (paragon_node_mhz/10);
        printf(  "%3d", time_stamp / 10000000);
        time_stamp = time_stamp % 10000000;
        printf(  ".%03d", time_stamp / 10000);
        time_stamp = time_stamp % 10000;
        printf(  ".%03d: ", time_stamp / 10);
    }
#endif

    GET_TRACE(fmt, char *);
    trace_type = TT_NONE;
    i=0;

    if (!(kt_mode & KT_MODE_PARAM))
        printf("%s", fmt);
    else {
        while(1) {
            if(*fmt == '%' || *fmt == '\0') {
                boolean_t is_long = FALSE;
                s[i] = '\0';
                switch(trace_type) {
                    case TT_NONE:
                        if(i>0)
                            printf(s);
                        break;
                    case TT_INT:
                        {
                        int value;
                        GET_TRACE(value, int);
                        printf(s, value);
                        }
                        break;
                    case TT_LONG:
                        {
                        long value;
                        GET_TRACE(value, long);
                        printf(s, value);
                        }
                        break;
                    case TT_STRING:
                        {
                        char *value;
                        GET_TRACE(value, char *);
                        printf(s, value);
                        }
                        break;
                }
                 if(*fmt == '\0')
                    break;
                i = 0;
                while(1) {
                    s[i++] = *fmt++;
                    if(*fmt == 'l') {
                        is_long = TRUE;
                        continue;
                    }
                    if(*fmt == 's') {
                        trace_type = TT_STRING;
                        break;
                    }
                    if((*fmt > 'a' && *fmt <= 'z') || (*fmt > 'A' && *fmt <= 'Z')) {
                        if(is_long)
                            trace_type = TT_LONG;
                        else
                            trace_type = TT_INT;
                        break;
                    }
                    panic("tprintf: Illegal format string");
                }
            }
            s[i++] = *fmt++;
        }
    }
    GET_TRACE(size, unsigned);
}

void
trace_back(record_p)
    trace_entry_t **record_p;
{
    trace_entry_t *record = *record_p;

    if(record == svm_trace_buff) {
           record -= *( (unsigned*) (svm_trace_buff+TRACE_BUFFER_ELEMS-1) );
    } else {
        record -= *( (unsigned*) (record-1) );
    }
    if(record < svm_trace_buff) {
        record += TRACE_BUFFER_ELEMS;
    }
    *record_p = record;
}

int
p_svm_trace()
{
    trace_entry_t *record;

    printf("Trace record from node %u (most recent events first):\n\n", 
        node_self());

    record = svm_trace_top; 

    
    while(record != svm_trace_start) {
        trace_back(&record);
        p_svm_trace_record(record);
    }
    return(0);
}

int
p_svm_trace_skip(n)  
    unsigned n;
{
    trace_entry_t *record;
    printf("Trace record from node %u (most recent events first, %u events skipped):\n\n", node_self(), n);
    
    record = svm_trace_top;

    while(record != svm_trace_start && n--) {
        trace_back(&record);
    }

    if(record == svm_trace_start && n > 0) {
        printf("not that many entries.\n");
    }

    while(record != svm_trace_start) {
        trace_back(&record);
        p_svm_trace_record(record);
    }
    return(0);
}

/*
 *  Debugger command 'show ktrace'
 *  /f : apply filter (the 'addr' parameter must be present in a trace entry)
 *  /s : skip n entries (n is the value of the 'count' parameter)
 *  /c : clear kernel trace after printing 
 */
void
db_show_ktrace(addr, have_addr, count, modif)
    db_expr_t addr;
    boolean_t have_addr;
    db_expr_t count;
    char *modif;
{
    trace_entry_t *record;
    boolean_t filter = FALSE;
    unsigned long filter_value;  
    unsigned skip_count = 0;
    unsigned entry_count = 0;
    boolean_t clear = FALSE;

    if(db_option(modif, 'f')) {
        /* Filter option */
        if (have_addr) {
            filter = TRUE;
            filter_value = addr;
        }
    }
    if(db_option(modif, 's')) {
        /* Skip option */
        skip_count = count;
    }
    if(db_option(modif, 'c')) {
        /* Clear option */
        clear = TRUE;
    }

    printf("Trace record from node %u (most recent events first):\n\n", 
        node_self());

    /*
     *  Skip entries if the /s option was present.
     */
    record = svm_trace_top;
    while(record != svm_trace_start && skip_count--) {
        trace_back(&record);
        entry_count++;
    }
    if(record == svm_trace_start && skip_count > 0) {
        printf("not that many entries.\n");
    } else if(entry_count > 0) {
        printf("# %u\n", entry_count);
    }

    /*
     *  Now print entries.
     *  If the /f option was present, print only entries that contain
     *  the filter value. 
     */
    while(record != svm_trace_start) {

        trace_back(&record);
        entry_count++;

        /*
         *  Apply filter
         */
        if(filter) {

            int size; 
            char *fmt;
            unsigned long value;
            trace_entry_t *record_start = record;

            record_start = record;
            GET_TRACE(size, int);
            GET_TRACE(fmt, char *);
            size -= 3;
            while(size > 0) {
                GET_TRACE(value, unsigned long);
                if(value == filter_value) {
                    break;
                }
                size--;
            }
            record = record_start;
            if(size != 0) {
                printf("# %u\n", entry_count);
                p_svm_trace_record(record);
                printf("\n");
            }

        } else {

            p_svm_trace_record(record);

        }

    } 

    /*
     *  Clear kernel trace if the /c option was present.
     */
    if(clear) {
        svm_trace_start = svm_trace_top = svm_trace_actual = svm_trace_buff;
    }
}

#undef printf

#endif MACH_KDB
