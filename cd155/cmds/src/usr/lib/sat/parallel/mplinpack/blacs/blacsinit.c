#include "global.h"

/* global machine dependent node parameters  */

int mypid00,                    /* node process id                       */
    nprow00, npcol00,           /* logical row and column dimension grid */
    myrow00, mycol00;           /* logical row and column index of node  */

char space[MAXSPACE];

blacs_init_()
/*
    Purpose
    =======

    initialize the blacs routines, including node parameters

    THIS VERSION DATED 06/15/92
    R. VAN DE GEIJN
*/

{
  /* local scalars */
  int i, me;                            /* local scalars */

  /* external functions */
  int mypid(), mynode(), numnodes();

  /* get node information */
  me = mynode();                        /* me = this node's index        */
  mypid00 = mypid();                    /* mypid00 = process id of nodes */

  if (me == 0) {
    /* get logical node grid dimensions */
    printf("enter nprow, npcol:");
    scanf("%d%d", &nprow00, &npcol00);
    if (nprow00 * npcol00 != numnodes()) {
      printf("illegal grid dimensions\n");
      exit(0);
    }
    
    /* send logical grid information to all nodes */
    csend(1000, &nprow00, 4, -1, mypid00);
    csend(999, &npcol00, 4, -1, mypid00);
  }
  else {
    /* receive logical grid information */
    crecv(1000, &nprow00, 4);
    crecv(999, &npcol00, 4);
  }

  /* compute this node's row and column index */
  myrow00 = me/npcol00;
  mycol00 = me%npcol00;
  
  /* initialize type generation parameters */
  type_init(); 
}


