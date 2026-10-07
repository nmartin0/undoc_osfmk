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
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: crontab.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:05:09 $";
#endif
/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: crontab
 *
 * ORIGINS: 27
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
 * crontab.c   1.15  com/cmd/oper/cron,3.1,9021 4/3/90 19:08:38
 */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>

extern priv_t *privvec();
#endif

#include <sys/id.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <pwd.h>
#include <stdio.h>
#include <fcntl.h>
#include <locale.h>
#include <time.h>
#include "cron.h"
#include <NLctype.h>
#define ISDIGIT NCisdigit
#include "cron_msg.h"
#define MSGSTR(Num,Str) catgets(catd,MS_CRON,Num,Str)
nl_catd catd;

#define TMPFILE		"_cron"		/* prefix for tmp file */
#define CRMODE		0440	/* mode for creating crontabs */

#define DEFAULT_EDITOR  "vi"    /* default editor for -e option */

#define BADCREATE	"cannot create your crontab file in the crontab directory."
#define INVALIDUSER	"you are not a valid user (no entry in /etc/passwd)."
#define NOTALLOWED	"you are not authorized to use cron.  Sorry."
#define EOLN		"unexpected end of line."
#define UNEXPECT	"unexpected character found in line."
#define OUTOFBOUND	"number out of bounds."
#define BADOPEN		"cannot open your crontab file."
#define BADPARM		"%s: specify only one flag or file name\n"
#define NOBLANK		"Blank lines are not allowed.\n"
#if SEC_MAC || SEC_ILB || SEC_NCAV
#define INSUFFPRIV      "%s: insufficient privileges\n"
#endif
#define RFLAG		0x00000001
#define LFLAG		0x00000002
#define VFLAG		0x00000004
#define EFLAG           0x00000008

int err,cursor;
char *cf,*tnam,line[CTLINESIZE];
extern char *xmalloc();
char login[UNAMESIZE];

extern int list_cj();
extern int remove_cj();

extern struct passwd *getpwuid(uid_t uid);
extern uid_t getuid(void);

int usage(void);
int sendmsg( char action, char *fname );
void crabort( char *msg );
static void cerror( char *msg );
int catch(void);
int next_field( int lower, int upper );
void copycron( FILE *fp );
void editcron(void);

/*
 * NAME: crontab
 *                                                                    
 * FUNCTION:  crontab: Submits a schedule of commands to cron.
 *	description:	This program implements crontab (see cron(1)).
 *			This program should be set-uid to root.
 *	files:
 *		/var/adm/cron 			drwxr-xr-x root sys
 *		/var/adm/cron/cron.allow 	-rw-r--r-- root sys
 *		/var/adm/cron/cron.deny 	-rw-r--r-- root sys
 */  
