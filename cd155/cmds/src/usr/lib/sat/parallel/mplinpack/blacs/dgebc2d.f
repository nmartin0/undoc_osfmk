      subroutine dgebc2d(top, dir, m, n, a, lda, isrc, jsrc, 
     $     msgid)
*     
*     .. Scalar Arguments ..
      character*1          top, dir
      integer            m, n, lda, isrc, jsrc, msgid
*     ..
*     .. Array Arguments ..
      double precision    a( lda, * )
*
*  Purpose
*  =======
*
*  DGEBC2D broadcasts a message from node ISRC,JSRC to 
*  the indicated nodes in the same row or column, or to all 
*  nodes (Fortran to C interface)
*
*  Arguments
*  =========
*
*  TOP     (input) CHARACTER*1
*          Topology to be used for broadcast.  Possibilities
*          include 'RING', 'TREE', 'CTREE' (counter tree), or 
*          'BRING' (backward ring).
*
*  DIR     (input) CHARACTER*1
*          Direction or scope of broadcast. Possibilities include
*          'ROW', 'COLUMN', or 'ALL'
*
*  M       (input) INTEGER
*          The number of rows of the matrix A to be broadcast.  
*          M >= 0.
*
*  N       (input) INTEGER
*          The number of columns of the matrix A to be broadcast.  
*          M >= 0.
*
*  A       (input/output) double precision array of dimension ( LDA, N )
*          Data to be broadcast/received
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
*     .. Parameters ..
*
*     Topology parameters
*
      INTEGER            TREE, RING, CTREE, BRING
      PARAMETER          ( RING = 1, 
     $                     BRING = -1,
     $                     TREE = 2, 
     $                     CTREE = -2 ) 
*
*    Scope parameters
*
      INTEGER            COLUMN, ROW, ALL
      PARAMETER          ( COLUMN = 1, 
     $                     ROW = 2,
     $                     ALL = 3 )

*     ..
*     .. Local Scalars ..
      integer            idir, itop
*     ..
*     .. External Functions ..
      integer            lsame
*     ..
*     .. External Subroutines ..
      external           xerbla, xgebc2d
*     ..
*     .. Executable Statements ..

      if (lsame(top,'R')) then
         itop = RING
      elseif (lsame(top,'T')) then
         itop = TREE
      elseif (lsame(top,'C')) then
         itop = CTREE
      elseif (lsame(top,'B')) then
         itop = BRING
      else
         call xerbla( 'DGEBC2D', 1 )
         stop
      endif

      if (lsame(dir,'C')) then
         idir = COLUMN
      elseif (lsame(dir,'R')) then
         idir = ROW
      elseif (lsame(dir,'A')) then
         idir = ALL
      else
         call xerbla( 'DGEBC2D', 2 )
         stop
      endif

      call xgebc2d(itop, idir, 8*m, n, a, 8*lda, isrc, 
     $     jsrc, msgid)

      return
*
*  End of DGEBC2d
*
      end
