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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: random.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:40 $";
#endif
/*
 * FUNCTIONS: random, srandom, setstate, initstate
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 * 
 * random.c	1.4  com/lib/c/gen,3.1,8943 10/16/89 09:21:05
 */

#include	<stdio.h>
#ifdef _THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex _random_rmutex;
#endif

/*
 * FUNCTION: random number generation operations
 *
 */


#ifdef MSG
#include "libc_msg.h"
#define MSGSTR(n,s)	NLgetamsg(MF_LIBC,MS_LIBC,n,s)
#else
#define MSGSTR(n,s)	(s)
#endif
extern long random(void);

/*
 * random.c:
 * An improved random number generation package.  In addition to the standard
 * rand()/srand() like interface, this package also has a special state info
 * interface.  The initstate() routine is called with a seed, an array of
 * bytes, and a count of how many bytes are being passed in; this array is then
 * initialized to contain information for random number generation with that
 * much state information.  Good sizes for the amount of state information are
 * 32, 64, 128, and 256 bytes.  The state can be switched by calling the
 * setstate() routine with the same array as was initiallized with initstate().
 * By default, the package runs with 128 bytes of state information and
 * generates far better random numbers than a linear congruential generator.
 * If the amount of state information is less than 32 bytes, a simple linear
 * congruential R.N.G. is used.
 * Internally, the state information is treated as an array of longs; the
 * zeroeth element of the array is the type of R.N.G. being used (small
 * integer); the remainder of the array is the state information for the
 * R.N.G.  Thus, 32 bytes of state information will give 7 longs worth of
 * state information, which will allow a degree seven polynomial.  (Note: the 
 * zeroeth word of state information also has some other information stored
 * in it -- see setstate() for details).
 * The random number generation technique is a linear feedback shift register
 * approach, employing trinomials (since there are fewer terms to sum up that
 * way).  In this approach, the least significant bit of all the numbers in
 * the state table will act as a linear feedback shift register, and will have
 * period 2^deg - 1 (where deg is the degree of the polynomial being used,
 * assuming that the polynomial is irreducible and primitive).  The higher
 * order bits will have longer periods, since their values are also influenced
 * by pseudo-random carries out of the lower bits.  The total period of the
 * generator is approximately deg*(2**deg - 1); thus doubling the amount of
 * state information has a vast influence on the period of the generator.
 * Note: the deg*(2**deg - 1) is an approximation only good for large deg,
 * when the period of the shift register is the dominant factor.  With deg
 * equal to seven, the period is actually much longer than the 7*(2**7 - 1)
 * predicted by this formula.
 */



/*
 * For each of the currently supported random number generators, we have a
 * break value on the amount of state information (you need at least this
 * many bytes of state info to support this random number generator), a degree
 * for the polynomial (actually a trinomial) that the R.N.G. is based on, and
 * the separation between the two lower order coefficients of the trinomial.
 */

#define		TYPE_0		0		/* linear congruential */
#define		BREAK_0		8
#define		DEG_0		0
#define		SEP_0		0

#define		TYPE_1		1		/* x**7 + x**3 + 1 */
#define		BREAK_1		32
#define		DEG_1		7
#define		SEP_1		3

#define		TYPE_2		2		/* x**15 + x + 1 */
#define		BREAK_2		64
#define		DEG_2		15
#define		SEP_2		1

#define		TYPE_3		3		/* x**31 + x**3 + 1 */
#define		BREAK_3		128
#define		DEG_3		31
#define		SEP_3		3

#define		TYPE_4		4		/* x**63 + x + 1 */
#define		BREAK_4		256
#define		DEG_4		63
#define		SEP_4		1


/*
 * Array versions of the above information to make code run faster -- relies
 * on fact that TYPE_i == i.
 */

#define		MAX_TYPES	5		/* max number of types above */

static  int		degrees[ MAX_TYPES ]	= { DEG_0, DEG_1, DEG_2,
								DEG_3, DEG_4 };

static  int		seps[ MAX_TYPES ]	= { SEP_0, SEP_1, SEP_2,
								SEP_3, SEP_4 };



/*
 * Initially, everything is set up as if from :
 *		initstate( 1, &randtbl, 128 );
 * Note that this initialization takes advantage of the fact that srandom()
 * advances the front and rear pointers 10*rand_deg times, and hence the
 * rear pointer which starts at 0 will also end up at zero; thus the zeroeth
 * element of the state information, which contains info about the current
 * position of the rear pointer is just
 *	MAX_TYPES*(rptr - state) + TYPE_3 == TYPE_3.
 */

