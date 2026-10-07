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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: setlocale.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:06 $";
#endif
/*
 *
 * LIBCGEN: setlocale
 *                                                                    
 * ORIGIN: IBM
 *
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1988
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 *                                                                    
 *     setlocale.c	1.27  com/lib/c/gen,3.1,9013 3/12/90 16:21:01
 *
 * FUNCTION: Localization support function for international character
 *	     support.  The setlocale function selects the appropriate
 *	     portion of the program's locale as specified by the "category"
 *	     and "locale" arguments. The setlocale function may be used
 *	     to change or query the program's entire current locale or
 *	     portions thereof.
 *
 * ARGUMENTS: category - The category of locale to be changed.  Following
 *	                  are valid choices: LC_COLLATE, LC_CTYPE, LC_MONETARY, 
 *					     LC_NUMERIC, LC_TIME, LC_MESSAGES, 
 *					     and LC_ALL.
 *
 *	       locale -   The string which designates certain conventions
 *	                  for the named locale.  
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	      If locale is a pointer to a valid locale, and the selection
 *	      can be honored, setlocale returns the string associated with
 *	      the new locale.  If the selection cannot be honored, a NULL
 *	      is returned. 
 *
 *    	      If locale is a null pointer, setlocale returns the string
 *	      associated with the category for the program's current locale.
 */
#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned short wchar_t;
#endif

#include	<sys/localedef.h>
#include	<sys/types.h>
#include	"setlocale.h"
#include	<string.h>
#include	<locale.h>
#include	<fcntl.h>
#include	<sys/stat.h>
#include	<sys/limits.h>
#include	<stdlib.h>
#include	<stddef.h>

#ifdef  _THREAD_SAFE
#include	<errno.h>
#include	"rec_mutex.h"

extern struct rec_mutex	_locale_rmutex;
#endif


#ifndef	__STDC__
#undef	offsetof
#define offsetof(s_name, s_member) (size_t)&(((s_name *)0)->s_member)
typedef	char *univ_t;
#else
typedef	void *univ_t;
#endif /* __STDC__ */

/*
*****************************************************************
**
**      Section:        Tables used in setlocale()
**
*****************************************************************
*/

/*
** This table lookup simplifies index to string conversion.
*/

static char *map_LC[MAXLOCAL]= {
                "LC_COLLATE",
                "LC_CTYPE" ,
                "LC_MONETARY" ,
                "LC_NUMERIC" ,
                "LC_TIME" ,
                "LC_MESSAGES"
};
	
		
/*
** The default value for the environment variable LOCPATH
** and the current value for directories to search.
*/
static char locpath_dflt[] = ".:/usr/lib/nls/loc:/etc/nls/loc";
static char *locpath;
static char *locpath_ptr;
static char *longlocale = NULL;		/* allocated if needed */
#define	LONGLOCSIZE	((PATH_MAX * MAXLOCAL) + MAXLOCAL)

/*
**	The following gives the correct sequence of calls to setlocale() before
**	a process starts.
**		1. The locale information is statically initialized at the
**		   time the library is built (in the liblocale.c file).
**		   No call to setlocale is required to use the locale information.
**		2. A user process can call setlocale(LC_ALL,"") to change the
**		   locale to any other locale.
**		 
**	
*/

/*
**	Main global variables: 
**	      	_locp is a loc_t pointer, initialized statically by the libloc
**		program.
**		_envp is a env_t pointer to a linked list of statically-initialized
**		env_t structures, initialized statically by the libloc program.
**	      
*/	

#ifdef _THREAD_SAFE
#ifdef _NO_PROTO
int
setlocale_r(category, locale, result)
int category; 
char *locale;
char *result;
#else	/* _NO_PROTO */
int
setlocale_r(int category, const char *locale, char *result)
#endif	/* _NO_PROTO */
#else	/* _THREAD_SAFE */
char *
#ifndef _NO_PROTO
setlocale( int category, const char *locale)
#else
setlocale( category, locale)
int category; 
char *locale;
#endif	/* _NO_PROTO */
#endif	/* _THREAD_SAFE */
{
	int 	i;
	struct	locale_info	info[MAXLOCAL];
	char	*temp = NULL;
	char	*res;

#ifdef	_THREAD_SAFE
#define RETURN(val) { \
	if (temp != NULL) \
		(void)free(temp); \
	if (result != NULL && (val) != NULL) \
		(void)strcpy(result, (val)); \
	rec_mutex_unlock(&_locale_rmutex); \
	return((val) == NULL? -1: 0); \
	}
#else
#define RETURN(val) { \
	if (temp != NULL) \
		(void)free(temp); \
	return((val)); \
	}
