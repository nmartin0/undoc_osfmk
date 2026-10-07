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
static char rcsid[] = "@(#)$RCSfile: catopen.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:51 $";
#endif
/*
 * COMPONENT_NAME: LIBCMSG
 *
 * FUNCTIONS: rmalloc, catopen, NLcatopen, _do_open, make_set, opencatfile,
 * 	cat_already_open, add_open_cat, cat_close, cat_hard_close,  
 *	substitute.
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * sccsid[] = "catopen.c        1.33  com/lib/c/msg,3.1,9021 4/23/90 15:05:58";
 */


#include "catio.h"
#include <string.h>

#ifdef	_THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex _catalog_rmutex;

#undef	RETURN
#define	RETURN(s) \
	return(rec_mutex_unlock(&_catalog_rmutex), seterrno(errno_save), s)

#define	fread	unlocked_fread
#define	fseek	unlocked_fseek
#define	fclose	unlocked_fclose
#endif

static CATD *catsopen[NL_MAXOPEN];	/*---- list of open catalog pointers -*/
static int  catpid[NL_MAXOPEN];         /*---- list of pid associated with 
					       the corresponding to each 
					       catalog pointer in catsopen --*/
static int make_sets();
static FILE *opencatfile();
static void add_open_cat();

static void substitute();
static void cat_hard_close();  		/*---- phyically closes the cat ----*/
static nl_catd cat_already_open(); 	/*---- used to see if the cat has
                	                           already been opened ----*/

/*                                                                   
 EXTERNAL PROCEDURES CALLED: 	standard library functions
 */



/*
 * NAME: rmalloc
 *
 * FUNCTION: Allocates memory with error checking.
 *
 * EXECUTION ENVIRONMENT:
 *
 *	Rmalloc executes under a process.
 *
 * RETURNS: Returns a pointer to the memory malloc'ed or
 *          -1 if malloc fails.
 */

static char *rmalloc(int size)

{
	char *p;

	if (!(p = malloc(size)))
		return(NULL);
	memset(p,NULL,size);
	return(p);
}


	
	

/*
 * 
 * NAME: catopen
 *                                                                    
 * FUNCTION: Opens a catalog and return a valid nl_catd
 *
 * EXECUTION ENVIRONMENT:
 *
 * 	Catopen executes under a process.	
 *
 * NOTES:  Catopen does not always open the catalog. When a user issues a 
 * 	close on a catalog, the file is not closed.  The close on exec flag 
 * 	is set, but the file is not closed, nor are the data structures 
 *	assocciated with the catalog freed. If a second open is issued against 
 * 	the same catalog name, the catalog does not actually have to be opened.
 *      This is implemented primarily to prevent overuse of NLgetamsg from 
 *	producing an unacceptable number of opens and closes.
 *
 * RETURNS: Returns a pointer to a CATD. 
 *          Returns a (nl_catd) -1 if fails.
 *
 */  

nl_catd catopen (char *cat, int dummy)
	/*---- char *cat:  the name of the cat to be opened ----*/
	/*---- int dummy:  dummy variable  ----*/

{
        int errno_save;
	nl_catd _do_open();  	    /*---- routine that actually opens 
					   the catalog ---- */
	CATD *catd;

#ifdef	_THREAD_SAFE
	rec_mutex_lock(&_catalog_rmutex);
        errno_save = geterrno();
#else
        errno_save = errno;
#endif

	if (catd = cat_already_open(cat)) {
		catd->_count = catd->_count + 1;
		RETURN(catd);
	}
	catd = (CATD *)rmalloc (sizeof(CATD));
	if ( catd == NULL )
		RETURN(CATD_ERR);
	catd->_name = (char *)rmalloc(strlen(cat) + 1);
	if ( catd->_name == NULL )
		RETURN(CATD_ERR);
	strcpy(catd->_name,cat);
	catd->_fd = FALSE;
	catd->_mem = FALSE;
	catd->_pid = getpid();
	catd->_count = 1;
	if (_do_open(catd) != CATD_ERR)  
		RETURN(catd);
	else {
		free(catd->_name);
		free(catd); 
		RETURN(CATD_ERR);
	}
}



