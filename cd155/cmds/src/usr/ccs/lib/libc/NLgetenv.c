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
static char rcsid[] = "@(#)$RCSfile: NLgetenv.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:49 $";
#endif
/*
 * FUNCTIONS: NLgetenv
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * NLgetenv.c	1.21  com/lib/c/gen,3.1,9013 2/28/90 16:20:51
 */

#include <stdio.h>
#include <string.h>
#include <sys/localedef.h>

#ifdef _THREAD_SAFE
#include <errno.h>
#include "rec_mutex.h"

extern struct rec_mutex	_locale_rmutex;
#endif

/*
**  Get an NLS parameter from the locale information
**  setup by a call to setlocale().
**  This parameter should belong in one of the following categories:
**  LC_MONETARY, LC_NUMERIC, LC_TIME, LC_MESSAGES.
**  If the information solicited is not found in the tables setup
**  by setlocale(), an American English default table is searched
**  and the value in that default is returned.  
**  Return a NULL pointer if no data can be found.
*/

#ifdef _THREAD_SAFE
int
NLgetenv_r(char *name, char *buf, int buflen)
#else /* _THREAD_SAFE */
char *
NLgetenv(name)
char *name;
#endif /* _THREAD_SAFE */
{
	extern loc_t *_locp;
	
	mon_t *envmon;
	num_t *envnum;
	tim_t *envtim;
	msg_t *envmsg;

#ifdef _THREAD_SAFE
#define	RETURN(s) \
	{ strncpy(buf, s, buflen); \
	  rec_mutex_unlock(&_locale_rmutex); \
	  return(0); \
	}
#else
#define	RETURN(s)	return(s)
#endif

#ifdef _THREAD_SAFE
	if (buf == NULL || buflen < 1) {
		seterrno(EINVAL);
		return(-1);
	}
#endif
	/* 
	** With name (string) as the key, search through
	** the loc_t structure pointed at by _locp for the
	** particular field of interest.
	** This is done by stepping through the
	** mon_t, num_t, time_t and msg_t structs.
	*/
#ifdef _THREAD_SAFE
	rec_mutex_lock(&_locale_rmutex);
#endif

	envmon = _locp->lc_montbl;
	envnum = _locp->lc_numtbl;
	envtim = _locp->lc_timtbl;
	envmsg = _locp->lc_msgtbl;
	
	/* LC_MONETARY */	

		/* international currency symbol */
	if(strcmp(name, "INT_CUR_SYM")==0)
		RETURN(envmon->int_curr_symbol);	

		/* national currency symbol */
	if(strcmp(name, "CUR_SYM")==0)
		RETURN(envmon->currency_symbol);

		/* currency decimal point */
	if(strcmp(name, "MON_DEC_PNT")==0)
		RETURN(envmon->mon_decimal_point);
			
		/* currency thousands separator */
	if(strcmp(name, "MON_THOUS")==0)
		RETURN(envmon->mon_thousands_sep);

		/* currency digits grouping */
	if(strcmp(name, "MON_GRP")==0)
 		RETURN(envmon->mon_grouping );

		/* currency plus sign */
	if(strcmp(name, "POS_SGN")==0)
		RETURN(envmon->positive_sign);

		/* currency minus sign */
	if(strcmp(name, "NEG_SGN")==0)
		RETURN(envmon->negative_sign );	

		/* internat currency fract digits */
	if(strcmp(name, "INT_FRAC")==0)
		RETURN((char *)&envmon->int_frac_digits);

		/*currency fractional digits */
	if(strcmp(name, "FRAC_DIG")==0)
		RETURN((char *)&envmon->frac_digits );

		/* currency plus location */
	if(strcmp(name, "P_CS_PRE")==0)
		RETURN((char *)&envmon->p_cs_precedes);	

		/* currency plus space ind. */
	if(strcmp(name, "P_SEP_SP")==0)
		RETURN((char *)&envmon->p_sep_by_space );	

		/* currency minus location */
	if(strcmp(name, "N_CS_PRE")==0)
		RETURN((char *)&envmon->n_cs_precedes );	

		/* currency minus space ind. */
	if(strcmp(name, "N_SEP_SP")==0)
		RETURN((char *)&envmon->n_sep_by_space );	

		/* currency plus position */
	if(strcmp(name, "P_SGN_POS")==0)
		RETURN((char *)&envmon->p_sign_posn );	

		/* currency minus position */
	if(strcmp(name, "N_SGN_POS")==0)
		RETURN((char *)&envmon->n_sign_posn );	

	/* LC_NUMERIC */

	if(strcmp(name, "DEC_PNT")==0)
		RETURN(envnum->decimal_point );

	if(strcmp(name, "THOUS_SEP")==0)
		RETURN(envnum->thousands_sep );

	if(strcmp(name, "GROUPING")==0)
		RETURN(envnum->grouping );
		
	/* LC_TIME */

		/* NLTIME; date %X descriptor */
	if(strcmp(name,"NLTIME")==0)
		RETURN(envtim->t_fmt );         
		
		/* NLDATE; date %x descriptor */
	if(strcmp(name,"NLDATE" ) ==0)
		RETURN(envtim->d_fmt );         

		/* NLLDATE  long form */
	if(strcmp(name,"NLLDATE")==0)
		RETURN(envtim->nlldate );       
		
		/* NLDATIM, date %c descriptor */
	if(strcmp(name,"NLDATIM")==0)
		RETURN(envtim->d_t_fmt );       

		/* NLSDAY; date %a descriptor */
	if(strcmp(name,"NLSDAY")==0)
		RETURN(envtim->abday );         

		/* NLLDAY; date %A descriptor */
	if(strcmp(name,"NLLDAY")==0)
		RETURN(envtim->day );           

		/* NLSMONTH; date %b descriptor */
	if(strcmp(name,"NLSMONTH")==0)
		RETURN(envtim->abmon );         

		/* NLLMONTH; date %B descriptor */
	if(strcmp(name,"NLLMONTH")==0)
		RETURN(envtim->mon );

		/* NLTMISC at;each;every;on;through */
	if(strcmp(name,"NLTMISC")==0)
		RETURN(envtim->misc );

		/* am and pm */
	if(strcmp(name,"AMPMSTR" )==0)
		RETURN(envtim->am_pm );
		
        	/* NLTSTRS */
	if(strcmp(name,"NLTSTRS")==0)
		RETURN(envtim->tstrs );       

		/* NLTUNITS */
	if(strcmp(name,"NLTUNITS" )==0)
		RETURN(envtim->tunits );       

		/* Name of the year and the starting time */
	if(strcmp(name,"NLYEAR" )==0)
		RETURN(envtim->year );		

	/* LC_MESSAGES */

		/* Message Catalog name */
	if(strcmp(name,"MESSAGES")==0)
		RETURN(envmsg->messages ) ;	
		
		/* Response string for affirmation */
	if(strcmp(name,"YESSTR")==0)
		RETURN(envmsg->yes_string );	
		
		/* Response string for negation */
	if(strcmp(name, "NOSTR")==0)
		RETURN(envmsg->no_string );
	
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_locale_rmutex);
	seterrno(ESRCH);
	return(-1);
#else
	return((char *)NULL);
#endif
}