#endif

#ifdef	_THREAD_SAFE
	rec_mutex_lock(&_locale_rmutex);
#endif

	/*
	**	Validate category. 
	*/
	if ((category != LC_ALL) &&
	    ((category < LC_COLLATE) || (category >= MAXLOCAL)) )
		RETURN(NULL);

        /*
        **   A NULL pointer for locale causes the setlocale function to
        **   return the current locale for specified category.
        */
        if (locale == NULL) {
		res = current_loc(category);
		RETURN(res);
        }

	/* If we got one of the hokey space-separated locale strings in,
	 * we need to copy it to temporary storage so we can strtok()
	 * it later.  We'll free the temporary storage when we're
	 * done.
	 */

	if (strpbrk(locale, " ")) {
		temp = strdup((char *)locale);
		locale = temp;
	}

	/* Get the names of the locales to be changed, filling in the
	** name fields of the locale_info structure for each locale.
	** A name of NULL indicates that the current locale for that
	** category is not to be changed.
	**/

	if (get_locale_names(category, (char *)locale, info) == FAIL)
		RETURN( (char *) NULL);

	/*
	**	For each category to be changed, read the locale
	**	information into memory from the disk file.  The
	**	information may be found in the in-memory cache,
	**	or it may have to be read in from the disk file
	**	found through the locpath.
	*/
	locpath = NULL;

	for(i = 0;i < MAXLOCAL; i++) {

		if (info[i].name == NULL) /* no change needed */
			continue;

		/* Check disk file (and cache) for the locale */
		if(get_diskfile(i, &info[i]) == FOUND)
			continue;	/* Fetch next category */

		RETURN( (char *) NULL);

	}	/* For loop */

	/* All categories obtained. Update the new locale */

	(void)update_locale(info);

	/* Return the new locale for the category */

	res = current_loc(category);
	RETURN(res);
		
}

/*
**	Function:	Return a static string pointer to the current locale.
**
*/
static char *
#ifndef _NO_PROTO
current_loc( int category )
#else
current_loc( category)
int category;
#endif
{
	int	cat;
	char	*ret;

	if( category == LC_ALL) {

		category = LC_COLLATE;
		ret = lc_col_lcname;
		for (cat = category + 1; cat < MAXLOCAL; cat++) {

			if (strcmp(ret, current_loc(cat)) != 0) {

				/* If locales are not all the same, we
				 * have to return a string that allows
				 * us to re-construct the exact
				 * current locale.  We do this in a
				 * slightly (!) hokey way: return a
				 * space- separated string of all the
				 * locale names, constructed into an
				 * allocated buffer.
				 */

				if (longlocale == NULL)
					longlocale = malloc(LONGLOCSIZE);
				strcpy(longlocale, lc_col_lcname);
				for (cat = LC_COLLATE + 1; cat < MAXLOCAL; cat++) {
					strcat(longlocale, " ");
					strcat(longlocale, current_loc(cat));
				}
				return(longlocale);
			}
		}
		return(ret);
	}

	switch (category) {
	      case LC_COLLATE:
		ret = lc_col_lcname;
		break;
	      case LC_CTYPE:
		ret = lc_chr_lcname;
		break;
	      case LC_MONETARY:
		ret = lc_mon_lcname;
		break;
	      case LC_NUMERIC:
		ret = lc_num_lcname;
		break;
	      case LC_TIME:
		ret = lc_tim_lcname;
		break;
	      case LC_MESSAGES:
		ret = lc_msg_lcname;
		break;
	}
	return(ret);
}


