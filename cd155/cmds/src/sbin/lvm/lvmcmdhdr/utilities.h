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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */

/*
 * OSF/1 Release 1.0
 */

/*
 *   utilities.h
 *   
 *   Contents:
 *	General purpose functions, useful for many commands.
 *	See the LVM commands library module "utilities.c".
 */

/* Modes for isblock_and_clean() */
#define CHECKLVMTAB		TRUE
#define DONTCHECKLVMTAB		FALSE

/* Modes for deattach_pvs() */
#define WITH_WARNING		TRUE
#define NO_WARNING		(!WITH_WARNING)

char *checked_alloc(unsigned int bytes);
char *checked_realloc(char *old_area, unsigned int old_size,
		     unsigned int new_size);
char *memdup(char *area, unsigned int bytes);
int rm_dir(char *path);
int special_f_tst(char *path, int file_type, dev_t *devnum);
char *mk_clean_path(char *path);
char *mk_save_clean_path(char *path);
int install_pv(int vg_fd, char *vgname, struct lv_installpv *installpv, 
	       char **pv_paths, int pv_paths_cnt);
int isblock_and_clean(char *path, char **ret_path, int mode);
char *check_and_openvg(char *vgpath, int *vg_fd);
int attach_pvs(int vg_fd, char **pvs, int pvs_cnt);
int deattach_pvs(int vg_fd, char **pvs, int pvs_cnt, int mode);
int multiple_query_lv(int vg_fd, char **lv_names, unsigned int lv_cnt,
	  struct lv_querylv **querylv_array_ptr);
int multiple_query_pv(int vg_fd, char **pv_names, unsigned int pv_cnt,
	  struct lv_querypvpath **querypv_array_ptr, char **pv_queried);
int query_driver(int vg_fd, int op_code, void *buf);
void disable_intr();
