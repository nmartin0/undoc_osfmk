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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *      This software is supplied under the terms of a license
 *      agreement or nondisclosure agreement with Intel Corpo-
 *      ration and may not be copied or disclosed except in
 *      accordance with the terms of that agreement.
 *
 */
/* macs_lib.c
 *
* $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/macs_lib.c,v $
 *
 * DESCRIPTION:
 *
 *	The module contains a number of general subroutines for the 
 *	 buddy system. 
 *
 *	Developer:
 *	----------
 *
 *	Michael Wan of San Diego Supercomputer Center.
 *
 *
 *
 */
/*
 * HISTORY
 * $Log: macs_lib.c,v $
 * Revision 1.13  1995/03/21  22:10:41  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 10507
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/include/nqs/buddy.h
 * 	cmds_libs/src/usr/include/nqs/buddyvar.h
 * 	cmds_libs/src/usr/lib/nqs/macs_lib.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_spawn.c
 * 	cmds_libs/src/usr/lib/nqs/smd_msg.c
 *
 * Revision 1.12  1995/03/17  18:20:44  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 9765
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/include/nqs/buddy.h
 *    cmds_libs/src/usr/include/nqs/buddyvar.h
 *    cmds_libs/src/usr/lib/nqs/macs_lib.c
 *    cmds_libs/src/usr/lib/nqs/nqs_bsc.c
 *    cmds_libs/src/usr/lib/nqs/nqs_vtimer.c
 *    cmds_libs/src/usr/lib/nqs/macs_sched.c
 *    cmds_libs/src/usr/lib/nqs/macs_rootp.c
 *
 * Revision 1.11  1994/11/19  02:52:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/10/07  00:51:44  kremenek
 *  Reviewer: davidl doyle
 *  Risk: Low
 *  Benefit or PTS #: 9252 8997
 *  Testing: EATS
 *  Module(s): cmds_libs/src/usr/lib/nqs/macs_lib.c,cmds_libs/src/usr/lib/nqs/nqs_upp.c
 *
 * Revision 1.9  1994/08/31  20:23:23  bradf
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
 * Revision 1.7.2.1  1994/08/05  20:37:19  kremenek
 *  Reviewer: George Kremenek SDSC
 *  Risk: Low
 *  Benefit or PTS #: 6589 9491 9950 10355
 *  Testing: EATS passed
 *  Module(s):
 *
 * Revision 1.8  1994/08/05  17:15:09  mwan
 *  Reviewer: George Kremenek
 *  Risk: Low
 *  Benefit or PTS #: 6589 9491 9950 10355
 *  Testing: EATS
 *  Module(s):
 *
 * Revision 1.6.2.4  1994/04/26  21:54:01  mwan
 * Fixed 8230, 8802, 6877 and 9010.
 *
 * Reviewer: kremenek
 *  Risk: Medium
 *  Benefit or PTS #: 8230, 8802, 6877 and 9010.
 *  Testing:
 *  Module(s): usr/lib/nqs/macs_rootp.c netdaemon.c nqs_pip.c nqs_reqser.c
 * 	    ccs/lib/libnqs/listq.c
 *
 * Revision 1.6.2.3  1994/03/30  02:01:12  mwan
 * Fixed pts 8088, 8597
 *
 *  Reviewer: kremenek
 *  Risk: L
 *  Benefit or PTS #: 8088, 8597
 *  Testing:
 *  Module(s): usr/lib/nqs/nqs_spawn.c, usr/lib/nqs/macs_lib.c,
 * 	    usr/lib/nqs/res_msg.c.
 *
 * Revision 1.6.2.2  1994/03/23  17:51:05  mwan
 * Fixed pts 8599,8576,8600,8601.
 *
 *  Reviewer: kremenek
 *  Risk: L
 *  Benefit or PTS #: 8599,8576,8600,8601
 *  Testing:
 *  Module(s): usr/lib/nqs/macs_rootp.c, usr/include/nqs/buddy.h,
 * 	    usr/lib/nqs/macs_lib.c, usr/lib/nqs/nqs_spawn.c,
 * 	    usr/lib/nqs/macs_sched.c, usr/lib/nqs/macs_job.
 *
 * Revision 1.6.2.1  1994/03/16  22:45:23  mwan
 * Fixed 8346 and 8325
 *
 *  Reviewer: kremenek
 *  Risk: L
 *  Benefit or PTS #: 8346 and 8325
 *  Testing:
 *  Module(s):usr/ccs/lib/libnqs/listq.c, usr/ccs/lib/libnqs/main_dsp.c
 *                    usr/lib/nqs/nqs_spawn.c, usr/lib/nqs/macs_rootp.c,
 *                    usr/lib/nqs/macs_lib.c, usr/lib/nqs/macs_sched.c
 *
 * Revision 1.6  1993/11/02  00:56:43  mwan
 * R1.2 mods
 *
 * Revision 1.5  1993/09/07  20:48:51  shala
 * Fixed bug #6360 by jkearns.
 *
 * Revision 1.4.6.1  1993/09/07  18:08:55  shala
 * Fixed bug #6360. Fixed by jkearns.
 *
 * Revision 1.4  1993/07/13  17:52:39  mwan
 * T11 - fixed PTS 5022
 *
 * Revision 1.3  1992/12/16  23:25:59  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  22:24:30  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 *
 */

#ifdef SDSC
#define TIMESTRING (128)
#define BASE_PART_PATH	".compute."	/* base part path */

#include <stdio.h>
#include <fcntl.h>                      /* File control */
#include <pwd.h>                        /* File control */
#include <time.h>                       /* File control */
#include <signal.h>                     /* File control */
#include <errno.h>
#include "nqs.h"
#include "nqsxvars.h"
#include "buddyxvar.h"

