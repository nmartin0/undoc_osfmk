      subroutine igesd2d( m, n, a, lda, idest, jdest, msgid)
*     
*     .. Scalar Arguments ..
      integer            m, n, lda, idest, jdest, msgid
*     ..
*     .. Array Arguments ..
      integer            a( lda, * )
*
*  Purpose
*  =======
*
*  IGESD2D sends a message from this node to node IDEST,JDEST
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
*  A       (input) INTEGER array of dimension ( LDA, N )
*          Data to be sent
*
*  LDA     (input) INTEGER
*          Leading dimension of A
*  
*  IDEST   (input) INTEGER
*          Row index of destination node
*
*  JDEST   (input) INTEGER
*          Column index of destination node
*
*  MSGID   (input) INTEGER
*          Message identifier
*
*  =============================================================
*
*     THIS VERSION DATED 06/15/92
*     R. VAN DE GEIJN
*
*     ..
*     .. Local Scalars ..
*     ..
*     .. External Functions ..
*     ..
*     .. External Subroutines ..
      external           xerbla, xgesd2d
*     ..
*     .. Executable Statements ..

      call xgesd2d( 4*m, n, a, 4*lda, idest, jdest, msgid )

      return
*
*  End of IGESD2d
*
      end
