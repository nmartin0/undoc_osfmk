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
 * HISTORY
 * $Log: listr.c,v $
 * Revision 1.16  1995/02/17  17:57:12  kremenek
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 11361
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/listr.c
 *
 * Revision 1.15  1994/12/16  19:30:39  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 11754
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/listr.c
 *
 * Revision 1.14  1994/12/07  23:52:38  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 5398
 *  Testing: Developer testing
 *  Module(s): mds_libs/src/usr/ccs/lib/libnqs/listr.c
 *
 * Revision 1.13  1994/11/19  02:27:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.12  1994/10/19  20:47:06  kremenek
 *  Reviewer: davidl doyle
 *  Risk: low
 *  Benefit or PTS #: 11276
 *  Testing: EATS
 *  Module(s): listr.c
 *
 * Revision 1.10  1994/08/31  20:22:08  bradf
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.8.2.1  1994/08/05  20:43:12  kremenek
 *  Reviewer: George Kremenek SDSC
 *  Risk: Low
 *  Benefit or PTS #: 6589 9491 9950 10355
 *  Testing: EATS passed
 *  Module(s):
 *
 * Revision 1.9  1994/08/05  17:24:16  mwan
 *  Reviewer: George Kremenek SDSC
 *  Risk: Low
 *  Benefit or PTS #: 6589 9491  9950 10355
 *  Testing: EATS
 *  Module(s):
 *
 * Revision 1.7.2.1  1994/02/23  17:31:02  mwan
 *  Fixed PTS 7738 and 8087
 *
 *  Reviewer: kremenek
 *  Risk: L
 *  Benefit or PTS #: 7738 and 8087
 *  Testing:
 *  Module(s): usr/ccs/lib/libnqs/fetchpwdb.c
 *             usr/lib/nqs/macs_blk.c
 *             usr/lib/nqs/macs_rootp.c
 *             usr/bin/qmgr/qmgr.hlp
 * 	    usr/ccs/lib/libnqs/listq.c
 *             usr/ccs/lib/libnqs/listr.c
 *
 * Revision 1.5  1993/07/13  17:57:15  mwan
 * T11 - fixed PTS 5012
 *
 * Revision 1.3  1992/12/16  23:29:09  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  20:16:56  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 *
 */

/*
#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)listr.c	1.7 (listr.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif
*/

#include <stdio.h>
#include <ctype.h>
#include "nqs.h"		/* NQS definitions */
/* #include "netpacket.h"			NQS networking */
#include <pwd.h>			/* Password stuff */

#if	BSD42 | BSD43 | ULTRIX | UTS
#include <sys/file.h>			/* file stuff */
#else
#if	SYS52 | SGI | UNICOS | OSF 
#include <sys/fcntl.h>			/* file stuff */
#else
BAD SYSTEM TYPE
#endif
#endif

#if	UNICOS
#include "scpnqs.h"
#include <sys/param.h>
#include <sys/jtab.h>
#include <signal.h>
#include <sys/user.h>
#include <sys/sysmacros.h>
#else
#if	BSD42 | BSD43 | SGI | SYS52 | ULTRIX | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
#include "nqsxdirs.h"		/*  NQS directories */
#include "nqsxvars.h"		/* NQS directories */
#ifdef SDSC
#include "buddy.h"
#include <time.h>
#endif


/*
 *	External functions.
 */
extern int gethostname();               /* Get host name */
extern char *getmacname();               /* Get machine name */
extern char *destqueue();               /* Strip "@machine" */
extern struct passwd *fetchpwuid();	/* NQS version of getpwuid */
extern long filecopyentire();		/* File descr to file descr copy */
extern void interclear();		/* Zero out inter() structs */
extern void interw32i();		/* Write a 32 bit integer */
extern void interwstr();		/* Write a string */
extern int localmid();			/* Get our mid */
extern struct gendescr *nextdb();	/* Advance to next entry in db */
extern void nqssleep();			/* NQS version of sleep() */
extern int seekdb();			/* Seek on a db file */
extern char *strncpy();			/* Anchored string copy */
extern void analyzetcm();		/* Print message */
extern char *bufsockch();		/* Ptr to extra chars from socket */
extern int  daepres();		/* is local NQS daemon present & running */
extern char *destqueue();		/* Strip "@machine" */
extern int establish();			/* Set up connection */
extern int extrasockch();		/* Number of extra chars from socket */
extern char *fmttime();		/* Format time long integer to local time */
				/* string */
extern char *getgrpnam();	/* Get group name given gid */
extern char *getmacnam();	/* Get machine name from machine-id */
extern int getrreq();		/* Get a control file request header */
extern uid_t getuid();		/* Get real user-id */
extern char *getusenam();	/* Get user name for user-id */
extern long lseek();		/* Seek on a file */
extern struct gendescr *nextdb();	/* Return next allocated entry */
extern struct confd *opendb();	/* Open an NQS database file */
extern int openqacc();		/* Open a queue access file */
extern int openqord();		/* Open a queue ordering file */
extern void seekdbb();		/* Buffered seek on an NQS database file */
extern int strlen();		/* String length */
extern int strcmp();		/* String comparison */
extern int strncmp();		/* Anchored string comparison */
extern long telldb();		/* Return position of last read on an NQS */
				/* database file */
#if	UNICOS
extern int scpflag;		/* UNICOS called by SCP flag */
extern struct jtab jtab[];	/* Job table */
#else
#if	BSD42 | BSD43 | SGI | SYS52 | ULTRIX | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif

static int acccolumn = 0;	/* Used by shoqueacc(), shogrp(), shousr() */

#ifdef SDSC
int read_glb_param ();
void lget_time ();
int lfill_nodes ();
int init_tsched ();
int list_tsched ();
struct inp inp;
struct glb glb;
struct node_grp node_grp;
#endif


/*** listr
 *
 * Read the requests and
 * send to correct display
 * (short or long format).
 */
int listr (file, que_descr, flags, selector,req,daemon)
register struct confd *file;			/* NQS queue file */
register struct gendescr *que_descr;		/* the queue descriptor */
register long flags;				/* Display flags */
register char * selector;
register char * req;
register short daemon;
{

#ifdef SDSC
	struct qentry *cache;
#else
	struct qentry cache [QOFILE_CACHESIZ];
#endif
	struct rawreq	request;
	int cacheindex = 0;		/* Current buffer cache index */
	int nth_request;		/* Rank of this request */
	int fd;				/* Queue ordering file descriptor */
	int i;				/* Loop var */
	int csize;
	int count;
	int cfd;
	short daepresent;
#ifdef SDSC
	int iMaxCount = 0;
#endif

	short user =0;
	nth_request = 0;		/* No requests yet displayed */

#ifdef SDSC
	/* read the age factor from the sched_param file */

	read_glb_param ();
        iMaxCount = que_descr->v.que.departcount
                 + que_descr->v.que.queuedcount
                 + que_descr->v.que.runcount
                 + que_descr->v.que.stagecount
                 + que_descr->v.que.waitcount
                 + que_descr->v.que.holdcount
                 + que_descr->v.que.arrivecount;
	cache = (struct qentry *) malloc(sizeof(*cache)*iMaxCount);
	if (cache == NULL) {
		return(-1);
	}
#endif
 
	if (!daemon) {
		que_descr->v.que.queuedcount += 
			que_descr->v.que.runcount;
		que_descr->v.que.runcount = 0;
	}

	/* step 2. Open the request queue ordering file.  
	 *  fd >= 0 if the queue has requests, and we successfully
	 *	    opened the queue ordering file.
	 *  fd = -1 if the queue has no requests.
	 *  fd = -2 if the queue has requests, but an error prevented
	 *	    us from opening the queue ordering file for
	 *	    the queue.
	 *  fd = -3 if the queue was deleted.
	 */
	fd = openqord (file, que_descr);
	if (fd < -1) {
#ifdef SDSC
		(void)free((void*)cache);
#endif
		return (fd);	/* Error or queue was deleted */
	}

	/* step 3. Departing Requests display, if all or in transit is requested*/
	if  (flags & SHO_RS_EXIT )
	  if ( (count = que_descr->v.que.departcount) != 0)
	   for(i=0; i<count; i++) {
#ifdef SDSC
                csize = read(fd,(char *) cache, sizeof (*cache)*(iMaxCount));
#else
                csize = read(fd,(char *) cache, sizeof (cache));
#endif
                csize /= sizeof(struct qentry);
                if ((cfd = getrreq((long)cache[cacheindex].orig_seqno, cache
                        [cacheindex].orig_mid, &request)) != -1) {
				if (flags & SHO_FULL){
					full_listr(&request,que_descr,SHO_RS_EXIT,flags,
						selector,req);
				}
				else {
	                                dsp_req(que_descr,flags, 
						SHO_RS_EXIT,&request, selector,req);
				}
#ifdef SDSC
                                close(cfd);  /* important to avoid to many open files */
#endif
		}
                cacheindex++;
        }

	/* step 4. Running Requests display, if all or running is requested*/
	if (flags & SHO_RS_RUN )	
	  if ( (count = que_descr->v.que.runcount) != 0)
	   for(i=0; i<count; i++) {
#ifdef SDSC
                csize = read(fd,(char *) cache, sizeof (*cache)*(iMaxCount));
#else
                csize = read(fd,(char *) cache, sizeof (cache));
#endif
                csize /= sizeof(struct qentry);
                if ((cfd = getrreq((long)cache[cacheindex].orig_seqno, cache
                        [cacheindex].orig_mid, &request)) != -1) {
				if (flags & SHO_FULL){
					full_listr(&request,que_descr,SHO_RS_RUN,flags,
						selector,req);
				}
				else {
	                                dsp_req(que_descr,flags, 
						SHO_RS_RUN,&request, selector,req);
				}
#ifdef SDSC
                                close(cfd);  /* important to avoid to many open files */
#endif
		}
                cacheindex++;
        }

	/* step 5. Staged Requests display, if all or in transit is requested*/

	if (flags & SHO_RS_EXIT )
	  if ( (count = que_descr->v.que.stagecount) != 0)
	   for(i=0; i<count; i++) {
#ifdef SDSC
                csize = read(fd,(char *) cache, sizeof (*cache)*(iMaxCount));
#else
                csize = read(fd,(char *) cache, sizeof (cache));
#endif
                csize /= sizeof(struct qentry);
                if ((cfd = getrreq((long)cache[cacheindex].orig_seqno, cache
                        [cacheindex].orig_mid, &request)) != -1) {
				if (flags & SHO_FULL){
					full_listr(&request,que_descr,SHO_RS_STAGE,flags,
						selector,req);
				}
				else {
	                                dsp_req(que_descr,flags, 
						SHO_RS_STAGE,&request, selector,req);
				}
#ifdef SDSC
                                close(cfd);  /* important to avoid to many open files */
#endif
		}
                cacheindex++;
        }

	/* step 6. Queued Requests display, if all or queued is requested*/

	if (flags & SHO_RS_QUEUED )
	  if ( (count = que_descr->v.que.queuedcount) != 0)
	   for(i=0; i<count; i++) {
#ifdef SDSC
                csize = read(fd,(char *) cache, sizeof (*cache)*(iMaxCount));
#else
                csize = read(fd,(char *) cache, sizeof (cache));
#endif
                csize /= sizeof(struct qentry);
                if ((cfd = getrreq((long)cache[cacheindex].orig_seqno, cache[cacheindex].orig_mid, &request)) != -1) {
				if (flags & SHO_FULL){
					full_listr(&request,que_descr,SHO_RS_QUEUED,flags,
						selector,req);
				}
				else {
	                                dsp_req(que_descr,flags, 
						SHO_RS_QUEUED,&request, selector,req);
				}
#ifdef SDSC
                                close(cfd);  /* important to avoid to many open files */
#endif
		}
                cacheindex++;
        }

	/* step 7. Waiting Requests display, if all or held is requested*/

	if (flags & SHO_RS_HOLD )
	  if ( (count = que_descr->v.que.waitcount) != 0)
	   for(i=0; i<count; i++) {
#ifdef SDSC
                csize = read(fd,(char *) cache, sizeof (*cache)*(iMaxCount));
#else
                csize = read(fd,(char *) cache, sizeof (cache));
#endif
                csize /= sizeof(struct qentry);
                if ((cfd = getrreq((long)cache[cacheindex].orig_seqno, cache
                        [cacheindex].orig_mid, &request)) != -1) {
				if (flags & SHO_FULL){
					full_listr(&request,que_descr,SHO_RS_WAIT,flags,
						selector,req);
				}
				else {
	                                dsp_req(que_descr,flags, 
						SHO_RS_WAIT,&request, selector,req);
				}
#ifdef SDSC
                                close(cfd);  /* important to avoid to many open files */
#endif
		}
                cacheindex++;
        }
          
	/* step 8. Holding Requests display, if all or held is requested*/

	if (flags & SHO_RS_HOLD )
	  if ( (count = que_descr->v.que.holdcount) != 0)
	   for(i=0; i<count; i++) {
#ifdef SDSC
                csize = read(fd,(char *) cache, sizeof (*cache)*(iMaxCount));
#else
                csize = read(fd,(char *) cache, sizeof (cache));
#endif
                csize /= sizeof(struct qentry);
                if ((cfd = getrreq((long)cache[cacheindex].orig_seqno, cache
                        [cacheindex].orig_mid, &request)) != -1) {
				if (flags & SHO_FULL){
					full_listr(&request,que_descr,SHO_RS_HOLD,flags,
						selector,req);
				}
				else {
	                                dsp_req(que_descr,flags, 
						SHO_RS_HOLD,&request, selector,req);
				}
#ifdef SDSC
                                close(cfd);  /* important to avoid to many open files */
#endif
		}
                cacheindex++;
        }
          
	/* step 9. Arriving Requests display, if all or in transit is requested*/

	if (flags & SHO_RS_EXIT )
	  if ( (count = que_descr->v.que.arrivecount) != 0)
	   for(i=0; i<count; i++) {
#ifdef SDSC
                csize = read(fd,(char *) cache, sizeof (*cache)*(iMaxCount));
#else
                csize = read(fd,(char *) cache, sizeof (cache));
#endif
		csize /= sizeof(struct qentry);
                if ((cfd = getrreq((long)cache[cacheindex].orig_seqno, cache
                        [cacheindex].orig_mid, &request)) != -1) {
				if (flags & SHO_FULL){
					full_listr(&request,que_descr,SHO_RS_ARRIVE,flags,
						selector,req);
				}
				else {
	                                dsp_req(que_descr,flags, 
						SHO_RS_ARRIVE,&request, selector,req);
				}
#ifdef SDSC
                                close(cfd);  /* important to avoid to many open files */
#endif
		}
                cacheindex++;
        }
          
	/* step 10. close queue ording file and return */
	close (fd);			
#ifdef SDSC
	(void)free((void*)cache);
#endif

	return (0);			
}

