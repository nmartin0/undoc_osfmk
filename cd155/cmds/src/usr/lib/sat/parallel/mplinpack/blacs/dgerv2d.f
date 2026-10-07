      subroutine dgerv2d( m, n, a, lda, isrc, jsrc, msgid)
*     
*     .. Scalar Arguments ..
      integer            m, n, lda, isrc, jsrc, msgid
*     ..
*     .. Array Arguments ..
      double precision   a( lda, * )
*
*  Purpose
*  =======
*
*  DGERV2D receives a message from node ISRC,JSRC
*  (Fortran to C interface)
*
*  Arguments
*  =========
*
*  M       (input) INTEGER
*          The number of rows of the matrix A to be sent.  
*          M >= 0.
*
*  N       (input) INTEGER
*          The number of columns of the matrix A to be sent.  
*          M >= 0.
*
*  A       (input) double precision array of dimension ( LDA, N )
*          Data to be sent
*
*  LDA     (input) INTEGER
*          Leading dimension of A
*  
*  ISRC    (input) INTEGER
*          Row index of source node
*
*  JSRC    (input) INTEGER
*          Column index of source node
*
*  MSGID   (input) INTEGER
*          Message identifier
*
*  =============================================================
*
*     THIS VERSION DATED 09/18/92
*     R. VAN DE GEIJN
*
*     ..
*     .. Local Scalars ..
*     ..
*     .. External Functions ..
*     ..
*     .. External Subroutines ..
      external           xerbla, xgerv2d
*     ..
*     .. Executable Statements ..

      call xgerv2d( 8*m, n, a, 8*lda, isrc, jsrc, msgid )

      return
*
*  End of DGERV2D
*
      end