main(int argc, char **argv)
{
	char *strcat(),*strcpy();
	char *pp;
	FILE *fp;
	struct passwd *nptr;
	int funct=0;
	int flag;
	(void ) setlocale(LC_ALL,"");

	catd = catopen(MF_CRON,0);

#if SEC_BASE
        set_auth_parameters(argc, argv);
        initprivs();
#if SEC_MAC
        disablepriv(SEC_MULTILEVELDIR);
#endif
#if SEC_MAC || SEC_ILB || SEC_NCAV
        if (forceprivs(privvec(
#if SEC_MAC
                                SEC_WRITEUPSYSHI,
#endif
#if SEC_ILB
                                SEC_ILNOFLOAT,
#endif
#if SEC_NCAV
                                SEC_ALLOWILBACCESS,
#endif
                                -1), (priv_t *) 0)) {
                fprintf(stderr, MSGSTR(MS_INSUFFPRIV, INSUFFPRIV), "crontab");
                exit(1);
        }
#endif /* SEC_MAC || SEC_ILB || SEC_NCAV */
#endif /* SEC_BASE */

	/*
	 * only 2 arguments allowed
	 */

	if (argc>=3) {
		fprintf(stderr, MSGSTR(MS_BADPARM, BADPARM),"crontab");
		exit(1);
	}

#if SEC_BASE
        if ((nptr=getpwuid(getluid())) == NULL) /* check the user id */
#else
	if ((nptr=getpwuid(getuid())) == NULL) 	/* check the user id */
#endif 
		crabort(MSGSTR(MS_INVALIDUSER, INVALIDUSER));
	else
		pp = nptr->pw_name;		/* user's crontab file */

	strcpy(login,pp);			/* save login name */

	/*
	 * determine function: list, verbose list, remove or add
	 */

	while ((flag = getopt (argc, argv, "rlve")) != EOF) {
		switch (flag) {
			case 'r':
				funct = RFLAG;
				break;
			case 'l':
				funct = LFLAG;
				break;
			case 'v':
				funct = VFLAG;
				break;
                       case 'e':
                                funct = EFLAG;
                                break;
			default:
				usage ();
		}
	}
	if ((argc==2)&&(!funct)&&(argv[1][0]=='-')) /* invalid command line */
		usage();
		
	/*
	 * check the .permit and .deny files (part of SVID)
	 */

	if (!allowed(login,CRONALLOW,CRONDENY)) 
		crabort(MSGSTR(MS_CRNOTALLOWED, NOTALLOWED));

	switch(funct) {
		case VFLAG:     		/* vebose listing */
			list_cj(CRON_NON_VERBOSE,pp);
			break;

		case LFLAG:     		/* list */
			list_cj(CRON_VERBOSE,pp);
			break;

		case RFLAG:     		/* remove */
			remove_cj(CRON_QUIET,pp);
			break;

                case EFLAG:                     /* edit */
                        editcron();
                        sendmsg(ADD, login);
                        break;

		default:			/* add */
			if (argc==1) 
				copycron(stdin);
			else {			/* open the crontab file */
				if ((fp=fopen(argv[1],"r"))==NULL) 
					crabort(MSGSTR(MS_BADOPEN, BADOPEN));
		     		else 
					copycron(fp);
			}
			sendmsg(ADD,login);
	}
	exit(0);
}

/*
 * NAME: copycron
 *                                                                    
 * FUNCTION: copy cron event to cron directory
 */  

void
copycron(FILE *fp)
{
	FILE *tfp;
	char pid[6],*strcat(),*strcpy(),*strchr();
	int t;
	struct passwd *pw;
	int line_num=0;

	/*
	 * create the new crontab file name
	 */

	cf = xmalloc(strlen(CRONDIR)+strlen(login)+2);
	sprintf(cf,"%s/%s",CRONDIR,login);

	sprintf(pid,"%-5d",getpid());
	tnam=xmalloc(strlen(CRONDIR)+strlen(TMPFILE)+7);
	sprintf(tnam,"%s/%s%s",CRONDIR,TMPFILE,pid);
	/* catch SIGINT, SIGHUP, SIGQUIT signals */
	if (signal(SIGINT,(void (*)(int))catch) == SIG_IGN) 
		signal(SIGINT,SIG_IGN);
	if (signal(SIGHUP,(void (*)(int))catch) == SIG_IGN) 
		signal(SIGHUP,SIG_IGN);
	if (signal(SIGQUIT,(void (*)(int))catch) == SIG_IGN) 
		signal(SIGQUIT,SIG_IGN);
	if (signal(SIGTERM,(void (*)(int))catch) == SIG_IGN) 
		signal(SIGTERM,SIG_IGN);
#if SEC_BASE
        if ((t = crontab_secure_create(tnam)) == -1)
#else
	if ((t=creat(tnam,(mode_t)CRMODE))==-1) 
#endif
		crabort(MSGSTR(MS_BADCREATE, BADCREATE));
	if ((tfp=fdopen(t,"w"))==NULL) {
		unlink(tnam);
		crabort(MSGSTR(MS_BADCREATE, BADCREATE)); 
	}
	
	err=0;	/* if errors found, err set to 1 */
	while (fgets(line,CTLINESIZE,fp) != NULL) {
		cursor=0;
		line_num++;
		while(NCisspace(line[cursor]) || line[cursor] == '\t')
			cursor++;
		if ((line[cursor] == '\n') || (line[cursor] == '\0'))
			crabort(MSGSTR(MS_NOBLANK, NOBLANK));

		if ((line[cursor] == '#') && (line_num == 1)) {
			if ((pw=getpwuid(getuid())) == NULL)
				crabort(MSGSTR(MS_INVALIDUSER, 
					INVALIDUSER));
			else {
				fputs(strcat(pw->pw_name,"\n"),tfp);
				fputs(line,tfp);
				continue;
			}
		}
		else {		/* only login name on 1st line */
			if (strchr(line,' ') == NULL) 
				goto cont;
			if (line[cursor] == '#')
				goto cont;
		}
		if (next_field(0,59)) continue;
		if (next_field(0,23)) continue;
		if (next_field(1,31)) continue;
		if (next_field(1,12)) continue;
		if (next_field(0,06)) continue;
		if (line[++cursor] == '\0') {
			cerror(MSGSTR(MS_EOLN, EOLN));
			continue; 
		}
cont:
		if (fputs(line,tfp) == EOF) {
			unlink(tnam);
			crabort(MSGSTR(MS_BADCREATE, BADCREATE)); 
		}
	}
	fclose(fp);
	fclose(tfp);
	if (!err) {
		/* make file tfp the new crontab */
		unlink(cf);
		if (link(tnam,cf)==-1) {
			unlink(tnam);
			crabort(MSGSTR(MS_BADCREATE, BADCREATE)); 
		} 
	}
	unlink(tnam);
}

