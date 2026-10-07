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
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: load_level_types.h,v $
 * Revision 1.12  1995/02/01  23:47:59  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.11  1994/11/19  03:07:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/11/18  20:53:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/05/27  12:55:39  stefan
 * The paging behavior of a node is included in the calculation of the load
 * value. Added definitions for paging statistics.
 *
 *  Reviewer: yazz
 *  Risk: low
 *  Benefit or PTS #:
 *  Testing: developer testing
 *  Module(s): load_level_types.h
 *
 * Revision 1.8  1994/02/16  16:28:48  stefan
 * Merged version 1.7.4.1 into main trunk.
 *
 * Revision 1.7.4.1  1994/02/16  14:38:44  stefan
 *  Reviewer: bolsen (Locus)
 *  Risk: low to medium
 *  Benefit or PTS #: 7899, 8028, 8039
 *  Testing: developer testing
 *  Module(s): svr/user/etc/load_level/load_level_types.h
 *             svr/user/etc/load_level/load_leveld.c
 *             svr/user/etc/load_level/loadlevel
 *             svr/user/etc/load_level/osf1_dep.c
 *             svr/user/etc/load_level/sll_load_level_types.h
 *             svr/user/etc/load_level/sll_load_leveld.c
 *             svr/user/etc/load_level/parameters
 *
 * For PTS #7899 I have added 2 new parameters to the load leveler daemon:
 * root_fs_node_target   boolean that specifies if root_fs_node should be
 *                       used as a target node for load leveling.
 *                       Default: 0
 * root_fs_node_source   boolean that specifies if root_fs_node should be
 *                       used as source node for load leveling.
 *                       Default: 1
 *
 * For PTS #8028 the default behaviour has been changed so that process
 * migration is disabled by default and can be switched on using the command
 * line switch -d.
 *
 * For PTS #8039 a check has been implemented if a load_leveld is already
 * running on a node and if this is the case the new load leveler exits with
 * an error message.
 * Also, I have fixed a problem where load_leveld dumped core in migrate() when
 * the startup node is not included in the nodes_to_use parameter.
 * Instead of migrating to another node load_leveld now exits on the startup
 * node after rforking it's children.
 * In addition, now it is checked if get_tnc_port() returns MACH_PORT_DEAD and
 * in this case it is assumed that the corresponding peer is simply a little
 * late.
 *
 * Revision 1.7  1993/07/14  18:50:54  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.6  1993/05/06  19:28:43  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.5  1993/07/01  21:17:01  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/01/22  19:30:23  stefan
 * Merged Locus 01-20-93 code drop.
 *
 * Revision 1.4  1992/12/14  13:45:41  stefan
 * Made adjustments for performance tunings and bug fixes:
 *   - struct node_info no longer needed - deleted.
 *   - extern int num_nodes replaced by extern int num_slots.
 *   - extern struct node_info *node_info_ptr replaced by
 *     extern char *node_in_use.
 *   - new variable extern int local_node_num.
 *
 * Revision 1.3  1992/11/30  23:01:40  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:36:09  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:49:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 1.1.1.3  1993/05/03  17:57:36  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.7  1993/04/23  16:17:15  bolsen
 * [SPE 0007] removed common structure definitions (ie. those common to
 * 	load_leveld and onnode (fast & fastnode)).
 *
 * Revision 3.6  92/12/30  16:00:54  mbarnett
 * The node_info structure was deleted and external declarations for num_slots,
 * node_info_ptr, and node_in_use were added.  These changes have to do with
 * resolving SPE #0008.
 * 
 * Revision 3.5  92/12/28  15:22:12  mbarnett
 * Added an external declaration for the new variable local_node_num which is
 * used to cache the return value from calling node_self.
 * 
 * Revision 3.4  92/10/23  15:20:12  mbarnett
 * Added declaration for new variable mypid which is used to cache the pid of the
 * local load leveler daemon.  This change is associated with bug #0084.
 * 
 * Revision 3.3  92/06/15  13:00:45  mbarnett
 * Removed proc_info structure and modified load_info structure.  Also, added
 * declaration of node_info structure and load_vec_info structure.
 * 
 * Revision 3.2  92/05/13  09:39:33  mbarnett
 * Fixed history log by removing duplicate log.
 * 
 * Revision 3.1  92/05/13  08:31:17  mbarnett
 * Added os and os-dependent common definitions and also added external 
 * declarations.
 * 
 * Revision 3.0  92/03/27  16:31:06  mbarnett
 * initial check-in
 *
 */

/* maximum length of command name (OS dependent) */
#define CMD_LEN 20

/*
 * Maximum number of page-fault statistics.
 */
#define MAX_PG_STATS	50

/*
 * structure for page-fault statistics
 */
struct pg_statistics {
	int	page_ins;
	int	page_outs;
	double	time_stamp;
};

/* external variables */
extern int *node_num_ptr;
extern int num_nodes;
extern struct load_info *load_info_ptr;
extern unsigned num_lvec_elements;
extern int inclusion_list;
extern double min_cputime;
extern int iparm;
extern pid_t mypid;
extern int num_slots;
extern int local_node_num;
extern char *node_in_use;
extern int root_fs_node;
extern int root_fs_node_target;
extern struct pg_statistics pg_stat[];
extern int pg_stat_index;
