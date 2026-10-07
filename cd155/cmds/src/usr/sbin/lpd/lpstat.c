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
static char rcsid[] = "@(#)$RCSfile: lpstat.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:08:29 $";
#endif
/*
 * lpstat -- Give various status information about the spooling system 
 *
 * This interfaces just to lpq, and lpc the "official" programs.
 * And does some lookups in printcap and /usr/spool... about trivial requests
 */

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <locale.h>
#include "lp.h"

#define TRUE 1
#define FALSE 0

#define LPQ "lpq"
#define LPQPATH "/usr/bin/lpq"
#define LPC "lpc"
#define LPCPATH "/usr/sbin/lpc"

#define DASH  '-'
#define COMMA ','
#define SPACE ' '

#define NAMESIZE 20
#define ID 1
#define JOB 0
struct list			/* List of USER or JOB names attached */
{				/* to an option/switch */
   struct list *next;
   short type; 
   char name[NAMESIZE+1];
};
typedef struct list List;

struct option			/* List of options/switches which */
{				/* might have Lists attached */
    struct option *next;
    char   optChar;
    List   *selectors;
};
typedef struct option Option;

struct job			/* Printer Job numbers to examine */
{
    struct job *next;
    char *jobNumber;
};
typedef struct job Job;
Job *JobList;			/* a dsa sd sdsdksjk jsdfkjsk fjf jd */
Option *OptionList;		/* Yes! */

short printSchedulerStatus,	/* Flags/Options */
    printSystemDefault,
    printSummary,
    printAll;

char line[BUFSIZ], pbuf[BUFSIZ/2], *bp = pbuf;

char *DefaultDest;		/* The default printers name */
void fatal();

main(argc, argv)
int argc;
char *argv[];
{
    void getOptions(),
         getSystemDefault(),
         showOutputStatusForUsers(),
         showSchedulerStatus(),
         showDevices(),
         showOutputStatus(),
         showPrinterStatus(),
         showAcceptanceStatus();
    Option *thisOption;

    (void) setlocale( LC_ALL, "" );
    catd = catopen(MF_PRINTER,0);
    getSystemDefault();
    if (argc == 1)		/* No Options == Information for this user */
    {
	List thisUser;
	char *getlogin();
	char *loginName = getlogin();
	
	if (loginName == NULL)
	    loginName = (getpwuid(getuid()))->pw_name;
	thisUser.type = ID;
	strncpy(thisUser.name, loginName, NAMESIZE);
	thisUser.next = NULL;
	showOutputStatusForUsers(&thisUser);
	exit(0);
    }
	    
    getOptions(argc, argv);	/* and all the lists associated */
    if (printSystemDefault && !printSummary && !printAll)
	printf(MSGSTR(LPSTAT_1, "System default destination: %s\n"), DefaultDest);
    if (printSchedulerStatus && !printSummary && !printAll)
	showSchedulerStatus();
    if (printSummary || printAll)
    {
	printf(MSGSTR(LPSTAT_1, "System default destination: %s\n"), DefaultDest);
	showSchedulerStatus();
	showDevices((List *)NULL);
	if (printAll)
	{
	    showAcceptanceStatus((List *)NULL);
	    showOutputStatus((List *)NULL);
	}
	exit(0);		/* Showed everything already */
    }
    for (thisOption = OptionList; thisOption; thisOption = thisOption->next)
	if (thisOption->optChar == 'a')
	    showAcceptanceStatus(thisOption->selectors);
	else if (thisOption->optChar == 'o')
	    showOutputStatus(thisOption->selectors);
	else if (thisOption->optChar == 'p')
	    showPrinterStatus(thisOption->selectors);
	else if (thisOption->optChar == 'u')
	    showOutputStatusForUsers(thisOption->selectors);
	else if (thisOption->optChar == 'v')
	    showDevices(thisOption->selectors);
    exit(0);
}


