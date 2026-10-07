#include "global.h"

#define dabs(x) ( (x) >= 0.0 ? (x) : -(x) )

dgamax2dc_( idir, m, n, a, lda, b, ldb, irsrc, ldirsrc, icsrc, ldicsrc,
	   irdest, icdest )
int *idir, *m, *n, *lda, *ldb, *ldirsrc, *ldicsrc, *irdest, *icdest,
*irsrc, *icsrc;
double *a, *b;
{
  int i, j, ii, ibit, idest, ione, jj, iii, jjj, *itempp;
  double *dtempp;

  if ( *m > *lda ) {
    printf("DGMAX: error parameter 5\n");
    exit(0);
  }
  else if ( *m > *ldb ) {
    printf("DGMAX: error parameter 7\n");
    exit(0);
  }

  ione = 1;

  dtempp = (double *) space;
  itempp = (int *) space+*m*8;
  if (*idir == COLUMN) {
    ii = 0;
    jj = 0;
    iii = 0;
    jjj = 0;
    for (j=0; j<*n; j++) {
      for (i=0; i<*m; i++) {
	icsrc[iii+i] = mycol00;
	irsrc[jjj+i] = myrow00;
      }
      /* currently only exact power of two supported */
      dcopy_( m, &a[ii], &ione, &b[jj], &ione );
      for (ibit=1; ibit<nprow00; ibit=ibit<<1) {
	idest = myrow00^ibit;
	SEND2D( &b[jj], *m*8, itype_to_( &idest, &mycol00 ),
	       idest, mycol00 );
	SEND2D( &irsrc[jjj], *m*4, itype_to_( &idest, &mycol00 ),
	       idest, mycol00 );
	RECV2D( dtempp, *m*8, itype_from_(&idest, &mycol00 ) );
	RECV2D( itempp, *m*4, itype_from_(&idest, &mycol00 ) );
	for (i=0; i<*m; i++) {
	  if ( dabs(dtempp[i]) > dabs(b[jj+i]) ) {
	    b[jj+i] = dtempp[i];
	    irsrc[jjj+i] = itempp[i];
	  } 
	  else if (dabs(dtempp[i]) == dabs(b[jj+i]) ) {
	    irsrc[jjj+i] = (irsrc[jjj+i] < itempp[i] ? 
			    irsrc[jjj+i] : itempp[i] );
	  } 
	}
      }
      ii = ii+*lda;
      jj = jj+*ldb;
      iii = iii+*ldicsrc;
      jjj = jjj+*ldirsrc;
    }
  }
  else if (*idir == ROW) { 
    ii = 0;
    jj = 0;
    iii = 0;
    jjj = 0;
    for (j=0; j<*n; j++) {
      for (i=0; i<*m; i++) {
	icsrc[iii+i] = mycol00;
	irsrc[jjj+i] = myrow00;
      }
      /* currently only exact power of two supported */
      dcopy_( m, &a[ii], &ione, &b[jj], &ione );
      for (ibit=1; ibit<npcol00; ibit=ibit<<1) {
	idest = mycol00^ibit;
	SEND2D( &b[jj], *m*8, itype_to_( &myrow00, &idest ),
	       myrow00, idest );
	SEND2D( &icsrc[iii], *m*4, itype_to_( &myrow00, &idest ),
	       myrow00, idest ); 
	RECV2D( dtempp, *m*8, itype_from_(&myrow00, &idest ) ); 
	RECV2D( itempp, *m*4, itype_from_(&myrow00, &idest ) ); 
	for (i=0; i<*m; i++) {
	  if ( dabs(dtempp[i]) > dabs(b[jj+i]) ) {
	    b[jj+i] = dtempp[i];
	    icsrc[iii+i] = itempp[i];
	  } 
	  else if (dabs(dtempp[i]) == dabs(b[jj+i]) ) {
	    icsrc[iii+i] = (icsrc[iii+i] < itempp[i] ? 
			    icsrc[iii+i] : itempp[i] );
	  } 
	}
      }
      ii = ii+*lda;
      jj = jj+*ldb;
      iii = iii+*ldicsrc;
      jjj = jjj+*ldirsrc;
    }
  }
  else if (*idir == ALL) {
    printf("DGSUM: ALL not yet supported");
    exit(0);
  }
}

    
  
