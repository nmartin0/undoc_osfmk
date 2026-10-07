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
 * $Log: listq.c,v $
 * Revision 1.9  1995/02/24  23:36:26  kremenek
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 5134
 *  Testing: Developer testing
 *  Module(s):      ./cmds_libs/src/usr/bin/qmgr/mgr_cmd.c
 *                 ./cmds_libs/src/usr/bin/qmgr/mgr_main.c
 *                 ./cmds_libs/src/usr/bin/qmgr/mgr_packet.c
 *                 ./cmds_libs/src/usr/bin/qmgr/qmgr.hlp
 *                 ./cmds_libs/src/usr/ccs/lib/libnqs/listq.c
 *                 ./cmds_libs/src/usr/include/nqs/nqspacket.h
 *                 ./cmds_libs/src/usr/include/nqs/nqsstruct.h
 *                 ./cmds_libs/src/usr/lib/nqs/macs_sched.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_ldconf.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_main.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_nsq.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_upq.
 *
 * Revision 1.8  1994/11/19  02:27:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/11/18  20:36:00  davidl
 * Several minor changes that were not properly merged from R1.2 into
 * R1.3.  These included typos in printfs in listq.c and macs_rootp.c,
 * missing .h file targets in usr/include/macs/Makefile, and missing
 * "#ifdef SDSC" declarations in db.h and mac.h.
 *
 *  Reviewer: tracy
 *  Risk: Low
 *  Benefit or PTS #: 8230
 *  Testing: Developer testing, EATs
 *  Module(s):     cmds_libs/src/usr/include/macs/Makefile
 *                 cmds_libs/src/usr/ccs/lib/libnqs/listq.c
 *                 cmds_libs/src/usr/lib/nqs/macs_rootp.c
 *                 cmds_libs/src/usr/include/macs/db.h
 *                 cmds_libs/src/usr/include/macs/mac.h
 *
 * Revision 1.6  1994/11/02  18:14:24  kremenek
 *  Reviewer: davidl doyle
 *  Risk: low
 *  Benefit or PTS #: 7149
 *  Testing: EATS
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/listq.c
 *
 * Revision 1.5  1994/04/06  22:12:28  mwan
 * Update R1_3 to R1_2 WW15
 *
 *  Reviewer: kremenek
 *  Risk: M
 *  Benefit or PTS #: 7738,8087,8346, 8325,8599,8576,8600,8601,8088,8597,8876,8886
 *  Testing:
 *  Module(s): fetchpwdb.c listq.c listr.c main_dsp.c
 *
 * Revision 1.4.2.1  1994/02/23  17:30:59  mwan
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
 *          usr/ccs/lib/libnqs/listq.c
 *             usr/ccs/lib/libnqs/listr.c
 *     
 * Revision 1.3  1993/07/13  17:57:27  mwan
 * T11 - fixed PTS 5012
 *
 * Revision 1.2  1993/05/19  17:48:30  mwan
 * Before T10 frozen. Fixed several PTS
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include "nqs.h"
#include "nqsdeflim.h"

#define DESTSET_MARGIN	13
#define OUTPUT_WIDTH	78
#define R_BRKT		"{"
#define L_BRKT		"}"
static int acccolumn=0;

extern int gethostname();
extern struct gendescr *nextdb(); /* Next NQS database file generic descr */
extern char *getmacnam();

/***dsp_q
 * 
 * send queue off to correct display
 * format if the type should be shown
 */
int dsp_q(file, que_descr, flag, dae_present,qmapfile,pipeqfile,qcomplexfile)
struct confd *file;                /* NQS queue file containing.... */
struct gendescr *que_descr;
short dae_present;                 /* Boolean non-zero if the local NQS */
long flag;
struct confd *qmapfile;            /* NQS queue file containing.... */
struct confd *pipeqfile;           /* NQS queue file containing.... */
struct confd *qcomplexfile;        /* NQS queue file containing.... */
{
 
