/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992 Intel Corporation.
 *
 *
 * HISTORY
 * $Log: pfs_emath.c,v $
 * Revision 1.7  1994/11/18  20:24:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/02/04  19:46:37  brad
 * Modified extended math support so that: 1) Emath routines set a new
 * error parameter instead of relying on a return value of -1 on overflow.
 * The latter method did not handle valid return values of -1 (this caused
 * eseek with resulting offset of -1 to return EQESIZE instead of EINVAL,
 * for example).  2) The emath code can be reused by libesize.a and libnx.a,
 * instead of having multiple copies of the same code in different places.
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #:
 *  Testing: Ran PFS EATs, ran emath tests.
 *  Module(s): fsvr_user_side.c pfs2_user_side.c pfs_emath.c pfs_fdt.h
 *             pfs_iomode.c pfs_tokenmgt.c pfs_user_side.c
 *
 * Revision 1.5  1994/01/07  02:18:15  brad
 * Merged pfs_emath.c changes from R1.2.
 *
 * Revision 1.2.6.1  1994/01/06  19:00:29  brad
 * Checking in results of an extended math code review by Greg Tensa.
 *
 *  Reviewer: Greg Tensa, Brad Rullman.
 *  Risk: Low.
 *  Benefit or PTS #: 7150,5958,5213,5215,5216,1632.
 *  Testing: Ran PFS EATs.
 *  Module(s): emulator/pfs_emath.c
 *
 * Revision 1.4  1994/01/05  17:08:47  brad
 * Fixed lint warnings in PFS-related code.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Some PFS source now passes lint
 *  Testing: Ran PFS EATs
 *  Module(s): emulator/emul_callback.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs2_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_fdt.h
 *             emulator/pfs_iomode.c
 *             emulator/pfs_tokenmgt.c
 *             emulator/pfs_user_side.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr2.defs
 *             server/uxkern/fsvr2_server_side.c
 *             server/uxkern/fsvr_types.defs
 *             server/uxkern/pfs2.defs
 *
 * Revision 1.3  1993/12/23  01:47:13  brad
 * Fixed various compilers warnings, lint errors, and lint warnings.
 *
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #: None.
 *  Testing: Booted and ran minimal PFS tests.
 *  Module(s): emulator/emul_stack_alloc.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_user_side.c
 *             emulator/pfs2_user_side.c
 *             server/pfs/pfs_vfsops.c
 *             server/uxkern/fsvr_types.defs
 *             server/uxkern/fsvr_server_side.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr_types.h
 *             server/uxkern/pfs2.defs
 *
 * Revision 1.2  1993/04/03  03:18:23  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.3  1993/03/10  06:28:30  brad
 * Removed _emin1() ... don't need it after all.
 *
 * Revision 1.1.2.2  1993/03/10  06:25:37  brad
 * Added _emin1().
 *
 * Revision 1.1.2.1  1993/02/04  00:36:49  brad
 * Corrected a comment.
 *
 * Revision 1.1  1993/01/20  19:50:46  brad
 * Initial revision
 *
 */

/*
 * Functions to perform extended arithmetic with esize_t variables.
 */

#ifdef	PFS

#include <sys/types.h>
#include <sys/param.h>
#include <sys/errno.h>

#define abs(x)	(x >= 0 ? x : -(x))


/*
 * Name:
 *	__eadd
 *
 * Description:	
 *	Add two extended numbers.
 *
 * Parameters:
 *	e1	First extended number.
 *
 *	e2	Second extended number.
 *
 *	error	Pointer to location for error return.
 *
 * Returns:
 *	The extended result of the addition e1 + e2, or an extended -1 if an
 *	overflow occurred.
 */
esize_t
__eadd(e1, e2, error)
esize_t	e1;
esize_t	e2;
int	*error;
{
	ulong 	sav;
	esize_t	sum;

	*error = ESUCCESS;
	sav = (ulong)e2.slow;
	sum.slow = 0;
	sum.shigh = 0;

	sum.slow = (ulong) e1.slow + (ulong) e2.slow;
	if (((ulong)sum.slow < (ulong)e1.slow) || ((ulong)sum.slow < sav))
		sum.shigh = e2.shigh + e1.shigh + 1;
	else 
		sum.shigh = e2.shigh + e1.shigh;

	/*
	 * Check for overflow or underflow (detected by sign change).
	 */
	if (((e1.shigh >= 0 && e2.shigh >= 0) && sum.shigh <  0) ||
	    ((e1.shigh <  0 && e2.shigh <  0) && sum.shigh >= 0)) {
		sum.slow = -1;
		sum.shigh = -1;
		*error = EQESIZE;
	}

	return(sum);
}


