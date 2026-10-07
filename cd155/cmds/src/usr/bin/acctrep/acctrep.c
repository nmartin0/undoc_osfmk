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
 
/*****************************************************************************
 *              Copyright (c) 1990 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:        acctrep.c
 *
 * Abstract:    This file contains acctrep, which runs jrec and/or arep
 *              produces various accounting reports
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include "actable.h"
#include "actab_cons.h"
#include "filename.h"

/*===========================================================================*
 * Function:    acctrep
 *
 * Abstract:    This function construct command lines for jrec, sort and
 *		arep and execute them to produce the specified report
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
    extern char *optarg;
    extern int optind, opterr;
    int sflg=0,eflg=0,errflg=0,oflg=0,jflg=0,dflg=0;
    int nflg=0,hflg=0,fflg=0,mflg=0,tflg=0;
    char jrecstr[512],sortstr[256],arepstr[256], *ptr;
    int i, j, jreclen, sortlen, areplen;
    char *site, *fptr, *total_nodes, order[SORT_LEVELS+1];
    char *startdate, *enddate, *down_file;
    int verbose=0;
    char dquote;
    
    dquote = '"';

    /* 
     * parsing command line arguments
     */
    while((c=getopt(argc,argv,"jmtvd:f:h:n:o:s:e:")) != -1) {
        switch(c) {
            case 'd':
                dflg++;
		down_file = (char *)malloc (strlen(optarg)+1);
                (void) strcpy(down_file,optarg);
                break;
            case 'h':
                hflg++;
                site=optarg;
                break;
            case 'n':
                nflg++;
                total_nodes = optarg;
                break;
            case 'o':
		/* before arep -o implementation finishes, use this code */
		(void) fprintf (stderr, "Acctrep:  -o not implemented yet.\n");
		break;
		/* In what order user want the data be sorted, summarized and 
		 * reported?  The valid keys for all the data items are grouped 
		 * into 3 levels for the purpose of sorting:
		 *    account level:
		 *       a       account name
		 *       l       login name
		 *    job level:
		 *       q       queue type
		 *    usage level:
		 *       p       average job-partition size
		 *       c       total cpu-hours
		 *       i       total idle cpu-hours
		 *       u       total under-used cpu-hours
		 *       t       total charge
		 * Only one key from each group can be used at a time to specify
		 * a desired sorting sequence for an accounting report.  
		 * If the leading sort key is "a" or if no sorting sequence is 
		 * specified, then default sort-order is assumed.  The report 
		 * will be sorted by account (sorted by site, within a site is 
		 * sorted by project and within a project is sorted by group), 
		 * then within a project is sorted by login name, within a 
		 * login name is sorted by queue type ... and so on
		 * For non-default sorting sequence, an account-level key and 
		 * an usage-level key is absolutely necessary and the account 
		 * level key must be immediately following an usage level key.  
		 * If the job-level key does not lead the sort sequence, it 
		 * will assume the last position.  If the SORT_ORDER is defined 
		 * to be "clq" or "cl", then the report will show the biggest 
		 * cpu-time consumer in the period first; within an user is 
		 * sorted by account; within an user account is sorted by 
		 * queue type.
		 */
		if (strlen(optarg) < 2) {
		    (void) fprintf (stderr,
			"Acctrep:  Sorting option %s is too short, require at least 2 keys\n", optarg);
		   errflg++;
		    break;
		}
		if (optarg[0] == 'a') break;	/* use default */
		if (optarg[0] == 'l') {
		    (void) fprintf (stderr,
			"Acctrep:  Invalid sorting option %s\n", optarg);
		    errflg++;
		    break;
		}
                if (optarg[0] == 'q') {
		    if (strlen(optarg) < 3) {
			(void) fprintf (stderr,
			    "Acctrep:  -o must specify at least 3 level sorting orders\n");
			errflg++;
			break;
		    }
		    optarg[3] = '\0';
		    ptr = optarg+1;
		}
		else {
		    ptr = optarg;
		}
		for (i=0; i<N_USAGE_KEYS; i++) 
		    if (*ptr == sort_key_grp[USAGE_GROUP][i]) break;
		if (i >= N_USAGE_KEYS) {
		    (void) fprintf (stderr,
			"Acctrep:  Invalid sorting key %c, should be one of the usage key %s\n", 
			*ptr, sort_key_grp[USAGE_GROUP]);
		    errflg++;
		    break;
		}
		ptr++;
		if (*ptr != 'a' && *ptr != 'l') {
		    (void) fprintf (stderr,
			"Acctrep:  Invalid sorting key %c, should be one of the account key %s\n", 
			*ptr, sort_key_grp[ACCT_GROUP]);
		    errflg++;
		    break;
		}
		if (optarg[0] != 'q') { 
		    (void) strncpy (order, optarg, 2);
		    order[2] = 'q';
		    order[3] = '\0';
		}
                else (void) strncpy (order, optarg, 3);
                oflg++;
                break;
            case 'j':
                jflg++;
                break;
            case 'm':
                mflg++;
                break;
            case 't':
                tflg++;
                break;
            case 'v':
                verbose++;
                (void) fprintf (stderr,
                    "Acctrep starts\nAcctrep:  Generate intel usage summary report ...\n");
                break;
            case 's':
                sflg++;
                startdate = optarg;
                break;
            case 'e':
                eflg++;
                enddate = optarg;
                break;
            case 'f':
                fflg++;
                fptr = optarg;
                            if (fptr == NULL) errflg++;
                break;
            default:
                errflg++;
        }
    }
    if (!fflg && (!sflg || !eflg)) {
        (void) fprintf(stderr,
        "The start_date and end_date must be specified.\n"); 
        errflg++;
    }
    if (oflg && jflg) {
        (void) fprintf(stderr,
        "The -j and -o options are mutually exclusive.\n"); 
        errflg++;
    }