/*** get_rstate
 *
 * get state of request by queue typed
 * and state
 */
static char *get_rstate(type, state)
short   type;
long    state;
{


 /* determine the state of the request 
  * according to queue type and request 
  */

 switch (state) {
         case SHO_RS_EXIT:
                if (type == QUE_BATCH) {
                        return(  "EXITING");
                }
                else if (type == QUE_PIPE) {
                        return("DEPARTING");
                }
                else return("UNKNOWN");
                break;
        case SHO_RS_RUN:
                if (type == QUE_PIPE)
                        return("ROUTING");
                else
                        return("RUNNING");
                break;
        case SHO_RS_STAGE:
                if (type == QUE_BATCH) {
                return("STAGING");
                }
                else return("UNKNOWN");
                break;
        case SHO_RS_QUEUED:
                return("QUEUED");
                break;
        case SHO_RS_WAIT:
                return("WAITING");
                break;
        case SHO_RS_HOLD:
                return("HOLDING");
                break;
        case SHO_RS_ARRIVE:
                return("ARRIVING");
                break;
        default:
                return("UNKNOWN");     /* We should never get here */
                break;
                }

}
 
/*** dsp_req
 *
 * display request, short form
 */
dsp_req(queue,flags,state,req,selector,def_req)
	struct gendescr *queue;		/* the queue descriptor */
	long	flags;
        long    state;
        struct  rawreq  *req;
	char 	* selector;
	char 	* def_req;
{
	char 	mac[11];
        int     cnt;
        char    *mach;
        char    status[8];
	long	val;
	char 	*machinename;
#ifdef SDSC
	float   req_pri;
        char    sFormat [80];
#endif

	/* if not all validate whom to see */
	if (!(flags & SHO_R_ALLUID)) {
		if (def_req != NULL) {
			val=0;
			while (*def_req >= '0' && *def_req <='9'){
				val *=10;
				val += *def_req -'0';
				def_req++;
			}
			if (val !=req->orig_seqno) 
				return(0);
			
			if (val_user(selector,req->username)== -1)
			{
				printf("invalid administrator at host or did not use administrator option -U or -u\n");
				return(0);
			}
				/* if (adm) */
		}
		else {
			if (val_user(selector,req->username)== -1)
				return (0);
			}
	}


	/* get originating machine */
	machinename=getmacnam(req->orig_mid);
	strncpy(mac,machinename,8);
	mac[8]='\0';


#ifdef SDSC
        if (state == SHO_RS_QUEUED)
	    req_pri = (float) queue->v.que.priority +
            ((float) (time (0) - req->enter_time) * AGE_FACTOR / 3600.0);
	else
	    req_pri = (float) queue->v.que.priority;
        strcpy(sFormat, "%5d.%-7.7s  %-7.7s %-10.10s %-10.10s %-5.1f  %-3d");
        if (strlen(req->quename) > 10) {
            strcpy(sFormat, "%5d.%-7.7s  %-7.7s %-10.10s %-15.15s %-5.1f  %-3d");
        }
        printf(sFormat,
#else
        printf("%5d.%-7.7s  %-7.7s  %-7.7s  %-7.7s  %-3d   %-3d",  /* CERN Boissat */
#endif
	req->orig_seqno,		/* print request id */
	mac,				/* print originating machine */
        req->reqname,			/* print request name */
        req->username,			/* print owner */
        req->quename,			/* print queue */
#ifdef SDSC
	req_pri,
#else
        req->rpriority == (-1) ? /* CERN Boissat queue->v.que.priority, print priority */
		MAX_RPRIORITY/2 : req->rpriority,	/* See ../src/nqs_psc.c */
#endif

#ifdef SDSC
        req->v.bat.ppnice);		/* print nice value */
	/* PTS 5398 fix for SSD */
#else
        req->v.bat.ppnice +20);         /* print nice value */
#endif

#ifdef SDSC
	/* get cpu seconds */
	if ((req->v.bat.infinite & LIM_PRCPUT) == 0)
		printf("%-7lu",req->v.bat.prcputime.max_seconds);
	else
	if ((req->v.bat.explicit & LIM_PRCPUT)
		|| (queue->v.que.type== QUE_BATCH)) 
		printf("UNLIM. ");
	else
		printf("UNSPEC.");
#else
	/* get cpu seconds */
	if ((req->v.bat.infinite & LIM_PPCPUT) == 0)
		printf("  %-7lu",req->v.bat.ppcputime.max_seconds);
	else
	if ((req->v.bat.explicit & LIM_PPCPUT)
		|| (queue->v.que.type== QUE_BATCH)) 
		printf("  UNLIM. ");
	else
		printf("  UNSPEC.");
#endif

#ifdef SDSC
	/* get memory */
	if ((req->v.bat.infinite & LIM_PPMEM) == 0)
		printf(" %-7lu",req->v.bat.ppmemsize.max_quota);
	else
	if ((req->v.bat.explicit & LIM_PPMEM)
		|| (queue->v.que.type== QUE_BATCH)) 
		printf(" UNLIM. ");
	else
		printf(" UNSPEC.");
#else
	/* get memory */
	if ((req->v.bat.infinite & LIM_PPMEM) == 0)
		printf("  %-7lu",req->v.bat.ppmemsize.max_quota);
	else
	if ((req->v.bat.explicit & LIM_PPMEM)
		|| (queue->v.que.type== QUE_BATCH)) 
		printf("  UNLIM. ");
	else
		printf("  UNSPEC.");
#endif
	/* get state of request */
#ifdef SDSC
	if (req->start_time > 0 &&
        queue->v.que.priority >= TSCHED_PRI) {
            if (state == SHO_RS_WAIT) {
		printf(" TWAITING\n");
            } else if (state == SHO_RS_RUN) {
		printf(" TRUNNING\n");
	    } else {
              if (flags & SHO_VERBOSE) {
                printf(" %-s",get_rstate(queue->v.que.type, state));
                printf(" \t%3d",req->v.bat.prncpus);
                printf(" \t%s\n",fmttime(&req->create_time)); /* enter_time */
              }
              else {
                printf(" %-s\n",get_rstate(queue->v.que.type, state));
              }
	    }
	} else {
           if (flags & SHO_VERBOSE) {
              printf(" %-s",get_rstate(queue->v.que.type, state));
              printf(" \t%3d",req->v.bat.prncpus);
              printf(" \t%s\n",fmttime(&req->enter_time));
           }
           else {
              printf(" %-s\n",get_rstate(queue->v.que.type, state));
           }
	}
#else
	printf("   %-s\n",get_rstate(queue->v.que.type, state));
#endif

} /*  end of request */

 
qcmplx_hdr()
{
char hostname[256];

	gethostname(hostname,255);
	hostname[255] = '\0';

        printf("=========================================\n");
        printf(" COMPLEXES for %s\n",hostname);
        printf("=========================================\n");
}                                              



