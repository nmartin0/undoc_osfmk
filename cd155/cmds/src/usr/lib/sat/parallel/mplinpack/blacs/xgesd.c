#include "global.h"

/*
  xgesd2d

  purpose
  =======

  send general (rectangular) array of data of type char
  to logical node (irdest, icdest)
*/


xgesd2d_(m, n, a, lda, irdest, icdest, msgid)
int 
  *m, *n,                /* row and column dimension of array        */
  *lda,                  /* leading dimension of array a             */
  *msgid,                /* message type (identifier)                */
  *irdest, *icdest;      /* row and column index of destination node */
char
  *a;                    /* array of data to be sent                 */
{
  int i, j;
  char *dp1, *dp2;

  if (*m == *lda || *n == 1) {
    SEND2D( a, *m * *n , *msgid, *irdest, *icdest);
  }
  else {
    if (*m* *n > MAXSPACE) {
      printf("xgesd2d: message too large\n");
      exit(0);
    }
    /* copy into temporary buffer */
    dp1 = (char *) space;
    for (j=0; j<*n; j++) {
      dp2 = &a[j* *lda];
      for (i=0; i<*m; i++) *dp1++ = *dp2++;
    } 
    SEND2D( space, *m * *n, *msgid, *irdest, *icdest );
  }
}



xgesd2d(m, n, a, lda, irdest, icdest, msgid)
int 
  m, n,                  /* row and column dimension of array        */
  lda,                   /* leading dimension of array a             */
  msgid,                 /* message type (identifier)                */
  irdest, icdest;        /* row and column index of destination node */
char
  *a;                    /* array of data to be sent                 */
{
  int i, j;
  char *dp1, *dp2;

  if (m == lda || n == 1) {
    SEND2D( a, m * n , msgid, irdest, icdest);
  }
  else {
    if (m* n > MAXSPACE) {
      printf("xgesd2d: message too large\n");
      exit(0);
    }
    /* copy into temporary buffer */
    dp1 = (char *) space;
    for (j=0; j<n; j++) {
      dp2 = &a[j* lda];
      for (i=0; i<m; i++) *dp1++ = *dp2++;
    } 
    SEND2D( space, m * n, msgid, irdest, icdest );
  }
}