/*
 * Name:
 *	__eadd1
 *
 * Description:	
 *	Add an integer to an extended number, producing an extended result.
 *
 * Parameters:
 *	e	Extended number.
 *
 *	n	Integer (long).
 *
 *	error	Pointer to location for error return.
 *
 * Returns:
 *	The extended result of the addition e + n, or an extended -1 if an
 *	overflow occurred.
 */
esize_t
__eadd1(e, n, error)
esize_t	e;
long	n;
int	*error;
{
	esize_t e2;

	e2.slow = n;
	e2.shigh = (n < 0) ? -1 : 0;

	return(__eadd(e, e2, error));
}


/*
 * Name:
 *	__esub
 *
 * Description:	
 *	Subtract two extended numbers.
 *
 * Parameters:
 *	e1	Extended number to subtract from.
 *
 *	e2	Extended number to subtract from e1.
 *
 *	error	Pointer to location for error return.
 *
 * Returns:
 *	The extended result of the subtraction e1 - e2, or an extended -1 if
 *	an overflow occurred.
 */
esize_t
__esub(e1, e2, error)
esize_t	e1;
esize_t	e2;
int	*error;
{
	esize_t	diff;

	*error = ESUCCESS;
	diff.slow = 0;
	diff.shigh = 0;

	if ((ulong)e2.slow > (ulong)e1.slow) {
		diff.slow = (UINT_MAX - (ulong)e2.slow) + (ulong)e1.slow + 1;
		diff.shigh = e1.shigh - e2.shigh - 1;
	} else {
		diff.slow  = (ulong)e1.slow - (ulong)e2.slow;
		diff.shigh  = e1.shigh - e2.shigh;
	}

	/* 
	 * Check for overflow (detected by sign change).
	 */
	if (((e1.shigh < 0) && (e2.shigh >= 0) && (diff.shigh >= 0)) ||
	    ((e1.shigh >= 0) && (e2.shigh < 0) && (diff.shigh < 0))) {
		diff.slow = -1;
		diff.shigh = -1;
		*error = EQESIZE;
		return(diff);
	}

	return(diff);
}


/*
 * Name:
 *	__esub1
 *
 * Description:	
 *	Subtract an integer from an extended number, producing an extended
 *	result.
 *
 * Parameters:
 *	e	Extended number to subtract from.
 *
 *	n	Integer to subtract from e1.
 *
 *	error	Pointer to location for error return.
 *
 * Returns:
 *	The extended result of the subtraction e - n, or an extended -1 if
 *	an overflow occurred.
 */
esize_t
__esub1(e, n, error)
esize_t	e;
long	n;
int	*error;
{
	esize_t e2;

	e2.slow = n;
	e2.shigh = (n < 0) ? -1 : 0;

	return(__esub(e, e2, error));
}


/*
 * Name:
 *	__ecmp
 *
 * Description:	
 *	Compare two extended numbers.
 *
 * Parameters:
 *	e1	First extended number.
 *
 *	e2	Second extended number.
 *
 * Returns:
 *	The result of the compare:
 *
 *		e1 > e2:	returns  1
 *		e1 = e2:	returns  0
 *		e1 < e2:	returns -1
 */
long
__ecmp(e1, e2)
esize_t e1;
esize_t	e2;
{
	if ((e1.shigh > e2.shigh) || ((e1.shigh == e2.shigh) &&
				      ((ulong)e1.slow > (ulong)e2.slow)))
		return(1);

	if ((e1.shigh == e2.shigh) && (e1.slow == e2.slow))
		return(0);

	return(-1);
}


/*
 * Name:
 *	__emul
 *
 * Description:	
 *	Multiply an extended number by an integer.
 *
 * Parameters:
 *	e	Extended number.
 *
 *	n	Integer.
 *
 *	error	Pointer to location for error return.
 *
 * Returns:
 *	The extended result of the multiplication e * n, or an extended -1
 *	if an overflow occurred.
 */
