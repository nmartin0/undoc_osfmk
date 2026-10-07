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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid[] = "@(#)xgencat.c	1.7  com/cmd/msg,3.1,8943 9/27/89 15:57:36";

/*
 * COMPONENT_NAME: CMDMSG
 *
 * FUNCTIONS: main, bump_msg, set_quote, set_message, store_msg, set_set,
 *            set_msg, delset, get_text, write_msg, msg_comp, load_cat,
 *            open_source
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
 */

/*                                                                   
 * EXTERNAL PROCEDURES CALLED: standard library functions
 */

#include "catio.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

/*              include file for message texts          */
#ifdef MSG
#include "msgfac_msg.h" 
#endif

/*
 * NAME: main 
 * 
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * FUNCTION: Parses the arguments. Reads the input stream and
 *           drives the rest of the program.
 *                                                                    
 * RETURNS: 0 on success, 1 on failure.
 *
 */ 
 
#ifndef MSG
#define NLgetamsg(a,b,c,s)	s
#endif


#define MAXMSG 32
char		quote;	/*---- current quote character ----*/
unsigned short	set;	/*---- current set number  ----*/
unsigned short	msglen = NL_TEXTMAX;	/*---- current msglen ----*/

int current = -1;		/*---- current _message index into emsg[] ----*/
int msgmax  = 0;		/*---- current dimension of emsg[] ----*/
struct _message	*emsg;	/*---- array of _message structs (holds all _messages ----*/
#define isahex(c) ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
#define isaoct(c) (c >= '0' && c <= '7')


/*______________________________________________________________________
	These internal reoutines all have void data types.  (i.e. if
	they fail there is no recovery (they die()).
  ______________________________________________________________________*/
void set_quote();
void get_message();
void store_msg();
void set_set();
void set_msg();
void delset();
void get_text();
void write_msg();
void load_cat();
char *rmalloc();
FILE *open_source();

#ifdef _ANSI
main(int argc, char *argv[], char *envp[])
#else
main(argc,argv,envp)
int 	argc;	/*---- These are the standard arguments to main ----*/
char	*argv[],
	*envp[];
#endif
{
	extern char	quote;

	char 	*target,		/* Target file name */
		line[NL_TEXTMAX];		/* current line of text*/

	FILE	*sf,		/*---- source stream ----*/
		*tf;		/*---- target stream ----*/

	int file_no = 2;
	
/*______________________________________________________________________
	Check the input arguemnts, open the input and output files
  ______________________________________________________________________ */

	if (argc < 2) 		/*---- die if no target cat specified ----*/
		die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_0, "Usage: gencat <target_cat>") );
	if (argc == 2)
		sf = stdin;		/*---- read from stdin ----*/
	else {
		sf = (FILE *)open_source(argv[file_no]);
	}

	target = argv[1];
	load_cat(target);	/*---- Load any existing catalog into memory ----*/

	do {
		while (!feof(sf)) {	/*---- read through the input and branch on any keywords----*/
			fgets(line,NL_TEXTMAX,sf);
			if (feof(sf))
				continue;
			if ( !memcmp(line,"$ ",2) || !memcmp(line,"$\t",2)) {
				continue;	/*----  check for comment  ---*/
			}
			else if (!memcmp(line,"$quote ",strlen("$quote "))) {
				set_quote(line);
			}
			else if (!memcmp(line,"$delset",strlen("$delset")))
				delset(line);
			else if (!memcmp(line,"$set",strlen("$set"))) {
				set_set(line);
			}
			else if (!memcmp(line,"$msg",strlen("$msg"))) {
				set_msg(line);
			}

			else {
				char *p = line;
				skip_to_nwhite(p);
				if (isdigit(*p)) {
					get_message(p,sf);		
				}
			}
/*
			else if (isdigit(line[0])) {
				get_message(line,sf);
			}
*/
		}
	}
	while (++file_no < argc && (sf = (FILE *)open_source(argv[file_no]))); 

	tf = fopen(target,"w");
	if (!tf)
		die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_1, "gencat:  Unable to open target file.") );

	write_msg(emsg,tf);	/*---- Write the output ----*/

	fclose(sf);
	fclose(tf);

	exit(0);
	return( 1 );
}

/*
 * NAME: bump_msg
 *
 * FUNCTION: Increments the current _message pointer.
 *           Checks for room in emsg. If there is not enough room, it 
 *           calls realloc().
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS:  void
 */ 


