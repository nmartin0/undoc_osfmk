#include "global.h"
#define FORCE_TYPE 1100000000

/*
  xgesds

  purpose
  =======

  send general (rectangular) array of data of type char
  to logical node (irdest, icdest)
*/


xgesds_(m, n, a, lda, irdest, icdest, msgid)
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
      printf("xgesds: message too large\n");
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



xgesds(m, n, a, lda, irdest, icdest, msgid)
int 
  m, n,                  /* row and column dimension of array        */
  lda,                   /* leading dimension of array a             */
  msgid,                 /* message type (identifier)                */
  irdest, icdest;        /* row and column index of destination node */
char
  *a;                    /* array of data to be sent                 */
{
  int i, j, itype_from_(), itype;
  char *dp1, *dp2;

  if (m == lda || n == 1) {
    itype = itype_from_( &irdest, &icdest );
/*    printf("%d %d itype = %d sending to %d %d\n", myrow00, mycol00, itype,
	   irdest, icdest); */
    RECV2D( NULL, 0, itype );
    SEND2D( a, m * n , msgid+FORCE_TYPE, irdest, icdest);
  }
  else {
    if (m* n > MAXSPACE) {
      printf("xgesds: message too large\n");
      exit(0);
    }
    /* copy into temporary buffer */
    dp1 = (char *) space;
    for (j=0; j<n; j++) {
      dp2 = &a[j* lda];
      for (i=0; i<m; i++) *dp1++ = *dp2++;
    } 
    RECV2D( NULL, 0, itype_from_( &irdest, &icdest ) );
    SEND2D( space, m * n, msgid+FORCE_TYPE, irdest, icdest );
  }
}