void getOptions(argc, argv)
int argc;
char **argv;
{

   List *getList();
    while (--argc)
    {
	char *arg = *++argv;
	if (*arg != DASH)	/* Must be a request id, i.e. job number */
        {
	    Job *currentJob = (Job *)malloc(sizeof(Job));
	    if (currentJob == NULL)
		fatal(MSGSTR(LPSTAT_2, "Out of memory"));
	    currentJob->jobNumber = arg;
	    currentJob->next = JobList;
	    JobList = currentJob;
	}
	else
	{
	    switch (arg[1])
	    {
	    case 'd': printSystemDefault = TRUE; break;
	    case 'r': printSchedulerStatus = TRUE; break;
	    case 's': printSummary = TRUE; break;
	    case 't': printAll = TRUE; break;
	    case 'a':		/* acceptance status */
	    case 'o':		/* output request status */
	    case 'p':		/* printer status */
	    case 'u':		/* user request status */
	    case 'v':		/* printers and paths */
	    {
		Option *currentOption = (Option *)malloc(sizeof(Option));
		if (currentOption == NULL)
		    fatal(MSGSTR(LPSTAT_2, "Out of memory"));
		currentOption->optChar = arg[1];
		currentOption->selectors = getList(arg+2);
		currentOption->next = OptionList;
		OptionList = currentOption;
		break;
	    }
	    case 'c':		/* class names ??? */
		fprintf(stderr, MSGSTR(LPSTAT_3, "Printer classes currently not supported\n"));
		break;

	    default:
		usage();
		/* fprintf(stderr, "wrong option: -%c ...ignored\n", arg[1]);*/
		break;
            }
	}
    }
#ifdef DEBUG
    (void)showOptions();
#endif
}    

	

/*
 * getList takes a list consiting of usernames, printernames or
 * jobnumbers separated by commas and/or space and puts it into
 * a List structure for further use.
 */        
List *getList(text)
char *text;
{
    char *this = text;
    List *head = (List *)NULL,*tail = (List *)NULL,
   	*currentElement;
    int firstCount = 0;

    if (*this == NULL)
	return (List*)NULL;

    while (*this)		/* So at least on thing is there */
    {
	int i = 0, nan = FALSE;	/* nan == Not A Number */

	currentElement = (List*)malloc(sizeof(List));
	if (currentElement == NULL)
	    fatal(MSGSTR(LPSTAT_2, "Out of memory"));

		currentElement->next = (List *)NULL;;

        if(firstCount == 0) 
		head = currentElement;
        else 
         	tail->next = currentElement;

                tail =  currentElement;

	while(*this && *this != COMMA && *this != SPACE)
	{			/* within one specifier */
	    if (!isdigit(*this))
		nan = TRUE;
	    currentElement->name[i++] = *this++;
	    if (i == NAMESIZE)	/* skip & drop the rest */
		while (*this && *this != COMMA && *this != SPACE)
		    this++;
	}
	currentElement->name[i] = '\0';
	currentElement->type = nan;
	
	while (*this && (*this == COMMA || *this == SPACE))
	    this++;		/* skip separators */

        firstCount++;
    }
    return head;
}

void getSystemDefault()
{
      char *getenv();
      
      DefaultDest = getenv("LPDEST");
      if (DefaultDest == NULL || strlen(DefaultDest) == 0)
	  DefaultDest = getenv("PRINTER");
      if (DefaultDest == NULL || strlen(DefaultDest) == 0)
	  DefaultDest = DEFLP;
}

void showOutputStatusForUsers(users)
List *users;
{
    char **args;
    int  l;

    if (users == NULL)
    {
	void showOutputStatus();
	showOutputStatus((List *)NULL);
	return;
    }
    args = (char **)malloc((2+len(users)) * sizeof(char *));
    if (args == NULL)
	fatal(MSGSTR(LPSTAT_2, "Out of memory"));
    args[0] = LPQ;
    l = 1;
    while (users)
    {
	args[l++] = users->name;
	users = users->next;
    }
    args[l] = NULL;
    if (sFork())
	wait(NULL);		/* Just let the output finish */
    else
	execv(LPQPATH, args);
}