/*
** 	Function:	Obtain default locale for the given category.
**			and map the locale to a file name.
** 	Called from main().
**	Input: 	category
**	Output: fill in the names in the info structure.
**	NOTE: may write into the locale string, if the locale string
**	contains spaces (ie. is a multi-locale string).
*/
static int
#ifndef _NO_PROTO
get_locale_names(int category, char *locale, struct locale_info *info )
#else
get_locale_names(category, locale, info )
int category;
char *locale;
struct locale_info *info;
#endif _NO_PROTO
{
	int 	i;
	char 	*locptr;
	char	*langptr;
	char	*cp;
	int	flag;

	flag = (category == LC_ALL ? FLAG_DOINGALL : FLAG_NONE);
	/* 
	** Obtain the new locale names from the the locale specifed,
	** the LC_ environment variables, or from LANG. If none set,
	** use default values.  Check to see whether locale is changing
	** for each category.
	*/

	/* Step through the categorie(s) in consideration */
	langptr = NULL;
	for (i = 0; i < MAXLOCAL; i++) {

		info[i].flags = flag;

		/* If not trying to change this category, skip it */

		if ((category != LC_ALL) && (category != i)) {

			info[i].name = NULL; /* show no change requested */
			continue;
		}

		/* If an explict locale name was supplied, use it */

		if (*locale != '\0') {

			info[i].name = locale;

			/* If it's one of the hokey space-separated locales,
			 * use just the first component for this locale name.
			 */

			if ((cp = strpbrk(locale, " ")) != NULL) {
				*cp = '\0';
				locale = ++cp;
			}
			continue;
		}

		/* 
		** The empty string was supplied as the locale.
		** If the environment variables LC_COLLATE,etc are set,
		** obtain the locale infor from those variables.
		*/

		locptr = (char *)getenv( map_LC[ i ] );
	
		/*
		** If the LC_* variable(s) are not set or are set to
		** the null string, try to obtain the locale info from LANG
		** environment variable.
		*/

		if( locptr == (char *)NULL || *locptr == '\0' ) {
			if (langptr == NULL)
				langptr = (char *)getenv("LANG");
			locptr = langptr;
		}

		if( locptr == (char *)NULL || *locptr == '\0' )
			/* Case of default */
			locptr = "C";	

		info[i].name = locptr;
	}

	/* Now having determined the requested locale name for each
	 * category, loop through again and check to see whether we
	 * are already using that locale.  If so, don't need to do
	 * any work for that category, so NULL out the name.
	 */

	for (i = 0; i < MAXLOCAL; i++) {

		if (info[i].name != NULL &&
		    strcmp(info[i].name, current_loc(i)) == 0)
			info[i].name = NULL;

		/* Validate the locale name lengths also */

		if ((info[i].name != NULL) && (strlen(info[i].name) > PATH_MAX))
			return(FAIL);
	}

	return(PASS);
}


/* Read in the locale information for the specified category from
 * the corresponding disk file.  Outer loop computes the disk file
 * name based on the locale path environment variable.  Inner loop
 * then checks for the specified locale, first in the cache, then
 * on the disk.  On success, fills in the env_t pointer in the
 * locale info to point to the env_t structure filled in with the
 * disk information.
 */

static
#ifndef _NO_PROTO
int get_diskfile( int lc_cat, struct locale_info *info)
#else
int get_diskfile( lc_cat, info)
int lc_cat; 
struct locale_info *info;
#endif
{
	char	pathbuf[PATH_MAX];
	char	*locptr;

	locptr = info->name;
	/*
	**  Obtain the value of the LOCPATH environment variable if
	**  it has not already been obtained.
	**  If LOCPATH is not set, use the default value.
	*/
	if (locpath == NULL) {
		locpath = (char *)getenv("LOCPATH");
		if(!getuid() || (locpath == (char *)NULL) || (*locpath == '\0'))
			locpath = locpath_dflt;
	}

	/*
	**  Initialize locpath_ptr to point to the start of locpath.
	*/
	locpath_ptr = locpath;

	/*
	**  Try to find the file in each directory indicated in locpath
	**  if the name is a relative pathname.  Quit when locpath is
	**  exhausted (get_filenames returns NOTFOUND).
	**  Otherwise, try the absolute pathname.
	*/
	do {
		if (get_filenames(lc_cat, info, pathbuf) == FOUND) {
			if(get_file(lc_cat, info, pathbuf) == FOUND) {
				return(FOUND);
			}
		} else
			return(NOTFOUND);
	} while ( *locptr != '/' );
	return(NOTFOUND);
}

static
#ifndef _NO_PROTO
int get_filenames( int lc_cat, struct locale_info *info, char *buf)
#else
int get_filenames( lc_cat, info, buf)
int lc_cat; 
struct locale_info *info;
char *buf;
#endif
{
	int sp;
	char *locptr;

	locptr = info->name;
	/* 
	**  Obtain  the file name.
	**  If the locale is an absolute file pathname, use it.
	**  Otherwise, get the directory from locpath pointed to by
	**  locpath_ptr and append the locale to it to form an 
	**  absolute file pathname.
	*/
	if( *locptr == '/' ){
		strcpy(buf, info->name);
		/*
		 * If get_filenames is being called for all
		 * categories because the original category
		 * was LC_ALL, the flag is equal to FLAG_DOINGALL.
		 * In this case, append ".en" to an absolute
		 * pathname for categories that need it.
		 */
		if ((lc_cat != LC_CTYPE) && (lc_cat != LC_COLLATE) &&
		    (info->flags & FLAG_DOINGALL)) {
			if ((strlen(buf) + 4) > PATH_MAX){
				return(NOTFOUND);
			}else{
				strcat( buf, ".en");
			}
		}
	} else {
		/*
		 ** locpath_ptr points to the next directory in
		 ** locpath to try.
		 */
		while (*locpath_ptr == ':')
			++locpath_ptr;
		sp = strcspn(locpath_ptr, ":");
		if (sp == 0)
			return(NOTFOUND);
		if( (sp + strlen("/") + strlen(locptr) + 2 ) > PATH_MAX )
			return(NOTFOUND);

		strncpy( buf, locpath_ptr, (size_t)sp);
		buf[sp] = 0;
		strcat( buf, "/");
		strcat( buf, locptr);
		/*
		 **  Update locpath_ptr to point to the next
		 **  directory in locpath to try.
		 */
		locpath_ptr += sp;

		if( ( lc_cat != LC_COLLATE ) && (lc_cat != LC_CTYPE) ) {
			if (strlen(buf) + 4 > PATH_MAX)
				return(NOTFOUND);
			strcat( buf, ".en" ) ;
		}
	}

	return(FOUND);
}


