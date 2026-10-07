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
 *		Copyright (c) Locus Computing, 1991-92
 * 		This is UNPUBLISHED source code that is
 * 		the property of Locus Computing, containing
 *		proprietary secrets of LCC.  Any disclosure
 *		is strictly prohibited.  Locus makes no warantee,
 *		explicit or implicit, on the functionality of this code.
 * $Log: common.c,v $
 * Revision 1.4  1994/11/18  21:05:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  01:41:47  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.3  93/03/17  11:50:46  jpaul
 * Bug #140: fix shared memory.
 * Bug #200: fast case 13 failing; guard against shmem detaches
 *           when never attached.
 * Bug #203: fast should destroy shared memory created;  fastnode
 *           needs this too, common.c gets the changes and has functions
 *           called from both.
 * 
 * Revision 3.2  93/03/01  13:27:46  jpaul
 * Changes for common code to access shared memory.
 * 
 * Revision 3.1  92/12/18  11:50:09  jpaul
 * Add host for new VSTNC ping suite.
 * 
 * Revision 3.0  92/07/22  16:49:15  jpaul
 * Initial Checkin
 * 
 *
 *		This file contains routines used by more than one test.
 * 		It is compiled and linked in when tests are compiled.
 *		Functions:
 *			init_config_globals()
 *			conv_arg()
 *			add_node()
 *			my_stablk()
 *			fprintf() (commented out)
 *			setup_shared()
 *			error_fatal()
 *			shmemdestroy()
 *
 *
 */


/*******************************************************************************

Function:	init_config_globals

Returns:	0 for success, -1 for internal error.

Parameters:	none

Purpose:	The configuration file is read and its contents are
		stored in globals, available to all tests.  All the
		globals are defined here and externally referenced by
		the vstnc.h include file.  The globals are:

			config_nodecnt	(int) number of valid nodes
			config_nodelist	(int array) list of of valid node
			config_mynode	(int) the node the tests are run on
			config_goodnode	(int) a valid node number
			config_badnode	(int) a positive, invalid node number
			config_hinode	(int) the highest node number
			config_lonode	(int) the lowest node number
			config_userid1	(int) a non-root userid tests can use
			config_userid2	(int) a different non-root userid
			config_badaddr	(ptr) an invalid user address (not 0)
			config_PATH	(char array) $PATH environment variable
			config_TZ	(char array) $TZ environment variable


*******************************************************************************/

#include "../common/vstnc.h"
#include <stdio.h>
#include <ctype.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/stat.h>

/*
#define DEBUG 1
*/

#define PASSWHITESPACE(p)	while( isspace(*(p)) ) { ++(p); }
#define PASSDIGITS(p)		while( isdigit(*(p)) ) { ++(p); }
#define PASSALNUM(p)		while( isalnum(*(p)) ) { ++(p); }

#define LINELEN	4096

extern int	ntests;		/* each test program must define this */

int	config_nodecnt = 0;	/* init to 0, not -1, for nodecnt */
int	config_nodelist[MAXNODES];
int	config_goodnode = -1;
int	config_badnode = -1;
int	config_mynode = -1;
int	config_lonode = -1;
int	config_hinode = -1;
int	config_userid1 = -1;
int	config_userid2 = -1;
int	*config_badaddr = 0;	/* init to 0, not -1, for badaddr */
char	config_PATH[LINELEN] = { '\0' };
char	config_TZ[LINELEN] = { '\0' };
char 	*key_dir = {"/tmp"};

int conv_arg(char *);
int add_node(int);
int my_stablk(char *, char**);
int error_fatal(char *);
int shmid = 0;

/* for the fast and fastnode tests */
struct load_vec_info
{
	int node; /* node number */
	double lm; /* load measure */
};

struct load_info
{
   	int num_elements;
	struct load_vec_info load_vector[200];
} *load_i_ptr;