static  long		randtbl[ DEG_3 + 1 ]	= { TYPE_3,
			    0x9a319039, 0x32d9c024, 0x9b663182, 0x5da1f342, 
			    0xde3b81e0, 0xdf0a6fb5, 0xf103bc02, 0x48f340fb, 
			    0x7449e56b, 0xbeb1dbb0, 0xab5c5918, 0x946554fd, 
			    0x8c2e680f, 0xeb3d799f, 0xb11ee0b7, 0x2d436b86, 
			    0xda672e2a, 0x1588ca88, 0xe369735d, 0x904f35f7, 
			    0xd7158fd6, 0x6fa6f051, 0x616e6b96, 0xac94efdc, 
			    0x36413f93, 0xc622c298, 0xf5a42ab8, 0x8a88d77b, 
					0xf5ad9d0e, 0x8999220b, 0x27fb47b9 };

/*
 * fptr and rptr are two pointers into the state info, a front and a rear
 * pointer.  These two pointers are always rand_sep places aparts, as they cycle
 * cyclically through the state information.  (Yes, this does mean we could get
 * away with just one pointer, but the code for random() is more efficient this
 * way).  The pointers are left positioned as they would be from the call
 *			initstate( 1, randtbl, 128 )
 * (The position of the rear pointer, rptr, is really 0 (as explained above
 * in the initialization of randtbl) because the state table pointer is set
 * to point to randtbl[1] (as explained below).
 */

static  long		*fptr			= &randtbl[ SEP_3 + 1 ];
static  long		*rptr			= &randtbl[ 1 ];



/*
 * The following things are the pointer to the state information table,
 * the type of the current generator, the degree of the current polynomial
 * being used, and the separation between the two pointers.
 * Note that for efficiency of random(), we remember the first location of
 * the state information, not the zeroeth.  Hence it is valid to access
 * state[-1], which is used to store the type of the R.N.G.
 * Also, we remember the last location, since this is more efficient than
 * indexing every time to find the address of the last element to see if
 * the front and rear pointers have wrapped.
 */

static  long		*state			= &randtbl[ 1 ];

static  int		rand_type		= TYPE_3;
static  int		rand_deg		= DEG_3;
static  int		rand_sep		= SEP_3;

static  long		*end_ptr		= &randtbl[ DEG_3 + 1 ];



/*
 * srandom:
 * Initialize the random number generator based on the given seed.  If the
 * type is the trivial no-state-information type, just remember the seed.
 * Otherwise, initializes state[] based on the given "seed" via a linear
 * congruential generator.  Then, the pointers are set to known locations
 * that are exactly rand_sep places apart.  Lastly, it cycles the state
 * information a given number of times to get rid of any initial dependencies
 * introduced by the L.C.R.N.G.
 * Note that the initialization of randtbl[] for default usage relies on
 * values produced by this routine.
 */

srandom( x )
unsigned	x;
{
    	register int	i;

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_random_rmutex);
#endif

	if(  rand_type  ==  TYPE_0  )  {
	    state[ 0 ] = x;
	}
	else  {
	    state[ 0 ] = x;
	    for( i = 1; i < rand_deg; i++ )  {
		state[i] = 1103515245*state[i - 1] + 12345;
	    }
	    fptr = &state[ rand_sep ];
	    rptr = &state[ 0 ];
	    for( i = 0; i < 10*rand_deg; i++ )  random();
	}

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_random_rmutex);
#endif
}



/*
 * initstate:
 * Initialize the state information in the given array of n bytes for
 * future random number generation.  Based on the number of bytes we
 * are given, and the break values for the different R.N.G.'s, we choose
 * the best (largest) one we can and set things up for it.  srandom() is
 * then called to initialize the state information.
 * Note that on return from srandom(), we set state[-1] to be the type
 * multiplexed with the current value of the rear pointer; this is so
 * successive calls to initstate() won't lose this information and will
 * be able to restart with setstate().
 * Note: the first thing we do is save the current state, if any, just like
 * setstate() so that it doesn't matter when initstate is called.
 * Returns a pointer to the old state.
 */

