      subroutine   diagst   ( v, rowidx, nzv, i, value )
c
c     ==================================================================
c     ==================================================================
c     ==== diagst -- add entry to specified entry of sparse vector  ====
c     ==================================================================
c     ==================================================================

      integer           rowidx (*), nzv, i

      real*8            v (*), value

      integer           k

c     ==================================================================

c     ... put entry with index 'i' and value 'value' into
c         sparse vector.  used to put specified value on diagonal
c         of generated sparse matrix.

c     ----------------------------------------------------
c     ... search for first occurence of row index
c         if one exists, set corresponding numerical entry
c         to "value"
c     ----------------------------------------------------

      do 10 k = 1, nzv
         if  ( rowidx(k) .eq. i )  then
            v(k) = value
            go to 200
         endif
 10   continue

c     ------------------------------------------
c     ... no entry now exists in list -- add one
c     ------------------------------------------

      nzv          = nzv + 1
      v(nzv)       = value
      rowidx (nzv) = i

 200  return

      end