void bump_msg()	/*----  incements the current _message pointer, 
			checks for room in emsg[], if there 
			is not enough it will realloc() ----*/
{
	extern struct _message 	*emsg;
	extern int 		current;
	extern int 		msgmax;

	if (current >= msgmax - 2) {
		register int i;

		msgmax += MAXMSG;
		if (msgmax > 0) {	/*---- if emsg exists ----*/
			if (!(emsg = (struct _message *)realloc(emsg,msgmax * sizeof (struct _message))))
				die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_2, "gencat:  Unable to realloc().") );
		}
		else {			/*---- if this is the first time ----*/
			if (!(emsg = (struct _message *)rmalloc(msgmax * sizeof (struct _message))))
				die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_3, "gencat:  Unable to realloc().") );
		}
		for (i = current + 1 ; i < msgmax ; i++) {	/*---- set up the new _messages ----*/
			emsg[i]._text = FALSE;
			emsg[i]._set = emsg[i]._msg = emsg[i]._old = FALSE;
		}
	}
	current++;	/*---- bump current ----*/
}

/*
 * NAME: set_quote
 *
 * FUNCTION: Resets the current quote character.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */


#ifdef _ANSI
void set_quote(char *line)
#else
void set_quote(line)
char *line;	/*---- input line (must include a ^$quote ----*/
#endif
{
	extern char quote;	/*---- the current quote char ----*/
	
	skip_to_white(line);
	skip_to_nwhite(line);

	quote = *line;
}

/*
 * NAME: get_message()
 *
 * FUNCTION: Gets the _message starting on the current line
 *           and store the resulting _message structure in emsg[].
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */

#ifdef _ANSI
void get_message(char *line, FILE *file)
#else
void get_message(line,file)
char	*line;	/*---- Line the where the _message begins ----*/
FILE	*file;	/*---- File it came from (in case of a continuation) ----*/
#endif
{

	char 			ttxt[NL_TEXTMAX];	/*---- place to store the text ----*/
	struct _message 		msg;	/*---- _message we are getting ----*/
	static struct _message	omsg;	/*---- old _message (order check) ----*/
	static char		started = 'N';	/*---- is there an old _message?? ----*/


	sscanf(line,"%hu",&msg._msg);
	get_text(line,ttxt,file);
	if (strlen(ttxt) > msglen)  {
		die("message text longer than $len value.");
	}
 	msg._text = (char *)rmalloc(strlen(ttxt) + 1);
	strcpy(msg._text,ttxt);
	msg._set = set;

	if (started == 'Y' && msg_comp(&msg,&omsg) <= 0) {
		printf( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_0, "gencat:  The message numbers/sets became out of order just after:\n msg:  %d,  set %d\n %s") ,
			omsg._msg,omsg._set,omsg._text);
		die ("");
	}
	omsg = msg;
	started = 'Y';

	store_msg(&msg);	/*---- Store the _message (used to replace old ones) ----*/
}


/*
 * NAME: store_msg
 *
 * FUNCTION: Insterts a _message into emsg[]. Overwrites an existing
 *           _message if the catalog is being updated and there is a 
 *           duplicated _message in the old version of the catalog.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */


#ifdef _ANSI
void store_msg(struct _message *msg)
#else
void store_msg(msg)
struct _message *msg;	/*---- _message to be inserted in emsg ----*/
#endif
{
	extern struct _message 	*emsg;
	extern int		current,
				msgmax;
	int 	i;


/*______________________________________________________________________
	Search to see if there is a duplicate in the old _messages
  ______________________________________________________________________*/
	for (i = 0 ; i <= current ; i++) {
		if (!msg_comp(msg,&emsg[i]))
			break;
	}
	if (i <= current) {
		emsg[i] = *msg;		/*---- If there is an old one, replace it ----*/
	}
	else {
		bump_msg();		/*---- else add a new one ----*/
		emsg[current] = *msg;
	}
}
			

/*
 * NAME: set_set
 *
 * FUNCTION: Sets the current set number and stores the value in the
 *           global variable 'set'.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */


#ifdef _ANSI
void set_set(char *line)
#else
void set_set(line)	/*---- set the current set number ----*/
char *line;	/*---- line with $set n command  ----*/
#endif
{

	skip_to_white(line);
	skip_to_nwhite(line);
	
	sscanf(line,"%hu",&set);

	if (set < SETMIN || set > SETMAX)
		die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_5, "gencat:  Invalid set number.") );
}

/*
 * NAME: set_msg
 *
 * FUNCTION: Sets the current msg number and stores the value in the
 *           global variable 'msglen'.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */

#ifdef _ANSI
void set_msg(char *line)
#else
void set_msg(line)	/*---- set the current msg number ----*/
char *line;	/*---- line with $msg n command  ----*/
#endif
{

	skip_to_white(line);
	skip_to_nwhite(line);
	
	sscanf(line,"%hu",&msglen);

	if (set < 1 || set > NL_TEXTMAX)
		die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_5, "gencat:  Invalid message length.") );
}