char  *
initstate( seed, arg_state, n )
unsigned	seed;			/* seed for R. N. G. */
char		*arg_state;		/* pointer to state array */
int		n;			/* # bytes of state info */
{
	register  char		*ostate		= (char *)( &state[ -1 ] );

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_random_rmutex);
#endif

	if(  rand_type  ==  TYPE_0  )  state[ -1 ] = rand_type;
	else  state[ -1 ] = MAX_TYPES*(rptr - state) + rand_type;
	if(  n  <  BREAK_1  )  {
	    if(  n  <  BREAK_0  )  {
		fprintf( stderr, MSGSTR(M_INITST, 
"initstate: not enough state (%d bytes) with which to do operation; ignored.\n"), 
                	n );
#ifdef _THREAD_SAFE
		rec_mutex_unlock(&_random_rmutex);
#endif
		return(NULL);
	    }
	    rand_type = TYPE_0;
	    rand_deg = DEG_0;
	    rand_sep = SEP_0;
	}
	else  {
	    if(  n  <  BREAK_2  )  {
		rand_type = TYPE_1;
		rand_deg = DEG_1;
		rand_sep = SEP_1;
	    }
	    else  {
		if(  n  <  BREAK_3  )  {
		    rand_type = TYPE_2;
		    rand_deg = DEG_2;
		    rand_sep = SEP_2;
		}
		else  {
		    if(  n  <  BREAK_4  )  {
			rand_type = TYPE_3;
			rand_deg = DEG_3;
			rand_sep = SEP_3;
		    }
		    else  {
			rand_type = TYPE_4;
			rand_deg = DEG_4;
			rand_sep = SEP_4;
		    }
		}
	    }
	}
	state = &(  ( (long *)arg_state )[1]  );	/* first location */
	end_ptr = &state[ rand_deg ];	/* must set end_ptr before srandom */
	srandom( seed );
	if(  rand_type  ==  TYPE_0  )  state[ -1 ] = rand_type;
	else  state[ -1 ] = MAX_TYPES*(rptr - state) + rand_type;
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_random_rmutex);
#endif
	return( ostate );
}



/*
 * setstate:
 * Restore the state from the given state array.
 * Note: it is important that we also remember the locations of the pointers
 * in the current state information, and restore the locations of the pointers
 * from the old state information.  This is done by multiplexing the pointer
 * location into the zeroeth word of the state information.
 * Note that due to the order in which things are done, it is OK to call
 * setstate() with the same state as the current state.
 * Returns a pointer to the old state information.
 */

char  *
setstate( arg_state )
char		*arg_state;
{
	register  long		*new_state	= (long *)arg_state;
	register  int		type		= new_state[0]%MAX_TYPES;
	register  int		rear		= new_state[0]/MAX_TYPES;
	char			*ostate		= (char *)( &state[ -1 ] );

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_random_rmutex);
#endif

	if(  rand_type  ==  TYPE_0  )  state[ -1 ] = rand_type;
	else  state[ -1 ] = MAX_TYPES*(rptr - state) + rand_type;
	switch(  type  )  {
	    case  TYPE_0:
	    case  TYPE_1:
	    case  TYPE_2:
	    case  TYPE_3:
	    case  TYPE_4:
		rand_type = type;
		rand_deg = degrees[ type ];
		rand_sep = seps[ type ];
		break;

	    default:
		fprintf( stderr, MSGSTR(M_SETST,
		"setstate: state info has been damaged; not changed.\n") );
	}
	state = &new_state[ 1 ];
	if(  rand_type  !=  TYPE_0  )  {
	    rptr = &state[ rear ];
	    fptr = &state[ (rear + rand_sep)%rand_deg ];
	}
	end_ptr = &state[ rand_deg ];		/* set end_ptr too */
	return( ostate );

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_random_rmutex);
#endif
}



/*
 * random:
 * If we are using the trivial TYPE_0 R.N.G., just do the old linear
 * congruential bit.  Otherwise, we do our fancy trinomial stuff, which is the
 * same in all ther other cases due to all the global variables that have been
 * set up.  The basic operation is to add the number at the rear pointer into
 * the one at the front pointer.  Then both pointers are advanced to the next
 * location cyclically in the table.  The value returned is the sum generated,
 * reduced to 31 bits by throwing away the "least random" low bit.
 * Note: the code takes advantage of the fact that both the front and
 * rear pointers can't wrap on the same call by not testing the rear
 * pointer if the front one has wrapped.
 * Returns a 31-bit random number.
 */

long
random(void)
{
	long		i;
	
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_random_rmutex);
#endif
	if(  rand_type  ==  TYPE_0  )  {
	    i = state[0] = ( state[0]*1103515245 + 12345 )&0x7fffffff;
	}
	else  {
	    *fptr += *rptr;
	    i = (*fptr >> 1)&0x7fffffff;	/* chucking least random bit */
	    if(  ++fptr  >=  end_ptr  )  {
		fptr = state;
		++rptr;
	    }
	    else  {
		if(  ++rptr  >=  end_ptr  )  rptr = state;
	    }
	}
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_random_rmutex);
#endif
	return( i );
}

#ifdef _THREAD_SAFE
/*
 * The thread-safe versions of the random routines.
 * The interfaces are similar, but completely re-entrant.
 */
#include <random.h>
#include <errno.h>

