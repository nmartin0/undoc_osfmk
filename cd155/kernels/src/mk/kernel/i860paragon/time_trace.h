/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *  Tick counter.
 */
typedef	union {
	u_long	ticks[2];	/* 64 bit counter */
	double	dticks;		/* double for atomic counter reads */
} tick_t;

/*
 *  Long form of log record
 */
typedef struct {
	caddr_t		function;	/* function being timed */
	thread_t	thread;		/* current thread */
	tick_t		time;		/* time stamp */
	caddr_t		caller;		/* call of the timed function */
	union {
		u_long	args[3];	/* first 3 args on entry */
		struct {
			u_long	ret_val;/* return value at exit */
			u_long	val_1;	/* function settable value */
			u_long	val_2;	/* function settable value */
		} vals;
	} vars;
} long_log_rec_t;


/*
 *  Short form of log record
 */
typedef struct {
	caddr_t		function;	/* function being timed */
	thread_t	thread;		/* current thread */
	tick_t		time;		/* time stamp */
} short_log_rec_t;

/*
 *  Log record
 */
typedef union {
	long_log_rec_t	l_rec;
	short_log_rec_t	s_rec;
} log_rec_t;

/*
 *  Log buffer descriptor.
 */
typedef struct log_bd {
	struct log_bd	*next;		/* pointer to next descriptor */
	log_rec_t	*first;		/* pointer to first record */
	log_rec_t	*current;	/* pointer to current record */
	log_rec_t	*last;		/* pointer to last possble record */
	short		 full;		/* when true the buffer is full */
	short		 overrun;	/* when true buffer overran */
} log_bd_t;

/*
 *  Log configuration
 */
typedef struct {
	thread_t	thread;		/* thread to trace */
	log_bd_t	*put;		/* next buffer descriptor to write */
	log_bd_t	*take;		/* next buffer descriptor to read */
	double		*log_counter;	/* pointer to free running clock */
	u_long		npt;		/* nanoseconds per tick */
	u_long		flags;		/* trace flags */
#define				TR_EXTENDED	1
#define				TR_OVERWRITE	2
#define				TR_CLOCK_SYNC	4
	u_long		trace_in_load;	/* avg overhead for trace_in() call */
	u_long		trace_out_load;	/* avg overhead for trace_out() call */
} log_config_t;

/*
 *  getstat/setstat flavors
 */
#define	TR_FLAGS	1	/* set/get trace flags */
#define	TR_GET_RD_SIZE	2	/* get size of trace buffer */
#define	TR_RESET	3	/* reset/calibrate driver (retains old thread)*/
#define	TR_SET_THREAD	4	/* set thread, 0=all, odd=set but don't start */
#define	TR_START	5	/* start trace (clear low order thread bit) */
#define	TR_STOP		6	/* stop trace (set low order thread bit) */
