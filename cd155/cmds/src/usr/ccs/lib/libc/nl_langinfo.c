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
static char rcsid[] = "@(#)$RCSfile: nl_langinfo.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:17 $";
#endif

/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: nl_langinfo
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
 * static char sccsid[] = "@(#)nl_langinfo.c	1.12  com/lib/c/gen,3.1,9021 3/28/90 15:55:15";
 */

#include <stddef.h>
#include <nl_types.h>
#include <langinfo.h>
#include <locale.h>
#include <sys/localedef.h>
#ifdef _THREAD_SAFE
#include <errno.h>
#include "rec_mutex.h"

extern struct rec_mutex	_locale_rmutex;
#endif
 
#ifndef NULL
#define NULL	0
#endif

#ifndef	__STDC__
#undef	offsetof
#define offsetof(s_name, s_member) (size_t)&(((s_name *)0)->s_member)
#endif __STDC__

/* NAME: nl_langinfo
 *
 * FUNCTION: Information retrieval function concerning the active 
 *	     language or cultural area. The active language or 
 *	     cultural area is determined by the default value of 
 *	     environment variables or the most recent setlocale()
 *	     call. NLgetenv does all the work needed to retrieve
 *           the information.	
 *
 * PARAMETERS: item - The element to be retrieved which is defined in
 *		      langinfo.h
 *
 * RETURN VALUE DESCRIPTIONS:
 *		Return the value corresponding to the item desired for
 *		current locale. If the locale table does not have any value
 *		for the item desired, return the corresponding value of
 *	 	the C locale.
 *	        
 *	
 */                                                                   

/*
** C locale information 
** The following C locale default values are specified as per C locale
** requirements and these values are returned if the current locale has no
** values for these items. (Those C locale values that are NULLs are not
** included here.
*/

/*
**  LC_NUMERIC information
*/

const static char *C_DEC_PNT= ".";

/*
** LC_TIME information
*/

const static char *C_NLTIME 	= "%H:%M:%S";
const static char *C_NLDATE 	= "%m/%d/%y";
const static char *C_NLLDATE	= "%b %d %Y";
const static char *C_NLDATIM	= "%a %b %d %H:%M:%S %Y";
const static char *C_NLSDAY	= "Sun:Mon:Tue:Wed:Thu:Fri:Sat";
const static char *C_NLLDAY	= "Sunday:Monday:Tuesday:Wednesday:Thursday:Friday:Saturday";
const static char *C_NLSMONTH	= "Jan:Feb:Mar:Apr:May:Jun:Jul:Aug:Sep:Oct:Nov:Dec";
const static char *C_NLLMONTH	= "January:February:March:April:May:June:July:August:September:October:November:December";
const static char *C_NLYEAR	= "19890108,Heisei:19261225,Showa:";
const static char *C_NLTMISC	= "at:each:every:on:through:am:pm:zulu";
const static char *C_NLTSTRS	= "now:yesterday:tomorrow:noon:midnight:next:weekdays:weekend:today";
const static char *C_NLTUNITS	= "minute:minutes:hour:hours:day:days:week:weeks:month:months:year:years:min:mins";
const static char *C_AMPMSTR	= "AM:PM";

/*
** LC_MESSAGES information
*/

const static char *C_YESSTR	= "yes:y:Y";
const static char *C_NOSTR	= "no:n:N";

const static char *C_NULL	= NULL;

#ifdef _THREAD_SAFE
#define RETURN(val)	{ \
	strncpy(rbuf, val, buflen); \
	rec_mutex_unlock(&_locale_rmutex); \
	return(0); \
}
#else
#define RETURN(val)	return(val)
#endif

#define	MAXLOCAL	6

static char *get_elem(char *to, char *from, int elem); /* to get the element separated by ':' in a string */ 


struct	item_info {	/* descriptor for item type */
	nl_item		item;		/* this item */
	int		category;	/* category in locale tables */
	size_t		offset;		/* offset from start of tables */
	const char	**deflt;	/* default value if not set */
	int		elemno;		/* element in multi-element string */
};

#define	ELEMNOALL	(-1)		/* want entire string */

