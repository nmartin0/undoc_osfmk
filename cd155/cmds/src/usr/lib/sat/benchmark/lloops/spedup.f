C
C***********************************************************************
      SUBROUTINE  SPEDUP( iou, NAME, RATE )
C***********************************************************************
C                                                                      *
C     SPEDUP  - Computes Speed-ups: A circumspect method of comparison.*
C               Computers are ranked by their Geometric Mean Rates.    *
C                                                                      *
C     iou     - input scalar,  i/o unit number                         *
C     NAME    - input array ,  system name                             *
C     RATE    - input array ,  execution rates (Mflops)                *
C                                                                      *
C***********************************************************************
C
C
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      parameter( nsys= 5, ns= nsys+1, nd= 11, nt= 4 )
      CHARACTER  NAME*8, NAMES*8, ijk*8
      DIMENSION  RATE(nt), NAME(nt), RATIO(nd)
      CHARACTER*8  IT(nt)
      COMMON /TAGS/  NAMES(nd,nt)
      COMMON /RATS/  RATED(nd,nt)
C
      CALL TRACE ('SPEDUP  ')
C                            Rank computer NAME by its Geometric Mean.
      DO  2  k= 1,nsys
            IF( RATE(2) .GT. RATED(k,2))  GO TO 4
    2 continue
    4   insert= k
C                            Pushdown Tables to allow insertion.
      DO  8  i= nd, insert+1, -1
      DO  6  j= 1,nt
      NAMES(i,j)=  NAMES(i-1,j)
      RATED(i,j)=  RATED(i-1,j)
    6 continue
    8 continue
C                            Insert new computer NAME
      DO 10  j= 1,nt
      NAMES(insert,j)=  NAME(j)
      RATED(insert,j)=  RATE(j)
   10 continue
C                            Print Table of Speed-ups of Mean Rates.
      CALL PAGE( iou)
      IT(1)= 'AM='
      IT(2)= 'GM='
      IT(3)= 'HM='
      ijk  = '--------'
      fuzz = 1.0d-9
      WRITE( iou,111)
      WRITE( iou,104)
  104 FORMAT(26X,'TABLE OF SPEED-UP RATIOS OF MEAN RATES (72 Samples)')
      WRITE( iou,105)
  105 FORMAT(/,26X,'Arithmetic, Geometric, Harmonic Means (AM,GM,HM)')
      WRITE( iou,106)
  106 FORMAT(26X,'The Geometric Mean is the least biased statistic.',/)
      WRITE( iou,109) ( ijk, m= 1,ns)
  109 FORMAT(1X,'--------  ----  ------  ',11(1X,A ))
      WRITE( iou,110) ( NAMES(m,2), m= 1,ns)
  110 FORMAT(1X,'SYSTEM    MEAN  MFLOPS',2X,11(1X,A ))
      WRITE( iou,109) ( ijk, m= 1,ns)
C
      DO 40  i= 1,ns
      WRITE( iou,111)
  111 FORMAT(/)
C
      DO 26  j= 1,nt-1
C
      DO 22  m= 1,ns
      RATIO(m)= RATED(i,j) / (RATED(m,j) + fuzz)
   22 continue
C
      WRITE( iou,112) NAMES(i,j), IT(j), RATED(i,j), (RATIO(m), m=1,ns)
  112 FORMAT(1X,A ,2X,A3,F9.3,2H :,11F9.3)
   26 continue
C
      WRITE( iou,114)  RATED(i,4)
  114 FORMAT(11X,3HSD=,F9.3)
   40 continue
C
      CALL TRACK ('SPEDUP  ')
      RETURN
      END
