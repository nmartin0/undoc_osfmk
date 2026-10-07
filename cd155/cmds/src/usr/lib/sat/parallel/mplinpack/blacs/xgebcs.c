#include "global.h"

/*
  xgebcs

  purpose
  =======

  broadcast general (rectangular) array of data of type char
  from node *isrc to all nodes in indicated direction.
*/


xgebcs_(itop, idir, m, n, a, lda, isrc, jsrc, msgid)
int 
  *itop,                 /* topology to be used for broadcast        */
  *idir,                 /* direction of broadcast                   */
  *m, *n,                /* row and column dimension of array        */
  *lda,                  /* leading dimension of array a             */
  *isrc,                 /* row index of source node                 */
  *jsrc,                 /* column index of source node              */
  *msgid;                /* message type (identifier)                */
char
  *a;                    /* array of data to be sent                 */
{
  int ibit, imark, imask, ione, i;
  char *datap;

  ione = 1;
/*  if (*m != *lda && *n != 1) {
    datap = space;
    for (i=0; i<*n; i++) 
      bcopy( &a[i * *lda], &datap[i * *m], *m );
  }
  else {
    datap = a;
  } */

  switch (*idir) {
  case COLUMN:
    if (nprow00 == 1 || *jsrc != mycol00) {
      return;
    }
    switch (*itop) {
    case RING:
      if (myrow00 != *isrc) 
	xgervs( *m, *n, a, *lda, (myrow00-1+nprow00)%nprow00, 
	       mycol00, *msgid );
      if ((myrow00+1)%nprow00 != *isrc) 
	xgesds( *m, *n, a, *lda, (myrow00+1)%nprow00, mycol00,
		  *msgid );
      break;
    case CRING:
      if (myrow00 != *isrc) {
	xgervs( *m, *n, a, *lda, (myrow00+1)%nprow00, mycol00,
	       *msgid );
      };
      if ((myrow00-1+nprow00)%nprow00 != *isrc) {
	xgesds( *m, *n, a, *lda, (myrow00-1+nprow00)%nprow00, 
		  mycol00, *msgid );
      }
      break;
    case TREE:
      imark = myrow00^*isrc;
      if (myrow00 != *isrc) {
	for (ibit=1; ibit<=imark; ibit=ibit<<1); 
	xgervs( *m, *n, a, *lda, myrow00^(ibit>>1), mycol00,
	       *msgid );
      }
      else 
	ibit = 1;

      for (; ibit<nprow00; ibit=ibit<<1) 
	  xgesds( *m, *n, a, *lda, (myrow00^ibit), mycol00,
		  *msgid );
      break;
    case CTREE:
      imark = myrow00^*isrc;
      imask = nprow00-1;
      if (myrow00 != *isrc) {
	for (ibit=1; !(ibit&imark); ibit=ibit<<1);
	xgervs( *m, *n, a, *lda, myrow00^ibit, mycol00, *msgid );
	ibit=ibit>>1;
      }
      else 
	ibit = nprow00>>1;

      for (; ibit>0; ibit=ibit>>1) {
	  xgesds( *m, *n, a, *lda, (myrow00^ibit), mycol00,
		  *msgid );
      }
      break;
    }
    break;

  case ROW:
    if (npcol00 == 1 || *isrc != myrow00) return;
    switch (*itop) {
    case RING:
      if (mycol00 != *jsrc) 
	xgervs( *m, *n, a, *lda, myrow00, 
		 (mycol00-1+npcol00)%npcol00, *msgid );
      if ((mycol00+1)%npcol00 != *jsrc) 
	xgesds( *m, *n, a, *lda, myrow00, (mycol00+1)%npcol00, 
		  *msgid );
      break;
    case CRING:
      if (mycol00 != *jsrc) {
	xgervs( *m, *n, a, *lda, myrow00, 
	       (mycol00+1)%npcol00, *msgid );
      };
      if ((mycol00-1+npcol00)%npcol00 != *jsrc) {
	xgesds( *m, *n, a, *lda, myrow00, (mycol00-1+npcol00)%npcol00, 
		  *msgid );
      }
      break;
    case TREE:
      imark = mycol00^*jsrc;
      if (mycol00 != *jsrc) {
	for (ibit=1; ibit<=imark; ibit=ibit<<1); 
	xgervs( *m, *n, a, *lda, myrow00, mycol00^(ibit>>1), *msgid );
      }
      else 
	ibit = 1;

      for (; ibit<npcol00; ibit=ibit<<1) 
	  xgesds( *m, *n, a, *lda, myrow00, (mycol00^ibit), 
		  *msgid );
      break;
    case CTREE:
      imark = mycol00^*jsrc;
      imask = npcol00-1;
      if (mycol00 != *jsrc) {
	for (ibit=1; !(ibit&imark); ibit=ibit<<1);
	xgervs( *m, *n, a, *lda, myrow00, mycol00^ibit, *msgid );
	ibit=ibit>>1;
      }
      else 
	ibit = npcol00>>1;

      for (; ibit>0; ibit=ibit>>1) {
	  xgesds( *m, *n, a, *lda, myrow00, (mycol00^ibit), 
		  *msgid );
      }
      break;
    }
    break;
  case ALL:
    printf("broadcast ALL not yet implemented\n");
    exit(1);
    break;
  default:
    printf("broadcast direction %d unknown\n");
    exit(1);
    break;
  }

/*  if (*m != *lda && *n != 1) {
    for (i=0; i<*n; i++) 
      bcopy( &datap[i * *m], &a[i * *lda], *m );
  } */
}
