C
C
C***********************************************************************
      SUBROUTINE SIMD( HM,  iou,RATES,WG,FR,m,  MAP,TV1,TV2,TV3,n)
C***********************************************************************
C                                                                      *
C     SIMD  - Sensitivity Of Harmonic Mean Rate(Mflops) SISD/SIMD Model*
C                                                                      *
C     HM      - result array,  Harmonic Mean Rates(k)= f( FR(k))       *
C     iou     - input scalar,  i/o unit number                         *
C     RATES   - input array ,  execution rates (Mflops)                *
C     WG      - input array ,  weights paired with RATES               *
C     FR      - input array ,  fractions of flops executed SIMD        *
C     m       - input scalar,  number of fractions                     *
C                                                                      *
C     MAP,TV,TV1,TV2,TV3    -  output temporary arrays                 *
C     n       - input scalar,  number of rates, etc.                   *
C                                                                      *
C***********************************************************************
C
C
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
c SENSITIVITY OF NET MFLOPS RATE TO USE OF OPTIMAL FORTRAN CODE(SISD/SIMD MODEL)
c Out
      DIMENSION  HM(m)
c In
      DIMENSION  FR(m), RATES(n), WG(n)
c Temp
      DIMENSION  MAP(n), TV1(n), TV2(n), TV3(n), STAT2(20)
C
      CALL TRACE ('SIMD    ')

C                 Compress valid data sets RATES,  mapping on MAP.

            meff= 0
            neff= 0
              bl= 1.0d-5
              bu= 1.0d+5
            CALL  VALID( TV1,MAP,neff,  bl, RATES, bu, n)

        DO  1  k= 1,neff
          TV3(k)=    WG( MAP(k))
    1  continue


C                 Compress valid data sets WG,  mapping on MAP.

            CALL  VALID( TV3,MAP,meff,  bl, TV3, bu, neff)

        DO  3  k= 1,meff
          TV2(k)= TV1( MAP(k))
    3  continue

C                 Sort RATES,WT into descending order.

            CALL  STATW( STAT2,TV1,MAP, TV2, TV3, meff)
             med= meff + 1 - INT(STAT2(8))
              lh= meff + 1 - med

        DO  5  k= 1,meff
          TV2(k)= TV3( MAP(k))
    5  continue


C                 Estimate vector rate= HMean of top LFK quartile.

              nq= meff/4
            CALL  STATW( STAT2,TV3,MAP, TV1,TV2,nq)
             vmf= STAT2(5)

C                 Estimate scalar rate= HMean of lowest two LFK quartiles.

            CALL  STATW( STAT2,TV3,MAP, TV1(med),TV2(med),lh)
             smf= STAT2(5)
            fuzz= 1.0d-9

               g= 1.0d0 -   smf/( vmf + fuzz)
           HM(1)= smf

          DO 7 k= 2,m
           HM(k)=   smf/( 1.0d0 - FR(k)*g + fuzz)
   7      continue
C
      IF( iou .GT. 0)  THEN
C
          WRITE ( iou,7001)
          WRITE ( iou,7001)
          WRITE ( iou,7001)
          WRITE ( iou,7101)
          WRITE ( iou,7102) ( HM(k), k= 1,9)
          WRITE ( iou,7102) ( FR(k), k= 1,9)
          WRITE ( iou,7103)
          WRITE ( iou,7001)
 7001 FORMAT(/)
 7101 FORMAT(' SENSITIVITY OF NET MFLOPS RATE TO USE OF OPTIMAL FORTRAN
     1CODE(SISD/SIMD MODEL)' )
 7102 FORMAT(/,1X,5F7.2,4F8.2)
 7103 FORMAT(3x,52H Fraction Of Operations Run At Optimal Fortran Rates)
C
      ENDIF
C
      CALL TRACK ('SIMD    ')
      RETURN
C
      END
