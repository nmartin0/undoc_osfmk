C
C***********************************************************************
      SUBROUTINE  TALLY( iou, mode )
C***********************************************************************
C                                                                      *
C    TALLY      computes average and minimum Cpu timings and variances.*
C                                                                      *
C               iou -  i/o unit number                                 *
C                                                                      *
C              mode -  = 1 selects average run time: Preferred mode.   *
C                      = 2 selects minimum run time: Less accurate mode*
C                                                                      *
C***********************************************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
Cout  DOUBLE  PRECISION  cs                                             REDUNDNT
C
C/      PARAMETER( nk= 47, nl= 3, nr= 8 )
C
      COMMON /ALPHA/ mk,ik,im,ml,il,Mruns,Nruns,jr,iovec,NPFS(8,3,47)
      COMMON /BETA / tic, TIMES(8,3,47), SEE(5,3,8,3),
     1              TERRS(8,3,47), CSUMS(8,3,47),
     2              FOPN(8,3,47), DOS(8,3,47)
C
      COMMON /SPACE0/ TIME(47), CSUM(47), WW(47), WT(47), ticks,
     1                FR(9), TERR1(47), SUMW(7), START,
     2              SKALE(47), BIAS(47), WS(95), TOTAL(47), FLOPN(47),
     3                IQ(7), NPF, NPFS1(47)
C
      DIMENSION  S1(20), S2(20), S3(20), S4(20)
      DIMENSION  T1(47), T4(47)
C
      CALL TRACE ('TALLY   ')
C
           CALL  SIZES(-1)
C
      m= 1
      IF( mode .EQ. 2 )  m= 3
      CALL  PAGE(iou)
      WRITE( iou, 99)
      WRITE( iou,100)
C                        Checks valid domain for min and max of data sets
      DO 2 j= 1,Nruns
      WRITE( iou,102)  j, ( SEE(k,1,j,il), k= 1,2)
      T1(j)= SEE(1,1,j,il)
      i= 0
      IF( (SEE(3,2,j,il).LT. 0.01) .OR. (SEE(4,2,j,il).GT. 1.0))  i= i+1
      IF( (SEE(3,3,j,il).LT. 0.01) .OR. (SEE(4,3,j,il).GT. 1.0))  i= i+1
      IF( i.GT.0 )  THEN
      WRITE( iou,131)  j, il
      ENDIF
      IF( ( j.EQ.Nruns ) .OR. ( i.GT.0 ))  THEN
      WRITE( iou,104)  j, ( SEE(k,2,j,il), k= 1,4)
      WRITE( iou,104)  j, ( SEE(k,3,j,il), k= 1,4)
      ENDIF
    2 continue
C
      CALL STATS( S1, T1, Nruns)
      WRITE( iou,102)  Nruns, ( S1(k), k= 1,4)
C
C
C
      WRITE( iou,120) Nruns
      WRITE( iou,122)
      WRITE( iou,121)
      WRITE( iou,122)
C                        Computes and Checks experimental timing errors
      DO 8 k= 1,mk
        npft= 0
          cs= 0.0d0
C
      DO 4 j= 1,Nruns
        npft= npft +  NPFS(j,il,k)
          cs= cs   + CSUMS(j,il,k)
    4 continue
C
      CALL  STATS( S2, TIMES(1,il,k), Nruns)
      TIME(k)= S2(m)
      CSUM(k)= cs
      TERR1(k)= 100.0d0*( S2(2)/( S2(1) + 1.0d-9))
      T4(k)= TERR1(k)
C
C
C     If this clock resolution test fails, you must increase Loop (Subr. SIZES)
C
      CALL  STATS( S3, TERRS(1,il,k), Nruns)
         IF( S3(1) .GT. 15.0)  THEN
            WRITE( iou,113) k
         ENDIF
C
      WRITE( iou,123) k, S2(3), S2(1), S2(4), TERR1(k), S3(1), npft
      TERR1(k)= MAX( TERR1(k), S3(1))
      CALL  STATS( S1, DOS(1,il,k), Nruns)
      TOTAL(k)= S1(1)
           IF( (S1(1).LE.0.0d0) .OR. (ABS(S1(3)-S1(4)).GT.1.0d-5)) THEN
           WRITE( iou,131) il, k, ( S1(k4), k4= 1,4)
           ENDIF
      CALL  STATS( S4, FOPN(1,il,k), Nruns)
      FLOPN(k)= S4(1)
           IF( (S4(1).LE.0.0d0) .OR. (ABS(S4(3)-S4(4)).GT.1.0d-5)) THEN
           WRITE( iou,131) il, k, ( S4(k4), k4= 1,4)
           ENDIF
    8 continue
C
      WRITE( iou,122)
      CALL  STATS( S4, T4, mk)
      WRITE(   *,124)
      WRITE(   *,133)
      WRITE(   *,125)  ( S4(k), k= 1,4)
      WRITE( iou,124)
      WRITE( iou,133)
      WRITE( iou,125)  ( S4(k), k= 1,4)
C
      CALL TRACK ('TALLY   ')
      RETURN
C
   99 FORMAT(//,29H time TEST overhead (t err):  )
  100 FORMAT(/,6X,3HRUN,8X,7HAVERAGE,8X,7HSTANDEV,8X,7HMINIMUM,8X,
     1 7HMAXIMUM )
  102 FORMAT(1X,5HTICK ,I3,4E15.6)
  104 FORMAT(1X,5HDATA ,I3,4E15.6)
  113 FORMAT(/,1X,I2,45H POOR CPU CLOCK RESOLUTION; NEED LONGER RUN. )
  120 FORMAT(//,39H THE EXPERIMENTAL TIMING ERRORS FOR ALL,I3,5H RUNS)
  121 FORMAT(55H  k   T min      T avg      T max    T err   tick   P-F)
  122 FORMAT(55H --  ---------  ---------  --------- -----  -----   ---)
  123 FORMAT(1X,I2,3E11.4,F6.2,1H%,F6.2,1H%,1X,I5)
  124 FORMAT(//,50H NET CPU TIMING VARIANCE (T err);  A few % is ok: )
  125 FORMAT(4X,5H Terr,4(F14.2,1H%))
  131 FORMAT(1X,32H**  TALLY: ERROR INVALID DATA** ,2I6,4E14.6)
  133 FORMAT(/,17X,7HAVERAGE,8X,7HSTANDEV,8X,7HMINIMUM,8X,7HMAXIMUM )
      END
