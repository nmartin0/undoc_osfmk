#include "global.h"
#define FORCE_TYPE 1100000000

/*
  xgervs

  purpose
  =======

  receive general (rectangular) array of data of type double 
*/


xgervs_(m, n, a, lda, msgtype)
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
      printf("xgervs: message too large\n");
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





xgervs(m, n, a, lda, isrc, jsrc, msgtype)
int 
  m, n,                /* row and column dimension of array        */
  isrc, jsrc,           /* source of message to be received          */
  lda,                  /* leading dimension of array a             */
  msgtype;                /* message type (identifier)                */
char
  *a;                    /* array into which data is to be received  */
{
  int i, j, msgid, itype_to_(), itype;
  char *dp1, *dp2;

  if (m == lda || n == 1) {
    msgid = IRECV2D( a, m  *n, msgtype + FORCE_TYPE );

    /* synchronize */
    itype = itype_to_( &isrc, &jsrc);
/*    printf("%d %d itype = %d receiving form %d %d\n", myrow00, mycol00, itype,
	   isrc, jsrc); */
    SEND2D( NULL, 0, itype, isrc, jsrc );
    msgwait( msgid );
  }
  else {
    if (m* n > MAXSPACE) {
      printf("xgervs: message too large\n");
      exit(0);
    }
    msgid = IRECV2D( space, m * n , msgtype + FORCE_TYPE );

    /* synchronize */
    SEND2D( NULL, 0, itype_to_( &isrc, &jsrc ), isrc, jsrc );
    msgwait( msgid );

    /* copy from temporary buffer */
    dp1 = space;
    for (j=0; j<n; j++) {
      dp2 = &a[j* lda];
      for (i=0; i<m; i++) *dp2++ = *dp1++;
    }
  }
}