/*
 * NAME: delset
 *
 * FUNCTION: Deletes an existing set of _messages from emsg[].
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */

#ifdef _ANSI
void delset(char *line)
#else
void delset(line)	/*---- remove any occurences of an existing set ----*/
char *line;	/*---- line with $delset n command	        ----*/
#endif
{
	extern struct _message 	*emsg;
	extern int		current,
				msgmax;

	unsigned short 		dset;	/*---- set to be deleted ----*/
	int			i;	/*---- Misc counter(s) used for loops ----*/

	skip_to_white(line);
	skip_to_nwhite(line);
	
	sscanf(line,"%hu",&dset);	/*---- get set to be removed ----*/

	if (dset < SETMIN || dset > SETMAX)
		die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_6, "gencat:  Invalid set number.") );


/*______________________________________________________________________
	Shuffle the _messages to delete any existing sets
  ______________________________________________________________________*/
	for (i = 0 ; emsg[i]._text && i <= current ; i++) {	/* Speed should not be a factor */
		if (emsg[i]._set == dset) {
			int j;
			free(emsg[i]._text);
			for (j = i ; j < current ; j++) {
				emsg[j] = emsg[j + 1];
			}
			emsg[j]._text = FALSE;
			current--;
			i--;
		}	
	}
}
	

		
/*
 * NAME: get_text
 *
 * FUNCTION: Assembles a string of _message text which has been stored in the 
 *           gencat (._msg file) format.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */

#ifdef _ANSI
void get_text(char *source, char *target, FILE *file)
#else
void get_text(source,target,file)
char	*source,	/*---- source string ----*/
	*target;	/*---- target string ----*/
FILE 	*file;		/*---- file (used for multi-line _messages) ----*/
#endif
{
	char 	quoted = FALSE,
		*base,
		*targetbase;

	base = source;
	targetbase = target;

	skip_to_white(source);
	skip_to_nwhite(source);

	if (*source == quote)  {
		quoted = TRUE;
		source++;
	}

	while (*source && target - targetbase < NL_TEXTMAX) {
		if (*source == '\\') {			/*---- Process backslash codes ----*/
			source++;
			if (isaoct(*source)) {	/*---- Octal number ----*/
				int octal;
				sscanf(source,"%3o",&octal);
				*target++ = octal;
				for (octal = 0 ; octal < 3 && isaoct(*(source + octal)) ; octal++) 
						;
				source += octal;
			}
			else {
				switch (*source) {
					case 'n': {
						*target++ = '\n';
						source++;
						break;
					}
					case 't': {	/*---- tab ----*/
						*target++ = '\t';
						source++;
						break;
					}
					case 'r': {	/*---- return ----*/
						*target++ = '\r';
						source++;
						break;
					}
					case 'b': {	/*---- backspace ----*/
						*target++ = '\b';
						source++;
						break;
					}
					case 'f': {	/*---- form feed ----*/
						*target++ = '\f';
						source++;
						break;
					}
					case 'v': {	/*---- vertical tab ----*/
						*target++ = '\v';
						source++;
						break;
					}
					case 'x': {	/*---- hex number (two or for digits) ----*/
						int 	hex,
							hexlen = 0;
						source++;
						while (isahex(*(source + hexlen++)))
							;
						if (hexlen == 2) {
							sscanf(source,"%2x",&hex);
							*target++ = hexlen;
							source += hexlen;
						}
						else if (hexlen == 4) {	
							sscanf(source,"%4hx",target);
							target += 2;
							source += 4;
						}
						else {
							die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_7, "Bad hex len (the length of a hex number must be either two or four digits.)") );
						}
						break;
					}
					case '\n': {			/*---- continuation ----*/
						source = base;
						fgets(source,NL_TEXTMAX,file);
						break;
					}
					default: {
						*target++ = *source++;
					}
				}
			}
		}
		else if (quoted && *source == quote) 
			break;
		else if (*source == '\n')
			if (quoted)
				die ( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_8, "gencat:  Unexpected newline within quotes.") );
			else
				break;
		else {
			*target++ = *source++;
		}
		if (!(target - targetbase < NL_TEXTMAX))
			die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_9, "gencat:  Message string longer than NL_TEXTMAX") );
	}
	if (!*source) 
		die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_10, "gencat:  Unexpected end of string, (no newline or end of quotes)") );
	*target = NULL;
}


/*
 * NAME: write msg
 *
 * FUNCTION: Converts emsg[] into a format suitable for fast access and write
 *           the result to the target file (file).
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */ 


