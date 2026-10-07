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
/* macs_rootp.c
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/macs_rootp.c,v $
 *
 * DESCRIPTION:
 *
 *	The module contains a number of general subroutines for managing 
 *	 the root partition of the buddy system. 
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
 * $Log: macs_rootp.c,v $
 * Revision 1.24  1995/03/17  18:21:45  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 9765
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/include/nqs/buddy.h
 * 	cmds_libs/src/usr/include/nqs/buddyvar.h
 * 	cmds_libs/src/usr/lib/nqs/macs_lib.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_bsc.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_vtimer.c
 * 	cmds_libs/src/usr/lib/nqs/macs_sched.c
 * 	cmds_libs/src/usr/lib/nqs/macs_rootp.c
 *
 * Revision 1.23  1995/01/24  19:14:07  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 11897
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/lib/nqs/macs_rootp.c
 *
 * Revision 1.22  1995/01/10  22:26:26  kremenek
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 10696
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/lib/nqs/macs_rootp.c
 *
 * Revision 1.21  1994/11/19  02:52:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.20  1994/11/18  20:36:07  davidl
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
 *  Revision 1.19  1994/10/07  01:07:17  kremenek
 *  Reviewer:  davidl doyle
 *  Risk: Low
 *  Benefit or PTS #: 7858
 *  Testing: EATS
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/macs_rootp.c
 *
 * Revision 1.18  1994/08/31  20:23:25  bradf
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
 * Revision 1.16.2.1  1994/08/05  20:36:46  kremenek
 *  Reviewer: George Kremenek SDSC
 *  Risk: Low
 *  Benefit or PTS #: 6589 9491 9950 10355
 *  Testing: EATS passed
 *  Module(s):
 *
 * Revision 1.17  1994/08/05  17:14:40  mwan
 *  Reviewer: George Kremenek
 *  Risk: Low
 *  Benefit or PTS #: 6589 9491 9950 10355
 *  Testing: EATS
 *  Module(s):
 *
 * Revision 1.13.2.5  1994/04/26  21:54:16  mwan
 * Fixed 8230, 8802, 6877 and 9010.
 *
 * Reviewer: kremenek
 *  Risk: Medium
 *  Benefit or PTS #: 8230, 8802, 6877 and 9010.
 *  Testing:
 *  Module(s): usr/lib/nqs/macs_rootp.c netdaemon.c nqs_pip.c nqs_reqser.c
 * 	    ccs/lib/libnqs/listq.c
 *
 * Revision 1.13.2.4  1994/03/23  17:51:13  mwan
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
 * Revision 1.9  1993/08/07  01:18:34  mwan
 * T11 - fix the problem with NX_GANG definition. Now gets it in nx.h
 *
 * Revision 1.8  1993/07/21  18:46:36  mwan
 * T11 - change default value of block_pri and block_ts_pri.
 *
 * Revision 1.6  1993/05/24  23:10:05  mwan
 * Fixed PTS 4769 - OPEN_P doesn't work
 *
 * Revision 1.5  1993/05/19  18:03:51  mwan
 * Before T10 freeze. Fixed several PTS
 *
 * Revision 1.4  1992/12/16  23:26:05  mwan
 * T6 update 2
 *
 * Revision 1.3  1992/10/26  18:47:22  mwan
 * T6 update 1
 *
 * Revision 1.2  1992/10/09  22:24:34  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 *
 */

#ifdef SDSC
#define WALLPROG "/usr/sbin/wall"
/* #define NX_GANG	0 */			/* now defined in nx.h */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>                  /* Stat file */
#include <errno.h>
#include <fcntl.h>                      /* File control */
#include <pwd.h>                        /* File control */
#include <time.h>                       /* File control */
#include <signal.h>                     /* File control */
#include <errno.h>
#include <nx.h>
#include <string.h>
#include <sys/stat.h>
#include "nqs.h"
#include "nqsxvars.h"
#include "buddyxvar.h"
#include "dir.4.2.h"                    /* Include directory walking defs */

/* External function */

extern int init_block ();
extern int rel_blk ();
extern int add_blk ();
extern int comb_blk ();
extern struct block *get_blk ();
extern int alloc_blk ();
extern int alloc_blk_ts ();
extern struct block *srch_blk ();
extern struct block *srch_child ();
extern int free_to_job ();
extern int adj_count ();
extern int split_blk ();
extern void prt_tree ();
extern int bindex ();
extern int get_order ();
extern int mv_blk ();
extern int mv_all_blk ();
extern int rem_blk ();
extern struct block *get_best_chd ();
extern struct block *match_blk ();
extern int init_allgrp ();
extern int init_grp ();

#ifdef SDSC
extern short sModeChange;
#endif

int init_rootp ();
int chk_nodef ();
int phynode_to_xy ();
int xy_to_phynode ();
int set_block_ts ();
int set_block ();
int alloc_base ();
struct job_que *find_inuse_que ();
int rmpart_r ();
int *read_phynode ();
int part_to_dir ();
int log_to_phy ();
int read_mesh ();
int set_rootp_attri ();
int get_partpri ();
void do_wall ();
void vLogModeSwitch ();
void vPerformBatchScript();
char *get_endpart ();
int alloc_set ();


/*** iRmAllPartsNqs(sPath)
 *
 * remove all NQS partitions.
 *
 *   sPath should be "/etc/nx/"
 */

int
iRmAllPartsNqs(sPath)
   char           *sPath;
{
   DIR            *dir;		/* Open queue order dir structure */
   struct direct  *dirent;	/* Ptr to directory entry */
   char            part_name[PART_NAME_LEN], sTmp[PART_NAME_LEN];
   char           *bp;
   int             c, jcnt, icnt, kcnt, iDuplDir;

   /*
    * Be sure that the NQS configuration file /usr/spool/nqs/conf/sched_param
    * was read before function iRmAllPartsNqs was called !
    * 
    * remove all partitions for all node sets from the NQS sched_param
    * configuration file ("rmpart -r ..") PTS 11754
    */

   for (jcnt = 0; jcnt < glb.num_set; jcnt++) {
      strcpy(sTmp, sPath);
      strcat(sTmp, "/");
      strcat(sTmp, rpart[node_set[glb.set_inx[jcnt]].rootp_inx].part_name);
      for (icnt = 0; icnt < strlen(sTmp); icnt++) {
	 if (sTmp[icnt] == '.')
	    sTmp[icnt] = '/';
      }

      iDuplDir = 0;
      for (kcnt = 0; kcnt < jcnt; kcnt++) {
	 if (0 == strcmp(rpart[node_set[glb.set_inx[kcnt]].rootp_inx].part_name,
		     rpart[node_set[glb.set_inx[jcnt]].rootp_inx].part_name)) {
	    iDuplDir = 1;
	    break;
	 }
      }

      if (iDuplDir == 0) {
	 /* Open the /etc/nx/* dir */

	 if ((dir = opendir(sTmp)) == (DIR *) 0) {
	    printf("I$iRmAllPartsNqs: Cannot open dir %s\n", sTmp);
	    fflush(stdout);
	    return (-1);
	 }
	 while ((dirent = readdir(dir)) != (struct direct *) 0) {
	    if (strncmp(dirent->d_name, PART_PFIX, strlen(PART_PFIX)) == 0) {
	       /* integer following PART_PFIX ? */
	       bp = dirent->d_name + strlen(PART_PFIX);
	       while ((c = (int) *bp) != '\0') {
		  if ((isdigit(c) == 0) && (c != '_'))
		     break;
		  bp++;
	       }
	       if (c != '\0')	/* not a digit */
		  continue;

	       strcpy(sTmp, rpart[node_set[glb.set_inx[jcnt]].rootp_inx].part_name);
	       strcat(sTmp, ".");
	       strcat(sTmp, dirent->d_name);

	       if (nx_rmpart(sTmp, 1, 1) != 0) {
		  printf("W$iRmAllPartsNqs: Problem with removing part : %s\n", sTmp);
		  return (-1);
	       } else {
		  if (Debug > 2) {
		     printf("I$iRmAllPartsNqs: Removed old partition : %s\n", sTmp);
		     fflush(stdout);
		  }
	       }
	    }
	 }
	 closedir(dir);
      }
   }				/* for */
   return (0);
}

/*** init_rootp ()
 *
 * initialize the root partition, the layer and the root_blk structures.
 *
 */