char *keywordtable[] = {
		"nodelist",	/* case 0 */	/* can be several */
		"goodnode",	/* case 1 */
		"badnode",	/* case 2 */
		"mynode",	/* case 3 */
		"userid1",	/* case 4 */
		"userid2",	/* case 5 */
		"badaddr",	/* case 6 */
		"verbose",	/* case 7 */	/* just a flag */
		"TZ",		/* case 8 */
		"PATH",		/* case 9 */
		0 };	/* must terminate list with 0 */


void init_config_globals()
{
	FILE *configfp;
	char line[LINELEN+1];
	char keyword[LINELEN+1];
	char errmsg[1024];
	char tmpch;
	char *p, *q;
	int node, lonode, hinode;
	int linenum = 0;
	int casenum;
	int verboseflag = 0;
	int configerr = 0;
	int len;
	int st;
	int i;

	setbuf(stdout, NULL);		/* prevent unflushed output */
	setbuf(stderr, NULL);		/* to both stderr and stdout */

	configfp = fopen(CONFIGFILE, "r");
	if (configfp == NULL) {
		perror("Open failed");
errmsg_exit:
		strcat(errmsg, " configuration file ");
		fprintf(stderr, "%s%s\n\n", errmsg, CONFIGFILE);
		fprintf(stderr, "TEST ABORTED.\n\n");
		exit(-1);
	}

	while( fgets(line, LINELEN, configfp) != NULL ) {
		++linenum;		/* number lines */
		len = strlen(line) - 1;	/* point at newline */
		if( len == 0 || line[0] == '#' )
			{ continue; }
		if( line[len] != '\n' ) {
			sprintf(errmsg, "Line %d too long in", linenum);
			goto errmsg_exit;
		}
		line[len--] = '\0';	/* null out the linefeed */
		while( isspace(line[len]) )
			{ --len; }	/* null out any trailing whitespace */	

		p = line;		/* p will point thru input line */
		PASSWHITESPACE(p);	/* find 1st ch past whitespace */
		if( strlen(p) == 0 )
			{ continue; }	/* just whitespace.  get next line */
		q = keyword;		/* where keyword starts */
		while( isalnum(*p) )
			{ *q++ = *p++; }/* copy keyword */
		*q = '\0';		/* terminate keyword string */
		casenum = my_stablk(keyword, keywordtable);
		if( casenum < 0 ) {
keyword_error:
			sprintf(errmsg, "Bad keyword '%s' on line %d of",
					keyword, linenum);
			goto errmsg_exit;
		}

		PASSWHITESPACE(p);
		if( *p++ != '=' ) {
bad_data_error:
			sprintf(errmsg, "Bad data for keyword '%s' line %d of",
					keyword, linenum);
			goto errmsg_exit;
		}
		PASSWHITESPACE(p);

		switch( casenum ) {

		case 0:	/* nodelist -- these accumulate */
		while( 1 ) {
			st = sscanf(p, "%d", &node);
			if( st == EOF )
				{ break; }	/* no more nodes, exit while */
			if( st != 1  ||  add_node(node) != 0 )
				{ goto bad_data_error; }
			PASSDIGITS(p);		/* pass number we just got */
			PASSWHITESPACE(p);
			if( *p == '-' ) {	/* parse "a - b" */
				++p;
				PASSWHITESPACE(p);
				lonode = node + 1;
				st = sscanf(p, "%d", &hinode);
				if( st != 1 )
					{ goto bad_data_error; }
				for( i=lonode; i<=hinode; ++i ) {
					if( add_node(i) != 0 )
						{ goto bad_data_error; }
					PASSDIGITS(p);
				}
				PASSWHITESPACE(p);
			}
		}
		break;

		case 1:	/* goodnode */
			if( sscanf(p, "%d", &config_goodnode) != 1 )
				{ goto bad_data_error; }
			PASSDIGITS(p);
			if( *p != '\0' )
				{ goto bad_data_error; }
			break;

		case 2:	/* badnode */
			if( sscanf(p, "%d", &config_badnode) != 1 ) {
				goto bad_data_error;
			}
			PASSDIGITS(p);
			if( *p != '\0' )
				{ goto bad_data_error; }
			break;

		case 3:	/* mynode */
			if( sscanf(p, "%d", &config_mynode) != 1 )
				{ goto bad_data_error; }
			PASSDIGITS(p);
			if( *p != '\0' )
				{ goto bad_data_error; }
			break;

		case 4:	/* userid1 */
			if( sscanf(p, "%d", &config_userid1) != 1 )
				{ goto bad_data_error; }
			if ( config_userid1 <= 0)
				{ goto bad_data_error; }
			PASSDIGITS(p);
			if( *p != '\0' )
				{ goto bad_data_error; }
			break;

		case 5:	/* userid2 */
			if( sscanf(p, "%d", &config_userid2) != 1  ||
					config_userid2 <= 0 )
				{ goto bad_data_error; }
			PASSDIGITS(p);
			if( *p != '\0' )
				{ goto bad_data_error; }
			break;

		case 6:	/* badaddr */
			config_badaddr = (int *)strtol(p, &p, 0);
			if( config_badaddr == 0 )
				{ goto bad_data_error; }
			if( *p != '\0' )
				{ goto bad_data_error; }
			break;

		case 7:	/* verbose -- just a flag */
			++verboseflag;
			break;

		case 8:	/* TZ */
			if( (len=strlen(p)) != 7 && len != 4 )
				{ goto bad_data_error; }
			strcpy(config_TZ, p);
			break;

		case 9:	/* PATH */
			if( (len=strlen(p)) < 3 || len > 128 )
				{ goto bad_data_error; }
			strcpy(config_PATH, p);
			break;

		default:
			goto keyword_error;
		}
	}
	fclose(configfp);		/* close the file now */


	/*
	 * Check that the configuration info is OK.
	 */

	if (config_nodecnt == 0) {
		fprintf(stderr, "'nodelist' keyword missing.\n");
		++configerr;
	}
	if (config_goodnode == -1) {
		fprintf(stderr, "'goodnode' keyword missing.\n");
		++configerr;
	}
	if (config_badnode == -1) {
		fprintf(stderr, "'badnode' keyword missing.\n");
		++configerr;
	}
	if (config_mynode == -1) {
		fprintf(stderr, "'mynode' keyword missing.\n");
		++configerr;
	}
	if (config_userid1 == -1) {
		fprintf(stderr, "'userid1' keyword missing.\n");
		++configerr;
	}
	if (config_userid2 == -1) {
		fprintf(stderr, "'userid2' keyword missing.\n");
		++configerr;
	}
	if (strlen(config_TZ) == 0 ) {
		fprintf(stderr, "'TZ' keyword missing.\n");
		++configerr;
	}
	if (strlen(config_PATH) == 0 ) {
		fprintf(stderr, "'PATH' keyword missing.\n");
		++configerr;
	}

	/*
	 * Check that config'd users are unique and neither are superuser.
	 */
	if ((config_userid1 == config_userid2)  &&  (config_userid1 != -1)) {
		fprintf(stderr, "'userid1' and 'userid2' must be different.\n");
		++configerr;
	}
	if ((config_userid1 == 0) || (config_userid2 == 0)) {
		fprintf(stderr, "config_userids can not be root (0)");
		++configerr;
	}

	/*
	 * Calculate some more official config_xxx values using the
	 * ones we've already validated.
	 */
	config_lonode = config_nodelist[0];
	config_hinode = config_nodelist[config_nodecnt - 1];

	if( verboseflag != 0 ) {
		printf("Verbose output requested:\n\n");
		printf("goodnode=%d   badnode=%d   userid1=%d   userid2=%d\n",
			config_goodnode, config_badnode, config_userid1,
			config_userid2);
		printf("nodecnt=%d   ", config_nodecnt);
		printf("nodelist=");
		for( i=0; i<config_nodecnt; ++i ) {
			printf(" %d", config_nodelist[i]);
		}
		printf("\n");
		fflush(stdout);
	}

	switch( configerr ) {
	case 0:
		return;		/* normal return, else we'll exit */

	case 1:
		fprintf(stderr, "\nThere was 1 configuration error\n");
		break;

	default:
		fprintf(stderr, "\nThere were %d configuration errors\n");
		break;
	}
	fprintf(stderr, "Correct the configuration file '%s' ");
	fprintf(stderr, "and run the tests again.\n");

	exit(1);
}