#ifdef _ANSI
void write_msg(struct _message msg[], FILE *file)
#else
void write_msg(msg,file)
struct _message 	msg[];	/*---- _message array to be written (this is actually equal to emsg[]) ----*/
FILE 		*file;	/*---- File to write msg[] to ----*/
#endif
{

	short		i,j;	/*---- Misc counter(s) used for loops ----*/
	int 		total_sets;	/*---- total sets used ----*/
	int 		total_msgs;	/*---- total _messages in msg[] ----*/
	int 		header_size = sizeof(struct _header);	/*---- header size ----*/
	int 		msg_offset;	/*---- place to write the text of the next
						_message ----*/
	struct _msgptr 	mp;		/*---- structure used to accellerate the 
						_message retrieval ----*/
	struct _header	hd;	/*---- _header record of the .cat file ----*/
	extern int msg_comp();	/*---- compare for qsort ----*/
	extern int qsort();		/*---- used to sort msg[] ----*/

/*______________________________________________________________________
	Use qsort to sort msg[] by _message within set
  ______________________________________________________________________*/

	for (i = 0 ; msg[i]._text ; i++) 
		;

	qsort ((char *)msg,i,sizeof(struct _message),msg_comp); 

/*______________________________________________________________________
	Set up:
		total_sets,
		total_msgs,
		setmax
  ______________________________________________________________________*/
	for (i = 0 , total_sets = 0 , hd._setmax = 0 ; msg[i]._text ; i++) {
		if (!i || msg[i]._set != msg[i - 1]._set)
			total_sets++;
		if (msg[i]._set > hd._setmax)
			hd._setmax = msg[i]._set;
	}

	total_msgs = i;

	msg_offset = 	total_msgs * sizeof(struct _msgptr) + 	/*---- base of the _message text ----*/
			sizeof(struct _header) + 
			total_sets * 2 * sizeof(unsigned short);
	hd._magic = CAT_MAGIC;
	hd._n_sets = total_sets;
	fwrite(&hd,header_size,1,file);	/*---- write the header to the file ----*/

	for (i = 0 ; i < total_msgs ; i++) {		/*---- write the index table to the file ----*/
		if (!i || msg[i]._set != msg[i - 1]._set) {	/*---- when the set changes ----*/
			fwrite(&msg[i]._set,2,1,file);	/*---- set number ----*/
			for (j = 0 ; j + i < total_msgs ; j++) {
				if (msg[i + j]._set != msg[i]._set) 
					break;
			}
			fwrite(&j,2,1,file);	/*---- number of _messages ----*/
		}
		mp._msgno = msg[i]._msg;		/*---- write an 'mp' for each _message ----*/
		mp._msglen = strlen(msg[i]._text);
		mp._offset = msg_offset;
		fwrite(&mp,sizeof(mp),1,file);
		msg_offset += mp._msglen + 1;
	}
	if (ftell(file) != total_msgs * sizeof(struct _msgptr) + 
				sizeof(struct _header) + 
				total_sets * 2 * sizeof(unsigned short))
		die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_11, "gencat:  internal error.") );		/*---- file pointer consistancey check ----*/

	for (i = 0 ; i < total_msgs ; i++) {
		fwrite(msg[i]._text,strlen(msg[i]._text) + 1,1,file);
	}
	if (ftell(file) != msg_offset)
		die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_12, "gencat: internal error (bad file position)") );	/*---- file pointer 
										consistancey check  ----*/
}

/*
 * NAME: msg_comp
 *
 * FUNCTION: Compares _message structures and return a value which is
 *           approprite for qsort.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS:  a > b : 1
 *           a < b : -1
 *	     a = b : 0
 */


#ifdef _ANSI
int msg_comp(struct _message *a, struct _message *b)
#else
int msg_comp(a,b) 	/*---- used by qsort and get_msg ----*/
struct _message *a,*b;	/*---- the two _messages to be compared ----*/
#endif
{
	if (a->_set != b->_set)
		return(a->_set - b->_set);
	else 
		return(a->_msg - b->_msg);
}


/*
 * NAME: load_cat
 *
 * FUNCTION: Uses catopen to open a .cat file. Reformats the catd 
 *           structure into the emsg array, and close the .cat file.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */


#ifdef _ANSI
void load_cat(char *tcat)
#else
void load_cat(tcat)
char *tcat;	/*---- catalog name to be loaded ----*/
#endif
{
	nl_catd catd;	/*---- catalog descriptor ----*/
	struct _msgptr mpt;	/*---- catd style _message pointer ----*/
	int i,j;		/*---- Misc counter(s) used for loops ----*/
	struct stat sbuf;
	char cat[PATH_MAX];

	if (strchr(tcat,'/')) {
		strcpy(cat,tcat);
	}
	else {
		sprintf(cat,"./%s",tcat);
	}
	

	if (access(cat,R_OK))
		return;
	if (!stat(cat, &sbuf) && !sbuf.st_size ) {
		unlink(cat);
		return;
	}
	if ((catd = catopen( cat )) == CATD_ERR) {
		die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_13, "Unable to load specified catalog.") );	/*---- target cat exists, but is not
									a real cat ----*/
	}