/*** p_req_hdr
 *
 * print header for request 
 */
p_req_hdr(flags)
long flags;
{
char hostname[256];
	printf("===============================================================\n");
	printf("NQS Version:%d	",VERSION);

        if(flags &  SHO_BATCH)
                printf(" BATCH ");

#ifndef SDSC
        if(flags &  SHO_DEVICE)
                printf(" DEVICE ");
#endif

        if(flags &  SHO_PIPE)
                printf(" PIPE ");

        printf("REQUESTS on ");
	gethostname(hostname,255);
	hostname[255] = '\0';
	printf("%s \n",hostname);
	

	printf("===============================================================\n");
#ifdef SDSC
        if (flags & SHO_VERBOSE) {
           printf(" REQUEST       NAME    OWNER      QUEUE      PRI  NICE CPU     MEM     STATE    NODES   SUBMITTED AT\n");
/*                 12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456
7890123456789012 */
          /* verbose mode 132 characters per line */
        }
        else {
          printf(" REQUEST       NAME    OWNER      QUEUE      PRI  NICE CPU     MEM     STATE\n");
        }
#else
	printf("REQUEST      NAME   OWNER   QUEUE    PRI   NICE  CPU      MEM   STATE\n");
#endif
}




/*** val_user 
 * Determine if selector, the name we are looking for is
 * equal to the name we have 
 */
val_user(selector,name)
char * selector; 
char * name;
{

	if (selector != NULL)	/* This is to allow networking of old NQS */
		if (strcmp(name,selector) == 0)
			return(0);
		else
			return(-1);
}


/*** list
 *
 *
 *	int list():
 *
 *	Print on stdout information about the named object.
 *	Use this to determine if the object is queue, complex, request
 *	or a mistake.
 *	Returns: 0 if output was produced.
 *		-1 if no output was produced.
 */
int list (file, qcomplexfile,name, flags, selector ,qmapfile,pipeqfile)
struct confd *file;			/* NQS queue file */
register struct confd *qcomplexfile;	/*NQS queue complex definition file */
char *name;				/* Name with possible '@' */
long flags;				/* SHO_??? */
char * selector;				/* Name with possible '@' */
register struct confd *qmapfile;	/*NQS qmap definition file */
register struct confd *pipeqfile;	/*NQS qmap definition file */
{

	int whomuid;
	int itsmid;
	short daepresent;
	char	*localname;
#ifdef SDSC
	static int sho_reqhdr = 0;  /* show req hdr already ? 0->no, 1->yes */
	static int sho_qstathdr = 0;  /* show qstat hdr already ? */
#endif

	/*   step 1. Override database if daemon not present.  */
#if 	BSD42 | BSD43 | ULTRIX /* | OSF */
	daepresent = daepres(file);
#else
#if     SGI | SYS52 | UNICOS | UTS | OSF
        daepresent = daepres();
#else    
BAD SYSTEM TYPE
#endif
#endif          

#ifdef SDSC
	/* case 0:
	 *	display tennis court time slot.
	 *
	 */

	if (flags & SHO_TSCHED) {
	    if (name == NULL) {
		printf("A queue-name must be specified with the -t option\n");
		return (-1);
	    }
	    if (list_tsched (file,name,NULL,flags,selector,daepresent,
	    qmapfile,pipeqfile,qcomplexfile) == SUCCESS)
		return(0);
	    else
		return (-1);
	}
#endif

	/* case 1:
	 *	display all queues as no list 
	 *	was sent (name ) and then exit
	 */

	if ((name == NULL) && ! (flags & SHO_CMPLX)){
#ifdef SDSC
		if ((flags & SHO_QUE) && ! (flags & SHO_NOHDR) && 
		! (flags & SHO_FULL) && (sho_qstathdr == 0)) {
			qstat_hdr(flags);
			sho_qstathdr = 1;
		}

		if ((flags & SHO_REQ) && ! (flags & SHO_NOHDR) && 
		! (flags & SHO_FULL) && (sho_reqhdr == 0)) {
			p_req_hdr(flags);
			sho_reqhdr = 1;
		}
#else
		if ((flags & SHO_QUE) && ! (flags & SHO_NOHDR) && ! (flags & SHO_FULL))
			qstat_hdr(flags);

		if ((flags & SHO_REQ) && ! (flags & SHO_NOHDR) && ! (flags & SHO_FULL))
			p_req_hdr(flags);
#endif
		
		if (search_q(file,name,NULL,flags,selector,daepresent,
				qmapfile,pipeqfile,qcomplexfile) == SUCCESS)
			return(0);
	}

	/* case 2: 
	 *	check to see if the queue is on a remote machine
	 *	if the queue is remote then send it on and return
	 */

	if ((name != NULL))
	  if (check_rmt(name, &itsmid) == SUCCESS) {
		localname = destqueue(name);
		whomuid = getuid();
		if (whomuid == 0)
			printf("Root is not allowed to access other nqs machines via the network for security.\n");
		if (flags & SHO_CMPLX) flags |= SHO_QUE;
		call_host(whomuid,flags,itsmid,localname);
		return(0);
	  }

	/* case 3:
	 *	check to see if name starts with a number
 	 *	if it does then this is a request, else check
	 *	queue
	 */
	if (flags & SHO_REQ ) {
#ifdef SDSC
		if ( ! (flags & SHO_NOHDR) && ! (flags & SHO_FULL)
		&& (sho_reqhdr == 0)) {
			p_req_hdr(flags);
			sho_reqhdr = 1;
		}
#else
		if ( ! (flags & SHO_NOHDR) && ! (flags & SHO_FULL))
			p_req_hdr(flags);
#endif

		if (search_req(file,name,flags,selector,daepresent,qmapfile,
				pipeqfile,qcomplexfile) == SUCCESS)
			return(0);
	}



	/* case 4:
	 *	the list was not remote or a complex, thus check
	 *	for a specific queue if so the queue will be 
	 *	displayed through search_q and then return
	 */

#ifdef SDSC
	if ((flags & SHO_QUE) && (name != NULL) && !(flags & SHO_CMPLX)) {
		if ( ! (flags & SHO_NOHDR) && ! (flags & SHO_FULL)
		&& (sho_qstathdr == 0)) {
			qstat_hdr(flags);
			sho_qstathdr = 1;
		}
	}
#else
	if ((flags & SHO_QUE) && (name != NULL) && !(flags & SHO_CMPLX)) {
		if ( ! (flags & SHO_NOHDR) && ! (flags & SHO_FULL))
			qstat_hdr(flags);
	}
#endif

	if (! (flags & SHO_CMPLX))
	if (search_q(file,name,NULL,flags,selector,daepresent,
			qmapfile,pipeqfile,qcomplexfile) == SUCCESS)
		return(0);

	/* case 4:
	 *	the list was not for a remote machine, 
	 *	thus check to see if the list is a complex
	 *	if it is then the queues in the complex will
	 *	be displayed through search_cmplx() and then return
	 */
	if (search_cmplx(file,qcomplexfile,name,flags,selector,
				daepresent,qmapfile,pipeqfile) == SUCCESS)
		return(0);

	/* case 6:
	 *	was not request, remote, complex or queue
	 *	thus print error message and return
	 */

	printf("%s was not found in the complex or queue list\n");
	return (-1);
	
}

/*** search_cmplx
 * 
 * determine if the object is a complex
 * if it is call search q for each queue in the complex
 */
search_cmplx(file,qcomplexfile,name,flags,selector,daepresent,qmapfile,pipeqfile)
register struct confd *file;            /* MUST be the NQS queue file */
register struct confd *qcomplexfile;	/*NQS queue complex definition file */
char * name;				/* search value  */
register long flags;                    /* Display flags */
register char * selector;               /* account, group, or user list */
register short daepresent;
register struct confd *qmapfile;	/*NQS qmap definition file */
register struct confd *pipeqfile;	/*NQS qmap definition file */
{

	register struct gendescr *descr;
	int i;
#ifdef SDSC
	static sho_hdr = 0;	/* display deader already ? 0 -> no. 1 -> yes */
#endif

	/* display header if this is for show complexes */
	if (flags & SHO_CMPLX)
#ifdef SDSC
	if (! (flags & SHO_NOHDR) && sho_hdr == 0) {
                qcmplx_hdr(flags);
		sho_hdr = 1;
	}
#else
	 if (! (flags & SHO_NOHDR) )
                qcmplx_hdr(flags);
#endif

	/* read first record */
	descr = nextdb(qcomplexfile);	
	if (descr == NULL)
		if ( ! (flags & SHO_QUE ) && ! (flags & SHO_REQ))
			printf("\t\t No complexes on host. \n");

	while (descr != NULL){
#ifdef SDSC
	    if (name != NULL)   {
		if (strcmp(descr->v.qcom.name,name) != 0) {
			descr = nextdb(qcomplexfile);
			continue;
		}
	    }
	    if  (flags & SHO_CMPLX )  { 
		printf("COMPLEX: %s    run-limit = %d\n", 
		descr->v.qcom.name, descr->v.qcom.runlimit);
		if (flags & SHO_BATCH) {
 		    	printf("QUEUE NAME      STATUS   TOTAL   RUNNING  ");
			printf ("QUEUED   HELD TRANSITION\n");
			for(i=0; i< MAX_QSPERCOMPLX; i++) {
				search_q(file,descr->v.qcom.queues[i]
				,NULL,flags,selector,daepresent,qmapfile,
				pipeqfile,qcomplexfile);
			}
		}
	    }
	    if (name != NULL)
		return (0);
#else
	  if (name != NULL)   {
		if (strcmp(descr->v.qcom.name,name)==0) {
			if  (flags & SHO_CMPLX )  { 
				printf("QUEUES FOR COMPLEX: %s with run-limit = %d :\n", descr->v.qcom.name, descr->v.qcom.runlimit);
 				printf("QUEUE NAME      STATUS   TOTAL   RUNNING  QUEUED   HELD TRANSITION\n");
			}

			for(i=0; i< MAX_QSPERCOMPLX; i++) {
					search_q(file,descr->v.qcom.queues[i]
						,NULL,flags,selector,daepresent,qmapfile,pipeqfile,qcomplexfile);
			}
			return(0);
		}
	  }
	  else 		/* name == NULL, object is not queue or request so print 
			   complex name */
		if (flags & SHO_CMPLX)
			printf("\t %s \n",descr->v.qcom.name);
#endif
	descr = nextdb(qcomplexfile);
		
	}
#ifdef SDSC
	if (descr == NULL && name != NULL) 
	    printf ("\t complex: %s not found.\n", name);
#endif

	if (flags & SHO_CMPLX)
		return (0);

	return(-1);

}	/* end of search_cmplx */



