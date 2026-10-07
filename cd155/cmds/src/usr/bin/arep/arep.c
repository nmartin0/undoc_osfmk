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
 
/******************************************************************************
* Copyright (c) 1990 San Diego Supercomputer Center.
* All rights reserved.  The SDSC software License Agreement
* specifies the terms and conditions for redistribution.
*
* File: arep.c
*
* Abstract:     This file contains routines to format accounting
*		reports for usage summaries
*
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include "filename.h"
#include "nx/nxacct.h"
#include "actable.h"
#include "actab_cons.h"

void newbin();
void newqueue();
void newuser();
void newacct();
void prhead();
void prline2();
void prgrandt();
void prgrand();

FILE *fp = stdin;
FILE *fout=stdout;

char infile[257];

#define INLENGTH 256 

char inputln[INLENGTH+1];
char *acct,*oldacct,login[9],oldlogin[9];
char *acctblank, *acctline1, *acctline2;
int acct_name_len;
int nline=0;
int total_nodes, old_bin, nnodes, oldqtype, qtype;

int lead, *bincnt;
int old_idx, bin_idx;

float bin_cpu,bin_idle,bin_under,bin_chrg;
float *bin_cpu_total,*bin_idle_total,*bin_under_total,*bin_chrg_total;
float que_cpu,que_idle,que_under,que_chrg;
float user_cpu,user_idle,user_under,user_chrg;
float acct_cpu,acct_idle,acct_under,acct_chrg;

char startdate[9],enddate[9];
time_t starttime,endtime;
char starthms[9],endhms[9];

int verbose=0, tflg=0, mflg=0;
struct actable *actab;

/*===========================================================================*
 * Function:    arep
 *
 * Abstract:    This program takes sorted output from jrep and produces an
 *              usage summary report.
 *
 * Arguments:   argc
 *		argv
 *
 * Return value:
 *              None
 *
 * Notes:
 *===========================================================================*/