static int
#ifndef _NO_PROTO
get_file( int lc_cat, struct locale_info *info, char *localefile)
#else
get_file( lc_cat, info, path)
int lc_cat;
struct locale_info *info;
char *localefile;
#endif
{
	/* 
	**   Get appropriate portion of programs locale as specified by
	**   the category and locale arguments.  Save information in
	**   newenv field of info structure for this category.
        */

	switch(lc_cat) {

		case LC_COLLATE:
		case LC_CTYPE:
			/*
			** get_ctab() mallocs and places data at
			** buffer pointed to by newenv.
			*/
			if( get_ctab( lc_cat, info, localefile) == FAIL) 
				return (NOTFOUND);

			break;
		default:
			/*
			** The following is executed only if the category 
			** is one of:
			** LC_MONETARY, LC_NUMERIC, LC_TIME and LC_MESSAGES.
			*/

			if ( get_locinfo( lc_cat, info, localefile) == FAIL)
				return(NOTFOUND);

			break;

	} /* end switch */

	return(FOUND);
}	


static env_t *
#ifndef _NO_PROTO
lookup_oldenv(char *locale, int category)
#else 
lookup_oldenv(locale, category)
char *locale;
int category;
#endif
{
	env_t	*oldenvp;

	oldenvp = _envp;			/* Global pointer */
	while (oldenvp) {
		if (strcmp( ((category == LC_CTYPE) || (category == LC_COLLATE)) ?
			   oldenvp->locale :
			   oldenvp->locale_en, 
			   locale )==0)
			return(oldenvp);
		oldenvp = oldenvp->next;
	}
	return(NULL);
}


static int
#ifndef _NO_PROTO
alloc_newenv( char *locale, int category)
#else
alloc_newenv( locale, category)
char *locale;
int category;
#endif

{
	env_t	 *newenvp;

	newenvp = (env_t *)malloc(sizeof(env_t) );
	if(newenvp == NULL)
		return(FAIL);
	switch (category) {
	      case LC_COLLATE:
	      case LC_CTYPE:
		strcpy(newenvp->locale, locale );
		break;
	      default:
		strcpy(newenvp->locale_en, locale);
		break;
	}
	newenvp->buffer = NULL;
	newenvp->next = _envp;
	_envp = newenvp;		/* Update the global current env pointer */
	return(PASS);

}


/*
	get_ctab -   Reads a file containing collation and character type
		    information for categories:  LC_CTYPE and LC_COLLATION.

	Arguments:  category: LC_COLLATE or LC_CTYPE only. 
		    info: ptr to info for this category
		    locfile: full pathname of the locale file to read.

	Returns:    pass/fail

 */