        int fd;                         /* Queue ordering file descriptor */
        char state[10];
 
        /* step 1.  Override database if daemon not present.  */
        if (!dae_present) {
                que_descr->v.que.queuedcount
                        += que_descr->v.que.runcount;
                que_descr->v.que.runcount = 0;
        }
        
        
	/* step 2. if batch queues are to be shown, show them */
        if (flag & SHO_BATCH)
           if (que_descr->v.que.type == QUE_BATCH) {
			if (flag & SHO_FULL)
				full_q(file,que_descr,"BATCH",
					qmapfile,pipeqfile,qcomplexfile);
			else
                        	qstat_q( que_descr,dae_present);
        
                }

	/* step 3. if pipe queues are to be shown, show them */
        if (flag & SHO_PIPE)
                if (que_descr->v.que.type == QUE_PIPE) {
			if (flag & SHO_FULL)
				full_q(file,que_descr,"PIPE",
					qmapfile,pipeqfile,qcomplexfile);
			else
                        	qstat_q( que_descr,dae_present);
                }

	/* step 4. if pipe queues are to be shown, show them */
            if (flag & SHO_DEVICE)
                if (que_descr->v.que.type == QUE_DEVICE) {
			if (flag & SHO_FULL)
				full_q(file,que_descr,"DEVICE",
					qmapfile,pipeqfile,qcomplexfile);
			else
                        	qstat_q( que_descr,dae_present);
                }

	/* 	Network queues are not defined 
	** step 5. if pipe queues are to be shown, show them **
        if (flag & SHO_NETWORK)
                 if (que_descr->v.que.type == QUE_NET) {
			if (flag & SHO_FULL)
				full_q(file,que_descr,"NETWORK",qmapfile,pipeqfile,qcomplexfile);
			else
                        	qstat_q( que_descr,dae_present);
                }
	*/
}

 
/*** qstat_q
 * 
 * show queue in short format 
 */
