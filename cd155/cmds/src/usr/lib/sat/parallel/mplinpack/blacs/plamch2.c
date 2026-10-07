#include "global.h"

plamch2_( nprow, npcol, myrow, mycol )
/*
  PLAMCH2 returns the logical machine parameters
  nprow, npcol   (row and column dimension of logical grid)
  myrow, mycol   (this nodes indices within the grid)

  C to Fortran interface
*/
/* Scalar Argument */
int *nprow, *npcol, 
    *myrow, *mycol;
{
  *nprow = nprow00;
  *npcol = npcol00;
  *myrow = myrow00;
  *mycol = mycol00;
}
