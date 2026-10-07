#define max(x,y) ((x)<(y)?(y):(x))
#define NULL 0
mainc_()
{
  int nprow, npcol, myrow, mycol, nbad;
  int n, nb, mym, myn, lda, iworksize, itemp, i, j;
  double rcond, dmax, pdgeres_();
  double time1, time2, time3;
  int info, idummy, nstart, nfinish, ninc,
      nbstart, nbfinish, nbinc;
  double *a, *work, *x, *y, anrm1, pdmnrm1_(), *aold;
  int *ipiv, iidummy;
  int itype_row(), itype_col(); 
  double dclock();
  double timelu, timecon;
  int i_one = 1, i_zero = 0 ;

  gsync();
  nbad = 0;
/*
*
*     initialize blacs
*/
  blacs_init_();
/*
*     get architecture dependent information
*/
  plamch2_(&nprow, &npcol, &myrow, &mycol);
/*
*     get problem dimension and block size used for 
*     wrapping 
*/
  if ( myrow == 0 && mycol == 0 ) {
    printf("enter nstart, nfinish, ninc\n");
    scanf("%d", &nstart);
    scanf("%d", &nfinish);
    scanf("%d", &ninc);
    printf("enter nbstart, nbfinish, nbinc:\n");
    scanf("%d", &nbstart);
    scanf("%d", &nbfinish);
    scanf("%d", &nbinc);
  }

/*
*     broadcast parameters within first row
*/
  if ( myrow == 0 ) {
    itemp = itype_row_();
    igebc2d_( "ring", "row", &i_one, &i_one, &nstart, &i_one, 
	     &i_zero, &i_zero, &itemp );
    itemp = itype_row_();
    igebc2d_( "ring", "row", &i_one, &i_one, &nfinish, &i_one, 
	    &i_zero, &i_zero, &itemp );
    itemp = itype_row_();
    igebc2d_( "ring", "row", &i_one, &i_one, &ninc, &i_one, 
	     &i_zero, &i_zero, &itemp );
    itemp = itype_row_();
    igebc2d_( "ring", "row", &i_one, &i_one, &nbstart, &i_one, 
	     &i_zero, &i_zero, &itemp );
    itemp = itype_row_();
    igebc2d_( "ring", "row", &i_one, &i_one, &nbfinish, &i_one, 
	     &i_zero, &i_zero, &itemp );
    itemp = itype_row_();
    igebc2d_( "ring", "row", &i_one, &i_one, &nbinc, &i_one, 
	     &i_zero, &i_zero, &itemp );
  }
/*
*     broadcast parameters within columns
*/
  itemp = itype_col_();
  igebc2d_( "tree", "col", &i_one, &i_one, &nstart, &i_one, &i_zero, &mycol, 
	   &itemp );
  itemp = itype_col_();
  igebc2d_( "tree", "col", &i_one, &i_one, &nfinish, &i_one, &i_zero, &mycol, 
	   &itemp );
  itemp = itype_col_();
  igebc2d_( "tree", "col", &i_one, &i_one, &ninc, &i_one, &i_zero, &mycol, 
	   &itemp );
  itemp = itype_col_();
  igebc2d_( "tree", "col", &i_one, &i_one, &nbstart, &i_one, &i_zero, &mycol, 
	   &itemp );
  itemp = itype_col_();
  igebc2d_( "tree", "col", &i_one, &i_one, &nbfinish, &i_one, &i_zero, &mycol, 
	   &itemp );
  itemp = itype_col_();
  igebc2d_( "tree", "col", &i_one, &i_one, &nbinc, &i_one, &i_zero, &mycol, 
	   &itemp );

  for (n=nstart; n<=nfinish; n+=ninc) {
     for (nb=nbstart; nb<=nbfinish; nb+=nbinc) {
/*
*     determine size of local block
*/
       imypart_( &i_one, &n, &nb, &idummy, &mym, &myrow, &nprow );
       imypart_( &i_one, &n, &nb, &idummy, &myn, &mycol, &npcol );

       for (i=0; i<nprow;i++)
	 for (j=0; j<npcol; j++) {
	   gsync();
	   if (myrow != i || mycol != j) continue;
	     
	   aold = a = (double *) malloc( sizeof(double) * myn * mym +32 );

	   if (a == NULL) {
	     printf("not enough space to store matrix\n");
	     exit(0);
	   };

	   iidummy = irecv( 999999999, a, sizeof(double) * 
                        myn * mym );
	   
	   work = (double *) malloc( sizeof(double) * (mym + myn + nb) * 
				    (nb > 1 ? nb : 2) );
	   if (work == NULL) {
	     printf("not enough work space\n");
	     exit(0);
	   };

	   iidummy = irecv( 999999999, work, sizeof(double) *
			   (mym + myn + nb) * nb );

	   iworksize = (mym + myn + nb) * nb;
	   x = (double *) malloc( sizeof(double) * max(mym, myn) );
	   if (x == NULL) {
	     printf("not enough space for x\n");
	     exit(0);
	   };

	   iidummy = irecv( 999999999, x, sizeof(double) *
			   max(mym, myn) );


	   y = (double *) malloc( sizeof(double) * max(mym, myn) );
	   if (y == NULL) {
	     printf("not enough space for y\n");
	     exit(0);
	   };

	   iidummy = irecv( 999999999, y, sizeof(double) *
			   max( mym, myn) );

	   ipiv = (int *) malloc( sizeof(int) * n );
	   if (ipiv == NULL) {
	     printf("not enough space for ipiv\n");
	     exit(0);
	   };

	   iidummy = irecv( 999999999, ipiv, sizeof(int) * n );

	 }

/*

*     set leading dimension of matrix
*/
      lda = max( mym, 1 );
/*
*     generate matrix
*/
/*jrb 7/9/93      if (myrow == 0 && mycol == 0) 
	printf("generating test problem\n");   7/9/93 jrb */

      matrix_( &n, &n, &nb, a, &lda );
/*
*     generate right-hand-side
*     on all other nodes, x must be initialized to zero
*/
/*      if ( mycol == 0 ) 
         dvector_( "column", &n, &nb, x );
      else
         dzerovec_( "column", &n, &nb, x );
*/
      if ( myrow == 0 )
	for (j=0; j<myn; j++) y[j] = 1.0;
      else
	for (j=0; j<myn; j++) y[j] = 0.0;

      pdgemv_( &n, &nb, a, &lda, y, x, work, &info );

       if (mycol != 0)
	 for (i=0; i<mym; i++) x[i] = 0.0;

      gsync();

/* jrb 7/9/93      if (myrow == 0 && mycol == 0) {
	printf("done generating test problem\n");
	printf("starting solve\n");
      }   jrb 7/9/93  */

      time1 = dclock();
/*
*     factor matrix 
*/
      pdlubr_( &n, &n, &nb, a, &lda, work, &iworksize, ipiv, &info );
      
      gsync();

      time2 = dclock();
      time1 = time2 - time1;
/*
*     pivot right-hand-side consistent with the pivoting of the
*     matrix during the factorization
*/ 
      
      if (mycol ==0) 
         pdrhsswap_( &n, &nb, x, ipiv ); 
/*
*     forward solve
*/
      pdtrsv_("lower triangular", "no transpose", 
	     "unit", &n, &nb, a, &lda, x, &i_one, work, 
	     &iworksize, &info); 
/*      
*     backward solve
*/
      pdtrsv_("upper triangular", "no transpose", 
	     "non unit", &n, &nb, a, &lda, x, &i_one, work, 
	     &iworksize, &info); 
/*
*     send result to first row of nodes, leaving it distributed
*     like he first row of the matrix
*/
      pdvredistr_( "column", "diag", &i_zero, &n, &nb, x, 
		 "row", "regular", &i_zero, work );

      gsync();
      time2 = dclock() - time2;
/*
*     print first and last entry of solution
*/
      if ( myrow == 0 && mycol == 0 ) {
	printf( "*****************************************\n");
	printf( "first entry of solution: %15.12f\n", x[0] );
      }

      gsync();
      if ( myrow == 0 && mycol == npcol-1 )
	printf( "last entry of solution : %15.12f\n\n", x[myn-1] );
/*
*     regenerate matrix
*/
      if (myrow == 0 && mycol == 0) {
	printf("regenerating test problem\n");
      }


      matrix_( &n, &n, &nb, a, &lda );
/*
*     compute || A ||_1 
*/
/*
      anrm1 = pdmnrm1_( &n, &n, &nb, a, &lda, work );
*/
      anrm1 = n;
/*
*     regenerate right-hand-side
*     on all other nodes, b must be initialized to zero
*/
/*      if ( mycol == 0 ) 
         dvector_( "column", &n, &nb, y );
      else
         dzerovec_( "column", &n, &nb, y ); */

      if ( myrow == 0 )
	for (j=0; j<myn; j++) work[j] = 1.0;
      else
	for (j=0; j<myn; j++) work[j] = 0.0;

      pdgemv_( &n, &nb, a, &lda, work, y, &work[mym+myn], &info );

       if (mycol != 0)
	 for (i=0; i<mym; i++) y[i] = 0.0;

/*
*     compute || Ax - b ||  (residual check)
*/

      dmax = pdgeres_( &n, &nb, a, &lda, x, y, work );


      if (myrow == 0 && mycol == 0) {
	printf( "MAX RESIDUAL:            %lf\n", dmax);
/*
*     compute || Ax - b ||/ n* eps * anmr1 * || x ||
*     (should be < 1)
*/
/*	printf( "|| Ax - b ||/ (n* eps * anrm1 * || x ||) = %f \n",
	       dmax / ( 1.0e-15 * anrm1 * n ) ); */
	if ( dmax / ( 1.0e-15 * anrm1 * n ) > 1.0) {
	  printf("ERROR:  Residual too large\n");
	  nbad++;
	}
	else
	  printf( "PASSED RESIDUAL CHECK\n\n");
      }
      
/*
*     report timing results
*/
      if (myrow == 0 && mycol == 0) {
	printf("np, nprow         = %d %d\n", nprow*npcol, nprow);
	printf("n, nb             = %d %d\n", n, nb); 
	
	printf("time    lu        = %e\n", time1);
	printf("mflops  lu        = %f\n", 2.0/3.0 * n * n * n / 
	       time1 * 1.0e-6);
	printf("time    tr. solve = %e\n", time2);
	printf("mflops  tr. solve = %f\n", 2.0 * n * n / 
	       time2 * 1.0e-6);
	printf("time    total     = %e\n", time1 + time2);
	printf("mflops  total     = %f\n", 2.0/3.0 * n * n * n / 
	       (time1 + time2) * 1.0e-6 );
        printf("mflops/node total = %f\n", 2.0/3.0 * n * n * n / 
	       (time1 + time2) * 1.0e-6 / (nprow*npcol));
	printf( "*****************************************\n");
	if (nbad > 0) printf("nbad so far: %d\n", nbad);
        printf("\n");
      }
	free( aold );
	free( work );
	free( x );
	free( y );
	free( ipiv );

     }
   }
  gsync();
  if (myrow == 0 && mycol == 0)
    printf("TOTAL RESIDUAL PROBLEMS DETECTED: %d\n", nbad);
  gsync();

  exit(0);
}



