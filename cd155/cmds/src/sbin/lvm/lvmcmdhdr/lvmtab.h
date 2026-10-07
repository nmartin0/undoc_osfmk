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
 *   lvmtab.h
 *   
 *   Contents:
 *	All the functions to deal with the "lvmtab" file, under "/etc/lvm".
 *	See the LVM commands library module "lvmtab.c".
 */

#define LVMTABPATH	"/etc/lvmtab"
#define SAVELVMTABPATH	"/etc/lvmtab.save"
#define LVMTABPERMISS	0600			/* permission bits */

/* Flags for "must_write" */
#define DOWRITE		TRUE
#define NOWRITE		FALSE

int lvmtab_read();
int lvmtab_write();
int lvmtab_getvgnames(char ***name_list, int *items_in_list);
int lvmtab_getpvnames(char *vg_path, char ***name_list, int *items_in_list);
int lvmtab_isvgdefined(char *vg_path);
int lvmtab_getvgid(char *vg_path, lv_uniqueID_t *vg_id);
int lvmtab_addvg(char *vg_path, lv_uniqueID_t *vg_id, int must_write);
int lvmtab_removevg(char *vg_path, int must_write);
int lvmtab_ispvinvg(char *vg_path, char *pv_path);
int lvmtab_ispvinsomevg(char *pv_path, char **vg_path);
int lvmtab_addpvtovg(char *vg_path, char *pv_path, int must_write);
int lvmtab_removepvfromvg(char *vg_path, char *pv_path, int must_write);