/*
 * NAME: NLcatopen
 *                                                                    
 * FUNCTION: Sets up a deferred open for a catalog. If the catalog 
 *	is referenced, the partial open started here will be completed by
 *	_do_open.
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *
 * 	NLcatopen executes under a process.	
 *
 * RETURNS: Returns a pointer to CATD
 *
 */  
 
nl_catd NLcatopen(char *cat, int dummy)

	 	/*---- name of the catalog to be opened ----*/
		/*----  dummy variable  ----*/

{ 
        int errno_save;
	CATD *catd;

#ifdef	_THREAD_SAFE
	rec_mutex_lock(&_catalog_rmutex);
        errno_save = geterrno();
#else
        errno_save = errno;
#endif

	if (catd = cat_already_open(cat)) {
  		catd->_count = catd->_count + 1;
		RETURN(catd);
	}
	catd = (CATD *)rmalloc (sizeof(CATD));
	if ( catd == NULL )
		RETURN(CATD_ERR);
	catd->_name = (char *)rmalloc(strlen(cat) + 1);
	if ( catd->_name == NULL )
		RETURN(CATD_ERR);
	strcpy(catd->_name,cat);
	catd->_fd = FALSE;
	catd->_mem = FALSE;
	catd->_pid = getpid();
	catd->_count = 1;
	RETURN(catd);
}

 


/*
 * 
 * NAME: _do_open
 *                                                                    
 * FUNCTION: Opens a catalog file, reads in and builds an index table.
 *                                                                    
 * EXECUTION ENVIRONMENT:
 * 	_do_open executes under a process.	
 *
 * NOTES: _do_open does all the necessary operations for catopen() 
 *	and NLcatopen().
 *
 * RETURNS: Returns a pointer to a CATD structure (nl_catd)
 *	If the open fails, _do_open returns a NULL pointer.
 *
 */  

#ifdef	_THREAD_SAFE
/*
 * Caller should take _catalog_rmutex lock before calling this routine.
 */
#endif

nl_catd _do_open(nl_catd catd)

	/*---- pointer to the partially set up cat descriptor ----*/