/*******************************************************************************

Function:	conv_arg

Returns:	The passed string converted to an int, or 0 if error.

Parameter:	string, a string that is valid iff a positive decimal
		value from 1 to 99.

Purpose:	Convert a positive integer argument, valid between one
		and 99 inclusive, from string to integer.


*******************************************************************************/


int
conv_arg( char *string )
{
	int value, i;

	value = atoi(string);
	if( value == 0 ) {
		for( i=1; i<=value; ++i )
			printf("%d ", i);
		printf("\n");
		exit(0);
	}

	if( value <= 0  ||  value > ntests )
		return(0);	/* error case */
	return( value );
}

/*******************************************************************************

Function:	add_node

Returns:	0 for success, -1 for error

Parameter:	node, a node to add to the nodelist

Purpose:	Perform various checks on the passed node, put it into
		the config_nodelist array, and count it in config_nodecnt.


*******************************************************************************/


int
add_node(
	int	node)
{
	int i;

	if( node < 0 )
		{ return( -1 ); }

	for( i=0; i<config_nodecnt; ++i ) {
		if( node <= config_nodelist[i] )
			{ return( -1 ); }	/* dup or not sorted */
	}

	config_nodelist[config_nodecnt++] = node;
						/* node ok: add and count it */
	return( 0 );

}