qstat_q(que_descr,dae_present)
struct gendescr *que_descr;
short dae_present;	/* if NQS daemon is running */
{
 
char state[9];
 
        /* step 1.  Set the state of the queue */
	if (dae_present) {
        	if ( que_descr->v.que.status & QUE_ENABLED)
               	 if ( que_descr->v.que.status & QUE_RUNNING)
               	         strcpy(state,"AVAILBL");
               	 else
               	         strcpy(state,"STOPPED");
 
        	if (! (que_descr->v.que.status & QUE_ENABLED))
               	 if ( !(que_descr->v.que.status & QUE_RUNNING))
               	         strcpy(state,"UNAVAIL");
               	 else
               	         strcpy(state,"DISABLED");
	}	/* end of nqs daemon is running */
	else
		strcpy(state,"NQS DOWN");
	
                                                  
 
        /*
         *      How many requests are in queue
         */                                    
        printf("%-15.15s\t%-8s   %d ",
                que_descr->v.que.namev.name,                       
                state,                       
                que_descr->v.que.queuedcount +
                que_descr->v.que.runcount +    
                que_descr->v.que.departcount +
                que_descr->v.que.stagecount +  

                que_descr->v.que.waitcount +
                que_descr->v.que.arrivecount +
                que_descr->v.que.holdcount);
	switch (que_descr->v.que.type) {
	case QUE_BATCH:
#ifdef SDSC
		printf("\t%3d/%-3d",                
#else
		printf("\t%4d/%d ",
#endif
			que_descr->v.que.runcount,
			que_descr->v.que.v1.batch.runlimit);
		break;
	case QUE_PIPE:
#ifdef SDSC
		printf("\t%3d/%-3d",                
#else
		printf("\t%4d/%d ",
#endif
                        que_descr->v.que.runcount,
                        que_descr->v.que.v1.pipe.runlimit);
                break;
        case QUE_NET:
#ifdef SDSC
		printf("\t%3d/%-3d",                
#else
		printf("\t%4d/%d ",
#endif
                        que_descr->v.que.runcount,
                        que_descr->v.que.v1.network.runlimit);
                break;
	}
#ifdef SDSC
	if (que_descr->v.que.type == QUE_BATCH) {
	    printf("\t%6d \t%6d \t%6d \t%6d\n",	
                que_descr->v.que.queuedcount,
                que_descr->v.que.waitcount +
                que_descr->v.que.holdcount,
                que_descr->v.que.arrivecount +
                que_descr->v.que.departcount +
                que_descr->v.que.stagecount,
		que_descr->v.que.v1.batch.node_group);
	} 
        else { /* to support PIPE and other queues */
           printf("\t%6d \t%6d \t%6d\n",
                que_descr->v.que.queuedcount,
                que_descr->v.que.waitcount +
                que_descr->v.que.holdcount,
                que_descr->v.que.arrivecount +
                que_descr->v.que.departcount +
                que_descr->v.que.stagecount);
        }
#else
	printf("\t%6d \t%6d \t%6d\n",	
                que_descr->v.que.queuedcount,
                que_descr->v.que.waitcount +
                que_descr->v.que.holdcount,
                que_descr->v.que.arrivecount +
                que_descr->v.que.departcount +
                que_descr->v.que.stagecount);
#endif
}


/*** qstat_hdr(flags)
 * 
 * Display the header for qstat.
 * VERSION is defined in nqs.h
 *
 */
qstat_hdr(flags)
long 	flags;
{
char hostname[256];
        printf("============================================\n");
	printf("NQS Version:%2d ",VERSION);

        if(flags &  SHO_BATCH) 
		printf(" BATCH ");

#ifndef SDSC        
        if(flags &  SHO_DEVICE) 
		printf(" DEVICE ");
#endif

        if(flags &  SHO_PIPE) 
		printf(" PIPE ");
 
	printf("QUEUES on ");
	gethostname(hostname,255);
	hostname[255] = '\0';
	printf("%s \n",hostname);
        printf("============================================\n");


#ifdef SDSC
        printf("QUEUE NAME      STATUS   TOTAL   RUNNING  QUEUED   HELD TRANSITION  NODE_GROUP\n");
#else
        printf("QUEUE NAME      STATUS   TOTAL   RUNNING  QUEUED   HELD TRANSISTION\n");
#endif
        printf("-----------------------------------------------------------------------------\n");

}


/*** shoquedesmap
 *
 *
 *      void shoquedesmap():
 *      Display the destination set for a pipe queue.
 */
static void shoqdesmap (que_descr, qmapfile, pipeqfile)
struct gendescr *que_descr;             /* The queue descriptor */
struct confd *qmapfile;                 /* Queue/device/destination mapping */
                                        /* database file */
struct confd *pipeqfile;                /* Pipe-destination database file */
{
        register struct gendescr *qmap1;/* NQS database file generic descr */
        register struct gendescr *pipe2;/* NQS database file generic descr */
        register short column;          /* Current output column */
        register short fldsiz;          /* Size of output field */
        short preamble;                 /* Display flag */
        short first;                    /* Display flag */
        char *rhostname;                /* Remote host destination name */

	/* make sure  we start at beginning of file */
        seekdbb (qmapfile, 0L); /* Seek to the beginning */
        qmap1 = nextdb (qmapfile);
        preamble = 0;           /* "Destset = [" not displayed yet */
        first = 1;              /* If destination displayed, it will */
                                /* be the first in the set */
        while (qmap1 != (struct gendescr *) 0) {
                if (!qmap1->v.map.qtodevmap &&
                    strcmp (qmap1->v.map.v.qdestmap.lqueue,
                            que_descr->v.que.namev.name) == 0) {
                         /*
                         *  We have located a pipe-queue/destination mapping
                         *  for the pipe queue.
                         *
                         *  Now, locate the database description for the
                         *  pipe queue destination referenced in the mapping.
                         */
                        if (telldb (pipeqfile) != -1) {
                                /*
                                 *  We are not necessarily at the start of the
                                 *  pipe queue descriptor database file....
                                 */
                                seekdbb (pipeqfile, 0L);/* Seek to start */
                        }
                        pipe2 = nextdb (pipeqfile);
                        while (pipe2 != (struct gendescr *) 0 &&
                              (strcmp (qmap1->v.map.v.qdestmap.rqueue,
                                       pipe2->v.dest.rqueue) ||
                               qmap1->v.map.v.qdestmap.rhost_mid !=
                               pipe2->v.dest.rhost_mid)) {
                                /*
                                 *  We have not yet located the pipe queue
                                 *  destination referred to in the mapping
                                 *  descriptor.
                                 */
                                pipe2 = nextdb (pipeqfile);
                        }
                        if (pipe2 != (struct gendescr *) 0) {
                            /*
                             *  We have located the pipe-queue

                             *  descriptor for the mapping.
                             */
                            if (!preamble) {
                                /*
                                 *  Display destination set
                                 *  header.
                                 */
                                preamble = 1;
                                printf ("  Destset = %s",R_BRKT);
                                column = DESTSET_MARGIN; /* From 0 */
                            }
                            rhostname = getmacnam (pipe2->v.dest.rhost_mid);
                            if (!(pipe2->v.dest.status & DEST_ENABLED) &&
                                 (pipe2->v.dest.status & DEST_RETRY)) {
                                /*
                                 *  The destination is in the retry
                                 *  state.  Show the time of the
                                 *  next earliest retry, and the
                                 *  time of the first failure.
                                 */
                                if (column != DESTSET_MARGIN) {
                                        printf (",\n%*s", DESTSET_MARGIN, "");
                                }
                                printf ("%s@%s [RETRY]\n",
                                        pipe2->v.dest.rqueue, rhostname);
                                printf ("%*s<Unreachable since: %s>\n",
                                        DESTSET_MARGIN+4, "",

                                        fmttime (&pipe2->v.dest.retrytime));
                                printf ("%*s<Next retry at: %s>\n%*s",
                                        DESTSET_MARGIN+4, "",
                                        fmttime (&pipe2->v.dest.retry_at),
                                        DESTSET_MARGIN, "");
                                column = DESTSET_MARGIN;
                            }
                            else {
                                /*
                                 *  The destination is either enabled,
                                 *  or has been marked as failed.
                                 */
                                fldsiz = strlen (pipe2->v.dest.rqueue)
                                       + strlen (rhostname) + 1;
                                if (pipe2->v.dest.status & DEST_FAILED) {
                                        /*
                                         *  This destination has been
                                         *  marked as failed.
                                         */
                                        fldsiz += 9;    /* _[FAILED] */
                                }
                                if (!first) {
                                        putchar (',');
                                        fldsiz += 2;    /*+2 for ', '*/
                                }
                                if (column + fldsiz >= OUTPUT_WIDTH) {
                                        printf ("\n%*s", DESTSET_MARGIN, "");

                                        if (!first) column -= 2;
                                }
                                else {
                                        if (!first) putchar (' ');
                                        column += fldsiz;
                                }
                                printf ("%s@%s", pipe2->v.dest.rqueue,
                                        rhostname);
                                if (pipe2->v.dest.status & DEST_FAILED) {
                                        printf (" [FAILED]");
                                }
                            }    
                            first = 0;          /* Not the first */
                        }
                }
                qmap1 = nextdb (qmapfile);
        }
        if (preamble) printf ("%s;\n",L_BRKT);
}




/*** shgrp
 *
 *
 *      void shgrp():
 *      Display a group name.
 */
static void shgrp (gid)
unsigned long gid;
{
        if (acccolumn >= 4) {
                acccolumn = 0;
                printf ("\n\t\t");
        }
	printf ("%s\t", getgrpnam ((int) gid));
        acccolumn++;
}


/*** shusr
 *
 *
 *      void shusr():
 *      Display a user name.
 */
static void shusr (uid)
unsigned long uid;
{
        if (acccolumn >= 4) {
                acccolumn = 0;
                printf ("\n    ");
        }
        printf ("%s\t", getusenam ((int) uid));
        acccolumn++;
}


/*** shoqacc
 *
 *
 *      void shoqacc():
 *      Display the groups and users allowed to access a queue.
 */
static void shoqacc (file, que_descr)
struct confd *file;
struct gendescr *que_descr;
{
        void shusr();                  /* Show user */

        int fd;                         /* File descriptor */
        unsigned long buffer [QAFILE_CACHESIZ];
        int done;                       /* Boolean */
        int cachebytes;                 /* Try to read this much */
        int bytes;                      /* Did read this much */
        int entries;                    /* Did read this many */
        int i;                          /* Loop variable */


        fd = openqacc (file, que_descr);
        /*
         * Openqacc returns 0 if the queue is restricted and the file
         * is open; -1 if the queue is unrestricted; -2 if the queue
         * queue is restricted but the file is not open, and -3
         * if the queue appears to have been deleted.
         */
        if (fd < -1) {
                printf ("  Failed to open queue access file.\n");
                return;
        }

        if (fd == -1) {                          
                printf ("  Unrestricted access\n");
                return;
        }
        printf ("\tGroups: ");
        acccolumn = 2;
        done = 0;
        cachebytes = QAFILE_CACHESIZ * sizeof (unsigned long);
        while ((bytes = read (fd, (char *) buffer, cachebytes)) > 0) {
                entries = bytes / sizeof (unsigned long);
                for (i = 0; i < entries; i++) {                        
                        /* Zero comes only at the end */

                                                         
                        if (buffer [i] == 0) {

                                               
                                done = 1;

                                          
                                break;

                                       
                        }
                        if ((buffer [i] & MAKEGID) != 0) /* A group */
                          
                                shgrp (buffer [i] & ~MAKEGID);
                }
                if (done) break;                                
        }        
        putchar ('\n');
        lseek (fd, 0, 0);                       /* Back to the beginning */
        printf ("\tUsers: ");
        acccolumn = 2;
        done = 0;                          
        shusr ((unsigned long) 0);             /* Root always has access */

        while ((bytes = read (fd, (char *) buffer, cachebytes)) > 0) {
                entries = bytes / sizeof (unsigned long);
                for (i = 0; i < entries; i++) {
                        /* Zero comes only at the end */
                        if (buffer [i] == 0) {
                                done = 1;
                                break;
                        }
                        if ((buffer [i] & MAKEGID) == 0) /* Not a group */
                                shusr (buffer [i]);
                }
                if (done) break;
        }
        putchar ('\n');
}


/***get_qstate
 *
 * get state of queue
 */
get_qstate(type,status)
short type;
char *status;
{
        if ( type & QUE_ENABLED)
                if ( type & QUE_RUNNING)
                        strcpy(status,"AVAILBL");
                else
                        strcpy(status,"STOPPED");

        if (! (type & QUE_ENABLED))
                if ( !(type & QUE_RUNNING))
                        strcpy(status,"UNAVAIL");
                else
                        strcpy(status,"DISABLED");
        strcat(status,"\0");

}



/*** full_q
 * 
 * show queue in full format
 */
full_q(file,queue,type,qmapfile,pipeqfile,qcomplexfile)
struct confd *file;
struct gendescr *queue;
char * type;
struct confd *qmapfile;
struct confd *pipeqfile;
struct confd *qcomplexfile;
{
	void shoqlims();
#ifdef SDSC
        char  	host[256] ;
        char    status[16];
#else
        char  	host[255] ;
        char    status[8];
#endif
        char    complex[80];
 
	gethostname(host,255);
	host[255] = '\0';

        get_qstate(queue->v.que.status,status);

        printf("\n\n");
        printf("=====================================================\n");
        printf("NQS Version:%d  %s      QUEUE:  %s.%s \t\t status:  %s\n",VERSION,type,               
                queue->v.que.namev.name,host,status);
        printf("=====================================================\n");

        printf("\t\t\t\t\tPriority: %d\n",queue->v.que.priority);

        /* printf("ALIASES: \n");
         *      not supported at this time
         */

        printf("ENTRIES: \n");
        printf("\tTotal:   %d \tRunning: %d \n",
               queue->v.que.queuedcount +
                queue->v.que.runcount +
                queue->v.que.departcount +
                queue->v.que.stagecount +
                queue->v.que.waitcount +
                queue->v.que.arrivecount +
                queue->v.que.holdcount,
                queue->v.que.runcount);
#ifdef SDSC
        printf("\tQueued:  %d \tHeld: \t %d \tTransition: %d \n\n",
#else
        printf("\tQueued:  %d \tHeld: \t %d \tTransiting: %d \n\n",
#endif
                queue->v.que.queuedcount,
                queue->v.que.waitcount +
                queue->v.que.holdcount,
                queue->v.que.arrivecount +
                queue->v.que.departcount +
                queue->v.que.stagecount);

#ifdef SDSC
        printf("RUN_LIMIT: \n");
        printf("\tRunlimit:  %d \n\n", queue->v.que.v1.batch.runlimit);

	printf ("NODE_GROUP: \n");
	printf ("\tNode_group: %d \n\n", queue->v.que.v1.batch.node_group);
#endif
        printf("COMPLEX MEMBERSHIP:\n");
        get_cmplx(queue->v.que.namev.name,qcomplexfile);

	printf("\nRESOURCES:\n");
	shoqlims(queue);  

        printf("\nACCESS\n");
        shoqdesmap(queue,qmapfile,pipeqfile);
	shoqacc(file,queue);

	printf("\n");

}


/*** get_units
 *
 *
 *      char *get_units():
 *      Get quota units.
 */
static char *get_units (units)
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

/*** shoquelims
 *
 *
 *      void shoquelims():
 *      Display the limits associated with a queue.
 */
static void shoqlims (que_descr)
struct gendescr * que_descr;            /* The queue */
{
static char *unlimited = "UNLIMITED";
static char *def = " <DEFAULT>";

#if     (VALID_LIMITS & LIM_PRDRIVES)
        printf ("  Per-request tape drives limit= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PRDRIVES) {
                fputs (unlimited, stdout);
        }
        else printf ("%1d", que_descr->v.que.v1.batch.prdrives);
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PRDRIVES)) {
                fputs (def, stdout);
        }

        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PPCORE)
        printf ("  Per-proc core file size limit= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PPCORE) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s",
                        que_descr->v.que.v1.batch.ppcorecoeff,
                        get_units (que_descr->v.que.v1.batch.ppcoreunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PPCORE)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PPDATA)
        printf ("  Per-process data size limit  = ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PPDATA) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s",
                        que_descr->v.que.v1.batch.ppdatacoeff,
                        get_units (que_descr->v.que.v1.batch.ppdataunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PPDATA)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PPPFILE)
        printf ("  Per-proc perm file size limit= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PPPFILE) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s",
                        que_descr->v.que.v1.batch.pppfilecoeff,
                        get_units (que_descr->v.que.v1.batch.pppfileunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PPPFILE)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PRPFILE)
        printf ("  Per-req perm file space limit= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PRPFILE) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s",
                        que_descr->v.que.v1.batch.prpfilecoeff,
                        get_units (que_descr->v.que.v1.batch.prpfileunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PRPFILE)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PPMEM)
        printf ("  Per-process memory size limit= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PPMEM) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s", que_descr->v.que.v1.batch.ppmemcoeff,
                        get_units (que_descr->v.que.v1.batch.ppmemunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PPMEM)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PRMEM)
        printf ("  Per-request memory size limit= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PRMEM) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s", que_descr->v.que.v1.batch.prmemcoeff,
                        get_units (que_descr->v.que.v1.batch.prmemunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PRMEM)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PPNICE)
        printf ("  Per-proc execution nice value= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PPNICE) {
                fputs (unlimited, stdout);
        }
        else printf ("%1d", que_descr->v.que.v1.batch.ppnice);
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PPNICE)) {
                fputs (def, stdout);
        }
        putchar ('\n');

