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
 
/* Function prototypes for utils.c
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/utils.h,v 1.4 1994/11/19 02:45:06 mtm Exp $
*/

/* History
 * $Log: utils.h,v $
 * Revision 1.4  1994/11/19  02:45:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/12/18  02:21:05  carbajal
 * pathname tools
 *
 * Revision 1.2  1992/10/13  00:40:47  carbajal
 * Added opendir_r and readdir_r as external prototypes.
 *
 * Revision 1.1  1992/10/08  19:32:54  carbajal
 * New .h files for function prototypes
 *
*/

/* External data declarations
*/

extern int verify_name(char *name);
extern int check_access(uid_t uid, gid_t gid, uid_t obj_uid, gid_t obj_gid, int op, int rights);
extern ino_t get_inode(char *filename);
extern int get_access(char *partname,PARTREQ_T *part);
extern char *create_partname(char *name);
extern void dottoslash(char *name);
extern void slashtodot(char *name);
extern int read_partinfo(FILE *fd,PARTREQ_T *partreq, BITMAP_T **bitmap, LP_MAP_T *lp_map);
extern int get_part_info(char *path, PARTREQ_T *partreq, BITMAP_T **bitmap,LP_MAP_T *lp_map);
extern int parse_mod(char *argument,int *access);
extern int isdir(char *path);
extern int isnumber(char *descr);
extern int check_range(long value,long lower,long upper );
extern int is_ipsc860();
extern int opendir_r(char *path, DIR *dir);
extern int readdir_r(DIR *dir, struct dirent *dent); 

extern char * init_pathtype(PATHTYPE *p);
extern char * append_path(PATHTYPE *p,char *name);
extern void free_path(PATHTYPE *p);
extern BITMAP_T *build_badbitmap(int max_nodes,BITMAP_T *src);