/*
 * NAME: next_field
 *                                                                    
 * FUNCTION:  check the next field for invalid input
 */  
int
next_field(int lower, int upper)
{
	int num,num2;

	while ((line[cursor]==' ') || (line[cursor]=='\t')) cursor++;
	if (line[cursor] == '\0') {
		cerror(MSGSTR(MS_EOLN, EOLN));
		return(1); 
	}
	if (line[cursor] == '*') {
		cursor++;
		if ((line[cursor]!=' ') && (line[cursor]!='\t')) {
			cerror(MSGSTR(MS_UNEXPECT, UNEXPECT));
			return(1); 
		}
		return(0); 
	}
	while (TRUE) {
		if (!ISDIGIT(line[cursor])) {
			cerror(MSGSTR(MS_UNEXPECT, UNEXPECT));
			return(1); 
		}
		num = 0;
		do { 
			num = num*10 + (line[cursor]-'0'); 
			++cursor;	/* for KANJI */
		} while (ISDIGIT(line[cursor]));
		if ((num<lower) || (num>upper)) {
			cerror(MSGSTR(MS_OUTOFBOUND, OUTOFBOUND));
			return(1); 
		}
		if (line[cursor]=='-') {
			++cursor;	/* for KANJI */
			if (!ISDIGIT(line[cursor])) {
				cerror(MSGSTR(MS_UNEXPECT, UNEXPECT));
				return(1); 
			}
			num2 = 0;
			do { 
				num2 = num2*10 + (line[cursor]-'0'); 
				++cursor;	/* for KANJI */
			} while (ISDIGIT(line[cursor]));
			if ((num2<lower) || (num2>upper)) {
				cerror(MSGSTR(MS_OUTOFBOUND, OUTOFBOUND));
				return(1); 
			}
		}
		if ((line[cursor]==' ') || (line[cursor]=='\t')) break;
		if (line[cursor]=='\0') {
			cerror(MSGSTR(MS_EOLN, EOLN));
			return(1); 
		}
		if (line[cursor++]!=',') {
			cerror(MSGSTR(MS_UNEXPECT, UNEXPECT));
			return(1); 
		}
	}
	return(0);
}


/*
 * NAME: cerror
 *                                                                    
 * FUNCTION:  print out error message for cron event
 */  
static void
cerror( char *msg)
{
	char message[256];
	strcpy(message,msg);	/* save message */
	fprintf(stderr,MSGSTR(MS_LINERR, "%scrontab: error on previous line; %s\n"),line,message);
	err=1;
}


/*
 * NAME: catch
 *                                                                    
 * FUNCTION: unlink if crontab object is invalid
 */  
int
catch(void)
{
	unlink(tnam);
	exit(1);
}


/*
 * NAME: crabort
 *                                                                    
 * FUNCTION:  print error message if crontab failed
 */  
void
crabort( char *msg)
{
	fprintf(stderr,MSGSTR(MS_CRABORT, "crontab: %s\n"),msg);
	exit(1);
}