static int
#ifndef _NO_PROTO
get_ctab( int category, struct locale_info *info, char *locfile)
#else
get_ctab( category, info, locfile)
int category; 
struct locale_info *info;
char *locfile;
#endif
{

	int 	cfile;
	int	locfsize;
	struct	stat	statbuf;
	env_t 	*oldenvp; 
	loc_t	*ctabptr;
	char	*locale_name;

	oldenvp = lookup_oldenv(info->name, category);
	if(oldenvp != NULL) {
		info->newenv = oldenvp;
		return(PASS);
	}
	
	/*
	** At this stage, either the locale is not in the linked list
	** at all, or a partially-filled copy is in the list 
	** by a previous call to setlocale() and this has to be 
	** augmented by the current call.
	*/

	if ((cfile = open(locfile, O_RDONLY, 0)) < 0) 
		return(FAIL);

	/*
	** Allocate space for the ctab table.  
	*/
	
	if (fstat(cfile, &statbuf) < 0) {
		close(cfile);
		return(FAIL);
	}
	locfsize = statbuf.st_size;
    	if( (ctabptr = (loc_t *)malloc((unsigned)locfsize)) == NULL){
		close(cfile);
		return(FAIL);
	}
    	if( read(cfile, ctabptr, locfsize)  != locfsize) {
		(void)close(cfile);
		free(ctabptr);
		return(FAIL);
	}
	(void)close(cfile);
    	if( 	ctabptr->lc_mag0 != NLCTMAG0 ||
		ctabptr->lc_mag1 != NLCTMAG1 ){
			free(ctabptr);
			return(FAIL);
	}

	/*
	** 	The envp structure will copy the collation /character tables
	**	only from the disk structure. 
	*/
	if( alloc_newenv(info->name, category) == FAIL){
		free(ctabptr);
		return(FAIL);
	}
		
	reloc_ctab(ctabptr);
	reloc_ctype(ctabptr);	/* Relocate ctype table too */

	locale_name = strdup(info->name);
	ctabptr->lc_coltbl->lc_locale_name  = locale_name;
	ctabptr->lc_chrtbl->lc_locale_name = locale_name;

	_envp->loc_info     = *ctabptr;
	info->newenv = _envp;

	return(PASS);
}


/*
	get_locinfo  Puts local information from locale specified by
		     LC_* category into locale environment 
		     definition list.

	Arguments:   locname  -  Name of a locale
	             category -  Name of a category		
			
	Returns:     pass/fail

	Function: -  reads in a file that contains locale specific 
		     name=value pairs.  The name=value pairs contain
		     information about time, month, days of the week,
		     currency, etc.  
		     Read a given locale file only one time. Obtain all 4
		     categories (LC_MONETARY..LC_MESSAGES.) information 
		     and save it for later use.  There is overhead incurred here 
		     if the locale is not same for all the categories as we
		     parese strings foe all categories.
*/

static int
#ifndef _NO_PROTO
get_locinfo( int category, struct locale_info *info, char *locfile)
#else
get_locinfo( category, info, locfile)
int category; 
struct locale_info *info;
char *locfile;
#endif
{

	int 	fd;
	struct	stat	statbuf;
	int	locfsize;
	env_t	*oldenvp;
	char	*infoptr;


	oldenvp = lookup_oldenv(info->name, category);
	if(oldenvp != NULL) {
		info->newenv = oldenvp;
		return(PASS);
	}

	/*
	**	The locale file is not read from the disk yet.
	**	Read and parse it and save it in the linked list.
	*/
	if ((fd = open(locfile, O_RDONLY, 0)) < 0)
		return (FAIL);

	if (fstat(fd, &statbuf) < 0) {
		close(fd);
		return(FAIL);
	}
	locfsize = statbuf.st_size;
	if( (infoptr = (char *)malloc(locfsize + 1)) == NULL) {
		close(fd);
		return(FAIL);
	}

	if (read(fd, infoptr, (unsigned)locfsize) != locfsize) {
		close(fd);
		free(infoptr);
		return (FAIL);
	}
	close (fd);

	if( alloc_newenv(info->name, category) == FAIL){
		free(infoptr);
		return(FAIL);
	}

	/*
	**   Parse the entries in the buffer. ALL strings will be terminated 
	**   with a NULL character.
	**   Only pointers, and not strings, are copied from the
	**   infoptr to the newenvp.
	*/

	if(parse_entries(infoptr, info->name)  == FAIL){
		free(infoptr);
		return (FAIL);
	}

	info->newenv = _envp;

	return(PASS);

}