void showSchedulerStatus()
{
    int lfd;
    extern int errno;
 
   (void) umask(0);
    lfd = open(MASTERLOCK, O_RDONLY);
    if (lfd < 0 || flock(lfd, LOCK_SH|LOCK_NB) == 0)
    {
	printf(MSGSTR(LPSTAT_4, "Scheduler is not running\n"));
	close(lfd);
	return;
    }
    printf(MSGSTR(LPSTAT_5, "Scheduler is running\n"));
    close(lfd);
}

void showDevices(lst)
List *lst;
{
    char *lp, *rm, *rp;

    if (lst == NULL)		/* No selectors == all printers */
    {
	while (getprent(line) > 0)
	{
	    char *cp = line;	/* grab printer's first name */
	    bp = pbuf;
	    while (*cp != '|' && *cp != ':' && *cp != '\0')
		*bp++ = *cp++;
	    *bp = '\0';
	    printf(MSGSTR(LPSTAT_6, "Output for printer %s is sent to "), pbuf);
	    bp = pbuf;
	    lp = pgetstr("lp", &bp);
	    rm = pgetstr("rm", &bp);
	    rp = pgetstr("rp", &bp);
	    if (rm && strlen(rm) != 0)
		printf(MSGSTR(LPSTAT_7, "remote printer %s on %s\n"), rp, rm);
	    else if (lp && strlen(lp) != 0)
		printf("%s\n", lp);
	    else
		printf(MSGSTR(LPSTAT_8, "NOWHERE. Error in printcap file\n"));
	}
        endprent();
    }
    else while (lst)
    {
	if (pgetent(line, lst->name) <= 0)
	{
	    fprintf(stderr, MSGSTR(LPSTAT_9, "unknown printer: %s\n"), lst->name);
            lst = lst->next;
	    continue;
        }
        bp = pbuf;
	lp = pgetstr("lp", &bp);
	rm = pgetstr("rm", &bp);
	rp = pgetstr("rp", &bp);
	printf(MSGSTR(LPSTAT_6, "Output for printer %s is sent to "), lst->name);
	if (rm && strlen(rm) != 0)
	    printf(MSGSTR(LPSTAT_7, "remote printer %s on %s\n"), rp, rm);
	else if (lp && strlen(lp) != 0)
	    printf("%s\n", lp);
	else
	    printf(MSGSTR(LPSTAT_8, "NOWHERE. Error in printcap file\n"));
	lst = lst->next;
    }
}

void showAcceptanceStatus(lst)
List *lst;
{
    if (lst == NULL)
	if (sFork())
	    wait(NULL);
	else
	    execl(LPCPATH, LPC, "status", NULL);
    else
    {
	int l = 2;		/* 0, 1 are taken by lpc+status */
	char **args = (char **)malloc((3+len(lst)) * sizeof(char *));
	if (args == NULL)
	    fatal(MSGSTR(LPSTAT_2, "Out of memory"));
	args[0] = LPC;
	args[1] = "status";
	while (lst)
	{
	    args[l++] = lst->name;
	    lst = lst->next;
	}
	args[l] = NULL;
	if (sFork())
	    wait(NULL);		/* Just let the output finish */
	else
	    execv(LPCPATH, args);
    }
}

void showOutputStatus(lst)
List *lst;
{
    if (lst == NULL)
    {				/* Get all printers sequentially lpq'ed */
	while (getprent(line) > 0)
	{
	    char *cp = line;
	    bp = pbuf;
	    while (*cp != '|' && *cp != ':' && *cp != '\0')
		*bp++ = *cp++;
	    *bp = '\0';
	    printf(MSGSTR(LPSTAT_10, "Requests on %s:\n"), pbuf);
            if (sFork())
		wait(NULL);
	    else
		execl(LPQPATH, LPQ, "-P", pbuf, NULL);
	    printf("\n");
	}
        endprent();
    }
    else while (lst)
    {
	if (lst->type == ID)
	{
	    printf(MSGSTR(LPSTAT_10, "Requests on %s:\n"), pbuf);
	    if (sFork())
		wait(NULL);
	    else
		execl(LPQPATH, LPQ, "-P", lst->name, NULL);
	}
	else
	    if (sFork())
		wait(NULL);
	    else
		execl(LPQPATH, LPQ, lst->name, NULL);
	lst = lst->next;
    }
}