{
	FILE *opencatfile();	/*---- routine to search the lang path 
						and open the .cat file ----*/
	void add_open_cat();	/*---- routine to keep a list of 
                                               opened cats ----*/
	long int magic;
	int i;			/*---- Misc counter(s) used for loop */
	struct _catset cs;
        int errno_save;

#ifdef	_THREAD_SAFE
/*
 * can't use the same macro, since we are not locking
 */
#undef	RETURN
#define	RETURN(s)	return(seterrno(errno_save), s)

        errno_save = geterrno();
#else
        errno_save = errno;
#endif

	catd->_fd = opencatfile( catd->_name );
	if ( !catd->_fd ) {
		return( CATD_ERR );
	}
	fread((void *)&magic,(size_t)4,(size_t)1,catd->_fd);
	if (magic != CAT_MAGIC){
		fclose(catd->_fd);
		catd->_fd = NULL;
		return( CATD_ERR );
	}
/*	if ((catd->_mem = shmat((int)fileno(catd->_fd), (char *)0, SHM_MAP | SHM_RDONLY))
           == (char * )ERR ) {   */

	if (1) {      /* disable the shmat, share memory segemnt */

/*______________________________________________________________________
	If the file can not be mapped then simulate mapping for the index
	table so that make_sets cat set things up. (rmalloc an area big
 	enough for the index table and read the whole thing in)
  ______________________________________________________________________*/

                /* reset the file pointer to the beginning of catalog */
		fseek(catd->_fd,(long)0,0);

                /* malloc the header, if fails return error */
		catd->_hd = (struct _header *) rmalloc(sizeof(struct _header));
		if ( catd->_hd == NULL )
			RETURN(CATD_ERR);

                /* read in the whole header */
		fread((void *)catd->_hd,(size_t)sizeof(struct _header),(size_t)1,catd->_fd);

                /* cs is a dummpy to hold a set temperorily. The purpose of */
                /* this for loop is to fread the whole catalog so that the  */
                /* file pointer will be moved to the end of the catalog.    */
		for (i = 0 ; i < catd->_hd->_n_sets ; i++) {
			fread((void *)&cs,(size_t)4,(size_t)1,catd->_fd);
			fseek(catd->_fd, (long)(cs._n_msgs * sizeof(struct _msgptr)),1);
		}

                /* after the for loop, ftell returns the byte offset of the */
                /* end of the catalog relative to the begining of the file. */
                /* i.e. i contains the byte offset of the whole catalog.    */
		i = ftell(catd->_fd);

                /* malloc _mem as a temp pointer to hold the entire catalog. */
		catd->_mem = (char *)rmalloc(i);
		if ( catd->_mem == NULL )
			RETURN(CATD_ERR);

                /* reset the file pointer to the begining. */
		fseek(catd->_fd,(long)0,0);

                /* read in the whole catalog into _mem */
		fread((void *)catd->_mem,(size_t)i,(size_t)1,catd->_fd);

                /* malloc one extra set more than the max. set number */
		catd->_set = (struct _catset *) rmalloc((catd->_hd->_setmax+1)* 
                              sizeof (struct _catset));
		if ( catd->_set == NULL )
			RETURN(CATD_ERR);

                /* save the max. set number in catd->_setmax */
		catd->_setmax = catd->_hd->_setmax;
                /* call make_set to malloc memory for every message */
                if(make_sets(catd->_set,catd->_mem,catd->_hd->_n_sets) == -1)
                        return (CATD_ERR);
		free(catd->_mem);
		catd->_mem = FALSE;
		add_open_cat(catd);
		return(catd);
	}
	else {

/*______________________________________________________________________
	Normal mapping has occurred, set a few things up and call make_sets
  ______________________________________________________________________*/

		catd->_hd =( struct _header * )( catd->_mem );
		catd->_setmax = catd->_hd->_setmax;
		catd->_set = (struct _catset *) rmalloc((catd->_hd->_setmax+1)*
                             sizeof (struct _catset));
		if ( catd->_set == NULL )
			RETURN(CATD_ERR);
                if(make_sets(catd->_set,catd->_mem,catd->_hd->_n_sets) == -1)
                        return (CATD_ERR);
		add_open_cat(catd);
		return(catd);
	}
}



/*
 * 
 * NAME: make_sets
 *
 * FUNCTION: Expands the compacted version of the catalog index table into
 *	the fast access memory version.
 *
 * EXECUTION ENVIRONMENT:
 *
 *	Make_set executes under a process.	
 *
 * RETURNS: int
 */


static int make_sets(struct _catset *cset, char *base, int n_sets)

	/*---- cset: place to store the sets ----*/
	/*---- base: base of the catalog memory (mapped or not) ----*/
	/*---- n_sets: number of sets in _header table ----*/

