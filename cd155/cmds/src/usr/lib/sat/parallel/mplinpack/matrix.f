      SUBROUTINE MATRIX( M, N, NB, A, LDA )
      INTEGER        M, N, NB, LDA
      DOUBLE PRECISION A( LDA, * )
*
*  Purpose
*  =======
*
*  Generate matrix of linear system to be solved.
*
*  Arguments
*  =========
*
*  M      (input) integer
*         row dimension of matrix to be solved/factored
*
*  N      (input) integer
*         column dimension of matrix to be solved/factored
*
*  NB     (input) integer
*         block size used for wrapping the matrix
*
*  A      (output) double precision array, dimension ( lda, myn )
*         local portion of the matrix to be solve/factored
*
*  LDA    (input) integer
*         leading dimention of array that holds local portion 
*         of the matrix ( lda >= mym )
*
*  Details
*  =======
*  The matrix must be distributed among the logical grid of
*  nodes roughly in 2d blocked form:  
*  e.g. if the grid of nodes is 3 x 4, then the assignment is as 
*  follows:
*
*               +----------+----------+----------+----------+
*               |          |          |          |          |
*               |          |          |          |          |
*               |   (0,0)  |   (0,1)  |   (0,2)  |   (0,3)  |
*               |          |          |          |          |
*               |          |          |          |          |
*               +----------+----------+----------+----------+
*               |          |          |          |          |
*               |          |          |          |          |
*               |   (1,0)  |   (1,1)  |   (1,2)  |   (1,3)  |
*               |          |          |          |          |
*               |          |          |          |          |
*               +----------+----------+----------+----------+
*               |          |          |          |          |
*               |          |          |          |          |
*               |   (2,0)  |   (2,1)  |   (2,2)  |   (2,3)  |
*               |          |          |          |          |
*               |          |          |          |          |
*               +----------+----------+----------+----------+

*
*  The size of each block is determined by the fact that the
*  solve routine will be assuming a two dimensional wrap mapping
*  with block size nb.  This makes it complicated to determine
*  what portion of the matrix is assigned to which node.
*
*  Consider wrap mapping the matrix using block size nb:
*
*               
*                +-----+-----+-----+-----+-----+-----+-----+
*                |     |     |     |     |     |     |     |
*                |(0,0)|(0,1)|(0,2)|(0,3)|(0,0)|(0,1)|(0,2)|
*                +-----+-----+-----+-----+-----+-----+-----+
*                |     |     |     |     |     |     |     |
*                |(1,0)|(1,1)|(1,2)|(1,3)|(1,0)|(1,1)|(1,2)|
*                +-----+-----+-----+-----+-----+-----+-----+
*                |     |     |     |     |     |     |     |
*                |(2,0)|(2,1)|(2,2)|(2,3)|(2,0)|(2,1)|(2,2)|
*                +-----+-----+-----+-----+-----+-----+-----+
*                |     |     |     |     |     |     |     |
*                |(0,0)|(0,1)|(0,2)|(0,3)|(0,0)|(0,1)|(0,2)|
*                +-----+-----+-----+-----+-----+-----+-----+
*                |     |     |     |     |     |     |     |
*                |(1,0)|(1,1)|(1,2)|(1,3)|(1,0)|(1,1)|(1,2)|
*                +-----+-----+-----+-----+-----+-----+-----+
*                |     |     |     |     |     |     |     |
*                |(2,0)|(2,1)|(2,2)|(2,3)|(2,0)|(2,1)|(2,2)|
*                +-----+-----+-----+-----+-----+-----+-----+
*                |     |     |     |     |     |     |     |
*                |(0,0)|(0,1)|(0,2)|(0,3)|(0,0)|(0,1)|(0,2)|
*                +-----+-----+-----+-----+-----+-----+-----+
*
*  Notice that some nodes get more blocks than others, and 
*  some nodes get blocks that aren't completely filled.
*
*  We will use the following subroutine to help us determine
*  how large our block is, and where it starts:
*       
*      subroutine imypart( i, j, nb, ii, jj, myproc, nproc )
*
*  consider a vector of length n that has been wrapped among
*  a ring of nproc nodes using block size nb.  Then subvectors 0,
*  nproc, 2*nproc etc are assigned to node 0, subvectors 1, nproc+1,
*  etc to node 1, etc.  Moreover, these subvectors are concatenated
*  to form the local portion of the total vector.  The above
*  subroutine returns the indices ii and jj that indicate the first
*  last element of the the local portion of the vector that starts 
*  with element i, and finishes with element j.
*
*  A special case of this subroutine is to call it like
*
*     call imypart( 1, n, nb, idummy, mym, myrow, nprow )
*     call imypart( 1, n, nb, idummy, myn, mycol, npcol )
* 
*  in which case mym and myn equal the row and column dimensions 
*  of the local portion of a nxn matrix
*
*     THIS VERSION DATED 08/18/92
*     R. VAN DE GEIJN
*
*     All rights reserved
*  .. 
*  .. Local Scalars ..
*
*     nprow          row dimension of node grid
*     npcol          column dimension of node grid
*     myrow          my row index
*     mycol          my column index
*
      INTEGER        NPROW, NPCOL, MYROW, MYCOL