/*______________________________________________________________________
	Reorder the catd structures into the emsg style[] structure
	while expanding emsg as needed.
  ______________________________________________________________________*/
	for (i = 0 ; i <= catd->_hd->_setmax ; i++ ) {
		for (j = 0 ; j < catd->_set[i]._n_msgs ; j++) {
			if (catd->_set[i]._mp[j]._offset) {
				mpt = catd->_set[i]._mp[j];
				fseek(catd->_fd,mpt._offset,0);
				bump_msg();
				emsg[current]._text = (char *) rmalloc(mpt._msglen + 1);
				if (!fread(emsg[current]._text,mpt._msglen + 1,1,catd->_fd))
					die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_14, "Unable to read old catalog file.") );
				emsg[current]._set = i;
				emsg[current]._msg = j;
				emsg[current]._old = TRUE;
			}
		}
	}
	catclose(catd);
	fclose(catd->_fd);
}

/*
 * NAME: open_source
 *
 * FUNCTION: Opens a source stream.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: A pointer to the source stream.
 */

#ifdef _ANSI
FILE *open_source(char *file)
#else
FILE *open_source(file)
char *file;
#endif
{
	FILE *f;

	if (!(f = fopen(file,"r"))) {
		printf( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_1, "Gencat: Unable to open %s\n") ,file);
		exit(1);
	}
	return(f);
}


/*
 * NAME: rmalloc
 *
 * FUNCTION: Performs a malloc with some error checking.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: returns a pointer to the result of the malloc.
 */


#ifdef _ANSI
char *rmalloc(int n)
#else
char *rmalloc(n)
int n;		/*----  the number of bytes to be malloc'ed  ----*/
#endif
{
	char *t;

	t = (char *) malloc(n);
	if (!t)
		die( NLgetamsg(MF_MSGFAC, MS_GENCAT, M_MSG_15, "Unable to malloc memory.") );
	return(t);
}
/*
 * 
 * NAME: catopen
 *                                                                    
 * FUNCTION: Opens a catalog and return a valid nl_catd.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * NOTES: Catopen does not always open the catalog, when a user issues a 
 *        close on a catalog, the file is not closed.  The close on exec 
 *        flag is set, but the file is not closed, nor are the data 
 *        structures assocciated with the catalog freed. If a second open 
 *        is issued against the same catalog name, the catalog does not 
 *        actually have to be opened.  This is implemented primarily to 
 *        prevent overuse of NLgetamsg from porducing an aunacceptable 
 *        number of opens and closes.
 *                                                                    
 * RETURNS: A pointer to a CATD. 
 *          A NULL pointer on failure.
 *
 */

  
#ifdef _ANSI
nl_catd catopen(char *cat)
#else	/*  _ANSI ifdef ...  */
nl_catd catopen( cat )
char *cat;	/*---- the name of the cat to be opened ----*/
#endif	/*  _ANSI ifdef ...  */
{
	nl_catd _do_open();	/*---- routine that actually opens the catalog ----*/
	nl_catd cat_already_open();	/*---- used to see if the cat has already been opened ----*/

	CATD *catd;

	if ((catd = cat_already_open(cat)))
		return(catd);


	catd = (CATD *)rmalloc (sizeof(CATD));

	catd->_name = (char *)rmalloc(strlen(cat) + 1);
	strcpy(catd->_name,cat);
	catd->_fd = FALSE;
	catd->_mem = FALSE;
	catd->_stat = 0;
	if (_do_open(catd) != CATD_ERR)
		return(catd);
	else {
		free(catd);
		return(CATD_ERR);
	}
}

/*
 * NAME: NLcatopen
 *                                                                    
 * FUNCTION: Sets up a deferred open for a catalog (if the catalog 
 *           is referenced the partial open started here will be 
 *           completed by _do_open.)
 *                                                                    
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: Returns a pointer to CATD
 *
 */  

/*______________________________________________________________________*/
#ifdef _ANSI
nl_catd NLcatopen( char *cat)
#else	/*  _ANSI ifdef ...  */
nl_catd NLcatopen( cat )
char *cat;	/*---- name of the catalog to be opened ----*/
#endif	/*  _ANSI ifdef ...  */
{
	nl_catd cat_already_open();	/*---- check to see if the cat has already been opened ----*/


	CATD *catd;

	if ((catd = cat_already_open(cat)))
		return(catd);
	
	catd = (CATD *)rmalloc (sizeof(CATD));
	catd->_name = (char *)rmalloc(strlen(cat) + 1);
	strcpy(catd->_name,cat);
	catd->_fd = FALSE;
	catd->_mem = FALSE;
	catd->_stat = 0;
	return(catd);
}