int init_rootp ()
{
        int i, j, k, tmp_inx;
	int parent_nodecnt;
 
        /* initialize the major and minor partition root structures */
 
	for (i = 0; i < MAX_LAYER; i++) {
	  for (k = 0; k <= MAX_PERIOD; k++) {
            layer[i].inuse_que[k].top = NULL;
            layer[i].inuse_que[k].last = NULL;
            layer[i].wait_que.top = NULL;
            layer[i].wait_que.last = NULL;
	  }
	}
 

        /* split the root_blk of each set */
 
	for (i = 0; i < glb.num_set; i++) {
	    tmp_inx = glb.set_inx[i];
	    if (node_set[tmp_inx].root_blk[0]->free_nodes > 0) {
        	if (split_blk (node_set[tmp_inx].root_blk[0]) < 0)
		    printf ("I$init_rootp: probem with split_blk\n");
 
        	/* Sort the def_order. Should be in decreasing order of size of
         	 * children.
         	 */
 
        	if (node_set[tmp_inx].root_blk[0]->child_cnt == 4 &&
        	node_set[tmp_inx].root_blk[0]->child[3]->free_nodes >
        	node_set[tmp_inx].root_blk[0]->child[1]->free_nodes) {
		    node_set[tmp_inx].def_order[0] = 0;
		    node_set[tmp_inx].def_order[1] = 3;
		    node_set[tmp_inx].def_order[2] = 2;
		    node_set[tmp_inx].def_order[3] = 1;
        	} else {
		    node_set[tmp_inx].def_order[0] = 0;
		    node_set[tmp_inx].def_order[1] = 1;
		    node_set[tmp_inx].def_order[2] = 2;
		    node_set[tmp_inx].def_order[3] = 3;
		}
	    }
	}
 
	/* Remove all NQS partition */

	if (iRmAllPartsNqs (PART_ROOTPATH) != 0) {
            printf ("F$init_rootp : problem with iRmAllPartsNqs \n");
            nqs_abort();    /* Abort execution */
	}
        if ((glb.work_lognode = (int *) malloc ((MESH_H * MESH_W) *
        sizeof (int))) == (int *) NULL) {
            printf ("W$init_rootp : malloc problem\n");
            fflush (stdout);
            return (-1);
        }
	
	/* fill in phynode for each root partition */

	for (i = 0; i < glb.rpart_cnt; i++) {

	    /* read in the rpart phynode */

	    if ((rpart[i].phynode = read_phynode (rpart[i].part_name, 
	    &parent_nodecnt)) == NULL) {
		printf ("I$init_rootp: Problem with read_phynode for part %s\n",
		rpart[i].part_name);
	    	return (-1);
	    }
	}

	if (chk_rootp () < 0) {
	    printf ("I$init_rootp: problem with chk_rootp\n");
	    fflush (stdout);
	    return (-1);
	}

	if (init_allgrp () < 0) {
            printf ("I$init_rootp: problem with init_allgrp\n");
            fflush (stdout);
            return (-1);
        }

/* the functionality of the allocator changed in R1.2
	nx_chpart_rq(".compute", ROLLIN_QUAN) should not be called 
#ifndef R1_1
	if (nx_chpart_rq (".compute", ROLLIN_QUAN) < 0) {
	    printf ("I$init_rootp: Unable to nx_chpart_rq.\n");
            fflush (stdout);
	}
#endif R1_1
*/
	return (0);
}

/*** part_to_dir ()
 *
 * convert the partition name to directory path. Also returns the length
 * on the parent name.
 * 
 *
 */
int part_to_dir (part, dir, parent_len)
char *part;
char *dir;
int *parent_len;
{
	int i;
	char *bp, *bp1;
	char buf[PART_NAME_LEN];

        *parent_len = 0;
 
        buf[0] = '\0';
        bp = buf;
        bp1 = part;
        
        i = 0;
        while (*bp1 != '\0') {
            if (*bp1 == '.') {
                *bp = '/';
                *parent_len = i;
            } else {
                *bp = *bp1;
            }
            if ((i = i + 1) > PART_NAME_LEN - strlen (DF_PART_ROOTPATH)) { 
		printf ("I$Part name: %s too long\n", part); 
		return (-1);
	    }
            bp1 ++;
            bp ++;
        }
        *bp = '\0';
	if (buf[0] == '/')
            sprintf (dir, "%s%s", PART_ROOTPATH, buf);
	else
            sprintf (dir, "%s%s", DF_PART_ROOTPATH, buf);
	return (0);
}
 
/*** rmpart_r ()
 *
 * remove all partitions recursively starting at top_part.
 * 
 *
 */
int rmpart_r (top_part)
char *top_part;
{
	char top_dir[PART_NAME_LEN];
        DIR *dir;                       /* Open queue order dir structure */
        struct direct *dirent;          /* Ptr to directory entry */
	char part_name[PART_NAME_LEN];
	int parent_len;

	if (part_to_dir (top_part, top_dir, &parent_len) != 0) 
	    return (-1);

	if ((dir = opendir (top_dir)) == (DIR *) 0) {
	    printf ("I$rmpart_r: Cannot open dir %s\n", top_dir); 
	    return (0);
	}

        while ((dirent = readdir (dir)) != (struct direct *) 0) {
	    if (strcmp (dirent->d_name, PARTINFO) == 0) 
		continue;
	    if (strcmp (dirent->d_name, ".") == 0) 
		continue;
	    if (strcmp (dirent->d_name, "..") == 0) 
		continue;
	    sprintf (part_name, "%s.%s", top_part, dirent->d_name);
	    if (rmpart_r (part_name) != 0) {
		printf ("I$rmpart_r: Unable to rmpart_r: %s\n", part_name);
	    }
        }
        closedir (dir);     
#if DEBUG
	printf ("I$rmpart: Removing part : %s\n", top_part);
#endif
	if (nx_rmpart (top_part, 1, 1) != 0) {
	    printf ("I$rmpart: Problem with removing part : %s\n", top_part);
	    return (-1);
	}
        else {
           if (Debug > 2) {
		     printf("I$rmpart: Removed part : %s\n", top_part);
		     fflush(stdout);
	   }
	}

	return (0);
}

/*** chk_nodef ()
 *
 * Subroutine to check the node def of the partitions.
 *
 * return value :
 *	0 - no error.
 *     -1 - error
 */

int chk_nodef ()

