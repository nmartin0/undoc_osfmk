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
static char rcsid[] = "@(#)$RCSfile: localeconv.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:53 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: localeconv
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * sccsid[] = "localeconv.c      1.12  com/lib/c/gen,3.1,9021 2/11/90 18:08:09";
 */

#include <locale.h>
#include <sys/localedef.h>


#ifdef _THREAD_SAFE
#include <errno.h>
#include "rec_mutex.h"

extern struct rec_mutex	_locale_rmutex;

/*
 *
 * FUNCTION: Localization support function for international character support.
 *	     localeconv_r() sets the components of an object with type struct
 *           lconv with values appropriate for the formatting of numeric
 *	     quantities according to the rules of the current locale.
 *
 * PARAMETERS: 
 *	     struct lconv * xlconv  -- address of a structure to be filled in 
 *				       by the function.
 *	     char *buf -- buffer to be used to hold the strings.
 *	     int buflen -- length of buffer.
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns ESUCCESS on success, EINVAL if parameter is NULL.
 *	     - members of the structure may return NULL to indicate that
 *	       the value is not available in the current locale
 *
 * EXTERNAL PROCEDURES CALLED:
 *	     NLgetenv_r () - return the current value of the parameter
 *	                    according to the current locale
 *
 */


int
localeconv_r(register struct lconv * xlconv,
	     register char *buf,
	     register int buflen)
{
	register int size;
	mon_t *envmon = _locp->lc_montbl;
	num_t *envnum = _locp->lc_numtbl;
	
/* The buffer buf is used to hold a series of strings.  This macro
 * doles out the buffer space and makes sure that it doesn't 
 * overflow */
#define LCONVSET_S(field, table)  \
	if ((size = (strlen(table->field) + 1)) >= buflen) { \
		seterrno(ENOMEM); \
		rec_mutex_unlock(&_locale_rmutex); \
		return(-1); \
	} \
	xlconv->field = strcpy(buf, table->field); \
	buflen -= size; \
	buf += size; \
	}

	IF ((XLCONV == NULL) || (BUF == NULL)) {
		SETERRNO (EINVAL);
		RETURN (-1);
	}
	
	rec_mutex_lock(&_locale_rmutex);

	LCONVSET_S (decimal_point, envnum);
	LCONVSET_S (thousands_sep, envnum);
   	LCONVSET_S (grouping, envnum);
   	LCONVSET_S (int_curr_symbol, envmon);
   	LCONVSET_S (currency_symbol, envmon);
   	LCONVSET_S (mon_decimal_point, envmon);
   	LCONVSET_S (mon_thousands_sep, envmon);
   	LCONVSET_S (mon_grouping, envmon);
   	LCONVSET_S (positive_sign, envmon);
   	LCONVSET_S (negative_sign, envmon);
   	xlconv.int_frac_digits = envmon->int_frac_digits;
   	xlconv.frac_digits = envmon->frac_digits;
   	xlconv.p_cs_precedes = envmon->p_cs_precedes;
   	xlconv.p_sep_by_space = envmon->p_sep_by_space;
   	xlconv.n_cs_precedes = envmon->n_cs_precedes;
   	xlconv.n_sep_by_space = envmon->n_sep_by_space;
   	xlconv.p_sign_posn = envmon->p_sign_posn;
   	xlconv.n_sign_posn = envmon->n_sign_posn;

	rec_mutex_unlock(&_locale_rmutex);
	return(0);
}

#else /* _THREAD_SAFE */

/*
 *
 * FUNCTION: Localization support function for international character support.
 *	     localeconv() sets the components of an object with type struct
 *           lconv with values appropriate for the formatting of numeric
 *	     quantities according to the rules of the current locale.
 *
 * PARAMETERS: 
 *	     none
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns a pointer to a structure of type lconv. (see locale.h)
 *	     - members of the structure may return NULL to indicate that
 *	       the value is not available in the current locale
 *
 * EXTERNAL PROCEDURES CALLED:
 *	     NLgetenv () - return the current value of the parameter
 *	                    according to the current locale
 *
 */

static struct lconv xlconv;

struct lconv *
localeconv(void)
{
	mon_t *envmon = _locp->lc_montbl;
	num_t *envnum = _locp->lc_numtbl;
	
	xlconv.decimal_point = envnum->decimal_point;
	xlconv.thousands_sep = envnum->thousands_sep;
   	xlconv.grouping = envnum->grouping;
   	xlconv.int_curr_symbol = envmon->int_curr_symbol;
   	xlconv.currency_symbol = envmon->currency_symbol;
   	xlconv.mon_decimal_point = envmon->mon_decimal_point;
   	xlconv.mon_thousands_sep = envmon->mon_thousands_sep;
   	xlconv.mon_grouping = envmon->mon_grouping;
   	xlconv.positive_sign = envmon->positive_sign;
   	xlconv.negative_sign = envmon->negative_sign;
   	xlconv.int_frac_digits = envmon->int_frac_digits;
   	xlconv.frac_digits = envmon->frac_digits;
   	xlconv.p_cs_precedes = envmon->p_cs_precedes;
   	xlconv.p_sep_by_space = envmon->p_sep_by_space;
   	xlconv.n_cs_precedes = envmon->n_cs_precedes;
   	xlconv.n_sep_by_space = envmon->n_sep_by_space;
   	xlconv.p_sign_posn = envmon->p_sign_posn;
   	xlconv.n_sign_posn = envmon->n_sign_posn;
	return(&xlconv);
}
#endif /* _THREAD_SAFE */