*
*     mym, myn       row and column dimensions of local portion
*
      INTEGER        MYM, MYN
*
*     misc.
*
      INTEGER        I, J, II, JJ, IDUMMY
      double precision  DUMMY
*  .. 
*  .. Local Arrays ..
*  .. 
*  .. Intrinsic Functions ..
*  .. 
*  .. External Functions ..
      DOUBLE PRECISION  RAND
*  .. 
*  .. External Subroutines ..
      EXTERNAL       PLAMCH2, IMYPART
*     ..
*     .. Executable Statements ..
*
*     Get machine parameters
*
      CALL PLAMCH2(NPROW, NPCOL, MYROW, MYCOL)
*    
*     compute where my part of matrix starts
*     inefficient, but it works
*
      II = 1
      DO 50 I=0, MYROW-1
         CALL IMYPART( 1, N, NB, IDUMMY, MYM, I, NPROW )
         II = II+MYM
 50   CONTINUE

      JJ = 1
      DO 60 J=0, MYCOL-1
         CALL IMYPART( 1, N, NB, IDUMMY, MYN, J, NPCOL )
         JJ = JJ+MYN
 60   CONTINUE
      
*
*     this node's portion of the matrix is dimension ( mym, myn )
*
      CALL IMYPART( 1, N, NB, IDUMMY, MYM, MYROW, NPROW )
      CALL IMYPART( 1, N, NB, IDUMMY, MYN, MYCOL, NPCOL )
*
*     generate the portion of the matrix that starts at 
*     element ii, jj, of size ( mym, myn )
*     
*
*     seed the random number generator
*
      DDUMMY = RAND( 1 )
c     New seed to ensure the same test matrix is always generated
c     regardless of nprow, npcol, nb
c
      do 65 j=1,jj-1
         do 64 i=1,n
            ddummy = rand( 0 )
 64      continue
 65   continue


      DO 80 J=JJ, JJ+MYN-1
         do 67 I=1, II-1
            ddummy = rand( 0 )
 67      continue
         DO 70 I=II, II+MYM-1
*
*           a( i-ii+1, j-jj+1 ) = (i,j) element of matrix to 
*                                 be solved
*
            a(i-ii+1, j-jj+1) = rand( 0 )
 70      CONTINUE
         do 75 i=II+mym, n
            ddummy = rand( 0 )
 75      continue
 80   CONTINUE

      do 85 j=jj+myn,n
         do 84 i=1,n
            ddummy = rand( 0 )
 84      continue
 85   continue

      RETURN
      END