main(argc,argv)
int argc;
char *argv[];
{
    int c;
    int fflg=0,errflg=0;
    int hflg=0, nflg=0, acflg=0;
    int i, j, fplen, temp; 
    char *acct_name_buf, *hsite, buf[128], *order;
    float cpu_time, idle_time, under_time, cpu_charge;
    extern char *optarg, *malloc(), *calloc(), *strncpy();
    extern int optind, opterr;

    /* 
     * Initialize 
     */
    actab = get_actab();
    if (actab == NULL) {
        (void) fprintf (stderr, "Fail in reading %s file\n", MACS_ACTABLE_FILE);
            exit(-1);
    }
    if (actab->n_actlevel > 0)
        for (i=0, acct_name_len=0; i<actab->n_actlevel; i++)
            acct_name_len += actab->actlevel[i];

    /* 
     * 	column heading for account name, format and print 
     */
    acct_name_buf = (char *)malloc (acct_name_len +1);
    (void) strncpy (acct_name_buf, "account name", acct_name_len);
    for (i=12; i<acct_name_len; i++) acct_name_buf[i] = ' ';
    acct_name_buf[acct_name_len] = '\0';

    /* 
     * account name buffer for input and comparison 
     */
    oldacct = (char *)malloc (acct_name_len*2);
    acct = (char *)malloc (acct_name_len*2);

    /* 
     * account bland/-/= lines for format and print 
     */
    acctblank = (char *)malloc (acct_name_len+1);
    for (i=0; i<acct_name_len; i++) acctblank[i]=' ';
    acctblank[acct_name_len] = '\0';
    acctline1 = (char *)malloc (acct_name_len+1);
    for (i=0; i<acct_name_len; i++) acctline1[i]='-';
    acctline1[acct_name_len] = '\0';
    acctline2 = (char *)malloc (acct_name_len+1);
    for (i=0; i<acct_name_len; i++) acctline2[i]='-';
    acctline2[acct_name_len] = '\0';

    /* 
     * parsing command line arguments 
     */
    while((c=getopt(argc,argv,"mtvn:h:f:o:")) != -1) {
        switch(c) {

            case 'h':
                hflg++;
		hsite = (char *)malloc (strlen(optarg) + 1);
                (void) strcpy (hsite,optarg);
                break;

            case 'n':
                nflg++;
                total_nodes = atoi(optarg);
                break;

            case 'm':
                mflg++;
                break;

            case 'o':
		/* before implementation finish, use this code */
		(void) fprintf (stderr, "Arep:  -o not implemented yet.\n");
		(void) fflush (stderr);
		break;

            case 't':
                tflg++;
                break;

            case 'v':
                verbose++;
                (void) fprintf (stderr,
                    "Arep starts\nArep:  Parsing arguments ...\n");
		(void) fflush (stderr);
                break;

            case 'f':
                fflg++;
                strncpy(infile,optarg,256);
                if ((fplen=strlen(optarg)) > 256) {
                    (void) fprintf(stderr, "Arep:  File name too long\n");
		    (void) fflush (stderr);
                    errflg++;
                }
                else 
                    infile[fplen] = '\0';    
                break;

            default:
                errflg++;
        }
    }

#ifdef DEBUG
    (void) fprintf(stderr, "fflg=%d errflg=%d\n", fflg,errflg);
    (void) fflush (stderr);
#endif

    if (errflg) {
        (void) fprintf(stderr,"Usage:  \n");
        (void) fprintf(stderr, "arep [-mtv]\n");
        (void) fprintf (stderr, "\t[-h site_name]\n"); 
        (void) fprintf (stderr, "\t[-n total#nodes]\n"); 
        (void) fprintf (stderr, "\t[-f file]\n");
        exit(1);
    }

    if (verbose) (void) fprintf (stderr, "Arep:  Initialize ...\n");

    /*
     * open input data file
     */
    if (fflg) {
        if ((fp=fopen(infile,"r"))==NULL) {
            (void) fprintf(stderr, "Arep:  Error opening jrec file %s\n",infile);
            exit(1);
        }
    }

    /*
     * get or set bins for partition sizes, allocate accumulators for
     * grant total of each bin
     */
    if (!nflg) {
        if (actab->n_bins > 0) total_nodes = actab->bins[actab->n_bins-1];
        else {
            (void) fprintf (stderr,
                "Total number of nodes unknown, use -n option\n");
            exit(1);
        }
	bincnt = actab->bins;
    }
    else {
        for (i=1, temp=1; (temp*=2) < total_nodes; i++);
	actab->n_bins = i+1;
        bincnt = (int *) malloc (actab->n_bins*sizeof(int));
        for (i=0, temp=1; i<actab->n_bins; temp*=2,i++) bincnt[i] = temp;
    }
    bin_cpu_total = (float *) calloc (actab->n_bins, sizeof(float));
    bin_idle_total = (float *) calloc (actab->n_bins, sizeof(float));
    bin_under_total = (float *) calloc (actab->n_bins, sizeof(float));
    bin_chrg_total = (float *) calloc (actab->n_bins, sizeof(float));
#ifdef DEBUG
    (void) fprintf (stderr, "total_nodes=%d\n", total_nodes);
    for (i=0; i<actab->n_bins; i++)
        (void) fprintf (stderr, "bincnt[%d]=%d\n", i, bincnt[i]);
    (void) fflush (stderr);
#endif

    /*
     * print headers
     */
    if (verbose) (void) fprintf (stderr, "Arep:  Output headers ...\n");
    if (hflg) (void) fprintf(fout, "%s Intel PARAGON Usage Summary Report\n", 
	hsite);
    else (void) fprintf(fout, "Intel PARAGON Usage Summary Report\n"); 
    {
    time_t now = time(0);
    (void) fprintf(fout, "- produced on %s\n\n", ctime(&now));
    }

    prhead(); 

    if (verbose) (void) fprintf (stderr,
        "Arep:  Process summary records ...\n");
    (void) fflush (stderr);

    /*
     * parse the first job line and set all counters
     */
    if (sscanf(inputln,"%s %s %d %d %f %f %f %f",
        oldacct,oldlogin,&oldqtype,&old_bin,&cpu_time,&idle_time,&under_time,
        &cpu_charge) == EOF) {
        (void) fprintf(stderr,
            "Arep:  Error reading input line number %d\n%s\n",
            nline, inputln);
        exit(1);
    }

    /* 
     * fill oldacct buffer to max length with blanks, for print format 
     */
    for (i=strlen(oldacct); i<acct_name_len; i++) oldacct[i] = ' ';
    oldacct[acct_name_len] = '\0';

    /* 
     * unit is minutes instead of hours 
     */
    if (mflg) {
	cpu_time*=60.;
	idle_time*=60.;
	under_time*=60.;
    }

    /* 
     * which bin the partition size belongs to 
     */
    for (i=0; i<actab->n_bins; i++) if (old_bin <= bincnt[i]) break;
    if (i>=actab->n_bins) i=actab->n_bins-1;
    old_bin = bincnt[i];
    old_idx = bin_idx = i;

    /* 
     * print column headings 
     */
    (void) fprintf(fout, "%s login    queue", acct_name_buf);
    if (mflg) (void) fprintf(fout,
        "    size  cpu minutes idle minutes  under-used   cpu charge\n");   
    else (void) fprintf (fout,
        "    size   cpu hours   idle hours   under-used   cpu charge\n");   
    (void) fprintf(fout, "%s ======== ======== ==== ============ ",acctline2);
    (void) fprintf(fout, "============ ============ =============\n");


    /*
     * process one line at a time and print summaries
     */
    for (lead=3; fgets(inputln,INLENGTH,fp); nline++) {
	/* 
	 * update the accumulators
	 */
	bin_cpu += cpu_time;
	bin_idle += idle_time;
	bin_under += under_time;
	bin_chrg += cpu_charge;
	bin_cpu_total[bin_idx] += cpu_time;
	bin_idle_total[bin_idx] += idle_time;
	bin_under_total[bin_idx] += under_time;
	bin_chrg_total[bin_idx] += cpu_charge;
	que_cpu += cpu_time;
	que_idle += idle_time;
	que_under += under_time;
	que_chrg += cpu_charge;
	user_cpu += cpu_time;
	user_idle += idle_time;
	user_under += under_time;
	user_chrg += cpu_charge;
	acct_cpu += cpu_time;
	acct_idle += idle_time;
	acct_under += under_time;
	acct_chrg += cpu_charge;

#ifdef DEBUG
        (void) fprintf(stderr, "inputln:\n%s\n",inputln);   
        (void) fflush (stderr);
#endif
        /*
         * parse the line
         */
        if (sscanf(inputln,"%s %s %d %d %f %f %f %f",
            acct,login,&qtype,&nnodes,&cpu_time,
            &idle_time,&under_time,&cpu_charge) == EOF) {
            (void) fprintf(stderr,
                "Arep:  Error reading input line number %d\n%s\n",
                nline, inputln);
            exit(1);
        }

	/* 
	 * fill oldacct buffer to max length with blanks, for print format
	 */
	for (i=strlen(acct); i<acct_name_len; i++) acct[i] = ' ';
	acct[acct_name_len] = '\0';

#ifdef DEBUG
        (void) fprintf(stderr,"%s %s %d %d %f %f %f %f\n",
            acct,login,qtype,nnodes,cpu_time,
            idle_time,under_time,cpu_charge);
        (void) fflush (stderr);
#endif
	/*
	 * minutes instead of hours
	 */
        if (mflg) cpu_time*=60.;
    
	/*
	 * which bin the partition size belongs to
	 */
	for (i=0; i<actab->n_bins; i++) if (nnodes <= bincnt[i]) break;
	if (i>=actab->n_bins) i=actab->n_bins-1;
	nnodes = bincnt[i];
	bin_idx = i;

	/*
	 * new account ?
	 */
        if (strcmp(acct,oldacct) != 0) {
#ifdef DEBUG
            (void) fprintf (stderr, "new acct %s\n", acct);
            (void) fflush (stderr);
#endif
            newbin();
            newqueue();
            newuser();
            newacct();
	    lead = 3;
            continue;
        }
        
	/*
	 * new user ?
	 */
        if (strcmp(login,oldlogin) != 0) {
#ifdef DEBUG
            (void) fprintf (stderr, "new login %s\n", login);
            (void) fflush (stderr);
#endif
            newbin();
            newqueue();
            newuser();
	    lead = 2;
            continue;
        }

	/*
	 * new queue type ?
	 */
        if (qtype != oldqtype) {
#ifdef DEBUG
            (void) fprintf (stderr, "new queue %d\n", qtype);
            (void) fflush (stderr);
#endif
            newbin();
            newqueue();
	    lead = 1;
            continue;
        }

	/*
	 * new bin size ?
	 */
        if (nnodes != old_bin) {
#ifdef DEBUG
            (void) fprintf (stderr, "new nnodes %d\n", nnodes);
            (void) fflush (stderr);
#endif
            newbin();
            continue;
        }

    }

    /* 
     * the last line: update the accumulators 
     */
    bin_cpu += cpu_time;
    bin_idle += idle_time;
    bin_under += under_time;
    bin_chrg += cpu_charge;
    bin_cpu_total[bin_idx] += cpu_time;
    bin_idle_total[bin_idx] += idle_time;
    bin_under_total[bin_idx] += under_time;
    bin_chrg_total[bin_idx] += cpu_charge;
    que_cpu += cpu_time;
    que_idle += idle_time;
    que_under += under_time;
    que_chrg += cpu_charge;
    user_cpu += cpu_time;
    user_idle += idle_time;
    user_under += under_time;
    user_chrg += cpu_charge;
    acct_cpu += cpu_time;
    acct_idle += idle_time;
    acct_under += under_time;
    acct_chrg += cpu_charge;
    newbin();
    newqueue();
    newuser();
    newacct();

    /*
     * print grand totals
     */
    (void) fprintf(fout,"\n%s %s to %s %s\n", startdate,starthms,enddate,endhms);
    prgrandt();

    if (fflg && fclose(fp) == EOF && verbose)
        (void) fprintf (stderr, 
            "Arep:  Could not close file %s\n",infile);
    if (verbose) (void) fprintf (stderr,
        "Arep:  --- end arep ---\n");

}