{
	int i, j, k, temp;
	int tmp_inx, tmp_inx1;
	int tmp_w, tmp_h, tmp_x, tmp_y;
	long st_node;		/* starting node, logical */
	long phy_node;		/* physical starting node */
	long min_x;
	long max_x;
	long min_y;
	long max_y;
	long min_x1;
	long max_x1;
	long min_y1;
	long max_y1;
	char *bp;


	/* check if the root partitions exists */

	for (i = 0; i < glb.rpart_cnt; i++) {
	    if (fnx_get_partid(rpart[i].part_name) < 0) {
		printf ("I$chk_nodef: Input partition: %s does not exist. ",
		rpart[i].part_name);
		printf ("I$NQS exiting\n");
		return (-1);
	    }
	}

	
	/* read the mesh */

	if (read_mesh (rpart[0].part_name) < 0) {
	    return (-1);
	}

	/* allocated block to partition */

	for (i = 0; i < MAX_LAYER; i++) {
	    for (j = 0; j < glb.num_set; j++) {
		tmp_inx = glb.set_inx[j];
            	if ((node_set[tmp_inx].root_blk[i] = 
		    get_blk (&free_blk_que, TOP)) == NULL) {
	    	    printf ("I$Chk_nodef : Unable to get_blk.\n");
	    	    return (-1);
	    	} else {
            	    if (init_block (node_set[tmp_inx].root_blk[i]) < 0)
		    	printf ("I$chk_nodef: probem with init_block\n");
	    	    node_set[tmp_inx].root_blk[i]->node_set = 
		    &node_set[tmp_inx];
	    	    node_set[tmp_inx].root_blk[i]->layer_inx = i;
		}
	    }
	}

	/* parse the nodef first */

	for (i = 0; i < glb.num_set; i++) {
	    tmp_inx = glb.set_inx[i];
	    if (strcmp (node_set[tmp_inx].nodef, "0") == 0) {
		for (j = 0; j < MAX_LAYER; j++ ) {
	    	    node_set[tmp_inx].root_blk[j]->width = 0;
	    	    node_set[tmp_inx].root_blk[j]->height = 0;
	    	    node_set[tmp_inx].root_blk[j]->x = 0;
	    	    node_set[tmp_inx].root_blk[j]->y = 0;
		    node_set[tmp_inx].root_blk[j]->next = NULL;
		    node_set[tmp_inx].root_blk[j]->prev = NULL;
		    node_set[tmp_inx].root_blk[j]->no_combo = 1;
		    node_set[tmp_inx].root_blk[j]->free_nodes = 0;
		}
		node_set[tmp_inx].size = 0;
	    } else {
	    	temp = sscanf (node_set[tmp_inx].nodef, "%dx%d:%d", 
	    	&tmp_h, &tmp_w, &st_node);

	    	if (temp !=3) {		/* try upper case */
	    	    temp = sscanf (node_set[tmp_inx].nodef, "%dX%d:%d", 
	    	    &tmp_h, &tmp_w, &st_node);
	    	    if (temp !=3) {
			printf ("I$param file error : problem with syntax");
			printf (" of nodef for node_set %d.\n", tmp_inx);
			return (-1);
		    }
	    	}
	    
	    	/* convert from logical to physical */

		tmp_inx1 = node_set[tmp_inx].rootp_inx;
	    	if ((phy_node = log_to_phy (rpart[tmp_inx1].part_name, st_node))
		< 0) {
		    printf ("I$chk_nodef:problem with log_to_phy()-st_node=%d\n"
		    , st_node);
		    return (-1);
	    	}
		
#ifdef DEBUG
	    	printf ("I$major st_phy_node = %d\n", phy_node);
#endif
	    	if (phynode_to_xy (&phy_node, &tmp_x, &tmp_y) != 0) {
		    printf ("I$param file error : ");
		    printf ("starting node of node_set %d out of range.\n", 
		    tmp_inx);
		    return (-1);
	    	}
		
		if (tmp_w + tmp_x > MESH_W || tmp_h + tmp_y > MESH_H) {
		    printf ("I$param file error : ");
		    printf ("dimension of node set %d out of range.\n", 
		    tmp_inx);
		    return (-1);
	    	}

		for (j = 0; j < MAX_LAYER; j++ ) {
	    	    node_set[tmp_inx].root_blk[j]->width = tmp_w;
	    	    node_set[tmp_inx].root_blk[j]->height = tmp_h;
	    	    node_set[tmp_inx].root_blk[j]->x = tmp_x;
	    	    node_set[tmp_inx].root_blk[j]->y = tmp_y;
		    node_set[tmp_inx].root_blk[j]->next = NULL;
		    node_set[tmp_inx].root_blk[j]->prev = NULL;
		    node_set[tmp_inx].root_blk[j]->no_combo = 1;
		    node_set[tmp_inx].root_blk[j]->free_nodes = tmp_w * tmp_h;
		}
		node_set[tmp_inx].size = tmp_w * tmp_h;
	    }
	}

	/* Check for overlapping major/minor partition */

	for (i = 0; i < glb.num_set; i++) {
	    tmp_inx = glb.set_inx[i];
	    if (node_set[tmp_inx].size <= 0)
		continue;
	    for (j = i + 1; j < glb.num_set; j++) {
	        tmp_inx1 = glb.set_inx[j];
	        if (node_set[tmp_inx1].size <= 0)
		    continue;
		min_x = node_set[tmp_inx].root_blk[0]->x;
		max_x = min_x + 
		node_set[tmp_inx].root_blk[0]->width - 1;
		min_y = node_set[tmp_inx].root_blk[0]->y;
		max_y = min_y + 
		node_set[tmp_inx].root_blk[0]->height -1;
		min_x1 = node_set[tmp_inx1].root_blk[0]->x;
		max_x1 = min_x1 + node_set[tmp_inx1].root_blk[0]->width - 1;
		min_y1 = node_set[tmp_inx1].root_blk[0]->y;
		max_y1 = min_y1 + node_set[tmp_inx1].root_blk[0]->height -1;
		if (((min_x1 >= min_x && min_x1 <= max_x) ||
		(max_x1 >= min_x && max_x1 <= max_x)) &&
		((min_y1 >= min_y && min_y1 <= max_y) ||
		(max_y1 >= min_y && max_y1 <= max_y))) {
		    printf ("I$param file error : ");
		    printf ("Node_set %d and node_set %d overlap\n",
		    tmp_inx, tmp_inx1);
		    return (-1);
		}
	    }
	}	

	return (0);
}

/*** read_mesh ()
 *
 * Convert from logical to physical through the .partinfo file.
 *
 */
int read_mesh (part)
char *part;
{
	FILE *pi_fd;		/* fd for the .partinfo file */
	char pi_path[PART_NAME_LEN];
	int temp, i, c;
	char buf[8];
	char *bp;


	if (part_to_dir (part, pi_path, &temp) < 0)
	    return (-1);

	strcat (pi_path, "/");
	strcat (pi_path, PARTINFO);

        /* open the partinfo file */

        if ((pi_fd = fopen (pi_path , "r")) == NULL) {
            printf ("I$read_mesh: Unable to open partinfo file %s.\n", pi_path);
            fflush (stdout);
            return (-1);
        }
	 
	/* throw away the first 11 lines */

	for (i = 0; i < 11; i++) {
	    while ((c = getc (pi_fd)) != '\n') {
		if (c == EOF) {
		    fclose (pi_fd);
		    return (-1);
		}
		continue;
	    }
	}

	/* read height */

        buf[0] = '\0';
        bp = buf;
        while ((c = getc (pi_fd)) != '\n') {
            if (c == EOF)
                break;
            *bp ++ = c;
        }
        *bp = '\0';
        if ((MESH_H = atoi (buf)) <= 0) {
	    printf ("I$read_mesh : Inconsistent mesh_h\n");
            fclose (pi_fd);
	    return (-1);
	}

	/* read width */

        buf[0] = '\0';
        bp = buf;
        while ((c = getc (pi_fd)) != '\n') {
            if (c == EOF)
                break;
            *bp ++ = c;
        }
        *bp = '\0';
        if ((MESH_W = atoi (buf)) <= 0) {
	    printf ("I$read_mesh : Inconsistent mesh_w\n");
            fclose (pi_fd);
	    return (-1);
	}
	printf ("I$read_mesh: mesh_h = %d, mesh_w = %d\n", MESH_H, MESH_W);
	fclose (pi_fd);
	return (0);
}
 
/*** log_to_phy ()
 *
 * Convert from logical to physical through the .partinfo file.
 *
 */
int log_to_phy (part, log_node)
char *part;
int log_node;
{
	FILE *pi_fd;		/* fd for the .partinfo file */
	char pi_path[PART_NAME_LEN];
	int temp, i, c;
	char buf[8];
	char *bp;
	int phynode;


	if (part_to_dir (part, pi_path, &temp) < 0) {
            printf ("I$log_to_phy: problem with part_to_dir%s.\n", part);
            fflush (stdout);
	    return (-1);
	}

	strcat (pi_path, "/");
	strcat (pi_path, PARTINFO);

        /* open the partinfo file */

        if ((pi_fd = fopen (pi_path , "r")) == NULL) {
            printf ("I$Unable to open partinfo file %s.\n", pi_path);
            fflush (stdout);
            return (-1);
        }
	 
	/* throw away the first 10 lines */

	for (i = 0; i < 10; i++) {
	    while ((c = getc (pi_fd)) != '\n') {
		if (c == EOF) {
            	    printf ("I$Reached end of partinfo file %s.\n", pi_path);
            	    fflush (stdout);
		    fclose (pi_fd);
		    return (-1);
		}
		continue;
	    }
	}

	/* read the physical node array */

	/* skip log_node "," */

	for (i = 0; i < log_node; i++) {
            while ((c = getc (pi_fd)) != ',') {
		if (c == EOF || c == '\n') {
            	    printf ("I$Reached end of partinfo file %s.\n", pi_path);
		    fclose (pi_fd);
		    return (-1);
		}
	    }
	}
	
        buf[0] = '\0';
        bp = buf;
	while ((c = getc (pi_fd)) != ',') {
	    if (c == EOF || c == '\n')
		break;
	    *bp ++ = c;
	}
	*bp = '\0';
	phynode = atoi (buf);

	fclose (pi_fd);
	return (phynode);
}
 
