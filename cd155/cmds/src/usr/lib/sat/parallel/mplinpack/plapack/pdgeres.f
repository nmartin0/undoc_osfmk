      DOUBLE PRECISION FUNCTION PDGERES( N, NB, A, LDA, X, 
     $     B, WORK )
*
*     Parallel routine for computing overwrite b =  Ax - b 
*     and return largest entry of this in absolute value.
*
*
*     .. Scalar Arguments ..
      INTEGER            LDA, N, NB
*     ..
*     .. Array Arguments ..
      DOUBLE PRECISION   A( LDA, * ), X( * ), B( * ), 
     $                   WORK( * )
*     ..
*
*  Purpose
*  =======
*
*  PDGERES computes the residual b =  Ax - b  and the largest
*  element of this residual
*
*  Arguments
*  =========
*
*  N       (input) INTEGER
*          The number of rows and columns of the matrix A.  N >= 0.
*
*  NB      (input) INTEGER
*          The block size for the blocked algorithm.  NB > 0.
*
*  A       (input) DOUBLE PRECISION  array, dimension (LDA,LOCALN)
*          This node's part of matrix A.
*
*  LDA     (input) INTEGER
*          The leading dimension of the array A that holds the local
*          portion of the matrix.  LDA >= max(1,LOCALM).
*
*  X       (input) double precision array of dimension ( localn )
*          On first row of nodes: estimate of the solution to Ax = b.
*          (DISTRIBUTED LIKE FIRST ROW OF MATRIX) 
*
*  B       (input/output) DOUBLE PRECISION array, dimension ( LOCALM )
*          On column of nodes: 
*          Before: local portion of original right hand side of system 
*          After: local portion of b = Ax-b
*          (DISTRIBUTED LIKE FIRST COLUMN OF MATRIX) 
*
*  WORK    DOUBLE PRECISION array, dimension ( 2*LOCALM )
*          Work array      
*
*  =====================================================================
*
*     THIS VERSION DATED 12/1/92
*     R. VAN DE GEIJN
*
*     All rights reserved
*
*     .. Parameters ..
      DOUBLE PRECISION   ZERO, ONE
      PARAMETER          ( ZERO = 0.0D+0 ,
     $                     ONE =  1.0D+0 )
*     
*     Scope parameters
*
      INTEGER            COLUMN, ROW
      PARAMETER          ( COLUMN = 1, ROW = 2 )
*
*     Topology parameters
*
      INTEGER            TREE, RING, CTREE
      PARAMETER          ( RING = 1, TREE = 2, CTREE = -2 )
*     ..
*     .. Local Scalars ..
*
*     ..
*     .. Misc. ..
      INTEGER            I, INFO, IINFO
*
*     nprow          row dimension of node grid
*     npcol          column dimension of node grid
*     myrow          my row index
*     mycol          my column index
*
      INTEGER           NPROW, NPCOL, MYROW, MYCOL
      INTEGER           LOCALM, LOCALN, IROW, ICOL, IDUMMY
      INTEGER           IMAX
      DOUBLE PRECISION  DMAX, DTEMP, DABS
*     ..
*     .. External Functions ..
      LOGICAL           LSAME
      INTEGER           IDAMAX
*     ..
*     .. External Subroutines ..
      EXTERNAL           PDTRSV, XERBLA, IMYPART, 
     $                   PLAMCH2
*     ..
*     .. Executable Statements ..
*
*
*     get machine parameters
*
      CALL PLAMCH2( NPROW, NPCOL, MYROW, MYCOL )
*
*     determine local number of rows and columns
*
      CALL IMYPART( 1, N, NB, IDUMMY, LOCALM, MYROW, NPROW )
      CALL IMYPART( 1, N, NB, IDUMMY, LOCALN, MYCOL, NPCOL )
      
*
*     check input parameters.  
*
      INFO = 0
      IF( N.LT.0 ) THEN
         INFO = -1
      ELSE IF( LDA.LT.MAX( 1, LOCALM ) ) THEN
         INFO = -4
      ELSE IF( NB.LE.0 ) THEN
         INFO = -2
      END IF
      IF( INFO.NE.0 ) THEN
         CALL XERBLA( 'DZGERES', -INFO )
         RETURN
      END IF

*
*     compute A x
*
      CALL PDGEMV( N, NB, A, LDA, X, WORK, WORK(LOCALM+1), INFO )

      if (MYCOL .EQ. 0) then 
*     
*     compute b - A x = b - work  (local portion)
*
         CALL Daxpy( localM, -ONE, work, 1, b, 1 )
*
*     Compute largest local error
*
         imax = IDAMAX( localM, B, 1 )
*
*     Compute global absolute error
*
         dtemp = dabs( B( imax ) )
         call DGAMAX2D( 'COLUMN', 1, 1, dtemp, 1, dmax, 1, irow, 1,
     $        icol, 1, 0, 0 )
      endif

      pdgeres = dmax

      return
      end