/* External functions */

extern int init_job_req ();
extern void init_que ();
extern int release_job ();
extern int add_job ();
extern int rel_blkinjob ();
extern struct job_req *get_job ();
extern void prt_job ();
extern int rem_job ();
extern int rem_job_ts ();
extern struct job_req *find_job_by_name ();
extern void release_all ();
extern int set_tsnodes ();
extern struct job_req *find_job ();
extern void release_all ();

int get_mode ();
int read_param ();
int smail ();
int read_box ();
int power2 ();
char *timstr();
void chk_nxaccounts ();
void prt_param ();
int init_allgrp ();
int init_grp ();

/* get_mode ()
 *
 * Subroutine to get the scheduling mode.
 *
 * input :
 *		time
 * output :
 *		mode
 *
 */
int get_mode (time)
long time;
{

	if (time >= glb.p_start && time < glb.p_end) {
		return (PRIME_M);
	} else {
		return (NPRIME_M);
	}
}


/*** read_param
 *
 * Subroutine to read input parameters
 *
 * return value :
 *	0 - no error.
 *	1 - param file does not exist
 *     -1 - input error
 */

int read_param ()
{
	FILE *param_fp;
	char key_buf[NAME_LEN];
	char buf[INSIZE];               /* buffer to read in param data */

	char *bp;                 /* temp buffer pointer */
	register int i, j;                    /* temp index */
	long temp;
	int read_flag[N_KEYWD];   /* flag to indicate reading of keywd data */
	int comment_flag;
	register c;
	int got_colon;
	int got_digit;
	int in_len;
	int space_last;		/* last char is a space */
	int num_entry;		/* number of entries */
	char tmp_part_name[PART_NAME_LEN];
	char tmp_part_name1[PART_NAME_LEN];
	char tmp_nodef[NAME_LEN];	/* temp nodef */
	int tmp_inx, set_inx;
	int tmp_flag[MAX_SET];
	float tmp_float[DAYS_PER_W];

	/* open the param file */

	if ((param_fp = fopen (PARAM_FILE , "r")) == NULL) {
		printf ("I$Unable to open param file. \n");
		fflush (stdout);
		return (1);
	}

        if (glb.param_read == 0) {
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

		for (i = 0; i < MAX_GROUP; i++) {
		    node_grp[i].num_set = 0;
		    for (j = 0; j < MAX_SET; j++) {
		   	node_grp[i].time_flag[j] = 0;
		    }
		}
	}

	for (i = 0;i < N_KEYWD;i++) 
		read_flag[i] = 0;

	/* read the data */

	while (1) {

		/* read 1 line at a time */

		buf[0] = '\0';
		key_buf[0] = '\0';
		tmp_part_name[0] = '\0';

		bp = buf;
		comment_flag = 0;
		space_last = 1;
		got_colon = 0;	/* replace only the first colon */
		got_digit = 0;	/* replace only the first colon */
		num_entry = 0;
		in_len = 0;	/* input length in bytes */

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
				*bp ++= ' ';	/* put a blank before the */
						/* first digit */
				space_last = 1;
				got_digit = 1;
			}
			if (c == '\t')
			    c = ' ';
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
				    printf ("I$read_param: Problem with the");
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
		for (i = 0; i < N_KEYWD; i++) {
			if (strcmp (key_wd[i], key_buf) == 0)
				break;
		}
		if (i >= N_KEYWD) {
			printf ( "I$unmatched keyword :%s\n",key_buf);
			fflush (stdout);
			continue;
		}

		/* only prime_list, nprime_list, node_set and node_group
		 * can be followed by index.
		 */

		if (tmp_inx >= 0 && i != 5 && i != 6 && i != 7 && i != 8) {
		    printf ("I$read_param: Problem with the");
		    printf (" following input line : %s\n", buf);
		    return (1);
		}
		switch (i) {
			case 0 :       /* time zone */
				strcpy (TIME_ZONE, "TZ=");
				bp = TIME_ZONE + 3;
				temp = sscanf (buf, "%s%s", key_buf, 
				bp);
				if (strlen (bp) >= TZ_LEN -3) {
				    printf ("I$read_param:TIME_ZONE length ");
				    printf ("too long. Must be < %s\n",
				    TZ_LEN - 3);
				    return (1);
				} 
				if (strncmp ("TZ=", bp, 3) == 0) {
				    bp = TIME_ZONE;
				    temp = sscanf (buf, "%s%s", key_buf, bp);
				}
				read_flag[i] = 1;
				break;
			case 1 :       /* mesh_w */
				temp = sscanf (buf, "%s%d", key_buf, 
				&MESH_W); 
				read_flag[i] = 1;
				break;
			case 2 :       /* mesh_w */
				temp = sscanf (buf, "%s%d", key_buf, 
				&MESH_H); 
				read_flag[i] = 1;
				break;
			case 3 :       /* prime start time */
				temp = sscanf (buf, "%s%f%f%f%f%f%f%f", key_buf
				,&tmp_float[0]
				,&tmp_float[1]
				,&tmp_float[2]
				,&tmp_float[3]
				,&tmp_float[4]
				,&tmp_float[5]
				,&tmp_float[6]);
				for (j = 0; j < DAYS_PER_W; j++) {
					PRIME_START[j] = (int) 
					(tmp_float[j] * 3600.0);
				}
				read_flag[i] = 1;
				break;
			case 4 :       /* prime end time */
				temp = sscanf (buf, "%s%f%f%f%f%f%f%f", key_buf
				,&tmp_float[0]
				,&tmp_float[1]
				,&tmp_float[2]
				,&tmp_float[3]
				,&tmp_float[4]
				,&tmp_float[5]
				,&tmp_float[6]);
				for (j = 0; j < DAYS_PER_W; j++) {
					PRIME_END[j] = (int)
					(tmp_float[j] * 3600.0);
				}
				read_flag[i] = 1;
				break;
			case 5 :       /* Prime_list */
				read_flag[i] = 1;
				if (glb.param_read > 0)
                                        break;
                                if (tmp_inx < 0) {
                                    printf ("I$read_param: Problem with the");
                                    printf (" following input line : %s\n",
                                    buf);
                                    return (1);
                                }
                                if (num_entry > MAX_SET - 2 ||
                                num_entry < 2) {
				if (num_entry == 2)
				    break;
                                    printf
                                    ("I$read_param: Number of entries for ");
                                    printf
                                    ("prime_list%d out of range.", tmp_inx);
                                    printf (" MAX = %d\n", MAX_SET);
                                    return (1);
                                }
                                if (tmp_inx < 0 || tmp_inx >= MAX_GROUP) {
                                    printf
                                    ("I$read_param: Node_group index ");
                                    printf ("%d out of range. ", tmp_inx);
                                    printf (" MAX = %d\n", MAX_GROUP - 1);
                                    return (1);
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
                                        printf
                                        ("I$read_param:Sets %d in ", set_inx);
					printf
					("prime_list %d out of range.\n", 
					tmp_inx);
                                        break;
                                    }
				    node_grp[tmp_inx].time_flag[set_inx] |= 1;

                                    while ((c = (int) *bp) != ' ') {
                                        if (c == '\0')
                                            break;
                                        bp++;
                                    }
                                }
				break;
			case 6 :       /* nprime_list */
				read_flag[i] = 1;
                                if (glb.param_read > 0)
                                        break;
                                if (tmp_inx < 0) {
                                    printf ("I$read_param: Problem with the");
                                    printf (" following input line : %s\n",
                                    buf);
                                    return (1);
                                }
                                if (num_entry > MAX_SET - 2 ||
                                num_entry < 2) {
                                    printf
                                    ("I$read_param: Number of entries for ");
                                    printf
                                    ("nprime_list%d out of range.", tmp_inx);
                                    printf (" MAX = %d\n", MAX_SET);
                                    return (1);
                                }
				if (num_entry == 2)
				    break;
                                if (tmp_inx < 0 || tmp_inx >= MAX_GROUP) {
                                    printf
                                    ("I$read_param: Node_group index ");
                                    printf ("%d out of range. ", tmp_inx);
                                    printf (" MAX = %d\n", MAX_GROUP - 1);
                                    return (1);
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
                                        printf
                                        ("I$read_param:Sets %d in ", set_inx);
                                        printf
                                        ("nprime_list %d out of range.\n",
                                        tmp_inx);
                                        break;
                                    }
                                    node_grp[tmp_inx].time_flag[set_inx] |= 2;

                                    while ((c = (int) *bp) != ' ') {
                                        if (c == '\0')
                                            break;
                                        bp++;
                                    }
                                }
				break;

			case 7 :       /* node_set definition */
				read_flag[i] = 1;
                                if (glb.param_read > 0)
                                        break;
				if (num_entry != 4) { 
                                    printf
                                    ("I$read_param: Wrong number of entries ");
				    printf
				    ("for node_set %d", tmp_inx);
				    return (1);
				}
                                temp = sscanf (buf, "%s%d%s%s", key_buf,
                                &tmp_inx, tmp_part_name, tmp_nodef);

				if (tmp_inx < 0 || tmp_inx >= MAX_SET) {
                                    printf
                                    ("I$read_param: Node_set index ");
				    printf ("%d out of range. ", tmp_inx);
				    printf ("MAX = %d\n", MAX_SET - 1);
					return (1);
				}

				if (node_set[tmp_inx].rootp_inx >= 0) {
                                    printf
                                    ("I$read_param: Duplicate node_set %d\n",
				    tmp_inx);
				    return (1);
				}

				glb.set_inx[glb.num_set] = tmp_inx;
				glb.num_set ++;
                                if (tmp_part_name[0] == '.') {
                                    strcpy (tmp_part_name1, tmp_part_name);
                                } else {
                                    strcpy (tmp_part_name1, BASE_PART_PATH);
                                    strcat (tmp_part_name1, tmp_part_name);
                                }

				for (j = 0; j < glb.rpart_cnt; j++) {
				    if (strcmp (rpart[j].part_name,
				    tmp_part_name1) == 0) { 
					node_set[tmp_inx].rootp_inx = j;
					break;
				    }
				}
				if (j >= glb.rpart_cnt) {  /* New entry */
				    if (glb.rpart_cnt >= MAX_RPART) {
                                        printf ("I$read_param: Number of root");
					printf (" partition out of range. ");
				        printf ("MAX = %d\n", MAX_RPART);
				        return (1);
				    }
				    strcpy (rpart[j].part_name,
				    tmp_part_name1);
				    node_set[tmp_inx].rootp_inx = j;
				    glb.rpart_cnt ++;
				}
				strncpy (node_set[tmp_inx].nodef, 
				tmp_nodef, NAME_LEN);
				break;

			case 8 :       /* node_group */
				read_flag[i] = 1;
                                if (glb.param_read > 0)
                                        break;
				if (tmp_inx < 0) {
				    printf ("I$read_param: Problem with the");
				    printf (" following input line : %s\n",
				    buf);
				    return (1);
				}
                                if (num_entry > MAX_SET - 2 ||
                                num_entry <= 2) {
                                    printf
                                    ("I$read_param: Number of entries for ");
                                    printf
                                    ("group_set %d out of range.", tmp_inx);
                                    printf (" MAX = %d\n", MAX_SET);
                                    return (1);
                                }

				if (tmp_inx < 0 || tmp_inx >= MAX_GROUP) {
                                    printf
                                    ("I$read_param: Node_group index ");
				    printf ("%d out of range. ", tmp_inx);
                                    printf (" MAX = %d\n", MAX_GROUP - 1);
				    return (1);
				}
				if (node_grp[tmp_inx].num_set > 0) {
                                    printf
                                    ("I$read_param: Duplicate node_group %d\n",
				    tmp_inx);
				    return (1);
				}

				glb.grp_inx[glb.num_group] = tmp_inx;
				glb.num_group ++;
				node_grp[tmp_inx].num_set = num_entry - 2;

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
                                        printf
                                        ("I$read_param:node_set cnt problem ");
					printf ("for node_group %d\n", tmp_inx);
					return (1);
                                        break;
                                    }
                                    node_grp[tmp_inx].set_inx[j] = atoi (bp);
                                    j++;

                                    while ((c = (int) *bp) != ' ') {
                                        if (c == '\0')
                                            break;
                                        bp++;
                                    }
                                }

				break;
                        case 9 :       /* partition name, curent open */
                                temp = sscanf (buf, "%s%s", key_buf,
                                tmp_part_name);
                                if (tmp_part_name [0] == '\0')
                                        break;
				bzero (CUR_OPEN_NAME, PART_NAME_LEN);
                                if (tmp_part_name[0] == '.') {
                                    strcpy (CUR_OPEN_NAME, tmp_part_name);
                                } else {
                                    strcpy (CUR_OPEN_NAME, BASE_PART_PATH);
                                    strcat (CUR_OPEN_NAME, tmp_part_name);
                                }
                                read_flag[i] = 1;
                                break;
			case 10 :       /* partition name, open_np */
				temp = sscanf (buf, "%s%s", key_buf, 
				tmp_part_name);
				if (tmp_part_name [0] == '\0')
					break;
				bzero (OPEN_NP_NAME, PART_NAME_LEN);
				if (tmp_part_name[0] == '.') {
				    strcpy (OPEN_NP_NAME, tmp_part_name);
				} else {
				    strcpy (OPEN_NP_NAME, BASE_PART_PATH);
				    strcat (OPEN_NP_NAME, tmp_part_name);
				}
				read_flag[i] = 1;
				break;
			case 11 :       /* partition name, open_p */
				temp = sscanf (buf, "%s%s", key_buf, 
				tmp_part_name);
				if (tmp_part_name [0] == '\0')
					break;
				bzero (OPEN_P_NAME, PART_NAME_LEN);
				if (tmp_part_name[0] == '.') {
				    strcpy (OPEN_P_NAME, tmp_part_name);
				} else {
				    strcpy (OPEN_P_NAME, BASE_PART_PATH);
				    strcat (OPEN_P_NAME, tmp_part_name);
				}
				read_flag[i] = 1;
				break;
			case 12  :      /* timeshare */
				temp = sscanf (buf, "%s%d", key_buf, 
				&TIMESHARE); 
				read_flag[i] = 1;
				break;
			case 13  :      /* timeshare the minor part ? */
				temp = sscanf (buf, "%s%d", key_buf, 
				&TIMESH_MINOR); 
				read_flag[i] = 1;
				break;
			case 14  :      /* min timeshare pri */
				temp = sscanf (buf, "%s%d", key_buf, 
				&TIMESH_PRI); 
				read_flag[i] = 1;
				break;
			case 15  :      /* min pri for timesh blocking */
				temp = sscanf (buf, "%s%d", key_buf, 
				&BLOCK_TS_PRI); 
				read_flag[i] = 1;
				break;
			case 16  :      /* min pri for tsched */
				temp = sscanf (buf, "%s%d", key_buf, 
				&TSCHED_PRI); 
				read_flag[i] = 1;
				break;
			case 17  :      /* chk NQS runlimit ? */
				temp = sscanf (buf, "%s%d", key_buf, 
				&CHK_RUNLIMIT); 
				read_flag[i] = 1;
				break;
			case 18  :      /* time coeff for priority */
				temp = sscanf (buf, "%s%f", key_buf, 
				&AGE_FACTOR); 
				read_flag[i] = 1;
				break;
			case 19 :       /* grace_time */
				temp = sscanf (buf, "%s%d", key_buf, 
				&GRACE_TIME); 
				read_flag[i] = 1;
				break;
			case 20 :       /* rollin_quan */
				temp = sscanf (buf, "%s%d", key_buf, 
				&ROLLIN_QUAN); 
				/* convert to millsec */
				ROLLIN_QUAN = ROLLIN_QUAN * 1000;
				read_flag[i] = 1;
				break;
			case 21 :       /* do_wall */
				temp = sscanf (buf, "%s%d", key_buf, 
				&DO_WALL); 
				read_flag[i] = 1;
				break;
			case 22  :      /* min pri for blocking */
				temp = sscanf (buf, "%s%d", key_buf, 
				&BLOCK_PRI); 
				read_flag[i] = 1;
				break;
                        case 23  :      /* macs_flag */
                                temp = sscanf (buf, "%s%d", key_buf,
                                &MACS_FLAG);
                                read_flag[i] = 1;
                                if (MACS_FLAG < MACS_OFF ||
                                MACS_FLAG > MACS_ON_NOKILL) {
                                    printf ("I$read_param : Input macs_flag");
                                    printf ("out of range. Must be 0-1. ");
                                    printf ("Set to 0\n");
                                    MACS_FLAG = DF_MACS_FLAG;
                                }
                                break;
                        case 24  :      /* np_overun */
                                temp = sscanf (buf, "%s%d", key_buf,
                                &NP_OVERRUN);
                                read_flag[i] = 1;
                                if (NP_OVERRUN != NO_OVERRUN &&
                                NP_OVERRUN != OK_OVERRUN) {
                                    printf ("I$read_param : Input np_overrun");
                                    printf ("out of range. Must be 0-1. ");
                                    printf ("Set to 0\n");
                                    NP_OVERRUN = DF_NP_OVERRUN;
                                }
                                break;
                        case 25  :      /* use_login */
                                temp = sscanf (buf, "%s%d", key_buf,
                                &USELOGIN);
                                read_flag[i] = 1;
                                break;
                        case 26  :      /* nosched */
                                temp = sscanf (buf, "%s%d", key_buf,
                                &NOSCHED);
                                read_flag[i] = 1;
                                break;
                        case 27  :       /* script name from non_prime to prime */
                                temp = sscanf (buf, "%s%s", key_buf,
                                tmp_part_name);
                                if (tmp_part_name [0] == '\0')
                                        break;
                                bzero (SCR2PRIM, MAX_PATHNAME);
                                strncpy(SCR2PRIM, tmp_part_name, MAX_PATHNAME);
                                read_flag[i] = 1;
                                break;
                        case 28  :       /* script name from prime to non_prime */
                                temp = sscanf (buf, "%s%s", key_buf,
                                tmp_part_name);
                                if (tmp_part_name [0] == '\0')
                                        break;
                                bzero (SCR2NONPRIM, MAX_PATHNAME);
                                strncpy(SCR2NONPRIM, tmp_part_name, MAX_PATHNAME);
                                read_flag[i] = 1;
                                break;
                        case 29  :       /* = 0 use SMD time; = 1 use wall clock time */
                                temp = sscanf (buf, "%s%d", key_buf,
                                &WALL_CLOCK);
                                read_flag[i] = 1;
                                break;
		}
	}

	fclose (param_fp);

	/* Check if we got all input data */

	for (i = 0; i < N_KEYWD; i++) {
	    if (!read_flag[i]) {
		switch (i) {
		    case 0 :
			printf ("I$No time_zone. Default to ");
                        strcpy (TIME_ZONE, "TZ=");
                        strcat (TIME_ZONE, DF_TIME_ZONE);
                        printf ("%s\n",TIME_ZONE);
			fflush (stdout);
			break;
		    case 1 :
			/* get it from .partinfo file */
			break;
		    case 2 :
			/* get it from .partinfo file */
			break;
		    case 3 :
			printf ("I$No prime_start input. ");
			for (j = 0; j < DAYS_PER_W; j++)
				PRIME_START[j] = DF_PRIME_START;
			printf ("Default to %d\n", PRIME_START[0]);
			fflush (stdout);
			break;
		    case 4 :
			printf ("I$No prime_end input. ");
			for (j = 0; j < DAYS_PER_W; j++)
				PRIME_END[j] = DF_PRIME_END;
			printf ("Default to %d\n", PRIME_END[0]);
			fflush (stdout);
			break;
		    case 5 :
			printf ("I$No prime_list input. ");
			printf ("Default to set %d\n", DF_PRIME_SET);
			fflush (stdout);
			break;
		    case 6 :
			printf ("I$No nprime_list input. ");
			printf ("Default to set %d\n", DF_NPRIME_SET);
			fflush (stdout);
			break;
		    case 7 :
			if (glb.param_read > 0)
				break;
			printf ("I$No node_set input. NQS exiting.\n");
			fflush (stdout);
			return (1);
			break;
		    case 8 :
			if (glb.param_read > 0)
				break;
			printf ("I$No node_group input. NQS exiting.\n");
			fflush (stdout);
			return (1);
			break;
                    case 9 :
                        printf ("I$No cur_open_name input. ");
                        strcpy (CUR_OPEN_NAME, DF_CUR_OPEN_NAME);
                        printf ("Default to %s\n", CUR_OPEN_NAME);
                        fflush (stdout);
                        break;
		    case 10 :
			printf ("I$No open_np_name input. ");
			strcpy (OPEN_NP_NAME, DF_OPEN_NP_NAME);
			printf ("Default to %s\n", OPEN_NP_NAME);
			fflush (stdout);
			break;
		    case 11 :
			printf ("I$No open_p_name input. ");
			strcpy (OPEN_P_NAME, DF_OPEN_P_NAME);
			printf ("Default to %s\n", OPEN_P_NAME);
			fflush (stdout);
			break;
		    case 12 :
			printf ("I$No timeshare. Default to ");
			TIMESHARE = DF_TIMESHARE;
			printf ("Default to %d\n",TIMESHARE);
			fflush (stdout);
			break;
		    case 13 :	/* no longer needed */
			TIMESH_MINOR = DF_TIMESH_MINOR;
			break;
		    case 14 :
                        printf ("I$No timesh_pri. ");
                        TIMESH_PRI = DF_TIMESH_PRI;
                        printf ("Default to %d\n",TIMESH_PRI);
                        fflush (stdout);
                        break;
		    case 15 :	/* no longer needed */
			BLOCK_TS_PRI = DF_BLOCK_TS_PRI;
			break;
		    case 16 :
			printf ("I$No tsched_pri. ");
			TSCHED_PRI = DF_TSCHED_PRI;
			printf ("Default to %d\n",TSCHED_PRI);
			fflush (stdout);
			break;
		    case 17 :
			printf ("I$No chk_runlimit. ");
			CHK_RUNLIMIT = DF_CHK_RUNLIMIT;
			printf ("Default to %d\n",CHK_RUNLIMIT);
			fflush (stdout);
			break;
		    case 18 :
			printf ("I$No age_factor. Default to ");
			AGE_FACTOR = DF_AGE_FACTOR;
			printf ("%f\n",AGE_FACTOR);
			fflush (stdout);
			break;
		    case 19 :
			printf ("I$No grace_time input. ");
			GRACE_TIME = DF_GRACE_TIME;
			printf ("Default to %d\n", GRACE_TIME); 
			fflush (stdout);
			break;
		    case 20 :
			printf ("I$No rollin_quan input. ");
			ROLLIN_QUAN = DF_ROLLIN_QUAN * 1000;
			printf ("Default to %d\n", ROLLIN_QUAN); 
			fflush (stdout);
			break;
		    case 21 :
			printf ("I$No do_wall input. ");
			DO_WALL = DF_DO_WALL;
			printf ("Default to %d\n", DO_WALL); 
			fflush (stdout);
			break;
		    case 22 :
			printf ("I$No block_pri. ");
			BLOCK_PRI = DF_BLOCK_PRI;
			printf ("Default to %d\n",BLOCK_PRI);
			fflush (stdout);
			break;
                    case 23 :
                        printf ("I$No macs_flag. ");
                        MACS_FLAG = DF_MACS_FLAG;
                        printf ("Default to %d\n",MACS_FLAG);
                        fflush (stdout);
                        break;
                    case 24 :
                        printf ("I$No np_overrun. ");
                        NP_OVERRUN = DF_NP_OVERRUN;
                        printf ("Default to %d\n",NP_OVERRUN);
                        fflush (stdout);
                        break;
                    case 25 :
                        printf ("I$No use_login. ");
                        USELOGIN = DF_USELOGIN;
                        printf ("Default to %d\n",USELOGIN);
                        fflush (stdout);
                        break;
                    case 26 :
                        printf ("I$No nosched. ");
                        NOSCHED = DF_NOSCHED;
                        printf ("Default to %d\n",NOSCHED);
                        fflush (stdout);
                        break;
                    case 27 :       /* script name from non_prime to prime */
		         printf ("I$No prime_script.  Default to no script for transition from non-prime to prime time.\n");
                        strcpy (SCR2PRIM, DF_SCR2PRIM);
                        fflush (stdout);
                        break;
                    case 28 :       /* script name from prime to non_prime */
		        printf ("I$No nprime_script.  Default to no script for transition from prime to non-prime time.\n");
                        strcpy (SCR2NONPRIM, DF_SCR2NONPRIM);
                        fflush (stdout);
                        break;
                    case 29 :
                        printf ("I$No wallclock_limits. ");
			WALL_CLOCK = DF_WALL_CLOCK;
                        printf ("Default to %d\n",WALL_CLOCK);
                        fflush (stdout);
                        break;
		}
	    }
	}

	/* Check some of the input */

	for (j = 0; j < DAYS_PER_W; j++) {
		if (PRIME_START[j] > SEC_P_DAY || PRIME_START[j] < 0) {
		    printf ("I$param file error : ");
		    printf ("prime_start[%2d] < 0 or > 24\n", j);
		    PRIME_START[j] = DF_PRIME_START;
		    printf ("I$Default to %d\n", PRIME_START[j]);
		    fflush (stdout);
		}
		if (PRIME_END[j] > SEC_P_DAY || PRIME_END[j] < 0) {
		    printf ("I$param file error : ");
		    printf ("prime_end[%2d] < 0 or > 24\n", j);
		    PRIME_END[j] = DF_PRIME_END;
		    printf ("I$Default to %d\n", PRIME_END[j]);
		    fflush (stdout);
		}
		if (PRIME_START[j] > PRIME_END[j]) {
		    printf ("I$param file warning : ");
		    printf ("prime_start[%2d] > prime_end[%2d]\n", j, j);
		    fflush (stdout);
		}
	}

	if (GRACE_TIME <= 0)
		GRACE_TIME = DF_GRACE_TIME;

	/* Some sanity check of node_set and node_group */

	/* node_set in a group must have the same root partition */

	if (glb.param_read == 0) {
	  for (i = 0; i < glb.num_group; i++) {
	    for (j = 1; j < node_grp[glb.grp_inx[i]].num_set; j++) {
	    	tmp_inx = node_grp[glb.grp_inx[i]].set_inx[j];
	    	if (node_set[tmp_inx].rootp_inx != 
	    	node_set[node_grp[glb.grp_inx[i]].set_inx[0]].rootp_inx) {
		    printf ("I$read_param : Root Partitions in node_group %d",
		    glb.grp_inx[i]);
		    printf (" are not identical. NQS exiting.\n");
		    return (1);
	    	}
	    }
	  }

	  /* check the time_flags in each node_group. */
	  
	  for (i = 0; i < glb.num_group; i++) {
	    tmp_inx = glb.grp_inx[i];
	    for (j = 0; j < MAX_SET; j++) {
		tmp_flag[j] = node_grp[tmp_inx].time_flag[j];
/*
printf ("I$gp=%d, time_flag=%d\n", tmp_inx, tmp_flag[j]);
*/
		node_grp[tmp_inx].time_flag[j] = 0;
	    }
	    for (j = 0; j < node_grp[tmp_inx].num_set; j++) {
	    	set_inx = node_grp[tmp_inx].set_inx[j];
		if (tmp_flag[set_inx] == 3) {
		    node_grp[tmp_inx].time_flag[j] = MAJOR;
		} else if (tmp_flag[set_inx] == 2) {
		    node_grp[tmp_inx].time_flag[j] = MINOR;
	    	} else if (tmp_flag[set_inx] == 1) {
		    printf ("I$read_param: Set %d in prime_list%-d is not in ",
		    set_inx, tmp_inx);
		    printf ("nprime_list.\n");
		    printf ("I$The set has been added to nprime_list\n");
		    node_grp[tmp_inx].time_flag[j] = MAJOR;
		} else {
                    printf ("I$read_param : Set %d of group %d is neither on ",
                    set_inx, tmp_inx);
                    printf ("the prime_list nor the nprime_list.\n");
		    printf ("I$ Set to default value.\n");
                    node_grp[tmp_inx].time_flag[j] = DF_TIME_FLAG;
		}
	    }
	  }
	}

	glb.param_read = 1;

        /* test connection to MACS at NQS start time (PTS# 9252) */

        if (MACS_FLAG != MACS_OFF) {
            for (i = 0; i <= 3; i++) {
               if (my_open() < 0) {
                  sleep(2);
                  if (i >= 3) {
                     printf("I$Unable to connect to MACS. ");
                     NOSCHED = 1;
                     printf(
                      "No jobs will be scheduled to run.\n");
                     fflush(stdout);
                  }
               } else {
                  /* MACS connection O.K. lets continue */
                  i = 4;
		  (void) c_close(); /* close the socket to MACS */
                  break;
               }
            }
        }

	return (0);
}