static int 
#ifndef _NO_PROTO
parse_entries( char *ptr, char *locale)
#else
parse_entries( ptr, locale)
char *ptr;
char *locale;
#endif
{

	char 	*line,	/* Pointer to the beginning of a line */
		*locale_name,
		*lhsval,
		*rhsval,
		*bufp,
	 	*delimiter; 

	mon_t	*envmon;
	num_t	*envnum;
	tim_t	*envtim;
	msg_t	*envmsg;
	
	char	*tables[MAXLOCAL];

	/* The parse tables are the lists of items from the evironment
	 * file that have string values.  For each item, we list:
	 *   the keyword
	 *   the table (ie category) to which it belongs
	 *   the offset from the start of the table of the variable
	 *   which is to hold its value.
	 * A NULL keyword indicates end of table.  There are separate parse
	 * tables for each type of value (currently just strings and
	 * integers).
	 */

	struct	parse_table {
		char	*token;		/* item name */
		int	category;	/* table to store item in */
		size_t	offset;		/* offset from start of table */
	};

	const struct	parse_table	*tbl_ptr;

	const struct parse_table strings_table[] = {
		/* LC_MONETARY */
		{ "INT_CUR_SYM", LC_MONETARY, offsetof(mon_t, int_curr_symbol) },
		{ "CUR_SYM", LC_MONETARY, offsetof(mon_t, currency_symbol) },
		{ "MON_DEC_PNT", LC_MONETARY, offsetof(mon_t, mon_decimal_point) },
		{ "MON_THOUS", LC_MONETARY, offsetof(mon_t, mon_thousands_sep) },
		{ "MON_GRP", LC_MONETARY, offsetof(mon_t, mon_grouping) },
		{ "POS_SGN", LC_MONETARY, offsetof(mon_t, positive_sign) },
		{ "NEG_SGN", LC_MONETARY, offsetof(mon_t, negative_sign) },
		/* LC_NUMERIC */
		{ "DEC_PNT", LC_NUMERIC, offsetof(num_t, decimal_point) },
		{ "THOUS_SEP", LC_NUMERIC, offsetof(num_t, thousands_sep) },
		{ "GROUPING", LC_NUMERIC, offsetof(num_t, grouping) },
		/* LC_TIME */
		{ "NLTIME", LC_TIME, offsetof(tim_t, t_fmt) },
		{ "NLDATE", LC_TIME, offsetof(tim_t, d_fmt) },
		{ "NLLDATE", LC_TIME, offsetof(tim_t, nlldate) },
		{ "NLDATIM", LC_TIME, offsetof(tim_t, d_t_fmt) },
		{ "NLSDAY", LC_TIME, offsetof(tim_t, abday) },
		{ "NLLDAY", LC_TIME, offsetof(tim_t, day) },
		{ "NLSMONTH", LC_TIME, offsetof(tim_t, abmon) },
		{ "NLLMONTH", LC_TIME, offsetof(tim_t, mon) },
		{ "NLTMISC", LC_TIME, offsetof(tim_t, misc) },
		{ "AMPMSTR", LC_TIME, offsetof(tim_t, am_pm) },
		{ "NLTSTRS", LC_TIME, offsetof(tim_t, tstrs) },
		{ "NLTUNITS", LC_TIME, offsetof(tim_t, tunits) },
		{ "NLYEAR", LC_TIME, offsetof(tim_t, year) },
		/* LC_MESSAGES */
		{ "MESSAGES", LC_MESSAGES, offsetof(msg_t, messages) },
		{ "YESSTR", LC_MESSAGES, offsetof(msg_t, yes_string) },
		{ "NOSTR", LC_MESSAGES, offsetof(msg_t, no_string) },

		{ NULL, -1, 0 }
	};

	const struct parse_table char_table[] = {
		/* LC_MONETARY */
		{ "INT_FRAC", LC_MONETARY, offsetof(mon_t, int_frac_digits) },
		{ "FRAC_DIG", LC_MONETARY, offsetof(mon_t, frac_digits) },
		{ "P_CS_PRE", LC_MONETARY, offsetof(mon_t, p_cs_precedes) },
		{ "P_SEP_SP", LC_MONETARY, offsetof(mon_t, p_sep_by_space) },
		{ "N_CS_PRE", LC_MONETARY, offsetof(mon_t, n_cs_precedes) },
		{ "N_SEP_SP", LC_MONETARY, offsetof(mon_t, n_sep_by_space) },
		{ "P_SGN_POS", LC_MONETARY, offsetof(mon_t, p_sign_posn) },
		{ "N_SGN_POS", LC_MONETARY, offsetof(mon_t, n_sign_posn) },

		{ NULL, -1, 0 }
	};

	locale_name = strdup(locale);

	/*
	** Parse the info in the malloced buffer to create a sorted
	** list of locale info. 
	*/

	envmon = (mon_t *)malloc(sizeof(mon_t));
	_envp->le_montbl  =envmon;
	envmon->lc_locale_name = locale_name; 
	tables[LC_MONETARY] = (char *)envmon;

	envnum = (num_t *)malloc(sizeof(num_t) );
	_envp->le_numtbl = envnum;
	envnum->lc_locale_name = locale_name; 
	tables[LC_NUMERIC] = (char *)envnum;

	envtim = (tim_t *)malloc(sizeof(tim_t) );
	_envp->le_timtbl= envtim; 
	envtim->lc_locale_name = locale_name; 
	tables[LC_TIME] = (char *)envtim;

	envmsg = (msg_t *)malloc(sizeof(msg_t) );
	_envp->le_msgtbl = envmsg;
	envmsg->lc_locale_name = locale_name; 
	tables[LC_MESSAGES] = (char *)envmsg;
	

	/*
	**	The ptr points at the locale information
	**	for all the categories. The parsing is done
	**	to get the information pertaining to all these
	**	categories. Later, the information for the category
	**	of interest is processed.
	*/
	

	bufp = (char *)ptr;
	for(;;){
		/*
		**	get_token() returns a NULL on end of buffer.
		**	It returns a line containing LHS=RHS pair with a 
		**	NULL termination. The bufp points to the beginning
		**	of the next line each time it is called.
		*/ 
		if( (line = (char *)get_line(&bufp) ) == NULL ) 
			break ;		/* No more info */


		/*
		** Obtain the LHS and RHS separately. 
		** Error check and return on no occurence of '=' in the line.
		*/
		if(!(rhsval = (char *)strpbrk(line, "=") ))
			continue;
		/*
		** Break at any occurence of ' ', '\t' or '='.
		*/
		delimiter = (char *)strpbrk(line, " \t=");
		*delimiter = '\0';
		/*
		** Look past '=' sign and white space 
		*/
		++rhsval;
		rhsval  += strspn(rhsval, " \t");
		/*
		** The lhsval is a string that can be mapped into an index 
		** (integer ) value.
		** Save the rhsval in the localeinfo[] structure at the place 
		** corresponding to the index value.
		*/
		lhsval = line;
		/* 
		** Search for a match of the lhs value (string)
		** so that the env structure can point to the rhs value.
		** First try the table of items with string values, then
		** the table of items with char values.
		*/
	
		for (tbl_ptr = strings_table; tbl_ptr->token != NULL; tbl_ptr++) {

			char	**ptr;

			if (strcmp(lhsval, tbl_ptr->token) != 0)
				continue;

			ptr = (char **)(tables[tbl_ptr->category] + tbl_ptr->offset);
			*ptr = rhsval;
			goto found;
		}


		/*
		** Not in strings table; try table of items with char values.
		*/

		for (tbl_ptr = char_table; tbl_ptr->token != NULL; tbl_ptr++) {

			char	*ptr;

			if (strcmp(lhsval, tbl_ptr->token) != 0)
				continue;

			ptr = (char *)(tables[tbl_ptr->category] + tbl_ptr->offset);
			if(strcmp(rhsval, "CHAR_MAX")==0)
				*ptr =  CHAR_MAX;
			else
				*ptr = atoi(rhsval);
			goto found;
		}

		/* String wasn't found, so just ignore it */

found:
		;			/* null statement */
	};

	return (PASS);
}

		
/*
**	Allow for commnet character (#) in the first position only.
*/
	
