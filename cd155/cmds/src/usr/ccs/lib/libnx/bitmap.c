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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/bitmap.c,v 1.20 1994/11/19 02:29:07 mtm Exp $
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
 * allocate_bitmap() - Allocate a bitmap structure.
 */
BITMAP_T *
allocate_bitmap(int cols, int rows)
{
	BITMAP_T *p;

	/*
	 * Make sure that number of rows is not more than the number of
	 * bits in a column bitmap.
	 */
	if (rows > MAX_ROWS)
		return (NULL);
	/*
	 * Allocate enough room for the fixed sized portion of the structure
	 * and the variable length array of column bitmaps. We must subtract
	 * 1 from column count to allow for the single element declared in
	 * the typedef.
	 */

	p = (BITMAP_T *) MALLOC(sizeof(BITMAP_T) +
	                        sizeof(COLUMN_MAP_T) * (cols - 1));

	if (p) {
		/*
		 * Initialize size information.
		 */
		p->rows = rows;
		p->cols = cols;
		p->row_offset = 0;
	}

	return(p);
}

/*
 * bitmap_size() - Return size in bytes of a bitmap structure.
 */
int
bitmap_size(BITMAP_T *descr)
{

	if (descr != (BITMAP_T*) 0)
	    return (sizeof(BITMAP_T) + sizeof(COLUMN_MAP_T)*(descr->cols - 1));
	else
	    return 0;
}

/*
 * bitmap_clone() - Make an exact copy of a bitmap structure.
 */
BITMAP_T *
bitmap_clone(descr)
BITMAP_T *descr;	/* Bitmap descriptor to copy */
{
	BITMAP_T *p;
	int	 i;

	/*
	 * Allocate enough room for the fixed sized portion of the structure
	 * and the variable length array of column bitmaps. We must subtract
	 * 1 from column count to allow for the single element declared in
	 * the typedef.
	 */

	p = (BITMAP_T *) MALLOC(sizeof(BITMAP_T) +
	                        sizeof(COLUMN_MAP_T) * (descr->cols - 1));

	if (p) {
		/*
		 * Initialize new bitmap.
		 */
		p->rows = descr->rows;
		p->cols = descr->cols;
		p->row_offset = descr->row_offset;
		for (i = 0; i < descr->cols; i++)
			p->colmap[i] = descr->colmap[i];
	}

	return(p);
}

/*
 * invert_bitmap() - Make an inverted copy of a bitmap
*/
BITMAP_T *
invert_bitmap(BITMAP_T *descr)
{
	BITMAP_T *p;
	int	 i;

	/*
	 * Allocate enough room for the fixed sized portion of the structure
	 * and the variable length array of column bitmaps. We must subtract
	 * 1 from column count to allow for the single element declared in
	 * the typedef.
	 */

	p = (BITMAP_T *) MALLOC(sizeof(BITMAP_T) +
	                        sizeof(COLUMN_MAP_T) * (descr->cols - 1));

	if (p) {
		/*
		 * Initialize new bitmap.
		 */
		p->rows = descr->rows;
		p->cols = descr->cols;
		p->row_offset = descr->row_offset;
		for (i = 0; i < descr->cols; i++)
			p->colmap[i] = ~descr->colmap[i];
	}

	return(p);
}

/* Or two bitmaps together
 *
 *	Returns NULL if the two passed in bitmaps are not
 *	identical in size.
 *
*/