/*===========================================================================*
 * Function:    newbin
 *
 * Abstract:    This function print a total line for the old bin size
 *		This is the most detailed level of output for the
 *		summary report
 *
 * Arguments:   None
 *
 * Return value:
 *              None
 *
 * Notes:	lead == 3	all columns need to be printed
 *			2	ommit account name
 *			1	ommit account name and user name
 *			0	ommit account name, user name and queue type
 *===========================================================================*/
void newbin()
{
    int i;
#ifdef DEBUG
    (void) fprintf(stderr, "Enter newbin()\n");
#endif

    if (lead == 3)
        (void) fprintf(fout, 
	    "%s %-8.8s %-8.8s %4d %12.3f %12.3f %12.3f %13.3f\n",
            oldacct,oldlogin,qtype_name[oldqtype],old_bin,
	    bin_cpu,bin_idle,bin_under,bin_chrg);
    else if (lead == 2)
        (void) fprintf(fout, 
	    "%s %-8.8s %-8.8s %4d %12.3f %12.3f %12.3f %13.3f\n",
            acctblank,oldlogin,qtype_name[oldqtype],old_bin,
	    bin_cpu,bin_idle,bin_under,bin_chrg);
    else if (lead == 1)
        (void) fprintf(fout, 
	    "%s %-8.8s %-8.8s %4d %12.3f %12.3f %12.3f %13.3f\n",
            acctblank," ",qtype_name[oldqtype],old_bin,
	    bin_cpu,bin_idle,bin_under,bin_chrg);
    else if (lead == 0)
        (void) fprintf(fout, "%s %17s %4d %12.3f %12.3f %12.3f %13.3f\n",
            acctblank," ",old_bin,bin_cpu,bin_idle,bin_under,bin_chrg);
    else (void) fprintf (stderr, "Newbin(): Internal error - lead=%d\n",lead);

    lead = 0;
    bin_cpu = bin_idle = bin_under = bin_chrg = 0.0;
    old_bin = nnodes;
    old_idx = bin_idx;
#ifdef DEBUG
    (void) fprintf(stderr, "Exit newbin()\n");
#endif
}

