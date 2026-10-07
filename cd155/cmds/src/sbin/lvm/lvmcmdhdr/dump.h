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
 *   dump.h
 *   
 *   Contents:
 *	All the functions to deal with dumping (i.e., visualizing information)
 *	about volume groups, logical volumes and physical volumes.
 *	See the LVM commands library module "dump.c".
 *
 */

/* Values for the mode argument */
#define TERSE		1
#define VERBOSE		2

int dump_vg(char *vg_path, int mode);
void dump_vg_title();
int dump_lv(char *lv_path, int mode);
void dump_lv_title();
int dump_pv(char *pv_path, int mode);
void dump_pv_title();