/*** power2 ()
 *
 * Given an integer, return the largest power of 2 value that is <= the 
 * integer. 
 *
 */
 
int power2 (val)
long val;
{
	long p2;

	if (val <= 0) 
		return (-1);

	p2 = 1;

	while ( p2 <= val) {
		p2 *= 2;
	}

	return (p2 / 2);
}

/*
 * timestring = timstr()
 *
 * This little function returns the current system time.
 *
 *
*/
 
char *timstr(now)
time_t now;
{
	int end;
	static char timestring[TIMESTRING];
 
	/* Get the formatted time string - strip off '\n' */
 
	sprintf(timestring, "%s", ctime(&now));
	end = strlen(timestring)-1;
	if (timestring[end] == '\n') timestring[end] = '\000';
	return(timestring);
}

/*
 * chk_nxaccounts()
 *
 * check if the nxaccounts file exists.
 *
 *
*/
 
void chk_nxaccounts ()
{
 
        FILE *nxacct_fd;       

        if ((nxacct_fd = fopen (NXACCOUNTS , "r")) == NULL) {
		glb.nxaccounts = 0;
        } else {  
		glb.nxaccounts = 1;
                fclose (nxacct_fd);
	}
}

/*
 * prt_param()
 *
 * print the parameters resd in from sched_param
 *
 *
*/
 