{
	int 	i;	/*---- Misc counter(s) used for loops ----*/
	int	j;	/*---- Misc counter(s) used for loops ----*/
	int 	msgmax;	/*---- The maximum number of _messages in a set ----*/
	char 	*cmpct_set_ptr;	/*---- pointer into the index table ----*/
	struct _catset	cs;	/*---- used to look at the sets in the table -*/

	cmpct_set_ptr = base + sizeof(struct _header);

	for (i = 0 ; i < n_sets ; i++) {
	    /* loop through each compacted set */

		cs = *(struct _catset *)cmpct_set_ptr;	
                /* set the _catset ptr to the base of the current 
                   compacted set.        */

		cs._mp = (struct _msgptr *)(cmpct_set_ptr +
                          2 * sizeof(unsigned short));
                          /* set the ms array ptr to the base of
			     compacted array of _msgptr's     */

		for (j = 0 ,msgmax = 0 ; j < cs._n_msgs ; j++) {
	            /* find the highest msgno in the set */

			if (cs._mp[j]._msgno > msgmax)
				msgmax = cs._mp[j]._msgno;
		}
		msgmax++;   /* allocate memory for the expanded 
			       array (this one will have holes) */

                if((cset[cs._setno]._mp = (struct _msgptr *) rmalloc(msgmax *
                                       sizeof(struct _msgptr))) == NULL)
                        return (-1);
                if((cset[cs._setno]._msgtxt = (char **)rmalloc(msgmax *
                                           sizeof (char **))) == NULL)
                        return (-1);

		for (j = 0 ; j < msgmax ; j++)  
                    /* mark all the _msgptr's as being empty */

			cset[cs._setno]._mp[j]._offset = FALSE;
		for (j = 0 ; j < cs._n_msgs ; j++) {    
                     /* Fill the appropriate ones with data */

			cset[cs._setno]._mp[cs._mp[j]._msgno] = cs._mp[j];
		}
		cset[cs._setno]._n_msgs = msgmax;	
		cset[cs._setno]._setno = cs._setno;
       	        /* Superfluous but should have the correct data. Increment 
                   the base of the set pointer.          */

		cmpct_set_ptr += 2 * sizeof(unsigned short) + cs._n_msgs *
                                 sizeof(struct _msgptr);
	}
	return(0);
}



/*
 * 
 * NAME: opencatfile
 *
 * FUNCTION: Opens a catalog file, looking in the language path first (if 
 *	there is no slash) and returns a pointer to the file stream.
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *
 * 	Opencatfile executes under a process.	
 *
 * RETURNS:  Returns a pointer to the file stream, and a NULL pointer on
 *	failure.
 */

static FILE *opencatfile(char *file)
{
	extern char *getenv();
	char 	fl[PATH_MAX];	/*---- place to hold full path ----*/
	char 	*nlspath;	/*---- pointer to the nlspath val ----*/
	FILE	*fp;		/*---- file pointer ----*/
	char	cpth[PATH_MAX]; /*---- current value of nlspath ----*/
	char    *p,*np;
	char	*fulllang;	/* %L language value */
	char	lang[PATH_MAX]; /* %l language value */
	char	*territory;	/* %t language value */
	char	*codeset;	/* %c language value */
	char	*ptr;		/* for decompose of $LANG */
	char *str;
	char *optr;
	int nchars;
	int lenstr;
	char outptr[PATH_MAX];
	int valid;

	if (strchr(file,'/')) {
                if ((fp = fopen(file,"r")))
                   {  fcntl(fileno(fp),F_SETFD,1);
                                /* set the close-on-exec flag for
                                    child process                */
                      return(fp);
                   }
	}
	else {
   		if (!(fulllang = getenv("LANG"))) 
			fulllang = DEFAULT_LANG;
		if (!(nlspath = getenv("NLSPATH"))) {
			/*
			 * The NLSPATH variable is not set.  If LANG is the
			 * default (C), do not search for any message catalogs.
			 * Otherwise, set nlspath to system default.
			 */
			if (!strcmp(fulllang, DEFAULT_LANG))
				return(NULL);
			else
				nlspath = PATH_FORMAT; 
		}
		else {
                       /*
                         * If the requestor is priviledged then don't search
                         * NLSPATH, only search the default path.
                         */
                        if (!getuid()) {

                            if (!strcmp(fulllang, DEFAULT_LANG))
                                        return(NULL);
                            else
                                        nlspath = PATH_FORMAT;
                        }
		}

		/*
		** LANG is a composite of three fields:
		** language_territory.codeset
		** and we're going to break it into those
		** three fields.
		*/

		strcpy(lang, fulllang);

		territory = "";
		codeset = "";

		ptr = strchr( lang, '_' );
		if (ptr != NULL) {
			territory = ptr+1;
			*ptr = '\0';
			ptr = strchr(territory, '.');
			if (ptr != NULL) {
				codeset = ptr+1;
				*ptr = '\0';
			}
		} else {
			ptr = strchr( lang, '.' );
			if (ptr != NULL) {
				codeset = ptr+1;
				*ptr = '\0';
			}
		}

		np = nlspath;
		while (*np) {
			p = cpth;
			while (*np && *np != ':')
				*p++ = *np++;
			*p = NULL;
			if (*np)	/*----  iff on a colon then advance --*/
				np++;
			valid = 0;
			if (strlen(cpth)) {
				ptr = cpth;
				optr = outptr;

				nchars = 0;
				while (*ptr != '\0') {
					while ((*ptr != '\0') && (*ptr != '%') 
						      && (nchars < PATH_MAX)) {
						*(optr++) = *(ptr++);
						nchars++;
					}
					if (*ptr == '%') {
						switch (*(++ptr)) {
							case '%':
								str = "%";
								break;
							case 'L':
								valid = 1;
								str = fulllang;
								break;
							case 'N':
								valid = 1;
			    					str = file;
								break;
							case 'l':
								str = lang;
								break;
							case 't':
								str = territory;
								break;
							case 'c':
								str = codeset;
								break;
							default:
								str = "";
								break;
						}
						lenstr = strlen(str);
						nchars += lenstr;
						if (nchars < PATH_MAX) {
							strcpy(optr, str);
							optr += lenstr;
						} else {	
							break;
						} 
						ptr++;
					} else {
						if (nchars >= PATH_MAX) {
							break;
						}
					}
				}
				*optr = '\0';
				strcpy(cpth, outptr);
			}
			else {		/*----  iff leading | trailing | 
                                                adjacent colons ... --*/
				valid = 1;
				strcpy(cpth,file);
			}
			if (valid == 1 && (fp = fopen(cpth,"r")))
                            {  fcntl(fileno(fp),F_SETFD,1);
                                        /* set the close-on-exec flag for
                                           child process                */
                               return(fp);
                            }
		}
                if (fp = fopen(file,"r"))
                   {  fcntl(fileno(fp),F_SETFD,1);
                                 /* set the close-on-exec flag for
                                    child process                */
                       return(fp);
                    }
	}
	return (NULL);
}