void showPrinterStatus(lst)
List *lst;
{
    void showOne();
    if (lst == NULL)
    {
	while (getprent(line) > 0)
	{
	    char *cp = line;
	    bp = pbuf;
	    while (*cp != '|' && *cp != ':' && *cp != '\0')
		*bp++ = *cp++;
	    *bp = '\0';
	    printf("%s: \n", pbuf);
	    showOne();
	}
	endprent();
    }
    else while (lst)
    {
	if (pgetent(line, lst->name) <= 0)
	{
	    fprintf(stderr, MSGSTR(LPSTAT_9, "unknown printer: %s\n"), lst->name);
            lst = lst->next;
	    continue;
        }
	printf("%s: \n", lst->name);
	showOne();
	lst = lst->next;
    }
}
	

void showOne()
{
    struct stat stbuf;
    char *sd, *lo;

    bp = pbuf;
    if ((sd = pgetstr("sd", &bp)) == NULL)
	sd = DEFSPOOL;
    if ((lo = pgetstr("lo", &bp)) == NULL)
	lo = DEFLOCK;
    
    sprintf(line, "%s/%s", sd, lo);
    if (stat(line, &stbuf) >= 0)
    {
	printf(MSGSTR(LPSTAT_11, "Queuing is %s\n"),
	       (stbuf.st_mode & 010) ? MSGSTR(LPSTAT_12, "disabled") : MSGSTR(LPSTAT_13, "enabled"));
	printf(MSGSTR(LPSTAT_14, "Printing is %s\n"),
	       (stbuf.st_mode & 0100) ? MSGSTR(LPSTAT_12, "disabled") : MSGSTR(LPSTAT_13, "enabled"));
    }
    else
	fprintf(stderr, MSGSTR(LPSTAT_15, "Cannot stat %s\n"), line);
}


int len(lst)
List *lst;
{
    int i = 0;
    while (lst)
    {
	i++;
	lst = lst->next;
    }
    return i;
}


void fatal(s)
char *s;
{
    fputs(s, stderr);
    fputc('\n', stderr);
    exit(1);
}

#ifdef DEBUG

void printList(l)
List *l;
{
    List *old;

    while (l)
    {
	printf(MSGSTR(LPSTAT_16, "type %s\ttext %s\n"), 
	       l->type ? MSGSTR(LPSTAT_17, "ident") : MSGSTR(LPSTAT_18, "job"),
	       l->name);
	old = l;
	l = l->next;
	free(old);
    }
    printf("\n\n");
}

void showOptions()
{
    Option *ol = OptionList;
    Job *jl = JobList;

   if (printSchedulerStatus)
       printf(MSGSTR(LPSTAT_19, "SchedulerStatus\n"));
   if (printSystemDefault)
       printf(MSGSTR(LPSTAT_20, "SystemDefault\n"));
   if (printSummary)
       printf(MSGSTR(LPSTAT_21, "Summary\n"));
   if (printAll)
       printf(MSGSTR(LPSTAT_22, "All\n"));

   while (ol)
   {
       printf(MSGSTR(LPSTAT_23, "Optionchar = %c\n"), ol->optChar);
       printList(ol->selectors);
       ol = ol->next;
   }
   while (jl)
   {
       printf(MSGSTR(LPSTAT_24, "Job: %s\n"), jl->jobNumber);
       jl = jl->next;
   }
}
#endif				/* DEBUG */

int sFork()
{
   int ret = fork();
   if (ret == -1)
   {
       fatal(MSGSTR(LPSTAT_25, "Can't fork!"));
       exit(1);
   }
   return ret;
}

usage()
{
    fprintf(stderr, MSGSTR(LPSTAT_26, "lpstat [-aList] [-d] [-oList] [-pList] [-r] [-s] [-t]  [-uList] [-vList]\n"));
    exit(1);
}
