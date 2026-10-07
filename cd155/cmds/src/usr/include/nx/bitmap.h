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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/bitmap.h,v 1.12 1994/11/19 02:44:38 mtm Exp $
 *
 */


/*
 * bitmap.h - dfc 12/91
 *
 * Definitions needed to use functions exported by bitmap.c.
 */
#ifndef	_bitmap_h
#define	_bitmap_h

#ifndef _COLUMN_MAP_T_H_
#define _COLUMN_MAP_T_H_
typedef unsigned long	COLUMN_MAP_T;	/* bitmap of one column */
#endif

#define	BITS_PER_BYTE	8
#define MAX_ROWS	(sizeof(COLUMN_MAP_T) * BITS_PER_BYTE)


/*
 * The following structure defines a bitmap which is used to describe the
 * nodes of a rectangular sub-mesh. The number of columns in the bitmap
 * exactly matches the  number of columns in the sub-mesh, but the number 
 * of rows in the bitmap may be larger than the actual number of rows in 
 * the submesh. The row_offset field specifies which portion of the bitmap 
 * contains the bits describing the sub-mesh, rows and cols describes the
 * size of the bitmap. The following picture should make all this clear:
 * 
 *	             11111111112222222222
 *	   012345678901234567890123456789
 * 	 0 ..............................
 * 	 1 ..............................
 *	  +------------------------------+ 
 * 	 2|..............................| <- row_offset
 * 	 3|..............................|
 * 	 4|..............................|
 * 	 5|..............................|
 * 	 :|..............................|
 * 	13|..............................|
 * 	14|..............................| 
 *	  +------------------------------+
 * 	15 ..............................
 * 	16 ..............................
 * 	 : .............................. 
 * 	31 ..............................
 *
 * in the figure above cols == 30, rows == 13 (2-14), and row_offset == 2.
 */ 
#ifndef _BITMAP_T_H_
#define _BITMAP_T_H_
typedef struct {
	int		rows;		/* Number of rows */
	int		cols;		/* Number of columns */
	int		row_offset;	/* Starting row of bitmap */
	COLUMN_MAP_T	colmap[1];	/* Variable length array of column
					   bitmaps */
} BITMAP_T;
#endif

/*
 * Exported bitmap functions.
 */
#ifdef _NO_PROTO_
BITMAP_T *allocate_bitmap();
BITMAP_T *bitmap_clone();
BITMAP_T *or_bitmaps();
BITMAP_T *AND_bitmaps();
BITMAP_T *invert_bitmap();
void init_bitmap();
int bitmap_find_space();
int bitmap_size();
int bitmap_allocate_space();
int bitmap_mask();
int bitmap_space_there();
int bitmap_free_space();
int bitmap_fill_rect();
void print_bitmap();
void show_bitmap();
void show_free_bitmap();
void show_free_matching_bitmap();
int bitmap_encl_rect();
int bitmap_provide_coord();
int bitmap_encl_rect();
int bitmap_provide_coord();
int num_nodes_in_map();

#else		/* _NO_PROTO */
extern BITMAP_T *allocate_bitmap(int cols, int rows);
extern int bitmap_size(BITMAP_T *descr);
extern BITMAP_T *bitmap_clone(BITMAP_T *descr);
extern BITMAP_T *invert_bitmap(BITMAP_T *descr);
extern BITMAP_T *or_bitmaps(BITMAP_T *b1,BITMAP_T *b2);
extern BITMAP_T *AND_bitmaps(BITMAP_T *b1,BITMAP_T *b2);
extern void init_bitmap(int fill, BITMAP_T *descr);
extern int bitmap_fill_rect(BITMAP_T *descr, int fill, int start_col, 
                        int start_row, int num_cols, int num_rows);
extern int bitmap_find_space(BITMAP_T *src,BITMAP_T *dest, int *col, int *row);
extern int bitmap_free_space(BITMAP_T *src, BITMAP_T *dest);
extern int bitmap_allocate_space(BITMAP_T *src, BITMAP_T *dest);
extern int bitmap_mask(BITMAP_T *src, BITMAP_T *dest);
extern int bitmap_space_there(BITMAP_T *src, BITMAP_T *dest);
extern void print_bitmap(BITMAP_T *descr);
extern void show_bitmap(BITMAP_T *bad,BITMAP_T *root,BITMAP_T *descr);
extern void show_free_bitmap(BITMAP_T *bad,BITMAP_T *root,BITMAP_T *descr,
				BITMAP_T *free_map);
extern void show_free_matching_bitmap(BITMAP_T *bad,BITMAP_T *root,
				BITMAP_T *descr,
				BITMAP_T *free_map, BITMAP_T* matching);
extern int setbit_bitmap(BITMAP_T *descr, int bit, int position);
extern int getbit_bitmap(BITMAP_T *descr, int position);
extern int bitmap_encl_rect(BITMAP_T *bitmap, int *col, int *row, 
				int *width, int *height);
extern int bitmap_provide_coord(unsigned long physnode, 
			unsigned long mesh_y, 
			unsigned long mesh_x,
                        unsigned long *myrow, unsigned long *mycol);
int num_nodes_in_map(BITMAP_T *descr);

#endif 		/* _NO_PROTO */

/*
 * This used to be a function, but can now be a macro.
 */
#define free_bitmap(p)	free((void *) (p))

#endif	/* !_bitmap_h */

