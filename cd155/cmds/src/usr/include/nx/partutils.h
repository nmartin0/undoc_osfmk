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
 
/* Function prototypes for partutils.c
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/partutils.h,v 1.11 1994/11/19 02:45:03 mtm Exp $
*/

/* History
 * $Log: partutils.h,v $
 * Revision 1.11  1994/11/19  02:45:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/06/01  20:28:53  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): nx.h, fnx.h, allocsys.h, nx/alloc_types.h, nx/allocator.h
 * 	    nx/bitmap.h, nx/hash.h, nx/partprint.h, nx/partutils.h,
 * 	    nx/schedule.h
 *  Related: server, emulator, allocator, bootmesh, mkpart, showpart, lspart
 *
 * Revision 1.9  1993/11/17  20:30:13  carbajal
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Removed ask_for_allocator_port()
 *  Testing:
 *  Module(s):
 *
 * Revision 1.8  1993/11/17  02:49:04  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.7  1993/07/18  19:44:35  carbajal
 * Added support for chpart system call
 *
 * Revision 1.6  1993/07/13  21:16:03  carbajal
 * New calls added for allocator system call support
 * Passing in more parameters to allocator_cmds_setup()
 *
 * Revision 1.5  1993/05/06  18:19:29  carbajal
 * Removed write_partinfo() prototype
 *
 * Revision 1.4  1992/12/18  02:20:04  carbajal
 * allocator_cmds_setup
 *
 * Revision 1.3  1992/11/03  00:23:55  carbajal
 * No sizeable changes
 *
 * Revision 1.2  1992/10/13  00:39:48  carbajal
 * Added defpart to allocator_cmds_setup call
 *
 * Revision 1.1  1992/10/08  19:32:51  carbajal
 * New .h files for function prototypes
 *
*/

/* External function prototypes
*/

extern void
allocator_cmds_setup(int argcount, int argc, char *argv[], char **name, \
                     char **partname,char *progname,char *defpart,	\
                        PARTREQ_T *part, BITMAP_T **bitmap, LP_MAP_T *p_ndlist);

extern char *
get_partition_name(int argcount, int argc, char *argv[], char **name, \
		     char *progname,char *defpart);
extern int lock_part_info(char *path, FILE **fd,PARTREQ_T *partreq, BITMAP_T **bitmap,LP_MAP_T *lp_map);

extern
int
grab_part_info(char *path,PARTREQ_T *partreq,BITMAP_T **bitmap,LP_MAP_T *lp_map);

extern int
write_part_info(char *path,PARTREQ_T *partreq,BITMAP_T *bitmap,LP_MAP_T lp_map);

extern 
int
create_part_info(char *path,PARTREQ_T *partreq,BITMAP_T *bitmap,LP_MAP_T lp_map);

extern
int
NX_mkpart_attr(int flag,char *partname, nx_part_info_t partinfo, nx_node_list_t node_list, int relaxed, int selector_cnt, char* selector_buf, int selector_buflen);

extern
char *
marshall_mkpart_rpc(int flag, char *pathname,PARTREQ_T mypart, 
		BITMAP_T *my_bitmap,
                LP_MAP_T my_nodes,int num_nodes,int *buf_size);

extern
int
unmarshall_mkpart_rpc(int buf_size,char *buffer,
		int	*flag,
		char **pathname,
                PARTREQ_T *my_part,
                BITMAP_T **my_bitmap,LP_MAP_T *my_nodes,int *num_nodes);


extern 
char *
marshall_chpart_rpc(int *buf_size,char *pathname,PARTREQ_T *mypart,char *new_pathname);

extern
int
unmarshall_chpart_rpc(int buf_size,char *buffer,
                char    **pathname,
                char    **new_pathname,
                PARTREQ_T *my_part);

extern
int
NX_chpart(char *partname, nx_part_info_t partinfo, char *newname);

extern
char *
make_header_name(char *relative_to,char *name);

extern int find_initial(char *s1, char *s2);


extern char *filename_to_partname(char *filename);