/*** get_cmplx
 * 
 * get complex match
 */
get_cmplx(name,qcomplexfile)
char * name;
register struct confd *qcomplexfile;	/*NQS queue complex definition file */
{
	register struct gendescr *descr;
	int i;


	seekdbb(qcomplexfile,0L);
	printf("\t");
	/* read first record */
	descr = nextdb(qcomplexfile);	

	while (descr != NULL) {
		for(i=0; i< MAX_QSPERCOMPLX; i++) {
			if (descr->v.qcom.queues[i] != NULL)
				if (strcmp(descr->v.qcom.queues[i],name)==0) 
					printf("%s ", descr->v.qcom.name);
		}
	descr = nextdb(qcomplexfile);
	}
	printf("\n");
}


/*** search_q
 * 
 * determine if object is a queue
 * and act accordingly
 */
search_q(file,name,req,flags,selector,daepresent,qmapfile,pipeqfile,qcomplexfile)
register struct confd *file;            /* the NQS queue file */
char * name;				/* search value  */
char * req;				/* request value  */
register long flags;                    /* Display flags */
register char * selector;               /* account, group, or user list */
register short daepresent;
register struct confd *qmapfile;            /* the NQS qmap file */
register struct confd *pipeqfile;            /* the NQS pipe file */
struct confd *qcomplexfile;	/*NQS queue complex definition file */
{

	register struct gendescr *descr;

	seekdbb(file,0L);
	descr = nextdb(file);

	while (descr != NULL) {
		if (name != NULL) {	/* print only specific queue */
		        if (strcmp(descr->v.que.namev.name,name)== 0) {
				if (flags & SHO_QUE) 
						dsp_q(file,descr,flags,
						daepresent,qmapfile,
						pipeqfile,qcomplexfile);
	
				else
					if (flags & SHO_REQ)
						listr(file, descr, flags, 
						    selector,NULL,daepresent);
				return(0);
			}
			descr = nextdb (file);
		}
		else {	/* going to print all queues */
			if (flags & SHO_QUE)  {
					dsp_q(file,descr,flags, daepresent,
						qmapfile,pipeqfile,qcomplexfile);
			}
			else{
				if (flags & SHO_REQ)
					listr(file, descr, flags, selector,
						req,daepresent);
			}
			descr = nextdb (file);
		}
	}
#ifdef SDSC
	if (name != NULL && descr == NULL && ! (flags & SHO_CMPLX)) {
		printf ("\t queue: %s not found.\n", name);
	}
#endif
	return(0);
}	/* end of search_q */

/*** shomd
 *
 *
 *      void shomd():
 *      Display an output mode.
 */
static void shomd (mode)
char mode;
{
        if (mode & OMD_SPOOL) {
                printf ("SPOOL");
        }
        else if (mode & OMD_EO) {
                printf ("EO");
        }
        else printf ("NOSPOOL");
}


/*** gunits
 *
 *
 *      char *gunits():
 *      Get quota units.
 */  
static char *gunits (units)
register short units;
{
        switch (units) {
        case QLM_BYTES:
                return ("bytes");
        case QLM_WORDS:
                return ("words");
        case QLM_KBYTES:
                return ("kilobytes");
        case QLM_KWORDS:
                return ("kilowords");
        case QLM_MBYTES:
                return ("megabytes");
        case QLM_MWORDS:
                return ("megawords");
        case QLM_GBYTES:
                return ("gigabytes");
        case QLM_GWORDS:
                return ("gigawords");
        }
        return ("");            /* Unknown units! */
 
}     
 

/*** shoquol
 *
 *
 *      void shoquol():
 *      Display a quota limit.
 */
static void shoquol (quota, explicit, infinite, que_descr)
register struct quotalimit *quota;
long explicit;
long infinite;
struct gendescr *que_descr;
{
        if (infinite == 0) {
                printf ("= [%1lu %s, %1lu %s]", quota->max_quota,
                        gunits (quota->max_units), quota->warn_quota,
                        gunits (quota->warn_units));
                if (explicit == 0) printf ("\t<DEFAULT>");
        }
        else {
                /*
                 *  The limit is infinite, or is not specified.
                 */
                if (que_descr->v.que.type == QUE_BATCH || explicit) {
                        printf ("= UNLIMITED ");
                        if (explicit == 0) printf ("\t<DEFAULT>");
                }
                /* Infinite, default limits are meaningless in a pipe queue */
                else printf ("= UNSPECIFIED");
        }
        putchar ('\n');
}  
 
 
/*** shoquetime
 *
 *
 *      void shoquetime():
 *      Display the cumulative cpu time used by requests in a queue.
 */
static void shoquetime (que_descr)
struct gendescr *que_descr;
{
        printf ("  Cumulative system space time = ");
#if     SGI | SYS52 | UNICOS | UTS | OSF

#if !CPU_MEAS_GRAN		/* temporary GJK for OSF */
#define CPU_MEAS_GRAN 60
#endif

        printf ("%ld.%02d seconds\n", que_descr->v.que.ru_stime / CPU_MEAS_GRAN,                ((que_descr->v.que.ru_stime % CPU_MEAS_GRAN) * 100) /
                CPU_MEAS_GRAN);
#else
#if     BSD42 | BSD43 | ULTRIX
        printf ("%ld.%06ld seconds\n", que_descr->v.que.ru_stime,
                que_descr->v.que.ru_stime_usec);
#else
BSD SYSTEM TYPE
#endif
#endif
        printf ("  Cumulative user space time = ");
#if     SGI | SYS52 | UNICOS | UTS | OSF
        printf ("%ld.%02d seconds\n", que_descr->v.que.ru_utime / CPU_MEAS_GRAN,                ((que_descr->v.que.ru_utime % CPU_MEAS_GRAN) * 100) /
                CPU_MEAS_GRAN);
#else
#if     BSD42 | BSD43 | ULTRIX
        printf ("%ld.%06ld seconds\n", que_descr->v.que.ru_utime,
                que_descr->v.que.ru_utime_usec);
#else
BSD SYSTEM TYPE
#endif
#endif
}
 
/*** shocpul
 *
 *
 *      void shocpul():
 *      Display a CPU time limit quota.
 */
static void shocpul (cpu, explicit, infinite, que_descr)
register struct cpulimit *cpu;
long explicit;
long infinite;
struct gendescr *que_descr;
{
        if (infinite == 0) {
                printf ("= [%1lu.%1d, %1lu.%1d]",
                         cpu->max_seconds, cpu->max_ms,
                         cpu->warn_seconds, cpu->warn_ms);
                if (explicit == 0) printf ("\t\t<DEFAULT>");
        }
        else {
                /*
                 *  The limit is infinite, or is not specified.
                 */
                if (que_descr->v.que.type == QUE_BATCH || explicit) {
                        printf ("= UNLIMITED ");
                        if (explicit == 0) printf ("\t\t<DEFAULT>");
                }
                else printf ("= UNSPECIFIED");
        }
        putchar ('\n');
}





/*** shoreql
 *
 *
 *      void shoreql():
 *      Display the limits associated with a request.
 */
