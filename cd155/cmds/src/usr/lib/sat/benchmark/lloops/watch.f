C
C***********************************************
      SUBROUTINE WATCH( mode)
C***********************************************
C
C  WATCH is called at every subroutine entry and exit point by TRACE .
C  COMMON variables may be tested continually during execution(watched)
C  for known error conditions so the occurance of the error is localized.
C  WATCH may be used for programmable data-breakpoints to aid debugging.
C
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
c     parameter( ntests=  1, krs1= 24 + 1 )
      parameter( ntests= 14, krs1= 24 + 1 )
C
      CHARACTER  name*8, ISTACK*8
      COMMON /DEBUG/     ISTACK(20)
      COMMON /ORDER/ inseq, match, NSTACK(20), isave, iret
C
      COMMON /ALPHA/ mk,ik,im,ml,il,Mruns,Nruns,jr,iovec,NPFS(8,3,47)
      COMMON /TAU/   tclock, tsecov, testov, cumtim(4)
      COMMON /BETA / tic, TIMES(8,3,47), SEE(5,3,8,3),
     1              TERRS(8,3,47), CSUMS(8,3,47),
     2              FOPN(8,3,47), DOS(8,3,47)
C
      COMMON /SPACE0/ TIME(47), CSUM(47), WW(47), WT(47), ticks,
     1                FR(9), TERR1(47), SUMW(7), START,
     2              SKALE(47), BIAS(47), WS(95), TOTAL(47), FLOPN(47),
     3                IQ(7), NPF, NPFS1(47)
C
      COMMON /SPACES/ ion,j5,k2,k3,MULTI,laps,Loop,m,kr,LP,n13h,ibuf,nx,
     1 L,npass,nfail,n,n1,n2,n13,n213,n813,n14,n16,n416,n21,nt1,nt2,
     2 last,idebug,mpy,Loops2,mucho,mpylim, intbuf(16)
      DIMENSION  IE(20)
c     LOGICAL BOUNDS
c     BOUNDS(A,X,B,E)= ((((A)*(1.-E)).LE.(X)).AND.((X).LE.((B)*(1.+E))))
C
C                                       Debug Trace Info
                       name= 'watch'
c     IF( made.EQ.1 )  name= ' ENTRY  '
c     IF( made.EQ.2 )  name= ' RETURN '
c     WRITE(*,101) inseq, name, ISTACK(1)
c 101 FORMAT(1X,I6,5X,A ,1X,A )
C
C                                       Domain Tests of Critical Variables
      DO 1 k= 1,ntests
    1  IE(k)= 0
      IF(    testov  .NE. ticks      ) IE(1)= 1
      IF(    tsecov  .NE. tic        ) IE(2)= 2
      IF( inseq.LE.0 .OR. inseq.NE.isave .OR. inseq.GT.99999) IE(3)= 3
      IF( Nruns.LT.1 .OR. Nruns.GT.8 ) IE(4)= 4
      IF(    il.LT.1 .OR. il.GT.3    ) IE(5)= 5
      IF(    mk.LT.1 .OR. mk.GT.24   ) IE(6)= 6
      IF(    ik.LT.0 .OR. ik.GT.krs1  ) IE(7)= 7
      IF(    jr.LT.1 .OR. jr.GT.8    ) IE(8)= 8
      IF(    Loops2  .LT. 1          ) IE(9)= 9
      IF(    Loops2  .NE. mpylim     ) IE(10)= 10
      IF(    MULTI   .LT. 1          ) IE(11)= 11
      IF(    MULTI   .NE. mucho      ) IE(12)= 12
      IF(    Loop    .LT. 1          ) IE(13)= 13
      IF(    Loop    .NE. LP         ) IE(14)= 14
C
C                        Insert your debug data tests here
c     IF( BOUNDS( 1.7669e+5,CSUMS(jr,1,8),1.7669e+5,1.0e-3)) IE(15)= 15
C
      ierr= 0
      DO 2 k= 1,ntests
    2 ierr= ierr + IE(k)
          IF( ierr.NE.0 )   THEN
              io= ABS( ion)
              IF( io.LE.0 .OR. io.GT.10 ) io=6
                   k1=0
                   k2=0
              WRITE(  *,111)
              WRITE(  *,112) (    k , k= 1,ntests )
              WRITE(  *,112) ( IE(k), k= 1,ntests )
              WRITE(  *,112) k1,k2,inseq,Nruns,il,mk,ik,jr,
     .                       Loops2,mpylim,MULTI,mucho,Loop,LP
              WRITE( io,111)
              WRITE( io,112) (    k , k= 1,ntests )
              WRITE( io,112) ( IE(k), k= 1,ntests )
              WRITE( io,112) k1,k2,inseq,Nruns,il,mk,ik,jr,
     .                       Loops2,mpylim,MULTI,mucho,Loop,LP
  111         FORMAT(/,' WATCH: STORAGE FAULT DETECTED.  IE=')
  112         FORMAT(1X,15I5)
              CALL WHERE( mode)
          ENDIF
      RETURN
      END
