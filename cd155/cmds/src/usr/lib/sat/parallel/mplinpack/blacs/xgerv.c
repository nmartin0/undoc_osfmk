#include "global.h"

/*
  xgerv2d

  purpose
  =======

  receive general (rectangular) array of data of type double 
*/


xgerv2d_(m, n, a, lda, msgtype)
int 
  *m, *n,                /* row and column dimension of array        */
  *lda,                  /* leading dimension of array a             */
  *msgtype;                /* message type (identifier)                */
char
  *a;                    /* array into which data is to be received  */
{
  int i, j;
  char *dp1, *dp2;

  if (*m == *lda || *n == 1) {
    RECV2D( a, *m * *n, *msgtype);
  }
  else {
    if (*m* *n > MAXSPACE) {
      printf("xgerv2d: message too large\n");
      exit(0);
    }
    RECV2D( space, *m * *n , *msgtype);
    /* copy from temporary buffer */
    dp1 = space;
    for (j=0; j<*n; j++) {
      dp2 = &a[j* *lda];
      for (i=0; i<*m; i++) *dp2++ = *dp1++;
    }
  }
}




xgerv2d(m, n, a, lda, msgtype)
int 
  m, n,                /* row and column dimension of array        */
  lda,                  /* leading dimension of array a             */
  msgtype;                /* message type (identifier)                */
char
  *a;                    /* array into which data is to be received  */
{
  int i, j;
  char *dp1, *dp2;

  if (m == lda || n == 1) {
    RECV2D( a, m  *n, msgtype);
  }
  else {
    if (m* n > MAXSPACE) {
      printf("xgerv2d: message too large\n");
      exit(0);
    }
    RECV2D( space, m * n , msgtype);
    /* copy from temporary buffer */
    dp1 = space;
    for (j=0; j<n; j++) {
      dp2 = &a[j* lda];
      for (i=0; i<m; i++) *dp2++ = *dp1++;
    }
  }
}