static void shoreql (que_descr, rawreqp)
struct gendescr *que_descr;             /* Needed to detect pipe queues */
struct rawreq *rawreqp;                 /* The request as a rawreq */
{
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PPCPUT)) {
                printf ("  Per-proc. CPU time limit      ");
                shocpul (&rawreqp->v.bat.ppcputime,
                           rawreqp->v.bat.explicit & LIM_PPCPUT,
                           rawreqp->v.bat.infinite & LIM_PPCPUT,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PRCPUT)) {
                printf ("  Per-req. CPU time limit       ");

                shocpul (&rawreqp->v.bat.prcputime,
                           rawreqp->v.bat.explicit & LIM_PRCPUT,
                           rawreqp->v.bat.infinite & LIM_PRCPUT,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PRDRIVES)) {
                printf ("  Per-req. tape drives limit    ");
                if ((rawreqp->v.bat.infinite & LIM_PRDRIVES) == 0) {
                        printf (" = %1d", rawreqp->v.bat.prdrives);
                        if (!(rawreqp->v.bat.explicit & LIM_PRDRIVES)) {
                                printf (" \t<DEFAULT>");
                        }
                }
                else printf (" = UNSPECIFIED");
                putchar ('\n');
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PPCORE)) {
                printf ("  Per-proc. core file size limit");
                shoquol (&rawreqp->v.bat.ppcoresize,
                           rawreqp->v.bat.explicit & LIM_PPCORE,
                           rawreqp->v.bat.infinite & LIM_PPCORE,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PPDATA)) {
                printf ("  Per-proc. data size limit     ");
                shoquol (&rawreqp->v.bat.ppdatasize,
                           rawreqp->v.bat.explicit & LIM_PPDATA,
                           rawreqp->v.bat.infinite & LIM_PPDATA,
                           que_descr);

        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PPPFILE)) {
                printf ("  Per-proc. perm file ");
                printf ("size limit");
                shoquol (&rawreqp->v.bat.pppfilesize,
                           rawreqp->v.bat.explicit & LIM_PPPFILE,
                           rawreqp->v.bat.infinite & LIM_PPPFILE,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PRPFILE)) {
                printf ("  Per-req. perm file ");
                printf ("space limit");
                shoquol (&rawreqp->v.bat.prpfilespace,
                           rawreqp->v.bat.explicit & LIM_PRPFILE,
                           rawreqp->v.bat.infinite & LIM_PRPFILE,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PPMEM)) {
                printf ("  Per-proc. memory size limit   ");
                shoquol (&rawreqp->v.bat.ppmemsize,
                           rawreqp->v.bat.explicit & LIM_PPMEM,
                           rawreqp->v.bat.infinite & LIM_PPMEM,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PRMEM)) {
                printf ("  Per-req. memory size limit   ");
                shoquol (&rawreqp->v.bat.prmemsize,
                           rawreqp->v.bat.explicit & LIM_PRMEM,
                           rawreqp->v.bat.infinite & LIM_PRMEM,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PRNCPUS)) {
                printf ("  Per-req. # of cpus limit");
                if ((rawreqp->v.bat.infinite & LIM_PRNCPUS) == 0) {
                        printf (" = %1d", rawreqp->v.bat.prncpus);
                        if (!(rawreqp->v.bat.explicit & LIM_PRNCPUS)) {
                                printf (" \t<DEFAULT>");
                        }
                }
                else printf (" = UNSPECIFIED");
                putchar ('\n');
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PPQFILE)) {
                printf ("  Per-proc. quick file ");
                printf ("size limit");
                shoquol (&rawreqp->v.bat.ppqfilesize,
                           rawreqp->v.bat.explicit & LIM_PPQFILE,
                           rawreqp->v.bat.infinite & LIM_PPQFILE,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PRQFILE)) {
                printf ("  Per-req. quick file space limit");
                shoquol (&rawreqp->v.bat.prqfilespace,
                           rawreqp->v.bat.explicit & LIM_PRQFILE,
                           rawreqp->v.bat.infinite & LIM_PRQFILE,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PPSTACK)) {
                printf ("  Per-proc. stack size limit    ");
                shoquol (&rawreqp->v.bat.ppstacksize,
                           rawreqp->v.bat.explicit & LIM_PPSTACK,
                           rawreqp->v.bat.infinite      & LIM_PPSTACK,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PPTFILE)) {
                printf ("  Per-proc. temp file size limit");
                shoquol (&rawreqp->v.bat.pptfilesize,
                           rawreqp->v.bat.explicit & LIM_PPTFILE,
                           rawreqp->v.bat.infinite & LIM_PPTFILE,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PRTFILE)) {
                printf ("  Per-req. temp file space limit");
                shoquol (&rawreqp->v.bat.prtfilespace,
                           rawreqp->v.bat.explicit & LIM_PRTFILE,
                           rawreqp->v.bat.infinite & LIM_PRTFILE,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PPWORK)) {
                printf ("  Per-proc. working set limit   ");
                shoquol (&rawreqp->v.bat.ppworkset,
                           rawreqp->v.bat.explicit & LIM_PPWORK,
                           rawreqp->v.bat.infinite & LIM_PPWORK,
                           que_descr);
        }
        if (que_descr->v.que.type == QUE_PIPE ||
           (VALID_LIMITS & LIM_PPNICE)) {
                printf ("  Per-proc. execution nice pri.");
                if ((rawreqp->v.bat.infinite & LIM_PPNICE) == 0) {
#ifdef SDSC
                        printf (" = %1d", rawreqp->v.bat.ppnice);
			/* PTS 5398 fix for SSD */
#else
                        printf (" = %1d", rawreqp->v.bat.ppnice +20);
#endif
                        if (!(rawreqp->v.bat.explicit & LIM_PPNICE)) {
                                printf (" \t\t\t\t<DEFAULT>");
                        }
                }
                else printf (" = UNSPECIFIED");

                putchar ('\n');
        }
}


/*** search_req
 * 
 * determine if the object is a request 
 */

search_req(file,name,flags,selector,daepresent,qmapfile,pipeqfile,qcomplexfile) 
register struct confd *file;            /* the NQS queue file */
char * name;				/* search value  */
register long flags;                    /* Display flags */
register char * selector;               /* account, group, or user list */
register short daepresent;
register struct confd *qmapfile;            /* the NQS qmap file */
register struct confd *pipeqfile;            /* the NQS pipeto file */
struct confd *qcomplexfile;	/*NQS queue complex definition file */
{

	/* step 1. 
	 * 	see if name starts with a letter, if
	 *	so then assume it must be a queue 
	 */
	if (isdigit (*name)) { 
		search_q(file,NULL,name,flags,selector,daepresent,qmapfile,pipeqfile,qcomplexfile); 
	}
	else
		return(-1);
}


/*** full_listr
 *
 * list request in full format
 */
full_listr(req,queue,state,flags,selector,def_req)
struct  rawreq  *req;
register struct gendescr *queue;		/* the queue descriptor */
long  state;
long  flags;
char * selector;
char * def_req;
{

char * machinename = NULL;
char hostname[256];
char type[8];
long val;

	/* if not all validate whom to see */
	if (!(flags & SHO_R_ALLUID)) {
		if (def_req != NULL) {
			val=0;
			while (*def_req >= '0' && *def_req <='9'){
				val *=10;
				val += *def_req -'0';
				def_req++;
			}
			if (val !=req->orig_seqno) 
				return(0);
			
			if (val_user(selector,req->username)== -1)
			{
				printf("invalid administrator at host or did not use administrator option -U or -u\n");
				return(0);
			}
				/* if (adm) */
		}
		else {
			if (val_user(selector,req->username)== -1)
				return (0);
			}
	}

	/* step 2. get host we are on */
	gethostname(hostname,255);
	hostname[255] = '\0';

	/* step 3. get originating host */
	machinename=getmacnam(req->orig_mid);

	/* step 4. determine type of queue */
	if (queue->v.que.type == QUE_BATCH)
		strcpy(type,"BATCH");
	if (queue->v.que.type == QUE_PIPE)
		strcpy(type,"PIPE");
	if (queue->v.que.type == QUE_DEVICE)
		strcpy(type,"DEVICE");

	/* step 5. print header information */
	printf("=====================================================\n");
	printf("NQS version:%d	%s	REQUEST:	%d.%s\n",VERSION,type,
		req->orig_seqno,machinename);
	printf("=====================================================\n");

	/* step 6. print name ect */
	printf("\tName:  %s \t\t\t Owner: %s\n",req->reqname,req->username);
#ifdef SDSC
        if (state == SHO_RS_QUEUED)
	    printf("\tPriority: %6.1f\n",
	    (float) queue->v.que.priority +
            ((float) (time (0) - req->enter_time) * AGE_FACTOR / 3600.0));
	else
	    printf("\tPriority: %d\n",queue->v.que.priority);
#else
	printf("\tPriority: %d\n",queue->v.que.priority);
#endif
	printf("\tState: ");

	/* step 7. get time */
#ifdef SDSC
	if (req->start_time > 0 && req->period_inx >= 0 &&
	queue->v.que.priority >= TSCHED_PRI && req->period_inx <= MAX_PERIOD) {
	    printf(" T%s",get_rstate(queue->v.que.type, state));
	} else {
	    printf(" %s",get_rstate(queue->v.que.type, state));
	}
#else
	printf(" %s",get_rstate(queue->v.que.type, state));
#endif

#ifdef SDSC
	if ((state == SHO_RS_HOLD) || (state == SHO_RS_WAIT))
		printf(" \teligible at: %s \n",fmttime(&req->start_time));
	else
        if (state != SHO_RS_QUEUED)
		printf(" \t\t entered at: %s \n",fmttime(&req->enter_time));
#else
        if ((state == SHO_RS_HOLD) || (state == SHO_RS_WAIT) ||
                        (state== SHO_RS_RUN) )
                printf(" \teligible at: %s ",fmttime(&req->start_time));
        else
                printf(" \t\t entered at: %s \n",fmttime(&req->enter_time));
#endif
	printf("\tCreated at %s ",fmttime(&req->create_time));
	
	/* step 8. get queue */
	printf("\nQUEUE\n");
	printf("\tName:  %s@%s \n",req->quename,hostname);

	/* step 9. get resources */
	printf("\nRESOURCES\n");
	shoreql(queue,req);

	/* step 10. get stdout, stderr */
	printf("\nFILES\t\tMODE\t\tNAME\n");
        if ((req->v.bat.stderr_acc & OMD_EO) == 0) {
        	if (req->v.bat.stderr_acc & OMD_M_KEEP) {
                	if (queue->v.que.type == QUE_BATCH) {
                                    /*
                                     *  Use the name of the local host.
                                     */
                                    machinename = hostname;
                        }
                                else machinename = "<execution-host>";
                 }
                 else {
                                machinename=getmacnam(req->v.bat.stderr_mid);
                 }
                 printf("\tStderr: ");
		 shomd(req->v.bat.stderr_acc);
		 printf("\t%s\n",  req->v.bat.stderr_name);
	 }
         if (req->v.bat.stdout_acc & OMD_M_KEEP) {
                 if (queue->v.que.type==QUE_BATCH) {
                                /*
                                 *  Use the name of the local
                                 *  host.
                                 */
                                machinename = hostname;
                 }
                 else machinename = "<execution-host>";
                 }
                 else {
                            machinename = getmacnam (req->v.bat.stdout_mid);
         }
        printf ("\tStdout: ");
	shomd (req->v.bat.stdout_acc);
        printf ("  \t%s\n", req->v.bat.stdout_name);


	/* step 10. get mail */
	printf("\nMAIL\n");
	printf ("\tAddress: %s@%s\n", req->mail_name,
        getmacnam (req->mail_mid));

	printf ("\tWhen: ");
	if (req->flags & RQF_BEGINMAIL) {
		if (req->flags & RQF_ENDMAIL) 
			printf("begin, end\n");
		else printf ("begin\n");
	}
	else if (req->flags & RQF_ENDMAIL)  {
			printf(" end\n");
	}
	else printf("report of exception condition\n");
 
	/* step 10. get misc */
	printf("\nMISC\n");
	printf("\tRerunnable: "); 

	if (req->flags & RQF_RESTARTABLE)
		printf("yes ");
	else printf("no ");

	printf(" \t\t User Mask: %d \n",req->v.bat.umask);
	printf("\t\t\t\t\t Orig. Owner: %s \n",req->username);
        printf ("\tShell: ");
        if (req->v.bat.shell_name [0] == '\0') {
        	printf ("DEFAULT\n");
        }
        else {  /* An explicit shell spec is given */
        	fputs (req->v.bat.shell_name, stdout);
        	putchar ('\n');
        }
#ifdef SDSC
	/* list partition if is is a dedicated job */

	printf ("\n");
	if (req->start_time > 0 && req->period_inx >= 0 &&
	queue->v.que.priority >= TSCHED_PRI && req->period_inx <= MAX_PERIOD) {
	    printf ("Dedicated partition: %s%-d_%-d\n", 
	    PART_PFIX, req->orig_mid, req->orig_seqno); 
	}
#endif

	/*
	 * NOT DEFINED AT THIS POINT
 	 *
	 *printf("\nEXPORTED VARIABLES\n");
	 *printf("\t not defined presently\n");
	 *printf("\nARGUMENTS \n");
	 *printf("\t not defined presently\n");
	 */

	printf("\n");
	
}