void prt_param ()
{
	int i, j, k;
	int tmp_inx, set_inx;

        /* print user hard limit and user soft limit */
        printf ("I$soft user limit set to %d\n",
        Maxsoftulimit);
        printf ("I$hard user limit set to %d\n",
        Maxhardulimit);

	printf ("I$Sched_param file values :\n");
	for (i = 0; i < N_KEYWD; i++) {
		switch (i) {
		    case 0 :
                        printf ("I$%s:%s\n",key_wd[i], TIME_ZONE);
			break;
		    case 1 :
			/* get it from .partinfo file */
			break;
		    case 2 :
			/* get it from .partinfo file */
			break;
		    case 3 :
			printf 
			("I$%s:%5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f\n", 
			key_wd[i],
			(float) (PRIME_START[0]) / 3600.,
			(float) (PRIME_START[1]) / 3600.,
			(float) (PRIME_START[2]) / 3600.,
			(float) (PRIME_START[3]) / 3600.,
			(float) (PRIME_START[4]) / 3600.,
			(float) (PRIME_START[5]) / 3600.,
			(float) (PRIME_START[6]) / 3600.);
			break;
		    case 4 :
			printf
			("I$%s:%5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f\n", 
			key_wd[i],
			(float) (PRIME_END[0]) / 3600.,
			(float) (PRIME_END[1]) / 3600.,
			(float) (PRIME_END[2]) / 3600.,
			(float) (PRIME_END[3]) / 3600.,
			(float) (PRIME_END[4]) / 3600.,
			(float) (PRIME_END[5]) / 3600.,
			(float) (PRIME_END[6]) / 3600.);
			break;
		    case 5 :
			for (k = 0; k < glb.num_group; k++) {
            		    tmp_inx = glb.grp_inx[k];
			    printf ("I$%s%-d: ", key_wd[i], tmp_inx);
            		    for (j = 0; j < node_grp[tmp_inx].num_set; j++) {
				if (node_grp[tmp_inx].time_flag[j] == MAJOR) {
                		    set_inx = node_grp[tmp_inx].set_inx[j];
			    	    printf ("%d ", set_inx);
				}
			    }
			    printf ("\n");
			}
			break;
		    case 6 :
			for (k = 0; k < glb.num_group; k++) {
            		    tmp_inx = glb.grp_inx[k];
			    printf ("I$%s%-d: ", key_wd[i], tmp_inx);
            		    for (j = 0; j < node_grp[tmp_inx].num_set; j++) {
                		set_inx = node_grp[tmp_inx].set_inx[j];
			    	printf ("%d ", set_inx);
			    }
			    printf ("\n");
			}
			break;
		    case 7 :
			for (j = 0; j < glb.num_set; j++) {
			    tmp_inx = glb.set_inx[j];
			    printf ("I$%s%-d:%s %s\n", key_wd[i], tmp_inx,
			    rpart[node_set[tmp_inx].rootp_inx].part_name,
			    node_set[tmp_inx].nodef);
			}
			break;
		    case 8 :
			for (j = 0; j < glb.num_group; j++) {
			    tmp_inx = glb.grp_inx[j];
			    printf ("I$%s%-d: ", key_wd[i], tmp_inx); 
			    for (k = 0; k < node_grp[tmp_inx].num_set; k ++) {
				printf (" %d", node_grp[tmp_inx].set_inx[k]);
			    }
			    printf ("\n");
			}
			break;
		    case 9 :
			printf ("I$%s:%s\n", key_wd[i], CUR_OPEN_NAME);
			break;
		    case 10 :
			printf ("I$%s:%s\n", key_wd[i], OPEN_NP_NAME);
			break;
		    case 11 :
			printf ("I$%s:%s\n", key_wd[i], OPEN_P_NAME);
			break;
		    case 12 :
			printf ("I$%s:%d\n", key_wd[i], TIMESHARE);
			break;
		    case 13 :
			printf ("I$%s:%d\n", key_wd[i], TIMESH_MINOR);
			break;
		    case 14 :
			printf ("I$%s:%d\n", key_wd[i], TIMESH_PRI);
			break;
		    case 15 :
			printf ("I$%s:%d\n", key_wd[i], BLOCK_TS_PRI);
			break;
		    case 16 :
			printf ("I$%s:%d\n", key_wd[i], TSCHED_PRI);
			break;
		    case 17 :
			printf ("I$%s:%d\n", key_wd[i], CHK_RUNLIMIT);
			break;
		    case 18 :
			printf ("I$%s:%f\n", key_wd[i], AGE_FACTOR);
			break;
		    case 19 :
			printf ("I$%s:%d\n", key_wd[i], GRACE_TIME); 
			break;
		    case 20 :
			printf ("I$%s:%d\n", key_wd[i], ROLLIN_QUAN / 1000); 
			break;
		    case 21 :
			printf ("I$%s:%d\n", key_wd[i], DO_WALL); 
			break;
		    case 22 :
			printf ("I$%s:%d\n", key_wd[i], BLOCK_PRI);
			break;
                    case 23 :
                        printf ("I$%s:%d\n", key_wd[i], MACS_FLAG);
                        break;
                    case 24 :
                        printf ("I$%s:%d\n", key_wd[i], NP_OVERRUN);
                        break;
                    case 25 :
                        printf ("I$%s:%d\n", key_wd[i], USELOGIN);
                        break;
                    case 26:
                        printf ("I$%s:%d\n", key_wd[i], NOSCHED);
                        break;
                    case 27 :       /* script name from non_prime to prime */
                        printf ("I$%s:%s\n", key_wd[i], SCR2PRIM);
                        break;
                    case 28 :       /* script name from prime to non_prime */
                        printf ("I$%s:%s\n", key_wd[i], SCR2NONPRIM);
                        break;
                    case 29 :
                        printf ("I$%s:%d\n", key_wd[i], WALL_CLOCK);
                        break;

 		}
	    }
	}

