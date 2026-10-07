C******************************************************************************C
C******************************************************************************C
C***                        compute the l2-norm of vector v                 ***C
C******************************************************************************C
C******************************************************************************C

        subroutine l2norm (v,sum,ldx,ldy)

        include 'appbt.incl'

        dimension v(ldx,ldy,5), sum(5)

        dimension work(5)

C******************************************************************************C
C******************************************************************************C

        do 10 m = 1, 5
           sum(m) = 0.d0
 10     continue

        if ((my_col.gt.1).and.(my_col.lt.node_col)) then

           do 110 j = jbeg_loop, jend_loop
              do 120 i = 2, nxm1

                 sum(1) = sum(1) + v(i,j,1)*v(i,j,1)
                 sum(2) = sum(2) + v(i,j,2)*v(i,j,2)
                 sum(3) = sum(3) + v(i,j,3)*v(i,j,3)
                 sum(4) = sum(4) + v(i,j,4)*v(i,j,4)
                 sum(5) = sum(5) + v(i,j,5)*v(i,j,5)

 120          continue
 110       continue

        endif

c***collect contributions from all processors

        call gdsum (sum, 5, work)

        do 200 m = 1, 5
           sum(m) = dtotal*dsqrt(sum(m))
 200    continue

        return

        end

C******************************************************************************C
C******************************************************************************C