#ifdef SDSC
/*** read_glb_param
 *
 * Subroutine to read some global parameters from the sched_param files.
 * Parameters read :
 *	AGE_FACTOR
 *	major_sz
 *	minor_sz
 *	PRIME_START
 *	PRIME_END
 *
 * return value :
 *      0 - no error.
 *      1 - param file does not exist
 *     -1 - input error
 */  
 
read_glb_param ()
{
        FILE *param_fp;
        char key_buf[NAME_LEN];
        char buf[INSIZE];               /* buffer to read in param data */
        char *bp;                 /* temp buffer pointer */
        long temp;
        register c;
        int comment_flag;
        int got_colon;
	int height, width, j;
	float tmp_float[DAYS_PER_W];
 
	/* set default values */

	AGE_FACTOR = DF_AGE_FACTOR;
	MACS_FLAG = DF_MACS_FLAG;
	TSCHED_PRI = DF_TSCHED_PRI;
        strcpy (TIME_ZONE, "TZ=");
        strcat (TIME_ZONE, DF_TIME_ZONE);
	for (j = 0; j < DAYS_PER_W; j++) {
	    PRIME_START[j] = DF_PRIME_START; 
	    PRIME_END[j] = DF_PRIME_END; 
	}

        /* open the param file */
 
        if ((param_fp = fopen (PARAM_FILE , "r")) == NULL) 
                return (1);
 
        /* read the data */
 
        while (1) {
 
                /* read 1 line at a time */
 
            buf[0] = '\0';
            key_buf[0] = '\0';
 
            bp = buf;
            comment_flag = 0;
            got_colon = 0;  /* replace only the first colon */
            while ((c = getc (param_fp)) != '\n') {
                if (c == EOF) break;
                if (c == '#') {
                    comment_flag = 1;
                    continue;
                }
                if (comment_flag) continue;
                if (c == ':' && got_colon == 0) {
                    c=' ';
                    got_colon = 1;
                }
                *bp ++= c;
            }
            *bp = '\0';
            if (c == EOF) break;

            /* parse the line */

            /* read the key word */

	    if ((temp = sscanf (buf,"%s",key_buf)) <= 0)
		continue;

            if (strcmp (key_buf, "age_factor") == 0) {
	        temp = sscanf (buf, "%s%f", key_buf, &AGE_FACTOR);
	    } else if (strcmp (key_buf, "prime_start") == 0) {
               temp = sscanf (buf, "%s%f%f%f%f%f%f%f", key_buf
               ,&tmp_float[0]
               ,&tmp_float[1]
               ,&tmp_float[2]
               ,&tmp_float[3]
               ,&tmp_float[4]
               ,&tmp_float[5]
               ,&tmp_float[6]);
               for (j = 0; j < DAYS_PER_W; j++) {
                   PRIME_START[j] = (int) (tmp_float[j] * 3600.0);
               }
	    } else if (strcmp (key_buf, "prime_end") == 0) {
               temp = sscanf (buf, "%s%f%f%f%f%f%f%f", key_buf
               ,&tmp_float[0]
               ,&tmp_float[1]
               ,&tmp_float[2]
               ,&tmp_float[3]
               ,&tmp_float[4]
               ,&tmp_float[5]
               ,&tmp_float[6]);
               for (j = 0; j < DAYS_PER_W; j++) {
                   PRIME_END[j] = (int) (tmp_float[j] * 3600.0);
               }
	    } else if (strcmp (key_buf, "time_zone") == 0) {
		strcpy (TIME_ZONE, "TZ=");
		bp = TIME_ZONE + 3;
		temp = sscanf (buf, "%s%s", key_buf, bp);
	    } else if (strcmp (key_buf, "tsched_pri") == 0) {
		temp = sscanf (buf, "%s%d", key_buf, &TSCHED_PRI);
	    } else if (strcmp (key_buf, "macs_flag") == 0) {
		temp = sscanf (buf, "%s%d", key_buf, &MACS_FLAG);
		if (MACS_FLAG < MACS_OFF || MACS_FLAG > MACS_ON_NOKILL) {
		    MACS_FLAG = DF_MACS_FLAG;
		}

	    }
	}
	fclose (param_fp);
	
	/* parse nodef */
/*
	if (strcmp (MAJORP_NODEF, "0") == 0) {
	    glb.major_sz = 0;
        } else {
            temp = sscanf (MAJORP_NODEF, "%dx%d", &height, &width);
	    glb.major_sz = height * width;
	}

	if (strcmp (MINORP_NODEF, "0") == 0) {
	    glb.minor_sz = 0;
        } else {
            temp = sscanf (MINORP_NODEF, "%dx%d", &height, &width);
	    glb.minor_sz = height * width;
	}
*/

	return (0);
}

/* read_grp (grp_inx)
 * 
 * routine to read in the number of nodes in available in the specified 
 * grp_inx
 *
 */
