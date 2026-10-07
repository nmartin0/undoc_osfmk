      subroutine   makea   ( n     , maxnz , nonzer, rcond , outunt,
     1                       a     , xstart, yindex, arow  , acol  ,
     2                       aelt  , v     , iv    , tran  , amult ,
     3                       nprocs, frstrw, lastrw, frstcl, lastcl,
     4                       totnnz, toobig )

c     ==================================================================
c     ==================================================================
c     ==== makea -- generate test matrix of prescribed density      ====
c     ==================================================================
c     ==================================================================

      integer           n     , maxnz , nonzer, outunt, nprocs,
     1                  frstrw, lastrw, frstcl, lastcl, totnnz

      integer           itoobig
      logical           toobig

      double precision  rcond, tran, amult

      integer           yindex (maxnz), xstart (n), iv (1),
     1                  arow   (maxnz), acol   (maxnz)

      double precision  a (maxnz), v (n), aelt (maxnz)

c     ==================================================================

c       generate the test problem for benchmark 6 (conjugate gradient)
c       makea generates a sparse matrix with a
c       prescribed sparsity distribution

c       parameter    type        usage

c       input

c       n            i           number of rows/columns of matrix
c       maxnz        i           nonzeros as declared array size
c       nonzer       i           minimum density per row
c       rcond        r*8         condition number
c       outunt       i           output unit number

c       output

c       a            r*8         array for nonzeros
c       yindex       i           row or column indices
c       xstart       i           column or row pointers pointers
c                                (above two parameters change roles
c                                 as function of cmprss routine
c                                 called)

c       workspace

c       iv, arow, acol i
c       v, aelt        r*8
c       tran, amult    r*8       random number generator parameters
c
c     parallel implementation -- input
c
c       frstrw         i         first row of this processor's block
c       lastrw         i         last row of this processor's block
c       frstrw         i         first column of this processor's block
c       lastrw         i         last column of this processor's block
c       totnnz         i         total number of nonzeros in matrix
c       toobig       l            .true. if too many nonzeros
c                                requested for workspace (use more
c                                processors

c     ==================================================================

c     ... local variables
c     -------------------

      integer           frstdg, i     , lastdg, iouter, irow  , ivelt ,
     1                  ivelt1, jcol  , nnza  , nzv

      integer           ilwork
      logical           lwork

      double precision  size, ratio, scale

      external          cmprss, sprnvc, diagst

c     ==================================================================

      size   = 1.0d0
      ratio  = rcond ** ( 1.0d0 / dfloat (n) )
      nnza   = 0
      toobig = .false.

      do 300 iouter = 1, n
         
         nzv = nonzer
         
         call sprnvc ( n         , nzv         , v      , iv     ,
     1                 yindex (1), yindex (n+1), tran   , amult   )
         
         call diagst  ( v, iv, nzv, iouter, 0.5d0 )
         
         do 200 ivelt = 1, nzv

            jcol = iv (ivelt)

c           -------------------------------------
c           ... save only entries in my submatrix
c           -------------------------------------

            if  ( jcol .ge. frstcl  .and.  jcol .le. lastcl )  then

               scale = size * v (ivelt)

               do 100 ivelt1 = 1, nzv
                  irow = iv (ivelt1)
                  if  ( irow .ge. frstrw  .and.
     1                  irow .le. lastrw        )  then
                     nnza        = nnza + 1
                     if  ( nnza .le. maxnz )  then
                        arow (nnza) = irow
                        acol (nnza) = jcol
                        aelt (nnza) = v (ivelt1) * scale
                     else
                        toobig = .true.
                     endif
                  endif
 100           continue

            endif
            
 200     continue
            
         size = size * ratio

 300  continue

      if (toobig. eq. .true.) then
         itoobig = 1
      else
         itoobig = 0
      endif 

      if (toobig .eq. .true.) print *, "TOOOO BIGG"
c      call glor ( toobig, 1, lwork )
      call glor ( itoobig, 1, ilwork )

      if (itoobig .eq. 1) then
         toobig = .true.
      else
         toobig = .false.
      endif 
c
c   ERROR???
c
c      toobig = lwork

      if  ( toobig ) return

c     -------------------------------------------------------------
c     ... add the identity * rcond to the generated matrix to bound
c         the smallest eigenvalue from below by rcond (but only
c         those diagonal entries in my submatrix)
c     -------------------------------------------------------------
      
      frstdg = max ( frstrw, frstcl )
      lastdg = min ( lastrw, lastcl )

      do 400 i = frstdg, lastdg
         nnza        = nnza + 1
         arow (nnza) = i
         acol (nnza) = i
         aelt (nnza) = rcond
 400  continue
      
c     ----------------------------------------------------------------
c     ... make the sparse matrix from list of elements with duplicates
c         (v and iv are used as  workspace)
c     ----------------------------------------------------------------
      
      call cmprss ( n     , arow  , acol  , aelt  , nnza    , a     ,
     1              yindex, xstart, v     , iv (1), iv (n+1), outunt,
     2              nprocs, frstrw, lastrw, frstcl, lastcl  , totnnz )
      
      return

      end



