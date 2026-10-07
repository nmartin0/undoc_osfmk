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
 *   debug.h
 *   
 *   Contents:
 *	Useful functions for dumping debug information. This
 *	modules are bind to the commands only if "-DDEBUG" is
 *	specified at compile time.
 *	See the LVM commands library module "debug.c".
 *
 */

/* Values for the "mode" and "want_map" parameters */
#define DBG_BEFORE		TRUE
#define DBG_AFTER		FALSE
#define DBG_WITH_MAP		TRUE
#define DBG_WITHOUT_MAP		FALSE

void dbg_indent();
void dbg_entry(char *fname);
void dbg_exit();
void dbg_createvg_dump(struct lv_createvg *vgcr_struct);
void dbg_installpv_dump(struct lv_installpv *pvinst_struct);
void dbg_attachpv_dump(char *attachpv);
void dbg_queryvg_dump(struct lv_queryvg *vgqu_struct, int mode);
void dbg_querylv_dump(struct lv_querylv *qu_struct, int mode);
void dbg_querypv_dump(struct lv_querypv *qu_struct, int mode);
void dbg_querypvpath_dump(struct lv_querypvpath *dmp_struct, int mode);
void dbg_querypvmap_dump(struct lv_querypvmap *dmp_struct, int mode);
void dbg_lvsize_dump(struct lv_lvsize *dmp_struct, int mode, int want_map);
void dbg_statuslv_dump(struct lv_statuslv *dmp_struct);
void dbg_changepv_dump(struct lv_changepv *dmp_struct);
void dbg_lvID_dump(int *dmp_struct);
void dbg_pvID_dump(int *dmp_struct);
void dbg_activatevg_dump(int *dmp_struct);
int dummy_ioctl(int fd, int request, char *argp);
void dbg_removepv_dump(unsigned short pv_key);