/*===========================================================================*
 * Function:    newqueue
 *
 * Abstract:    This function print a total line for the old queue type
 *
 * Arguments:   None
 *
 * Return value:
 *              None
 *
 *===========================================================================*/
void newqueue()
{
    int i;
#ifdef DEBUG
    (void) fprintf(stderr, "Enter newqueue()\n");
#endif
    
    (void) fprintf(fout, 
	"%s          ------------------------------------------------------------------\n",
	acctblank);
    (void) fprintf(fout, 
	"%s %-8.8s %-8.8s total%12.3f %12.3f %12.3f %13.3f\n\n",
        acctblank," ",qtype_name[oldqtype],que_cpu,
	que_idle, que_under,que_chrg);

    que_cpu = que_idle = que_under = que_chrg = 0;
    oldqtype = qtype;
#ifdef DEBUG
    (void) fprintf(stderr, "Exit newqueue()\n");
#endif
}

/*===========================================================================*
 * Function:    newuser
 *
 * Abstract:    This function print a total line for the old user
 *
 * Arguments:   None
 *
 * Return value:
 *              None
 *
 *===========================================================================*/
void newuser()
{
#ifdef DEBUG
    (void) fprintf(stderr, "Enter newuser()\n");
#endif
    
    (void) fprintf(fout, 
	"%s ---------------------------------------------------------------------------\n",
	acctblank);
    (void) fprintf(fout, 
	"%s %-8.8s          total%12.3f %12.3f %12.3f %13.3f\n\n",
        acctblank,oldlogin,user_cpu,user_idle,user_under,user_chrg);

    user_cpu = user_idle = user_under = user_chrg = 0;
    (void) strncpy (oldlogin, login, 8);
#ifdef DEBUG
    (void) fprintf(stderr, "Exit newuser()\n");
#endif
}