/*** phynode_to_xy ()
 *
 * Convert from start_node to x, y.
 *
 */
int phynode_to_xy (phy_node, x, y)
long *phy_node;
long *x;
long *y;
{
	if (*phy_node < 0 || *phy_node >= MESH_W * MESH_H) {
		return (-1);
	} else {
		*x = *phy_node - *phy_node / MESH_W * MESH_W;
		*y = *phy_node / MESH_W;
	}
	return (0);
}

/*** xy_to_phynode ()
 *
 * Convert from x, y to start_node.
 *
 */
int xy_to_phynode (phy_node, x, y)
long *phy_node;
long *x;
long *y;
{
	if (*x >= MESH_W || *y >= MESH_H) 
		return (-1);
	*phy_node = *y * MESH_W + *x;
	return (0);
}

/*** alloc_base ()
 *
 * A routine to the best set of nodes for a job in the base layer.
 * Return MAJOR, MINOR or COMBO
 *
 */
int alloc_base (tj_req)
struct job_req *tj_req;
{
	int i, j, k;
	int grp_inx;
	int set_inx;
	int major_sz;
	int minor_sz;
	int major_free;
	int minor_free;
	int ts_minor;		/* nodes used or blocked in minor in the ts
				 * layer. */

	grp_inx = tj_req->grp_inx;

	/* Enough free nodes ? */

	major_free = node_grp[grp_inx].free_nodes[BASE_LAYER][MAJOR];
	minor_free = node_grp[grp_inx].free_nodes[BASE_LAYER][MINOR];
	major_sz = node_grp[grp_inx].size[MAJOR];
	minor_sz = node_grp[grp_inx].size[MINOR];
	ts_minor = minor_sz - node_grp[grp_inx].free_nodes[TS_LAYER][MINOR];

	if (tj_req->req_nodes > major_free + minor_free)
	    return (-1);

	/* Try the MAJOR node_sets first */

	if (tj_req->req_nodes <= major_free)
		return (MAJOR);

	/* We have to put it in either COMBO or MINOR */

	if (glb.cur_mode != NPRIME_M)
		return (-1);
 
	if (NP_OVERRUN == NO_OVERRUN && glb.cur_time24 + tj_req->req_time
	> glb.p_start)
		return (-1);

	/* Large enough to be placed in combo partition ? */

	if (tj_req->req_nodes > major_sz && 
	tj_req->req_nodes > minor_sz &&
	ts_minor == 0 && 
	major_sz != 0 && minor_sz != 0)
		return (COMBO);

	if (tj_req->req_nodes <= minor_free && ts_minor == 0)
		return (MINOR);
	else if (glb.cur_mode == NPRIME_M && ts_minor == 0)
		return (COMBO);
	else 
		return (-1);
}

/*** alloc_ts ()
 *
 * A routine to the best set of nodes for a job in the timeshare layer.
 * Return MAJOR, MINOR or COMBO
 *
 */
int alloc_ts (tj_req)
struct job_req *tj_req;
{
        int i, j, k;
        int grp_inx;
        int set_inx;
        int major_sz;
        int minor_sz;
        int major_free;
        int minor_free;
	int base_minor;		/* nodes used or blocked in minor in the base
				 * layer. */

        grp_inx = tj_req->grp_inx;

	/* Enough free nodes ? */

        major_free = node_grp[grp_inx].free_nodes[TS_LAYER][MAJOR];
        minor_free = node_grp[grp_inx].free_nodes[TS_LAYER][MINOR];
        major_sz = node_grp[grp_inx].size[MAJOR];
        minor_sz = node_grp[grp_inx].size[MINOR];
	base_minor = minor_sz - node_grp[grp_inx].free_nodes[BASE_LAYER][MINOR];
        if (tj_req->req_nodes > major_free + minor_free)
            return (-1);

	if (tj_req->prior < TIMESH_PRI)
		return (NULL); 

	/* Large enough to be placed in combo partition ? */

        if (tj_req->req_nodes > major_sz &&
        tj_req->req_nodes > minor_sz &&
        glb.cur_mode == NPRIME_M &&
        base_minor == 0 &&
        major_sz != 0 && minor_sz != 0)
                return (COMBO);

        /* Try the MAJOR node_sets */

        if (tj_req->req_nodes <= major_free)
                return (MAJOR);

	/* the minor node_sets will not be time shared. */

        if (tj_req->req_nodes <= minor_free && glb.cur_mode == NPRIME_M
        && base_minor == 0)
                return (MINOR);
        else if (glb.cur_mode == NPRIME_M && base_minor == 0)
                return (COMBO);
        else
                return (-1);
}

/* alloc_set ()
 *
 * Given the period_inx and layer_inx. Go through the node_sets in the
 * node_grp and alloc the blocks.
 */
int alloc_set (j_req, layer_inx, period_inx, req_nodes)
struct job_req *j_req;
int layer_inx;
int period_inx;
int req_nodes;
{
	int i;
	int grp_inx, set_inx;
	int free_nodes[MAX_PERIOD + 1];
	struct block *root_blk;
	int nodes_remain, tmp_nodes;

	if (j_req == NULL)
		return (-1);

	grp_inx = j_req->grp_inx;

        free_nodes[MAJOR] = node_grp[grp_inx].free_nodes[layer_inx][MAJOR];
        free_nodes[MINOR] = node_grp[grp_inx].free_nodes[layer_inx][MINOR];
	free_nodes[COMBO] = free_nodes[MAJOR] + free_nodes[MINOR];

	if (req_nodes > free_nodes[period_inx])
		return (-1);

	/* pass 1, find the first set that has free_nodes that will fit */

	for (i = 0; i < node_grp[grp_inx].num_set; i ++) {
	    set_inx = node_grp[grp_inx].set_inx[i];
	    if (period_inx == COMBO || 
	    period_inx == node_grp[grp_inx].time_flag[i]) {
	    	root_blk = node_set[set_inx].root_blk[layer_inx];
	    	if (root_blk->free_nodes >= req_nodes) {
		    if (alloc_blk (j_req, req_nodes, root_blk) != 0) {
		        printf ("I$alloc_set: Problem with alloc_set for %d.\n",
		        j_req->orig_seqno);
		        return (-1);
		    } else {
		    	return (0);
	   	    }
	    	}
	    }
	}

	/* pass 2, just fill them in the set_inx order */

	if (req_nodes <= 0)
	    return (-1);
	nodes_remain = req_nodes;
        for (i = 0; i < node_grp[grp_inx].num_set; i ++) {
            set_inx = node_grp[grp_inx].set_inx[i];
            if (period_inx == COMBO ||
            period_inx == node_grp[grp_inx].time_flag[i]) {
                root_blk = node_set[set_inx].root_blk[layer_inx];
		if (root_blk->free_nodes <= 0)
		    continue;
		if (nodes_remain > root_blk->free_nodes)
		    tmp_nodes = root_blk->free_nodes;
		else
		    tmp_nodes = nodes_remain;

                if (alloc_blk (j_req, tmp_nodes, root_blk) != 0) {
                    printf ("I$alloc_set: Problem with alloc_blk for %d.\n",
                    j_req->orig_seqno);
		    rel_blkinjob (j_req);
                    return (-1);
                }
		nodes_remain -= tmp_nodes;
	    	if (nodes_remain <= 0)
		    return (0);		/* done */
	    }
        }
	printf ("I$alloc_set: Problem with free node counts.\n");
	rel_blkinjob (j_req);
	return (-1);
}


/* find_inuse_que ()
 *
 * Get the root partition of the inuse job.
 *
 */
struct job_que *find_inuse_que (j_req)
struct job_req *j_req;
{
	struct job_req *tj_req;
	int i, j;

	if (j_req == NULL)
		return (NULL);

	tj_req = j_req;

	/* move to the top of the queue */

	while (tj_req->prev != NULL) {
		tj_req = tj_req->prev;
	}

	for (i = 0; i < MAX_LAYER; i++) {
	    for (j = 0; j < MAX_PERIOD + 1; j++) {
		if (tj_req == layer[i].inuse_que[j].top) {
			return (&layer[i].inuse_que[j]); 
		}
	    }
	}

	return (NULL);
}


/*** set_block_ts ()
 *
 * A routine that set the the partition block_ts flag. It is called when 
 * we are unable to schedule a job and see if blocking further time share
 * should be done.
 *
 */
