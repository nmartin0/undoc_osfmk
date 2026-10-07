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
 
extern
BITMAP_T *
create_ndlst_bitmap(int parentsize,LP_MAP_T p_ndlist,BITMAP_T *parent_bitmap,
                        int my_size,LP_MAP_T l_ndlist,int *num_nodes,
                        LP_MAP_T *my_nodes);