esize_t
__emul(e, n, error)
esize_t	e;
long	n;
int	*error;
{
	esize_t	r1, r2;
	esize_t	temp_e	= e;
	long	temp_n	= n;
	int	nflg	= 0;

	if ((e.shigh == 0x80000000) && (e.slow == 0)) {
		/* Cannot convert to a positive number */
		r1.shigh = -1;
		r1.slow = -1;
		*error = EQESIZE;
		return(r1);
	}

	if (n < 0 && e.shigh < 0) {
		temp_n = -n;
		temp_e.slow = ~e.slow;
		temp_e.shigh = ~e.shigh;
		temp_e = __eadd1(temp_e, (long)1, error);
		nflg = 0;
	} else {
		if (n < 0) {
			temp_n = -n;
			nflg = 1;
		} else {
			if (e.shigh < 0) {
				temp_e.slow = ~e.slow;
				temp_e.shigh = ~e.shigh;
				temp_e = __eadd1(temp_e, (long)1, error);
				nflg = 1;
			}
		}
	}

	mulx(temp_e.slow, temp_n, &r1);
	mulx(temp_e.shigh, temp_n, &r2);
	if (r2.shigh != 0) {	/* Verify no overflow */
		r1.shigh = -1;
		r1.slow = -1;
		*error = EQESIZE;
		return(r1);
	}

	r2.shigh = r2.slow;
	r2.slow = 0;

	r1 = __eadd(r1, r2, error);
	if ((e.shigh >= 0 && n >= 0) && r1.shigh < 0 ) {
		r1.shigh = -1;
		r1.slow = -1;
		*error = EQESIZE;
		return(r1);
	}

	if (nflg) {
		r1.slow = ~r1.slow;
		r1.shigh = ~r1.shigh;
		r1 = __eadd1(r1, (long)1, error);
	}
		
	return(r1);
}


/*
 * Name:
 *	divx
 *
 * Description:	
 *	Divide an extended number by an integer.
 *
 * Parameters:
 *	e	Pointer to extended number.
 *
 *	d	Divisor; integer to divide into e.
 *
 *	q	Pointer to quotient; set by this function.
 *
 *	r	Pointer to remainder; set by this function.
 *
 * Returns:
 *	The quotient and remainder of e / d.
 */
		     /* 2 +  10  +  10  +  10  */
#define TWO32 ((double)4.0*1024.0*1024.0*1024.0)

void
divx(e, d, q, r)
esize_t	*e;
long	d;
ulong	*q, *r;
{
	double	f, fsave;
	esize_t	t1;
	ulong	ul;
	int	error;

	if (e->shigh >= 0) {
		ul = (ulong)e->slow;
		f = ((double)e->shigh) * (TWO32) + (double)ul;
	} else {
		e->shigh = ~e->shigh;
		e->slow = ~e->slow;
		*e = __eadd1(*e, (long)1, &error);
		ul = (ulong)e->slow;
		f = ((double)e->shigh) * (TWO32) + (double)ul;
		e->shigh = ~e->shigh;
		e->slow = ~e->slow;
		*e = __eadd1(*e, (long)1, &error);
		f = -f;
	}

	fsave = f;
	f = f / (double)d;
	*q = (ulong)f;
	t1.slow = d;
	t1.shigh = 0;
	t1 = __emul(t1, (long)*q, &error);
	t1 = __esub(*e, t1, &error);
	*r = (ulong)t1.slow;
	if (e->shigh >= 0) {
		if (t1.slow < 0) { /* Loss of precision - fix */
			*r += d;
			fsave = fsave - (double)*r;
			fsave = fsave / (double)d;
			*q = (ulong)fsave;
		} else if (((ulong)t1.slow) >= d) {
			*q += 1;
			*r -= d;
		}
	} else {
		if (((ulong)t1.slow) >= d) {
			*q -= 1;
			*r += d;
		}
	}
}


/*
 * Name:
 *	__emod
 *
 * Description:	
 *	Extended modulus operator.
 *
 * Parameters:
 *	e	Extended number.
 *
 *	n	Integer to divide e by.
 *
 *	error	Pointer to location for error return.
 *
 * Returns:
 *	The remainder of the division of e by n.
 */
long
__emod(e, n, error)
esize_t	e;
long	n;
int	*error;
{
	ulong 	q;
	ulong 	r;
	long	temp_n	= n;
	esize_t	temp_e	= e;
	esize_t	utemp_e;
	int	chg	= 0;
	
	*error = ESUCCESS;

	if ((e.shigh == 0x80000000) && (e.slow == 0)) {
		/* Cannot convert to a positive number */
		*error = EQESIZE;
		return(-1);
	}

	if (n < 0 && e.shigh < 0) {
		temp_n = -n;
		temp_e.slow = ~e.slow;
		temp_e.shigh = ~e.shigh;
		temp_e = __eadd1(temp_e, (long)1, error);
		chg = 1;
	} else {
		if (n < 0) {
			temp_n = -n;
		} else {
			if (e.shigh < 0) {
				temp_e.slow = ~e.slow;
				temp_e.shigh = ~e.shigh;
				temp_e = __eadd1(temp_e, (long)1, error);
				chg = 1;
			}
		}
	}

	if ((temp_n == 0) || (temp_e.shigh >= temp_n)) {
		*error = EQESIZE;
		return(-1);
	}

	if (!chg && (temp_e.slow < 0 || temp_e.shigh < 0)) {
		utemp_e.slow = (ulong)temp_e.slow;
		utemp_e.shigh = (ulong)temp_e.shigh;
		divx(&utemp_e, temp_n, &q, &r);
	} else {
		divx(&temp_e, temp_n, &q, &r);
	}

	if ((e.shigh >= 0 && n > 0) && (long)q < 0) {
		*error = EQESIZE;
		return(-1);
	}

	return((long)r);
}