/*===========================================================================*
 * Function:    newacct
 *
 * Abstract:    This function print a total line for the old account
 *
 * Arguments:   None
 *
 * Return value:
 *              None
 *
 *===========================================================================*/
void newacct()
{
#ifdef DEBUG
    (void) fprintf(stderr, "Enter newacct()\n");
#endif

    (void) fprintf(fout, 
	"%s----------------------------------------------------------------------------\n",
	acctline1);
    (void) fprintf(fout, 
	"%s                   total%12.3f %12.3f %12.3f %13.3f\n\n",
        oldacct,acct_cpu,acct_idle,acct_under,acct_chrg);

    acct_cpu = acct_idle = acct_under = acct_chrg = 0;
    (void) strncpy (oldacct, acct, acct_name_len);
#ifdef DEBUG
    (void) fprintf(stderr, "Exit newacct()\n");
#endif
}

                
/*===========================================================================*
 * Function:    prgrandt
 *
 * Abstract:    This function print grand total lines
 *
 * Arguments:   None
 *
 * Return value:
 *              None
 *
 *===========================================================================*/
void prgrandt()
{
    long aperc,tperc;
    float tusedcpu,tcputime,tcpuchrg;
    int i;

    /*
     * total wall-clock seconds of all the cpu's
     */
    tcputime = ((float)endtime - starttime) * total_nodes;

    /*
     * convert to minutes or hours
     */
    if (mflg) tcputime /= 60.;
    else tcputime /= 3600.;

    /*
     * add-up total cpu-time used and total cpu-charges
     */
    for (i=tusedcpu=tcpuchrg=0;i<actab->n_bins;i++) {
	tcpuchrg += bin_chrg_total[i];
	tusedcpu += bin_cpu_total[i];
    }

#ifdef DEBUG
    (void) fprintf (stderr, "prgrandt:  tcputime = %f\n",tcputime);
    (void) fprintf (stderr, "prgrandt:  tcpuchrg = %f\n",tcpuchrg);
    (void) fprintf (stderr, "prgrandt:  tusedcpu = %f\n",tusedcpu);
#endif
            
    /*
     * print grand totals for each bin size
     */
    (void) fprintf(fout, "\n              size");
    if (mflg)
        (void) fprintf(fout," cpu minutes    total charge %%_distr %%_usage\n");   
    else
        (void) fprintf(fout,"   cpu hours    total charge %%_distr %%_usage\n");   
    (void) fprintf(fout, "============= ==== ============= ============= ======= =======\n");

    for (i=0;i<actab->n_bins;i++) {
        tperc = bin_cpu_total[i]/tusedcpu * 10000.;
        aperc = bin_cpu_total[i]/tcputime * 10000.;

        (void) fprintf(fout,
	    "GRAND TOTAL   %4d %13.3f %13.3f  %3.3d.%2.2d  %3.3d.%2.2d\n",
            bincnt[i],bin_cpu_total[i],bin_chrg_total[i],tperc/100,tperc%100,
            aperc/100,aperc%100);
    }

    /*
     * print final totals
     */
    aperc = tusedcpu/tcputime * 10000.;

    (void) fprintf(fout,
    "\nFINAL TOTAL        %13.3f %13.3f  100.00  %3.3d.%2.2d\n",
        tusedcpu,tcpuchrg,aperc/100,aperc%100);

}