/*
 * NAME: sendmsg
 *                                                                    
 * FUNCTION: print error message if problems with cron or at
 */  
int
sendmsg(char action, char *fname)
{

	static	int	msgfd = -2;
	struct	message	*pmsg;

#if SEC_MAC || SEC_NCAV
        pmsg = (struct message *) crontab_set_message(sizeof *pmsg);
#else
	pmsg = &msgbuf;
#endif
	if(msgfd == -2)
		if((msgfd = open(FIFO,O_WRONLY|O_NDELAY)) < 0) {
			if(errno == ENXIO || errno == ENOENT)
				fprintf(stderr,MSGSTR(MS_NOCRON, "cron may not be running - call your system administrator\n"));
			else
				fprintf(stderr,MSGSTR(MS_MSGQERROR, "crontab: error in message queue open\n"));
			return;
		}
	pmsg->etype = CRON;
	pmsg->action = action;
	strncpy(pmsg->fname,fname,FLEN);
#if SEC_MAC || SEC_NCAV
        if (!crontab_write_message(msgfd, (char *) pmsg, sizeof *pmsg))
#else
	if(write(msgfd,pmsg,sizeof(struct message)) != sizeof(struct message))
#endif
		fprintf(stderr,MSGSTR(MS_MSGSERROR, "crontab: error in message send\n"));
}

/*
 * NAME: usage
 *                                                                    
 * FUNCTION: print usage message and exit
 *                                                                    
 * EXECUTION ENVIRONMENT: user process
 *                                                                   
 * RETURNS: does not return exit(1)
 */  

int
usage(void)
{
	fprintf(stderr, MSGSTR(MS_CRBADUSAGE, 
	"Usage: crontab [-l|-r|-v|-e|File]\n"));
	exit(1);
}

/*
 * NAME: editcron
 *
 * FUNCTION: edit cron event in the file under cron directory
 *
 * EXECUTION ENVIRONMENT:
 *      This function overrides the contents of the old crontab file.
 *      If there is no crontab file then it will be created.
 * 	The crontab file name is the login name.
 *
 * (NOTES:)  This function uses a temporary file.
 *
 *	Get a temp file name.
 *	If we had an old crontab file, copy it in to our temp file.
 *	Invoke the editor on the tempfile.
 *	Read the tempfile and parse it into a tempfile in the cron directory.
 *	If the parsing went well, link it in as the new crontab file.
 *
 * RETURNS:  none
 */