BITMAP_T *
or_bitmaps(BITMAP_T *b1,BITMAP_T *b2)
{
	BITMAP_T *p;
	int	 i;
	int	b1_offset,b2_offset;

	/*
	 * In the unlikely case that the bitmap's row offset is not
	 * 0 we will make it so and set it back when we are done.
	*/
	b1_offset = b1->row_offset;
	if (b1_offset != 0)
		shift_bitmap(0, b1);

	b2_offset = b2->row_offset;
	if (b2_offset != 0)
		shift_bitmap(0, b2);

	/* bitmaps b1 and b2 must be the same size */
	if ( (b1->rows != b2->rows) || (b1->cols != b2->cols) ||
		(b1->row_offset != b2->row_offset) ){
		p = (BITMAP_T *)0;
		goto or_done;
	}

	/*
	 * Allocate enough room for the fixed sized portion of the structure
	 * and the variable length array of column bitmaps. We must subtract
	 * 1 from column count to allow for the single element declared in
	 * the typedef.
	 */

	p = (BITMAP_T *) MALLOC(sizeof(BITMAP_T) +
	                        sizeof(COLUMN_MAP_T) * (b1->cols - 1));

	if (p) {
		/*
		 * Initialize new bitmap.
		 */
		p->rows = b1->rows;
		p->cols = b1->cols;
		p->row_offset = b1->row_offset;
		for (i = 0; i < b1->cols; i++)
			p->colmap[i] = b1->colmap[i] | b2->colmap[i];
	}

or_done:
	/* Set bitmaps back to original offset */
	if (b1_offset != 0)
		shift_bitmap(b1_offset,b1);
	if (b2_offset != 0)
		shift_bitmap(b2_offset,b2);
	return(p);
}

/* AND two bitmaps together
 *
 *	Returns NULL if the two passed in bitmaps are not
 *	identical in size.
 *
*/

BITMAP_T *
AND_bitmaps(BITMAP_T *b1,BITMAP_T *b2)
{
	BITMAP_T *p;
	int	 i;
	int	b1_offset,b2_offset;

	/*
	 * In the unlikely case that the bitmap's row offset is not
	 * 0 we will make it so and set it back when we are done.
	*/
	b1_offset = b1->row_offset;
	if (b1_offset != 0)
		shift_bitmap(0, b1);

	b2_offset = b2->row_offset;
	if (b2_offset != 0)
		shift_bitmap(0, b2);

	/* bitmaps b1 and b2 must be the same size */
	if ( (b1->rows != b2->rows) || (b1->cols != b2->cols) ||
		(b1->row_offset != b2->row_offset) ){
		p = (BITMAP_T *)0;
		goto AND_done;
	}

	/*
	 * Allocate enough room for the fixed sized portion of the structure
	 * and the variable length array of column bitmaps. We must subtract
	 * 1 from column count to allow for the single element declared in
	 * the typedef.
	 */

	p = (BITMAP_T *) MALLOC(sizeof(BITMAP_T) +
	                        sizeof(COLUMN_MAP_T) * (b1->cols - 1));

	if (p) {
		/*
		 * Initialize new bitmap.
		 */
		p->rows = b1->rows;
		p->cols = b1->cols;
		p->row_offset = b1->row_offset;
		for (i = 0; i < b1->cols; i++)
			p->colmap[i] = b1->colmap[i] & b2->colmap[i];
	}

AND_done:
	/* Set bitmaps back to original offset */
	if (b1_offset != 0)
		shift_bitmap(b1_offset,b1);
	if (b2_offset != 0)
		shift_bitmap(b2_offset,b2);
	return(p);
}

/*
 * shift_bitmap() - Shift the bitmap offset to a specified row.
 */
int
shift_bitmap(row, descr)
int	 row;		/* Row to which to shift the bitmap */
BITMAP_T *descr;	/* Bitmap descriptor */
{
	int i;
	int count;

	/*
	 * Do a sanity check of the row.
	 */
	if ((row > MAX_ROWS) | (row + descr->rows + 1 > MAX_ROWS))
		return(0);

	count = row - descr->row_offset;

	if (count > 0) {

		/*
		 * Shift each column of bitmap down.
		 */
		for (i = 0; i < descr->cols; i++)
			descr->colmap[i] <<= count;
	}

	else /*(count <= 0)*/ {

		/*
		 * Everything is easier if we make count positive.
		 */
		count = -count;

		/*
		 * Shift each column of bitmap up.
		 */
		for (i = 0; i < descr->cols; i++)
			descr->colmap[i] >>= count;
	}

	descr->row_offset = row;
	return(1);
}

/*
 * init_bitmap() - Initialize bits of a bitmap to a specified value.
 */