/*
 * NAME: _catisopen
 *
 * FUNCTION: Checks to see if specified catalog (keyed by catd) is still open
 *
 * RETURNS:	TRUE => catalog is open
 *		FALSE=> catalog has been closed or was never open
 */

int
_catisopen( nl_catd catd )
{
  int i;

  if(!catd) return FALSE;		/* NULL pointer can't be an open catalog */

  for (i = 0; i < NL_MAXOPEN; i++)
    if (catd == catsopen[i])		/* It looks pretty good */
      return TRUE;

  return FALSE;
}
 

/*
 * 
 * NAME: cat_already_open
 *
 * FUNCTION: Checkes to see if a specific cat has already been opened.
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *
 * 	Cat_already_open executes under a process.
 *
 * RETURNS: Returns a pointer to the existing CATD if one exists, and 
 *	a NULL pointer if no CATD exists.
 */

static nl_catd cat_already_open(char *cat)
			/*---- name of the catalog to be opened ----*/

{
	int i;			/*---- Misc counter(s) used for loops ----*/
	
	for (i = 0 ; i < NL_MAXOPEN && catsopen[i] ; i++)  {
		if (!strcmp(cat,catsopen[i]->_name) && getpid()==catsopen[i]->_pid) {
		       	return(catsopen[i]);
		}
	}
	return(0);
}



/*
 * 
 * NAME: add_open_cat
 *
 * FUNCTION: Adds a cat to the list of already opened catalogs.
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *
 * 	Add_open_cat executes under a process.	
 *
 * RETURNS: void
 */

static void add_open_cat(nl_catd catd)
	 	/*---- catd to be added to the list of catalogs ----*/

{
	int i = 0;	/*---- Misc counter(s) used for loops ----*/
	while (i < NL_MAXOPEN && catsopen[i]) {
		if (!strcmp(catd->_name,catsopen[i]->_name) && 
		    getpid()==catsopen[i]->_pid)
			return;	/*---- The catalog is already here ----*/
		i++;
	}

	if (i < NL_MAXOPEN) {
		catsopen[i] = catd;
		catpid[i] = getpid();
	}
}