void
editcron()
{
        FILE *tfp, *tfp2, *fp;
        char pid[6];
        char *edit,*s;
	char *tnam2;
        int wpid,fid,st;
	int j,t,line_num=0;
	int i;
        struct stat fst;
        /*
         * create the crontab file  and temporary file names
         */
        cf = xmalloc(strlen(CRONDIR)+strlen(login)+2);
        sprintf(cf,"%s/%s",CRONDIR,login);

	tnam = tmpnam(NULL);

	/* catch SIGINT, SIGHUP, SIGQUIT signals */
	if (signal(SIGINT,catch) == SIG_IGN) 
		signal(SIGINT,SIG_IGN);
	if (signal(SIGHUP,catch) == SIG_IGN) 
		signal(SIGHUP,SIG_IGN);
	if (signal(SIGQUIT,catch) == SIG_IGN) 
		signal(SIGQUIT,SIG_IGN);
	if (signal(SIGTERM,catch) == SIG_IGN) 
		signal(SIGTERM,SIG_IGN);

        if ((tfp=fopen(tnam,"w"))== NULL) {
                unlink(tnam); 
		free(cf);
       		crabort(MSGSTR(MS_BADCREATE, BADCREATE));
	}

	/* if the crontab exist? then open for read */
        if ((fid = stat(cf,&fst)) != -1) { 
        	if ((fp=fopen(cf,"r"))==NULL) {
			unlink(tnam);
			free(cf);
       			crabort(MSGSTR(MS_BADOPEN, BADOPEN));
		}
		/* copy the contents */
        	while (fgets(line,CTLINESIZE,fp) != NULL) 
                	if (fputs(line,tfp) == EOF ) {
        			fclose(fp);
        			fclose(tfp);
				unlink(tnam);
				free(cf);
        			crabort(MSGSTR(MS_BADCREATE, BADCREATE));
			}
        	fclose(fp);
        }
        fclose(tfp);

	/* Edit the temp file */
        wpid = fork();
        if (wpid <0) { 
		unlink(tnam);
		free(cf);
                crabort(MSGSTR(MS_BADCREATE, BADCREATE)); 
        }
        if (wpid == 0) { /* child process */

		/* give the file to the user */
		chmod(tnam, S_IRUSR|S_IWUSR);
		chown(tnam, getuid(), getgid());

                edit = getenv("EDITOR");
                if (edit == NULL )
                	edit= DEFAULT_EDITOR;
                execlp(edit, edit, tnam, 0);
                exit(0);
        }
        if ((wait(&st) != wpid) || (st)) {
		unlink(tnam);
		free(cf);
                crabort(MSGSTR(MS_BADCREATE, BADCREATE)); 
        } 

	/* Copy the tempfile in to a tempfile in the cron directory */
	/* We parse it as we go along for errors.  */

        if ((tfp=fopen(tnam,"r"))==NULL) {
		unlink(tnam);
		free(cf);
                crabort(MSGSTR(MS_BADCREATE, BADCREATE)); 
	}

        sprintf(pid,"%-5d\0",getpid());
        tnam2=xmalloc(strlen(CRONDIR)+strlen(TMPFILE)+7);
        sprintf(tnam2,"%s/%s%s\0",CRONDIR,TMPFILE,pid);
        for (j=strlen(tnam2)-1 ; j > 0 ; j--) {
                if (tnam2[j] == ' ') tnam2[j] = '\0';
                else break;
        }
#if SEC_BASE
        if ((t = crontab_secure_create(tnam2)) == -1)
#else
	if ((t = creat(tnam2,CRMODE))== -1) 
#endif
        	crabort(MSGSTR(MS_BADCREATE, BADCREATE));

	if ((tfp2=fopen(tnam2,"w"))== NULL) {
		unlink(tnam2);
		free(tnam2);
		free(cf);
		crabort(MSGSTR(MS_BADCREATE, BADCREATE));
	}

	/* At this point:
	 *		tfp = /tmp/foo  (source)
	 * 		tfp2 = /var/adm/cron/crontabs/_cron12345 (dest)
	 *
	 * Parse tfp in to tfp2.
	 */

        err=0;  /* if errors found, err set to 1 */
	while (fgets(line,CTLINESIZE,tfp) != NULL) {
		cursor=0;
		line_num++;
		while(isspace(line[cursor]) || line[cursor] == '\t')
			cursor++;
/*
		if ((line[cursor] == '\n') || (line[cursor] == '\0')) {
			unlink(tnam);
			crabort(MSGSTR(MS_NOBLANK, NOBLANK));
		}
*/
		if ((line[cursor] == '\n') || (line[cursor] == '\0'))
			continue;
		if (strchr(line,' ') == NULL) 
			goto cont;
		if (line[cursor] == '#')
			goto cont;
		if (next_field(0,59)) continue;
		if (next_field(0,23)) continue;
		if (next_field(1,31)) continue;
		if (next_field(1,12)) continue;
		if (next_field(0,06)) continue;
		if (line[++cursor] == '\0') {
			cerror(MSGSTR(MS_EOLN, EOLN));
			continue; 
		}
cont:
		if (fputs(line,tfp2) == EOF) {
			unlink(tnam2);
			crabort(MSGSTR(MS_BADCREATE, BADCREATE)); 
		}
	}
	fclose(tfp2);
        fclose(tfp);
	unlink(tnam);
        if (!err) {
                /* make file tfp2 the new crontab */
                unlink(cf);
                if (link(tnam2,cf)==-1) {
                        unlink(tnam2);
			free(tnam2);
			free(cf);
                        crabort(MSGSTR(MS_BADCREATE, BADCREATE)); 
		}
        } else {
		unlink(tnam2);
		free(cf);
		free(tnam2);
		exit(1);
	}
        unlink(tnam2);
	free(tnam2);
	free(cf);
}