void
init_bitmap(int fill, BITMAP_T *descr)
{
	int i;
	int mask;

	/*
	 * Set all bits to zero first. Zeros are used for rows
	 * "outside" of the initialized rectangle.
	 */ 
	for (i = 0; i < descr->cols; i++)
		descr->colmap[i] = 0;

	/*
	 * Now set bits "inside" rectangle if they are to be set to 1s.
	 */

	if (fill) {
	
		/*
		 * Build mask of relevant bits.
		 */
		mask = 0;
		for (i = 0; i < descr->rows; i++) {
			mask <<= 1;
			mask |= 1;
		}

		/*
		 * Initilaize each column.
		 */
		for (i = 0; i < descr->cols; i++)
			descr->colmap[i] |= mask;
	}
}

/*
 * bitmap_fill_rect() - Fill a rectangular portion of a bitmap with
 * a specified value.
 */
int
bitmap_fill_rect(BITMAP_T *descr, int fill, int start_col, 
			int start_row, int num_cols, int num_rows)
{
	int i;
	int mask;
	int orig_offset;	/* original row_offset of bitmap */

	/*
	 * Check to see that rectangle is contained in bitmap.
	 */
	if (((start_row + num_rows) > descr->rows) ||
	    ((start_col + num_cols) > descr->cols))  {
		errno = EPBADNODE;
		return(-1);
	}

	/*
	 * In the unlikely case that the bitmap's row offset is not
	 * 0 we will make it so and set it back when we are done.
	 */
	orig_offset = descr->row_offset;
	if (orig_offset != 0)
		shift_bitmap(0, descr);

	/*
	 * Build mask of relevant bits.
	 */
	mask = 0;
	for (i = 0; i < num_rows; i++) {
		mask <<= 1;
		mask |= 1;
	}
	mask <<= start_row;

	/*
	 * Initilaize each column.
	 */

	if (fill) {
		for (i = start_col; i < start_col + num_cols; i++)
			descr->colmap[i] |= mask;
	}
	else {
		mask ^= ~0;
		for (i = start_col; i < start_col + num_cols; i++)
			descr->colmap[i] &= mask;
	}

	if (orig_offset != 0)
		shift_bitmap(orig_offset, descr);

	return 0;
}

/*
 * bitmap_find_space()
 *
 * Move the source bitmap over the destination bitmap until we find an area
 * in which all of the bits that are 1s in the source bitmap are 1s in the
 * destination bitmap.
 */
int
bitmap_find_space(BITMAP_T *src,BITMAP_T *dest, int *col, int *row)
{
	int i, j, k;
	int orig_src_offset;	/* original row_offset of src */
	int orig_dest_offset;	/* original row_offset of dest */
	int col_slop;		/* # of cols by which dest is bigger than src */
	int row_slop;		/* # of rows by which dest is bigger than src */
	int found;		/* return value */
	int all_same;		/* TRUE if all cols of bitmap are identical */
	COLUMN_MAP_T and_map;	/* map of src and dest col bitwise ANDed */

	*row = 0;
	*col = 0;

	/*
	 * Assume that if the src bitmap is bigger than the dest bitmap
	 * then we don't have enough room. This assumption is true as long
	 * as the src bitmap is no bigger than it needs to be to hold
	 * the bits that are 1s.
	 */
	if ((src->rows > dest->rows) | (src->cols > dest->cols))
		return(0);

	/*
	 * In the unlikely case that the bitmaps' row offsets are not
	 * 0 we will make them so and set them back when we are done.
	 */
	orig_dest_offset = dest->row_offset;
	if (orig_dest_offset != 0)
		shift_bitmap(0, dest);

	orig_src_offset = src->row_offset;
	if (orig_src_offset != 0)
		shift_bitmap(0, src);

	/*
	 * XXX - It would be nice to start at some random point in the
	 * dest bitmap to avoid clumping everything in the upper left
	 * corner.
	 */

	/*
	 * Can optimize for bitmaps which are all the same.
	 */
	all_same = 1;
	for (j = 1; j < src->cols; j++) {
		if (src->colmap[0] != src->colmap[j]) {
			all_same = 0;
			break;
		}
	}

	/*
	 * Look for room in dest bitmap left to right, top to bottom.
	 */
	col_slop = dest->cols - src->cols;
	row_slop = dest->rows - src->rows;
	found = 0;

	for (i = 0; i <= row_slop; i++) {
		for (j = 0; j <= col_slop; j++) {
			/*
			 * We will compare the src bitmap right to left.
			 * This way, if all_same is TRUE, as soon as we don't
			 * match we can jump over several columns.
			 */
			for (k = src->cols - 1; k >= 0; k--) {
			    and_map  = src->colmap[k] & dest->colmap[j + k];
			    if (and_map != src->colmap[k]) {
				if (all_same)
					j += k;
				break;
			    }
			}
			if (k == -1) {
				*row = i;
				*col = j;
				found = 1;
				goto done;
			}
		}
		shift_bitmap(i + 1, src);
	}
		
done:
	/*
	 * Set back bitmaps now that we are done.
	 */
	shift_bitmap(orig_dest_offset, dest);
	shift_bitmap(orig_src_offset, src);

	return(found);
}
	