int
srandom_r(x, data)
unsigned		x;
register RANDOMD	*data;
{
    	int	i, j;

	if((data == NULL) || (data->state == NULL)) {
		seterrno(EINVAL);
		return(-1);
	}
	if(data->rand_type  ==  TYPE_0  )  {
	    data->state[ 0 ] = x;
	} else  {
	    data->state[ 0 ] = x;
	    for(i = 1; i < data->rand_deg; i++)  {
		data->state[i] = 1103515245*data->state[i - 1] + 12345;
	    }
	    data->fptr = &data->state[ data->rand_sep ];
	    data->rptr = &data->state[ 0 ];
	    for( i = 0; i < 10*data->rand_deg; i++ )
		random_r(data, &j);
	}
	return(0);
}

int
initstate_r(seed, arg_state, n, data)
unsigned		seed;		/* seed for R. N. G. */
char			*arg_state;	/* pointer to data->state array */
int			n;		/* # bytes of data->state info */
register RANDOMD	*data;
{
	if((arg_state == NULL) || (data == NULL)) {
		return (EINVAL);
	}
	if(n < BREAK_1)  {
	    if(n < BREAK_0)  {
		fprintf(stderr, MSGSTR(M_INITST, 
"initstate: not enough state (%d bytes) with which to do operation; ignored.\n"), n);
		seterrno(ENOMEM);
		return(-1);
	    }
	    data->rand_type = TYPE_0;
	    data->rand_deg = DEG_0;
	    data->rand_sep = SEP_0;
	} else {
	    if(n < BREAK_2)  {
		data->rand_type = TYPE_1;
		data->rand_deg = DEG_1;
		data->rand_sep = SEP_1;
	    } else {
		if(n < BREAK_3)  {
		    data->rand_type = TYPE_2;
		    data->rand_deg = DEG_2;
		    data->rand_sep = SEP_2;
		} else {
		    if(n < BREAK_4)  {
			data->rand_type = TYPE_3;
			data->rand_deg = DEG_3;
			data->rand_sep = SEP_3;
		    } else {
			data->rand_type = TYPE_4;
			data->rand_deg = DEG_4;
			data->rand_sep = SEP_4;
		    }
		}
	    }
	}
	data->state = &(((long *)arg_state)[1]);	/* first location */

	/* must set data->end_ptr before srandom */
	data->end_ptr = &data->state[data->rand_deg];
	srandom_r(seed, data);
	if(data->rand_type == TYPE_0)
		data->state[-1] = data->rand_type;
	else
		data->state[-1] = MAX_TYPES*(data->rptr - data->state) +
							data->rand_type;
	return(0);
}

char *
setstate_r(arg_state, rd)
char			*arg_state;
register RANDOMD	*rd;
{
	register  long		*new_state	= (long *)arg_state;
	register  int		type		= new_state[0]%MAX_TYPES;
	register  int		rear		= new_state[0]/MAX_TYPES;
	char			*ostate		= (char *)(&rd->state[-1]);
	long			*state		= rd->state;
	long			rand_type	= rd->rand_type;

	if(rand_type == TYPE_0)
		state[-1] = rand_type;
	else
		state[-1] = MAX_TYPES*(rd->rptr - state) + rand_type;
	switch(type) {
	    case  TYPE_0:
	    case  TYPE_1:
	    case  TYPE_2:
	    case  TYPE_3:
	    case  TYPE_4:
		rd->rand_type = type;
		rd->rand_deg = degrees[type];
		rd->rand_sep = seps[type];
		break;

	    default:
		fprintf(stderr, MSGSTR(M_SETST,
		"setstate: state info has been damaged; not changed.\n"));
	}
	state = rd->state = &new_state[1];
	if(rand_type  !=  TYPE_0)  {
	    rd->rptr = &state[rear];
	    rd->fptr = &state[(rear + rd->rand_sep)%rd->rand_deg];
	}
	rd->end_ptr = &state[rd->rand_deg];		/* set end_ptr too */
	return(ostate);
}

random_r(RANDOMD *data, int *randval)
{
	int	i;

	if((data == NULL) || (data->state == NULL)) {
		seterrno(EINVAL);
		return (-1);		/* illegal value for random_r() */
	}

	if(data->rand_type == TYPE_0) {
	    i = data->state[0] = (data->state[0]*1103515245+12345)&0x7fffffff;
	} else {
	    *data->fptr += *data->rptr;
	    i = (*data->fptr >> 1)&0x7fffffff;	/* chucking least random bit */
	    if(++data->fptr >= data->end_ptr)  {
		data->fptr = data->state;
		++data->rptr;
	    } else {
		if(++data->rptr >= data->end_ptr)
			data->rptr = data->state;
	    }
	}
	*randval = i;
	return(0);
}
#endif	/* _THREAD_SAFE */