static 
#ifndef _NO_PROTO
char *get_line(char **sbufp)
#else
char *get_line(sbufp)
char **sbufp;
#endif
{
	char 	*bp, 
		*p, 
		*newline;

	int 	linegot = 0;

	

	/*
	**	Validate the buffer pointer before starting the
	**	parse of the buffer to obtain expressions of the
	**	type LHS=RHS.
	**
	*/
	if (sbufp == NULL)
		return(NULL);

	p = (char *)*sbufp;
	
		/* LC_MONETARY */

	while (p && !linegot) {
		/* No more lines */
		p += strspn(p, "\n"  ) ;
		if ( *p == '\0' )
			break;

		/* A comment line.  Just continue. */
		if (*p == '#') {
			if ( (p = (char *)strchr(p, (int)'\n')) == NULL)
				break;
			p++;
			continue;
		}
	
		bp = (char *)strchr(p, (int)'\n');
		if(bp == NULL){
			p = (char *)NULL; 	
			break;
		}

		/*
		 * Advance buffer pointer past new-line or to null if 
		 * if there is no new-line.
		 */
		newline = (char *)p; /* Pointer to the beginning of the line */
		*bp = '\0';
		p = ++bp;
		linegot++;
	} 

	
	*sbufp = (char *)p; 	/* Point to the next line in the buffer */
	if(linegot)
		return((char *)newline);
	else
		return(NULL);

}

static 
#ifndef _NO_PROTO
void reloc_ctab( loc_t *ctab)
#else
void reloc_ctab( ctab)
loc_t *ctab;
#endif
{


	col_t	*coltbl;

	coltbl = (col_t *)((char *)ctab + (long)ctab->lc_coltbl);
	ctab->lc_coltbl = (col_t *)coltbl;

	if (coltbl->len_collate) {
		coltbl->lc_collate = (short *)
			((char *)ctab + (long)coltbl->lc_collate);
	}
	if (coltbl->len_coluniq) {
		coltbl->lc_coluniq = (short *)
			((char *)ctab + (long)coltbl->lc_coluniq);
	}
	if (coltbl->len_coldesc) {
		coltbl->lc_coldesc = (coldesc_t *)
			((char *)ctab + (long)coltbl->lc_coldesc);
	}
	if (coltbl->len_strings) {
		coltbl->lc_strings = (wchar_t *) 
			((char *)ctab + (long)coltbl->lc_strings);
	}
	
}



