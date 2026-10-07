      SUBROUTINE PDGEMV( N, NB, A, LDA, X, Y, WORK, INFO )
*
*     Parallel blocked matrix vector multiplication routine
*
*
*
*     .. Scalar Arguments ..
      INTEGER            LDA, N, NB, LDWORK, INFO
*     ..
*     .. Array Arguments ..
      DOUBLE PRECISION   A( LDA, * ), X( * ), Y( * ), WORK( * )
*     ..
*
*  Purpose
*  =======
*
*  PZGEMV computes 
*            y = A x 
*  where initially x is distributed like the first row of
*  the matrix, and y is left distributed like the first
*  column of the matrix.
*
*  Arguments
*  =========
*
*  N       (input) INTEGER
*          The number of rows and columns of the matrix A.  N >= 1.
*
*  NB      (input) INTEGER
*          The block size for the blocked algorithm.  NB > 0.
*
*  A       (input) DOUBLE PRECISION array, dimension (LDA,LOCALN)
*          This node's portion of n by n matrix 
*
*  LDA     (input) INTEGER
*          The leading dimension of the array A that holds the local
*          portion of the matrix.  LDA >= max(1,LOCALM).
*
*  X       (input) double precision array of dimension (LOCALN)
*          Vector to be multiplied by A
*
*  Y       (output) DOUBLE PRECISION array, dimension ( LOCALM )
*          Result vector.
*
*  WORK    DOUBLE PRECISION array, dimension ( LOCALM )
*          Work array.
*
*  INFO    (output) INTEGER
*          = 0: successful exit
*          < 0: if INFO = -k, the k-th argument had an illegal value
*
*  =====================================================================
*
*     THIS VERSION DATED 012/1/92
*     R. VAN DE GEIJN
*
*     All rights reserved
*
*     .. Parameters ..
      DOUBLE PRECISION     ZERO, ONE
      PARAMETER          ( ZERO =  0.0D+0, ONE = 1.0D+0)
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
      INTEGER            IINFO
      DOUBLE PRECISION   ANRM1
*
*     nprow          row dimension of node grid
*     npcol          column dimension of node grid
*     myrow          my row index
*     mycol          my column index
*
      INTEGER           NPROW, NPCOL, MYROW, MYCOL
      INTEGER           LOCALM, LOCALN, IDUMMY
*     ..
*     .. External Functions ..
      DOUBLE PRECISION  PDMNRM1
      LOGICAL           LSAME
      INTEGER           ITYPE_COL 
*     ..
*     .. External Subroutines ..
      EXTERNAL           XERBLA, IMYPART, 
     $                   PLAMCH2
*     ..
*     .. Intrinsic Functions ..
*     ..
*     .. Executable Statements ..
*
*     Test the input parameters.
*
      INFO = 0

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
*     check input parameters  
*
      IF( N.LT.0 ) THEN
         INFO = -1
      ELSE IF( LDA.LT.MAX( 1, LOCALM ) ) THEN
         print *, "LDA = ", lda, "localm = ", localm
         INFO = -4
      ELSE IF( NB.LE.0 ) THEN
         INFO = -2
      END IF
      IF( INFO.NE.0 ) THEN
         CALL XERBLA( 'PDGEMV ', -INFO )
         RETURN
      ENDIF
*
*     Distribute X within columns of nodes
*
      call DGEBC2D( 'TREE', 'COLUMN', LOCALN, 1, X, LOCALN, 
     $     0, MYCOL, itype_col() )
*
*     Perform local matrix vector multiply
*
      call Dgemv( 'No Transpose', LOCALM, LOCALN, ONE, A, 
     $     LDA, X, 1, ZERO, WORK, 1 )
*
*     Combine local results
*
      call DGSUM2D( 'Row', LOCALM, 1, WORK, LOCALM, y, LOCALM,
     $     MYROW, 0 )
*
*  End of PDGEMV
*
      END

      

