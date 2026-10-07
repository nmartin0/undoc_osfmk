/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * $Log: nx_struct.h,v $
 * Revision 1.3  1994/11/18  20:37:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  22:33:26  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:31:35  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:25:48  cfj
 * Bump major revision number.
 *
 * Revision 1.2  1992/07/28  09:50:06  nandy
 * NX integration
 *
 * Revision 1.2  92/06/11  16:03:47  cfj
 * Changed TAM_TABLE_SIZE and modified the tam_table structure.
 * 
 * Revision 1.1  92/06/11  11:26:11  cfj
 * Initial revision
 * 
 */

#define TAM_TABLE_SIZE (nproc >> 1)

typedef struct tam_table_entry {
    pid_t     pgrp;
    pid_t     tam_pid;
} TAM_TABLE_ENTRY, *TAM_TABLE_T;