read_grp (grp_inx)
int grp_inx;
{
        FILE *param_fp;
        char key_buf[NAME_LEN];
        char buf[INSIZE];               /* buffer to read in param data */

        char *bp;                 /* temp buffer pointer */
        register int i, j;                    /* temp index */
        long temp;
        int comment_flag;
        register c;
        int got_colon;
        int got_digit;
        int in_len;
        int space_last;         /* last char is a space */
        int num_entry;          /* number of entries */
        char tmp_part_name[PART_NAME_LEN];
        char tmp_nodef[NAME_LEN];       /* temp nodef */
        int tmp_inx, set_inx;
        int tmp_flag[MAX_SET];
	struct node_set node_set[MAX_SET];
	int height, width;

        /* open the param file */

        if ((param_fp = fopen (PARAM_FILE , "r")) == NULL) {
                printf ("Unable to open param file. \n");
                fflush (stdout);
                return (-11);
        }

        glb.rpart_cnt = 0;
        glb.num_set = 0;
        glb.num_group = 0;
        for (i = 0; i < MAX_SET; i++) {
            node_set[i].rootp_inx = -1;
            node_set[i].size = 0;
            for (j = 0; j < MAX_LAYER; j++) {
                node_set[i].blocked[j] = 0;
            }
        }

        node_grp.num_set = 0;
        for (j = 0; j < MAX_SET; j++) {
            node_grp.time_flag[j] = 0;
        }

        /* read the data */

        while (1) {

                /* read 1 line at a time */

                buf[0] = '\0';
                key_buf[0] = '\0';
                tmp_part_name[0] = '\0';

                bp = buf;
                comment_flag = 0;
                space_last = 1;
                got_colon = 0;  /* replace only the first colon */
                got_digit = 0;  /* replace only the first colon */
                num_entry = 0;
                in_len = 0;     /* input length in bytes */

                while ((c = getc (param_fp)) != '\n') {
                        in_len ++;
                        if (in_len >= INSIZE)
                                break;
                        if (c == EOF) break;
                        if (c == '#') {
                                comment_flag = 1;
                                continue;
                        }
                        if (comment_flag) continue;
                        if (c == ':' && got_colon == 0) {
                                c=' ';
                                got_colon = 1;
                        }
                        if (isdigit (c) != 0 && num_entry == 1 &&
                        got_digit == 0 && space_last == 0) {
                                *bp ++= ' ';    /* put a blank before the */
                                                /* first digit */
                                space_last = 1;
                                got_digit = 1;
                        }
                        if (c == ' ') {
                            space_last = 1;
                        } else {
                            if (space_last == 1)
                                num_entry ++;
                            space_last = 0;

                            /* The index entry must be all digit */

                            if (got_digit == 1 && num_entry == 2) {
                                if (isdigit (c) == 0) {
                                    *bp = '\0';
                                    printf ("read_grp: Problem with the");
                                    printf (" input line begins with : %s\n",
                                    buf);
                                    return (1);
                                }
                            }
                        }
                        *bp ++= c;
                }

                *bp = '\0';
                if (c == EOF) break;

                /* parse the line */

                /* read the key word */

                if (got_digit == 1) {
                    if ((temp = sscanf (buf,"%s %d", key_buf, &tmp_inx)) <= 0)
                         continue;
                } else {
                    tmp_inx = -1;
                    if ((temp = sscanf (buf,"%s", key_buf)) <= 0)
                         continue;
                }

                if (strcmp (key_buf, "node_group") == 0) {
		    if (tmp_inx != grp_inx)
			continue;

                    if (num_entry > MAX_SET - 2 || num_entry <= 2) {
                        printf ("read_grp: Number of entries for ");
                        printf ("group_set %d out of range.", tmp_inx);
                        printf (" MAX = %d\n", MAX_SET);
                        return (-1);
                    }

                    glb.grp_inx[glb.num_group] = tmp_inx;
                    glb.num_group ++;
                    node_grp.num_set = num_entry - 2;

                    bp = buf;

                    /* skip any space before keyword */

                    while ((c = (int) *bp) == ' ') {
                        bp++;
                    }

                    /* skip the key words */

                    while ((c = (int) *bp) != ' ') {
                        bp++;
                    }

                    /* skip any space before the index */

                    while ((c = (int) *bp) == ' ') {
                        bp++;
                    }

                    /* skip the index */

                    while ((c = (int) *bp) != ' ') {
                        bp++;
                    }

                    j = 0;

                    /* read in the value */


                    while ((c = (int) *bp) != '\0') {
                        if (c == ' ') {   /* skip space */
                            bp++;
                            continue;
                        }
                        if (j >= num_entry - 2) {
                            printf ("read_grp:node_set cnt problem ");
                            printf ("for node_group %d\n", tmp_inx);
                                return (-1);
                                break;
                        }
                        node_grp.set_inx[j] = atoi (bp);
                            j++;

                        while ((c = (int) *bp) != ' ') {
                            if (c == '\0')
                                 break;
                            bp++;
                        }
                   }
               } else if (strcmp (key_buf, "node_set") == 0) {
                   if (num_entry != 4) {
                       printf ("read_grp: Wrong number of entries ");
                       printf ("for node_set %d", tmp_inx);
                           return (-1);
                   }
                   temp = sscanf (buf, "%s%d%s%s", key_buf,
                   &tmp_inx, tmp_part_name, tmp_nodef);

                   if (tmp_inx < 0 || tmp_inx >= MAX_SET) {
                       printf ("read_grp: Node_set index ");
                       printf ("%d out of range. ", tmp_inx);
                       printf ("MAX = %d\n", MAX_SET - 1);
                           return (-1);
                   }

                   if (node_set[tmp_inx].rootp_inx >= 0) {
                       printf ("read_grp: Duplicate node_set %d\n", tmp_inx);
                       return (-1);
                   }

                   glb.set_inx[glb.num_set] = tmp_inx;
                   glb.num_set ++;
                   strncpy (node_set[tmp_inx].nodef, tmp_nodef, NAME_LEN);
                } else if (strcmp (key_buf, "prime_list") == 0) {
                   if (tmp_inx < 0) {
                       printf ("read_grp: Problem with the");
                       printf (" following input line : %s\n", buf);
                       return (-1);
                    }
   		    if (tmp_inx != grp_inx)
  			continue;
                    if (num_entry > MAX_SET - 2 || num_entry < 2) {
                        if (num_entry == 2)
                            continue;
                         printf ("read_grp: Number of entries for ");
                         printf ("prime_list%d out of range.", tmp_inx);
                         printf (" MAX = %d\n", MAX_SET);
                         return (-1);
                    }
                    if (tmp_inx < 0 || tmp_inx >= MAX_GROUP) {
                        printf ("read_grp: Node_group index ");
                        printf ("%d out of range. ", tmp_inx);
                        printf (" MAX = %d\n", MAX_GROUP - 1);
                        return (-1);
                    }

                     bp = buf;

                     /* skip any space before keyword */

                     while ((c = (int) *bp) == ' ') {
                         bp++;
                     }

                     /* skip the key words */

                     while ((c = (int) *bp) != ' ') {
                         bp++;
                     }

                     /* skip any space before the index */


                     while ((c = (int) *bp) == ' ') {
                         bp++;
                     }

                     /* skip the index */

                     while ((c = (int) *bp) != ' ') {
                         bp++;
                     }

                     /* read in the value */

                     while ((c = (int) *bp) != '\0') {
                         if (c == ' ') {   /* skip space */
                             bp++;
                             continue;
                         }
                         set_inx = atoi (bp);
                         if (set_inx >= MAX_SET || set_inx < 0) {
                             printf ("read_grp:Sets %d in ", set_inx);
                             printf ("prime_list %d out of range.\n", tmp_inx);
                             break;
                         }
                         node_grp.time_flag[set_inx] |= 1;

                         while ((c = (int) *bp) != ' ') {
                             if (c == '\0')
                                 break;
			      bp++;
                         }
                     }
                     continue;

                } else if (strcmp (key_buf, "nprime_list") == 0) {
                    if (tmp_inx < 0) {
                        printf ("read_grp: Problem with the");
                        printf (" following input line : %s\n", buf);
                        return (-1);
                    }
   		    if (tmp_inx != grp_inx)
  			continue;
                    if (num_entry > MAX_SET - 2 || num_entry < 2) {
                        printf ("read_grp: Number of entries for ");
                        printf ("nprime_list%d out of range.", tmp_inx);
                        printf (" MAX = %d\n", MAX_SET);
                            return (-1);
                    }
                    if (num_entry == 2)
                        continue;
                    if (tmp_inx < 0 || tmp_inx >= MAX_GROUP) {
                        printf ("read_grp: Node_group index ");
                        printf ("%d out of range. ", tmp_inx);
                        printf (" MAX = %d\n", MAX_GROUP - 1);
                            return (-1);
                    }

                    bp = buf;

                    /* skip any space before keyword */

                    while ((c = (int) *bp) == ' ') {
                        bp++;
                    }

                    /* skip the key words */

                    while ((c = (int) *bp) != ' ') {
                        bp++;
                    }

                    /* skip any space before the index */

                    while ((c = (int) *bp) == ' ') {
                        bp++;
                    }

                    /* skip the index */

                    while ((c = (int) *bp) != ' ') {
                        bp++;
                    }

                    /* read in the value */

                    while ((c = (int) *bp) != '\0') {
                        if (c == ' ') {   /* skip space */
                            bp++;
                            continue;
                        }
                        set_inx = atoi (bp);
                        if (set_inx >= MAX_SET || set_inx < 0) {
                            printf ("read_grp:Sets %d in ", set_inx);
                            printf ("nprime_list %d out of range.\n",tmp_inx);
                            break;
                        }
                        node_grp.time_flag[set_inx] |= 2;

                        while ((c = (int) *bp) != ' ') {
                            if (c == '\0')
                                break;
			    bp++;
                        }
                    }
		}
	}

        for (j = 0; j < MAX_SET; j++) {
            tmp_flag[j] = node_grp.time_flag[j];
            node_grp.time_flag[j] = 0;
        }
        for (j = 0; j < node_grp.num_set; j++) {
            set_inx = node_grp.set_inx[j];
            if (tmp_flag[set_inx] == 3) {
                node_grp.time_flag[j] = MAJOR;
            } else if (tmp_flag[set_inx] == 2) {
                node_grp.time_flag[j] = MINOR;
            } else if (tmp_flag[set_inx] == 1) {
                node_grp.time_flag[j] = MAJOR;
            } else {
                node_grp.time_flag[j] = DF_TIME_FLAG;
            }
        }

        for (i = 0; i < MAX_PERIOD; i++) {
            node_grp.size[i] = 0;
            for (j = 0; j < MAX_LAYER; j ++) {
                node_grp.free_nodes[j][i] = 0;
            }
        }

        for (i = 0; i < node_grp.num_set; i++) {
            set_inx = node_grp.set_inx[i];
            tmp_inx = node_grp.time_flag[i];
            temp = sscanf (node_set[set_inx].nodef, "%dx%d", &height, &width);
	    if (temp !=2) {
		sscanf (node_set[set_inx].nodef, "%dX%d", &height, &width);
	    }
            node_grp.free_nodes[0][tmp_inx] += height * width;
	}

	return (0);
}


/*** list_tsched
 * 
 * list the available time slot of the tennis court schedule
 */
int list_tsched (file,name,req,flags,selector,daepresent,qmapfile,
pipeqfile,qcomplexfile)
register struct confd *file;            /* the NQS queue file */
char * name;                            /* search value  */
char * req;                             /* request value  */
register long flags;                    /* Display flags */
register char * selector;               /* account, group, or user list */
register short daepresent;
register struct confd *qmapfile;            /* the NQS qmap file */
register struct confd *pipeqfile;            /* the NQS pipe file */
struct confd *qcomplexfile;     /*NQS queue complex definition file */
{

        register struct gendescr *que_descr;
        struct qentry cache [QOFILE_CACHESIZ];
	int cacheindex = 0;		/* Current buffer cache index */
        struct rawreq   request;
        int fd;                         /* Queue ordering file descriptor */
        int i;                          /* Loop var */
        int csize;
        int count;
        int cfd;
	struct schedule tsched[SEC_P_DAY / TSCHED_TSLOT];	
						/* 1 day worth of schedule */
	int slot_cnt, req_time;
	int n_day;
	char period[8];
        int major_used, minor_used, combo_used, combo_free;
        int major_free, minor_free;
        struct tm *tm;                  /* tm struct */
	int grp_inx;			/* node_group index */
 
        /* read the age factor from the sched_param file */
        
	read_glb_param ();
	
	/* fill in the tsched array */

        glb.cur_time = time (0);
        tm = localtime (&glb.cur_time);
	i = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

	/* round off to TSCHED_TSLOT boundary */

	n_day = atoi (selector);
	if (n_day < 0 || n_day > DAYS_PER_W) {
	    printf ("-t value out of range, must be >= 0 and <=6\n");
	    return (-1);
	}
	if (n_day == 0) {
            glb.cur_time24 = i / TSCHED_TSLOT * TSCHED_TSLOT;
	    glb.cur_time = glb.cur_time - i + glb.cur_time24;
	    req_time = SEC_P_DAY - glb.cur_time24;
	    slot_cnt = req_time / TSCHED_TSLOT;
	} else {
            glb.cur_time24 = 0;
	    glb.cur_time = glb.cur_time - i + n_day * SEC_P_DAY;
	    req_time = SEC_P_DAY;
	    slot_cnt = req_time / TSCHED_TSLOT;
	}

        seekdbb(file,0L);
        que_descr = nextdb(file);
	grp_inx = -1;
	