/*
 * 
 * NAME: catclose
 *                                                                    
 * FUNCTION: Closes catalog. 
 *
 * EXECUTION ENVIRONMENT:
 *
 * 	catclose executes under a process.	
 *
 * NOTES: Catclose closes the stream and frees the memory of the catalog.
 *
 * RETURNS: 0 on success, -1 on failure.
 *
 */  


int catclose(nl_catd catd)		/*---- the catd to be closed ----*/

{
	int i;

#undef	RETURN
#ifdef	_THREAD_SAFE
#define	RETURN(val)	return(rec_mutex_unlock(&_catalog_rmutex), val)

	rec_mutex_lock(&_catalog_rmutex);
#else
#define	RETURN(val)	return(val)
#endif

	if (catd == CATD_ERR)
		RETURN(-1);
	for (i=0; i< NL_MAXOPEN && catsopen[i]; i++) {
		if (catd == catsopen[i] && getpid()==catsopen[i]->_pid)
			break;
	}
	if (i == NL_MAXOPEN || catsopen[i] == NULL)
		RETURN (-1);
	if (catd->_fd == (FILE *)NULL)	
				/*----  return if this is an extra open or
					a bad catalog discriptor         ----*/
		RETURN(-1);
	if (cat_already_open(catd->_name)) {
		if (catd->_count == 1) {
			catd->_count = 0;
			cat_hard_close(catd);
	        	RETURN (0); 	/*--- the last legal clsoe ---*/
		}
	 	else if (catd->_count > 1) {
			catd->_count = catd->_count - 1;
			RETURN(0);	/*--- a legal close ---*/
		}
		else	
			RETURN (-1);	/*--- an extra illegal close ---*/
	}
	else {
		RETURN(-1);
	}
}
 



/*
 * NAME: cat_hard_close
 *
 * FUNCTION: Closes a catalog and frees the memory with it. Deletes the catd
 *	from the list of open catalogs.
 *
 * EXECUTION ENVIRONMENT:
 * 	Cat_hard_close executes under a process.	
 *
 * RETURNS: void
 */


static void cat_hard_close(nl_catd catd)
		/*---- the catd to be closed ----*/

{
	int i;			/*---- Misc counter(s) used for loops ----*/
	int j;			/*----  Misc counter ----*/

	if (catd == CATD_ERR)
		return;
	
/*______________________________________________________________________
	remove any entry for the catalog in the catsopen array
  ______________________________________________________________________*/

	for (i = 0 ; i < NL_MAXOPEN && catsopen[i] ; i++) {
		if (catd == catsopen[i]) {
			for (; i < NL_MAXOPEN-1; i++) {
			    catsopen[i] = catsopen[i+1];
			    catpid[i] = catpid[i+1];
			}
			catsopen[i] = NULL;
			catpid[i] = 0;
		}
	}

/*______________________________________________________________________
	close the cat and free up the memory
  ______________________________________________________________________*/
	if (catd->_mem == FALSE)
	{
	for (i = 0 ; i <= catd->_hd->_setmax ; i++) {
		if (catd->_set[i]._mp) 
			free(catd->_set[i]._mp);   
                        /*---- free the _message pointer arrays ----*/

		if (catd->_set[i]._msgtxt) {
			for (j = 0 ; j < catd->_set[i]._n_msgs ; j++) {
				if (catd->_set[i]._msgtxt[j])
					free(catd->_set[i]._msgtxt[j]);
			}
			free(catd->_set[i]._msgtxt);
		}
	}
	}

	if (catd->_fd)
		fclose(catd->_fd); 	/*---- close the ctatlog ----*/
	if (catd->_set) 
		free(catd->_set);	/*---- free the sets ----*/
	if (catd->_name)
		free(catd->_name);      /*---- free the name ----*/
	if (catd->_hd)
		free(catd->_hd);	/*---- free the header ----*/
	if (catd)
		free(catd);		/*---- free the catd ----*/
}
