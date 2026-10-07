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
 
extern int perk_active_partition(PART_T *part);
extern int find_best_tile(int default_szflg,int want_numnodes, PART_T *part, LAYER_T **layer_list,
		LAYER_T **my_layer,BITMAP_T **my_bitmap,LP_MAP_T *my_nodes,
		int *nrows,int *ncols, int relaxed, BITMAP_T* selector_mask);
extern void add_consumer_to_tile(int insert_layer,CONSUMER_T *my_consumer,LAYER_T **my_layer,
			LAYER_T **parent_layer,LAYER_T **layer_list);
extern void dump_consumers(LAYER_T *my_layer);
extern void
free_object_from_layer(int sched_tile,CONSUMER_T *c,LAYER_T **my_layer,LAYER_T **layer_list);
extern void
remove_consumer_from_tile(int doing_migration,int sched_tile,CONSUMER_T *c,LAYER_T **my_layer,
                BITMAP_T *bitmap,int nodes,LAYER_T **layer_list);
extern void cleanup_layer_list(int doing_migration,int sched_tile,
                        LAYER_T **layer_list,LAYER_T **my_layer);


