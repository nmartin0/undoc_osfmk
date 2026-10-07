      SUBROUTINE DGAMAX2D( DIR, M, N, A, LDA, B, LDB, IRSRC, LDIRSRC,
     $     ICSRC, LDICSRC, IRDEST, ICDEST )
*     
*     .. Scalar Arguments ..
      character*1        dir
      integer            m, n, lda, ldb, irsrc, icsrc, ldirsrc, 
     $                   ldicsrc, irdest, icdest
*     ..
*     .. Array Arguments ..
      double precision   a( lda, * ), b( ldb, * )
      integer            irsrc( ldirsrc, * ), icsrc( ldicsrc, * )
*
*  Purpose
*  =======
*
*  DGAMAX2D computes the global element-wise absolute maximimum 
*  of all arrays A of nodes indicated by DIR, leaving the
*  entries that yielded the absolute max in array B, and the
*  node indices that yielded the absolute max in arrays IRSRC
*  and ICSRC on the node indicated by IRDEST,ICDEST
*
*  Arguments
*  =========
*
*  DIR     (input) CHARACTER*1
*          Direction or scope of broadcast. Possibilities include
*          'ROW', 'COLUMN', or 'ALL'
*
*  M       (input) INTEGER
*          The number of rows of the matrix A involved in the 
*          global sum. M >= 0.
*
*  N       (input) INTEGER
*          The number of columns of the matrix A involved in the
*          global sum. M >= 0.
*
*  A       (input) double precision  array of dimension 
*          ( LDA, N )
*          This node's contibution to the global absolute max
*
*  LDA     (input) INTEGER
*          Leading dimension of A
*
*  B       (output) double precision  array of dimension 
*          ( LDB, N )
*          On the node(s) indicated by IRDEST,ICDEST, the result.
*          Intermediate results on other nodes (garbage).
*
*  IRSRC   (output) integer array of dimension ( LDIRSRC, N )
*          On the node(s) indicated by IRDEST,ICDEST, the row 
*          indices of the nodes that contributed the result.
*          
*  LDIRSRC (input) integer
*          Leading dimension of IRSRC
*
*  ICSRC   (output) integer array of dimension ( LDICSRC, N )
*          On the node(s) indicated by IRDEST,ICDEST, the row 
*          indices of the nodes that contributed the result.
*          
*  LDICSRC (input) integer
*          Leading dimension of ICSRC
*
*  LDB     (input) INTEGER
*          Leading dimension of B
*  
*  IRDEST  (input) INTEGER
*          Row index of destination node
*
*  ICDEST  (input) INTEGER
*          Column index of destination node
*
*  =============================================================
*
*     THIS VERSION DATED 06/15/92
*     R. VAN DE GEIJN
*
*     .. Parameters ..
*
*    Scope parameters
*
      INTEGER            COLUMN, ROW, ALL
      PARAMETER          ( COLUMN = 1, 
     $                     ROW = 2,
     $                     ALL = 3 )
*     ..
*     .. Local Scalars ..
      integer            idir
*     ..
*     .. External Functions ..
      integer            lsame


      
      IF (LSAME(DIR,'C')) THEN
         IDIR = COLUMN
      ELSEIF (LSAME(DIR,'R')) THEN
         IDIR = ROW
      ELSEIF (LSAME(DIR,'A')) THEN
         IDIR = ALL
      ELSE
         PRINT *, "ERROR IN DGSUM2D, PARAMETER 1"
         STOP
      ENDIF

      CALL DGAMAX2DC( IDIR, M, N, A, LDA, B, LDB, IRSRC, LDIRSRC, 
     $     ICSRC, LDICSRC, IRDEST, ICDEST )

      RETURN
      END