const static struct item_info all_items[] = {
	{ D_T_FMT, LC_TIME, offsetof(tim_t, d_t_fmt), &C_NLDATIM, ELEMNOALL },
	{ D_FMT, LC_TIME, offsetof(tim_t, d_fmt), &C_NLDATE,ELEMNOALL },
	{ T_FMT, LC_TIME, offsetof(tim_t, t_fmt), &C_NLTIME, ELEMNOALL },
	{ AM_STR, LC_TIME, offsetof(tim_t, am_pm), &C_AMPMSTR, 0 },
	{ PM_STR, LC_TIME, offsetof(tim_t, am_pm), &C_AMPMSTR, 1 },
	{ DAY_1, LC_TIME, offsetof(tim_t, day), &C_NLLDAY, 0 },
	{ DAY_2, LC_TIME, offsetof(tim_t, day), &C_NLLDAY, 1 },
	{ DAY_3, LC_TIME, offsetof(tim_t, day), &C_NLLDAY, 2 },
	{ DAY_4, LC_TIME, offsetof(tim_t, day), &C_NLLDAY, 3 },
	{ DAY_5, LC_TIME, offsetof(tim_t, day), &C_NLLDAY, 4 },
	{ DAY_6, LC_TIME, offsetof(tim_t, day), &C_NLLDAY, 5 },
	{ DAY_7, LC_TIME, offsetof(tim_t, day), &C_NLLDAY, 6 },
	{ ABDAY_1, LC_TIME, offsetof(tim_t, abday), &C_NLSDAY, 0 },
	{ ABDAY_2, LC_TIME, offsetof(tim_t, abday), &C_NLSDAY, 1 },
	{ ABDAY_3, LC_TIME, offsetof(tim_t, abday), &C_NLSDAY, 2 },
	{ ABDAY_4, LC_TIME, offsetof(tim_t, abday), &C_NLSDAY, 3 },
	{ ABDAY_5, LC_TIME, offsetof(tim_t, abday), &C_NLSDAY, 4 },
	{ ABDAY_6, LC_TIME, offsetof(tim_t, abday), &C_NLSDAY, 5 },
	{ ABDAY_7, LC_TIME, offsetof(tim_t, abday), &C_NLSDAY, 6 },
	{ MON_1, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 0 },
	{ MON_2, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 1 },
	{ MON_3, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 2 },
	{ MON_4, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 3 },
	{ MON_5, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 4 },
	{ MON_6, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 5 },
	{ MON_7, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 6 },
	{ MON_8, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 7 },
	{ MON_9, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 8 },
	{ MON_10, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 9 },
	{ MON_11, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 10 },
	{ MON_12, LC_TIME, offsetof(tim_t, mon), &C_NLLMONTH, 11 },
	{ ABMON_1, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 0 },
	{ ABMON_2, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 1 },
	{ ABMON_3, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 2 },
	{ ABMON_4, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 3 },
	{ ABMON_5, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 4 },
	{ ABMON_6, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 5 },
	{ ABMON_7, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 6 },
	{ ABMON_8, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 7 },
	{ ABMON_9, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 8 },
	{ ABMON_10, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 9 },
	{ ABMON_11, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 10 },
	{ ABMON_12, LC_TIME, offsetof(tim_t, abmon), &C_NLSMONTH, 11 },

	{ RADIXCHAR, LC_NUMERIC, offsetof(num_t, decimal_point), &C_DEC_PNT, ELEMNOALL },
	{ THOUSEP, LC_NUMERIC, offsetof(num_t, thousands_sep), &C_NULL, ELEMNOALL },

	{ YESSTR, LC_MESSAGES, offsetof(msg_t, yes_string), &C_YESSTR, 0 },
	{ NOSTR, LC_MESSAGES, offsetof(msg_t, no_string), &C_NOSTR, 0 },

	{ CRNCYSTR, LC_MONETARY, offsetof(mon_t, currency_symbol), &C_NULL, ELEMNOALL }
};

const static int nitems = sizeof(all_items) / sizeof (all_items[0]);



#ifdef  _THREAD_SAFE
int
nl_langinfo_r(nl_item item, char *rbuf, int buflen)
#else
char *
nl_langinfo(nl_item item)
#endif
{
	int i;
	const	struct	item_info	*iptr;
	char	**valptr;
	char	*val;
	static	char	tmpbuf[512];
	char	*tables[MAXLOCAL];

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_locale_rmutex);
#endif

	tables[LC_TIME] = (char *)(_locp->lc_timtbl);
	tables[LC_NUMERIC] = (char *)(_locp->lc_numtbl);
	tables[LC_MESSAGES] = (char *)(_locp->lc_msgtbl);
	tables[LC_MONETARY] = (char *)(_locp->lc_montbl);

	for (i = 0, iptr = all_items; i < nitems; i++, iptr++) {

		if (iptr->item == item) {

			/* Found; get the value */

			valptr = (char **)(tables[iptr->category] + iptr->offset);
			val = *valptr;

			if (val == NULL)
				val = (char *)(*(iptr->deflt));

			if (iptr->elemno != ELEMNOALL)
				val = get_elem(tmpbuf, val, iptr->elemno);

			RETURN(val);
		}
	}

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_locale_rmutex);
	seterrno(EINVAL);
	return(-1);
#else
	return("");			/* Empty string on invalid input */
#endif
}


/*
 * NAME: get_elem
 *                                                                    
 * FUNCTION: To get the element separated by ':' in a string 
 *	     containing names of the weekday or names of the
 *	     month. Note that ':' can be escaped also.
 *                                                                    
 *
 * RETURN VALUE DESCRIPTION: Return the null terminated string   
 *			     
 */  
static char *
get_elem(t,s,n)
char *t;
char *s;
int n;
{
	char *p; /* to hold the beginning address of the t  */
	int i;   /* index */

	p = t;

	/*
	**	If the string obtained from NLgetenv()
	**	is 0, no processing to be done.
	*/
	if(s == 0 || *s == 0) {
		*p = '\0';
		return(t);
	}
	
	/* skip the ':' by the offset */
	for (i=0; i<n; i++) {
		while( (*s) && (*s != '\n') ){
			/* End of field is when we see an unescaped ':' */
			if( (*s == ':') && (*(s-1) != '\\'))
				break;	/* Colon separator got */
			s++;
		}
		if(*s == '\n' || *s == 0)
			return(0);
		s++;	/* Get next field */
	}

	/* Beginning of the field at this point */

	 while (*s != '\n' && *s != '\0'){
                if ( *s == ':') 	
			if ( *(s-1) == '\\') 
				--p;	/* Overwrite escape */
                	else 
                        	break; /* Colon sep got */

                *p++ = *s++;	
        }
	
	*p ='\0';
	return(t);
}
