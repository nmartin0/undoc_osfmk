C
C***************************************
      SUBROUTINE INDATA( TK, iou)
C***************************************
C       INDATA     initialize variables
C
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
C/      PARAMETER( kn= 47, kn2= 95, np= 3, ls= 3*47, krs= 24)
C/      PARAMETER( nk= 47, nl= 3, nr= 8 )
      DIMENSION  TK(6)
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
      COMMON /ORDER/ inseq, match, NSTACK(20), isave, iret
      COMMON /SPACES/ ion,j5,k2,k3,MULTI,laps,Loop,m,kr,LP,n13h,ibuf,nx,
     1 L,npass,nfail,n,n1,n2,n13,n213,n813,n14,n16,n416,n21,nt1,nt2,
     2 last,idebug,mpy,Loops2,mucho,mpylim, intbuf(16)
C
       TK(1)= 0.00d0
       TK(2)= 0.00d0
      testov= 0.00d0
      ticks = 0.00d0
      tclock= 0.00d0
      tsecov= 0.00d0
      tic   = 0.00d0
C
      jr    = 1
      Nruns = 1
      il    = 1
      mk    = 1
      ik    = 1
C
      inseq = 0
      isave = 0
      iret  = 0
C
      Loops2= 1
      mpylim= Loops2
      mpy   = 1
      MULTI = 1
      mucho = 1
      L     = 1
      Loop  = 1
      LP    = Loop
      n     = 0
C
      iou   = 8
      ion   = iou
        CALL  INITIO( 8, 'output')
C       CALL  INITIO( 7, 'chksum')
C
      CALL TRACE ('INDATA  ')
CPFM       IF( INIPFM( ion, 0) .NE. 0 )  THEN
CPFM           CALL WHERE(20)
CPFM       ENDIF
C
CLLL.      call  Q8EBM
C
          WRITE (   *,7002)
          WRITE (   *,7003)
          WRITE (   *,7002)
          WRITE ( iou,7002)
          WRITE ( iou,7003)
          WRITE ( iou,7002)
 7002 FORMAT(  ' *********************************************' )
 7003 FORMAT(  ' THE LIVERMORE  FORTRAN KERNELS "MFLOPS" TEST:' )
      WRITE( iou, 797)
      WRITE( iou, 798)
  797 FORMAT(' >>> USE 72 SAMPLES LFK TEST RESULTS SUMMARY (line 330+)')
  798 FORMAT(' >>> USE ALL RANGE STATISTICS FOR OFFICIAL QUOTATIONS.  ')
      CALL TRACK ('INDATA  ')
      RETURN
      END