/*******************************************************************************

Function:	my_stablk

Returns:	-1 if no keyword match, else the index that matches

Parameter:	keyword, a string to match

		table, a NULL-terminated array of strings to try to match

Purpose:	Circumvent the absence of the library stablk routine.


*******************************************************************************/


int
my_stablk(
	char	*keyword,
	char	*keywordtable[])
{
	int i;

	for( i=0; keywordtable[i]!=NULL; ++i ) {
		if( strcmp(keyword, keywordtable[i]) == 0 )
			{ return( i ); }	/* we've got a match */
	}
	return( -1 );				/* nothing matched */
}
/*******************************************************************************

Function:	fprintf

Purpose:	Fake routine, since real fprintf is broken.  Send to
		stdout -- map fprintf into printf.

*******************************************************************************/

/*********
fprintf( stderr, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p )
int stderr;
int a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;
{
	return( printf(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) );
}
*******/


/*******************************************************************************

Function:	setup_shared

Returns:	0 for success, 1 for failure, -1 for internal error.

Parameters:	count: count of nodes in list.
		node_n_load[]: shared mem data for the nodes in list.

Purpose:	Support function for tests (fast and fastnode).

*******************************************************************************/

int
setup_shared(int count, 
	     struct load_vec_info node_n_load[3])
