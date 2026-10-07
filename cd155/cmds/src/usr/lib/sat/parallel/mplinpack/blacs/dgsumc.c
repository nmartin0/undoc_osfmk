#include "global.h"

dgsum2dc_( idir, m, n, a, lda, b, ldb, irdest, icdest )
int *idir, *m, *n, *lda, *ldb, *irdest, *icdest;
double *a, *b;
{
  double one;
  int ione, i, j, ii, jj, ibit, idest, isrc;

  one = 1.0;
  ione = 1;

  if ( *m > *lda ) {
    printf("DGSUM: error parameter 5\n");
    exit(0);
  }
  else if ( *m  > *ldb ) {
    printf("DGSUM: error parameter 7\n");
    exit(0);
  }

  ii = 0;
  jj = 0;
  if (*idir == COLUMN) {
    /* currently only exact power of two supported */
    for (i=0; i<*n; i++) {
      dcopy_( m, &a[ii], &ione, &b[jj], &ione );
      for (ibit=1; ibit<nprow00; ibit=ibit<<1) {
	idest = myrow00^ibit;
	SEND2D( &b[jj], *m*8, itype_to_( &idest, &mycol00 ),
	       idest, mycol00 );
	RECV2D( space, *m*8, itype_from_(&idest, &mycol00 ) );
	daxpy_( m, &one, space, &ione, &b[jj], &ione );
      }
      ii = ii+*lda;
      jj = jj+*ldb;
    }
  }
  else if (*idir == ROW) {
    /* currently only exact power of two supported */
    for (i=0; i<*n; i++) {
      dcopy_( m, &a[ii], &ione, &b[jj], &ione );
/*      for (ibit=1; ibit<npcol00; ibit=ibit<<1) {
	idest = mycol00^ibit;
	SEND2D( &b[jj], *m*8, itype_to_( &myrow00, &idest ),
	       myrow00, idest );
	RECV2D( space, *m*8, itype_from_(&myrow00, &idest ) );
	daxpy_( m, &one, space, &ione, &b[jj], &ione );
      } */
      /* very inefficient, but handles any number of columns */
      idest = (mycol00+1)%npcol00;
      isrc =  (mycol00-1+npcol00) % npcol00;
      for (j=1; j<npcol00;j++) {
	if (j==1) 
	  SEND2D( &a[ii], *m*8, itype_to_( &myrow00, &idest ),
		 myrow00, idest );
	else 
	  SEND2D( space, *m*8, itype_to_( &myrow00, &idest ),
		 myrow00, idest );

	RECV2D( space, *m*8, itype_from_(&myrow00, &isrc ) );
	daxpy_( m, &one, space, &ione, &b[jj], &ione );
      }
	    
      ii = ii+*lda;
      jj = jj+*ldb;
    }
  }
  else if (*idir == ALL) {
    printf("DGSUM: ALL not yet supported");
    exit(0);
  }
}

    
  