#endif
#if     (VALID_LIMITS & LIM_PRNCPUS)
        printf ("  Per-req number of cpus limit = ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PRNCPUS) {
                fputs (unlimited, stdout);
        }
        else printf ("%1d", que_descr->v.que.v1.batch.prncpus);
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PRNCPUS)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#ifdef SDSC
#if     (VALID_LIMITS & LIM_PRNCPUS)
        printf ("  Per-user # of requests limit = ");
        if (que_descr->v.que.v1.batch.sPerQueMaxQueuedULimit == 0) {
                fputs (unlimited, stdout);
                fputs (def, stdout);
        }
        else printf ("%1d", que_descr->v.que.v1.batch.sPerQueMaxQueuedULimit);
        putchar ('\n');
#endif
#endif
#if     (VALID_LIMITS & LIM_PPQFILE)
        printf ("  Per-proc quick file size limit= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PPQFILE) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s",
                        que_descr->v.que.v1.batch.ppqfilecoeff,
                        get_units (que_descr->v.que.v1.batch.ppqfileunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PPQFILE)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PRQFILE)
        printf ("  Per-req quick file space limit= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PRQFILE) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s",
                        que_descr->v.que.v1.batch.prqfilecoeff,
                        get_units (que_descr->v.que.v1.batch.prqfileunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PRQFILE)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PPSTACK)
        printf ("  Per-process stack size limit = ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PPSTACK) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s",

                        que_descr->v.que.v1.batch.ppstackcoeff,
                        get_units (que_descr->v.que.v1.batch.ppstackunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PPSTACK)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PPCPUT)
        printf ("  Per-process CPU time limit   = ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PPCPUT) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu.%1d", que_descr->v.que.v1.batch.ppcpusecs,
                        que_descr->v.que.v1.batch.ppcpums);
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PPCPUT)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PRCPUT)
        printf ("  Per-request CPU time limit   = ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PRCPUT) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu.%1d", que_descr->v.que.v1.batch.prcpusecs,
                        que_descr->v.que.v1.batch.prcpums);
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PRCPUT)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PPTFILE)
        printf ("  Per-proc temp file size limit= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PPTFILE) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s",
                        que_descr->v.que.v1.batch.pptfilecoeff,
                        get_units (que_descr->v.que.v1.batch.pptfileunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PPTFILE)) {
                fputs (def, stdout);
        }
        putchar ('\n');

#endif
#if     (VALID_LIMITS & LIM_PRTFILE)
        printf ("  Per-req temp file space limit= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PRTFILE) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s",
                        que_descr->v.que.v1.batch.prtfilecoeff,
                        get_units (que_descr->v.que.v1.batch.prtfileunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PRTFILE)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
#if     (VALID_LIMITS & LIM_PPWORK)
        printf ("  Per-process working set limit= ");
        if (que_descr->v.que.v1.batch.infinite & LIM_PPWORK) {
                fputs (unlimited, stdout);
        }
        else {
                printf ("%1lu %s",
                        que_descr->v.que.v1.batch.ppworkcoeff,
                        get_units(que_descr->v.que.v1.batch.ppworkunits));
        }
        if (!(que_descr->v.que.v1.batch.explicit & LIM_PPWORK)) {
                fputs (def, stdout);
        }
        putchar ('\n');
#endif
}



