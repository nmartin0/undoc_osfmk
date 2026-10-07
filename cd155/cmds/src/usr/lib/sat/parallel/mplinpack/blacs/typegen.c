#include "global.h"

static global_row = 0;
static global_col = 0;
static global_all = 0;
static global_row_mask = 0;
static global_col_mask = 0;
static global_all_mask = 0;
static int *in_proc_type = 0;
static int *out_proc_type = 0;

type_init()
/*
  initialize type generation routines.

  THIS VERSION DATED 06/15/92
  R. VAN DE GEIJN
*/
{
  int i;

  /* get some space for the arrays */
  in_proc_type = (int *) malloc(nprow00*npcol00*sizeof(int));
  out_proc_type = (int *) malloc(nprow00*npcol00*sizeof(int));
  if (in_proc_type == NULL || out_proc_type == NULL) {
    printf("%d %d malloc failed in type_init\n", myrow00, mycol00);
    exit(0);
  }

  /* initialize the base of the types */
  for (i=0; i<nprow00*npcol00; i++) 
    in_proc_type[i] = out_proc_type[i] = 0;

  global_row = 0;
  global_col = 0;
  global_all = 0;
  
  global_row_mask = 1;
  while (global_row_mask < MAX_TYPES * nprow00 * npcol00)
    global_row_mask = global_row_mask<<1;
  global_col_mask = global_row_mask<<1;
  global_all_mask = global_all_mask<<1;
}

int itype_from_(row, col)
/*
  return message type to be used for receiving from 
  node row,col

  THIS VERSION DATED 06/15/92
  R. VAN DE GEIJN
*/
int *row, *col;
{
  int src, result;

  src = *row*npcol00+*col;
  result = MASK + (src<<MAX_TYPE_BITS) + in_proc_type[src];
  in_proc_type[src] = (in_proc_type[src]+1)%MAX_TYPES;
  
/*  if (myrow00 == 0 && mycol00 == 0 && *row == 0 && *col == 3) 
    printf("%d %d type_from = %d, from %d %d\n", myrow00, mycol00,
	   result, *row, *col); */

  if (result == 999999999) printf("warning: 999999999 used\n");
  return(result);
}

int itype_to_(row, col)
/*
  return message type to be used for sending to 
  node row,col

  THIS VERSION DATED 06/15/92
  R. VAN DE GEIJN
*/
int *row, *col;
{
  int src, dest, result;

  src = myrow00*npcol00+mycol00;
  dest = *row*npcol00+*col;
  result = MASK + (src<<MAX_TYPE_BITS) + out_proc_type[dest];
  out_proc_type[dest] = (out_proc_type[dest]+1)%MAX_TYPES;

/*  if (myrow00 == 0 && mycol00 == 3 && *row == 0 && *col == 0) 
    printf("%d %d type_to = %d, from %d %d\n", myrow00, mycol00,
	   result, *row, *col); */

  if (result == 999999999) printf("warning: 999999999 used\n");
  return(result);
}

int itype_row_()
/*
  return message type to be used for global row operations

  THIS VERSION DATED 06/15/92
  R. VAN DE GEIJN
*/
{
  int result;

  result = MASK + global_row_mask + global_row;
  global_row = (global_row + 1) % MAX_TYPES;

  if (result == 999999999) printf("warning: 999999999 used\n");
  return(result);
}

int itype_col_()
/*
  return message type to be used for global column operations

  THIS VERSION DATED 06/15/92
  R. VAN DE GEIJN
*/
{
  int result;

  result = MASK + global_col_mask + global_col;
  global_col = (global_col + 1) % MAX_TYPES;

  if (result == 999999999) printf("warning: 999999999 used\n");
  return(result);
}

int itype_all_()
/*
  return message type to be used for global operations

  THIS VERSION DATED 06/15/92
  R. VAN DE GEIJN
*/
{
  int result;

  result = MASK + global_all_mask + global_all;
  global_all = (global_all + 1) % MAX_TYPES;

  if (result == 999999999) printf("warning: 999999999 used\n");
  return(result);
}
