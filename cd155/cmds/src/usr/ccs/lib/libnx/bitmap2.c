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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/bitmap2.c,v 1.4 1994/11/19 02:29:08 mtm Exp $
 *
 */

/*
 *
 * Bitmap manipulation utilities.
 */
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <nx/bitmap.h>
#include <nx/defines.h>
/*
 * bitmap_find_size()
 *
 * Try to locate a rectangle large enough for a specified size in a bitmap.
 * The sides of the rect will be <= a predefined ration (MAX_SIDE_RATIO),
 * and a square shape is perferred. The algorithm searches from min. width
 * to max. one, by ORing the width # of columns. When |w-h| <= a predefined
 * margin (SIDE_MARGIN),  the algorithm thinks the rect is "square" enough
 * and stop searching. 
 * Assumption: In the bitmap, we assume 1 means busy and 0 idle. 
 */

#define	MAX_SIDE_RATIO		2.0
#define SQRT_RATIO		sqrt((double) MAX_SIDE_RATIO)
#define SIDE_MARGIN		2
#ifndef MAX
#define MAX(a,b)        (((a)>(b))?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b)        (((a)<(b))?(a):(b))
#endif


int
bitmap_find_size(size, dest_bitmap, col, row, width, height)
int 	 size; 		/* Size of nodes requested */ 
BITMAP_T *dest_bitmap;	/* Bitmap describing what space is available */
int	 *row;		/* Row in dest where rect. was found (0 if not) */
int	 *col;		/* Column in dest where rect. was found (0 if not) */
int	 *width;	/* Width of the rect. found (0 if not) */ 
int	 *height;	/* Height of the rect. found (0 if not) */
{
	int i, j, k;
	int orig_dest_offset;	/* original row_offset of dest */
	int sqr_w;		/* sqare side length with area >= size */
	int min_w;		/* min. width of rect. to be searched */
	int max_w;		/* max. width of rect.to be searched  */
	int hgt;		/* height of rect to be searched  */
	int cur_h, cur_w;	/* current sides of the rect. found  */
	int cur_x, cur_y;	/* base of current rect. found  */
	int pos;		/* position of starting 0-seq. */
	int found;		/* return value */
	int col_bound;		/* rightmost bound during the search */
	COLUMN_MAP_T *col_cum;	/* storage for col. accumulation ops */
	COLUMN_MAP_T full_col;	/* all bits set to 1, for optimization only */
	COLUMN_MAP_T col_tmp;	
	BITMAP_T *dest;		/* Bitmap describing what space is available */

	*row = 0;
	*col = 0;
	*width = 0;
	*height = 0;
	found = 0;

	/*
	 * Check size is not too large for the bmap.
	 */
	if (size > dest_bitmap->rows * dest_bitmap->cols)
		return(0);

	dest = invert_bitmap(dest_bitmap);
	/*
	 * In the unlikely case that the bitmaps' row offsets are not
	 * 0 we will make them so and set them back when we are done.
	 */
	orig_dest_offset = dest->row_offset;
	if (orig_dest_offset != 0)
		shift_bitmap(0, dest);
	

	/*  allocate the space for column accumulation operations.
	 */
	col_cum = (COLUMN_MAP_T *) MALLOC(sizeof(COLUMN_MAP_T) * dest->cols);

	/* 
	 * the width we'd like to check for a rect.
	 */
	sqr_w = (int) sqrt( (double) size);
	min_w = MAX((int) (sqr_w / SQRT_RATIO), 1);
	max_w = MIN(MIN((int) (sqr_w * SQRT_RATIO), dest->cols), size);

	/* init. the col_cum array
	*/
	for (i = 0; i < dest->cols; i++)
		col_cum[i] = dest->colmap[i];

	/* build the full_col with relevant bits.
	 */
	full_col = 0;
	for (i = 0; i < dest->rows; i++) {
		full_col <<= 1;
		full_col |= 1;
	}

	/*
	 * XXX - It would be nice to start at some random point in the
	 * dest bitmap to avoid clumping everything in the upper left
	 * corner.
	 */

	/* 
	 * Search a rect. with width from min_w to max_w.
	 * An ideal one would have side be close to sqr_w.
	 */

	for (i = min_w; i <= max_w; i++) {
		/*
	 	 * Look for room in dest bitmap top-bottom, left-right.
	 	 */
		col_bound = dest->cols - i;

		/* determine the corresponding height
		*/
		if (size % i == 0)
			hgt = MIN(size / i, dest->rows);
		else
			hgt = MIN(size / i + 1, dest->rows);

		/* Check if we need this iteration */
		if (i * hgt < size)
			continue;

		/* Accumulate the first col_cum and partially remainings */

		for (j = i - 1; j > 0; j--) 
			col_cum[j-1] |= col_cum[j];

		/* Check for space, and build more col_cum if necessary */

		for (j = 0; j <= col_bound; j++) {
			/*
			 * If a column is full, skip the column. 
			 */
			if (col_cum[j] == full_col)
				continue;

			if (has_zero_seq(col_cum[j], dest->rows, hgt, &pos)) {
				/* Find a solution */
				if (!found) { 
					/* Save the first solution */
					found= 1;
					cur_w = i;
					cur_h = hgt;
					cur_x = j;
					cur_y = pos;
				} else if (abs(cur_w - cur_h) > abs(i - hgt)) {
					/* Not the first, but a better one */
					cur_w = i;
					cur_h = hgt;
					cur_x = j;
					cur_y = pos;
				}

				/* If good enough, or won't improve further, stop. */ 

				if ((abs(cur_w - cur_h) <= SIDE_MARGIN) ||
							(cur_w >= sqr_w))
					goto done; 
				else  
					/* Can be improved, check next i value */
					break; 

			} else if (j < col_bound) {
				/* build the next col_cum
				*/
				col_tmp = dest->colmap[j + i];
				for (k = i; k >= 1; k--)
					col_cum[j + k] |= col_tmp; 
			}
		}		 
	}

done:	if (found) {
		*row = cur_y;
		*col = cur_x;
		*width = cur_w;
		*height = cur_h;
	}	

	/* Set back the bitmap.
	*/
	if (orig_dest_offset != 0)
		shift_bitmap(orig_dest_offset, dest);

	free_bitmap(dest);
	/* Release the allocated space.
	*/
	FREE(col_cum);

	return(found);
}

