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
 
extern PART_T *
validate_allocator_access(ino_t parent_inode,ino_t inode,uid_t uid,
                         gid_t gid,int req_access,int * error);
extern int
select_nodes(int strictly_rect,int default_szflg,int size, 
	BITMAP_T *src_bitmap, BITMAP_T *dest_bitmap,
        LP_MAP_T src_lp, int src_lp_size,LP_MAP_T dest_lp,
	int *nrows,int *ncols);

extern int node_in_map(BITMAP_T *src_bitmap, LP_MAP_ENTRY_T node);

extern void
assign_rect_lp(BITMAP_T *bm, int size, int col, int row, int cols, int rows,LP_MAP_T lp);

extern int
deny_overlap(PART_T *part, LAYER_T *layer_list);

extern BITMAP_T *
overlay_layers(LAYER_T* layer_list);