#ifdef DEBUG
    (void) fprintf(stderr,"sflg=%d eflg=%d errflg=%d\n", sflg,eflg,errflg);
    (void) fprintf(stderr,"startdate = %s\n",startdate);
    (void) fprintf(stderr,"enddate = %s\n",enddate);
#endif

    if (errflg) {
        (void) fprintf(stderr,"Usage:  \n");
        (void) fprintf(stderr, "acctrep [-jmtv]\n"); 
        (void) fprintf (stderr, "\t[-d down-time-report_fname]\n"); 
        (void) fprintf (stderr, "\t[-h site_name]\n"); 
        (void) fprintf (stderr, "\t[-n total#nodes]\n"); 
/*
        (void) fprintf (stderr, "\t[-o sort_order]\n"); 
*/
        (void) fprintf (stderr, "\t[-s start_date]\n"); 
        (void) fprintf (stderr, "\t[-e end_date]\n"); 
        (void) fprintf (stderr, "\t[-f file_name]\n"); 
        exit(1);
    }

    /* 
     * construct command lines for jrec, sort and arep 
     */
    (void) sprintf(jrecstr,"%s", MACS_JREC_FNAME);     
    jreclen = strlen(jrecstr);
    if (!jflg) {
        (void) sprintf(sortstr,"sort");     
        sortlen = strlen(sortstr);
        (void) sprintf(arepstr,"%s", MACS_AREP_FNAME);     
        areplen = strlen(arepstr);
    }
    if (verbose) {
        (void) sprintf(&jrecstr[jreclen]," -v");
        jreclen += 3;

        if (!jflg) {
            (void) sprintf(&arepstr[areplen]," -v");
            areplen += 3;
        }
    }
    if (jflg) {
            (void) sprintf(&jrecstr[jreclen]," -j");
                jreclen += 3;
    }
    else {
	if (mflg) {
            (void) sprintf(&arepstr[areplen]," -m");
            areplen += 3;
        }
        if (tflg) {
            (void) sprintf(&arepstr[areplen]," -t");
            areplen += 3;
        }
        if (dflg) {
            (void) sprintf(&jrecstr[jreclen]," -d %s", down_file);
            jreclen += strlen(down_file) + 4;
        }
        if (hflg) {
            (void) sprintf(&arepstr[areplen]," -h %c%s%c",
                dquote,site,dquote);
            areplen += strlen(site) + 6;
        }
        if (oflg) {
	    char column_order[SORT_KEYS+1];
	    (void) strncpy(column_order, DEFAULT_SORT, SORT_KEYS+1);
	    for (i=0; i<SORT_LEVELS; i++)
		for (j=0; j<SORT_KEYS; j++)
		    if (order[i] == column_order[j]) {
			if (j>2) (void) sprintf(&sortstr[sortlen],
			    " +%dn -%dr", j, j+1);
			else (void) sprintf(&sortstr[sortlen],
			    " +%d -%d", j, j+1);
			sortlen = strlen(sortstr);
		    }
	    (void) sprintf(&arepstr[areplen]," -o %s",order);
	    areplen += strlen(order) + 4;
	}
    }
    if (nflg) {
        if (!jflg) {
            (void) sprintf(&arepstr[areplen]," -n %s",total_nodes);
            areplen += strlen(total_nodes) + 4;
        }
        (void) sprintf(&jrecstr[jreclen]," -n %s",total_nodes);
        jreclen += strlen(total_nodes) + 4;
    }
    if (sflg) {
        (void) sprintf(&jrecstr[jreclen]," -s %s",startdate);
        jreclen += strlen(startdate) + 4;
    }
    if (eflg) {
        (void) sprintf(&jrecstr[jreclen]," -e %s",enddate);
        jreclen += strlen(enddate) + 4;
    }
    if (fflg) {
        (void) sprintf(&jrecstr[jreclen]," -f %s",fptr);
        jreclen += strlen(fptr) + 4;
    }
    if (!jflg) (void) sprintf(&jrecstr[jreclen]," | %s | %s", sortstr, arepstr);

    if (verbose) (void) fprintf (stderr, "Command string:\n%s\n\n\n",jrecstr);
    system(jrecstr);
    if (verbose) (void) fprintf (stderr, "Acctrep:  --- end acctrep ---\n");
    exit(0);
}