/*
 * Name:
 *	__ediv
 *
 * Description:	
 *	Divide an extended number by an integer.
 *
 * Parameters:
 *	e	Extended number.
 *
 *	n	Divisor; integer to divide into e.
 *
 *	error	Pointer to location for error return.
 *
 * Returns:
 *	The 32-bit quotient of e / d, or a -1 if an overflow occured.
 */
long
__ediv(e, n, error)
esize_t	e;
long	n;
int	*error;
{
	ulong 	q;
	ulong 	r;
	long	temp_n	= n;
	esize_t	temp_e	= e;
	esize_t	utemp_e;
	int	nflg	= 0;
	int	chg	= 0;
	
	*error = ESUCCESS;

	if ((e.shigh == 0x80000000) && (e.slow == 0)) {
		/* Cannot convert to a positive number */
		*error = EQESIZE;
		return(-1);
	}

	if (n < 0 && e.shigh < 0) {
		temp_n = -n;
		temp_e.slow = ~e.slow;
		temp_e.shigh = ~e.shigh;
		temp_e = __eadd1(temp_e, (long)1, error);
		nflg = 0;
		chg = 1;
	} else {
		if (n < 0) {
			temp_n = -n;
			nflg = 1;
		} else {
			if (e.shigh < 0) {
				temp_e.slow = ~e.slow;
				temp_e.shigh = ~e.shigh;
				temp_e = __eadd1(temp_e, (long)1, error);
				nflg = 1;
				chg = 1;
			}
		}
	}

	if ((temp_n == 0) || ((temp_e.shigh >= temp_n))) {
		*error = EQESIZE;
		return(-1);
	}

	if (!chg && (temp_e.slow < 0 || temp_e.shigh < 0)) {
		utemp_e.slow = (ulong)temp_e.slow;
		utemp_e.shigh = (ulong)temp_e.shigh;
		divx(&utemp_e, temp_n, &q, &r);
	} else {
		divx(&temp_e, temp_n, &q, &r);
	}

	if ((e.shigh >= 0 && n > 0) && (long)q < 0) {
		*error = EQESIZE;
		return(-1);
	}

	if (nflg)
		return((long)(-q));
	return((long)q);
}


/*
 * Name:
 *	__etos
 *
 * Description:	
 *	Convert an extended number to a string.
 *
 * Parameters:
 *	e	Extended number.
 *
 *	s	Pointer to a character string (at least 20 characters in 
 *		length) which is filled by this function.
 *
 *	error	Pointer to location for error return.
 *
 * Returns:
 *	0 if successful, else -1 if an overflow occurred.
 */
long
__etos(e, s, error)
esize_t	e;
char	*s;
int	*error;
{
	ulong	q, r;
	int	nflg=0;

	*error = ESUCCESS;

	if (e.shigh < 0) {
		e.slow = ~e.slow;
		e.shigh = ~e.shigh;
		e = __eadd1(e, (long)1, error);
		nflg = 1;
	}

	/*
	 * Supports up to 0xffffffff * 1000000000 (approx. 2 to 60);
	 * otherwise q gets too big and overflows
	 */
	if (e.shigh > 999999999) {
		*error = EQESIZE;
		return(-1);
	}

	divx(&e, (long)1000000000, &q, &r);

	if (q) {
		if (nflg)
			sprintf(s, "-%lu%9.9lu", q, r);
		else
			sprintf(s, "%lu%9.9lu", q, r);
	} else {
		if (nflg)
			sprintf(s, "-%lu", r);
		else
			sprintf(s, "%lu", r);
	}

	return(0);
}