int set_block_ts (tj_req)
struct job_req *tj_req;
{
        int i, j;
        int grp_inx;
        int set_inx;
        int major_sz, minor_sz;
        int blocked_sz;
        int ts_minor;           /* nodes used or blocked in minor in the ts
                                 * layer. */
        int base_minor;           /* nodes used or blocked in minor in the base
                                 * layer. */
	int layer_inx;



	if (tj_req == NULL)
		return (-1);

	if (TIMESHARE == 0)
		return (0);

	grp_inx = tj_req->grp_inx;
        if (grp_inx < 0 ||  grp_inx >= MAX_GROUP) {
            printf ("I$set_block_ts: grp_inx = %d out of range\n", grp_inx);
            return (-1);
        }

	/* already blocked */

        for (i = 0; i < node_grp[grp_inx].num_set; i++) {
	    for (j = 0; j < MAX_LAYER - 1; j++) {
            	set_inx = node_grp[grp_inx].set_inx[i];
            	if (node_set[set_inx].blocked[j] > 0)
                    return (0);
	    }
        }

 
	/* sufficient priority */

	if (tj_req->prior < TIMESH_PRI ||
	tj_req->prior < BLOCK_PRI || BLOCK_PRI < 0)
		return (0);

	/* too big ? */

        major_sz = node_grp[grp_inx].size[MAJOR];
        minor_sz = node_grp[grp_inx].size[MINOR];
        blocked_sz = 0;
	if (tj_req->req_nodes > major_sz + minor_sz)
		return (0);

        base_minor = minor_sz - node_grp[grp_inx].free_nodes[BASE_LAYER][MINOR];
        ts_minor = minor_sz - node_grp[grp_inx].free_nodes[TS_LAYER][MINOR];

	/* Large enough to be placed in combo partition ? */

	if (tj_req->req_nodes > major_sz && 
	tj_req->req_nodes > minor_sz &&
	major_sz != 0 && minor_sz != 0) { 
	    if (glb.cur_mode == NPRIME_M) {
		/* decide which layer to block */
		if (ts_minor > 0)
		    layer_inx = TS_LAYER;
		else
		    layer_inx = BASE_LAYER;

            	for (i = 0; i < node_grp[grp_inx].num_set; i++) {
                    set_inx = node_grp[grp_inx].set_inx[i];
                    node_set[set_inx].blocked[layer_inx] = 1;
                    blocked_sz += node_set[set_inx].size;
                    if (blocked_sz >= tj_req->req_nodes)
                    	return (0);
                }
	    }
	    return (0);
	}

	/* Try MINOR, then MAJOR node_sets */

	if (tj_req->req_nodes <= minor_sz &&
	glb.cur_mode == NPRIME_M) { 
	    if (ts_minor > 0)
		layer_inx = TS_LAYER;
	    else
		layer_inx = BASE_LAYER;

            for (i = 0; i < node_grp[grp_inx].num_set; i++) {
                set_inx = node_grp[grp_inx].set_inx[i];
                if (node_grp[grp_inx].time_flag[i] != MINOR)
                    continue;
                node_set[set_inx].blocked[layer_inx] = 1;
                blocked_sz += node_set[set_inx].size;
                if (blocked_sz >= tj_req->req_nodes)
                    return (0);
            }
	}

	if (tj_req->req_nodes <= major_sz) {
            for (i = 0; i < node_grp[grp_inx].num_set; i++) {
                set_inx = node_grp[grp_inx].set_inx[i];
                if (node_grp[grp_inx].time_flag[i] != MAJOR)
                    continue;
                node_set[set_inx].blocked[TS_LAYER] = 1;
                blocked_sz += node_set[set_inx].size;
                if (blocked_sz >= tj_req->req_nodes)
                    return (0);
            }
	}
	return (0);
}

/*** set_block ()
 *
 * A routine that set the the partition block flag for the base layer. It is 
 * called when we are unable to schedule a job and see if blocking
 * should be done.
 *
 */
int set_block (tj_req)
struct job_req *tj_req;
{
	int i, j;
	int grp_inx;
	int set_inx;
	int major_sz, minor_sz;
	int blocked_sz;

	if (tj_req == NULL)
		return (-1);

	if (TIMESHARE == 1)
		return (0);

	grp_inx = tj_req->grp_inx;
        if (grp_inx < 0 ||  grp_inx >= MAX_GROUP) {
            printf ("I$set_block: grp_inx = %d out of range\n", grp_inx);
            return (-1);
        }

	/* already blocked ? */

        for (i = 0; i < node_grp[grp_inx].num_set; i++) {
            set_inx = node_grp[grp_inx].set_inx[i];
	    if (node_set[set_inx].blocked[BASE_LAYER] > 0)
		return (0);
	}

	/* sufficient priority */

	if (tj_req->prior < BLOCK_PRI || BLOCK_PRI < 0)
		return (0);

	/* too big ? */

	major_sz = node_grp[grp_inx].size[MAJOR];
	minor_sz = node_grp[grp_inx].size[MINOR];
	blocked_sz = 0;
	if (tj_req->req_nodes > major_sz + minor_sz) 
		return (0);

	/* Large enough to be placed in combo partition ? */

	if (tj_req->req_nodes > major_sz && 
	tj_req->req_nodes > minor_sz &&
	major_sz != 0 && minor_sz != 0) { 
	    if (NP_OVERRUN == OK_OVERRUN || glb.cur_time24 + tj_req->req_time
	    <= glb.p_start) {
            	for (i = 0; i < node_grp[grp_inx].num_set; i++) {
                    set_inx = node_grp[grp_inx].set_inx[i];
	            node_set[set_inx].blocked[BASE_LAYER] = 1;
	            blocked_sz += node_set[set_inx].size;
	            if (blocked_sz >= tj_req->req_nodes)
		    	return (0);
		}
		return (0);
	    }
	}

	/* Try root[MINOR] partition, then root[MAJOR] partition */

	if (tj_req->req_nodes <= minor_sz &&
	glb.cur_mode == NPRIME_M) { 
	    if (NP_OVERRUN == OK_OVERRUN || glb.cur_time24 + tj_req->req_time
	    <= glb.p_start) {
            	for (i = 0; i < node_grp[grp_inx].num_set; i++) {
                    set_inx = node_grp[grp_inx].set_inx[i];
		    if (node_grp[grp_inx].time_flag[i] != MINOR)
		    	continue;
                    node_set[set_inx].blocked[BASE_LAYER] = 1;
                    blocked_sz += node_set[set_inx].size;
                    if (blocked_sz >= tj_req->req_nodes)
                    	return (0);
            	}
            	return (0);
	    }
        }

	if (tj_req->req_nodes <= major_sz) {
            for (i = 0; i < node_grp[grp_inx].num_set; i++) {
                set_inx = node_grp[grp_inx].set_inx[i];
                if (node_grp[grp_inx].time_flag[i] != MAJOR)
                    continue;
                node_set[set_inx].blocked[BASE_LAYER] = 1;
                blocked_sz += node_set[set_inx].size;
                if (blocked_sz >= tj_req->req_nodes)
                    return (0);
            }
        }
	return (0);
}

/*** read_phynode ()
 *
 * Give the partition name, read the physical node array from the .partinfo
 * file. Retrun a pointer and the number of nodes read.
 * Return a NULL if error.
 * 
 */