static void
#ifndef _NO_PROTO
reloc_ctype( loc_t * ctab)
#else
reloc_ctype( ctab)
loc_t * ctab;
#endif
{
	ctype_t	*chrtbl;

	chrtbl = (ctype_t *)( (char *)ctab + (long)ctab->lc_chrtbl) ;
	ctab->lc_chrtbl = (ctype_t *)chrtbl;

	if (chrtbl->len_caseconv) {
		chrtbl->lc_caseconv = (wchar_t *)( (char *)ctab + 
					(long)chrtbl->lc_caseconv);
	}
	if (chrtbl->len_ctype) {
		chrtbl->lc_ctype = (unsigned short *)( (char *)ctab + 
					(long)chrtbl->lc_ctype);
	}
}

/*
**	Function: update_locale()
**		The loc_t pointer _locp is updated to point to current
**		locale values, from the specified locale_info structure.
**	
*/
	
static void
#ifndef _NO_PROTO
update_locale(struct locale_info *info)
#else
update_locale(info)
struct locale_info *info;
#endif
{
	int 	lc_cat;

	for (lc_cat = 0; lc_cat < MAXLOCAL; lc_cat++){

		/* Only update locales that have changed */

		if (info[lc_cat].name != NULL) {
			switch (lc_cat){
			      case LC_COLLATE:
				copy_collate(info[lc_cat].newenv);
				break;
			      case LC_CTYPE:
				copy_ctype(info[lc_cat].newenv);
				break;
			      case LC_MONETARY:
				copy_monetary(info[lc_cat].newenv);
				break;
			      case LC_NUMERIC:
				copy_numeric(info[lc_cat].newenv);
				break;
			      case LC_TIME:
				copy_time(info[lc_cat].newenv);
				break;
			      case LC_MESSAGES:
				copy_messages(info[lc_cat].newenv);
				break;
			      default:
				break;
			}
		}
	}
	return;
}
			
	
static int
#ifndef _NO_PROTO
copy_collate(env_t *p)
#else
copy_collate( p)
env_t *p;
#endif
{
	/* Copy LC_COLLATE locale info */

	_locp->lc_mag0      = p->le_mag0;
	_locp->lc_mag1      = p->le_mag1;
	_locp->lc_version   = p->le_version;
	_locp->lc_code_type = p->le_code_type;
	_locp->lc_length    = p->le_length;

	_locp->lc_coltbl = (col_t *)p->le_coltbl;

	return(PASS);
}
static int
#ifndef _NO_PROTO
copy_ctype( env_t *p)
#else
copy_ctype( p)
env_t *p;
#endif
{
	/*
	** 	Note that the ctabptr points to the loc_t structure
	** 	that is already relocated, so that the offsets in the
	**	disk file are converted into absolute addresses.
	*/

	_locp->lc_mag0      = p->le_mag0;
	_locp->lc_mag1      = p->le_mag1;
	_locp->lc_version   = p->le_version;
	_locp->lc_code_type = p->le_code_type;
	_locp->lc_length    = p->le_length;
	
	_locp->lc_chrtbl = (ctype_t *)p->le_chrtbl;

	return(PASS);

}

static int
#ifndef _NO_PROTO
copy_monetary( env_t *p)
#else
copy_monetary( p)
env_t *p;
#endif
{
	/* LC_MONETARY */

	_locp->lc_montbl = (mon_t *)p->le_montbl;
	
	return(PASS);
}
static int
#ifndef _NO_PROTO
copy_numeric( env_t *p)
#else
copy_numeric( p)
env_t *p;
#endif
{
	_locp->lc_numtbl = (num_t *)p->le_numtbl;

	return(PASS) ;
}

static int
#ifndef _NO_PROTO
copy_time( env_t *p)
#else
copy_time( p)
env_t *p;
#endif
{
	/* LC_TIME */
	_locp->lc_timtbl = (tim_t *)p->le_timtbl;

	return(PASS);
}
static int
#ifndef _NO_PROTO
copy_messages( env_t *p)
#else
copy_messages( p)
env_t *p;
#endif

{
	_locp->lc_msgtbl = (msg_t *)p->le_msgtbl;

	return(PASS);
}
