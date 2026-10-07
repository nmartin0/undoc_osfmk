C
C***********************************************
      FUNCTION TICK( iou, ntimes)
C***********************************************
C
C            TICK       measures timing overhead of subroutine test
C
C     iou    -  Logical Output Device Number                           *
C
C***********************************************
C
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
C
C/      PARAMETER( l1= 1001, l2=  101, l1d= 2*1001 )
C/      PARAMETER( l13=  64, l13h= l13/2, l213= l13+l13h, l813= 8*l13 )
C/      PARAMETER( l14=2048, l16=  75, l416= 4*l16 , l21= 25 )
C
C/      PARAMETER( kn= 47, kn2= 95, np= 3, ls= 3*47, krs= 24)
C
C/      PARAMETER( nk= 47, nl= 3, nr= 8 )
      parameter( l4813= 4*512, l4813p= l4813 + 1 )
      INTEGER TEST
C
      COMMON /ALPHA/ mk,ik,im,ml,il,Mruns,Nruns,jr,iovec,NPFS(8,3,47)
      COMMON /TAU/   tclock, tsecov, testov, cumtim(4)
      COMMON /BETA / tic, TIMES(8,3,47), SEE(5,3,8,3),
     1              TERRS(8,3,47), CSUMS(8,3,47),
     2              FOPN(8,3,47), DOS(8,3,47)
C
C
      COMMON /SPACES/ ion,j5,k2,k3,MULTI,laps,Loop,m,kr,LP,n13h,ibuf,nx,
     1 L,npass,nfail,n,n1,n2,n13,n213,n813,n14,n16,n416,n21,nt1,nt2,
     2 last,idebug,mpy,Loops2,mucho,mpylim, intbuf(16)
C
      COMMON /SPACER/ A11,A12,A13,A21,A22,A23,A31,A32,A33,
     2                AR,BR,C0,CR,DI,DK,
     3  DM22,DM23,DM24,DM25,DM26,DM27,DM28,DN,E3,E6,EXPMAX,FLX,
     4  Q,QA,R,RI,S,SCALE,SIG,STB5,T,XNC,XNEI,XNM
C
      COMMON /SPACE0/ TIME(47), CSUM(47), WW(47), WT(47), ticks,
     1                FR(9), TERR1(47), SUMW(7), START,
     2              SKALE(47), BIAS(47), WS(95), TOTAL(47), FLOPN(47),
     3                IQ(7), NPF, NPFS1(47)
C
      COMMON /SPACEI/ WTP(3), MUL(3), ISPAN(47,3), IPASS(47,3)
C
      INTEGER    E,F,ZONE
      COMMON /ISPACE/ E(96), F(96),
     1  IX(1001), IR(1001), ZONE(300)
C
      COMMON /SPACE1/ U(1001), V(1001), W(1001),
     1  X(1001), Y(1001), Z(1001), G(1001),
     2  DU1(101), DU2(101), DU3(101), GRD(1001), DEX(1001),
     3  XI(1001), EX(1001), EX1(1001), DEX1(1001),
     4  VX(1001), XX(1001), RX(1001), RH(2048),
     5  VSP(101), VSTP(101), VXNE(101), VXND(101),
     6  VE3(101), VLR(101), VLIN(101), B5(101),
     7  PLAN(300), D(300), SA(101), SB(101)
C
      COMMON /SPACE2/ P(4,512), PX(25,101), CX(25,101),
     1  VY(101,25), VH(101,7), VF(101,7), VG(101,7), VS(101,7),
     2  ZA(101,7)  , ZP(101,7), ZQ(101,7), ZR(101,7), ZM(101,7),
     3  ZB(101,7)  , ZU(101,7), ZV(101,7), ZZ(101,7),
     4  B(64,64), C(64,64), H(64,64),
     5  U1(5,101,2),  U2(5,101,2),  U3(5,101,2)
C
      DIMENSION  TIM(20), TER(20), TMX(20), INX(20), P1(l4813p)
      EQUIVALENCE( P,P1)
      SAVE retest
C
C
      CALL TRACE ('TICK    ')
C
      ion= iou
      kr = mk
      n  = 0
      k2 = 0
      k3 = 0
      m  = 0
      neff= 0
      IF( il .EQ. 1 )  THEN
C
C***********************************************************************
C     Measure tsecov:  Overhead time for calling SECOND
C***********************************************************************
C
      tsecov = SECOVT( iou)
         tic = tsecov
C
C***********************************************************************
C     Measure testov:  Overhead time for calling TEST
C***********************************************************************
C
         testo= 0.00d0
           klm= 8000
            io= ABS(iou)
            jj= 0
            nt= ntimes - 6
             j= nt
            IF( nt.LT.8 .OR. nt.GT.30 )  GO TO 911