/**** init_allgrp ()
 *
 * Subroutine to initialize all the node_group structures.
 *
 */

int init_allgrp ()
{
	int i, j, k;
	int grp_inx;

	for (i = 0; i < glb.num_group; i++) {
	    grp_inx = glb.grp_inx[i];
	    if (init_grp (grp_inx) < 0) {
		printf ("I$init_allgrp: Problem with init_grp %d\n", grp_inx);
		return (-1);
	    }
#ifdef DEBUG
	    for (k = 0; k < MAX_PERIOD; k++) {
		printf ("I$node_grp%-d: time_flag=%d, size=%d, free_nodes=", 
		grp_inx, k, node_grp[grp_inx].size[k]);
		for (j = 0; j < MAX_LAYER; j ++) {
		    printf (" %d ", node_grp[grp_inx].free_nodes[j][k]);
		}
		printf ("\n");
	    }
#endif
	}
	return (0);
}

/* init_grp 
 *
 * Initialize the free_node and size of a node_grp structure.
 *
 */

int init_grp (grp_inx)
int grp_inx;
{
	int j, k, l;
	int set_inx;
	int time_flag;

	if (grp_inx < 0 ||  grp_inx >= MAX_GROUP) {
	    printf ("I$init_grp: grp_inx = %d out of range\n", grp_inx);
	    return (-1);
	}

	for (j = 0; j < glb.num_group; j++) {
	    if (grp_inx == glb.grp_inx[j])
		break;
	}
	if (j >= glb.num_group) {
	    printf ("I$init_grp: group %d has not been configured\n", grp_inx);
	    return (-1);

	}
	if (node_grp[grp_inx].num_set <= 0 || 
	node_grp[grp_inx].num_set >= MAX_SET) {
	    printf ("I$init_grp: num_set for node_group %d out of range.\n",
	    grp_inx);
	    return (-1);
	}

	for (k = 0; k < MAX_PERIOD; k++) {
	    node_grp[grp_inx].size[k] = 0;
	    for (j = 0; j < MAX_LAYER; j ++) {
		node_grp[grp_inx].free_nodes[j][k] = 0;
	    }
	}

        for (l = 0; l < node_grp[grp_inx].num_set; l++) {
	    set_inx = node_grp[grp_inx].set_inx[l];
	    time_flag = node_grp[grp_inx].time_flag[l];
	    node_grp[grp_inx].size[time_flag] += node_set[set_inx].size;

	    for (j = 0; j < MAX_LAYER; j ++) {
		if (node_set[set_inx].blocked[j] == 0)
		    node_grp[grp_inx].free_nodes[j][time_flag] +=
		    node_set[set_inx].root_blk[j]->free_nodes;
	    }
	}
	return (0);
}
		   
#endif