/*
 * bitmap_free_space()
 *
 * AND the bits in the source bitmap with the bits in the destination bitmap.
 */	
int
bitmap_free_space(BITMAP_T *src, BITMAP_T *dest)
{
	int i;
	int orig_src_offset;	/* original row_offset of src */
	int orig_dest_offset;	/* original row_offset of dest */

	/*
	 * Assume that if the src bitmap is bigger than the dest bitmap
	 * then we don't have enough room. This assumption is true as long
	 * as the src bitmap is no bigger than it needs to be to hold
	 * the bits that are 1s.
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
	 * Set the bits.
	 */
	for (i = 0; i < src->cols; i++)
		dest->colmap[i] |= src->colmap[i];
		
	/*
	 * Set back bitmaps now that we are done.
	 */
	if (orig_dest_offset != 0)
		shift_bitmap(orig_dest_offset, dest);

	if (orig_src_offset != 0)
		shift_bitmap(orig_src_offset, src);

	return(0);
}

/*
 * bitmap_allocate_space()
 *
 * Set bits in the destination bitmap to 0 if the corresponding bits in
 * the source bitmap are 1.
 */	
int
bitmap_allocate_space(BITMAP_T *src, BITMAP_T *dest)
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
	 * Set the bits.
	 */
	for (i = 0; i < src->cols; i++) {
		mask = src->colmap[i] ^ ~0;
		dest->colmap[i] &= mask;
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

/*
 * bitmap_space_there()
 *
 * Return TRUE if the bits that are 1s in the source bitmap are 1s
 * in the destination bitmap.
 */	
int
bitmap_space_there(BITMAP_T *src, BITMAP_T *dest)
{
	int i;
	int orig_src_offset;	/* original row_offset of src */
	int orig_dest_offset;	/* original row_offset of dest */
	COLUMN_MAP_T and_map;	/* map of src and dest col bitwise ABDed */


	/*
	 * Assume that if the src bitmap is bigger than the dest bitmap
	 * then we don't have enough room. This assumption is true as long
	 * as the src bitmap is no bigger than it needs to be to hold
	 * the bits that are 1s.
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
	 * Look for room in dest bitmap.
	 */

	for (i = 0; i < src->cols; i++) {
		and_map = src->colmap[i] & dest->colmap[i];
		if (and_map != src->colmap[i])
			break;
	}

	/*
	 * Set back bitmaps now that we are done.
	 */
	if (orig_dest_offset != 0)
		shift_bitmap(orig_dest_offset, dest);

	if (orig_src_offset != 0)
		shift_bitmap(orig_src_offset, src);

	if (i == src->cols)
		return 1;
	else
		return 0;
}

void
print_bitmap(BITMAP_T *descr)
{
	COLUMN_MAP_T mask;
	int i, j, start_node;

	printf("    +-");
	for (j = 0; j < descr->cols; j++)
		printf("--");
	printf("+\n");

	mask = 1;
	start_node = 0;
	for (i = descr->row_offset; i < (descr->rows+descr->row_offset); i++) {
		printf("%4d| ", start_node);
		for (j = 0; j < descr->cols; j++) {
			if (mask & descr->colmap[j])
				printf("* ");
			else
				printf(". ");
		}

		printf("|\n");
		mask <<= 1;
		start_node += descr->cols;
	}

	printf("    +-");
	for (j = 0; j < descr->cols; j++)
		printf("--");
	printf("+\n");
}

void
show_bitmap(BITMAP_T *bad,BITMAP_T *root,BITMAP_T *descr)
{
	COLUMN_MAP_T mask;
	int i, j, start_node;

	printf("    +-");
	for (j = 0; j < descr->cols; j++)
		printf("--");
	printf("+\n");

	mask = 1;
	start_node = 0;
	for (i = descr->row_offset; i < (descr->rows+descr->row_offset); i++) {
		printf("%4d| ", start_node);
		for (j = 0; j < descr->cols; j++) {
			/* see if this node is in the partition */
			if (mask & descr->colmap[j]){
				/* Yes it is! */
				/* see if this is a bad node */
				if ((bad != (BITMAP_T *)0) && (mask & bad->colmap[j]))
					printf("X ");
				else
				/* see if this node is present */
				if ( (mask & root->colmap[j]) != mask)
					printf("- ");
				else
				/* this node is good and present */
					printf("* ");
			}
			else
				printf(". ");
		}

		printf("|\n");
		mask <<= 1;
		start_node += descr->cols;
	}

	printf("    +-");
	for (j = 0; j < descr->cols; j++)
		printf("--");
	printf("+\n");
}


void
show_free_bitmap(BITMAP_T *bad,BITMAP_T *root,BITMAP_T *descr,
		BITMAP_T *free_map)
{
    show_free_matching_bitmap(bad, root, descr, free_map, (BITMAP_T*)0);
}


void
show_free_matching_bitmap(BITMAP_T *bad,BITMAP_T *root,BITMAP_T *descr,
		BITMAP_T *free_map, BITMAP_T *matching)
{
	COLUMN_MAP_T mask;
	int i, j, start_node;

	printf("    +-");
	for (j = 0; j < descr->cols; j++)
		printf("--");
	printf("+\n");

	mask = 1;
	start_node = 0;
	for (i = descr->row_offset; i < (descr->rows+descr->row_offset); i++) {
		printf("%4d| ", start_node);
		for (j = 0; j < descr->cols; j++) {
			/* see if this node is a free node */
			if (free_map && (mask & free_map->colmap[j])){
				/* yes it is */
				if (!matching || mask & matching->colmap[j])
				    printf("F ");
				else
				    printf("f ");
			}
			else
			/* see if this node is in the partition */
			if (mask & descr->colmap[j]){
				/* Yes it is! */
				/* see if this is a bad node */
				if ((bad != (BITMAP_T *)0) && (mask & bad->colmap[j]))
					printf("X ");
				else
				/* see if this node is present */
				if ( (mask & root->colmap[j]) != mask)
					printf("- ");
				else {
				/* this node is good and present */
				    if (!matching || mask & matching->colmap[j])
					printf("* ");
				    else
					printf("o ");
				}
			}
			else
				printf(". ");
		}

		printf("|\n");
		mask <<= 1;
		start_node += descr->cols;
	}

	printf("    +-");
	for (j = 0; j < descr->cols; j++)
		printf("--");
	printf("+\n");
}

#define CABINET_SIZE 4

void
show_wide_matching_bitmap(BITMAP_T *bad,BITMAP_T *root,BITMAP_T *descr,
		BITMAP_T *free_map, BITMAP_T *matching)
{
	COLUMN_MAP_T mask;
	int i, j, k, start_node;
	int num_cabinets;

	num_cabinets = descr->cols / CABINET_SIZE;

	printf("\n    ");
	for (i=0; i < num_cabinets; i++) {
            printf("|%-+9d", (i * CABINET_SIZE));
	}
	printf("|\n");


	mask = 1;
	start_node = 0;
	for (i = descr->row_offset; i < (descr->rows+descr->row_offset); i++) {

	        if ((i % 4) == 0) {
		    printf("    +-");
		    for (k = 0; k < descr->cols; k++)
			printf("--");
		    for (k = 0; k < num_cabinets-1; k++) 
			printf("--");
		    printf("+\n");
		}

		printf("%4d| ", start_node);
		for (j = 0; j < descr->cols; j++) {
			/* see if this node is a free node */
			if (free_map && (mask & free_map->colmap[j])){
				/* yes it is */
				if (!matching || mask & matching->colmap[j])
				    printf(((j+1) % CABINET_SIZE) ? "F ": "F | ");
				else
				    printf(((j+1) % CABINET_SIZE) ? "f ": "f | ");
			}
			else
			/* see if this node is in the partition */
			if (mask & descr->colmap[j]){
				/* Yes it is! */
				/* see if this is a bad node */
				if ((bad != (BITMAP_T *)0) && (mask & bad->colmap[j]))
				        printf(((j+1) % CABINET_SIZE) ? "X ": "X | ");
				else
				/* see if this node is present */
				if ( (mask & root->colmap[j]) != mask)
				        printf(((j+1) % CABINET_SIZE) ? "- ": "- | ");
				else {
				/* this node is good and present */
				    if (!matching || mask & matching->colmap[j])
					printf(((j+1) % CABINET_SIZE) ? "* ": "* | ");
				    else
					printf(((j+1) % CABINET_SIZE) ? "o ": "o | ");
				}
			}
			else
			        printf(((j+1) % CABINET_SIZE) ? ". ": ". | ");
		}

		printf("\n");
		mask <<= 1;
		start_node += descr->cols; 
	}

	printf("    +-");
	for (j = 0; j < descr->cols; j++)
	    printf("--");
	for (j = 0; j < num_cabinets-1; j++) 
	    printf("--");
	printf("+\n");
	
	printf("    ");
	for (i=(num_cabinets-1); i>=0; i--)
	    printf("| Cab %2d  ", i);
	printf("|\n");
}

int 
setbit_bitmap(BITMAP_T *descr, int bit, int position)
{
	int num_col;		/* The bit will get set in this column */
	int num_row;		/* The bit will get set in this  row */
	unsigned long mask;    


	/*
	 * Divide the postion by the size of column to get the number
	 * of the column where the bit will be set
	 */
	num_col = position % descr->cols;

	/*
	 * Mod of the position with the size of a column will give
	 * the row in which the bit needs to be set
	 */
	num_row = position / descr->cols;

	/*
	 * Create a mask.
	 */
	mask = 1 << num_row;

	if (bit) {
		/*
		 * Make sure the bit is not already set
		 */
		if ((descr->colmap[num_col] & mask) != 0) {
			errno = EPBADNODE;
			return(-1);
		}
		descr->colmap[num_col] |= mask;
	} else {
		/*
		 * Make sure the bit is already set
		 */
		if ((descr->colmap[num_col] & mask) == 0) {
			errno = EPBADNODE;
			return(-1);
		}
		descr->colmap[num_col] &= ~mask;
	}

	return(0);
}

int
getbit_bitmap(BITMAP_T *descr, int position)
{
	int num_col;		/* Read bit from this column */
	int num_row;		/* Read bit from this row */
	unsigned long mask;

	/*
	 * Divide the postion by the size of column to get the number
	 * of the column from which the bit will be read.
	 */
	num_col = position % descr->cols;

	/*
	 * Mod of the position with the size of a column will give
	 * the row in which the bit needs to be set
	 */
	num_row = position / descr->cols;
 
	/*
	 * Create a mask.
	 */
	mask = 0;
	mask |= 1;
	mask <<= num_row;
	if (descr->colmap[num_col] & mask)
		return(1);
	else
		return(0);
}

/* forward function reference */
void col_end_bits();

/*
 * bitmap_encl_rect()
 *
 * Find the smallest enclosure rectangle of a consumer represented
 * by a bitmap. If the bitmap is empty, the function return 0; 
 * otherwise it return 1 and the parameters of the enclosure rectangle. 
 *
 *	Parameters:
 *		bitmap		source bitmap
 *		col		pointer to int, width of rectangle
 *		row		pointer to int, height of rectangle
 *		
 *	Returns:
 *		0		bitmap is empty
 *		1		bitmap is not empty	
 */
int
bitmap_encl_rect(BITMAP_T *bitmap, int *col, int *row, int *width, int *height)
{
	int i;
	int orig_bitmap_offset;	/* original row_offset of bitmap */
	int top_bit, btm_bit;
	int min_bit, max_bit;
	int left_col,right_col;
	int bit_vector,col_vector;

	/*
	 * In the unlikely case that the bitmaps' row offsets are not
	 * 0 we will make them so and set them back when we are done.
	 */
	orig_bitmap_offset = bitmap->row_offset;
	if (orig_bitmap_offset != 0)
		shift_bitmap(0, bitmap);
	
	/* 
	 * Search for the first nonzero column
	 */
	for (i = 0; i < bitmap->cols; i++)
		if (bitmap->colmap[i]) { /* has allocated bits */ 
			left_col = i;
			right_col = i;
			col_end_bits(bitmap->colmap[i], bitmap->rows - 1,
						&min_bit, &max_bit);
			break;
		}

	if (i == bitmap->cols) /* empty bitmap */
		return (0);

	while (i < bitmap->cols) {
		/* search through the remaining columns */
		if (bitmap->colmap[i]) {
			right_col = i;
			col_end_bits(bitmap->colmap[i], bitmap->rows - 1,
						&top_bit, &btm_bit);
			if (top_bit < min_bit)
				min_bit = top_bit;
			if (btm_bit > max_bit)
				max_bit = btm_bit;
		}
		i++;
	}

	/* Set back the bitmap.
	 */
	if (orig_bitmap_offset != 0)
		shift_bitmap(orig_bitmap_offset, bitmap);

	*width = right_col - left_col + 1;
	*height = max_bit - min_bit + 1;
	*row = *height;
	*col = *width;

	/* Now check to see if we have a contiguous rectangle */

 	/* build column vector that is height high */

	bit_vector = min_bit + 1;
	col_vector = 0;
	for (i = 0; i < *height; i++){
		col_vector |= bit_vector;
		bit_vector = bit_vector << 1; 
	}

	for(i = left_col; i < left_col + *width - 1;i++){
		if ( (bitmap->colmap[i] & col_vector) != col_vector){
			*row = -1;
			*col = -1;
			break;
		}
	}

	return (1);
}

/*
 * Find the hightes (top) and lowest (bottom) 1 positions in column.
 * Column must be nonzero !!
 */
void
col_end_bits(column, len, top, bottom)
COLUMN_MAP_T column;	/* The column to be checked */	
int len;		/* The max. length need to be checked */	
int *top;		/* Top bit 1 position */
int *bottom;		/* Bottom bit 1 position */
{
	int i;

	/* Search the top position */
	for (i = 0; i <= len; i++) {
		if (0x00000001 == (column & 0x00000001)) {
		 	*top = i;
			*bottom = i;
			break;
		} 
		column = column >> 1;
	}

	while (i <= len && column) {
		if (0x00000001 == (column & 0x00000001))
			*bottom = i;
		column = column >> 1;
		i++;
	}
}

/* Provide the row and column address of a given physical node 
 *
 *
 *	Parameters:
 *
 *
 *
 *	Returns:
 *
 *
 *
*/

int
bitmap_provide_coord(unsigned long physnode, 
			unsigned long mesh_y, unsigned long mesh_x,
			unsigned long *myrow, unsigned long *mycol)
{

	/* Make sure physnode is within bounds */

	if (physnode >= mesh_x * mesh_y)
		return(FALSE);
	*myrow = physnode / mesh_x;
	*mycol = physnode - *myrow * mesh_x;
	return(TRUE);
}

int
num_nodes_in_map(BITMAP_T *descr)
{
	int		i,j,n;
	unsigned long	mask;

	n = 0;

	mask = 1;
	if (descr != (BITMAP_T *)0)
	for (i = descr->row_offset; 
		i < (descr->rows+descr->row_offset); i++) {
		for (j = 0; j < descr->cols; j++) {
			/* see if this node is in the partition */
			if (mask & descr->colmap[j]){
				/* Yes it is! */
				n++;
			}
		}
		mask <<= 1;
	}

	return(n);
}