/*
 * Name:
 *	__etoxs
 *
 * Description:	
 *	Convert an extended number to a string containing an ASCII hexadecimal
 *	representation of the number.
 *
 * Parameters:
 *	e	Extended number.
 *
 *	s	Pointer to a character string (at least 20 characters in 
 *		length) which is filled by this function.
 *
 *	error	Pointer to location for error return.
 *
 * Returns:
 *	0 if successful, else -1 if an overflow occurred.
 */
long
__etoxs(e, s, error)
esize_t	e;
char	*s;
int	*error;
{
	ulong	q, r;
	int	nflg = 0;

	*error = ESUCCESS;

	if (e.shigh < 0) {
		e.slow = ~e.slow;
		e.shigh = ~e.shigh;
		e = __eadd1(e, (long)1, error);
		nflg = 1;
	}

	/*
	 * Supports up to 576460752303423455 = (2^59-33); otherwise q gets 
	 * too big and overflows.  The lower bound is less restrictive, so
	 * this check covers both.
	 */
	if ((abs(e.shigh) > 0x7ffffff) ||
	    ((abs(e.shigh) == 0x7ffffff) && (e.slow < 0) && (e.slow > -33))) {
		*error = EQESIZE;
		return(-1);
	}

	divx(&e, (long)0x10000000, &q, &r);
	if (q) {
		if (nflg)
			sprintf(s, "-%lx%7.7lx", q, r);
		else
			sprintf(s, "%lx%7.7lx", q, r);
	} else {
		if (nflg)
			sprintf(s, "-%lx", r);
		else
			sprintf(s, "%lx", r);
	}

	return(0);
}


/*
 * Name:
 *	__stoe
 *
 * Description:	
 *	Convert a string to an extended number.
 *
 * Parameters:
 *	s	Pointer to a null-terminated character string containing the
 * 		ASCII representation of a number.
 *
 *	error	Pointer to location for error return.
 *
 * Returns:
 *	The extended number represented by s, or an extended -1 if an
 *	overflow occurred.
 */
esize_t
__stoe(s, error)
char 	*s;
int	*error;
{
	esize_t	e, t;
	char	*p;
	int	nflg = 0;

	*error = ESUCCESS;
	e.slow = 0;
	e.shigh = 0;

	t = e;
	p = s;

	if (*p == '-') {
		nflg = 1;
		p++;
	} else {
		if (*p == '+')
			p++;
	}

	while (*p) {
		if (!(*p >= '0' && *p <= '9')) {
			e.shigh = -1; 
			e.slow = -1; 
			*error = EQESIZE;
			return(e);
		}
		e = __eadd1(__emul(e, (long)10, error),
			    (long)(*p - '0'),
			    error);
		if (__ecmp(e, t) < 0) {
			e.shigh = -1; 
			e.slow = -1; 
			*error = EQESIZE;
			return(e);
		}
		p++;
		t = e;
	}
	
	if (nflg) {
		e.slow = ~e.slow;
		e.shigh = ~e.shigh;
		e = __eadd1(e, (long)1, error);
	}

	return(e);
}


/*
 * Name:
 *	__xstoe
 *
 * Description:	
 *	Convert a string containing an ASCII hexadecimal representation of a 
 *	number to an extended number.
 *
 * Parameters:
 *	s	Pointer to a null-terminated character string containing the
 * 		ASCII representation of a hex number.
 *
 *	error	Pointer to location for error return.
 *
 * Returns:
 *	The extended number represented by s, or an extended -1 if an
 *	overflow occurred.
 */
esize_t
__xstoe(s, error)
char 	*s;
int	*error;
{
	esize_t	e, t;
	char	*p;
	int     nflg = 0;
	int	n;

	*error = ESUCCESS;
	e.slow = 0;
	e.shigh = 0;

	t = e;
	p = s;

	if (*p == '-') {
		nflg = 1;
		p++;
	} else {
		if (*p == '+')
			p++;
	}
	
	while (*p) {
		if (!(*p >= '0' && *p <= '9') &&
		    !(*p >= 'a' && *p <= 'f')) {
			e.shigh = -1; 
			e.slow = -1; 
			*error = EQESIZE;
			return(e);
		}

		if (*p >= '0' && *p <= '9')
			n = *p - '0';
		else
			n = *p - 'a' + 10;
		e = __eadd1(__emul(e, (long)16, error), (long)n, error);
		if (__ecmp(e, t) < 0) {
			e.shigh = -1; 
			e.slow = -1; 
			*error = EQESIZE;
			return(e);
		}
		p++;
		t = e;
	}
	
	if (nflg) {
		e.slow = ~e.slow;
		e.shigh = ~e.shigh;
		e = __eadd1(e, (long)1, error);
	}
	
	return(e);
}
#endif	PFS