C
      DO 820 j= 1,nt
             L= 1
           mpy= 1
        Loops2= 1
        mpylim= Loops2
          Loop= klm
            LP= Loop
C                                  Measure overhead time for empty loop
      cumtim(1)= 0.0d0
             t0= SECOND( cumtim(1))
  801        IF( TEST(-73) .GT. 0 )  GO TO 801
  802        IF( TEST(-73) .GT. 0 )  GO TO 802
  803        IF( TEST(-73) .GT. 0 )  GO TO 803
  804        IF( TEST(-73) .GT. 0 )  GO TO 804
  805        IF( TEST(-73) .GT. 0 )  GO TO 805
  806        IF( TEST(-73) .GT. 0 )  GO TO 806
  807        IF( TEST(-73) .GT. 0 )  GO TO 807
  808        IF( TEST(-73) .GT. 0 )  GO TO 808
  809        IF( TEST(-73) .GT. 0 )  GO TO 809
  810        IF( TEST(-73) .GT. 0 )  GO TO 810
      cumtim(1)= 0.0d0
             t1= SECOND( cumtim(1)) - tsecov
         elapst= t1 - t0
          testo= elapst/( REAL(10*klm) + 1.0e-9)
          toler= 0.020d0
           rerr= 1.00d0
C
C                                  Convergence test:  Rel.error .LT. 1%
            IF( elapst.GT. 1.00d04 ) GO TO 911
            IF( elapst.LT. 1.00d-9 .AND. j.GT.8 ) GO TO 911
            IF( elapst.GT. 1.00d-9 ) THEN
                     jj= jj + 1
                TIM(jj)= testo
                     IF( jj.GT.1 ) THEN
                         rerr= RELERR( TIM(jj), TIM(jj-1))
                     ENDIF
                TER(jj)= rerr
            ENDIF
C
            IF( iou.GT.0 ) THEN
         WRITE( iou,64) 10*klm,  testo, rerr
            ENDIF
            IF( rerr  .LT. toler   ) GO TO 825
            IF( elapst.GT. 10.00d0 ) GO TO 822
           klm= klm + klm
  820 continue
C                                  Poor accuracy on exit from loop
  822     IF( j .LE. 1 )  GO TO 911
          IF( jj.LT. 1 )  GO TO 911
         CALL SORDID( INX,TMX,  TER,jj,1)
       testo= TIM( INX(1))
        rerr= TMX(1)
       WRITE( io,63)  100.00d0*rerr
C                                  Good convergence, satifies 1% error tolerence
  825 continue
      testov        = testo
      retest        = rerr * testov
      ENDIF
C
C***********************************************************************
C                                  Generate data sets
      SEE(1,1,jr,il)= testov
      SEE(2,1,jr,il)= retest
      ticks         = testov
      TICK          = testov
      L      = 1
      Loop   = 1
      LP     = Loop
      j      = TEST(0)
C
      DO 20 k= 1,47
      TIME(k)= 0.0d0
      CSUM(k)= 0.0d0
20    CONTINUE
C
      IF( il .EQ. 1 )  THEN
          CALL  STATS( SEE(1,2,jr,il), U, nt1)
c         CALL  STATS( SEE(1,3,jr,il), P, nt2)
          CALL  STATS( SEE(1,3,jr,il), P1(l4813+1), nt2-l4813)
      ELSE
          DO 45 k= 1,5
              SEE( k,2,jr,il)= SEE( k,2,jr,1)
              SEE( k,3,jr,il)= SEE( k,3,jr,1)
   45     continue
      ENDIF
C
      IF( iou.GT.0 ) THEN
      WRITE( iou, 99)
      WRITE( iou,100)
      WRITE( iou,102)  ( SEE(k,1,jr,il), k= 1,2)
      WRITE( iou,104)  ( SEE(k,2,jr,il), k= 1,4)
      WRITE( iou,104)  ( SEE(k,3,jr,il), k= 1,4)
      ENDIF
C
      CALL TRACK ('TICK    ')
      RETURN
C
  911         WRITE( io,61)
              WRITE( io,62) elapst, j
              CALL WHERE(0)
C
   61 FORMAT(1X,'FATAL(TICK): cant measure overhead time of subr. TEST')
   62 FORMAT(/,13X,'using SECOND:  elapst=',1E20.8,6X,'J=',I4)
   63 FORMAT(1X,'WARNING(TICK):  TEST overhead time relerr',f9.4,'%')
   64 FORMAT(1X,'testov(TICK)',I12,E12.4,F11.4)
   99 FORMAT(//,17H CLOCK OVERHEAD:  )
  100 FORMAT(/,14X,7HAVERAGE,8X,7HSTANDEV,8X,7HMINIMUM,8X,7HMAXIMUM )
  102 FORMAT(/,1X,5H TICK,4E15.6)
  104 FORMAT(/,1X,5H DATA,4E15.6)
      END