/*
 * 
 * NAME: _do_open
 *                                                                    
 * FUNCTION: open a catalog file, read in and build an index table
 *                                                                    
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: A pointer to a CATD structure (nl_catd).
 *          A NULL pointer if failed.
 *
 */

  
/*______________________________________________________________________*/
#ifdef _ANSI
nl_catd _do_open(nl_catd catd)
#else	/*  _ANSI ifdef ...  */
nl_catd _do_open(catd)
nl_catd catd;	/*---- pointer to the partially set up cat descriptor ----*/
#endif	/*  _ANSI ifdef ...  */
{
#ifndef NO_SHMAT
	char *shmat(  );		/*---- maps the file ----*/
#endif
	static void make_sets();	/*---- routine to unpack the sets into 
						fast acccess mode ----*/
	static FILE *opencatfile();	/*---- routine to search the lang path 
						and open the .cat file ----*/
	static void add_open_cat();	/*---- routine to keep a list of opened cats ----*/
	long int magic;

	add_open_cat(catd);

	catd->_fd = opencatfile( catd->_name );
	if ( !catd->_fd ) {
			catd->_stat = -1;
			return( CATD_ERR );
	}
	
	fread(&magic,4,1,catd->_fd);
	if (magic != CAT_MAGIC){
		return( CATD_ERR );
	}
#ifndef NO_SHMAT
	if ((catd->_mem = shmat( fileno( catd->_fd ) , 0 , SHM_MAP | SHM_RDONLY ) )==( char * )ERR ) {
#else
	{
#endif
/*______________________________________________________________________
	If the file can not be mapped then simulate mapping for the index
	table so that make_sets cat set things up. (rmalloc an area big
 	enough for the index table and read the whole thing in)
  ______________________________________________________________________*/
		int i;	/*---- Misc counter(s) used for loops ----*/
		struct _catset cs;


		fseek(catd->_fd,0,0);
		catd->_hd = (struct _header *) rmalloc(sizeof(struct _header));
		fread(catd->_hd,sizeof(struct _header),1,catd->_fd);

		for (i = 0 ; i < catd->_hd->_n_sets ; i++) {
			fread(&cs,4,1,catd->_fd);
			fseek(catd->_fd, cs._n_msgs * sizeof(struct _msgptr), 1);
		}

		i = ftell(catd->_fd);
		catd->_mem = (char *)rmalloc(i);
		fseek(catd->_fd,0,0);
		fread(catd->_mem,i,1,catd->_fd);
		catd->_set = (struct _catset *) rmalloc((catd->_hd->_setmax + 1) * sizeof (struct _catset));
		catd->_setmax = catd->_hd->_setmax;
		make_sets(catd->_set,catd->_mem,catd->_hd->_n_sets);
		free(catd->_mem);
		catd->_mem = FALSE;
		return(catd);
	}
#ifndef NO_SHMAT
	else {
/*______________________________________________________________________
	Normal mapping has occurred, set a few things up and call make_sets
  ______________________________________________________________________*/
		catd->_hd =( struct _header * )( catd->_mem );
		catd->_setmax = catd->_hd->_setmax;
		catd->_set = (struct _catset *) rmalloc((catd->_hd->_setmax + 1) * sizeof (struct _catset));
	
		make_sets(catd->_set,catd->_mem,catd->_hd->_n_sets);
		return(catd);
	}
#endif
}



/*
 * 
 * NAME: make_sets
 *
 * FUNCTION: Expands the compacted version of the catalog index table into
 *           the fast access memory version.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */


/*______________________________________________________________________*/
#ifdef _ANSI
static void make_sets(struct _catset *cset, char *base, int n_sets)
#else	/*  _ANSI ifdef ...  */
static void make_sets(cset,base,n_sets)
struct 	_catset	*cset;	/*---- place to store the sets ----*/
char 		*base;	/*---- base of the catalog memory (mapped or not) ----*/
int		n_sets;	/*---- number of sets in _header table ----*/
#endif	/*  _ANSI ifdef ...  */
{
	int 	i;	/*---- Misc counter(s) used for loops ----*/
	int	j;	/*---- Misc counter(s) used for loops ----*/
	int 	msgmax;	/*---- The maximum number of _messages in a set ----*/
	char 	*cmpct_set_ptr;	/*---- pointer into the index table ----*/
	struct _catset	cs;	/*---- used to look at the sets in the table ----*/

	cmpct_set_ptr = base + sizeof(struct _header);

	for (i = 0 ; i < n_sets ; i++) {			/* loop through each compacted set 	*/
		cs = *(struct _catset *)cmpct_set_ptr;			/* set the _catset ptr to the base of
								   the current compacted set 		*/
		cs._mp = (struct _msgptr *)(cmpct_set_ptr + 2*sizeof(unsigned short));	/* set the ms array ptr to the base of
								   compacted array of _msgptr's 		*/
		for (j = 0 ,msgmax = 0 ; j < cs._n_msgs ; j++) {	/* find the highest msgno in the set	*/
			if (cs._mp[j]._msgno > msgmax)
				msgmax = cs._mp[j]._msgno;
		}
		msgmax++;					/* allocate memory for the expanded 
								   array (this one will have holes)	*/
		cset[cs._setno]._mp = (struct _msgptr *) rmalloc(msgmax * sizeof(struct _msgptr));
		cset[cs._setno]._msgtxt = (char **)rmalloc(msgmax * sizeof (char **));

		for (j = 0 ; j < msgmax ; j++) 			/* mark all the _msgptr's as being empty	*/
			cset[cs._setno]._mp[j]._offset = FALSE;

		for (j = 0 ; j < cs._n_msgs ; j++) {		/* Fill the appropriate ones with data 	*/
			cset[cs._setno]._mp[cs._mp[j]._msgno] = cs._mp[j];
		}
		cset[cs._setno]._n_msgs = msgmax;	
		cset[cs._setno]._setno = cs._setno;		/*----  superfluous but should 
									have the correct data  ----*/
								/* Increment the base of the set pointer */
		cmpct_set_ptr += 2 * sizeof(unsigned short) + cs._n_msgs * sizeof(struct _msgptr);
	}
}



/*
 * 
 * NAME: opencatfile
 *
 * FUNCTION: Opens a catalog file by first searching the language path, if 
 *           there is no slash. Returns a pointer to the file stream.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS:  A pointer to the file stream, and a NULL pointer on
 *           failure.
 */


#ifdef _ANSI
static FILE *opencatfile(char *file)
#else	/*  _ANSI ifdef ...  */
static FILE *opencatfile(file)
char 	*file;
#endif	/*  _ANSI ifdef ...  */
{
	char *NLgetenv();


	if (strchr(file,'/')) {
		return(fopen(file,"r"));
	}
	else {
		char 	fl[PATH_MAX];	/*---- place to hold full path ----*/
		char	*lang;		/*---- pointer to the lang value ----*/
		char 	*nlspath;	/*---- pointer to the nlspath val ----*/
		FILE	*fp;		/*---- file pointer ----*/
		char	cpth[PATH_MAX]; /*---- current value of nlspath ----*/
		char *p,*np;

		if (!(nlspath = NLgetenv("NLSPATH")))
			nlspath = PATH_FORMAT;
		if (!(lang = NLgetenv("LANG")))
			lang = DEFAULT_LANG;

		np = nlspath;
		while (*np) {
			p = cpth;
			while (*np && *np != ':')
				*p++ = *np++;
			*p = NULL;
			if (*np)			/*----  iff on a colon then advance  ----*/
				np++;
			
			if (strlen(cpth)) {
				substitute(cpth,"%N",file,PATH_MAX);
				substitute(cpth,"%L",lang,PATH_MAX);
			}
			else {			/*----  iff leading | trailing | adjacent colons ...  ----*/
				strcpy(cpth,file);
			}
			
			if ((fp = fopen(cpth,"r")))
				return(fp);
		}
		return(fopen(file,"r"));
	}
}

/*
 * 
 * NAME: cat_already_open
 *
 * FUNCTION: Checks to see if a specific cat has already been opened.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: A pointer to the existing CATD.
 *          A NULL pointer if no CATD exists.
 */

static CATD *catsopen[NL_MAXOPEN];	/*---- list of open catalog pointers ----*/

#ifdef _ANSI
static nl_catd cat_already_open(char *cat)
#else	/*  _ANSI ifdef ...  */
static nl_catd cat_already_open(cat)
char *cat;	/*---- name of the catalog to be opened ----*/
#endif
{
	int i;	/*---- Misc counter(s) used for loops ----*/
	
	for (i = 0 ; i < NL_MAXOPEN && catsopen[i] ; i++)  {
		if (!strcmp(cat,catsopen[i]->_name)) {
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
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */

#ifdef _ANSI
static void add_open_cat(nl_catd catd)
#else	/*  _ANSI ifdef ...  */
static void add_open_cat(catd)
nl_catd catd;	/*---- catd to be added to the list of catalogs ----*/
#endif	/*  _ANSI ifdef ...  */
{
	int i = 0;	/*---- Misc counter(s) used for loops ----*/

	while (i < NL_MAXOPEN && catsopen[i]) {
		if (!strcmp(catd->_name,catsopen[i]->_name))
			return;	/*---- The catalog is already here ----*/
		i++;
	}

	if (i < NL_MAXOPEN)
		catsopen[i] = catd;
}
/*
 * 
 * NAME: catclose
 *                                                                    
 * FUNCTION: Closes catalog. Closes the stream and frees the memory with it.
 *                                                                    
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: 0 on success.
 *         -1 on failure.
 *
 */ 


 
#ifdef _ANSI
int catclose(nl_catd catd)
#else	/*  _ANSI ifdef ...  */
int catclose(catd)
nl_catd 	catd;	/*---- the catd to be closed ----*/
#endif	/*  _ANSI ifdef ...  */
{
	void cat_hard_close();	/*----  physically closes the cat 
					and frees the memory 
					also removes entry from 'catsopen[]' ----*/


	if (catd == CATD_ERR || catd->_stat == -1)	/*----  return if catd bad  ----*/
		return(-1);
	
	if (cat_already_open(catd->_name)) {
		if (fcntl(fileno(catd->_fd),F_SETFD,1) == ERR) {
			cat_hard_close(catd);
			return(-1);
		}
		else {
			return(0);
		}
	}
	else {
		cat_hard_close(catd);
		return(0);
	}
}



/*
 *
 * NAME: cat_hard_close
 *
 * FUNCTION: Closes a catalog and frees the memory with it. Deletes the 
 *           catd from the list of open catalogs.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */


#ifdef _ANSI
static void cat_hard_close(nl_catd catd)
#else 	/*  _ANSI ifdef ...  */
static void cat_hard_close(catd)
nl_catd 	catd;	/*---- the catd to be closed ----*/
#endif	/*  _ANSI ifdef ...  */
{
	int i;	/*---- Misc counter(s) used for loops ----*/
	int j;	/*----  Misc counter ----*/

	if (catd == CATD_ERR)
		return;
	
/*______________________________________________________________________
	remove any entry for the catalog in the catsopen array
  ______________________________________________________________________*/
	for (i = 0 ; i < NL_MAXOPEN && catsopen[i] ; i++) {
		if (catd == catsopen[i]) {
			memcpy(catsopen[i],catsopen[i + 1],NL_MAXOPEN - i - 1);
			break;
		}
	}
/*______________________________________________________________________
	close the cat and free up the memory
  ______________________________________________________________________*/

	for (i = 0 ; i < catd->_hd->_setmax ; i++) {
		if (catd->_set[i]._mp) 
			free(catd->_set[i]._mp);	/*---- free the _message pointer arrays ----*/
		if (catd->_set[i]._msgtxt) {
			for (j = 0 ; j < catd->_set[i]._n_msgs ; j++) {
				if (catd->_set[i]._msgtxt[j])
					free(catd->_set[i]._msgtxt[j]);
			}
			free(catd->_set[i]._msgtxt);
		}
	}

	if (catd->_fd);
		fclose(catd->_fd);	/*---- close the ctatlog ----*/

	if (catd->_set) 
		free(catd->_set);	/*---- free the sets ----*/
	if (catd)
		free(catd);		/*---- free the catd ----*/


}


/*
 * NAME: substitute
 *
 * FUNCTION: Replaces occurrences of one string within another.
 *           Substitute value1 in line with value2.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */


#ifdef _ANSI
static void substitute( char *line, char *val1, char *val2)
#else	/*  _ANSI ifdef ...  */
static void substitute( line, val1, val2 )
char line[];
char val1[];
char val2[];
#endif	/*  _ANSI ifdef ...  */
{
	int i, j;
	int v1_len, 
	v2_len, 
	ln_len;

	v1_len = strlen( val1 );
	v2_len = strlen( val2 );
	ln_len = strlen( line );

	for ( i = 0 ; i < ln_len ; i++ ) {
		if ( !memcmp( &line[i], val1, v1_len ) ) {
			memmove( &line[i + v2_len], &line[i + v1_len], strlen( &line[i + v1_len] ) + 1 );
			memcpy(&line[i],val2,v2_len);
			return;
		}
	}
}


/*
 * NAME: memmove
 *
 * FUNCTION: Moves memory within overlapping areas.
 *
 * EXECUTION ENVIRONMENT:
 * 	User mode.
 *                                                                    
 * RETURNS: void
 */


#ifdef _ANSI
static void memmove(char *t, char *s, int n)
#else	/*  _ANSI ifdef ...  */
static void memmove(t,s,n)
char *t;
char *s;
int n;
#endif	/*  _ANSI ifdef ...  */
{
	char buf[1028]; 

	memcpy(buf,s,n);
	memcpy(t,buf,n);
}