/*===========================================================================*
 * Function:    prhead
 *
 * Abstract:    This function reads in all the lines in the beginning of
 *		the input data which contains all the timing information, 
 *		then compute and print as part of the report heading
 *
 * Arguments:   None
 *
 * Return value:
 *              None
 *
 * Notes:
 *===========================================================================*/
void prhead()
{
    int sched_boot, unsched_boot, si_cnt, ucount;
    int days, hours, minutes;
    int psched_boot, punsched_boot, psi_cnt, p_ucount;
    time_t downtime, uptime, remain, sched_down; 
    time_t p_starttime, p_endtime, p_downtime, p_scheddown;
    char p_startdate[16], p_starthms[16]; 
    char p_enddate[16], p_endhms[16];
    char *rval;

    /*
     * read and process one line at a time
     */
    for ( nline=1; 
        (rval=fgets(inputln,INLENGTH,fp)) && inputln[0]=='('; nline++) {

#ifdef DEBUG
	(void) fprintf(stderr,"inputln=%s\n",inputln);
#endif
        if (sscanf(inputln,
            "(%d) %s %s to (%d) %s %s - down %d seconds, scheduled down %d seconds, scheduled %d, reboot %d; unscheduled reboot %d.  Users %d",
            &p_starttime, p_startdate, p_starthms, &p_endtime, 
            p_enddate, p_endhms, &p_downtime, &p_scheddown,
            &psi_cnt, &psched_boot, &punsched_boot, &p_ucount) == EOF) {
            (void) fprintf(stderr,
                "Arep:  Error reading input line number %d\n%s\n",
                nline,inputln);
            exit(1);
        }
        if (nline==1) {
            starttime = p_starttime;
            (void) strcpy(startdate, p_startdate);
            (void) strcpy(starthms, p_starthms);
            endtime = p_endtime;
            (void) strcpy(enddate, p_enddate);
            (void) strcpy(endhms, p_endhms);
                        si_cnt = psi_cnt;
                        sched_boot = psched_boot;
                        unsched_boot = punsched_boot;
            downtime = p_downtime;
            sched_down = p_scheddown;
            ucount = p_ucount;
        }
        else {
            if (endtime < p_starttime) {
                (void) fprintf (stderr,
                    "Arep: No data from %s %s to %s %s\n",
                    enddate, endhms, 
                    p_startdate, p_starthms);
            }
            else if (endtime > p_starttime) {
                (void) fprintf (stderr,
                    "Arep: %s %s to %s %s double recorded\n",
                    p_startdate, p_starthms,
                    enddate, endhms); 
            }
            endtime = p_endtime;
            (void) strcpy(enddate, p_enddate);
            (void) strcpy(endhms, p_endhms);
                        si_cnt += psi_cnt;
                        sched_boot += psched_boot;
                        unsched_boot += punsched_boot;
            downtime += p_downtime;
            sched_down += p_scheddown;
            ucount += p_ucount;
        }

#ifdef DEBUG
        (void) fprintf(stderr,"startdate=%s\n",startdate);
        (void) fprintf(stderr,"starthms=%s\n",starthms);
        (void) fprintf(stderr,"enddate=%s\n",enddate);
        (void) fprintf(stderr,"endhms=%s\n",endhms);
        (void) fprintf(stderr,"starttime=%d\n",starttime);
        (void) fprintf(stderr,"endtime=%d\n",endtime);
        (void) fprintf(stderr,"downtime=%d\n",downtime);
#endif
    }

    if (nline == 1 && rval > NULL) {
        (void) fprintf (stderr, "Arep:  Error in getting time data lines from jrec\n");
        exit(1);
    }
    if (rval == NULL) {
        (void) fprintf (stderr, "Arep:  No data to be processed, exit\n");
        exit(0);
    }
    uptime = endtime - starttime - downtime;
    if (endtime == starttime) {
        (void) fprintf (stderr,
            "Arep:  Endtime = starttime, no reports\n");
        exit (0);
    }
    (void) fprintf(fout,"\n%s %s to %s %s\n",
        startdate,starthms,enddate,endhms);
    (void) fprintf (fout, "System Up %5.2f%c  (",
        (float) uptime*100/(endtime-starttime), '%');
    remain = uptime;
    if (days = remain / (24*60*60)) {
        remain -= 24*60*60*days;
        (void) fprintf (fout, " %d days", days);
    }
    if (hours = remain / 3600) {
        remain -= 3600*hours;
        (void) fprintf (fout, " %d hours", hours);
    }
    if (minutes = remain / 60) {
        remain -= 60*minutes;
        (void) fprintf (fout, " %d minutes", minutes);
    }
    if (remain) 
        (void) fprintf (fout, " %d seconds", remain);
    (void) fprintf (fout, " )\n");
    (void) fprintf (fout, "System Down %3.2f%c  (",
        (float) downtime*100/(endtime-starttime), '%');
    remain = downtime;
    if (days = remain / (24*60*60)) {
        remain -= 24*60*60*days;
        (void) fprintf (fout, " %d days", days);
    }
    if (hours = remain / 3600) {
        remain -= 3600*hours;
        (void) fprintf (fout, " %d hours", hours);
    }
    if (minutes = remain / 60) {
        remain -= 60*minutes;
        (void) fprintf (fout, " %d minutes", minutes);
    }
    if (remain) 
        (void) fprintf (fout, " %d seconds", remain);
    (void) fprintf (fout, " )\n");
        (void) fprintf (fout, "Scheduled Interrupts:  %d\n",
                                si_cnt);
        (void) fprintf (fout, "Scheduled Reboots:  %d\n",
                                sched_boot);
        (void) fprintf (fout, "Scheduled Down Time:  ");
    remain = sched_down;
    if (days = remain / (24*60*60)) {
        remain -= 24*60*60*days;
        (void) fprintf (fout, " %d days", days);
    }
    if (hours = remain / 3600) {
        remain -= 3600*hours;
        (void) fprintf (fout, " %d hours", hours);
    }
    if (minutes = remain / 60) {
        remain -= 60*minutes;
        (void) fprintf (fout, " %d minutes", minutes);
    }
    if (remain) 
        (void) fprintf (fout, " %d seconds", remain);
    (void) fprintf (fout, "\n");
    (void) fprintf (fout, "Unscheduled Reboots:  %d\n", unsched_boot);
    (void) fprintf (fout, "Total number of users:  %d\n\n\n", ucount);
}
