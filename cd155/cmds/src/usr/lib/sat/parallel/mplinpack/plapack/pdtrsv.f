      subroutine pdtrsv( uplo, trans, diag, n, nb, a, lda, x, incx,
     $     work, ldwork, info) 
*     .. scalar arguments ..
      integer            incx, lda, n, nb, ldwork
      character*1        diag, trans, uplo
*     .. array arguments ..
      double precision   a( lda, * ), x( * ), work( * )
*     ..
*
*  purpose
*  =======
*
*  pdtrsv  solves one of the systems of equations
*
*     a*x = b,   or   a'*x = b
*
*  where b and x are n element vectors and a is an n by n unit, or
*  non-unit, upper or lower triangular matrix.
*
*  this is a parallel version that uses a 2d block wrapped version
*  that is a generalization of the li-coleman algorithm.
*
*  no test for singularity or near-singularity is included in this
*  routine. such tests must be performed before calling this routine.
*
*  parameters
*  ==========
*
*  uplo   - character*1.
*           on entry, uplo specifies whether the matrix is an upper or
*           lower triangular matrix as follows:
*
*              uplo = 'u' or 'u'   a is an upper triangular matrix.
*
*              uplo = 'l' or 'l'   a is a lower triangular matrix.
*
*           unchanged on exit.
*
*  trans  - character*1.
*           on entry, trans specifies the equations to be solved as
*           follows:
*
*              trans = 'n' or 'n'   a*x = b.
*
*              trans = 't' or 't'   a'*x = b.
*
*           unchanged on exit.
*
*  diag   - character*1.
*           on entry, diag specifies whether or not a is unit
*           triangular as follows:
*
*              diag = 'u' or 'u'   a is assumed to be unit triangular.
*
*              diag = 'n' or 'n'   a is not assumed to be unit
*                                  triangular.
*
*           unchanged on exit.
*
*  n      - integer.
*           on entry, n specifies the order of the matrix a.
*           n must be at least zero.
*           unchanged on exit.
*
*  nb     - integer.
*           block size used for the data distribution.
*
*  a      - double precision array of dimension ( lda, n ).
*           before entry with  uplo = 'u' or 'u', the leading n by n
*           upper triangular part of the array a must contain the upper
*           triangular matrix and the strictly lower triangular part of
*           a is not referenced.
*           before entry with uplo = 'l' or 'l', the leading n by n
*           lower triangular part of the array a must contain the lower
*           triangular matrix and the strictly upper triangular part of
*           a is not referenced.
*           note that when  diag = 'u' or 'u', the diagonal elements of
*           a are not referenced either, but are assumed to be unity.
*           unchanged on exit.
*
*  lda    - integer.
*           on entry, lda specifies the first dimension of a as declared
*           in the calling (sub) program. lda must be at least
*           max( 1, n ).
*           unchanged on exit.
*
*  x      - double precision array of dimension at least
*           ( 1 + ( n - 1 )*abs( incx ) ).
*           before entry, the incremented array x must contain the n
*           element right-hand side vector b. on exit, x is overwritten
*           with the solution vector x.
*
*  incx   - integer.
*           on entry, incx specifies the increment for the elements of
*           x. incx must not be zero.
*           unchanged on exit.
*
*     this version dated 09/18/92
*     r. van de geijn
*
*     all rights reserved
*
*     .. parameters ..
      double precision   zero
      parameter        ( zero = 0.0d+0 )
*     .. local scalars ..
      integer            info
*     .. external functions ..
      logical            lsame
      external           lsame
*     .. external subroutines ..
      external           xerbla
*     .. intrinsic functions ..
      intrinsic          max
*     ..
*     .. executable statements ..
*
*     test the input parameters.
*
      info = 0
      if     ( .not.lsame( uplo , 'u' ).and.
     $         .not.lsame( uplo , 'l' )      )then
         info = 1
      else if( .not.lsame( trans, 'n' ).and.
     $         .not.lsame( trans, 't' )      )then
         info = 2
      else if( .not.lsame( diag , 'u' ).and.
     $         .not.lsame( diag , 'n' )      )then
         info = 3
      else if( n.lt.0 )then
         info = 4
*
*     check what goes here later
*
*      else if( lda.lt.max( 1, n ) )then
*         info = 6
      else if( incx.eq.0 )then
         info = 8
      else if( incx .ne. 1) then
*
*        incx .ne. 1 not yet implemented
*
         info = 8
      end if
      if( info.ne.0 )then
         call xerbla( 'pdtrsv ', info )
         return
      end if
*
*     quick return if possible.
*
      if( n.eq.0 )
     $   return


      if ( lsame( trans, 'n') ) then
         if (lsame( uplo, 'u')) then
            call pdutsv( diag, n, nb, a, lda, x, work )
         else
            call pdltsv( diag, n, nb, a, lda, x, work )
         endif
      elseif (lsame( trans, 't') ) then
         if (lsame( uplo, 'u')) then
c            call pduttsv( diag, n, nb, a, lda, x, work )
            print *, "pduttsv not implemented"
         else
c            call pdlttsv( diag, n, nb, a, lda, x, work )
            print *, "pdlttsv not implemented"
         endif
      endif         
*
      return
*
*     end of dtrsv .
*
      end