int *read_phynode (part, node_cnt)
char *part;
int *node_cnt;
{
        FILE *pi_fd;            /* fd for the .partinfo file */
        char pi_path[PART_NAME_LEN];
        int temp, i, c;
        char buf[8];
        char *bp;
	int *phy_node;		/* the physical node array */
	int count;


        if (part_to_dir (part, pi_path, &temp) < 0)
            return (NULL);
 
        strcat (pi_path, "/");
        strcat (pi_path, PARTINFO);
 
        /* open the partinfo file */
 
        if ((pi_fd = fopen (pi_path , "r")) == NULL) {
            printf ("I$Unable to open partinfo file %s.\n", pi_path);
            fflush (stdout);
            return (NULL);
        }
       
        /* throw away the first 3 lines */
 
        for (i = 0; i < 3; i++) {
            while ((c = getc (pi_fd)) != '\n') {
                if (c == EOF) {
            	    printf ("I$partinfo file %s too short\n", pi_path);
                    fclose (pi_fd);
                    return (NULL);
                }
                continue;
            }
        }

	/* read node_cnt */

        buf[0] = '\0';
        bp = buf;
        while ((c = getc (pi_fd)) != '\n') {
            if (c == EOF)
                break;
            *bp ++ = c;
        }
        *bp = '\0';
        count = atoi (buf);

        if ((phy_node = (int *) malloc (count * sizeof (int))) == 
	(int *) NULL) {
             printf ("I$Read_phynode : malloc problem\n");
             fclose (pi_fd);
             return (NULL);
	}

        /* throw away the next 6 lines */
 
        for (i = 0; i < 6; i++) {
            while ((c = getc (pi_fd)) != '\n') {
                if (c == EOF) {
            	    printf ("I$partinfo file %s too short\n", pi_path);
                    fclose (pi_fd);
                    return (NULL);
                }
                continue;
            }
        }

	/* read in the physical node array */

	for (i = 0; i < count; i ++) {
            buf[0] = '\0';
            bp = buf;
            while ((c = getc (pi_fd)) != ',') {
                if (c == EOF || c == '\n') {
		    if (i < count - 1) {
            	        printf ("I$phy_node in file %s too short\n", pi_path);
                    	fclose (pi_fd);
                    	return (NULL);
                    } else {
                	break;
		    }
		}
            	*bp ++ = c;
	    }
            *bp = '\0';
            phy_node[i] = atoi (buf);
	}
        fclose (pi_fd);
	*node_cnt = count;
	return (phy_node);
}


/*** mkpart()
 *
 * make the partition for the job.
 * If successful, return the partition id. If not, return -1.
 *
 */
int mkpart (j_req)
struct job_req *j_req;
{
	int *rootp_phynode;
	int rootp_nodecnt;
	int phynode;
	int st_phynode;
	int w, h, i, j, tmp;
	struct block *blk; 
	char part_name[PART_NAME_LEN];
	struct job_req *tj_req;
	int part_pri;	
	int tmp_inx;

	if((blk = j_req->blk_que.top) == NULL) {
	    printf ("I$mkpart: Problem with block struct\n");
	    return (-1);
	}

	tmp_inx = blk->node_set->rootp_inx;
	rootp_phynode = rpart[tmp_inx].phynode;
	rootp_nodecnt = MESH_W * MESH_H;
	if (blk->layer_inx == TC_LAYER) {
	    part_pri = PREEMPT_PART_PRI;
	} else {
            part_pri = HIGH_PART_PRI;
	}

	/* Loop through the job's blk_que */

	j = 0;
	while (blk != NULL) {
	    if (xy_to_phynode (&st_phynode, &blk->x, &blk->y) != 0) {
                printf ("I$mkpart: xy_to_phynode error.\n");
		return (-1);
	    }
	    for (h = 0; h < blk->height; h ++) {
	        for (w = 0; w < blk->width; w ++) {
		    phynode = st_phynode + w + h * MESH_W;

		    /* convert from physical node to logical node */

		    for (i = 0; i < rootp_nodecnt; i ++) {
		        if (phynode == rootp_phynode[i]) {
			    glb.work_lognode[j] = i;
			    j ++;
			    break;
		        }
		    }
		    if (i == rootp_nodecnt) {	/* no match */
            	        printf ("I$mkpart: no match for phynode %d\n", phynode);
		        return (-1);
		    }
	        }
	    }
	    blk = blk->next;
	}

	/* sort the lognode in increasing order */

	if (bubble_sort (glb.work_lognode, j_req->req_nodes) < 0)
	    printf ("I$mkpart: Problem with sort.\n");

        sprintf (part_name, "%s.%-s%-d_%-d", rpart[tmp_inx].part_name,
        PART_PFIX, j_req->orig_mid, j_req->orig_seqno);
	strncpy (j_req->part_name, part_name, PART_NAME_LEN - 1);
	j_req->part_name[PART_NAME_LEN - 1] = '\0';
	if (nx_mkpart_map 
#ifdef R1_1
	(part_name, j_req->req_nodes, glb.work_lognode, NX_GANG) < 0) {
#else
	(part_name, j_req->req_nodes, glb.work_lognode, NX_SPS) < 0) {
#endif
	    printf ("I$mkpart: Unable to nx_mkpart_map, errno = %d, part_name = %s\n", errno, part_name);
	    return (-1);
	}
	if ((j_req->part_id = fnx_get_partid(part_name)) < 0) {
	    printf ("I$mkpart: Unable to fnx_get_partid.\n");
	    nx_rmpart (part_name, 1, 1);
	    return (-1);
	}
 
#ifdef R1_1
	if (nx_chpart_rq (part_name, ROLLIN_QUAN) < 0) {
	    printf ("I$mkpart: Unable to nx_chpart_rq.\n");
	    nx_rmpart (part_name, 1, 1);
	    return (-1);
	}
#endif R1_1
	if (nx_chpart_epl (part_name, part_pri) < 0) {
	    printf ("I$mkpart: Unable to nx_chpart_epl.\n");
	    nx_rmpart (part_name, 1, 1);
	    return (-1);
	}
	if (nx_chpart_mod (part_name, PART_MOD) < 0) {
	    printf ("I$mkpart: Unable to nx_chpart_mod.\n");
	    nx_rmpart (part_name, 1, 1);
	    return (-1);
	}
 
	if (nx_chpart_owner (part_name, j_req->uid, 1) < 0) {
	    printf ("I$mkpart: Unable to nx_chpart_owner.\n");
	    nx_rmpart (part_name, 1, 1);
	    return (-1);
	}
	return (j_req->part_id);
}

/*** bubble_sort ()
 *
 * sort the integer array in increasing order.
 *
 */
int bubble_sort (i_array, num)
int *i_array;
int num; 
{
	int i, j, tmp;

	if (i_array == NULL) 
	    return (-1);

	/* sort the lognode in increasing order */

        for (i = 1; i < num; ++i) {
            for (j = num - 1; j >= i; --j) {
                if (i_array[j - 1] > i_array[j]) {
                    tmp = i_array[j - 1];
                    i_array[j - 1] = i_array[j];
                    i_array[j] = tmp;
                }
            }    
        }    
	return (0);
}

/*** get_partpri ()
 *
 * given a partitiion name, get the partition priority
 *
 */
int get_partpri (part_name)
char *part_name;
{
	FILE *pi_fd;		/* fd for the .partinfo file */
	char pi_path[PART_NAME_LEN];
	int temp, i, c;
	char buf[8];
	char *bp;
	int part_pri;


	if (part_to_dir (part_name, pi_path, &temp) < 0)
	    return (-1);

	strcat (pi_path, "/");
	strcat (pi_path, PARTINFO);

        /* open the partinfo file */

        if ((pi_fd = fopen (pi_path , "r")) == NULL) {
	    /*
            printf ("I$get_partpri: Unable to open partinfo file %s.\n", 
	    pi_path);
            fflush (stdout);
	    */
            return (-2);
        }
	 
	/* throw away the first 9 lines */

	for (i = 0; i < 9; i++) {
	    while ((c = getc (pi_fd)) != '\n') {
		if (c == EOF) {
		    fclose (pi_fd);
		    return (-1);
		}
		continue;
	    }
	}

	/* read part_pri */

        buf[0] = '\0';
        bp = buf;
        while ((c = getc (pi_fd)) != '\n') {
            if (c == EOF)
                break;
            *bp ++ = c;
        }
        *bp = '\0';
        if ((part_pri = atoi (buf)) < 0) {
	    printf ("I$get_part_pri : Inconsistent part_pri\n");
            fclose (pi_fd);
	    return (-1);
	}
	fclose (pi_fd);
	
	return (part_pri);
}

/*** set_rootp_attri ()
 *
 * Set the rollin quantum and part_pri of NQS jobs.
 *
 */