/*
 * Check if there is a seq of 0's of seq_len long in column.
 * If there is, return the position of the starting bit of the top seq.
 */
int 
has_zero_seq(column, col_len, seq_len, position)
COLUMN_MAP_T column;	/* Column to be checked */	
int col_len;		/* Max. len to be searched in column */
int seq_len;		/* Lengh of the 0 seq. */
int *position; 		/* Starting position of the top seq. */
{
	int i;
	int check_len;

	*position = -1; /* garbage position */

	if (seq_len > 1) 
		/* Shift up 1's up (seq_len -1) positions */
		for (i = 1; i < seq_len; i++)
			column = column | (column >> 1);

	column = ~column;
	check_len = col_len - seq_len;

	/*
	 * The top 1 bit position between 0 and check_len
	 * is the seq.'s starting position.
	 */ 
	for (i = 0; i <= check_len; i++)
		if (0x00000001 == (column & 0x00000001)) {
		 	*position = i;
			return (1);
		} else
			column = column >> 1;
	return (0);
}



/*
 * bitmap_select()
 *
 * AND the src into the dest
 */	
int
bitmap_mask(BITMAP_T *src, BITMAP_T *dest)
{
	int i;
	int orig_src_offset;	/* original row_offset of src */
	int orig_dest_offset;	/* original row_offset of dest */
	COLUMN_MAP_T mask;	/* bit mask */

	/*
	 * Return error if the src bitmap is bigger than the dest bitmap.
	 */
	if ((src->rows > dest->rows) | (src->cols > dest->cols)) {
		errno = EPBADNODE;
		return(-1);
	}

	/*
	 * In the unlikely case that the dest or src bitmaps' row offset
	 * are not 0 we will make them so and set them back when we are done.
	 */
	orig_dest_offset = dest->row_offset;
	if (orig_dest_offset != 0)
		shift_bitmap(0, dest);

	orig_src_offset = src->row_offset;
	if (orig_src_offset != 0)
		shift_bitmap(0, src);
	/*
	 * And the bits.
	 */
	for (i = 0; i < src->cols; i++) {
		dest->colmap[i] &= src->colmap[i];
	}
		
	/*
	 * Set back bitmaps now that we are done.
	 */
	if (orig_dest_offset != 0)
		shift_bitmap(orig_dest_offset, dest);

	if (orig_src_offset != 0)
		shift_bitmap(orig_src_offset, src);

	return(0);
}