/* setup the shared memory so it can be read by the tests.  */
{

#ifndef MAXNAMLEN
#define MAXNAMLEN 1024
#endif
#ifndef READ_BY_OTHERS	
#define READ_BY_OTHERS 0x00004 /* allow shared memory segment access */
#endif
#define MAX_NUM_LVEC_ELEMENTS 10
#define KEYFILELEN 1024
#define INITVALUE -1.0

	int i;				/* loop var */
	int retval;			/* function call return val */
	key_t key;			/* shared mem key */
	struct stat buf;
	FILE *filep;
	char key_filename[MAXNAMLEN];
	char project_id;		/* used for key generation */	
	

	/* stat dir and create if needed.  */
	retval = stat(key_dir, &buf);
	if (retval == -1) {
		if (errno == ENOENT) {
			retval = mkdir(key_dir, 022);
			if (retval == -1) {
				fprintf(stderr, 
					"common.c: can't mkdir %s errno=%d\n", 
				 	key_dir, errno);
				fflush(stderr);
				exit(-1);
			}
		} else {
			fprintf(stderr,
				"common.c: stat of %s failed: %d, errno=%d\n",
				key_dir, retval, errno);	
			fflush(stderr);
			exit(-1);
		}
	}
		
	/* calculate filename & id for shared mem key */
	sprintf(key_filename, "%s/key_%d", key_dir, (node_self()/128));
	project_id = (char)((node_self() % 128)) + 1;
	/* set up shared memory and initialize the load vector */
	filep = fopen(key_filename, "w+");
	if (filep == NULL) {
		fprintf(stderr, "unable to open/create key file:%s\n", key_filename);
		fflush(stderr);
		return(-1);
	}
	retval = fclose(filep);
	if (retval != 0) {
		fprintf(stderr, "unable to close key file:%s\n", key_filename);
		fflush(stderr);
		return(-1);
	}


	/* generate shared memory key */
	key = ftok(key_filename, project_id);
	if ((key_t)key == -1) {
		fprintf(stderr, "unable to get key for shared mem segment\n");
		fprintf(stderr, "errno=%d\n", errno);
		fflush(stderr);
		return(-1);
	}
#ifdef DEBUG
	printf("common/setup_shared, key = %d\n", key);
#endif 
	/* obtain shared mem segment.  */
    	shmid = shmget(key, sizeof(int) +
    		   MAX_NUM_LVEC_ELEMENTS*(sizeof(struct load_vec_info)), 
    		   IPC_CREAT | READ_BY_OTHERS);
    	if (shmid == -1) {
		fprintf(stderr, "shmget failed:errno = %d\n", errno);
		fflush(stderr);
		return(-1);
    	}
    	load_i_ptr = (struct load_info *)shmat(shmid, 0, 0);
	if (load_i_ptr == (struct load_info*)-1)
		error_fatal("shared memory attach operation failed");

	/* insert the size of the load vector(number of elements) into the
	 * load info structure 
	 */
	load_i_ptr->num_elements = count;

    	if (load_i_ptr == (struct load_info*)-1) {
		fprintf(stderr, "Shared memory attach failed:errno = %d\n", 
			  errno);
		fflush(stderr);
		return(-1);
    	}
    	/* insert local node number into first element of load vector, and
       	 * initialize the rest of the load vector with our values.
	 */
    	load_i_ptr->load_vector[0].node = node_self();
    	load_i_ptr->load_vector[0].lm = INITVALUE;
    	for (i = 0; i < count; i++) {
        		load_i_ptr->load_vector[i].node = node_n_load[i].node;
        		load_i_ptr->load_vector[i].lm = node_n_load[i].lm;
		}
#ifdef DEBUG
	dump_shared();
#endif /* DEBUG */

	return(shmid); /* programs will look for < 0 otherwise use shmid. */
}

#ifdef DEBUG  
int
dump_shared() /* dump shared mem.  */
{
	int i;
	printf("Dumping shared mem....\n");
    	for (i = 0; i < load_i_ptr->num_elements; i++) {
    		printf("node = %d, lm = %g\n", load_i_ptr->load_vector[i].node,
    	       	load_i_ptr->load_vector[i].lm);
    	}
}
#endif /* DEBUG */

int
error_fatal(char *errmess)
{
	perror(errmess);
	exit(-1);
}

shmemdestroy()
{
	int shret;
	struct shmid_ds buf;

	/* detach from shared memory */
	shret = shmctl(shmid, IPC_RMID, &buf);
	if (shret != 0)
		printf("common:shmemdestroy():shmdestroy failed, errno=%d\n", errno);
    	shret = shmdt((struct load_info *)load_i_ptr);
	return(shret);
}