int set_rootp_attri ()
{
        struct job_req *tj_req;
	int nqs_pri[MAX_PERIOD + 1];
	int i, j, k, tmp_pri;
	struct node_set *tmp_set;

	nqs_pri[MAJOR] = AVE_PART_PRI;
	if (glb.cur_mode == NPRIME_M) 
	    nqs_pri[MINOR] = nqs_pri[COMBO] = HIGH_PART_PRI;
	else
	    nqs_pri[MINOR] = nqs_pri[COMBO] = LOW_PART_PRI;

        for (i = 0; i < MAX_LAYER; i++) {
	  for (k = 0; k <= MAX_PERIOD; k++) {
	    if ( i == TC_LAYER)
		nqs_pri[MAJOR] = nqs_pri[MINOR] = nqs_pri[COMBO]
		= PREEMPT_PART_PRI;

            tj_req = layer[i].inuse_que[k].top;
            while (tj_req != NULL) {
	    	if ((tmp_set = tj_req->blk_que.top->node_set) == NULL) {
		    printf ("I$set_rootp_attri: Inconsistent node_set.\n");
		    return (-1);
		}
		sprintf (tj_req->part_name, "%s.%-s%-d_%-d",
		rpart[tmp_set->rootp_inx].part_name, PART_PFIX, 
		tj_req->orig_mid, tj_req->orig_seqno);
	    	if ((j = get_partpri (tj_req->part_name)) != nqs_pri[k] && 
		j >= 0) {
		    if (nx_chpart_epl (tj_req->part_name, nqs_pri[k]) < 0) {
		    	printf ("I$set_rootp_attri: nx_chpart_epl error.\n");
		    }
		    else {
		      if (Debug > 2) {
			printf ("I$set_rootp_attri:nx_chpart_epl(%s,%d) O.K.\n",
				tj_req->part_name, nqs_pri[k]);
		      }
		    }
	    	}
                tj_req = tj_req->next;
            }
	  }
	}


	if (glb.cur_mode == NPRIME_M) {
            if (OPEN_NP_NAME[0] != NULL &&
            CUR_OPEN_NAME[0] != NULL && OPEN_P_NAME[0] != NULL) {
                if ((tmp_pri = get_partpri (OPEN_NP_NAME)) >= 0) { /* exist */
                    if (get_partpri (CUR_OPEN_NAME) >= 0) { /* switch name */
                        if (nx_chpart_name(CUR_OPEN_NAME,
                        get_endpart(OPEN_P_NAME)) < 0) {
                            printf ("I$set_root_attri: nxchpart_name error\n");
                        } else {
                    	    if (nx_chpart_epl (OPEN_P_NAME, LOW_PART_PRI) < 0)
                        	printf ("I$set_rootp_attri:chpart_epl error\n");
                            if (DO_WALL > 0)
                                do_wall (glb.cur_mode);
			    vLogModeSwitch (glb.cur_mode);
                        }
                    }
                    if (nx_chpart_epl (OPEN_NP_NAME, HIGH_PART_PRI) < 0)
                        printf ("I$set_rootp_attri: nx_chpart_epl error.\n");
		    if (nx_chpart_name(OPEN_NP_NAME, 
		    get_endpart (CUR_OPEN_NAME)) < 0) 
		        printf ("I$set_root_attri: nxchpart_name error\n");
		} else if (tmp_pri == -2 && get_partpri (CUR_OPEN_NAME) == -2) {
		    printf ("I$set_root_attri: Partition %s does not exist.\n",
		    OPEN_NP_NAME);
		}
	    } else if (OPEN_P_NAME[0] != NULL) {  /* only open_p exists */
	        if ((tmp_pri = get_partpri (OPEN_P_NAME)) >= 0) {
		    if (tmp_pri != LOW_PART_PRI) {
                        if (nx_chpart_epl (OPEN_P_NAME, LOW_PART_PRI) < 0) {
                            printf ("I$set_rootp_attri:chpart_epl error\n");
			} else {
                            if (DO_WALL > 0)
                                do_wall (glb.cur_mode);
			    vLogModeSwitch (glb.cur_mode);
			}
		    }
		} else if (tmp_pri == -2) {
                    printf ("I$set_root_attri: Partition %s does not exist.\n",
                    OPEN_P_NAME);
		}
	    }
	} else {
	    if (OPEN_NP_NAME[0] != NULL && 
	    CUR_OPEN_NAME[0] != NULL && OPEN_P_NAME[0] != NULL) {
	    	if ((tmp_pri = get_partpri (OPEN_P_NAME)) >= 0) {
		    if (get_partpri (CUR_OPEN_NAME) >= 0) { /* switch name */
			if (nx_chpart_name(CUR_OPEN_NAME, 
			get_endpart (OPEN_NP_NAME)) < 0) {
			    printf ("I$set_root_attri: nxchpart_name error\n");
			} else {
                    	    if (nx_chpart_epl (OPEN_P_NAME, HIGH_PART_PRI) < 0)
                        	printf ("I$set_rootp_attri:chpart_epl error\n");
			    if (DO_WALL > 0)
			    	do_wall (glb.cur_mode);
			    vLogModeSwitch (glb.cur_mode);
			}
		    }
		    if (nx_chpart_epl (OPEN_NP_NAME, LOW_PART_PRI) < 0) 
		        printf ("I$set_rootp_attri: nx_chpart_epl error.\n");
		    if (nx_chpart_name(OPEN_P_NAME, 
		    get_endpart (CUR_OPEN_NAME)) < 0) 
		    	printf ("I$set_root_attri: nxchpart_name error\n");
		} else if (tmp_pri == -2 && get_partpri (CUR_OPEN_NAME) == -2) {
		    printf ("I$set_root_attri: Partition %s does not exist.\n",
		    OPEN_P_NAME);
	        }
	    } else if (OPEN_P_NAME[0] != NULL) {  /* only open_p exists */
	        if ((tmp_pri = get_partpri (OPEN_P_NAME)) >= 0) {
		    if (tmp_pri != HIGH_PART_PRI) {
                        if (nx_chpart_epl (OPEN_P_NAME, HIGH_PART_PRI) < 0) {
                            printf ("I$set_rootp_attri:chpart_epl error\n");
			}
		    }
		} else if (tmp_pri == -2) {
                    printf ("I$set_root_attri: Partition %s does not exist.\n",
                    OPEN_P_NAME);
		} 	
	    }
	}
        if ( sModeChange == 1)
        {
           (void) vPerformBatchScript(glb.cur_mode);
           sModeChange = 0;
        }
	return (0);
}
 
/*** do_wall ()
 *
 * send a broadcast message.
 *
 */
void do_wall (mode)
int mode;
{
        FILE *fp;
        char commandstr[256];
        sprintf(commandstr, "%s", WALLPROG);

        /* Open a pipe to the mail program */

        if((fp = popen (commandstr, "w")) == NULL) {
                printf ("I$do_wall: unable to open pipe\n");
                return;
        }

        /* Body of the message */

	if (mode == PRIME_M) {
	    fprintf (fp, "The current OPEN partition has been switched ");
	    fprintf (fp, "from Non-prime to Prime.\n");
	    fprintf (fp, "Your parallel applications running in the ");
	    fprintf (fp, "Non-prime OPEN partition may be suspended\n");
	} else {
	    fprintf (fp, "The current OPEN partition has been switched ");
	    fprintf (fp, "from Prime to Non-prime.\n");
	    fprintf (fp, "Your parallel applications running in the ");
	    fprintf (fp, "Prime OPEN partition may be suspended\n");
	}
        fflush (fp);
        pclose(fp);
}

/*** vLogModeSwitch ()
 *
 * log mode switch  message.
 *
 */
void vLogModeSwitch (mode)
int mode;
{
  if (Debug > 2) {
	if (mode == PRIME_M) {
	    printf("I$The current OPEN partition has been switched from Non-prime to Prime.\n");
	} else {
	    printf("I$The current OPEN partition has been switched from Prime to Non-prime.\n");
	}
        fflush (stdout);
      }
}

/*** vPerformBatchScript()
 *
 * run script when changing between prime <-> non_prime period
 *
 */
 