        while (que_descr != NULL) {

	    fd = openqord (file, que_descr);
	    if (fd < -1) return (fd);	/* Error or queue was deleted */

	    if (strcmp(que_descr->v.que.namev.name,name)== 0) {
		if (grp_inx == -1) {
		    grp_inx = que_descr->v.que.v1.batch.node_group;

		    /* Initialize the tsched array */

        	    if (init_tsched (tsched, slot_cnt, 
		    glb.cur_time, req_time, grp_inx) < 0) {
            		printf ("Prob with init. the time slot table\n");
            		return (-1);
        	    } 
		}
	    } else {
	        que_descr = nextdb (file);
		continue;
	    }

	    cacheindex = 0;		/* Current buffer cache index */
	    /* skip departing request */

	    if ( (count = que_descr->v.que.departcount) != 0) {
		for(i = 0; i < count; i++) {
            	    csize = read (fd, (char *) &cache, sizeof (cache));
		    cacheindex ++;		/* Current buffer cache index */
		}
	    }

	    /* process running request */

	    if ( (count = que_descr->v.que.runcount) != 0) {
		for(i = 0; i < count; i++) {
            	    csize = read (fd, (char *) &cache, sizeof (cache));
		    if ((cfd = getrreq ((long) cache[cacheindex].orig_seqno, 
		    cache[cacheindex].orig_mid, &request)) == -1) {
			printf ("getrreq error.\n");
		    }
		    cacheindex ++;		/* Current buffer cache index */
		    if (request.start_time > 0 &&
		    que_descr->v.que.priority >= TSCHED_PRI) {
			if (lfill_nodes (tsched, glb.cur_time, 
			glb.cur_time + req_time,
			request.start_time,
                    	request.start_time + 
		    	request.v.bat.prcputime.max_seconds,
                    	request.v.bat.prncpus,
                    	request.major_nodes) < 0) {
                            printf ("Problem filling time slot table\n");
                            return (-1);
			}
                    }
		}
	    }

	    /* skip staged request */

	    if ( (count = que_descr->v.que.stagecount) != 0) {
		for(i = 0; i < count; i++) {
            	    csize = read (fd, (char *) &cache, sizeof (cache));
		    cacheindex ++;		/* Current buffer cache index */
		}
	    }

	    /* process queued request */

	    if ( (count = que_descr->v.que.queuedcount) != 0) {
		for(i = 0; i < count; i++) {
            	    csize = read (fd, (char *) &cache, sizeof (cache));
		    if ((cfd = getrreq ((long) cache[cacheindex].orig_seqno, 
		    cache[cacheindex].orig_mid, &request)) == -1) {
			printf ("getrreq error.\n");
		    }
		    if (request.start_time > 0 &&
		    que_descr->v.que.priority >= TSCHED_PRI) {
			if (lfill_nodes (tsched, glb.cur_time, 
			glb.cur_time + req_time,
			request.start_time,
                    	request.start_time + 
		    	request.v.bat.prcputime.max_seconds,
                    	request.v.bat.prncpus,
                    	request.major_nodes) < 0) {
                            printf ("Problem filling time slot table\n");
                            return (-1);
			}
                    }
		    cacheindex ++;		/* Current buffer cache index */
		}
	    }
	    /* process waiting request */

	    if ( (count = que_descr->v.que.waitcount) != 0) {
		for(i = 0; i < count; i++) {
            	    csize = read (fd, (char *) &cache, sizeof (cache));
		    if ((cfd = getrreq ((long) cache[cacheindex].orig_seqno, 
		    cache[cacheindex].orig_mid, &request)) == -1) {
			printf ("getrreq error.\n");
		    }
		    if (request.start_time > 0 &&
		    que_descr->v.que.priority >= TSCHED_PRI) {
			if (lfill_nodes (tsched, glb.cur_time, 
			glb.cur_time + req_time,
			request.start_time,
                    	request.start_time + 
		    	request.v.bat.prcputime.max_seconds,
                    	request.v.bat.prncpus,
                    	request.major_nodes) < 0) {
                            printf ("Problem filling time slot table\n");
                            return (-1);
			}
                    }
		    cacheindex ++;		/* Current buffer cache index */
		}
	    }

	    que_descr = nextdb (file);
        }

	/* Now print the free slot table */

	printf ("Schedule for Dedicated Request starting - %s\n\n",
	ctime (&glb.cur_time));
        printf ("TIME    MODE    USED   FREE\n");
        printf ("-----  ------   -----------\n");

        for (i = 0; i < slot_cnt; i++) {
            tm = localtime (&glb.cur_time);
            if (tsched[i].mode == PRIME_M) {
                strcpy (period, "PRIME ");
                major_free = tsched[i].node_cnt[MAJOR];
                major_used = node_grp.free_nodes[0][MAJOR] - 
		tsched[i].node_cnt[MAJOR];
                minor_used = 0;
                minor_free = 0;
                combo_used = 0;
                combo_free = 0;
            } else {
                strcpy (period, "NPRIME");
                minor_used = node_grp.free_nodes[0][MINOR] - 
		tsched[i].node_cnt[MINOR];
                major_used = node_grp.free_nodes[0][MAJOR] - 
		tsched[i].node_cnt[MAJOR];
                combo_used = major_used + minor_used;
                minor_free = tsched[i].node_cnt[MINOR];
                major_free = tsched[i].node_cnt[MAJOR];
                combo_free = tsched[i].node_cnt[COMBO];
	    }
		    
	    if (tm->tm_min == 0) {
            	printf ("%2d:%1d0", tm->tm_hour, tm->tm_min);
	    } else {
            	printf ("%2d:%2d", tm->tm_hour, tm->tm_min);
	    }
            printf ("  %6s  %4d   %4d\n",
            period, major_used + minor_used, major_free + minor_free);

	    glb.cur_time += TSCHED_TSLOT;
        }
        fflush (stdout);

        return(0);
} 

/* init_tsched
 *
 * Routine that fills in the mode and node_cnt of the tsched schedule.
 * The node_cnt is the number of free nodes available. The node_cnt
 * for the COMBO partition are set to zero initially.
 */
int init_tsched (tsched, slot_cnt, start_time, req_time, grp_inx)
struct schedule *tsched;
int slot_cnt;
time_t start_time;
int req_time;
int grp_inx;
{
        int i, j;
        time_t cur_time;
        long p_start;
        long p_end;
        long cur_time24;
        long mode;
        int slot_used, cur_slot;
 
        if (tsched == NULL)
            return (-1);
 
        /* req_time too long ? */
 
        if (slot_cnt > MAX_SLOT_CNT) {
            printf ("Inconsistent number of time slots\n");
            return (-1);
        }
 
        if (slot_cnt * TSCHED_TSLOT != req_time) {
            printf ("Mismatch in slot_cnt and req_time\n");
            return (-1);
        }
 
	if (read_grp (grp_inx) < 0) {
	    return (-1);
	}

        /* Now fill in the slot */
 
        cur_time = start_time;
        cur_slot = 0;
        while (cur_slot < slot_cnt) {
            lget_time (cur_time, &p_start, &p_end, &cur_time24, &mode);
            if (mode == PRIME_M) {
                slot_used = (p_end - cur_time24) / TSCHED_TSLOT;
                if (slot_used <= 0)
                    slot_used = 1;  /* so it won't get stuck */
                if (cur_slot + slot_used > slot_cnt)
                    slot_used = slot_cnt - cur_slot;
                for (i = cur_slot; i < cur_slot + slot_used; i++) {
                    tsched[i].mode = mode;
                    tsched[i].node_cnt[MAJOR] = 
		    node_grp.free_nodes[0][MAJOR];
                    tsched[i].node_cnt[MINOR] = 0;
                    tsched[i].node_cnt[COMBO] = 0;
                }
            } else {
                slot_used = (cur_time24 - p_end) / TSCHED_TSLOT;
                if (slot_used <= 0)
                    slot_used = 1;  /* so it won't get stuck */
                if (cur_slot + slot_used > slot_cnt)
                    slot_used = slot_cnt - cur_slot;
                for (i = cur_slot; i < cur_slot + slot_used; i++) {
                    tsched[i].mode = mode;
                    tsched[i].node_cnt[MAJOR] = node_grp.free_nodes[0][MAJOR];
                    tsched[i].node_cnt[MINOR] = node_grp.free_nodes[0][MINOR];
                    tsched[i].node_cnt[COMBO] = node_grp.free_nodes[0][MAJOR] +
		    node_grp.free_nodes[0][MINOR];
                }
            }
            cur_slot += slot_used;
            cur_time += slot_used * TSCHED_TSLOT;
        }
        return (0);
}
 
/* lfill_nodes
 * 
 * Routine that decrease the node_cnt of the tsched schedule if a job's
 * j_start_time - j_end_time overlaps with the start_time and end_time
 * of the tsched.
 *
 */
int lfill_nodes (tsched, start_time, end_time, j_start_time, j_end_time,
req_nodes, major_nodes)    
struct schedule *tsched;
time_t start_time;
time_t end_time; 
time_t j_start_time;
time_t j_end_time; 
long req_nodes;  
{
        int i, j;
        int start_slot, end_slot;
	long minor_nodes;
 
        /* req_time too long ? */
 
	minor_nodes = req_nodes - major_nodes;
        if ((end_time - start_time) / TSCHED_TSLOT > MAX_SLOT_CNT) {
            printf ("Inconsistent number of time slot\n");
            return (-1);
        }
 
        if (j_start_time < start_time)
            start_slot = start_time;
        else if (j_start_time > end_time)
            start_slot = end_time;
        else
            start_slot = j_start_time;
        
        if (j_end_time < start_time)
            end_slot = start_time;
        else if (j_end_time > end_time)
            end_slot = end_time;
        else
            end_slot = j_end_time;
 
        if (end_slot > start_slot) {
            start_slot = (start_slot - start_time) / TSCHED_TSLOT;
            end_slot = (end_slot - start_time) /TSCHED_TSLOT - 1;
 
            for (i = start_slot; i <= end_slot; i++) {
                tsched[i].node_cnt[COMBO] -= req_nodes;
                tsched[i].node_cnt[MAJOR] -= major_nodes;
                tsched[i].node_cnt[MINOR] -= minor_nodes;

            }
        }
        return (0);
}


/*** lget_time ()
 *
 * Subroutine to set the time for the scheduler.
 * input : cur_time - time since epoch time in sec
 * output :
 *      p_start - prime start since the start of the day in sec
 *      p_end - prime end since the start of the day in sec
 *      cur_time24 - curent time since the start of the day
 *      mode
 *
 *
 *
 */
 
void lget_time (cur_time, p_start, p_end, cur_time24, mode)
time_t cur_time;
long *p_start;
long *p_end;
long *cur_time24;
long *mode;
{
        int i;
        struct tm *tm;                  /* tm struct */
 
        /* check current local time and adjust glb.cur_time24 */
 
        tm = localtime (&cur_time);
        i = tm->tm_wday;                /* day of the week */
        *cur_time24 = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;
        if (PRIME_START[i] > PRIME_END[i]) {
                if (*cur_time24 <= PRIME_END[i]) {
                        *p_end = PRIME_END[i];
                        i -= 1;         /* yesterday */
                        if (i < 0)
                                i = DAYS_PER_W - 1;
                        *p_start = PRIME_START[i] - SEC_P_DAY;
                } else {
                        *p_start = PRIME_START[i];
                        i += 1;         /* next day */
                        if (i >= DAYS_PER_W)
                                i = 0;
                        *p_end = PRIME_END[i] + SEC_P_DAY;
                }
        } else {
 
                if (*cur_time24 >= PRIME_END[i]) {
                        i += 1;         /* next day */
                        if (i >= DAYS_PER_W)
                                i = 0;
                        *p_start = PRIME_START[i] + SEC_P_DAY;
                        *p_end = PRIME_END[i] + SEC_P_DAY;
                        if (*p_end < *p_start)
                                *p_end = *p_start;
                } else {
                        *p_start = PRIME_START[i];
                        *p_end = PRIME_END[i];
                }
        }
 
        if (*cur_time24 >= *p_start && *cur_time24 < *p_end) {
                *mode = PRIME_M;
        } else {
                *mode = NPRIME_M;
        }
}
 
#endif
