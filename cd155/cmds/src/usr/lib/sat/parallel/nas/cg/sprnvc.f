      subroutine   sprnvc   ( n, nz, v, irow, nzloc, mark, tran, amult )

c     ==================================================================
c     ==================================================================
c     ====  sprnvc -- generate a sparse n-vector (v, irow) with     ====
c     ====            nzv nonzeros                                  ====
c     ==================================================================
c     ==================================================================

      double precision  v (nz), tran, amult

      integer           n, nz, irow (nz), nzloc (n)

      logical           mark(n)

      integer           i, nn1, nzcol

      double precision  vecelt, vecloc

c     ----------------------
c     ... external functions
c     ----------------------

      integer           icnvrt

      double precision  randlc

      external          randlc, icnvrt

c     ==================================================================

      do 100  i = 1, n
         mark(i) = .false.
 100  continue
      
      nzcol   = 0
      nn1     = 1

c         ----------------------------------------------------
c         ... nn1 is the smallest power of two not less than n
c         ----------------------------------------------------
      
 200      nn1 = 2 * nn1
          if ( nn1 .lt. n )   goto 200

c         -----------------------------------------------------
c         ... generate column vector with exactly  nnz distinct
c             nonzero entries
c         -----------------------------------------------------

 300      if  ( nzcol .ge. nz )  goto 400

          vecelt = randlc (tran, amult)
          
c         ------------------------------------------------------------
c         ....generate an integer between 1 and n in a portable manner
c         ------------------------------------------------------------
          
          vecloc = randlc ( tran, amult )
          i      = icnvrt ( vecloc, nn1 ) + 1
          if  ( i .gt. n )  goto 300
          
c         ---------------------------------------
c         ... was this integer generated already?
c         ---------------------------------------
          
          if  ( .not. mark (i) )  then
             mark (i)      = .true.
             nzcol         = nzcol + 1
             nzloc (nzcol) = i
             irow (nzcol)  = i
             v (nzcol)     = vecelt
          endif
          goto 300

 400  do 500 i = 1, nzcol
         mark (nzloc (i)) = .false.
 500  continue

      return

      end