void vPerformBatchScript(mode)
int mode;
{
   struct stat     stat_buf;    /* fstat() buffer */
   int             iBatchPid = (-1);
   char            sTmp[2*MAX_PATHNAME+1], s[MAX_PATHNAME+1];
 
   if (mode == PRIME_M) {
     strncpy(s, SCR2PRIM, MAX_PATHNAME);
   } else {
     strncpy(s, SCR2NONPRIM, MAX_PATHNAME);
   }
 
   if (s[0] == '\0') return;
   if (0 != stat(s, &stat_buf)) {
     if (sys_nerr >  errno)
       printf("E$Cannot stat file %s. %s\n", s, sys_errlist[errno]);
     else
       printf("E$Cannot stat file %s. errno = %d\n", s, errno);
     return;
   }
 
   switch (stat_buf.st_mode & S_IFMT) {
   case S_IFIFO:
      printf("E$File %s is a pipe.\n", s);
      return;
      break;                    /* Pipe */
   case S_IFCHR:
      printf("E$File %s is a character device.\n", s);
      return;
      break;                    /* Character device */
   case S_IFDIR:
      printf("$EFile %s is a directory.\n", s);
      return;
      break;
   case S_IFBLK:
      printf("$EFile %s is a block device.\n", s);
      return;
      break;
 
   case S_IFSOCK:
      printf("$EFile %s is a socket.\n", s);
      return;
      break;
 
   default:
      printf("$EFile %s of unknown type.\n", s);
 
   case S_IFREG:
   case S_IFLNK:
      /* regular file */
      if ((stat_buf.st_mode & S_IXUSR) == 0) {
         printf("$EFile %s doesn't have execute permission set. Mode = %o\n",
                s, stat_buf.st_mode);
      }
      break;                    /* Ordinary file */
   }
 
    if (Debug > 2)
     printf("I$Starting script %s.\n", s);
 
   if (stat_buf.st_size == 0) {
      printf("$EFile %s is empty.\n", s);
      return;
   }
   fflush(stdout);              /* Flush any diagnostic messages */
   fflush(stderr);              /* before system() call */
   strcpy(sTmp, s);
   strcat(sTmp, " >> /dev/null 2>&1 ");
   if (system(sTmp) < 0) {
          if (sys_nerr >  errno)
	     printf(
		   "E$system() call for file %s failed. %s (errno =%d)\n",
		    s, sys_errlist[errno], errno);
	  else
	    printf(
		   "E$system() call for file %s failed. (errno =%d)\n",
		   s, errno);
   }
   if (Debug > 2)
     printf("I$Script %s finished.\n", s);
}
 

/* get_endpart ()
 *
 * Routine that gets the partition at the end of an input path.
 * e.g., input - .compute.foo
 * returned value : A pointer to "f"
 */
char *get_endpart (part_name)
char *part_name;
{
	char *cptr, *partptr;
	int part_len;

        cptr = part_name;
        partptr = part_name;
	part_len = 0;
 
        while (*cptr != NULL && part_len < PART_NAME_LEN - 1) {
                if (*cptr == '.')
                        partptr = cptr + 1;
                cptr ++;
		part_len ++;
        }
	return (partptr);
}

/*** chk_rootp ()
 *
 * Check if the nodes in each node sets are in the .compute
 * partition. Take out any nodes that are not.
 *
 */
int chk_rootp ()
{
        int *rootp_phynode;
        int rootp_nodecnt;
	int i, j, k, l, h, w;
	int tmp_inx;
	struct block blk; 
	struct block *root_blk; 
	struct block *t_blk;
	int st_phynode;
	int phynode;

        rootp_nodecnt = MESH_W * MESH_H;
        for (k = 0; k < glb.num_set; k++) {
            tmp_inx = glb.set_inx[k];
            rootp_phynode = rpart[node_set[tmp_inx].rootp_inx].phynode;
	    root_blk = node_set[tmp_inx].root_blk[0];
            if (xy_to_phynode (&st_phynode, &root_blk->x, &root_blk->y) != 0) {
                printf ("I$chk_rootp: xy_to_phynode error.\n");
                return (-1);
            }
            for (h = 0; h < root_blk->height; h ++) {
                for (w = 0; w < root_blk->width; w ++) {
                    phynode = st_phynode + w + h * MESH_W;

                    /* convert from physical node to logical node */

                    for (i = 0; i < rootp_nodecnt; i ++) {
                        if (phynode == rootp_phynode[i]) {
                            break;
                        }
                    }

                    if (i == rootp_nodecnt) {   /* no match */
                        printf ("I$Phynode %d is not in .compute. Removed.\n", 
			phynode);
			fflush (stdout);
			blk.width = 1;
			blk.height = 1;
			blk.x = root_blk->x + w;
			blk.y = root_blk->y + h;
			blk.free_nodes = 1;

			/* remove the blk from each layer */

			for (l = 0; l < MAX_LAYER; l++) {
            		    if ((t_blk = match_blk (&blk, 
			    node_set[tmp_inx].root_blk[l])) != NULL) {
                	    	if (adj_count (t_blk, -1) < 0) {
                    	            printf ("I$chk_rootp: adj_count problem\n");
				    fflush (stdout);
			            return (-1);
			    	}
			        t_blk->no_combo = 1;
			    }
			}
			node_set[tmp_inx].size --;
                    }
                }
            }
	}
	return (0);
}

/*** fnx_get_partid ()
 *
 * will be replace by real nx call.
 * We have problem with the current fnx_get_partid.
 *
 */
int fnx_get_partid (part)
char *part;
{
#ifdef OSFTEST
	return (0);
#else
	int i;
	int part_id;
	char dir[PART_NAME_LEN];
	struct stat fs;    /* file status structure */

	/* get the inode of the partition */

	if (part_to_dir (part, dir, &i) < 0) {
	    part_id = -1;
	} else {
	    if(stat(dir, &fs) < 0 ) {
		printf ("I$fnx_get_partid: problem getting part directory\n");
		part_id = -1;
	    } else {
		part_id = (int) fs.st_ino;
	    }
	}
	return (part_id);
#endif
}

#ifdef NO_NXCALL
/*** nx_rmpart ()
 *
 * A dummy nx routine.
 *
 */
int nx_rmpart (part, force)
char *part;
int force;
{
	char command[128];

	sprintf (command, "/usr/bin/rmpart %s", part);
	printf ("I$nx_rmpart for %s\n", part);
	printf ("I$nx_rmpart: command line: %s\n", command);
	fflush (stdout);
	system (command);
	printf ("I$nx_rmpart: Done\n");
	fflush (stdout);
	return (0);
}

/*** nx_chpart_name ()
 *
 * A dummy nx routine.
 *
 */
int nx_chpart_name (old_part, new_part)
char *old_part;
char *new_part;
{
	printf ("I$nx_chpart_name: from %s to %s\n", old_part, new_part);
	return (0);
}

/*** nx_mkpart_map ()
 *
 * A dummy nx routine.
 *
 */
int nx_mkpart_map (part, size, map, attrib)
char *part;
int size;
int *map;
int attrib;
{
	int i;
	char command[4096];
	char *ptr;
	long status;


	strcpy (command, "/usr/bin/mkpart -nd ");
	printf ("I$nx_mkpart_map: mkpart for %s\n", part);
	printf ("I$lognode :\n");
	for (i = 0; i < size; i ++) {
	    printf ("I$%d\n", map[i]);
	    ptr = command + strlen (command);
	    if (i == 0)
	        sprintf (ptr, "%-d", map[i]);
	    else
	        sprintf (ptr, ",%-d", map[i]);
	}
	ptr = command + strlen (command);
	sprintf (ptr, " %s", part);
	printf ("I$nx_mkpart_map: command line: %s\n", command);
	fflush (stdout);
#ifndef OSFTEST
	system (command);
#endif

	return (0);
}

/*** nx_chpart_owner ()
 *
 * A dummy nx routine.
 *
 */
int nx_chpart_owner (part, uid, gid)
char *part;
int uid;
int gid;
{
	printf ("I$nx_chpart_owner for %s, uid = %d, gid = %d\n", 
	part, uid, gid);
	return (0);
}

/*** nx_chpart_mod ()
 *
 * A dummy nx routine.
 *
 */
int nx_chpart_mod (part, mod)
char *part;
int mod;
{
	printf ("I$nx_chpart_mod for %s, mod = %d\n", part, mod);
	fflush (stdout);
	return (0);
}

/*** nx_chpart_rq ()
 *
 * A dummy nx routine.
 *
 */
int nx_chpart_rq (part, rollin_quan)
char *part;
int rollin_quan;
{
	printf ("I$nx_chpart_rq for %s, rollin_quan = %d\n", 
	part, rollin_quan);
	fflush (stdout);
	return (0);
}

/*** nx_chpart_epl ()
 *
 * A dummy nx routine.
 *
 */
int nx_chpart_epl (part, maxpri)
char *part;
int maxpri;
{
	printf ("I$nx_chpart_epl for %s, maxpri = %d\n", 
	part, maxpri);
	fflush (stdout);
	return (0);
}
#endif

#endif
